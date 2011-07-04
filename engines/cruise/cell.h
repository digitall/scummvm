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

#ifndef CRUISE_CELL_H
#define CRUISE_CELL_H


#include "common/scummsys.h"

namespace Cruise {

struct gfxEntryStruct;

class Cell {
public:
    	int16 _idx;
	int16 _type;
	int16 _overlay;
	int16 _X;
	int16 _fieldC;
	int16 _spriteIdx;
	int16 _color;
	int16 _backgroundPlane;
	int16 _freeze;
	int16 _parent;
	int16 _parentOverlay;
	int16 _parentType;
	int16 _followObjectOverlayIdx;
	int16 _followObjectIdx;
	int16 _animStart;
	int16 _animEnd;
	int16 _animWait;
	int16 _animStep;
	int16 _animChange;
	int16 _animType;
	int16 _animSignal;
	int16 _animCounter;
	int16 _animLoop;
	gfxEntryStruct *_gfxPtr;

        Cell();
        ~Cell();
};

class CellListNode {
public:
	CellListNode *_next;
	CellListNode *_prev;
	Cell *_cell;

	CellListNode();
	void resetPtr();
};




CellListNode *addCell(CellListNode *pHead, int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber, int16 scriptType);
CellListNode *addCell(CellListNode *pHead, int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber);
void createTextObject(CellListNode *pObject, int overlayIdx, int messageIdx, int x, int y, int width, int16 color, int backgroundPlane, int parentOvl, int parentIdx);
void removeCell(CellListNode *objPtr, int ovlNumber, int objectIdx, int objType, int backgroundPlane);
void freezeCell(CellListNode * pObject, int overlayIdx, int objIdx, int objType, int backgroundPlane, int oldFreeze, int newFreeze);
void sortCells(int16 param1, int16 param2, CellListNode *objPtr);
void linkCell(CellListNode *pHead, int ovl, int obj, int type, int ovl2, int obj2);


} // End of namespace Cruise

#endif
