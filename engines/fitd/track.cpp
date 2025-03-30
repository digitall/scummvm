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
#include "common/debug.h"

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
	return x1 + ((x2 - x1) * y2) / y1;
}

int computeAngleModificatorToPositionSub1(int ax) {
	int xOut;
	int yOut;

	rotate(ax, 0, 1000, &xOut, &yOut);

	yOut *= angleCompZ;
	xOut *= angleCompX;

	yOut -= xOut;

	if (yOut == 0)
		return (0);

	if (yOut > 0)
		return (1);
	else
		return (-1);
}

int computeAngleModificatorToPosition(int x1, int z1, int beta, int x2, int z2) {
	int resultMin;
	int resultMax;

	angleCompX = x2 - x1;
	angleCompZ = z2 - z1;
	angleCompBeta = beta;

	resultMin = computeAngleModificatorToPositionSub1(beta - 4);
	resultMax = computeAngleModificatorToPositionSub1(beta + 4);

	if (resultMax == -1 && resultMin == 1) // in the middle
	{
		return (computeAngleModificatorToPositionSub1(beta));
	} else {
		return (((resultMax + resultMin) + 1) >> 1);
	}
}

void GereManualRot(int param) {
	if (localJoyD & 4) {
		if (currentProcessedActorPtr->direction != 1) {
			currentProcessedActorPtr->rotate.param = 0;
		}

		currentProcessedActorPtr->direction = 1;

		if (currentProcessedActorPtr->rotate.param == 0) {
			int oldBeta = currentProcessedActorPtr->beta;

			if (currentProcessedActorPtr->speed == 0) {
				InitRealValue(oldBeta, oldBeta + 0x100, param / 2, &currentProcessedActorPtr->rotate);
			} else {
				InitRealValue(oldBeta, oldBeta + 0x100, param, &currentProcessedActorPtr->rotate);
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
			int oldBeta = currentProcessedActorPtr->beta;

			if (currentProcessedActorPtr->speed == 0) {
				InitRealValue(oldBeta, oldBeta - 0x100, param / 2, &currentProcessedActorPtr->rotate);
			} else {
				InitRealValue(oldBeta, oldBeta - 0x100, param, &currentProcessedActorPtr->rotate);
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

unsigned int lastTimeForward = 0;

char *getRoomLink(unsigned int room1, unsigned int room2) {
	int i;
	int16 numOfZones;
	char *zoneData = (char *)getRoomData(room1);
	char *bestZone;

	zoneData += *(int16 *)(zoneData);
	numOfZones = *(int16 *)zoneData;
	zoneData += 2;

	bestZone = zoneData;

	for (i = 0; i < numOfZones; i++) {
		if (*(int16 *)(zoneData + 14) == 4) {
			bestZone = zoneData;

			if (*(int16 *)(zoneData + 12) == room2) {
				return bestZone;
			}
		}

		zoneData += 16;
	}

	return bestZone;
}

void processTrack(void) {
	switch (currentProcessedActorPtr->trackMode) {
	case 1: // manual
	{
		GereManualRot(60);
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
			if ((currentProcessedActorPtr->speed > 0) && (currentProcessedActorPtr->speed <= 4)) {
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
		int followedActorIdx = ListWorldObjets[currentProcessedActorPtr->trackNumber].objIndex;

		if (followedActorIdx == -1) {
			currentProcessedActorPtr->direction = 0;
			currentProcessedActorPtr->speed = 0;
		} else {
			tObject *followedActorPtr = &objectTable[followedActorIdx];

			int roomNumber = followedActorPtr->room;
			int x = followedActorPtr->roomX;
			int y = followedActorPtr->roomY;
			int z = followedActorPtr->roomZ;
			int angleModif;

			if (currentProcessedActorPtr->room != roomNumber) {
				char *link = getRoomLink(currentProcessedActorPtr->room, roomNumber);

				x = *(int16 *)(link) + (((*(int16 *)(link + 2)) - (*(int16 *)(link))) / 2);
				y = *(int16 *)(link + 4) + (((*(int16 *)(link + 6)) - (*(int16 *)(link + 4))) / 2);
				z = *(int16 *)(link + 8) + (((*(int16 *)(link + 10)) - (*(int16 *)(link + 8))) / 2);
			}

			angleModif = computeAngleModificatorToPosition(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
														   currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
														   currentProcessedActorPtr->beta, x, z);

			if (currentProcessedActorPtr->rotate.param == 0 || currentProcessedActorPtr->direction != angleModif) {
				InitRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif << 8), 60, &currentProcessedActorPtr->rotate);
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
		char *trackPtr = HQR_Get(listTrack, currentProcessedActorPtr->trackNumber);
		int16 trackMacro;

		trackPtr += currentProcessedActorPtr->positionInTrack * 2;

		trackMacro = *(int16 *)trackPtr;
		trackPtr += 2;

		// printf("Track macro %X\n",trackMacro);

		switch (trackMacro) {
		case TL_INIT_COOR: // warp
		{
			int roomNumber = *(int16 *)(trackPtr);
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

			currentProcessedActorPtr->worldX = currentProcessedActorPtr->roomX = *(int16 *)(trackPtr);
			trackPtr += 2;
			currentProcessedActorPtr->worldY = currentProcessedActorPtr->roomY = *(int16 *)(trackPtr);
			trackPtr += 2;
			currentProcessedActorPtr->worldZ = currentProcessedActorPtr->roomZ = *(int16 *)(trackPtr);
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
			int roomNumber = *(int16 *)(trackPtr);
			int x;
			int y;
			int z;
			unsigned int distanceToPoint;

			trackPtr += 2;

			x = *(int16 *)(trackPtr);
			trackPtr += 2;
			y = 0;
			z = *(int16 *)(trackPtr);
			trackPtr += 2;

			if (roomNumber != currentProcessedActorPtr->room) {
				// TODO: fix bug here...
				x -= (roomDataTable[currentProcessedActorPtr->room].worldX - roomDataTable[roomNumber].worldX) * 10;
				z += (roomDataTable[currentProcessedActorPtr->room].worldZ - roomDataTable[roomNumber].worldZ) * 10;
			}

			distanceToPoint = computeDistanceToPoint(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
													 currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
													 x, z);

			if (distanceToPoint >= DISTANCE_TO_POINT_TRESSHOLD) // not yet at position
			{
				int angleModif = computeAngleModificatorToPosition(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
																   currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
																   currentProcessedActorPtr->beta,
																   x, z);

				if ((currentProcessedActorPtr->rotate.param == 0) || (currentProcessedActorPtr->direction != angleModif)) {
					InitRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif * 64), 15, &currentProcessedActorPtr->rotate);
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
			int roomNumber = *(int16 *)(trackPtr);
			int x;
			int y;
			int z;
			unsigned int distanceToPoint;

			trackPtr += 2;

			x = *(int16 *)(trackPtr);
			trackPtr += 2;
			y = *(int16 *)(trackPtr);
			trackPtr += 2;
			z = *(int16 *)(trackPtr);
			trackPtr += 2;
			int time = *(int16 *)(trackPtr);
			trackPtr += 2;

			if (roomNumber != currentProcessedActorPtr->room) {
				// TODO: fix bug here...
				x -= (roomDataTable[currentProcessedActorPtr->room].worldX - roomDataTable[roomNumber].worldX) * 10;
				y += (roomDataTable[currentProcessedActorPtr->room].worldY - roomDataTable[roomNumber].worldY) * 10;
				z += (roomDataTable[currentProcessedActorPtr->room].worldZ - roomDataTable[roomNumber].worldZ) * 10;
			}

			// reached position?
			if ((y == currentProcessedActorPtr->roomY) && (computeDistanceToPoint(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX, currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ, x, z) < DISTANCE_TO_POINT_TRESSHOLD)) {
				currentProcessedActorPtr->positionInTrack += 6;
			} else {
				int angleModif = computeAngleModificatorToPosition(
					currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
					currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
					currentProcessedActorPtr->beta,
					x, z);

				if (currentProcessedActorPtr->YHandler.param == 0) {
					InitRealValue(0, y - (currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY), time, &currentProcessedActorPtr->YHandler);
				}

				if ((currentProcessedActorPtr->rotate.param == 0) || (currentProcessedActorPtr->direction != angleModif)) {
					InitRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif * 256), 60, &currentProcessedActorPtr->rotate);
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
			currentProcessedActorPtr->MARK = *(int16 *)(trackPtr);
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
			int betaDif = *(int16 *)(trackPtr);
			trackPtr += 2;

			if (((currentProcessedActorPtr->beta - betaDif) & 1023) > 512) {
				currentProcessedActorPtr->direction = 1; // left
			} else {
				currentProcessedActorPtr->direction = -1; // right
			}

			if (!currentProcessedActorPtr->rotate.param) {
				InitRealValue(currentProcessedActorPtr->beta, betaDif, 120, &currentProcessedActorPtr->rotate);
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
			int objNum = currentProcessedActorPtr->indexInWorld;

			ListWorldObjets[objNum].x = currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
			ListWorldObjets[objNum].y = currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
			ListWorldObjets[objNum].z = currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;

			currentProcessedActorPtr->positionInTrack++;

			break;
		}
		case TL_GOTO_3DX: // walk up/down stairs on X
		{
			int x;
			int y;
			int z;
			int objX;
			int objY;
			int objZ;

			x = *(int16 *)(trackPtr);
			trackPtr += 2;
			y = *(int16 *)(trackPtr);
			trackPtr += 2;
			z = *(int16 *)(trackPtr);
			trackPtr += 2;

			objX = ListWorldObjets[currentProcessedActorPtr->indexInWorld].x;
			objY = ListWorldObjets[currentProcessedActorPtr->indexInWorld].y;
			objZ = ListWorldObjets[currentProcessedActorPtr->indexInWorld].z;

			if (currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY < y - 100 || currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY > y + 100) {
				int propX = makeProportional(objY, y, x - objX, (currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX) - objX);

				int difY = propX - currentProcessedActorPtr->worldY;
				int angleModif;

				currentProcessedActorPtr->worldY += difY;
				currentProcessedActorPtr->roomY += difY;
				currentProcessedActorPtr->zv.ZVY1 += difY;
				currentProcessedActorPtr->zv.ZVY2 += difY;

				angleModif = computeAngleModificatorToPosition(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
															   currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
															   currentProcessedActorPtr->beta,
															   x, z);

				if (!currentProcessedActorPtr->rotate.param || currentProcessedActorPtr->direction != angleModif) {
					InitRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif << 8), 60, &currentProcessedActorPtr->rotate);
				}

				currentProcessedActorPtr->direction = angleModif;

				if (angleModif) {
					currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
				} else {
					currentProcessedActorPtr->rotate.param = 0;
				}

			} else {
				int difY = y - currentProcessedActorPtr->worldY;

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
			int x;
			int y;
			int z;
			int objX;
			int objY;
			int objZ;

			x = *(int16 *)(trackPtr);
			trackPtr += 2;
			y = *(int16 *)(trackPtr);
			trackPtr += 2;
			z = *(int16 *)(trackPtr);
			trackPtr += 2;

			objX = ListWorldObjets[currentProcessedActorPtr->indexInWorld].x;
			objY = ListWorldObjets[currentProcessedActorPtr->indexInWorld].y;
			objZ = ListWorldObjets[currentProcessedActorPtr->indexInWorld].z;

			if (currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY < y - 100 || currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY > y + 100) {
				int propZ = makeProportional(objY, y, z - objZ, (currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ) - objZ);

				int difY = propZ - currentProcessedActorPtr->worldY;

				int angleModif;

				currentProcessedActorPtr->worldY += difY;
				currentProcessedActorPtr->roomY += difY;
				currentProcessedActorPtr->zv.ZVY1 += difY;
				currentProcessedActorPtr->zv.ZVY2 += difY;

				angleModif = computeAngleModificatorToPosition(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
															   currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
															   currentProcessedActorPtr->beta,
															   x, z);

				if (!currentProcessedActorPtr->rotate.param || currentProcessedActorPtr->direction != angleModif) {
					InitRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif << 8), 60, &currentProcessedActorPtr->rotate);
				}

				currentProcessedActorPtr->direction = angleModif;

				if (angleModif) {
					currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
				} else {
					currentProcessedActorPtr->rotate.param = 0;
				}

			} else {
				int difY = y - currentProcessedActorPtr->worldY;

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
			currentProcessedActorPtr->alpha = *(int16 *)(trackPtr);
			trackPtr += 2;
			currentProcessedActorPtr->beta = *(int16 *)(trackPtr);
			trackPtr += 2;
			currentProcessedActorPtr->gamma = *(int16 *)(trackPtr);
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

void processTrack2(void) {
	switch (currentProcessedActorPtr->trackMode) {
	case 1: // manual
	{
		GereManualRot(60);
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
		int followedActorIdx = ListWorldObjets[currentProcessedActorPtr->trackNumber].objIndex;

		if (followedActorIdx == -1) {
			currentProcessedActorPtr->direction = 0;
			currentProcessedActorPtr->speed = 0;
		} else {
			tObject *followedActorPtr = &objectTable[followedActorIdx];

			int roomNumber = followedActorPtr->room;
			int x = followedActorPtr->roomX;
			// int y = followedActorPtr->roomY;
			int z = followedActorPtr->roomZ;
			int angleModif;

			if (currentProcessedActorPtr->room != roomNumber) {
				/*  char* link = getRoomLink(currentProcessedActorPtr->room,roomNumber);

				x = *(int16*)(link)+(((*(int16*)(link+2))-(*(int16 *)(link))) / 2);
				y = *(int16*)(link+4)+(((*(int16*)(link+6))-(*(int16 *)(link+4))) / 2);
				z = *(int16*)(link+8)+(((*(int16*)(link+10))-(*(int16 *)(link+8))) / 2); */
			}

			angleModif = computeAngleModificatorToPosition(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
														   currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
														   currentProcessedActorPtr->beta, x, z);

			if (currentProcessedActorPtr->rotate.param == 0 || currentProcessedActorPtr->direction != angleModif) {
				InitRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif << 8), 60, &currentProcessedActorPtr->rotate);
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
		char *trackPtr = HQR_Get(listTrack, currentProcessedActorPtr->trackNumber);
		int16 trackMacro;

		trackPtr += currentProcessedActorPtr->positionInTrack * 2;

		trackMacro = *(int16 *)trackPtr;
		trackPtr += 2;

		// printf("Track macro %X\n",trackMacro);

		switch (trackMacro) {
		case 0: // warp
		{
			int roomNumber = *(int16 *)(trackPtr);
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

			currentProcessedActorPtr->worldX = currentProcessedActorPtr->roomX = *(int16 *)(trackPtr);
			trackPtr += 2;
			currentProcessedActorPtr->worldY = currentProcessedActorPtr->roomY = *(int16 *)(trackPtr);
			trackPtr += 2;
			currentProcessedActorPtr->worldZ = currentProcessedActorPtr->roomZ = *(int16 *)(trackPtr);
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
			int roomNumber = *(int16 *)(trackPtr);
			int x;
			int y;
			int z;
			unsigned int distanceToPoint;

			trackPtr += 2;

			x = *(int16 *)(trackPtr);
			trackPtr += 2;
			y = 0;
			z = *(int16 *)(trackPtr);
			trackPtr += 2;

			if (roomNumber != currentProcessedActorPtr->room) {
				x -= (roomDataTable[currentProcessedActorPtr->room].worldX - roomDataTable[roomNumber].worldX) * 10;
				z += (roomDataTable[currentProcessedActorPtr->room].worldZ - roomDataTable[roomNumber].worldZ) * 10;
			}

			distanceToPoint = computeDistanceToPoint(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
													 currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
													 x, z);

			if (distanceToPoint >= DISTANCE_TO_POINT_TRESSHOLD) // not yet at position
			{
				int angleModif = computeAngleModificatorToPosition(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
																   currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
																   currentProcessedActorPtr->beta,
																   x, z);

				if (currentProcessedActorPtr->rotate.param == 0 || currentProcessedActorPtr->direction != angleModif) {
					InitRealValue(currentProcessedActorPtr->beta, currentProcessedActorPtr->beta - (angleModif << 6), 15, &currentProcessedActorPtr->rotate);
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
			currentProcessedActorPtr->MARK = *(int16 *)(trackPtr);
			trackPtr += 2;
			currentProcessedActorPtr->positionInTrack += 2;
			break;
		}
		case 5: {
			break;
		}
		case 0x6: {
			int betaDif = *(int16 *)(trackPtr);
			trackPtr += 2;

			if (((currentProcessedActorPtr->beta - betaDif) & 0x3FF) > 0x200) {
				currentProcessedActorPtr->direction = 1;
			} else {
				currentProcessedActorPtr->direction = -1;
			}

			if (!currentProcessedActorPtr->rotate.param) {
				InitRealValue(currentProcessedActorPtr->beta, betaDif, 120, &currentProcessedActorPtr->rotate);
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
