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

#ifndef CRUISE_CTP_H
#define CRUISE_CTP_H

#include "common/rect.h"

namespace Cruise {

class CtEntry {
public:
	CtEntry(int16 xs, int16 xe) { minX = xs; maxX = xe; }
	CtEntry() { minX = 0; maxX = 0; }

	int16 minX;
	int16 maxX;
};

class Ct {
public:
	int16 _num;
	int16 _color;
	Common::Rect _bounds;
	Common::Array<CtEntry> _slices;
};

class WalkBox {
public:
	int16 _array[40];
	int16 _zoom;
	int16 _color;		// saveVar4     // Type: 0x00 - non walkable, 0x01 - walkable, 0x02 - exit zone
	int16 _state;		// saveVar5		// walkbox can change its type: 0x00 - not changeable, 0x01 - changeable
						// Assumption: To change the type: walkboxColor[i] -= walkboxChane[i] and vice versa
	int setState(int nodeState);
};

int initCt(const char * ctpName, bool isLoading = false);
void initWalkBoxes(short int segmentSizeTable[7], uint8 *dataPointer, bool isLoading);
int computeDistance(int varX, int varY, int paramX, int paramY);

int getNode(int nodeIdx, int nodeResult[2]);
int setNodeColor(int nodeIdx, int nodeColor);

} // End of namespace Cruise

#endif
