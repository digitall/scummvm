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

#ifndef CRUISE_BACKGROUNDINCRUST_H
#define CRUISE_BACKGROUNDINCRUST_H

namespace Cruise {

class BackgroundIncrust {
public:
	uint16 _objectIdx;
	int16 _type;
	uint16 _overlayIdx;
	int16 _X;
	int16 _Y;
	uint16 _frame;
	uint16 _scale;
	uint16 _backgroundIdx;
	uint16 _scriptNumber;
	uint16 _scriptOverlayIdx;
	uint8 *_ptr;
	int16 _saveWidth;
	int16 _saveHeight;
	uint16 _saveSize;
	int16 _savedX;
	int16 _savedY;
	char _name[13];
	uint16 _spriteId;

	BackgroundIncrust(uint16 objectIdx, int16 type, uint16 overlayIdx, int16 X, int16 Y, uint16 frame, 
                        uint16 scale, uint16 backgroundIdx, uint16 scriptNumber, uint16 scriptOverlayIdx,
                        uint16 spriteId, char *name);
	void backup(int savedX, int savedY, int saveWidth, int saveHeight, uint8* pBackground);

};

class BackgroundIncrustListNode {
public:
	BackgroundIncrustListNode *next;
	BackgroundIncrustListNode *prev;
	BackgroundIncrust *backgroundIncrust;

        BackgroundIncrustListNode();
        void resetBackgroundIncrustList();
        void freeBackgroundIncrustList();
        void removeBackgroundIncrustNode(int overlay, int idx);
        void regenerateBackgroundIncrustList();
        void unmergeBackgroundIncrustList(int ovl, int idx);
        BackgroundIncrustListNode *addBackgroundIncrust(int16 overlayIdx, int16 param2, int16 scriptNumber, int16 scriptOverlay, int16 backgroundIdx, int16 param4);
};


} // End of namespace Cruise

#endif
