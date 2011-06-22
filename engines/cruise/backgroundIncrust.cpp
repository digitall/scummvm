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

void BackgroundIncrustListNode::resetBackgroundIncrustList() {
	next = NULL;
	prev = NULL;
}

// blit background to another one
void addBackgroundIncrustSub1(int fileIdx, int X, int Y, char *ptr2, int16 scale, char *destBuffer, char *dataPtr) {
	assert((dataPtr != NULL) && (*dataPtr != 0));

	buildPolyModel(X, Y, scale, ptr2, destBuffer, dataPtr);
}

void backupBackground(BackgroundIncrustListNode *pIncrust, int X, int Y, int width, int height, uint8* pBackground) {
	BackgroundIncrust *currentBackgroundIncrust = pIncrust->backgroundIncrust;
	currentBackgroundIncrust->_saveWidth = width;
	currentBackgroundIncrust->_saveHeight = height;
	currentBackgroundIncrust->_saveSize = width * height;
	currentBackgroundIncrust->_savedX = X;
	currentBackgroundIncrust->_savedY = Y;

	currentBackgroundIncrust->_ptr = (uint8 *)MemAlloc(width * height);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int xp = j + X;
			int yp = i + Y;

			currentBackgroundIncrust->_ptr[i * width + j] = ((xp < 0) || (yp < 0) || (xp >= 320) || (yp >= 200)) ?
				0 : pBackground[yp * 320 + xp];
		}
	}
}

void restoreBackground(BackgroundIncrustListNode *pIncrust) {
	BackgroundIncrust *currentBackgroundIncrust = pIncrust->backgroundIncrust;
	if (currentBackgroundIncrust->_type != 1)
		return;
	if (currentBackgroundIncrust->_ptr == NULL)
		return;

	uint8* pBackground = backgrounds[currentBackgroundIncrust->_backgroundIdx]._backgroundScreen;
	if (pBackground == NULL)
		return;

	backgrounds[currentBackgroundIncrust->_backgroundIdx]._isChanged = true;

	int X = currentBackgroundIncrust->_savedX;
	int Y = currentBackgroundIncrust->_savedY;
	int width = currentBackgroundIncrust->_saveWidth;
	int height = currentBackgroundIncrust->_saveHeight;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int xp = j + X;
			int yp = i + Y;

			if ((xp >= 0) && (yp >= 0) && (xp < 320) && (yp < 200))
				pBackground[yp * 320 + xp] = currentBackgroundIncrust->_ptr[i * width + j];
		}
	}
}

