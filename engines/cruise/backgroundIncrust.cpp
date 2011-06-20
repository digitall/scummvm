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

#include "cruise/cruise_main.h"
#include "cruise/cruise.h"

namespace Cruise {

void backgroundIncrustListNode::resetBackgroundIncrustList() {
	next = NULL;
	prev = NULL;
}

// blit background to another one
void addBackgroundIncrustSub1(int fileIdx, int X, int Y, char *ptr2, int16 scale, char *destBuffer, char *dataPtr) {
	assert((dataPtr != NULL) && (*dataPtr != 0));

	buildPolyModel(X, Y, scale, ptr2, destBuffer, dataPtr);
}

void backupBackground(backgroundIncrustListNode *pIncrust, int X, int Y, int width, int height, uint8* pBackground) {
	pIncrust->_saveWidth = width;
	pIncrust->_saveHeight = height;
	pIncrust->_saveSize = width * height;
	pIncrust->_savedX = X;
	pIncrust->_savedY = Y;

	pIncrust->_ptr = (uint8 *)MemAlloc(width * height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int xp = j + X;
			int yp = i + Y;

			pIncrust->_ptr[i * width + j] = ((xp < 0) || (yp < 0) || (xp >= 320) || (yp >= 200)) ?
				0 : pBackground[yp * 320 + xp];
		}
	}
}

void restoreBackground(backgroundIncrustListNode *pIncrust) {
	if (pIncrust->_type != 1)
		return;
	if (pIncrust->_ptr == NULL)
		return;

	uint8* pBackground = backgrounds[pIncrust->_backgroundIdx]._backgroundScreen;
	if (pBackground == NULL)
		return;

	backgrounds[pIncrust->_backgroundIdx]._isChanged = true;

	int X = pIncrust->_savedX;
	int Y = pIncrust->_savedY;
	int width = pIncrust->_saveWidth;
	int height = pIncrust->_saveHeight;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int xp = j + X;
			int yp = i + Y;

			if ((xp >= 0) && (yp >= 0) && (xp < 320) && (yp < 200))
				pBackground[yp * 320 + xp] = pIncrust->_ptr[i * width + j];
		}
	}
}

backgroundIncrustListNode *addBackgroundIncrust(int16 overlayIdx,	int16 objectIdx, backgroundIncrustListNode *pHead, int16 scriptNumber, int16 scriptOverlay, int16 backgroundIdx, int16 saveBuffer) {
	uint8 *backgroundPtr;
	uint8 *ptr;
	objectParamsQuery params;
	backgroundIncrustListNode *newElement;
	backgroundIncrustListNode *currentHead;
	backgroundIncrustListNode *currentHead2;

	getMultipleObjectParam(overlayIdx, objectIdx, &params);

	ptr = filesDatabase[params.fileIdx].subData.ptr;

	// Don't process any further if not a sprite or polygon
	if (!ptr) return NULL;
	if ((filesDatabase[params.fileIdx].subData.resourceType != OBJ_TYPE_SPRITE) &&
		(filesDatabase[params.fileIdx].subData.resourceType != OBJ_TYPE_POLY)) {
		return NULL;
	}

	backgroundPtr = backgrounds[backgroundIdx]._backgroundScreen;

	backgrounds[backgroundIdx]._isChanged = true;

	assert(backgroundPtr != NULL);

	currentHead = pHead;
	currentHead2 = currentHead->next;

	while (currentHead2) {
		currentHead = currentHead2;
		currentHead2 = currentHead->next;
	}

	newElement = (backgroundIncrustListNode *)mallocAndZero(sizeof(backgroundIncrustListNode));

	if (!newElement)
		return NULL;

	newElement->next = currentHead->next;
	currentHead->next = newElement;

	if (!currentHead2) {
		currentHead2 = pHead;
	}

	newElement->prev = currentHead2->prev;
	currentHead2->prev = newElement;

	newElement->_objectIdx = objectIdx;
	newElement->_type = saveBuffer;
	newElement->_backgroundIdx = backgroundIdx;
	newElement->_overlayIdx = overlayIdx;
	newElement->_scriptNumber = scriptNumber;
	newElement->_scriptOverlayIdx = scriptOverlay;
	newElement->_X = params.X;
	newElement->_Y = params.Y;
	newElement->_scale = params.scale;
	newElement->_frame = params.fileIdx;
	newElement->_spriteId = filesDatabase[params.fileIdx].subData.index;
	newElement->_ptr = NULL;
	strcpy(newElement->_name, filesDatabase[params.fileIdx].subData.name);

	if (filesDatabase[params.fileIdx].subData.resourceType == OBJ_TYPE_SPRITE) {
		// sprite
		int width = filesDatabase[params.fileIdx].width;
		int height = filesDatabase[params.fileIdx].height;
		if (saveBuffer == 1) {
			backupBackground(newElement, newElement->_X, newElement->_Y, width, height, backgroundPtr);
		}

		drawSprite(width, height, NULL, filesDatabase[params.fileIdx].subData.ptr, newElement->_Y,
			newElement->_X, backgroundPtr, filesDatabase[params.fileIdx].subData.ptrMask);
	} else {
		// poly
		if (saveBuffer == 1) {
			int newX;
			int newY;
			int newScale;
			char *newFrame;

			int sizeTable[4];	// 0 = left, 1 = right, 2 = bottom, 3 = top

			// this function checks if the dataPtr is not 0, else it retrives the data for X, Y, scale and DataPtr again (OLD: mainDrawSub1Sub1)
			flipPoly(params.fileIdx, (int16 *)filesDatabase[params.fileIdx].subData.ptr, params.scale, &newFrame, newElement->_X, newElement->_Y, &newX, &newY, &newScale);

			// this function fills the sizeTable for the poly (OLD: mainDrawSub1Sub2)
			getPolySize(newX, newY, newScale, sizeTable, (unsigned char*)newFrame);

			int width = (sizeTable[1] + 2) - (sizeTable[0] - 2) + 1;
			int height = sizeTable[3] - sizeTable[2] + 1;

			backupBackground(newElement, sizeTable[0] - 2, sizeTable[2], width, height, backgroundPtr);
		}

		addBackgroundIncrustSub1(params.fileIdx, newElement->_X, newElement->_Y, NULL, params.scale, (char *)backgroundPtr, (char *)filesDatabase[params.fileIdx].subData.ptr);
	}

	return newElement;
}

