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

#ifndef BOLT_BOLTLIB_BOLTLIB_H
#define BOLT_BOLTLIB_BOLTLIB_H

#include "common/file.h"
#include "common/rect.h"

#include "bolt/util.h"

namespace Bolt {

enum BltType {
	kBltS16Values = 2, // signed 16-bit values
	kBltU16Values = 3, // unsigned 16-bit values
	kBltResourceList = 6,
	kBltImage = 8,
	kBltPalette = 10,
	kBltColorCycles = 11,
	kBltColorCycleSlot = 12,
	kBltPlane = 26, // image, palette, hotspots
	kBltSpriteList = 27, // image, x, y
	kBltButtonColors = 28, // just some colors, used by palette mod
	kBltButtonPaletteMod = 29,
	kBltButtonGraphicsList = 30,
	kBltButtonList = 31,
	kBltScene = 32,
	kBltMainMenu = 33,
	kBltHub = 40,
	kBltHubItem = 41,
	kBltSlidingPuzzle = 44,
	kBltParticles = 46, // in action puzzles
};

struct BltShortId {
	static const uint16 kInvalid = 0xFFFF;

	BltShortId() : value(kInvalid) { }
	explicit BltShortId(uint16 v) : value(v) { }

	// ID is made of two 8-bit parts: <directory number> <resource number>.
	uint16 value;
};

struct BltId {
	static const uint32 kInvalid = 0xFFFFFFFFUL;

	BltId() : value(kInvalid) { }
	BltId(BltShortId shortId) : value(shortId.value << 16) { }
	explicit BltId(uint32 v) : value(v) { }
	bool isValid() const { return value != kInvalid; }

	// ID is made of two 16-bit parts: <short id> <offset>.
	// offset should always be zero.
	uint32 value;
};

typedef ScopedArray<byte> BltResource;

class Boltlib {
public:
	bool load(const Common::String &filename);

	BltResource::Movable loadResource(BltId id, uint32 expectedType);

private:
	// Warning: may clobber file cursor
	void ensureDirLoaded(byte dirNum);

	Common::File _file;

	struct DirectoryEntry {
		DirectoryEntry() { }
		DirectoryEntry(Common::File &file);

		uint32 numResources;
		uint32 compReadSize; // Number of bytes to read for decompression
		uint32 offset; // Offset of the resource table of this directory
		// (relative to beginning of file)
	};

	struct ResourceEntry {
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
class BltLoader {
	// Generic template for creating a simple loader and parser for a BLT
	// resource.
	// Template parameter T must have:
	// - static const uint32 kType: resource type number
	// - static const uint kSize: expected size
	// - T(): initialize T to default unloaded state.
	// - void load(const byte *src, BltFile &bltFile): parse from src.
	//   Additional resources may be loaded from bltFile.
	// Use arrow operator -> to access.
public:
	BltLoader() { }
	BltLoader(Boltlib &bltFile, BltId id) {
		load(bltFile, id);
	}

	void load(Boltlib &bltFile, BltId id) {
		BltResource res(bltFile.loadResource(id, T::kType));
		// Reset _data to unloaded state
		_data.~T();
		new(&_data) T();
		if (res) {
			assert(res.size() == T::kSize);
			_data.load(&res[0], bltFile);
		}
	}

	const T* operator->() const {
		return &_data;
	}
private:
	T _data;
};

template<class T>
class BltArrayLoader {
	// Generic template for creating a simple loader and parser for a BLT
	// resource. This template supports resources that are simple constant-
	// -sized elements in an array.
	// Template parameter T must have:
	// - static const uint32 kType: resource type number
	// - static const uint kSize: expected size of an element
	// - T(): initialize T to default unloaded state.
	// - void load(const byte *src, BltFile &bltFile): parse from src.
	//   Additional resources may be loaded from bltFile.
	// Use array indexing [] to access.
public:
	BltArrayLoader() { }
	BltArrayLoader(Boltlib &bltFile, BltId id) {
		load(bltFile, id);
	}

	operator bool() const {
		return _array;
	}

	void load(Boltlib &bltFile, BltId id) { // FIXME: expectedCount? Count is usually known in advance...
		BltResource res(bltFile.loadResource(id, T::kType));
		uint numItems = res.size() / T::kSize;
		_array.alloc(numItems);
		for (uint i = 0; i < numItems; ++i) {
			_array[i].load(&res[i * T::kSize], bltFile);
		}
	}

	uint size() const {
		return _array.size();
	}

	const T& operator[](uint i) const {
		return _array[i];
	}
private:
	ScopedArray<T> _array;
};

struct BltS16ValuesStruct { // type 2
	static const uint32 kType = kBltS16Values;
	static const uint kSize = 2;
	void load(const byte *src, Boltlib &boltlib) {
		value = READ_BE_INT16(&src[0]);
	}

	int16 value;
};

typedef BltArrayLoader<BltS16ValuesStruct> BltS16Values;

struct BltU16ValuesStruct { // type 3
	static const uint32 kType = kBltU16Values;
	static const uint kSize = 2;
	void load(const byte *src, Boltlib &bltFile) {
		value = READ_BE_UINT16(src);
	}

	uint16 value;
};

typedef BltArrayLoader<BltU16ValuesStruct> BltU16Values;

struct BltResourceListStruct { // type 6
	static const uint32 kType = kBltResourceList;
	static const uint kSize = 4;
	void load(const byte *src, Boltlib &bltFile) {
		value = BltId(READ_BE_UINT32(&src[0]));
	}

	BltId value;
};

typedef BltArrayLoader<BltResourceListStruct> BltResourceList;

} // End of namespace Bolt

#endif
