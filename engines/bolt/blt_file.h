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

#ifndef BOLT_BLT_FILE_H
#define BOLT_BLT_FILE_H

#include "common/file.h"

namespace Bolt {

enum BltType {
	kBltError = 0, // Not present in game data; only for internal use
	kBltImage = 8,
	kBltPalette = 10,
	kBltMenuBgImageAndPalette = 26,
	kBltLocImage = 27, // image ref plus x, y location?
	kBltColors = 28, // just some colors, used by palette mod
	kBltMenuPaletteMod = 29,
	kBltMenuHoverInfo = 30,
	kBltMenuButtonInfo = 31,
	kBltMenuBgInfo = 32,
	kBltMainMenuInfo = 33,
};

struct BltShortId {
	BltShortId() : value(0xFFFFU) { }
	explicit BltShortId(uint16 v) : value(v) { }
	uint16 value;
};

struct BltLongId {
	BltLongId() : value(0xFFFFFFFFUL) { }
	explicit BltLongId(uint32 v) : value(v) { }
	explicit BltLongId(BltShortId shortId) : value(shortId.value << 16) { }
	bool isValid() const { return value != 0xFFFFFFFFUL; }
	uint32 value;
};

class BltResource {
	friend class BltFile;
public:
	uint32 getType() const { return _type; }
	const Common::Array<byte>& getData() const { return _data; }

private:
	BltResource() : _type(kBltError) { }; // BltFile can create these

	uint32 _type;
	Common::Array<byte> _data;
};

typedef Common::SharedPtr<BltResource> BltResourcePtr;

class BltFile {
public:
	bool init(const Common::String &filename);

	BltResourcePtr loadShortId(BltShortId id); // id is two bytes: <dir num> <res num>
	BltResourcePtr loadLongId(BltLongId id); // id is two words: <short id> <offset>

private:
	// Warning: may clobber file cursor
	void ensureDirLoaded(byte dirNum);

	Common::File _file;

	struct DirectoryEntry {

		static const int SIZE = 0x10;
		DirectoryEntry() { }
		DirectoryEntry(Common::File &file);

		uint32 numResources;
		uint32 compBufSize; // Number of bytes to read for decompression
		uint32 offset; // Offset of the resource table of this directory
		// (relative to beginning of file)
	};

	struct ResourceEntry {

		static const int SIZE = 0x10;
		ResourceEntry() { }
		ResourceEntry(Common::File &file);

		byte compression; // 0: BOLT-LZ; 8: Raw
		uint32 type;
		uint32 size; // Uncompressed size
		uint32 offset; // Offset relative to beginning of file
	};

	struct Directory {
		DirectoryEntry entry;
		Common::Array<ResourceEntry> resEntries;
	};

	Common::Array<Directory> _dirs;
};

} // End of namespace Bolt

#endif
