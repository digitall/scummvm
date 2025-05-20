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

#ifndef HQR_H
#define HQR_H

#include "common/str.h"

namespace Fitd {

typedef struct hqrSubEntryStruct
{
    int16 key;
    int16 size;
    uint lastTimeUsed;
    char* ptr;
} hqrSubEntryStruct;

typedef struct hqrEntryStruct {
	Common::String string;
	uint16 maxFreeData;
	uint16 sizeFreeData;
	uint16 numMaxEntry;
	uint16 numUsedEntry;
	hqrSubEntryStruct *entries;
} hqrEntryStruct;

char *HQR_Get(hqrEntryStruct *hqrPtr, int index);
int HQ_Malloc(hqrEntryStruct *hqrPtr, int size);
char *HQ_PtrMalloc(hqrEntryStruct *hqrPtr, int index);
void HQ_Name(hqrEntryStruct * ptr, const char * name);
hqrEntryStruct *HQR_InitRessource(const char *name, int size, int numEntries);
hqrEntryStruct *HQR_Init(int size, int numEntry);
void HQR_Reset(hqrEntryStruct *hqrPtr);
void HQR_Free(hqrEntryStruct *hqrPtr);

struct sBody;
sBody *getBodyFromPtr(void *ptr);

struct sAnimation;
sAnimation *getAnimationFromPtr(void *ptr);

} // namespace Fitd

#endif
