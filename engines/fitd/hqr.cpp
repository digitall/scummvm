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

byte *hqrGet(HqrEntry *hqrPtr, int index) {

	if (index < 0)
		return nullptr;

	HqrSubEntry *foundEntry = quickFindEntry(index, hqrPtr->numUsedEntry, hqrPtr->entries);

	if (foundEntry) {
		foundEntry->lastTimeUsed = g_engine->_engine->timer;
		g_engine->_engine->hqLoad = 0;

		return foundEntry->ptr;
	}

	freezeTime();
	const int size = pakGetPakSize(hqrPtr->string, index);

	if (size == 0)
		return nullptr;

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

	return ptr;
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

	if (hqrPtr == g_engine->_engine->listBody) {
		for (uint i = 0; i < g_engine->_engine->bodies.size(); i++) {
			delete g_engine->_engine->bodies[i];
		}
		g_engine->_engine->bodies.resize(0);
	}

	for (uint i = 0; i < hqrPtr->numMaxEntry; i++) {
		if (hqrPtr->entries[i].ptr)
			free(hqrPtr->entries[i].ptr);

		hqrPtr->entries[i].ptr = nullptr;
	}
}

void hqrFree(HqrEntry *hqrPtr) {
	if (!hqrPtr)
		return;

	if (hqrPtr == g_engine->_engine->listBody) {
		for (uint i = 0; i < g_engine->_engine->bodies.size(); i++) {
			delete g_engine->_engine->bodies[i];
		}
		g_engine->_engine->bodies.clear();
	}

	if (hqrPtr == g_engine->_engine->listAnim) {
		for (uint i = 0; i < g_engine->_engine->animations.size(); i++) {
			delete g_engine->_engine->animations[i];
		}
		g_engine->_engine->animations.clear();
	}

	for (int i = 0; i < hqrPtr->numMaxEntry; i++) {
		if (hqrPtr->entries[i].ptr)
			free(hqrPtr->entries[i].ptr);
	}

	free(hqrPtr);
}

void hqrName(HqrEntry *ptr, const char *name) {
	memcpy(ptr->string, name, strlen(name));
}

