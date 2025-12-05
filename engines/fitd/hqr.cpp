/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "fitd/hqr.h"
#include "common/array.h"
#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/game_time.h"
#include "fitd/pak.h"
#include "fitd/vars.h"

namespace Fitd {

typedef struct HqrSubEntry
{
    int16 key;
    int16 size;
    uint lastTimeUsed;
    byte* ptr;
} HqrSubEntry;

typedef struct HqrEntry {
	char string[14];
	uint16 maxFreeData;
	uint16 sizeFreeData;
	uint16 numMaxEntry;
	uint16 numUsedEntry;
	HqrSubEntry *entries;
} HqrEntry;

static HqrSubEntry *quickFindEntry(int index, int numMax, HqrSubEntry *ptr) {
	// no RE. Original was probably faster
	for (int i = 0; i < numMax; i++) {
		if (ptr[i].key == index && ptr[i].ptr) {
			return &ptr[i];
		}
	}

	return nullptr;
}

HqrEntry *hqrInitRessource(const char *name, int size, int numEntries) {

	HqrEntry *dest = static_cast<HqrEntry *>(malloc(sizeof(HqrEntry)));
	if (!dest)
		return nullptr;

	numEntries = 2000;

	memcpy(dest->string, name, strlen(name) + 1);
	dest->sizeFreeData = size;
	dest->maxFreeData = size;
	dest->numMaxEntry = numEntries;
	dest->numUsedEntry = 0;
	dest->entries = static_cast<HqrSubEntry *>(malloc(numEntries * sizeof(HqrSubEntry)));

	for (int i = 0; i < numEntries; i++) {
		dest->entries[i].ptr = nullptr;
	}

	return dest;
}

int hqMalloc(HqrEntry *hqrPtr, int size) {

	if (hqrPtr->sizeFreeData < size)
		return -1;

	const int entryNum = hqrPtr->numUsedEntry;

	HqrSubEntry *dataPtr1 = hqrPtr->entries;

	const int hq_key = g_engine->_engine->hqrKeyGen;

	dataPtr1[entryNum].key = hq_key;

	dataPtr1[entryNum].size = size;
	dataPtr1[entryNum].ptr = static_cast<byte *>(malloc(size));

	hqrPtr->numUsedEntry++;
	hqrPtr->sizeFreeData -= size;

	g_engine->_engine->hqrKeyGen++;

	return hq_key;
}

byte *hqPtrMalloc(HqrEntry *hqrPtr, int index) {

	if (index < 0)
		return nullptr;

	HqrSubEntry *dataPtr = hqrPtr->entries;

	const HqrSubEntry *ptr = quickFindEntry(index, hqrPtr->numUsedEntry, dataPtr);

	if (!ptr)
		return nullptr;

	return ptr->ptr;
}

HqData hqrGet(HqrEntry *hqrPtr, int index) {

	if (index < 0) {
		HqData emptyData = { nullptr, 0 };
		return emptyData;
	}

	HqrSubEntry *foundEntry = quickFindEntry(index, hqrPtr->numUsedEntry, hqrPtr->entries);

	if (foundEntry) {
		foundEntry->lastTimeUsed = g_engine->_engine->timer;
		g_engine->_engine->hqLoad = 0;

		HqData data = { foundEntry->ptr, foundEntry->size };
		return data;
	}

	freezeTime();
	const int size = pakGetPakSize(hqrPtr->string, index);

	if (size == 0) {
		HqData emptyData = { nullptr, 0 };
		return emptyData;
	}

	if (size >= hqrPtr->maxFreeData) {
		error("%s", hqrPtr->string);
	}

	for (int i = 0; i < hqrPtr->numMaxEntry; i++) {
		if (hqrPtr->entries[i].ptr == nullptr) {
			foundEntry = &hqrPtr->entries[i];
			break;
		}
	}

	assert(foundEntry);

	g_engine->_engine->hqLoad = 1;

	foundEntry->key = index;
	foundEntry->lastTimeUsed = g_engine->_engine->timer;
	foundEntry->size = size;
	foundEntry->ptr = static_cast<byte *>(malloc(size));

	byte *ptr = foundEntry->ptr;

	pakLoad(hqrPtr->string, index, foundEntry->ptr);

	hqrPtr->numUsedEntry++;
	hqrPtr->sizeFreeData -= size;

	unfreezeTime();

	HqData data = { ptr, static_cast<int16>(size) };
	return data;
}

HqrEntry *hqrInit(int size, int numEntry) {
	assert(size > 0);
	assert(numEntry > 0);

	HqrEntry *dest = (HqrEntry *)malloc(sizeof(HqrEntry));
	numEntry = 2000;

	if (!dest)
		return nullptr;

	memcpy(dest->string, "_MEMORY_", 9);
	dest->sizeFreeData = size;
	dest->maxFreeData = size;
	dest->numMaxEntry = numEntry;
	dest->numUsedEntry = 0;
	dest->entries = static_cast<HqrSubEntry *>(malloc(numEntry * sizeof(HqrSubEntry)));

	for (int i = 0; i < numEntry; i++) {
		dest->entries[i].ptr = nullptr;
	}

	return dest;
}

void hqrReset(HqrEntry *hqrPtr) {
	hqrPtr->sizeFreeData = hqrPtr->maxFreeData;
	hqrPtr->numUsedEntry = 0;

	for (uint i = 0; i < hqrPtr->numMaxEntry; i++) {
		if (hqrPtr->entries[i].ptr)
			free(hqrPtr->entries[i].ptr);

		hqrPtr->entries[i].ptr = nullptr;
	}
}

void hqrFree(HqrEntry *hqrPtr) {
	if (!hqrPtr)
		return;

	for (int i = 0; i < hqrPtr->numMaxEntry; i++) {
		if (hqrPtr->entries[i].ptr)
			free(hqrPtr->entries[i].ptr);
	}
	free(hqrPtr->entries);

	free(hqrPtr);
}

void hqrName(HqrEntry *ptr, const char *name) {
	memcpy(ptr->string, name, strlen(name));
}

Body getBodyFromPtr(void *ptr) {
	uint8 *bodyBuffer = static_cast<uint8 *>(ptr);
	Body body;
	body.m_raw = bodyBuffer;
	return body;
}

} // namespace Fitd
