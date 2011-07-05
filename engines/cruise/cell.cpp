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

Cell::~Cell() {
        if (_gfxPtr)
	    freeGfx(_gfxPtr);
	_gfxPtr = NULL;
}

CellListNode::CellListNode(){
        _next = NULL;
        _prev = NULL;
        _cell = new Cell;

}

void CellListNode::resetPtr() {
	_next = NULL;
	_prev = NULL;
}

void freeMessageList(CellListNode *objPtr) {
	/*	if (objPtr) {
			 if (objPtr->next)
			 MemFree(objPtr->next);

			MemFree(objPtr);
		} */
}

CellListNode *CellListNode::addCell(int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber) {
	int16 var;

	CellListNode *newElement;
	CellListNode *currentHead = this;
	CellListNode *currentHead2;
	CellListNode *currentHead3;

	currentHead3 = currentHead;
	currentHead2 = currentHead->_next;

	getSingleObjectParam(overlayIdx, objIdx, 2, &var);
        
	while (currentHead2 && (currentHead2->_cell->_type != 3)) {

		if (currentHead2->_cell->_type != 5) {
			int16 lvar2;

			if (getSingleObjectParam(currentHead2->_cell->_overlay, currentHead2->_cell->_idx, 2, &lvar2) >= 0 && lvar2 >= var)
				break;
		}

		currentHead3 = currentHead2;
		currentHead2 = currentHead2->_next;
	}

	if (currentHead2) {
		if ((currentHead2->_cell->_overlay == overlayIdx) &&
		        (currentHead2->_cell->_backgroundPlane == backgroundPlane) &&
		        (currentHead2->_cell->_idx == objIdx) &&
		        (currentHead2->_cell->_type == type))

			return NULL;
	}

	currentHead = currentHead2;

	newElement = new CellListNode;

	if (!newElement)
		return 0;

	newElement->_next = currentHead3->_next;
	currentHead3->_next = newElement;

	newElement->_cell->_idx = objIdx;
	newElement->_cell->_type = type;
	newElement->_cell->_backgroundPlane = backgroundPlane;
	newElement->_cell->_overlay = overlayIdx;
	newElement->_cell->_parent = scriptNumber;
	newElement->_cell->_parentOverlay = scriptOverlay;

	if (currentHead) {
		newElement->_prev = currentHead->_prev;
		currentHead->_prev = newElement;
	} else {
		newElement->_prev = _prev;
		_prev = newElement;
	}

	return newElement;
}

CellListNode *CellListNode::addCell(int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber, int16 scriptType) {
	int16 var;
        CellListNode *newCellListNode;

        if (getSingleObjectParam(overlayIdx, objIdx, 2, &var) < 0) {
            return 0;
        }

        newCellListNode = addCell(overlayIdx, objIdx, type, backgroundPlane, scriptOverlay, scriptNumber);
        if(!newCellListNode)
            return 0;

        newCellListNode->_cell->_parentType = scriptType;
        newCellListNode->_cell->_followObjectIdx = objIdx;
        newCellListNode->_cell->_followObjectOverlayIdx = overlayIdx;
        return newCellListNode;
}


void CellListNode::createTextObject(int overlayIdx, int messageIdx, int x, int y, int width, int16 color, int backgroundPlane, int parentOvl, int parentIdx) {

	const char *ax;
        CellListNode *pNewElement;

        pNewElement = addCell(overlayIdx, messageIdx, OBJ_TYPE_MESSAGE, backgroundPlane, parentOvl, parentIdx);
        pNewElement->_cell->_X = x;
        pNewElement->_cell->_fieldC = y;
        pNewElement->_cell->_spriteIdx = width;
        pNewElement->_cell->_color = color;
        
	ax = getText(messageIdx, overlayIdx);

	if (ax) {
		pNewElement->_cell->_gfxPtr = renderText(width, ax);
	}

	// WORKAROUND: This is needed for the new dirty rect handling so as to properly refresh the screen
	// when the copy protection screen is being shown
	if ((messageIdx == 0) && !strcmp(overlayTable[overlayIdx].overlayName, "XX2"))
		backgrounds[0]._isChanged = true;
}

