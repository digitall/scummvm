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

CellListNode cellHead;

void resetPtr(CellListNode *ptr) {
	ptr->next = NULL;
	ptr->prev = NULL;
}

void freeMessageList(CellListNode *objPtr) {
	/*	if (objPtr) {
			 if (objPtr->next)
			 MemFree(objPtr->next);

			MemFree(objPtr);
		} */
}

CellListNode *addCell(CellListNode *pHead, int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber, int16 scriptType) {
	int16 var;

	CellListNode *newElement;
	CellListNode *currentHead = pHead;
	CellListNode *currentHead2;
	CellListNode *currentHead3;

	if (getSingleObjectParam(overlayIdx, objIdx, 2, &var) < 0) {
		return 0;
	}

	currentHead3 = currentHead;
	currentHead2 = currentHead->next;

	while (currentHead2 && (currentHead2->type != 3)) {

		if (currentHead2->type != 5) {
			int16 lvar2;

			if (getSingleObjectParam(currentHead2->overlay, currentHead2->idx, 2, &lvar2) >= 0 && lvar2 >= var)
				break;
		}

		currentHead3 = currentHead2;
		currentHead2 = currentHead2->next;
	}

	if (currentHead2) {
		if ((currentHead2->overlay == overlayIdx) &&
		        (currentHead2->backgroundPlane == backgroundPlane) &&
		        (currentHead2->idx == objIdx) &&
		        (currentHead2->type == type))

			return NULL;
	}

	currentHead = currentHead2;

	newElement = (CellListNode *) mallocAndZero(sizeof(CellListNode));

	if (!newElement)
		return 0;

	newElement->next = currentHead3->next;
	currentHead3->next = newElement;

	newElement->idx = objIdx;
	newElement->type = type;
	newElement->backgroundPlane = backgroundPlane;
	newElement->overlay = overlayIdx;
	newElement->freeze = 0;
	newElement->parent = scriptNumber;
	newElement->parentOverlay = scriptOverlay;
	newElement->gfxPtr = NULL;
	newElement->followObjectIdx = objIdx;
	newElement->followObjectOverlayIdx = overlayIdx;
	newElement->parentType = scriptType;

	newElement->animStart = 0;
	newElement->animEnd = 0;
	newElement->animWait = 0;
	newElement->animSignal = 0;
	newElement->animCounter = 0;
	newElement->animType = 0;
	newElement->animStep = 0;
	newElement->animLoop = 0;

	if (currentHead) {
		newElement->prev = currentHead->prev;
		currentHead->prev = newElement;
	} else {
		newElement->prev = pHead->prev;
		pHead->prev = newElement;
	}

	return newElement;
}

void createTextObject(CellListNode *pObject, int overlayIdx, int messageIdx, int x, int y, int width, int16 color, int backgroundPlane, int parentOvl, int parentIdx) {

	const char *ax;
	CellListNode *savePObject = pObject;
	CellListNode *cx;

	CellListNode *pNewElement;
	CellListNode *si = pObject->next;
	CellListNode *var_2;

	while (si) {
		pObject = si;
		si = si->next;
	}

	var_2 = si;

	pNewElement = (CellListNode *) MemAlloc(sizeof(CellListNode));
	memset(pNewElement, 0, sizeof(CellListNode));

	pNewElement->next = pObject->next;
	pObject->next = pNewElement;

	pNewElement->idx = messageIdx;
	pNewElement->type = OBJ_TYPE_MESSAGE;
	pNewElement->backgroundPlane = backgroundPlane;
	pNewElement->overlay = overlayIdx;
	pNewElement->x = x;
	pNewElement->field_C = y;
	pNewElement->spriteIdx = width;
	pNewElement->color = color;
	pNewElement->freeze = 0;
	pNewElement->parent = parentIdx;
	pNewElement->parentOverlay = parentOvl;
	pNewElement->gfxPtr = NULL;

	if (var_2) {
		cx = var_2;
	} else {
		cx = savePObject;
	}

	pNewElement->prev = cx->prev;
	cx->prev = pNewElement;

	ax = getText(messageIdx, overlayIdx);

	if (ax) {
		pNewElement->gfxPtr = renderText(width, ax);
	}

	// WORKAROUND: This is needed for the new dirty rect handling so as to properly refresh the screen
	// when the copy protection screen is being shown
	if ((messageIdx == 0) && !strcmp(overlayTable[overlayIdx].overlayName, "XX2"))
		backgrounds[0]._isChanged = true;
}

