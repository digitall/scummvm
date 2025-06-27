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

#include "fitd/common.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/room.h"
#include "fitd/track.h"
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

	yOut *= angleCompZ;
	xOut *= angleCompX;

	yOut -= xOut;

	if (yOut == 0)
		return 0;

	if (yOut > 0)
		return 1;
	return -1;
}

int computeAngleModificatorToPosition(int x1, int z1, int beta, int x2, int z2) {

	angleCompX = x2 - x1;
	angleCompZ = z2 - z1;
	angleCompBeta = beta;

	const int resultMin = computeAngleModificatorToPositionSub1(beta - 4);
	const int resultMax = computeAngleModificatorToPositionSub1(beta + 4);

	if (resultMax == -1 && resultMin == 1) // in the middle
	{
		return computeAngleModificatorToPositionSub1(beta);
	}
	return (resultMax + resultMin + 1) >> 1;
}

void gereManualRot(int param) {
	if (localJoyD & 4) {
		if (currentProcessedActorPtr->direction != 1) {
			currentProcessedActorPtr->rotate.param = 0;
		}

		currentProcessedActorPtr->direction = 1;

		if (currentProcessedActorPtr->rotate.param == 0) {
			const int oldBeta = currentProcessedActorPtr->beta;

			if (currentProcessedActorPtr->speed == 0) {
				initRealValue(oldBeta, oldBeta + 0x100, param / 2, &currentProcessedActorPtr->rotate);
			} else {
				initRealValue(oldBeta, oldBeta + 0x100, param, &currentProcessedActorPtr->rotate);
			}
		}

		currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
	}
	if (localJoyD & 8) {
		if (currentProcessedActorPtr->direction != -1) {
			currentProcessedActorPtr->rotate.param = 0;
		}

		currentProcessedActorPtr->direction = -1;

		if (currentProcessedActorPtr->rotate.param == 0) {
			const int oldBeta = currentProcessedActorPtr->beta;

			if (currentProcessedActorPtr->speed == 0) {
				initRealValue(oldBeta, oldBeta - 0x100, param / 2, &currentProcessedActorPtr->rotate);
			} else {
				initRealValue(oldBeta, oldBeta - 0x100, param, &currentProcessedActorPtr->rotate);
			}
		}

		currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
	}
	if (!(localJoyD & 0xC)) {
		currentProcessedActorPtr->direction = 0;
		currentProcessedActorPtr->rotate.param = 0;
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

			if (*(int16 *)(zoneData + 12) == (int16)room2) {
				return bestZone;
			}
		}

		zoneData += 16;
	}

	return bestZone;
}

