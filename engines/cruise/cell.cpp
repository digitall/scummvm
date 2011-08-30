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
#include "common/file.h"
#include "cruise/cell.h"
#include "common/list.h"
#include "cruise.h"

namespace Cruise {

extern autoCellStruct autoCellHead;

void addAutoCell(int overlayIdx, int idx, int type, int newVal, Cell *pObject) {
	autoCellStruct *pNewEntry;

	pNewEntry = new autoCellStruct;

	pNewEntry->next = autoCellHead.next;
	autoCellHead.next = pNewEntry;

	pNewEntry->ovlIdx = overlayIdx;
	pNewEntry->objIdx = idx;
	pNewEntry->type = type;
	pNewEntry->newValue = newVal;
	pNewEntry->pCell = pObject;
}

void freeAutoCell() {
	autoCellStruct *pCurrent = autoCellHead.next;

	while (pCurrent) {
		autoCellStruct *next = pCurrent->next;

		if (pCurrent->type == 5) {
			objInit(pCurrent->ovlIdx, pCurrent->objIdx, pCurrent->newValue);
		} else {
			setObjectPosition(pCurrent->ovlIdx, pCurrent->objIdx, pCurrent->type, pCurrent->newValue);
		}

		if (pCurrent->pCell->_animWait < 0) {
			objectParamsQuery params;

			getMultipleObjectParam(pCurrent->ovlIdx, pCurrent->objIdx, &params);

			pCurrent->pCell->_animCounter = params.state2 - 1;
		}

		delete pCurrent;

		pCurrent = next;
	}
}

void drawMessage(const gfxEntryStruct *pGfxPtr, int globalX, int globalY, int width, int newColor, uint8 *ouputPtr) {
	// this is used for font only

	if (pGfxPtr) {
		uint8 *initialOuput;
		uint8 *output;
		int xp, yp;
		int x, y;
		const uint8 *ptr = pGfxPtr->imagePtr;
		int height = pGfxPtr->height;

		if (width > 310)
			width = 310;
		if (width + globalX > 319)
			globalX = 319 - width;
		if (globalY < 0)
			globalY = 0;
		if (globalX < 0)
			globalX = 0;

		if (globalY + pGfxPtr->height >= 198) {
			globalY = 198 - pGfxPtr->height;
		}

		gfxModuleData_addDirtyRect(Common::Rect(globalX, globalY, globalX + width, globalY + height));

		initialOuput = ouputPtr + (globalY * 320) + globalX;

		for (yp = 0; yp < height; yp++) {
			output = initialOuput + 320 * yp;
			y = globalY + yp;

			for (xp = 0; xp < pGfxPtr->width; xp++) {
				x = globalX + xp;
				uint8 color = *(ptr++);

				if (color) {
					if ((x >= 0) && (x < 320) && (y >= 0) && (y < 200)) {
						if (color == 1) {
							*output = (uint8) 0;
						} else {
							*output = (uint8) newColor;
						}
					}
				}
				output++;
			}
		}
	}
}

int getValueFromObjectQuerry(objectParamsQuery *params, int idx) {
	switch (idx) {
	case 0:
		return params->X;
	case 1:
		return params->Y;
	case 2:
		return params->baseFileIdx;
	case 3:
		return params->fileIdx;
	case 4:
		return params->scale;
	case 5:
		return params->state;
	case 6:
		return params->state2;
	case 7:
		return params->nbState;
	}

	assert(0);

	return 0;
}

Cell::Cell() {
	_gfxPtr = NULL;

	_freeze = 0;
	_animStart = 0;
	_animEnd = 0;
	_animWait = 0;
	_animSignal = 0;
	_animCounter = 0;
	_animType = 0;
	_animStep = 0;
	_animLoop = 0;
}

Cell::Cell(int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber) {
	_idx = objIdx;
	_type = type;
	_backgroundPlane = backgroundPlane;
	_overlay = overlayIdx;
	_parent = scriptNumber;
	_parentOverlay = scriptOverlay;

	_gfxPtr = NULL;

	_freeze = 0;
	_animStart = 0;
	_animEnd = 0;
	_animWait = 0;
	_animSignal = 0;
	_animCounter = 0;
	_animType = 0;
	_animStep = 0;
	_animLoop = 0;
}

void Cell::remove() {
	if (_gfxPtr)
		freeGfx(_gfxPtr);
	_gfxPtr = NULL;
}

void Cell::setFollower(int16 parentType, int16 followObjectIdx, int16 followObjectOverlayIdx) {
	if (_parentType != -1)
		_parentType = parentType;
	_followObjectIdx = followObjectIdx;
	_followObjectOverlayIdx = followObjectOverlayIdx;
}

void Cell::sync(Common::Serializer &s) {
	uint16 dummyWord = 0;

	s.syncAsUint16LE(dummyWord);
	s.syncAsUint16LE(dummyWord);

	s.syncAsSint16LE(_idx);
	s.syncAsSint16LE(_type);
	s.syncAsSint16LE(_overlay);
	s.syncAsSint16LE(_X);
	s.syncAsSint16LE(_fieldC);
	s.syncAsSint16LE(_spriteIdx);
	s.syncAsSint16LE(_color);
	s.syncAsSint16LE(_backgroundPlane);
	s.syncAsSint16LE(_freeze);
	s.syncAsSint16LE(_parent);
	s.syncAsSint16LE(_parentOverlay);
	s.syncAsSint16LE(_parentType);
	s.syncAsSint16LE(_followObjectOverlayIdx);
	s.syncAsSint16LE(_followObjectIdx);
	s.syncAsSint16LE(_animStart);
	s.syncAsSint16LE(_animEnd);
	s.syncAsSint16LE(_animWait);
	s.syncAsSint16LE(_animStep);
	s.syncAsSint16LE(_animChange);
	s.syncAsSint16LE(_animType);
	s.syncAsSint16LE(_animSignal);
	s.syncAsSint16LE(_animCounter);
	s.syncAsSint16LE(_animLoop);
	s.syncAsUint16LE(dummyWord);
}

/*Cell::~Cell() {
    if (_gfxPtr)
        freeGfx(_gfxPtr);
    _gfxPtr = NULL;
}*/

void Cell::freeze(int oldFreeze, int newFreeze) {
	if ((_freeze == oldFreeze) || (oldFreeze == -1))
		_freeze = newFreeze;
}

void Cell::setAnim(int16 signal, int16 loop, int16 wait, int16 animStep, int16 end, int16 start, int16 type, int16 change) {
	_animSignal = signal;
	_animLoop = loop;
	_animWait = wait;
	_animStep = animStep;
	_animEnd = end;
	_animStart = start;
	_animType = type;
	_animChange = change;
}

void Cell::animate(objectParamsQuery params) {
	if (_animStep && (_animCounter <= 0)) {

		bool change = true;

		int newVal = getValueFromObjectQuerry(&params, _animChange) + _animStep;

		if (_animStep > 0) {
			if (newVal > _animEnd) {
				if (_animLoop) {
					newVal = _animStart;
					if (_animLoop > 0)
						_animLoop--;
				} else {
					int16 data2;
					data2 = _animStart;

					change = false;
					_animStep = 0;

					if (_animType) {  // should we resume the script ?
						if (_parentType == 20) {
							_vm->procScriptList.changeParam(_parentOverlay, _parent, -1, 0);
						} else if (_parentType == 30) {
							_vm->relScriptList.changeParam(_parentOverlay, _parent, -1, 0);
						}
					}
				}
			}
		} else {
			if (newVal < _animEnd) {
				if (_animLoop) {
					newVal = _animStart;
					if (_animLoop > 0)
						_animLoop--;
				} else {
					int16 data2;
					data2 = _animStart;

					change = false;
					_animStep = 0;

					if (_animType) {  // should we resume the script ?
						if (_parentType == 20) {
							_vm->procScriptList.changeParam(_parentOverlay, _parent, -1, 0);
						} else if (_parentType == 30) {
							_vm->relScriptList.changeParam(_parentOverlay, _parent, -1, 0);
						}
					}
				}
			}
		}

		if (_animWait >= 0) {
			_animCounter = _animWait;
		}

		if ((_animSignal >= 0) && (_animSignal == newVal) && (_animType != 0)) {
			if (_parentType == 20) {
				_vm->procScriptList.changeParam(_parentOverlay, _parent, -1, 0);
			} else if (_parentType == 30) {
				_vm->relScriptList.changeParam(_parentOverlay, _parent, -1, 0);
			}

			_animType = 0;
		}

		if (change)
			addAutoCell(_overlay, _idx, _animChange, newVal, this);
	} else
		_animCounter--;
}

void Cell::drawAsMessage() {
	drawMessage(_gfxPtr, _X, _fieldC, _spriteIdx, _color, gfxModuleData.pPage10);
}

void freeMessageList(CellList *objPtr) {
	/*  if (objPtr) {
	         if (objPtr->next)
	         MemFree(objPtr->next);

	        MemFree(objPtr);
	    } */
}

CellList::CellList(Common::List<Cell>::iterator firstElement,   Common::List<Cell>::iterator lastElement) {
	insert(begin(), firstElement, lastElement);
}

Cell *CellList::add(int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber) {

	int16 var;

	getSingleObjectParam(overlayIdx, objIdx, 2, &var);

	Common::List<Cell>::iterator iter = begin();
	while (iter != end() && (iter->_type != 3)) {

		if (iter->_type != 5) {
			int16 lvar2;

			if (getSingleObjectParam(iter->_overlay, iter->_idx, 2, &lvar2) >= 0 && lvar2 >= var)
				break;
		}
		iter++;
	}

	if (iter != end()) {
		if ((iter->_overlay == overlayIdx) &&
		        (iter->_backgroundPlane == backgroundPlane) &&
		        (iter->_idx == objIdx) &&
		        (iter->_type == type))

			return NULL;
	}

	Cell *pNewCell = new Cell(overlayIdx, objIdx, type, backgroundPlane, scriptOverlay, scriptNumber);

	insert(iter, *pNewCell);

	Cell *re  = &(*(--iter));   //since insert add element before the iter.
	return re;
}

Cell *CellList::add(int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber, int16 scriptType) {
	int16 var;
	Cell *pNewCell;

	if (getSingleObjectParam(overlayIdx, objIdx, 2, &var) < 0) {
		return 0;
	}

	pNewCell = add(overlayIdx, objIdx, type, backgroundPlane, scriptOverlay, scriptNumber);
	if (!pNewCell)
		return 0;
	pNewCell->setFollower(scriptType, objIdx, overlayIdx);

	return pNewCell;
}

void Cell::makeTextObject(int x, int y, int width, int16 color, const char *pText) {
	_X = x;
	_fieldC = y;
	_spriteIdx = width;
	_color = color;

	if (pText)
		_gfxPtr = renderText(width, pText);
}

void CellList::createTextObject(int overlayIdx, int messageIdx, int x, int y, int width, int16 color, int backgroundPlane, int parentOvl, int parentIdx) {

	const char *pText;
	Cell *pNewCell;

	pText = getText(messageIdx, overlayIdx);

	pNewCell = add(overlayIdx, messageIdx, OBJ_TYPE_MESSAGE, backgroundPlane, parentOvl, parentIdx);
	pNewCell->makeTextObject(x, y, width, color, pText);

	// WORKAROUND: This is needed for the new dirty rect handling so as to properly refresh the screen
	// when the copy protection screen is being shown
	if ((messageIdx == 0) && !strcmp(overlayTable[overlayIdx].overlayName, "XX2"))
		backgrounds[0]._isChanged = true;
}

void CellList::remove(int ovlNumber, int objectIdx, int objType, int backgroundPlane) {

	Common::List<Cell>::iterator iter = begin();
	while (iter != end()) {
		if ((iter->_type == -1) ||
		        (((iter->_overlay == ovlNumber) || (ovlNumber == -1)) &&
		         ((iter->_idx == objectIdx) || (objectIdx == -1)) &&
		         ((iter->_type == objType) || (objType == -1)) &&
		         ((iter->_backgroundPlane == backgroundPlane) || (backgroundPlane == -1)))) {
			iter->remove();
			iter = erase(iter);
		} else
			iter++;
	}
}

void CellList::clear() {
	remove(-1, -1, -1, -1); // since original clear does not free _gfxPtr
}

void CellList::linkCell(int ovl, int obj, int type, int ovl2, int obj2) {

	Common::List<Cell>::iterator iter = begin();
	while (iter != end()) {
		if (((iter->_overlay == ovl) || (ovl == -1)) &&
		        ((iter->_idx == obj) || (obj == -1)) &&
		        ((iter->_type == type) || (type == -1))) {
			iter->setFollower(-1, obj2, ovl2);
		}
		iter++;
	}
}

void CellList::freezeCell(int overlayIdx, int objIdx, int objType, int backgroundPlane, int oldFreeze, int newFreeze) {

	Common::List<Cell>::iterator iter = begin();

	while (iter != end()) {
		if (((iter->_overlay == overlayIdx) || (overlayIdx == -1)) &&
		        ((iter->_idx == objIdx) || (objIdx == -1)) &&
		        ((iter->_type == objType) || (objType == -1)) &&
		        ((iter->_backgroundPlane == backgroundPlane) || (backgroundPlane == -1))) {
			iter->freeze(oldFreeze, newFreeze);
		}
		iter++;
	}
}

void CellList::sort(int16 ovlIdx, int16 objIdx) {
	int16 newz, objz, sobjz;

	Common::List<Cell>::iterator iter = begin();
	Common::List<Cell>::iterator insertPos = end();

	CellList temp;
	getSingleObjectParam(ovlIdx, objIdx, 2, &sobjz);
	while (iter != end()) {
		if ((iter->_overlay == ovlIdx) && (iter->_idx == objIdx)) { // found
			temp.push_front(*iter);
			iter = erase(iter);
		} else {
			if (iter->_type == 5)
				newz = 32000;
			else {
				getSingleObjectParam(iter->_overlay, iter->_idx, 2, &objz);
				newz = objz;
			}
			if (newz < sobjz) {
				insertPos = iter;
			}
			iter++;
		}
	}

	if (temp.size()) {
		if (insertPos == end()) {    //the before is empty.
			insert(begin(), temp.begin(), temp.end());
		} else {
			insertPos++;    //to add after insertPos not before.
			insert(insertPos, temp.begin(), temp.end());
		}
	}
}

void CellList::syncCells(Common::Serializer &s) {
	int chunkCount = 0;
	Cell *p;

	if (s.isSaving()) {
		// Figure out the number of chunks to save
		chunkCount = size();
	}
	s.syncAsSint16LE(chunkCount);

	Common::List<Cell>::iterator iter = begin();
	for (int i = 0; i < chunkCount; ++i) {
		p = s.isSaving() ? &(*iter) : new Cell;
		p->sync(s);
		if (s.isLoading())
			push_back(*p);
		iter++;
	}
}

void clearMaskBit(int x, int y, unsigned char *pData, int stride) {
	unsigned char *ptr = y * stride + x / 8 + pData;

	unsigned char bitToTest = 0x80 >> (x & 7);

	*(ptr) &= ~bitToTest;
}

void drawMask(unsigned char *workBuf, int wbWidth, int wbHeight, unsigned char *pMask, int maskWidth, int maskHeight, int maskX, int maskY) {
	for (int y = 0; y < maskHeight; y++) {
		for (int x = 0; x < maskWidth * 8; x++) {
			if (testMask(x, y, pMask, maskWidth)) {
				int destX = maskX + x;
				int destY = maskY + y;

				if ((destX >= 0) && (destX < wbWidth * 8) && (destY >= 0) && (destY < wbHeight))
					clearMaskBit(destX, destY, workBuf, wbWidth);
			}
		}
	}
}

void CellList::processMask(unsigned char *workBuf, int width, int height, int xs, int ys) {

	Common::List<Cell>::iterator iter = begin();

	while (iter != end()) {
		if (iter->_type == OBJ_TYPE_BGMASK && iter->_freeze == 0) {
			objectParamsQuery params;

			getMultipleObjectParam(iter->_overlay, iter->_idx, &params);

			int maskX = params.X;
			int maskY = params.Y;
			int maskFrame = params.fileIdx;

			if (filesDatabase[maskFrame].subData.resourceType == OBJ_TYPE_BGMASK && filesDatabase[maskFrame].subData.ptrMask)
				drawMask(workBuf, width, height, filesDatabase[maskFrame].subData.ptrMask, filesDatabase[maskFrame].width / 8, filesDatabase[maskFrame].height, maskX - xs, maskY - ys);
			else if (filesDatabase[maskFrame].subData.resourceType == OBJ_TYPE_SPRITE && filesDatabase[maskFrame].subData.ptrMask)
				drawMask(workBuf, width, height, filesDatabase[maskFrame].subData.ptrMask, filesDatabase[maskFrame].width / 8, filesDatabase[maskFrame].height, maskX - xs, maskY - ys);

		}
		iter++;
	}
}

int16 CellList::drawMessages() {
	Common::List<Cell>::iterator iter = begin();
	int16 messageDrawn = 0;
	while (iter != _vm->cellList.end()) {
		if (iter->_type == OBJ_TYPE_MESSAGE && iter->_freeze == 0) {
			iter->drawAsMessage();
			messageDrawn = 1;
		}
		iter++;
	}
	return messageDrawn;
}

} // End of namespace Cruise
