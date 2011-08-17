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
#include "common/serializer.h"
#include "cruise/object.h"

namespace Cruise {

struct gfxEntryStruct;

class Cell {
public:
	int16 _idx;
	int16 _type;
	int16 _overlay;
	int16 _backgroundPlane;

	int16 _animWait;					//used in cell.cpp
	int16 _animCounter;

	int16 _freeze;						//used in cruise_main

	int16 _animChange;					//used in function
private:
	int16 _X;
	int16 _fieldC;
	int16 _spriteIdx;
	int16 _color;
	int16 _parent;
	int16 _parentOverlay;
	int16 _parentType;
	int16 _followObjectIdx;
	int16 _followObjectOverlayIdx;
	int16 _animStart;
	int16 _animEnd;

	int16 _animType;
	int16 _animSignal;
	int16 _animStep;
	int16 _animLoop;
	gfxEntryStruct *_gfxPtr;
public:
	Cell();
	Cell(int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber);
	void remove();

	int16 getFollowObjectOverlayIdx() { return _followObjectOverlayIdx;}
	int16 getFollowObjectIdx() {return _followObjectIdx;}

	void setAnim(int16 signal, int16 loop, int16 wait, int16 animStep, int16 end, int16 start, int16 type, int16 change);
	void animate(objectParamsQuery params);
	void setFollower(int16 parentType, int16 followObjectIdx, int16 followObjectOverlayIdx);
	void makeTextObject(int x, int y, int width, int16 color,const char *pText);
	void sync(Common::Serializer &s);
	void freeze(int oldFreeze, int newFreeze);
	void drawAsMessage();
};

struct autoCellStruct {
	struct autoCellStruct *next;
	short int ovlIdx;
	short int objIdx;
	short int type;
	short int newValue;
	Cell *pCell;
};

class CellList: private Common::List<Cell> {
public:

	CellList() {}
	CellList(Common::List<Cell>::iterator firstElement,	Common::List<Cell>::iterator lastElement);

	Common::List<Cell>::iterator begin() { return Common::List<Cell>::begin(); }
	Common::List<Cell>::iterator end() { return Common::List<Cell>::end(); }
	Common::List<Cell>::iterator reverse_begin() { return Common::List<Cell>::reverse_begin(); }

	Cell *add(int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber, int16 scriptType);
	Cell *add(int16 overlayIdx, int16 objIdx, int16 type, int16 backgroundPlane, int16 scriptOverlay, int16 scriptNumber);

	void remove(int ovlNumber, int objectIdx, int objType, int backgroundPlane);
	void clear();

	void createTextObject(int overlayIdx, int messageIdx, int x, int y, int width, int16 color, int backgroundPlane, int parentOvl, int parentIdx);
	void linkCell(int ovl, int obj, int type, int ovl2, int obj2);
	void freezeCell(int overlayIdx, int objIdx, int objType, int backgroundPlane, int oldFreeze, int newFreeze);
	void sort(int16 overlayIdx, int16 objIdx);

	void syncCells(Common::Serializer &s);
	void processMask(unsigned char *workBuf, int width, int height, int xs, int ys);
};

extern autoCellStruct autoCellHead;

void freeAutoCell();
void drawMessage(const gfxEntryStruct *pGfxPtr, int globalX, int globalY, int width, int newColor, uint8 *ouputPtr);
void drawMask(unsigned char *workBuf, int wbWidth, int wbHeight, unsigned char *pMask, int maskWidth, int maskHeight, int maskX, int maskY, int passIdx);
} // End of namespace Cruise

#endif
