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

#ifndef BOLT_BLT_FILE_H
#define BOLT_BLT_FILE_H

#include "common/file.h"

#include "bolt/util.h"

namespace Bolt {

enum BltType {
	kBltResourceList = 6,
	kBltImage = 8,
	kBltPalette = 10,
	kBltColorCycles = 11,
	kBltColorCycleSlot = 12,
	kBltPlane = 26, // image, palette, hotspots
	kBltSpriteList = 27, // image, x, y
	kBltButtonColors = 28, // just some colors, used by palette mod
	kBltButtonPaletteMod = 29,
	kBltButtonGraphics = 30,
	kBltButtonList = 31,
	kBltScene = 32,
	kBltMainMenu = 33,
	kBltHub = 40,
	kBltHubItem = 41,
};

struct BltShortId {
	BltShortId() : value(0xFFFFU) { }
	explicit BltShortId(uint16 v) : value(v) { }

	// ID is made of two 8-bit parts: <directory number> <resource number>.
	uint16 value;
};

struct BltLongId {
	BltLongId() : value(0xFFFFFFFFUL) { }
	BltLongId(BltShortId shortId) : value(shortId.value << 16) { }
	explicit BltLongId(uint32 v) : value(v) { }
	bool isValid() const { return value != 0xFFFFFFFFUL; }

	// ID is made of two 16-bit parts: <short id> <offset>.
	// offset should always be zero.
	uint32 value;
};

typedef ScopedArray<byte> BltResource;

class BltFile {
public:
	bool load(const Common::String &filename);

	BltResource::Movable loadResource(BltLongId id, uint32 expectedType);

private:
	// Warning: may clobber file cursor
	void ensureDirLoaded(byte dirNum);

	Common::File _file;

	struct DirectoryEntry {

		static const int SIZE = 0x10;
		DirectoryEntry() { }
		DirectoryEntry(Common::File &file);

		uint32 numResources;
		uint32 compressedSize; // Number of bytes to read for decompression
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

template<class T>
class BltSimpleReader {
	// Generic template for creating a simple loader and parser for a BLT
	// resource.
	// Template parameter T must have:
	// - static const uint32 kType equal to the resource type number
	// - static const uint kSize equal to the size of the resource in bytes
	// - constructor T(const byte *src) which parses from src into a T structure
public:
	BltSimpleReader(BltFile &bltFile, BltLongId id) {
		_res.reset(bltFile.loadResource(id, T::kType));
	}

	T get(uint i) const {
		assert(i < _res.size() / T::kSize);
		return T(&_res[i * T::kSize]);
	}
private:
	BltResource _res;
};

struct BltResourceListStruct { // type 6
	static const uint32 kType = kBltResourceList;
	static const uint kSize = 4;
	BltResourceListStruct(const byte *src) {
		value = BltLongId(READ_BE_UINT32(&src[0]));
	}

	BltLongId value;
};

typedef BltSimpleReader<BltResourceListStruct> BltResourceList;

} // End of namespace Bolt

#endif
