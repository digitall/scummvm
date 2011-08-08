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
#include "common/list.h"

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

BackgroundIncrust *BackgroundIncrustList::add(int16 overlayIdx, int16 objectIdx, int16 scriptNumber, int16 scriptOverlay, int16 backgroundIdx, int16 saveBuffer) {
	uint8 *backgroundPtr;
	uint8 *ptr;

	objectParamsQuery params;
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

	BackgroundIncrust newBackgroundIncrust;

	newBackgroundIncrust._objectIdx = objectIdx;
	newBackgroundIncrust._type = saveBuffer;
	newBackgroundIncrust._X = params.X;
	newBackgroundIncrust._Y = params.Y;
	newBackgroundIncrust._scale = params.scale;
	newBackgroundIncrust._frame = params.fileIdx;
	newBackgroundIncrust._backgroundIdx = backgroundIdx;
	newBackgroundIncrust._overlayIdx = overlayIdx;
	newBackgroundIncrust._scriptNumber = scriptNumber;
	newBackgroundIncrust._scriptOverlayIdx = scriptOverlay;

	newBackgroundIncrust._spriteId = filesDatabase[params.fileIdx].subData.index;
	newBackgroundIncrust._ptr = NULL;
	strcpy(newBackgroundIncrust._name, filesDatabase[params.fileIdx].subData.name);

	push_back(newBackgroundIncrust);

	Common::List<BackgroundIncrust>::iterator iter = reverse_begin();

	if (filesDatabase[params.fileIdx].subData.resourceType == OBJ_TYPE_SPRITE) {
		// sprite
		int width = filesDatabase[params.fileIdx].width;
		int height = filesDatabase[params.fileIdx].height;
		if (saveBuffer == 1) {
			iter->backup(iter->_X, iter->_Y, width, height, backgroundPtr);
		}

		drawSprite(width, height, NULL, filesDatabase[params.fileIdx].subData.ptr, iter->_Y,
			iter->_X, backgroundPtr, filesDatabase[params.fileIdx].subData.ptrMask);
	} else {
		// poly
		if (saveBuffer == 1) {
			int newX;
			int newY;
			int newScale;
			char *newFrame;

			int sizeTable[4];	// 0 = left, 1 = right, 2 = bottom, 3 = top

			// this function checks if the dataPtr is not 0, else it retrives the data for X, Y, scale and DataPtr again (OLD: mainDrawSub1Sub1)
			flipPoly(params.fileIdx, (int16 *)filesDatabase[params.fileIdx].subData.ptr, params.scale, &newFrame, iter->_X, iter->_Y, &newX, &newY, &newScale);

			// this function fills the sizeTable for the poly (OLD: mainDrawSub1Sub2)
			getPolySize(newX, newY, newScale, sizeTable, (unsigned char*)newFrame);

			int width = (sizeTable[1] + 2) - (sizeTable[0] - 2) + 1;
			int height = sizeTable[3] - sizeTable[2] + 1;

			iter->backup(sizeTable[0] - 2, sizeTable[2], width, height, backgroundPtr);
		}
		addBackgroundIncrustSub1(params.fileIdx, iter->_X, iter->_Y, NULL, params.scale, (char *)backgroundPtr, (char *)filesDatabase[params.fileIdx].subData.ptr);
	}
	return &(*iter);
}

void BackgroundIncrustList::regenerate() {

	lastAni[0] = 0;

	Common::List<BackgroundIncrust>::iterator iter = begin();

	while (iter != end()) {
		int frame = iter->_frame;

		if ((filesDatabase[frame].subData.ptr == NULL) || (strcmp(iter->_name, filesDatabase[frame].subData.name))) {
			frame = NUM_FILE_ENTRIES - 1;
			if (loadFile(iter->_name, frame, iter->_spriteId) < 0)
				frame = -1;
		}

		if (frame >= 0) {
			if (filesDatabase[frame].subData.resourceType == OBJ_TYPE_SPRITE) {
				// Sprite
				int width = filesDatabase[frame].width;
				int height = filesDatabase[frame].height;

				drawSprite(width, height, NULL, filesDatabase[frame].subData.ptr, iter->_Y, iter->_X, backgrounds[iter->_backgroundIdx]._backgroundScreen, filesDatabase[frame].subData.ptrMask);
			} else {
				// Poly
				addBackgroundIncrustSub1(frame, iter->_X, iter->_Y, NULL, iter->_scale, (char*)backgrounds[iter->_backgroundIdx]._backgroundScreen, (char *)filesDatabase[frame].subData.ptr);
			}

			backgrounds[iter->_backgroundIdx]._isChanged = true;
		}

		iter++;
	}

	lastAni[0] = 0;
}

void BackgroundIncrustList::remove(int overlay, int idx) {
	objectParamsQuery params;
	int var_4;
	int var_6;

	getMultipleObjectParam(overlay, idx, &params);

	var_4 = params.X;
	var_6 = params.Y;

	Common::List<BackgroundIncrust>::iterator iter = begin();

	while (iter != end()) {
		if ((iter->_type == -1) || ((iter->_overlayIdx == overlay || overlay == -1) && (iter->_objectIdx == idx || idx == -1) && (iter->_X == var_4) && (iter->_Y == var_6)))
			erase(iter);

		iter++;
	}

}

void BackgroundIncrustList::clear() {
	Common::List<BackgroundIncrust>::clear();
}

void BackgroundIncrustList::unmerge(int ovl, int idx) {
	objectParamsQuery params;
	getMultipleObjectParam(ovl, idx, &params);
	int x = params.X;
	int y = params.Y;

	Common::List<BackgroundIncrust>::iterator iter = begin();

	while (iter != end()) {
		if ((iter->_overlayIdx == ovl) || (ovl == -1))
			if ((iter->_objectIdx == idx) || (idx == -1))
				if ((iter->_X == x) && (iter->_Y == y))
					iter->restore();

			iter++;
	}
}

} // End of namespace Cruise
