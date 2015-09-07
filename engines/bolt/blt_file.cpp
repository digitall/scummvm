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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
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
static void decompressBoltLZ(ScopedArray<byte> &dst,
	const ScopedArray<byte> &src) {

	// BOLT's compression algorithm is an LZ variant with some questionable
	// design choices.
	int inCursor = 0;
	int outCursor = 0;
	while (outCursor < (int)dst.size()) {
		byte control = src[inCursor];
		++inCursor;

		byte comp = control >> 6;
		byte flag = (control >> 5) & 1;
		byte num = control & 0x1F; // in the range 0...31.

		if (comp == 0) {
			// Raw bytes
			int count = 31 - num; // 32 or 33 would have been more efficient.
			// Consider: If num == 31, count becomes 0 and this byte is wasted.
			memcpy(&dst[outCursor], &src[inCursor], count);
			outCursor += count;
			inCursor += count;
		}
		else if (comp == 1) {
			// Small repeat from previous data
			int count = 35 - num; // 35 makes the compression factor break even.
			int offset = src[inCursor] + (flag ? 256 : 0);
			++inCursor;
			// We must use byte-by-byte copy to behave correctly when count
			// exceeds offset causing data to repeat multiple times.
			for (byte i = 0; i < count; ++i) {
				dst[outCursor] = dst[outCursor - offset];
				++outCursor;
			}
		}
		else if (comp == 2) {
			// Big repeat from previous data
			int count = (32 - num) * 4 + (flag ? 2 : 0);
			int offset = src[inCursor] * 2;
			++inCursor;
			for (int i = 0; i < count; ++i) {
				dst[outCursor] = dst[outCursor - offset];
				++outCursor;
			}
		}
		else if (comp == 3 && flag) {
			// Original program checked for end of data here. We check on every
			// loop iteration.
		}
		else if (comp == 3 && !flag) {
			// Big block filled with constant byte
			int count = (32 - num + 32 * src[inCursor]) * 4;
			++inCursor;
			++inCursor; // This byte is ignored!
			byte b = src[inCursor];
			++inCursor;
			memset(&dst[outCursor], b, count);
			outCursor += count;
		}
		else {
			assert(false); // Unreachable
		}
	}
}

BltResource::Movable BltFile::loadShortId(BltShortId id, uint32 expectType) {
	byte dirNum = id.value >> 8;
	byte resNum = id.value & 0xFF;

	if (dirNum >= _dirs.size()) {
		error("Tried to load non-existent resource 0x%.04X", (int)id.value);
		return BltResource::Movable();
	}

	ensureDirLoaded(dirNum);
	Directory &dir = _dirs[dirNum];

	if (resNum >= dir.resEntries.size()) {
		error("Tried to load non-existent resource 0x%.04X", (int)id.value);
		return BltResource::Movable();
	}

	ResourceEntry &res = dir.resEntries[resNum];

	if (res.type != expectType) {
		error("Tried to load wrong resource type %d instead of %d",
			(int)res.type, (int)expectType);
		return BltResource::Movable();
	}

	// Load and decompress resource
	BltResource ptr;
	ptr.reset(res.size);
	if (res.compression == 0) {
		// BOLT-LZ
		ScopedArray<byte> buf;
		buf.reset(dir.entry.compBufSize);
		_file.seek(res.offset);
		_file.read(&buf[0], dir.entry.compBufSize);
		decompressBoltLZ(ptr, buf);
	}
	else if (res.compression == 8) {
		// Raw
		_file.seek(res.offset);
		_file.read(&ptr[0], res.size);
	}
	else {
		error("Unknown compression type %d", (int)res.compression);
		return BltResource::Movable();
	}

	return ptr.release();
}

BltResource::Movable BltFile::loadLongId(BltLongId id, uint32 expectType) {

	if (!id.isValid()) {
		return BltResource::Movable();
	}

	BltShortId shortId = BltShortId(id.value >> 16);
	uint16 offset = id.value & 0xFFFF;

	if (offset != 0) {
		// XXX: offset is not handled. It is always zero.
		error("offset part of long resource id is not 0 (it is 0x%.04X)", (int)offset);
		return BltResource::Movable();
	}

	return loadShortId(shortId, expectType);
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
