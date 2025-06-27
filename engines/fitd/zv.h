/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef FITD_ZV_H
#define FITD_ZV_H

#include "common/scummsys.h"

namespace Fitd {

typedef struct ZVStruct
{
	int32 ZVX1;
	int32 ZVX2;
	int32 ZVY1;
	int32 ZVY2;
	int32 ZVZ1;
	int32 ZVZ2;
} ZVStruct;

void getZvCube(const byte *bodyPtr, ZVStruct *zvPtr);
void giveZVObjet(const byte * bodyPtr, ZVStruct* zvPtr);
void getZvMax(byte* bodyPtr, ZVStruct* zvPtr);
void makeDefaultZV(ZVStruct* zvPtr);
struct RoomData;
int asmCheckListCol(const ZVStruct *zvPtr, RoomData *pRoomData);
void handleCollision(const ZVStruct *startZv, const ZVStruct *zvPtr2, const ZVStruct *zvPtr3);
void copyZv(const ZVStruct *source, ZVStruct *dest);
void getZvRelativePosition(ZVStruct *zvPtr, int startRoom, int destRoom);
int checkZvCollision(const ZVStruct *zvPtr1, const ZVStruct *zvPtr2);
int checkObjectCollisions(int actorIdx, const ZVStruct *zvPtr);
void getZvRot(byte *bodyPtr, ZVStruct *zvPtr, int alpha, int beta, int gamma);

}

#endif