void processTrack() {
	switch (currentProcessedActorPtr->trackMode) {
	case 1: // manual
	{
		gereManualRot(60);
		if (localJoyD & 1) // forward
		{
			if (timer - lastTimeForward < 10 && currentProcessedActorPtr->speed != 4) // start running ?
			{
				currentProcessedActorPtr->speed = 5;
			} else {
				if (currentProcessedActorPtr->speed == 0 || currentProcessedActorPtr->speed == -1) {
					currentProcessedActorPtr->speed = 4;
				}
			}

			/*        if(currentProcessedActorPtr->speed>0 && currentProcessedActorPtr->speed<4)
			currentProcessedActorPtr->speed = 5; */

			lastTimeForward = timer;
		} else {
			if (currentProcessedActorPtr->speed > 0 && currentProcessedActorPtr->speed <= 4) {
				currentProcessedActorPtr->speed--;
			} else {
				currentProcessedActorPtr->speed = 0;
			}
		}

		if (localJoyD & 2) // backward
		{
			if (currentProcessedActorPtr->speed == 0 || currentProcessedActorPtr->speed >= 4)
				currentProcessedActorPtr->speed = -1;

			if (currentProcessedActorPtr->speed == 5)
				currentProcessedActorPtr->speed = 0;
		}

		break;
	}
	case 2: // follow
	{
		const int followedActorIdx = ListWorldObjets[currentProcessedActorPtr->trackNumber].objIndex;

		if (followedActorIdx == -1) {
			currentProcessedActorPtr->direction = 0;
			currentProcessedActorPtr->speed = 0;
		} else {
			const Object *followedActorPtr = &objectTable[followedActorIdx];

			const int roomNumber = followedActorPtr->room;
			int x = followedActorPtr->roomX;
			// int y = followedActorPtr->roomY;
			int z = followedActorPtr->roomZ;

			if (currentProcessedActorPtr->room != roomNumber) {
				char *link = getRoomLink(currentProcessedActorPtr->room, roomNumber);

				x = *(int16 *)link + (*(int16 *)(link + 2) - *(int16 *)link) / 2;
				// y = *(int16 *)(link + 4) + (((*(int16 *)(link + 6)) - (*(int16 *)(link + 4))) / 2);
				z = *(int16 *)(link + 8) + (*(int16 *)(link + 10) - *(int16 *)(link + 8)) / 2;
			}

			const int angleModif = computeAngleModificatorToPosition(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
															   currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
															   currentProcessedActorPtr->beta, x, z);

			if (currentProcessedActorPtr->rotate.param == 0 || currentProcessedActorPtr->direction != angleModif) {
				initRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif << 8), 60, &currentProcessedActorPtr->rotate);
			}

			currentProcessedActorPtr->direction = angleModif;

			if (currentProcessedActorPtr->direction == 0) {
				currentProcessedActorPtr->rotate.param = 0;
			} else {
				currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
			}

			currentProcessedActorPtr->speed = 4;
		}
		break;
	}
	case 3: // track
	{
		byte *trackPtr = HQR_Get(listTrack, currentProcessedActorPtr->trackNumber);

		trackPtr += currentProcessedActorPtr->positionInTrack * 2;

		const int16 trackMacro = *(int16 *)trackPtr;
		trackPtr += 2;

		// printf("Track macro %X\n",trackMacro);

		switch (trackMacro) {
		case TL_INIT_COOR: // warp
		{
			const int roomNumber = *(int16 *)trackPtr;
			trackPtr += 2;

			if (currentProcessedActorPtr->room != roomNumber) {
				if (currentCameraTargetActor == currentProcessedActorIdx) {
					needChangeRoom = 1;
					newRoom = roomNumber;
				}

				currentProcessedActorPtr->room = roomNumber;
			}

			currentProcessedActorPtr->zv.ZVX1 -= currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
			currentProcessedActorPtr->zv.ZVX2 -= currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
			currentProcessedActorPtr->zv.ZVY1 -= currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
			currentProcessedActorPtr->zv.ZVY2 -= currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
			currentProcessedActorPtr->zv.ZVZ1 -= currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;
			currentProcessedActorPtr->zv.ZVZ2 -= currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;

			currentProcessedActorPtr->worldX = currentProcessedActorPtr->roomX = *(int16 *)trackPtr;
			trackPtr += 2;
			currentProcessedActorPtr->worldY = currentProcessedActorPtr->roomY = *(int16 *)trackPtr;
			trackPtr += 2;
			currentProcessedActorPtr->worldZ = currentProcessedActorPtr->roomZ = *(int16 *)trackPtr;
			trackPtr += 2;

			currentProcessedActorPtr->worldX -= (int16)((roomDataTable[currentRoom].worldX - roomDataTable[currentProcessedActorPtr->room].worldX) * 10);
			currentProcessedActorPtr->worldY += (int16)((roomDataTable[currentRoom].worldY - roomDataTable[currentProcessedActorPtr->room].worldY) * 10);
			currentProcessedActorPtr->worldZ += (int16)((roomDataTable[currentRoom].worldZ - roomDataTable[currentProcessedActorPtr->room].worldZ) * 10);

			currentProcessedActorPtr->zv.ZVX1 += currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
			currentProcessedActorPtr->zv.ZVX2 += currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
			currentProcessedActorPtr->zv.ZVY1 += currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
			currentProcessedActorPtr->zv.ZVY2 += currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
			currentProcessedActorPtr->zv.ZVZ1 += currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;
			currentProcessedActorPtr->zv.ZVZ2 += currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;

			currentProcessedActorPtr->speed = 0;
			currentProcessedActorPtr->direction = 0;
			currentProcessedActorPtr->rotate.param = 0;
			currentProcessedActorPtr->positionInTrack += 5;

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

			if (roomNumber != currentProcessedActorPtr->room) {
				// TODO: fix bug here...
				x -= (roomDataTable[currentProcessedActorPtr->room].worldX - roomDataTable[roomNumber].worldX) * 10;
				z += (roomDataTable[currentProcessedActorPtr->room].worldZ - roomDataTable[roomNumber].worldZ) * 10;
			}

			const uint distanceToPoint = computeDistanceToPoint(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
																  currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
																  x, z);

			if (distanceToPoint >= DISTANCE_TO_POINT_TRESSHOLD) // not yet at position
			{
				const int angleModif = computeAngleModificatorToPosition(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
																   currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
																   currentProcessedActorPtr->beta,
																   x, z);

				if (currentProcessedActorPtr->rotate.param == 0 || currentProcessedActorPtr->direction != angleModif) {
					initRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - angleModif * 64, 15, &currentProcessedActorPtr->rotate);
				}

				currentProcessedActorPtr->direction = angleModif;

				if (!angleModif) {
					currentProcessedActorPtr->rotate.param = 0;
				} else {
					currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
				}
			} else // reached position
			{
				currentProcessedActorPtr->positionInTrack += 4;
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

			if (roomNumber != currentProcessedActorPtr->room) {
				// TODO: fix bug here...
				x -= (roomDataTable[currentProcessedActorPtr->room].worldX - roomDataTable[roomNumber].worldX) * 10;
				y += (roomDataTable[currentProcessedActorPtr->room].worldY - roomDataTable[roomNumber].worldY) * 10;
				z += (roomDataTable[currentProcessedActorPtr->room].worldZ - roomDataTable[roomNumber].worldZ) * 10;
			}

			// reached position?
			if (y == currentProcessedActorPtr->roomY && computeDistanceToPoint(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX, currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ, x, z) < DISTANCE_TO_POINT_TRESSHOLD) {
				currentProcessedActorPtr->positionInTrack += 6;
			} else {
				const int angleModif = computeAngleModificatorToPosition(
					currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
					currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
					currentProcessedActorPtr->beta,
					x, z);

				if (currentProcessedActorPtr->YHandler.param == 0) {
					initRealValue(0, y - (currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY), time, &currentProcessedActorPtr->YHandler);
				}

				if (currentProcessedActorPtr->rotate.param == 0 || currentProcessedActorPtr->direction != angleModif) {
					initRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - angleModif * 256, 60, &currentProcessedActorPtr->rotate);
				}

				currentProcessedActorPtr->direction = angleModif;

				if (!angleModif) {
					currentProcessedActorPtr->rotate.param = 0;
				} else {
					currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
				}
			}

			break;
		}
		case TL_END: // stop
		{
			currentProcessedActorPtr->speed = 0;
			currentProcessedActorPtr->trackNumber = -1;
			setMoveMode(0, 0);
			break;
		}
		case TL_REPEAT: {
			currentProcessedActorPtr->positionInTrack = 0;
			break;
		}
		case TL_MARK: // MARK
		{
			currentProcessedActorPtr->MARK = *(int16 *)trackPtr;
			trackPtr += 2;
			currentProcessedActorPtr->positionInTrack += 2;
			break;
		}
		case TL_WALK: {
			currentProcessedActorPtr->speed = 4;
			currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_RUN: {
			currentProcessedActorPtr->speed = 5;
			currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_STOP: {
			currentProcessedActorPtr->speed = 0;
			currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_SET_ANGLE: // TL_SET_ANGLE
		{
			const int betaDif = *(int16 *)trackPtr;
			trackPtr += 2;

			if ((currentProcessedActorPtr->beta - betaDif & 1023) > 512) {
				currentProcessedActorPtr->direction = 1; // left
			} else {
				currentProcessedActorPtr->direction = -1; // right
			}

			if (!currentProcessedActorPtr->rotate.param) {
				initRealValue(currentProcessedActorPtr->beta, betaDif, 120, &currentProcessedActorPtr->rotate);
			}

			currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);

			if (currentProcessedActorPtr->beta == betaDif) {
				currentProcessedActorPtr->direction = 0;

				currentProcessedActorPtr->positionInTrack += 2;
			}

			break;
		}
		case TL_COL_OFF: {
			currentProcessedActorPtr->dynFlags &= 0xFFFE;
			currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_COL_ON: {
			currentProcessedActorPtr->dynFlags |= 1;
			currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_DEC_OFF: // background collision off
		{
			currentProcessedActorPtr->_flags &= ~AF_TRIGGER;
			currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_DEC_ON: // background collision on
		{
			currentProcessedActorPtr->_flags |= AF_TRIGGER;
			currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case TL_MEMO_COOR: {
			const int objNum = currentProcessedActorPtr->indexInWorld;

			ListWorldObjets[objNum].x = currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
			ListWorldObjets[objNum].y = currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
			ListWorldObjets[objNum].z = currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;

			currentProcessedActorPtr->positionInTrack++;

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

			const int objX = ListWorldObjets[currentProcessedActorPtr->indexInWorld].x;
			const int objY = ListWorldObjets[currentProcessedActorPtr->indexInWorld].y;
			// objZ = ListWorldObjets[currentProcessedActorPtr->indexInWorld].z;

			if (currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY < y - 100 || currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY > y + 100) {
				const int propX = makeProportional(objY, y, x - objX, currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX - objX);

				const int difY = propX - currentProcessedActorPtr->worldY;

				currentProcessedActorPtr->worldY += difY;
				currentProcessedActorPtr->roomY += difY;
				currentProcessedActorPtr->zv.ZVY1 += difY;
				currentProcessedActorPtr->zv.ZVY2 += difY;

				const int angleModif = computeAngleModificatorToPosition(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
																   currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
																   currentProcessedActorPtr->beta,
																   x, z);

				if (!currentProcessedActorPtr->rotate.param || currentProcessedActorPtr->direction != angleModif) {
					initRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif << 8), 60, &currentProcessedActorPtr->rotate);
				}

				currentProcessedActorPtr->direction = angleModif;

				if (angleModif) {
					currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
				} else {
					currentProcessedActorPtr->rotate.param = 0;
				}

			} else {
				const int difY = y - currentProcessedActorPtr->worldY;

				currentProcessedActorPtr->stepY = 0;
				currentProcessedActorPtr->worldY += difY;
				currentProcessedActorPtr->roomY += difY;
				currentProcessedActorPtr->zv.ZVY1 += difY;
				currentProcessedActorPtr->zv.ZVY2 += difY;

				currentProcessedActorPtr->positionInTrack += 4;
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

			const int objY = ListWorldObjets[currentProcessedActorPtr->indexInWorld].y;
			const int objZ = ListWorldObjets[currentProcessedActorPtr->indexInWorld].z;

			if (currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY < y - 100 || currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY > y + 100) {
				const int propZ = makeProportional(objY, y, z - objZ, currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ - objZ);

				const int difY = propZ - currentProcessedActorPtr->worldY;

				currentProcessedActorPtr->worldY += difY;
				currentProcessedActorPtr->roomY += difY;
				currentProcessedActorPtr->zv.ZVY1 += difY;
				currentProcessedActorPtr->zv.ZVY2 += difY;

				const int angleModif = computeAngleModificatorToPosition(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
																   currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
																   currentProcessedActorPtr->beta,
																   x, z);

				if (!currentProcessedActorPtr->rotate.param || currentProcessedActorPtr->direction != angleModif) {
					initRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif << 8), 60, &currentProcessedActorPtr->rotate);
				}

				currentProcessedActorPtr->direction = angleModif;

				if (angleModif) {
					currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
				} else {
					currentProcessedActorPtr->rotate.param = 0;
				}

			} else {
				const int difY = y - currentProcessedActorPtr->worldY;

				currentProcessedActorPtr->stepY = 0;
				currentProcessedActorPtr->worldY += difY;
				currentProcessedActorPtr->roomY += difY;
				currentProcessedActorPtr->zv.ZVY1 += difY;
				currentProcessedActorPtr->zv.ZVY2 += difY;

				currentProcessedActorPtr->positionInTrack += 4;
			}

			break;
		}
		case TL_ANGLE: // rotate
		{
			currentProcessedActorPtr->alpha = *(int16 *)trackPtr;
			trackPtr += 2;
			currentProcessedActorPtr->beta = *(int16 *)trackPtr;
			trackPtr += 2;
			currentProcessedActorPtr->gamma = *(int16 *)trackPtr;
			trackPtr += 2;

			currentProcessedActorPtr->direction = 0;

			currentProcessedActorPtr->positionInTrack += 4;

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

	currentProcessedActorPtr->beta &= 0x3FF;
}

void processTrack2() {
	switch (currentProcessedActorPtr->trackMode) {
	case 1: // manual
	{
		gereManualRot(60);
		if (localJoyD & 1) // forward
		{
			if (timer - lastTimeForward < 10 && currentProcessedActorPtr->speed != 4)
				currentProcessedActorPtr->speed = 5;
			else if (currentProcessedActorPtr->speed == 0 || currentProcessedActorPtr->speed == -1)
				currentProcessedActorPtr->speed = 4;

			/*        if(currentProcessedActorPtr->speed>0 && currentProcessedActorPtr->speed<4)
			currentProcessedActorPtr->speed = 5; */

			lastTimeForward = timer;
		} else {
			if (currentProcessedActorPtr->speed > 0 && currentProcessedActorPtr->speed <= 4) {
				currentProcessedActorPtr->speed--;
			} else {
				currentProcessedActorPtr->speed = 0;
			}
		}

		if (localJoyD & 2) // backward
		{
			if (currentProcessedActorPtr->speed == 0 || currentProcessedActorPtr->speed >= 4)
				currentProcessedActorPtr->speed = -1;

			if (currentProcessedActorPtr->speed == 5)
				currentProcessedActorPtr->speed = 0;
		}

		break;
	}
	case 2: // follow
	{
		const int followedActorIdx = ListWorldObjets[currentProcessedActorPtr->trackNumber].objIndex;

		if (followedActorIdx == -1) {
			currentProcessedActorPtr->direction = 0;
			currentProcessedActorPtr->speed = 0;
		} else {
			const Object *followedActorPtr = &objectTable[followedActorIdx];

			const int roomNumber = followedActorPtr->room;
			const int x = followedActorPtr->roomX;
			// int y = followedActorPtr->roomY;
			const int z = followedActorPtr->roomZ;

			if (currentProcessedActorPtr->room != roomNumber) {
				/*  char* link = getRoomLink(currentProcessedActorPtr->room,roomNumber);

				x = *(int16*)(link)+(((*(int16*)(link+2))-(*(int16 *)(link))) / 2);
				y = *(int16*)(link+4)+(((*(int16*)(link+6))-(*(int16 *)(link+4))) / 2);
				z = *(int16*)(link+8)+(((*(int16*)(link+10))-(*(int16 *)(link+8))) / 2); */
			}

			const int angleModif = computeAngleModificatorToPosition(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
															   currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
															   currentProcessedActorPtr->beta, x, z);

			if (currentProcessedActorPtr->rotate.param == 0 || currentProcessedActorPtr->direction != angleModif) {
				initRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif << 8), 60, &currentProcessedActorPtr->rotate);
			}

			currentProcessedActorPtr->direction = angleModif;

			if (currentProcessedActorPtr->direction == 0) {
				currentProcessedActorPtr->rotate.param = 0;
			} else {
				currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
			}

			currentProcessedActorPtr->speed = 4;
		}
		break;
	}
	case 3: // track
	{
		byte *trackPtr = HQR_Get(listTrack, currentProcessedActorPtr->trackNumber);

		trackPtr += currentProcessedActorPtr->positionInTrack * 2;

		const int16 trackMacro = *(int16 *)trackPtr;
		trackPtr += 2;

		// printf("Track macro %X\n",trackMacro);

		switch (trackMacro) {
		case 0: // warp
		{
			const int roomNumber = *(int16 *)trackPtr;
			trackPtr += 2;

			if (currentProcessedActorPtr->room != roomNumber) {
				if (currentCameraTargetActor == currentProcessedActorIdx) {
					needChangeRoom = 1;
					newRoom = roomNumber;
				}

				currentProcessedActorPtr->room = roomNumber;
			}

			currentProcessedActorPtr->zv.ZVX1 -= currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
			currentProcessedActorPtr->zv.ZVX2 -= currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
			currentProcessedActorPtr->zv.ZVY1 -= currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
			currentProcessedActorPtr->zv.ZVY2 -= currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
			currentProcessedActorPtr->zv.ZVZ1 -= currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;
			currentProcessedActorPtr->zv.ZVZ2 -= currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;

			currentProcessedActorPtr->worldX = currentProcessedActorPtr->roomX = *(int16 *)trackPtr;
			trackPtr += 2;
			currentProcessedActorPtr->worldY = currentProcessedActorPtr->roomY = *(int16 *)trackPtr;
			trackPtr += 2;
			currentProcessedActorPtr->worldZ = currentProcessedActorPtr->roomZ = *(int16 *)trackPtr;
			trackPtr += 2;

			currentProcessedActorPtr->worldX -= (int16)((roomDataTable[currentRoom].worldX - roomDataTable[currentProcessedActorPtr->room].worldX) * 10);
			currentProcessedActorPtr->worldY += (int16)((roomDataTable[currentRoom].worldY - roomDataTable[currentProcessedActorPtr->room].worldY) * 10);
			currentProcessedActorPtr->worldZ += (int16)((roomDataTable[currentRoom].worldZ - roomDataTable[currentProcessedActorPtr->room].worldZ) * 10);

			currentProcessedActorPtr->zv.ZVX1 += currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
			currentProcessedActorPtr->zv.ZVX2 += currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
			currentProcessedActorPtr->zv.ZVY1 += currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
			currentProcessedActorPtr->zv.ZVY2 += currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
			currentProcessedActorPtr->zv.ZVZ1 += currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;
			currentProcessedActorPtr->zv.ZVZ2 += currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;

			currentProcessedActorPtr->speed = 0;
			currentProcessedActorPtr->direction = 0;
			currentProcessedActorPtr->rotate.param = 0;
			currentProcessedActorPtr->positionInTrack += 5;

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

			if (roomNumber != currentProcessedActorPtr->room) {
				x -= (roomDataTable[currentProcessedActorPtr->room].worldX - roomDataTable[roomNumber].worldX) * 10;
				z += (roomDataTable[currentProcessedActorPtr->room].worldZ - roomDataTable[roomNumber].worldZ) * 10;
			}

			const uint distanceToPoint = computeDistanceToPoint(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
																  currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
																  x, z);

			if (distanceToPoint >= DISTANCE_TO_POINT_TRESSHOLD) // not yet at position
			{
				const int angleModif = computeAngleModificatorToPosition(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
																   currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
																   currentProcessedActorPtr->beta,
																   x, z);

				if (currentProcessedActorPtr->rotate.param == 0 || currentProcessedActorPtr->direction != angleModif) {
					initRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif << 6), 15, &currentProcessedActorPtr->rotate);
				}

				currentProcessedActorPtr->direction = angleModif;

				if (!angleModif) {
					currentProcessedActorPtr->rotate.param = 0;
				} else {
					currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
				}
			} else // reached position
			{
				currentProcessedActorPtr->positionInTrack += 4;
			}

			break;
		}
		case 2: // stop
		{
			currentProcessedActorPtr->speed = 0;
			currentProcessedActorPtr->trackNumber = -1;
			setMoveMode(0, 0);
			break;
		}
		case 3: {
			currentProcessedActorPtr->positionInTrack = 0;
			break;
		}
		case 4: // MARK
		{
			currentProcessedActorPtr->MARK = *(int16 *)trackPtr;
			trackPtr += 2;
			currentProcessedActorPtr->positionInTrack += 2;
			break;
		}
		case 5: {
			break;
		}
		case 0x6: {
			const int betaDif = *(int16 *)trackPtr;
			trackPtr += 2;

			if ((currentProcessedActorPtr->beta - betaDif & 0x3FF) > 0x200) {
				currentProcessedActorPtr->direction = 1;
			} else {
				currentProcessedActorPtr->direction = -1;
			}

			if (!currentProcessedActorPtr->rotate.param) {
				initRealValue(currentProcessedActorPtr->beta, betaDif, 120, &currentProcessedActorPtr->rotate);
			}

			currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);

			if (currentProcessedActorPtr->beta == betaDif) {
				currentProcessedActorPtr->direction = 0;

				currentProcessedActorPtr->positionInTrack += 2;
			}

			break;
		}
		case 0x7: {
			currentProcessedActorPtr->dynFlags &= 0xFFFE;
			currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case 0x8: {
			currentProcessedActorPtr->dynFlags |= 1;
			currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case 0xA: {
			currentProcessedActorPtr->_flags &= 0xFFBF;
			currentProcessedActorPtr->positionInTrack++;
			break;
		}
		case 0xB: {
			currentProcessedActorPtr->_flags |= 0x40;
			currentProcessedActorPtr->positionInTrack++;
			break;
		} /*
		  case 0x10:
		  {
		  int objNum = currentProcessedActorPtr->field_0;

		  objectTable[objNum].x = currentProcessedActorPtr->roomX + currentProcessedActorPtr->modX;
		  objectTable[objNum].y = currentProcessedActorPtr->roomY + currentProcessedActorPtr->modY;
		  objectTable[objNum].z = currentProcessedActorPtr->roomZ + currentProcessedActorPtr->modZ;

		  currentProcessedActorPtr->positionInTrack++;

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

		  objX = objectTable[currentProcessedActorPtr->field_0].x;
		  objY = objectTable[currentProcessedActorPtr->field_0].y;
		  objZ = objectTable[currentProcessedActorPtr->field_0].z;

		  if(   currentProcessedActorPtr->roomY + currentProcessedActorPtr->modY < y - 100
		  ||  currentProcessedActorPtr->roomY + currentProcessedActorPtr->modY > y + 100)
		  {
		  int propX = makeProportional(objY, y, x - objX, (currentProcessedActorPtr->roomX + currentProcessedActorPtr->modX) - objX);

		  int difY = propX - currentProcessedActorPtr->worldY;
		  int angleModif;

		  currentProcessedActorPtr->worldY += difY;
		  currentProcessedActorPtr->roomY += difY;
		  currentProcessedActorPtr->zv.ZVY1 += difY;
		  currentProcessedActorPtr->zv.ZVY2 += difY;

		  angleModif = computeAngleModificatorToPosition( currentProcessedActorPtr->roomX + currentProcessedActorPtr->modX,
		  currentProcessedActorPtr->roomZ + currentProcessedActorPtr->modZ,
		  currentProcessedActorPtr->beta,
		  x,z );

		  if(!currentProcessedActorPtr->rotate.param || currentProcessedActorPtr->field_72 != angleModif)
		  {
		  startActorRotation(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif<<8), 60, &currentProcessedActorPtr->rotate);
		  }

		  currentProcessedActorPtr->field_72 = angleModif;

		  if(angleModif)
		  {
		  currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
		  }
		  else
		  {
		  currentProcessedActorPtr->rotate.param = 0;
		  }

		  }
		  else
		  {
		  int difY = y - currentProcessedActorPtr->worldY;

		  currentProcessedActorPtr->modY = 0;
		  currentProcessedActorPtr->worldY += difY;
		  currentProcessedActorPtr->roomY += difY;
		  currentProcessedActorPtr->zv.ZVY1 += difY;
		  currentProcessedActorPtr->zv.ZVY2 += difY;

		  currentProcessedActorPtr->positionInTrack +=4;
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

		  objX = objectTable[currentProcessedActorPtr->field_0].x;
		  objY = objectTable[currentProcessedActorPtr->field_0].y;
		  objZ = objectTable[currentProcessedActorPtr->field_0].z;

		  if(   currentProcessedActorPtr->roomY + currentProcessedActorPtr->modY < y - 100
		  ||  currentProcessedActorPtr->roomY + currentProcessedActorPtr->modY > y + 100)
		  {
		  int propZ = makeProportional(objY, y, z - objZ, (currentProcessedActorPtr->roomZ + currentProcessedActorPtr->modZ) - objZ);

		  int difY = propZ - currentProcessedActorPtr->worldY;

		  int angleModif;

		  currentProcessedActorPtr->worldY += difY;
		  currentProcessedActorPtr->roomY += difY;
		  currentProcessedActorPtr->zv.ZVY1 += difY;
		  currentProcessedActorPtr->zv.ZVY2 += difY;

		  angleModif = computeAngleModificatorToPosition( currentProcessedActorPtr->roomX + currentProcessedActorPtr->modX,
		  currentProcessedActorPtr->roomZ + currentProcessedActorPtr->modZ,
		  currentProcessedActorPtr->beta,
		  x,z );

		  if(!currentProcessedActorPtr->rotate.param || currentProcessedActorPtr->field_72 != angleModif)
		  {
		  startActorRotation(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif<<8), 60, &currentProcessedActorPtr->rotate);
		  }

		  currentProcessedActorPtr->field_72 = angleModif;

		  if(angleModif)
		  {
		  currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
		  }
		  else
		  {
		  currentProcessedActorPtr->rotate.param = 0;
		  }

		  }
		  else
		  {
		  int difY = y - currentProcessedActorPtr->worldY;

		  currentProcessedActorPtr->modY = 0;
		  currentProcessedActorPtr->worldY += difY;
		  currentProcessedActorPtr->roomY += difY;
		  currentProcessedActorPtr->zv.ZVY1 += difY;
		  currentProcessedActorPtr->zv.ZVY2 += difY;

		  currentProcessedActorPtr->positionInTrack +=4;
		  }

		  break;
		  }
		  case 0x13: // rotate
		  {
		  currentProcessedActorPtr->alpha = *(int16*)(trackPtr);
		  trackPtr += 2;
		  currentProcessedActorPtr->beta = *(int16*)(trackPtr);
		  trackPtr += 2;
		  currentProcessedActorPtr->gamma = *(int16*)(trackPtr);
		  trackPtr += 2;

		  currentProcessedActorPtr->field_72 = 0;

		  currentProcessedActorPtr->positionInTrack +=4;

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

	currentProcessedActorPtr->beta &= 0x3FF;
}
} // namespace Fitd
