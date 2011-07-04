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

BackgroundIncrust::BackgroundIncrust() {
	_objectIdx = 0;
	_type = 0;
	_X = 0;
	_Y = 0;
	_scale = 0;
	_frame = 0;
	_backgroundIdx = 0;
	_overlayIdx = 0;
	_scriptNumber = 0;
	_scriptOverlayIdx = 0;

	_spriteId = 0;
	_ptr = NULL;

	_overlayIdx = 0;
	_saveWidth = 0;
	_saveHeight = 0;
	_saveSize = 0;
	_savedX = 0;
	_savedY = 0;
}

BackgroundIncrust::BackgroundIncrust(uint16 objectIdx, int16 type, uint16 overlayIdx, int16 X, int16 Y, uint16 frame,
                                     uint16 scale, uint16 backgroundIdx, uint16 scriptNumber, uint16 scriptOverlayIdx,
                                     uint16 spriteId, char* name) {
	_objectIdx = objectIdx;
	_type = type;
	_X = X;
	_Y = Y;
	_scale = scale;
	_frame = frame;
	_backgroundIdx = backgroundIdx;
	_overlayIdx = overlayIdx;
	_scriptNumber = scriptNumber;
	_scriptOverlayIdx = scriptOverlayIdx;

	_spriteId = spriteId;
	_ptr = NULL;
	strcpy(_name, name);

	_overlayIdx = 0;
	_saveWidth = 0;
	_saveHeight = 0;
	_saveSize = 0;
	_savedX = 0;
	_savedY = 0;
}

BackgroundIncrust::~BackgroundIncrust() {
	if (_ptr)
		MemFree(_ptr);
}

BackgroundIncrustListNode::BackgroundIncrustListNode() {
	next = NULL;
	prev = NULL;
	backgroundIncrust = new BackgroundIncrust;
}

void BackgroundIncrustListNode::resetBackgroundIncrustList() {
	next = NULL;
	prev = NULL;
}

// blit background to another one
void addBackgroundIncrustSub1(int fileIdx, int X, int Y, char *ptr2, int16 scale, char *destBuffer, char *dataPtr) {
	assert((dataPtr != NULL) && (*dataPtr != 0));

	buildPolyModel(X, Y, scale, ptr2, destBuffer, dataPtr);
}

void BackgroundIncrust::backup(int savedX, int savedY, int saveWidth, int saveHeight, uint8* pBackground) {
	_saveWidth = saveWidth;
	_saveHeight = saveHeight;
	_saveSize = saveWidth * saveHeight;
	_savedX = savedX;
	_savedY = savedY;

	_ptr = (uint8 *)MemAlloc(saveWidth * saveHeight);
	for (int i = 0; i < saveHeight; i++) {
		for (int j = 0; j < saveWidth; j++) {
			int xp = j + savedX;
			int yp = i + savedY;

			_ptr[i * saveWidth + j] = ((xp < 0) || (yp < 0) || (xp >= 320) || (yp >= 200)) ?
				0 : pBackground[yp * 320 + xp];
		}
	}
}

void BackgroundIncrust::restore() {
	if (_type != 1)
		return;
	if (_ptr == NULL)
		return;

	uint8* pBackground = backgrounds[_backgroundIdx]._backgroundScreen;
	if (pBackground == NULL)
		return;

	backgrounds[_backgroundIdx]._isChanged = true;

	int X = _savedX;
	int Y = _savedY;
	int width = _saveWidth;
	int height = _saveHeight;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int xp = j + X;
			int yp = i + Y;

			if ((xp >= 0) && (yp >= 0) && (xp < 320) && (yp < 200))
				pBackground[yp * 320 + xp] = _ptr[i * width + j];
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

	newListNode = new BackgroundIncrustListNode;

	if (!newListNode)
		return NULL;

	newListNode->next = currentHead->next;
	currentHead->next = newListNode;

	if (!currentHead2) {
		currentHead2 = this;
	}

	newListNode->prev = currentHead2->prev;
	currentHead2->prev = newListNode;
	newBackgroundIncrust = new BackgroundIncrust(objectIdx, saveBuffer, overlayIdx, params.X, params.Y, params.fileIdx, params.scale,
	                                             backgroundIdx, scriptNumber, scriptOverlay, filesDatabase[params.fileIdx].subData.index,
	                                             filesDatabase[params.fileIdx].subData.name);

	newListNode->backgroundIncrust = newBackgroundIncrust;

	if (filesDatabase[params.fileIdx].subData.resourceType == OBJ_TYPE_SPRITE) {
		// sprite
		int width = filesDatabase[params.fileIdx].width;
		int height = filesDatabase[params.fileIdx].height;
		if (saveBuffer == 1) {
			newListNode->backgroundIncrust->backup(newBackgroundIncrust->_X, newBackgroundIncrust->_Y, width, height, backgroundPtr);
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

			newListNode->backgroundIncrust->backup(sizeTable[0] - 2, sizeTable[2], width, height, backgroundPtr);
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

		delete pCurrent->backgroundIncrust;
		delete pCurrent;

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

			delete pCurrent->backgroundIncrust;
			delete pCurrent;

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
					pl->backgroundIncrust->restore();

		pl = pl2->next;
	}
}

} // End of namespace Cruise
