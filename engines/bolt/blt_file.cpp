/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "bolt/blt_file.h"

namespace Bolt {

struct BltFileHeader {

	static const int SIZE = 0x10;
	BltFileHeader(Common::File &file) {
		magic = file.readUint32BE();
		// Skip 7 unknown bytes (FIXME: what do these mean?)
		file.seek(7, SEEK_CUR);
		numDirs = file.readByte();
		fileSize = file.readUint32BE();
	}

	uint32 magic; // 'BOLT'
	uint32 numDirs;
	uint32 fileSize;
};

BltFile::DirectoryEntry::DirectoryEntry(Common::File &file) {
	numResources = file.readUint32BE();
	compBufSize = file.readUint32BE();
	offset = file.readUint32BE();
	// Unknown. FIXME: What is this?
	file.readUint32BE();
}

BltFile::ResourceEntry::ResourceEntry(Common::File &file) {
	uint32 fullType = file.readUint32BE();
	compression = fullType >> 24;
	type = fullType & 0x00FFFFFFUL;
	size = file.readUint32BE(); // This is uncompressed size.
	offset = file.readUint32BE(); // Relative to beginning of file
	// Unknown. FIXME: What is this?
	file.readUint32BE();
}

// A BLT file contains game resources of all types. The resources are
// arranged in directories.
// A resource is addressed by two bytes: <directory number> <resource number>.
// For example, the resource 0x9D01 refers to resource 1 in directory 0x9D.
bool BltFile::init(const Common::String &filename) {

	// Open the file
	if (!_file.open(filename)) {
		warning("Failed to open %s", filename.c_str());
		return false;
	}

	BltFileHeader header(_file);
	if (header.magic != MKTAG('B', 'O', 'L', 'T')) {
		warning("BOLT magic value not found");
		return false;
	}

	_dirs.reserve(header.numDirs);
	for (byte i = 0; i < header.numDirs; ++i) {
		Directory dir;
		dir.entry = DirectoryEntry(_file);
		_dirs.push_back(dir);
	}

	return true;
}

// Decompress a BOLT LZ-compressed resource. dst must be sized to fit the
// decompressed data.
static void decompressBoltLZ(Common::Array<byte> &dst,
	const Common::Array<byte> &src) {

	// BOLT's compression algorithm is an LZ variant with some questionable
	// design choices.
	int in_cursor = 0;
	int out_cursor = 0;
	while (out_cursor < dst.size()) {
		byte control = src[in_cursor];
		++in_cursor;

		byte comp = control >> 6;
		byte flag = (control >> 5) & 1;
		byte num = control & 0x1F; // in the range 0...31.

		if (comp == 0) {
			// Raw bytes
			int count = 31 - num; // 32 or 33 would have been more efficient.
			// Consider: If num == 31, count becomes 0 and this byte is wasted.
			memcpy(&dst[out_cursor], &src[in_cursor], count);
			out_cursor += count;
			in_cursor += count;
		}
		else if (comp == 1) {
			// Small repeat from previously-decoded window
			int count = 35 - num; // 35 makes the compression factor break even.
			int offset = src[in_cursor] + (flag ? 256 : 0);
			++in_cursor;
			// We must use byte-by-byte copy to support situation where count
			// exceeds offset, causing the window to repeat.
			for (byte i = 0; i < count; ++i) {
				dst[out_cursor] = dst[out_cursor - offset];
				++out_cursor;
			}
		}
		else if (comp == 2) {
			// Big repeat from previously-decoded window
			int count = (32 - num) * 4 + (flag ? 2 : 0);
			int offset = src[in_cursor] * 2;
			++in_cursor;
			for (int i = 0; i < count; ++i) {
				dst[out_cursor] = dst[out_cursor - offset];
				++out_cursor;
			}
		}
		else if (comp == 3 && flag) {
			// Original checked for end of data in here. I check on every loop
			// iteration.
			// Sometimes, they trigger this check even when the data has not
			// ended. It's a wasted byte!
		}
		else if (comp == 3 && !flag) {
			// Big block filled with single byte
			int count = (32 - num + 32 * src[in_cursor]) * 4;
			++in_cursor;
			++in_cursor; // Ignored!
			byte b = src[in_cursor];
			++in_cursor;
			memset(&dst[out_cursor], b, count);
			out_cursor += count;
		}
		else {
			assert(false); // Unreachable
		}
	}
}

BltResourcePtr BltFile::loadShortId(BltShortId id) {
	byte dirNum = id.value >> 8;
	byte resNum = id.value & 0xFF;

	ensureDirLoaded(dirNum);

	Directory &dir = _dirs[dirNum];
	ResourceEntry &res = dir.resEntries[resNum];

	// Load and decompress resource
	BltResourcePtr ptr(new BltResource);
	ptr->_type = res.type;
	ptr->_data.resize(res.size);
	if (res.compression == 0) {
		// BOLT-LZ
		Common::Array<byte> buf;
		buf.resize(dir.entry.compBufSize);
		_file.seek(res.offset);
		_file.read(&buf[0], dir.entry.compBufSize);
		decompressBoltLZ(ptr->_data, buf);
	}
	else if (res.compression == 8) {
		// Raw
		_file.seek(res.offset);
		_file.read(&ptr->_data[0], res.size);
	}
	else {
		error("Unknown compression type %d", (int)res.compression);
		return BltResourcePtr();
	}

	return ptr;
}

BltResourcePtr BltFile::loadLongId(BltLongId id) {

	if (id.value == 0xFFFFFFFFUL) {
		// Special value meaning "nothing"
		return BltResourcePtr();
	}

	BltShortId shortId = BltShortId(id.value >> 16);
	uint16 offset = id.value & 0xFFFF;

	if (offset != 0) {
		// XXX: offset is not handled. I don't believe it is ever non-zero.
		error("offset field in resource id is not 0 (it is 0x%.04X)", (int)offset);
		return BltResourcePtr();
	}

	return loadShortId(shortId);
}

void BltFile::ensureDirLoaded(byte dirNum) {
	Directory& dir = _dirs[dirNum];
	if (dir.resEntries.empty()) {

		// Seek to position of resource entries
		_file.seek(dir.entry.offset);

		// Load resource entries
		dir.resEntries.reserve(dir.entry.numResources);
		for (uint32 i = 0; i < dir.entry.numResources; ++i) {
			ResourceEntry entry(_file);
			dir.resEntries.push_back(entry);
		}
	}
}

} // End of namespace Bolt