BackgroundIncrustListNode *BackgroundIncrustListNode::addBackgroundIncrust(int16 overlayIdx, int16 objectIdx, int16 scriptNumber, int16 scriptOverlay, int16 backgroundIdx, int16 saveBuffer) {
	uint8 *backgroundPtr;
	uint8 *ptr;
	objectParamsQuery params;
	BackgroundIncrustListNode *newListNode;
	BackgroundIncrustListNode *currentHead;
	BackgroundIncrustListNode *currentHead2;
	BackgroundIncrust *newBackgroundIncrust;

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

	currentHead = this;
	currentHead2 = currentHead->next;

	while (currentHead2) {
		currentHead = currentHead2;
		currentHead2 = currentHead->next;
	}

	newListNode = (BackgroundIncrustListNode *)mallocAndZero(sizeof(BackgroundIncrustListNode));

	if (!newListNode)
		return NULL;

	newListNode->next = currentHead->next;
	currentHead->next = newListNode;

	if (!currentHead2) {
		currentHead2 = this;
	}

	newListNode->prev = currentHead2->prev;
	currentHead2->prev = newListNode;
	newBackgroundIncrust = (BackgroundIncrust *)mallocAndZero(sizeof(BackgroundIncrust));

	newBackgroundIncrust->_objectIdx = objectIdx;
	newBackgroundIncrust->_type = saveBuffer;
	newBackgroundIncrust->_backgroundIdx = backgroundIdx;
	newBackgroundIncrust->_overlayIdx = overlayIdx;
	newBackgroundIncrust->_scriptNumber = scriptNumber;
	newBackgroundIncrust->_scriptOverlayIdx = scriptOverlay;
	newBackgroundIncrust->_X = params.X;
	newBackgroundIncrust->_Y = params.Y;
	newBackgroundIncrust->_scale = params.scale;
	newBackgroundIncrust->_frame = params.fileIdx;
	newBackgroundIncrust->_spriteId = filesDatabase[params.fileIdx].subData.index;
	newBackgroundIncrust->_ptr = NULL;
	strcpy(newBackgroundIncrust->_name, filesDatabase[params.fileIdx].subData.name);

	newListNode->backgroundIncrust = newBackgroundIncrust;

	if (filesDatabase[params.fileIdx].subData.resourceType == OBJ_TYPE_SPRITE) {
		// sprite
		int width = filesDatabase[params.fileIdx].width;
		int height = filesDatabase[params.fileIdx].height;
		if (saveBuffer == 1) {
			backupBackground(newListNode, newBackgroundIncrust->_X, newBackgroundIncrust->_Y, width, height, backgroundPtr);
		}

		drawSprite(width, height, NULL, filesDatabase[params.fileIdx].subData.ptr, newBackgroundIncrust->_Y,
			newBackgroundIncrust->_X, backgroundPtr, filesDatabase[params.fileIdx].subData.ptrMask);
	} else {
		// poly
		if (saveBuffer == 1) {
			int newX;
			int newY;
			int newScale;
			char *newFrame;

			int sizeTable[4];	// 0 = left, 1 = right, 2 = bottom, 3 = top

			// this function checks if the dataPtr is not 0, else it retrives the data for X, Y, scale and DataPtr again (OLD: mainDrawSub1Sub1)
			flipPoly(params.fileIdx, (int16 *)filesDatabase[params.fileIdx].subData.ptr, params.scale, &newFrame, newBackgroundIncrust->_X, newBackgroundIncrust->_Y, &newX, &newY, &newScale);

			// this function fills the sizeTable for the poly (OLD: mainDrawSub1Sub2)
			getPolySize(newX, newY, newScale, sizeTable, (unsigned char*)newFrame);

			int width = (sizeTable[1] + 2) - (sizeTable[0] - 2) + 1;
			int height = sizeTable[3] - sizeTable[2] + 1;

			backupBackground(newListNode, sizeTable[0] - 2, sizeTable[2], width, height, backgroundPtr);
		}

		addBackgroundIncrustSub1(params.fileIdx, newBackgroundIncrust->_X, newBackgroundIncrust->_Y, NULL, params.scale, (char *)backgroundPtr, (char *)filesDatabase[params.fileIdx].subData.ptr);
	}

	return newListNode;
}

void BackgroundIncrustListNode::regenerateBackgroundIncrustList() {

	lastAni[0] = 0;

	BackgroundIncrustListNode* pl = next;

	while (pl) {
		BackgroundIncrustListNode* pl2 = pl->next;
		BackgroundIncrust *currentBackgroundIncrust = pl->backgroundIncrust;
		int frame = currentBackgroundIncrust->_frame;
		//int screen = pl->backgroundIdx;

		if ((filesDatabase[frame].subData.ptr == NULL) || (strcmp(currentBackgroundIncrust->_name, filesDatabase[frame].subData.name))) {
			frame = NUM_FILE_ENTRIES - 1;
			if (loadFile(currentBackgroundIncrust->_name, frame, currentBackgroundIncrust->_spriteId) < 0)
				frame = -1;
		}

		if (frame >= 0) {
			if (filesDatabase[frame].subData.resourceType == OBJ_TYPE_SPRITE) {
				// Sprite
				int width = filesDatabase[frame].width;
				int height = filesDatabase[frame].height;

				drawSprite(width, height, NULL, filesDatabase[frame].subData.ptr, currentBackgroundIncrust->_Y, currentBackgroundIncrust->_X, backgrounds[currentBackgroundIncrust->_backgroundIdx]._backgroundScreen, filesDatabase[frame].subData.ptrMask);
			} else {
				// Poly
				addBackgroundIncrustSub1(frame, currentBackgroundIncrust->_X, currentBackgroundIncrust->_Y, NULL, currentBackgroundIncrust->_scale, (char*)backgrounds[currentBackgroundIncrust->_backgroundIdx]._backgroundScreen, (char *)filesDatabase[frame].subData.ptr);
			}

			backgrounds[currentBackgroundIncrust->_backgroundIdx]._isChanged = true;
		}

		pl = pl2;
	}

	lastAni[0] = 0;
}