void CellListNode::removeCell(int ovlNumber, int objectIdx, int objType, int backgroundPlane) {
	CellListNode *currentObj = _next;
	CellListNode *previous;

	while (currentObj) {
		if (((currentObj->_cell->_overlay == ovlNumber) || (ovlNumber == -1)) &&
		        ((currentObj->_cell->_idx == objectIdx) || (objectIdx == -1)) &&
		        ((currentObj->_cell->_type == objType) || (objType == -1)) &&
		        ((currentObj->_cell->_backgroundPlane == backgroundPlane) || (backgroundPlane == -1))) {
			currentObj->_cell->_type = -1;
		}

		currentObj = currentObj->_next;
	}

	previous = this;
	currentObj = _next;

	while (currentObj) {
		CellListNode *si;

		si = currentObj;

		if (si->_cell->_type == -1) {
			CellListNode *dx;
			previous->_next = si->_next;

			dx = si->_next;

			if (!si->_next) {
				dx = this;
			}

			dx->_prev = si->_prev;

			// Free the entry
			delete si->_cell;
			MemFree(si);

			currentObj = dx;
		} else {
			currentObj = si->_next;
			previous = si;
		}
	}
}

void linkCell(CellListNode *pHead, int ovl, int obj, int type, int ovl2, int obj2) {
	while (pHead) {
		if ((pHead->_cell->_overlay == ovl) || (ovl == -1)) {
			if ((pHead->_cell->_idx == obj) || (obj == -1)) {
				if ((pHead->_cell->_type == type) || (type == -1)) {
					pHead->_cell->_followObjectIdx = obj2;
					pHead->_cell->_followObjectOverlayIdx = ovl2;
				}
			}
		}

		pHead = pHead->_next;
	}
}

void CellListNode::freezeCell(int overlayIdx, int objIdx, int objType, int backgroundPlane, int oldFreeze, int newFreeze) {
	CellListNode *currentNode = this;
	while (currentNode) {
		if ((currentNode->_cell->_overlay == overlayIdx) || (overlayIdx == -1)) {
			if ((currentNode->_cell->_idx == objIdx) || (objIdx == -1)) {
				if ((currentNode->_cell->_type == objType) || (objType == -1)) {
					if ((currentNode->_cell->_backgroundPlane == backgroundPlane) || (backgroundPlane == -1)) {
						if ((currentNode->_cell->_freeze == oldFreeze) || (oldFreeze == -1)) {
							currentNode->_cell->_freeze = newFreeze;
						}
					}
				}
			}
		}

		currentNode = currentNode->_next;
	}
}

void sortCells(int16 ovlIdx, int16 ovjIdx, CellListNode *objPtr) {
	CellListNode *pl, *pl2, *pl3, *pl4, *plz, *pllast;
	CellListNode prov;
	int16 newz, objz, sobjz;

	pl4 = NULL;

	getSingleObjectParam(ovlIdx, ovjIdx, 2, &sobjz);
	pl = objPtr;
	prov._next = NULL;
	prov._prev = NULL;

	pl2 = pl->_next;
	pllast = NULL;
	plz = objPtr;

	while (pl2) {
		pl3 = pl2->_next;
		if ((pl2->_cell->_overlay == ovlIdx) && (pl2->_cell->_idx == ovjIdx)) {// found
			pl->_next = pl3;

			if (pl3) {
				pl3->_prev = pl2->_prev;
			} else {
				objPtr->_prev = pl2->_prev;
			}

			pl4 = prov._next;

			if (pl4) {
				pl4->_prev = pl2;
			} else {
				prov._prev = pl2;
			}

			pl2->_prev = NULL;
			pl2->_next = prov._next;
			prov._next = pl2;

			if (pllast == NULL) {
				pllast = pl2;
			}
		} else {
			if (pl2->_cell->_type == 5) {
				newz = 32000;
			} else {
				getSingleObjectParam(pl2->_cell->_overlay, pl2->_cell->_idx, 2, &objz);
				newz = objz;
			}

			if (newz < sobjz) {
				plz = pl2;
			}

			pl = pl->_next;
		}

		pl2 = pl3;
	}

	if (pllast) {
		pl2 = prov._next;
		pl4 = plz->_next;
		plz->_next = pl2;
		pllast->_next = pl4;

		if (plz != objPtr)
			pl2->_prev = plz;
		if (!pl4)
			objPtr->_prev = pllast;
		else
			pl4->_prev = pllast;
	}
}

} // End of namespace Cruise
