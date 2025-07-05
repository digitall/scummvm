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

#include "fitd/track.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/room.h"
#include "fitd/vars.h"

namespace Fitd {
#define TL_INIT_COOR 0
#define TL_GOTO 1
#define TL_END 2
#define TL_REPEAT 3
#define TL_MARK 4
#define TL_WALK 5
#define TL_RUN 6
#define TL_STOP 7
#define TL_BACK 8
#define TL_SET_ANGLE 9
#define TL_COL_OFF 10
#define TL_COL_ON 11
#define TL_SET_DIST 12
#define TL_DEC_OFF 13
#define TL_DEC_ON 14
#define TL_GOTO_3D 15
#define TL_MEMO_COOR 16
#define TL_GOTO_3DX 17
#define TL_GOTO_3DZ 18
#define TL_ANGLE 19
#define TL_CLOSE 20

int makeProportional(int x1, int x2, int y1, int y2) {
	return x1 + (x2 - x1) * y2 / y1;
}

int computeAngleModificatorToPositionSub1(int ax) {
	int xOut;
	int yOut;

	rotate(ax, 0, 1000, &xOut, &yOut);

	yOut *= g_engine->_engine->angleCompZ;
	xOut *= g_engine->_engine->angleCompX;

	yOut -= xOut;

	if (yOut == 0)
		return 0;

	if (yOut > 0)
		return 1;
	return -1;
}

int computeAngleModificatorToPosition(int x1, int z1, int beta, int x2, int z2) {

	g_engine->_engine->angleCompX = x2 - x1;
	g_engine->_engine->angleCompZ = z2 - z1;
	g_engine->_engine->angleCompBeta = beta;

	const int resultMin = computeAngleModificatorToPositionSub1(beta - 4);
	const int resultMax = computeAngleModificatorToPositionSub1(beta + 4);

	if (resultMax == -1 && resultMin == 1) // in the middle
	{
		return computeAngleModificatorToPositionSub1(beta);
	}
	return (resultMax + resultMin + 1) >> 1;
}

void gereManualRot(int param) {
	if (g_engine->_engine->localJoyD & 4) {
		if (g_engine->_engine->currentProcessedActorPtr->direction != 1) {
			g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
		}

		g_engine->_engine->currentProcessedActorPtr->direction = 1;

		if (g_engine->_engine->currentProcessedActorPtr->rotate.param == 0) {
			const int oldBeta = g_engine->_engine->currentProcessedActorPtr->beta;

			if (g_engine->_engine->currentProcessedActorPtr->speed == 0) {
				initRealValue(oldBeta, oldBeta + 0x100, param / 2, &g_engine->_engine->currentProcessedActorPtr->rotate);
			} else {
				initRealValue(oldBeta, oldBeta + 0x100, param, &g_engine->_engine->currentProcessedActorPtr->rotate);
			}
		}

		g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
	}
	if (g_engine->_engine->localJoyD & 8) {
		if (g_engine->_engine->currentProcessedActorPtr->direction != -1) {
			g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
		}

		g_engine->_engine->currentProcessedActorPtr->direction = -1;

		if (g_engine->_engine->currentProcessedActorPtr->rotate.param == 0) {
			const int oldBeta = g_engine->_engine->currentProcessedActorPtr->beta;

			if (g_engine->_engine->currentProcessedActorPtr->speed == 0) {
				initRealValue(oldBeta, oldBeta - 0x100, param / 2, &g_engine->_engine->currentProcessedActorPtr->rotate);
			} else {
				initRealValue(oldBeta, oldBeta - 0x100, param, &g_engine->_engine->currentProcessedActorPtr->rotate);
			}
		}

		g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
	}
	if (!(g_engine->_engine->localJoyD & 0xC)) {
		g_engine->_engine->currentProcessedActorPtr->direction = 0;
		g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
	}
}

#define DISTANCE_TO_POINT_TRESSHOLD 400

uint lastTimeForward = 0;

char *getRoomLink(uint room1, uint room2) {
	char *zoneData = (char *)getRoomData(room1);

	zoneData += *(int16 *)zoneData;
	const int16 numOfZones = *(int16 *)zoneData;
	zoneData += 2;

	char *bestZone = zoneData;

	for (int i = 0; i < numOfZones; i++) {
		if (*(int16 *)(zoneData + 14) == 4) {
			bestZone = zoneData;

			if (*(int16 *)(zoneData + 12) == static_cast<int16>(room2)) {
				return bestZone;
			}
		}

		zoneData += 16;
	}

	return bestZone;
}

void processTrack() {
	switch (g_engine->_engine->currentProcessedActorPtr->trackMode) {
	case 1: // manual
	{
		gereManualRot(60);
		if (g_engine->_engine->localJoyD & 1) // forward
		{
			if (g_engine->_engine->timer - lastTimeForward < 10 && g_engine->_engine->currentProcessedActorPtr->speed != 4) // start running ?
			{
				g_engine->_engine->currentProcessedActorPtr->speed = 5;
			} else {
				if (g_engine->_engine->currentProcessedActorPtr->speed == 0 || g_engine->_engine->currentProcessedActorPtr->speed == -1) {
					g_engine->_engine->currentProcessedActorPtr->speed = 4;
				}
			}

			/*        if(g_engine->_engine->currentProcessedActorPtr->speed>0 && g_engine->_engine->currentProcessedActorPtr->speed<4)
			g_engine->_engine->currentProcessedActorPtr->speed = 5; */

			lastTimeForward = g_engine->_engine->timer;
		} else {
			if (g_engine->_engine->currentProcessedActorPtr->speed > 0 && g_engine->_engine->currentProcessedActorPtr->speed <= 4) {
				g_engine->_engine->currentProcessedActorPtr->speed--;
			} else {
				g_engine->_engine->currentProcessedActorPtr->speed = 0;
			}
		}

		if (g_engine->_engine->localJoyD & 2) // backward
		{
			if (g_engine->_engine->currentProcessedActorPtr->speed == 0 || g_engine->_engine->currentProcessedActorPtr->speed >= 4)
				g_engine->_engine->currentProcessedActorPtr->speed = -1;

			if (g_engine->_engine->currentProcessedActorPtr->speed == 5)
				g_engine->_engine->currentProcessedActorPtr->speed = 0;
		}

		break;
	}
	case 2: // follow
	{
		const int followedActorIdx = g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->trackNumber].objIndex;

		if (followedActorIdx == -1) {
			g_engine->_engine->currentProcessedActorPtr->direction = 0;
			g_engine->_engine->currentProcessedActorPtr->speed = 0;
		} else {
			const Object *followedActorPtr = &g_engine->_engine->objectTable[followedActorIdx];

			const int roomNumber = followedActorPtr->room;
			int x = followedActorPtr->roomX;
			// int y = followedActorPtr->roomY;
			int z = followedActorPtr->roomZ;

			if (g_engine->_engine->currentProcessedActorPtr->room != roomNumber) {
				char *link = getRoomLink(g_engine->_engine->currentProcessedActorPtr->room, roomNumber);

				x = *(int16 *)link + (*(int16 *)(link + 2) - *(int16 *)link) / 2;
				// y = *(int16 *)(link + 4) + (((*(int16 *)(link + 6)) - (*(int16 *)(link + 4))) / 2);
				z = *(int16 *)(link + 8) + (*(int16 *)(link + 10) - *(int16 *)(link + 8)) / 2;
			}

			const int angleModif = computeAngleModificatorToPosition(g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX,
																	 g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ,
																	 g_engine->_engine->currentProcessedActorPtr->beta, x, z);

			if (g_engine->_engine->currentProcessedActorPtr->rotate.param == 0 || g_engine->_engine->currentProcessedActorPtr->direction != angleModif) {
				initRealValue(g_engine->_engine->currentProcessedActorPtr->beta, g_engine->_engine->currentProcessedActorPtr->beta - (angleModif << 8), 60, &g_engine->_engine->currentProcessedActorPtr->rotate);
			}

			g_engine->_engine->currentProcessedActorPtr->direction = angleModif;

			if (g_engine->_engine->currentProcessedActorPtr->direction == 0) {
				g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
			} else {
				g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
			}

			g_engine->_engine->currentProcessedActorPtr->speed = 4;
		}
		break;
	}
	case 3: // track
	{
		byte *trackPtr = hqrGet(g_engine->_engine->listTrack, g_engine->_engine->currentProcessedActorPtr->trackNumber);

		trackPtr += g_engine->_engine->currentProcessedActorPtr->positionInTrack * 2;

		const int16 trackMacro = *(int16 *)trackPtr;
		trackPtr += 2;

		// printf("Track macro %X\n",trackMacro);

		switch (trackMacro) {
		case TL_INIT_COOR: // warp
		{
			const int roomNumber = *(int16 *)trackPtr;
			trackPtr += 2;

			if (g_engine->_engine->currentProcessedActorPtr->room != roomNumber) {
				if (g_engine->_engine->currentCameraTargetActor == g_engine->_engine->currentProcessedActorIdx) {
					g_engine->_engine->needChangeRoom = 1;
					g_engine->_engine->newRoom = roomNumber;
				}

				g_engine->_engine->currentProcessedActorPtr->room = roomNumber;
			}

			g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 -= g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 -= g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 -= g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 -= g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 -= g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 -= g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ;

			g_engine->_engine->currentProcessedActorPtr->worldX = g_engine->_engine->currentProcessedActorPtr->roomX = *(int16 *)trackPtr;
			trackPtr += 2;
			g_engine->_engine->currentProcessedActorPtr->worldY = g_engine->_engine->currentProcessedActorPtr->roomY = *(int16 *)trackPtr;
			trackPtr += 2;
			g_engine->_engine->currentProcessedActorPtr->worldZ = g_engine->_engine->currentProcessedActorPtr->roomZ = *(int16 *)trackPtr;
			trackPtr += 2;

			g_engine->_engine->currentProcessedActorPtr->worldX -= static_cast<int16>((g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldX - g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldX) * 10);
			g_engine->_engine->currentProcessedActorPtr->worldY += static_cast<int16>((g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldY - g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldY) * 10);
			g_engine->_engine->currentProcessedActorPtr->worldZ += static_cast<int16>((g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldZ - g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldZ) * 10);

			g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 += g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 += g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 += g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 += g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ;

			g_engine->_engine->currentProcessedActorPtr->speed = 0;
			g_engine->_engine->currentProcessedActorPtr->direction = 0;
			g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack += 5;

			break;
		}
		case TL_GOTO: // goToPosition
		{
			const int roomNumber = *(int16 *)trackPtr;

			trackPtr += 2;

			int x = *(int16 *)trackPtr;
			trackPtr += 2;
			int z = *(int16 *)trackPtr;
			trackPtr += 2;

			if (roomNumber != g_engine->_engine->currentProcessedActorPtr->room) {
				// TODO: fix bug here...
				x -= (g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldX - g_engine->_engine->roomDataTable[roomNumber].worldX) * 10;
				z += (g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldZ - g_engine->_engine->roomDataTable[roomNumber].worldZ) * 10;
			}

			const uint distanceToPoint = computeDistanceToPoint(g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX,
																g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ,
																x, z);

			if (distanceToPoint >= DISTANCE_TO_POINT_TRESSHOLD) // not yet at position
			{
				const int angleModif = computeAngleModificatorToPosition(g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX,
																		 g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ,
																		 g_engine->_engine->currentProcessedActorPtr->beta,
																		 x, z);

				if (g_engine->_engine->currentProcessedActorPtr->rotate.param == 0 || g_engine->_engine->currentProcessedActorPtr->direction != angleModif) {
					initRealValue(g_engine->_engine->currentProcessedActorPtr->beta, g_engine->_engine->currentProcessedActorPtr->beta - angleModif * 64, 15, &g_engine->_engine->currentProcessedActorPtr->rotate);
				}

				g_engine->_engine->currentProcessedActorPtr->direction = angleModif;

				if (!angleModif) {
					g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
				} else {
					g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
				}
			} else // reached position
			{
				g_engine->_engine->currentProcessedActorPtr->positionInTrack += 4;
			}

			break;
		}
		case TL_GOTO_3D: // goToPosition
		{
			const int roomNumber = *(int16 *)trackPtr;

			trackPtr += 2;

			int x = *(int16 *)trackPtr;
			trackPtr += 2;
			int y = *(int16 *)trackPtr;
			trackPtr += 2;
			int z = *(int16 *)trackPtr;
			trackPtr += 2;
			const int time = *(int16 *)trackPtr;
			trackPtr += 2;

			if (roomNumber != g_engine->_engine->currentProcessedActorPtr->room) {
				// TODO: fix bug here...
				x -= (g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldX - g_engine->_engine->roomDataTable[roomNumber].worldX) * 10;
				y += (g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldY - g_engine->_engine->roomDataTable[roomNumber].worldY) * 10;
				z += (g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldZ - g_engine->_engine->roomDataTable[roomNumber].worldZ) * 10;
			}

			// reached position?
			if (y == g_engine->_engine->currentProcessedActorPtr->roomY && computeDistanceToPoint(g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX, g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ, x, z) < DISTANCE_TO_POINT_TRESSHOLD) {
				g_engine->_engine->currentProcessedActorPtr->positionInTrack += 6;
			} else {
				const int angleModif = computeAngleModificatorToPosition(
					g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX,
					g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ,
					g_engine->_engine->currentProcessedActorPtr->beta,
					x, z);

				if (g_engine->_engine->currentProcessedActorPtr->YHandler.param == 0) {
					initRealValue(0, y - (g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY), time, &g_engine->_engine->currentProcessedActorPtr->YHandler);
				}

				if (g_engine->_engine->currentProcessedActorPtr->rotate.param == 0 || g_engine->_engine->currentProcessedActorPtr->direction != angleModif) {
					initRealValue(g_engine->_engine->currentProcessedActorPtr->beta, g_engine->_engine->currentProcessedActorPtr->beta - angleModif * 256, 60, &g_engine->_engine->currentProcessedActorPtr->rotate);
				}

				g_engine->_engine->currentProcessedActorPtr->direction = angleModif;

				if (!angleModif) {
					g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
				} else {
					g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
				}
			}

			break;
		}
		case TL_END: // stop
		{
			g_engine->_engine->currentProcessedActorPtr->speed = 0;
			g_engine->_engine->currentProcessedActorPtr->trackNumber = -1;
			setMoveMode(0, 0);
			break;
		}
		case TL_REPEAT: {
			g_engine->_engine->currentProcessedActorPtr->positionInTrack = 0;
			break;
		}
		case TL_MARK: // MARK
		{
			g_engine->_engine->currentProcessedActorPtr->MARK = *(int16 *)trackPtr;
			trackPtr += 2;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack += 2;
			break;
		}
		case TL_WALK: {
			g_engine->_engine->currentProcessedActorPtr->speed = 4;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_RUN: {
			g_engine->_engine->currentProcessedActorPtr->speed = 5;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_STOP: {
			g_engine->_engine->currentProcessedActorPtr->speed = 0;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_SET_ANGLE: // TL_SET_ANGLE
		{
			const int betaDif = *(int16 *)trackPtr;
			trackPtr += 2;

			if (((g_engine->_engine->currentProcessedActorPtr->beta - betaDif) & 1023) > 512) {
				g_engine->_engine->currentProcessedActorPtr->direction = 1; // left
			} else {
				g_engine->_engine->currentProcessedActorPtr->direction = -1; // right
			}

			if (!g_engine->_engine->currentProcessedActorPtr->rotate.param) {
				initRealValue(g_engine->_engine->currentProcessedActorPtr->beta, betaDif, 120, &g_engine->_engine->currentProcessedActorPtr->rotate);
			}

			g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);

			if (g_engine->_engine->currentProcessedActorPtr->beta == betaDif) {
				g_engine->_engine->currentProcessedActorPtr->direction = 0;

				g_engine->_engine->currentProcessedActorPtr->positionInTrack += 2;
			}

			break;
		}
		case TL_COL_OFF: {
			g_engine->_engine->currentProcessedActorPtr->dynFlags &= 0xFFFE;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_COL_ON: {
			g_engine->_engine->currentProcessedActorPtr->dynFlags |= 1;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_DEC_OFF: // background collision off
		{
			g_engine->_engine->currentProcessedActorPtr->_flags &= ~AF_TRIGGER;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_DEC_ON: // background collision on
		{
			g_engine->_engine->currentProcessedActorPtr->_flags |= AF_TRIGGER;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_MEMO_COOR: {
			const int objNum = g_engine->_engine->currentProcessedActorPtr->indexInWorld;

			g_engine->_engine->worldObjets[objNum].x = g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX;
			g_engine->_engine->worldObjets[objNum].y = g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY;
			g_engine->_engine->worldObjets[objNum].z = g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ;

			g_engine->_engine->currentProcessedActorPtr->positionInTrack++;

			break;
		}
		case TL_GOTO_3DX: // walk up/down stairs on X
		{

			const int x = *(int16 *)trackPtr;
			trackPtr += 2;
			const int y = *(int16 *)trackPtr;
			trackPtr += 2;
			const int z = *(int16 *)trackPtr;
			trackPtr += 2;

			const int objX = g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].x;
			const int objY = g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].y;
			// objZ = g_engine->_engine->ListWorldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].z;

			if (g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY < y - 100 || g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY > y + 100) {
				const int propX = makeProportional(objY, y, x - objX, g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX - objX);

				const int difY = propX - g_engine->_engine->currentProcessedActorPtr->worldY;

				g_engine->_engine->currentProcessedActorPtr->worldY += difY;
				g_engine->_engine->currentProcessedActorPtr->roomY += difY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += difY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += difY;

				const int angleModif = computeAngleModificatorToPosition(g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX,
																		 g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ,
																		 g_engine->_engine->currentProcessedActorPtr->beta,
																		 x, z);

				if (!g_engine->_engine->currentProcessedActorPtr->rotate.param || g_engine->_engine->currentProcessedActorPtr->direction != angleModif) {
					initRealValue(g_engine->_engine->currentProcessedActorPtr->beta, g_engine->_engine->currentProcessedActorPtr->beta - (angleModif << 8), 60, &g_engine->_engine->currentProcessedActorPtr->rotate);
				}

				g_engine->_engine->currentProcessedActorPtr->direction = angleModif;

				if (angleModif) {
					g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
				} else {
					g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
				}

			} else {
				const int difY = y - g_engine->_engine->currentProcessedActorPtr->worldY;

				g_engine->_engine->currentProcessedActorPtr->stepY = 0;
				g_engine->_engine->currentProcessedActorPtr->worldY += difY;
				g_engine->_engine->currentProcessedActorPtr->roomY += difY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += difY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += difY;

				g_engine->_engine->currentProcessedActorPtr->positionInTrack += 4;
			}

			break;
		}
		case TL_GOTO_3DZ: // walk up/down stairs on Z
		{

			const int x = *(int16 *)trackPtr;
			trackPtr += 2;
			const int y = *(int16 *)trackPtr;
			trackPtr += 2;
			const int z = *(int16 *)trackPtr;
			trackPtr += 2;

			const int objY = g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].y;
			const int objZ = g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].z;

			if (g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY < y - 100 || g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY > y + 100) {
				const int propZ = makeProportional(objY, y, z - objZ, g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ - objZ);

				const int difY = propZ - g_engine->_engine->currentProcessedActorPtr->worldY;

				g_engine->_engine->currentProcessedActorPtr->worldY += difY;
				g_engine->_engine->currentProcessedActorPtr->roomY += difY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += difY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += difY;

				const int angleModif = computeAngleModificatorToPosition(g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX,
																		 g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ,
																		 g_engine->_engine->currentProcessedActorPtr->beta,
																		 x, z);

				if (!g_engine->_engine->currentProcessedActorPtr->rotate.param || g_engine->_engine->currentProcessedActorPtr->direction != angleModif) {
					initRealValue(g_engine->_engine->currentProcessedActorPtr->beta, g_engine->_engine->currentProcessedActorPtr->beta - (angleModif << 8), 60, &g_engine->_engine->currentProcessedActorPtr->rotate);
				}

				g_engine->_engine->currentProcessedActorPtr->direction = angleModif;

				if (angleModif) {
					g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
				} else {
					g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
				}

			} else {
				const int difY = y - g_engine->_engine->currentProcessedActorPtr->worldY;

				g_engine->_engine->currentProcessedActorPtr->stepY = 0;
				g_engine->_engine->currentProcessedActorPtr->worldY += difY;
				g_engine->_engine->currentProcessedActorPtr->roomY += difY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += difY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += difY;

				g_engine->_engine->currentProcessedActorPtr->positionInTrack += 4;
			}

			break;
		}
		case TL_ANGLE: // rotate
		{
			g_engine->_engine->currentProcessedActorPtr->alpha = *(int16 *)trackPtr;
			trackPtr += 2;
			g_engine->_engine->currentProcessedActorPtr->beta = *(int16 *)trackPtr;
			trackPtr += 2;
			g_engine->_engine->currentProcessedActorPtr->gamma = *(int16 *)trackPtr;
			trackPtr += 2;

			g_engine->_engine->currentProcessedActorPtr->direction = 0;

			g_engine->_engine->currentProcessedActorPtr->positionInTrack += 4;

			break;
		}
		default: {
			error("Unknown track macro %X\n", trackMacro);
			break;
		}
		}

		break;
	}
	}

	g_engine->_engine->currentProcessedActorPtr->beta &= 0x3FF;
}

void processTrack2() {
	switch (g_engine->_engine->currentProcessedActorPtr->trackMode) {
	case 1: // manual
	{
		gereManualRot(60);
		if (g_engine->_engine->localJoyD & 1) // forward
		{
			if (g_engine->_engine->timer - lastTimeForward < 10 && g_engine->_engine->currentProcessedActorPtr->speed != 4)
				g_engine->_engine->currentProcessedActorPtr->speed = 5;
			else if (g_engine->_engine->currentProcessedActorPtr->speed == 0 || g_engine->_engine->currentProcessedActorPtr->speed == -1)
				g_engine->_engine->currentProcessedActorPtr->speed = 4;

			/*        if(g_engine->_engine->currentProcessedActorPtr->speed>0 && g_engine->_engine->currentProcessedActorPtr->speed<4)
			g_engine->_engine->currentProcessedActorPtr->speed = 5; */

			lastTimeForward = g_engine->_engine->timer;
		} else {
			if (g_engine->_engine->currentProcessedActorPtr->speed > 0 && g_engine->_engine->currentProcessedActorPtr->speed <= 4) {
				g_engine->_engine->currentProcessedActorPtr->speed--;
			} else {
				g_engine->_engine->currentProcessedActorPtr->speed = 0;
			}
		}

		if (g_engine->_engine->localJoyD & 2) // backward
		{
			if (g_engine->_engine->currentProcessedActorPtr->speed == 0 || g_engine->_engine->currentProcessedActorPtr->speed >= 4)
				g_engine->_engine->currentProcessedActorPtr->speed = -1;

			if (g_engine->_engine->currentProcessedActorPtr->speed == 5)
				g_engine->_engine->currentProcessedActorPtr->speed = 0;
		}

		break;
	}
	case 2: // follow
	{
		const int followedActorIdx = g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->trackNumber].objIndex;

		if (followedActorIdx == -1) {
			g_engine->_engine->currentProcessedActorPtr->direction = 0;
			g_engine->_engine->currentProcessedActorPtr->speed = 0;
		} else {
			const Object *followedActorPtr = &g_engine->_engine->objectTable[followedActorIdx];

			const int roomNumber = followedActorPtr->room;
			const int x = followedActorPtr->roomX;
			// int y = followedActorPtr->roomY;
			const int z = followedActorPtr->roomZ;

			if (g_engine->_engine->currentProcessedActorPtr->room != roomNumber) {
				/*  char* link = getRoomLink(g_engine->_engine->currentProcessedActorPtr->room,roomNumber);

				x = *(int16*)(link)+(((*(int16*)(link+2))-(*(int16 *)(link))) / 2);
				y = *(int16*)(link+4)+(((*(int16*)(link+6))-(*(int16 *)(link+4))) / 2);
				z = *(int16*)(link+8)+(((*(int16*)(link+10))-(*(int16 *)(link+8))) / 2); */
			}

			const int angleModif = computeAngleModificatorToPosition(g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX,
																	 g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ,
																	 g_engine->_engine->currentProcessedActorPtr->beta, x, z);

			if (g_engine->_engine->currentProcessedActorPtr->rotate.param == 0 || g_engine->_engine->currentProcessedActorPtr->direction != angleModif) {
				initRealValue(g_engine->_engine->currentProcessedActorPtr->beta, g_engine->_engine->currentProcessedActorPtr->beta - (angleModif << 8), 60, &g_engine->_engine->currentProcessedActorPtr->rotate);
			}

			g_engine->_engine->currentProcessedActorPtr->direction = angleModif;

			if (g_engine->_engine->currentProcessedActorPtr->direction == 0) {
				g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
			} else {
				g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
			}

			g_engine->_engine->currentProcessedActorPtr->speed = 4;
		}
		break;
	}
	case 3: // track
	{
		byte *trackPtr = hqrGet(g_engine->_engine->listTrack, g_engine->_engine->currentProcessedActorPtr->trackNumber);

		trackPtr += g_engine->_engine->currentProcessedActorPtr->positionInTrack * 2;

		const int16 trackMacro = *(int16 *)trackPtr;
		trackPtr += 2;

		// printf("Track macro %X\n",trackMacro);

		switch (trackMacro) {
		case 0: // warp
		{
			const int roomNumber = *(int16 *)trackPtr;
			trackPtr += 2;

			if (g_engine->_engine->currentProcessedActorPtr->room != roomNumber) {
				if (g_engine->_engine->currentCameraTargetActor == g_engine->_engine->currentProcessedActorIdx) {
					g_engine->_engine->needChangeRoom = 1;
					g_engine->_engine->newRoom = roomNumber;
				}

				g_engine->_engine->currentProcessedActorPtr->room = roomNumber;
			}

			g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 -= g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 -= g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 -= g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 -= g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 -= g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 -= g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ;

			g_engine->_engine->currentProcessedActorPtr->worldX = g_engine->_engine->currentProcessedActorPtr->roomX = *(int16 *)trackPtr;
			trackPtr += 2;
			g_engine->_engine->currentProcessedActorPtr->worldY = g_engine->_engine->currentProcessedActorPtr->roomY = *(int16 *)trackPtr;
			trackPtr += 2;
			g_engine->_engine->currentProcessedActorPtr->worldZ = g_engine->_engine->currentProcessedActorPtr->roomZ = *(int16 *)trackPtr;
			trackPtr += 2;

			g_engine->_engine->currentProcessedActorPtr->worldX -= static_cast<int16>((g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldX - g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldX) * 10);
			g_engine->_engine->currentProcessedActorPtr->worldY += static_cast<int16>((g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldY - g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldY) * 10);
			g_engine->_engine->currentProcessedActorPtr->worldZ += static_cast<int16>((g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldZ - g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldZ) * 10);

			g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 += g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 += g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 += g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ;
			g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 += g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ;

			g_engine->_engine->currentProcessedActorPtr->speed = 0;
			g_engine->_engine->currentProcessedActorPtr->direction = 0;
			g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack += 5;

			break;
		}
		case 1: // goToPosition
		{
			const int roomNumber = *(int16 *)trackPtr;

			trackPtr += 2;

			int x = *(int16 *)trackPtr;
			trackPtr += 2;
			int z = *(int16 *)trackPtr;
			trackPtr += 2;

			if (roomNumber != g_engine->_engine->currentProcessedActorPtr->room) {
				x -= (g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldX - g_engine->_engine->roomDataTable[roomNumber].worldX) * 10;
				z += (g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldZ - g_engine->_engine->roomDataTable[roomNumber].worldZ) * 10;
			}

			const uint distanceToPoint = computeDistanceToPoint(g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX,
																g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ,
																x, z);

			if (distanceToPoint >= DISTANCE_TO_POINT_TRESSHOLD) // not yet at position
			{
				const int angleModif = computeAngleModificatorToPosition(g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX,
																		 g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ,
																		 g_engine->_engine->currentProcessedActorPtr->beta,
																		 x, z);

				if (g_engine->_engine->currentProcessedActorPtr->rotate.param == 0 || g_engine->_engine->currentProcessedActorPtr->direction != angleModif) {
					initRealValue(g_engine->_engine->currentProcessedActorPtr->beta, g_engine->_engine->currentProcessedActorPtr->beta - (angleModif << 6), 15, &g_engine->_engine->currentProcessedActorPtr->rotate);
				}

				g_engine->_engine->currentProcessedActorPtr->direction = angleModif;

				if (!angleModif) {
					g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
				} else {
					g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
				}
			} else // reached position
			{
				g_engine->_engine->currentProcessedActorPtr->positionInTrack += 4;
			}

			break;
		}
		case 2: // stop
		{
			g_engine->_engine->currentProcessedActorPtr->speed = 0;
			g_engine->_engine->currentProcessedActorPtr->trackNumber = -1;
			setMoveMode(0, 0);
			break;
		}
		case 3: {
			g_engine->_engine->currentProcessedActorPtr->positionInTrack = 0;
			break;
		}
		case 4: // MARK
		{
			g_engine->_engine->currentProcessedActorPtr->MARK = *(int16 *)trackPtr;
			trackPtr += 2;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack += 2;
			break;
		}
		case 5: {
			break;
		}
		case 0x6: {
			const int betaDif = *(int16 *)trackPtr;
			trackPtr += 2;

			if (((g_engine->_engine->currentProcessedActorPtr->beta - betaDif) & 0x3FF) > 0x200) {
				g_engine->_engine->currentProcessedActorPtr->direction = 1;
			} else {
				g_engine->_engine->currentProcessedActorPtr->direction = -1;
			}

			if (!g_engine->_engine->currentProcessedActorPtr->rotate.param) {
				initRealValue(g_engine->_engine->currentProcessedActorPtr->beta, betaDif, 120, &g_engine->_engine->currentProcessedActorPtr->rotate);
			}

			g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);

			if (g_engine->_engine->currentProcessedActorPtr->beta == betaDif) {
				g_engine->_engine->currentProcessedActorPtr->direction = 0;

				g_engine->_engine->currentProcessedActorPtr->positionInTrack += 2;
			}

			break;
		}
		case 0x7: {
			g_engine->_engine->currentProcessedActorPtr->dynFlags &= 0xFFFE;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case 0x8: {
			g_engine->_engine->currentProcessedActorPtr->dynFlags |= 1;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case 0xA: {
			g_engine->_engine->currentProcessedActorPtr->_flags &= 0xFFBF;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case 0xB: {
			g_engine->_engine->currentProcessedActorPtr->_flags |= 0x40;
			g_engine->_engine->currentProcessedActorPtr->positionInTrack++;
			break;
		} /*
		  case 0x10:
		  {
		  int objNum = g_engine->_engine->currentProcessedActorPtr->field_0;

		  objectTable[objNum].x = g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->modX;
		  objectTable[objNum].y = g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->modY;
		  objectTable[objNum].z = g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->modZ;

		  g_engine->_engine->currentProcessedActorPtr->positionInTrack++;

		  break;
		  }
		  case 0x11: // walk up/down stairs on X
		  {
		  int x;
		  int y;
		  int z;
		  int objX;
		  int objY;
		  int objZ;

		  x = *(int16*)(trackPtr);
		  trackPtr += 2;
		  y = *(int16*)(trackPtr);
		  trackPtr += 2;
		  z = *(int16*)(trackPtr);
		  trackPtr += 2;

		  objX = objectTable[g_engine->_engine->currentProcessedActorPtr->field_0].x;
		  objY = objectTable[g_engine->_engine->currentProcessedActorPtr->field_0].y;
		  objZ = objectTable[g_engine->_engine->currentProcessedActorPtr->field_0].z;

		  if(   g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->modY < y - 100
		  ||  g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->modY > y + 100)
		  {
		  int propX = makeProportional(objY, y, x - objX, (g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->modX) - objX);

		  int difY = propX - g_engine->_engine->currentProcessedActorPtr->worldY;
		  int angleModif;

		  g_engine->_engine->currentProcessedActorPtr->worldY += difY;
		  g_engine->_engine->currentProcessedActorPtr->roomY += difY;
		  g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += difY;
		  g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += difY;

		  angleModif = computeAngleModificatorToPosition( g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->modX,
		  g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->modZ,
		  g_engine->_engine->currentProcessedActorPtr->beta,
		  x,z );

		  if(!g_engine->_engine->currentProcessedActorPtr->rotate.param || g_engine->_engine->currentProcessedActorPtr->field_72 != angleModif)
		  {
		  startActorRotation(g_engine->_engine->currentProcessedActorPtr->beta, g_engine->_engine->currentProcessedActorPtr->beta - (angleModif<<8), 60, &g_engine->_engine->currentProcessedActorPtr->rotate);
		  }

		  g_engine->_engine->currentProcessedActorPtr->field_72 = angleModif;

		  if(angleModif)
		  {
		  g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
		  }
		  else
		  {
		  g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
		  }

		  }
		  else
		  {
		  int difY = y - g_engine->_engine->currentProcessedActorPtr->worldY;

		  g_engine->_engine->currentProcessedActorPtr->modY = 0;
		  g_engine->_engine->currentProcessedActorPtr->worldY += difY;
		  g_engine->_engine->currentProcessedActorPtr->roomY += difY;
		  g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += difY;
		  g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += difY;

		  g_engine->_engine->currentProcessedActorPtr->positionInTrack +=4;
		  }

		  break;
		  }
		  case 0x12: // walk up/down stairs on Z
		  {
		  int x;
		  int y;
		  int z;
		  int objX;
		  int objY;
		  int objZ;

		  x = *(int16*)(trackPtr);
		  trackPtr += 2;
		  y = *(int16*)(trackPtr);
		  trackPtr += 2;
		  z = *(int16*)(trackPtr);
		  trackPtr += 2;

		  objX = objectTable[g_engine->_engine->currentProcessedActorPtr->field_0].x;
		  objY = objectTable[g_engine->_engine->currentProcessedActorPtr->field_0].y;
		  objZ = objectTable[g_engine->_engine->currentProcessedActorPtr->field_0].z;

		  if(   g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->modY < y - 100
		  ||  g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->modY > y + 100)
		  {
		  int propZ = makeProportional(objY, y, z - objZ, (g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->modZ) - objZ);

		  int difY = propZ - g_engine->_engine->currentProcessedActorPtr->worldY;

		  int angleModif;

		  g_engine->_engine->currentProcessedActorPtr->worldY += difY;
		  g_engine->_engine->currentProcessedActorPtr->roomY += difY;
		  g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += difY;
		  g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += difY;

		  angleModif = computeAngleModificatorToPosition( g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->modX,
		  g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->modZ,
		  g_engine->_engine->currentProcessedActorPtr->beta,
		  x,z );

		  if(!g_engine->_engine->currentProcessedActorPtr->rotate.param || g_engine->_engine->currentProcessedActorPtr->field_72 != angleModif)
		  {
		  startActorRotation(g_engine->_engine->currentProcessedActorPtr->beta, g_engine->_engine->currentProcessedActorPtr->beta - (angleModif<<8), 60, &g_engine->_engine->currentProcessedActorPtr->rotate);
		  }

		  g_engine->_engine->currentProcessedActorPtr->field_72 = angleModif;

		  if(angleModif)
		  {
		  g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
		  }
		  else
		  {
		  g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
		  }

		  }
		  else
		  {
		  int difY = y - g_engine->_engine->currentProcessedActorPtr->worldY;

		  g_engine->_engine->currentProcessedActorPtr->modY = 0;
		  g_engine->_engine->currentProcessedActorPtr->worldY += difY;
		  g_engine->_engine->currentProcessedActorPtr->roomY += difY;
		  g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += difY;
		  g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += difY;

		  g_engine->_engine->currentProcessedActorPtr->positionInTrack +=4;
		  }

		  break;
		  }
		  case 0x13: // rotate
		  {
		  g_engine->_engine->currentProcessedActorPtr->alpha = *(int16*)(trackPtr);
		  trackPtr += 2;
		  g_engine->_engine->currentProcessedActorPtr->beta = *(int16*)(trackPtr);
		  trackPtr += 2;
		  g_engine->_engine->currentProcessedActorPtr->gamma = *(int16*)(trackPtr);
		  trackPtr += 2;

		  g_engine->_engine->currentProcessedActorPtr->field_72 = 0;

		  g_engine->_engine->currentProcessedActorPtr->positionInTrack +=4;

		  break;
		  } */
		default: {
			error("Unknown track macro %X\n", trackMacro);
			break;
		}
		}

		break;
	}
	}

	g_engine->_engine->currentProcessedActorPtr->beta &= 0x3FF;
}
} // namespace Fitd