void BackgroundIncrustListNode::freeBackgroundIncrustList() {
	BackgroundIncrustListNode *pCurrent = next;

	while (pCurrent) {
		BackgroundIncrustListNode *pNext = pCurrent->next;
		BackgroundIncrust *currentBackgroundIncrust = pCurrent->backgroundIncrust;

		if (currentBackgroundIncrust->_ptr)
			MemFree(currentBackgroundIncrust->_ptr);
		MemFree(pCurrent);

		pCurrent = pNext;
	}
        
	resetBackgroundIncrustList();
}

void BackgroundIncrustListNode::removeBackgroundIncrustNode(int overlay, int idx) {
	objectParamsQuery params;
	int var_4;
	int var_6;

	BackgroundIncrustListNode *pCurrent;
	BackgroundIncrustListNode *pCurrentHead;
	BackgroundIncrust *currentBacgroundIncrust;

	getMultipleObjectParam(overlay, idx, &params);

	var_4 = params.X;
	var_6 = params.Y;

	pCurrent = next;

	while (pCurrent) {
		currentBacgroundIncrust = pCurrent->backgroundIncrust;
		if ((currentBacgroundIncrust->_overlayIdx == overlay || overlay == -1) && (currentBacgroundIncrust->_objectIdx == idx || idx == -1) && (currentBacgroundIncrust->_X == var_4) && (currentBacgroundIncrust->_Y == var_6)) {
			currentBacgroundIncrust->_type = - 1;
		}

		pCurrent = pCurrent->next;
	}

	pCurrentHead = this;
	pCurrent = next;

	while (pCurrent) {
		currentBacgroundIncrust = pCurrent->backgroundIncrust;
		if (currentBacgroundIncrust->_type == - 1) {
			BackgroundIncrustListNode *pNext = pCurrent->next;
			BackgroundIncrustListNode *bx = pCurrentHead;
			BackgroundIncrustListNode *cx;

			bx->next = pNext;
			cx = pNext;

			if (!pNext) {
				cx = this;
			}

			bx = cx;
			bx->prev = pCurrent->next;

			if (currentBacgroundIncrust->_ptr) {
				MemFree(currentBacgroundIncrust->_ptr);
			}

			MemFree(pCurrent);

			pCurrent = pNext;
		} else {
			pCurrentHead = pCurrent;
			pCurrent = pCurrent->next;
		}
	}
}

void BackgroundIncrustListNode::unmergeBackgroundIncrustList(int ovl, int idx) {
	BackgroundIncrustListNode *pl;
	BackgroundIncrustListNode *pl2;
	BackgroundIncrust *currentBacgroundIncrust;

	objectParamsQuery params;
	getMultipleObjectParam(ovl, idx, &params);

	int x = params.X;
	int y = params.Y;

	pl = this;
	pl2 = pl;
	pl = pl2->next;

	while (pl) {
		pl2 = pl;
                currentBacgroundIncrust = pl->backgroundIncrust;
		if ((currentBacgroundIncrust->_overlayIdx == ovl) || (ovl == -1))
			if ((currentBacgroundIncrust->_objectIdx == idx) || (idx == -1))
				if ((currentBacgroundIncrust->_X == x) && (currentBacgroundIncrust->_Y == y))
					restoreBackground(pl);

		pl = pl2->next;
	}
}

} // End of namespace Cruise
