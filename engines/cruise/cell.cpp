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

namespace Cruise {

Cell::Cell(){
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

/*Cell::~Cell() {
	if (_gfxPtr)
		freeGfx(_gfxPtr);
	_gfxPtr = NULL;
}*/

void freeMessageList(CellList *objPtr) {
	/*	if (objPtr) {
			 if (objPtr->next)
			 MemFree(objPtr->next);

			MemFree(objPtr);
		} */
}

Cell *CellList::add(int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber) {

	int16 var;

	Cell newCell;

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

	newCell._idx = objIdx;
	newCell._type = type;
	newCell._backgroundPlane = backgroundPlane;
	newCell._overlay = overlayIdx;
	newCell._parent = scriptNumber;
	newCell._parentOverlay = scriptOverlay;

	insert(iter,newCell);

	Cell *re  = &(*(--iter));
	return re; 
}

Cell *CellList::add(int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber, int16 scriptType) {
	int16 var;
        Cell *pNewCell;

        if (getSingleObjectParam(overlayIdx, objIdx, 2, &var) < 0) {
            return 0;
        }

        pNewCell = add(overlayIdx, objIdx, type, backgroundPlane, scriptOverlay, scriptNumber);
        if(!pNewCell)
            return 0;

        pNewCell->_parentType = scriptType;
        pNewCell->_followObjectIdx = objIdx;
        pNewCell->_followObjectOverlayIdx = overlayIdx;
        return pNewCell;
}


void CellList::createTextObject(int overlayIdx, int messageIdx, int x, int y, int width, int16 color, int backgroundPlane, int parentOvl, int parentIdx) {

	const char *ax;
	Cell *pNewCell;

	pNewCell = add(overlayIdx, messageIdx, OBJ_TYPE_MESSAGE, backgroundPlane, parentOvl, parentIdx);
	pNewCell->_X = x;
	pNewCell->_fieldC = y;
	pNewCell->_spriteIdx = width;
	pNewCell->_color = color;

	ax = getText(messageIdx, overlayIdx);

	if (ax) {
		pNewCell->_gfxPtr = renderText(width, ax);
	}

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
			if(iter->_gfxPtr)
				freeGfx(iter->_gfxPtr);
			iter = erase(iter);
		} else
			iter++;
	}
}

void CellList::clear() {
	remove(-1,-1,-1,-1); // since clear does not free _gfxPtr
}

void CellList::linkCell(int ovl, int obj, int type, int ovl2, int obj2) {

	Common::List<Cell>::iterator iter = begin();
	while (iter != end()) {
		if (((iter->_overlay == ovl) || (ovl == -1)) &&
				((iter->_idx == obj) || (obj == -1)) &&
				((iter->_type == type) || (type == -1))) {
			iter->_followObjectIdx = obj2;
			iter->_followObjectOverlayIdx = ovl2;
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
				((iter->_backgroundPlane == backgroundPlane) || (backgroundPlane == -1)) &&
				((iter->_freeze == oldFreeze) || (oldFreeze == -1))) {
			iter->_freeze = newFreeze;
		}
		iter++;
	}
}

void printCellZ(int a) {
	Common::List<Cell>::iterator iter = cellHead.begin();
	int i = 1;
	int16 objz;
	while(iter != cellHead.end()) {
		getSingleObjectParam(iter->_overlay, iter->_idx, 2, &objz);
		if (a)
			debug("\t\t\t\t number = %d, z = %d", i, objz);
		else
			debug("number = %d, z = %d", i, objz);
		iter++;
		i++;
	}

}

void CellList::sort(int16 ovlIdx, int16 objIdx) {
	int16 newz, objz, sobjz;
	
	Common::List<Cell>::iterator iter = begin();
	Common::List<Cell>::iterator insertPos = end();

	CellList temp;
	getSingleObjectParam(ovlIdx, objIdx, 2, &sobjz);
	while(iter != end()) {
		if ((iter->_overlay == ovlIdx) && (iter->_idx == objIdx)) {	// found
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

	if(temp.size()) {
		if ( insertPos == end() ) {	//the before is empty.
			insert(begin(),temp.begin(), temp.end());
		} else {
			insertPos++;	//to add after insertPos not before.
			insert(insertPos, temp.begin(), temp.end());
		}
	}
}

Common::List<Cell>::iterator CellList::begin() {

	return Common::List<Cell>::begin();
}

Common::List<Cell>::iterator CellList::end() {

	return Common::List<Cell>::end();
}

Common::List<Cell>::iterator CellList::reverse_begin() {

	return Common::List<Cell>::reverse_begin();
}

} // End of namespace Cruise