void removeCell(CellListNode *objPtr, int ovlNumber, int objectIdx, int objType, int backgroundPlane) {
	CellListNode *currentObj = objPtr->next;
	CellListNode *previous;

	while (currentObj) {
		if (((currentObj->overlay == ovlNumber) || (ovlNumber == -1)) &&
		        ((currentObj->idx == objectIdx) || (objectIdx == -1)) &&
		        ((currentObj->type == objType) || (objType == -1)) &&
		        ((currentObj->backgroundPlane == backgroundPlane) || (backgroundPlane == -1))) {
			currentObj->type = -1;
		}

		currentObj = currentObj->next;
	}

	previous = objPtr;
	currentObj = objPtr->next;

	while (currentObj) {
		CellListNode *si;

		si = currentObj;

		if (si->type == -1) {
			CellListNode *dx;
			previous->next = si->next;

			dx = si->next;

			if (!si->next) {
				dx = objPtr;
			}

			dx->prev = si->prev;

			// Free the entry
			if (si->gfxPtr)
				freeGfx(si->gfxPtr);
			MemFree(si);

			currentObj = dx;
		} else {
			currentObj = si->next;
			previous = si;
		}
	}
}

void linkCell(CellListNode *pHead, int ovl, int obj, int type, int ovl2, int obj2) {
	while (pHead) {
		if ((pHead->overlay == ovl) || (ovl == -1)) {
			if ((pHead->idx == obj) || (obj == -1)) {
				if ((pHead->type == type) || (type == -1)) {
					pHead->followObjectIdx = obj2;
					pHead->followObjectOverlayIdx = ovl2;
				}
			}
		}

		pHead = pHead->next;
	}
}

void freezeCell(CellListNode * pObject, int overlayIdx, int objIdx, int objType, int backgroundPlane, int oldFreeze, int newFreeze) {
	while (pObject) {
		if ((pObject->overlay == overlayIdx) || (overlayIdx == -1)) {
			if ((pObject->idx == objIdx) || (objIdx == -1)) {
				if ((pObject->type == objType) || (objType == -1)) {
					if ((pObject->backgroundPlane == backgroundPlane) || (backgroundPlane == -1)) {
						if ((pObject->freeze == oldFreeze) || (oldFreeze == -1)) {
							pObject->freeze = newFreeze;
						}
					}
				}
			}
		}

		pObject = pObject->next;
	}
}

void sortCells(int16 ovlIdx, int16 ovjIdx, CellListNode *objPtr) {
	CellListNode *pl, *pl2, *pl3, *pl4, *plz, *pllast;
	CellListNode prov;
	int16 newz, objz, sobjz;

	pl4 = NULL;

	getSingleObjectParam(ovlIdx, ovjIdx, 2, &sobjz);
	pl = objPtr;
	prov.next = NULL;
	prov.prev = NULL;

	pl2 = pl->next;
	pllast = NULL;
	plz = objPtr;

	while (pl2) {
		pl3 = pl2->next;
		if ((pl2->overlay == ovlIdx) && (pl2->idx == ovjIdx)) {// found
			pl->next = pl3;

			if (pl3) {
				pl3->prev = pl2->prev;
			} else {
				objPtr->prev = pl2->prev;
			}

			pl4 = prov.next;

			if (pl4) {
				pl4->prev = pl2;
			} else {
				prov.prev = pl2;
			}

			pl2->prev = NULL;
			pl2->next = prov.next;
			prov.next = pl2;

			if (pllast == NULL) {
				pllast = pl2;
			}
		} else {
			if (pl2->type == 5) {
				newz = 32000;
			} else {
				getSingleObjectParam(pl2->overlay, pl2->idx, 2, &objz);
				newz = objz;
			}

			if (newz < sobjz) {
				plz = pl2;
			}

			pl = pl->next;
		}

		pl2 = pl3;
	}

	if (pllast) {
		pl2 = prov.next;
		pl4 = plz->next;
		plz->next = pl2;
		pllast->next = pl4;

		if (plz != objPtr)
			pl2->prev = plz;
		if (!pl4)
			objPtr->prev = pllast;
		else
			pl4->prev = pllast;
	}
}

} // End of namespace Cruise