static Body *createBodyFromPtr(void *ptr) {
	uint8 *bodyBuffer = static_cast<uint8 *>(ptr);

	Body *newBody = new Body;

	newBody->m_raw = ptr;
	newBody->m_flags = READ_LE_U16(bodyBuffer);
	bodyBuffer += 2;

	newBody->m_zv.ZVX1 = READ_LE_S16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_zv.ZVX2 = READ_LE_S16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_zv.ZVY1 = READ_LE_S16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_zv.ZVY2 = READ_LE_S16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_zv.ZVZ1 = READ_LE_S16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_zv.ZVZ2 = READ_LE_S16(bodyBuffer);
	bodyBuffer += 2;

	uint16 scratchBufferSize = READ_LE_U16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_scratchBuffer.resize(scratchBufferSize);
	for (int i = 0; i < scratchBufferSize; i++) {
		newBody->m_scratchBuffer[i] = READ_LE_U8(bodyBuffer);
		bodyBuffer += 1;
	}

	uint16 numVertices = READ_LE_U16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_vertices.resize(numVertices);
	for (int i = 0; i < numVertices; i++) {
		newBody->m_vertices[i].x = READ_LE_S16(bodyBuffer);
		bodyBuffer += 2;
		newBody->m_vertices[i].y = READ_LE_S16(bodyBuffer);
		bodyBuffer += 2;
		newBody->m_vertices[i].z = READ_LE_S16(bodyBuffer);
		bodyBuffer += 2;
	}

	if (newBody->m_flags & INFO_TORTUE) {
		assert(0); // never used
	}

	if (newBody->m_flags & INFO_ANIM) {
		uint16 numGroups = READ_LE_U16(bodyBuffer);
		bodyBuffer += 2;
		newBody->m_groupOrder.reserve(numGroups);
		newBody->m_groups.resize(numGroups);

		if (newBody->m_flags & INFO_OPTIMISE) // AITD2+
		{
			for (int i = 0; i < numGroups; i++) {
				uint16 offset = READ_LE_U16(bodyBuffer);
				assert(offset % 0x18 == 0);
				newBody->m_groupOrder.push_back(offset / 0x18);
				bodyBuffer += 2;
			}

			for (int i = 0; i < numGroups; i++) {
				newBody->m_groups[i].m_start = READ_LE_S16(bodyBuffer) / 6;
				bodyBuffer += 2;
				newBody->m_groups[i].m_numVertices = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_baseVertices = READ_LE_S16(bodyBuffer) / 6;
				bodyBuffer += 2;
				newBody->m_groups[i].m_orgGroup = READ_LE_S8(bodyBuffer);
				bodyBuffer += 1;
				newBody->m_groups[i].m_numGroup = READ_LE_S8(bodyBuffer);
				bodyBuffer += 1;
				newBody->m_groups[i].m_state.m_type = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_delta[0] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_delta[1] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_delta[2] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_rotateDelta[0] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_rotateDelta[1] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_rotateDelta[2] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				bodyBuffer += 2; // padding?
			}
		} else {
			for (int i = 0; i < numGroups; i++) {
				uint16 offset = READ_LE_U16(bodyBuffer);
				assert(offset % 0x10 == 0);
				newBody->m_groupOrder.push_back(offset / 0x10);
				bodyBuffer += 2;
			}

			for (int i = 0; i < numGroups; i++) {
				newBody->m_groups[i].m_start = READ_LE_S16(bodyBuffer) / 6;
				bodyBuffer += 2;
				newBody->m_groups[i].m_numVertices = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_baseVertices = READ_LE_S16(bodyBuffer) / 6;
				bodyBuffer += 2;
				newBody->m_groups[i].m_orgGroup = READ_LE_S8(bodyBuffer);
				bodyBuffer += 1;
				newBody->m_groups[i].m_numGroup = READ_LE_S8(bodyBuffer);
				bodyBuffer += 1;
				newBody->m_groups[i].m_state.m_type = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_delta[0] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_delta[1] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
				newBody->m_groups[i].m_state.m_delta[2] = READ_LE_S16(bodyBuffer);
				bodyBuffer += 2;
			}
		}
	}

	uint16 numPrimitives = READ_LE_U16(bodyBuffer);
	bodyBuffer += 2;
	newBody->m_primitives.resize(numPrimitives);
	for (int i = 0; i < numPrimitives; i++) {
		newBody->m_primitives[i].m_type = static_cast<PrimType>(READ_LE_U8(bodyBuffer));
		bodyBuffer += 1;

		switch (newBody->m_primitives[i].m_type) {
		case primTypeEnum_Line:
			newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_even = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_points.resize(2);
			for (uint j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6;
				bodyBuffer += 2;
			}
			break;
		case primTypeEnum_Poly:
			newBody->m_primitives[i].m_points.resize(READ_LE_U8(bodyBuffer));
			bodyBuffer += 1;
			newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			for (uint j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6;
				bodyBuffer += 2;
			}
			break;
		case primTypeEnum_Point:
		case primTypeEnum_BigPoint:
		case primTypeEnum_Zixel:
			newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_even = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_points.resize(1);
			for (uint j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6;
				bodyBuffer += 2;
			}
			break;
		case primTypeEnum_Sphere:
			newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_even = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_size = READ_LE_U16(bodyBuffer);
			bodyBuffer += 2;
			newBody->m_primitives[i].m_points.resize(1);
			for (uint j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6;
				bodyBuffer += 2;
			}
			break;
		case processPrim_PolyTexture8:
			newBody->m_primitives[i].m_points.resize(READ_LE_U8(bodyBuffer));
			bodyBuffer += 1;
			newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			for (uint j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6;
				bodyBuffer += 2;
			}
			break;
		case processPrim_PolyTexture9:
		case processPrim_PolyTexture10:
			newBody->m_primitives[i].m_points.resize(READ_LE_U8(bodyBuffer));
			bodyBuffer += 1;
			newBody->m_primitives[i].m_material = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			newBody->m_primitives[i].m_color = READ_LE_U8(bodyBuffer);
			bodyBuffer += 1;
			for (uint j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				newBody->m_primitives[i].m_points[j] = READ_LE_U16(bodyBuffer) / 6;
				bodyBuffer += 2;
			}
			// load UVS?
			for (uint j = 0; j < newBody->m_primitives[i].m_points.size(); j++) {
				READ_LE_U8(bodyBuffer);
				bodyBuffer += 1;
				READ_LE_U8(bodyBuffer);
				bodyBuffer += 1;
			}
			break;
		default:
			assert(0);
		}
	}

	g_engine->_engine->bodies.push_back(newBody);
	return newBody;
}

Body *getBodyFromPtr(void *ptr) {
	for (uint i = 0; i < g_engine->_engine->bodies.size(); i++) {
		if (g_engine->_engine->bodies[i]->m_raw == ptr) {
			return g_engine->_engine->bodies[i];
		}
	}

	return createBodyFromPtr(ptr);
}

} // namespace Fitd