void regenerateBackgroundIncrust(backgroundIncrustListNode *pHead) {

	lastAni[0] = 0;

	backgroundIncrustListNode* pl = pHead->next;

	while (pl) {
		backgroundIncrustListNode* pl2 = pl->next;

		int frame = pl->_frame;
		//int screen = pl->backgroundIdx;

		if ((filesDatabase[frame].subData.ptr == NULL) || (strcmp(pl->_name, filesDatabase[frame].subData.name))) {
			frame = NUM_FILE_ENTRIES - 1;
			if (loadFile(pl->_name, frame, pl->_spriteId) < 0)
				frame = -1;
		}

		if (frame >= 0) {
			if (filesDatabase[frame].subData.resourceType == OBJ_TYPE_SPRITE) {
				// Sprite
				int width = filesDatabase[frame].width;
				int height = filesDatabase[frame].height;

				drawSprite(width, height, NULL, filesDatabase[frame].subData.ptr, pl->_Y, pl->_X, backgrounds[pl->_backgroundIdx]._backgroundScreen, filesDatabase[frame].subData.ptrMask);
			} else {
				// Poly
				addBackgroundIncrustSub1(frame, pl->_X, pl->_Y, NULL, pl->_scale, (char*)backgrounds[pl->_backgroundIdx]._backgroundScreen, (char *)filesDatabase[frame].subData.ptr);
			}

			backgrounds[pl->_backgroundIdx]._isChanged = true;
		}

		pl = pl2;
	}

	lastAni[0] = 0;
}

void backgroundIncrustListNode::freeBackgroundIncrustList() {
	backgroundIncrustListNode *pCurrent = next;

	while (pCurrent) {
		backgroundIncrustListNode *pNext = pCurrent->next;

		if (pCurrent->_ptr)
			MemFree(pCurrent->_ptr);

		MemFree(pCurrent);

		pCurrent = pNext;
	}
        
	resetBackgroundIncrustList();
}

void removeBackgroundIncrust(int overlay, int idx, backgroundIncrustListNode * pHead) {
	objectParamsQuery params;
	int var_4;
	int var_6;

	backgroundIncrustListNode *pCurrent;
	backgroundIncrustListNode *pCurrentHead;

	getMultipleObjectParam(overlay, idx, &params);

	var_4 = params.X;
	var_6 = params.Y;

	pCurrent = pHead->next;

	while (pCurrent) {
		if ((pCurrent->_overlayIdx == overlay || overlay == -1) && (pCurrent->_objectIdx == idx || idx == -1) && (pCurrent->_X == var_4) && (pCurrent->_Y == var_6)) {
			pCurrent->_type = - 1;
		}

		pCurrent = pCurrent->next;
	}

	pCurrentHead = pHead;
	pCurrent = pHead->next;

	while (pCurrent) {
		if (pCurrent->_type == - 1) {
			backgroundIncrustListNode *pNext = pCurrent->next;
			backgroundIncrustListNode *bx = pCurrentHead;
			backgroundIncrustListNode *cx;

			bx->next = pNext;
			cx = pNext;

			if (!pNext) {
				cx = pHead;
			}

			bx = cx;
			bx->prev = pCurrent->next;

			if (pCurrent->_ptr) {
				MemFree(pCurrent->_ptr);
			}

			MemFree(pCurrent);

			pCurrent = pNext;
		} else {
			pCurrentHead = pCurrent;
			pCurrent = pCurrent->next;
		}
	}
}

void unmergeBackgroundIncrust(backgroundIncrustListNode * pHead, int ovl, int idx) {
	backgroundIncrustListNode *pl;
	backgroundIncrustListNode *pl2;

	objectParamsQuery params;
	getMultipleObjectParam(ovl, idx, &params);

	int x = params.X;
	int y = params.Y;

	pl = pHead;
	pl2 = pl;
	pl = pl2->next;

	while (pl) {
		pl2 = pl;
		if ((pl->_overlayIdx == ovl) || (ovl == -1))
			if ((pl->_objectIdx == idx) || (idx == -1))
				if ((pl->_X == x) && (pl->_Y == y))
					restoreBackground(pl);

		pl = pl2->next;
	}
}

} // End of namespace Cruise
