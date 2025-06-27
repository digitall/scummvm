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

#include "fitd/eval_var.h"
#include "common/debug.h"
#include "fitd/common.h"
#include "fitd/fitd.h"
#include "fitd/floor.h"
#include "fitd/hqr.h"
#include "fitd/inventory.h"
#include "fitd/room.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"

namespace Fitd {

int getPosRelTable[] = {4, 1, 8, 2, 4, 1, 8, 0};

int getMatrix(int param1, int actorIdx, int param2) {
	byte *matrixPtr = (byte *)HQR_Get(listMatrix, param1);

	const int matrixWidth = *matrixPtr++;
	(void)*matrixPtr++;

	matrixPtr += (objectTable[actorIdx].hardMat - 1) * matrixWidth;
	matrixPtr += objectTable[param2].hardMat - 1;

	if (g_engine->getGameId() == GID_AITD3) {
		return *matrixPtr;
	}
	return *(char *)matrixPtr;
}

int getPosRel(const Object *actor1, const Object *actor2) {
	const int beta1 = actor1->beta;
	int counter = 3;
	ZVStruct localZv;

	if (beta1 >= 0x80 && beta1 < 0x180) {
		counter = 2;
	}

	if (beta1 >= 0x180 && beta1 < 0x280) {
		counter = 1;
	}

	if (beta1 >= 0x280 && beta1 < 0x380) {
		counter = 0;
	}

	copyZv(&actor2->zv, &localZv);

	if (actor1->room != actor2->room) {
		getZvRelativePosition(&localZv, actor2->room, actor1->room);
	}

	const int centerX = (localZv.ZVX1 + localZv.ZVX2) / 2;
	const int centerZ = (localZv.ZVZ1 + localZv.ZVZ2) / 2;

	if (actor1->zv.ZVZ2 >= centerZ && actor1->zv.ZVZ1 <= centerZ) {
		if (actor1->zv.ZVX2 < centerX) {
			counter++;
		} else {
			if (actor1->zv.ZVX1 <= centerX) {
				return 0;
			} else {
				counter += 3;
			}
		}
	} else if (actor1->zv.ZVX2 >= centerX || actor1->zv.ZVX1 <= centerX) {
		if (actor1->zv.ZVZ2 < centerZ) {
			counter += 2;
		} else {
			if (actor1->zv.ZVZ1 <= centerZ) {
				return 0;
			}
		}
	} else {
		return 0;
	}

	return getPosRelTable[counter];
}

int calcDist(int X1, int Y1, int Z1, int X2, int Y2, int Z2) {
	const int Xdist = abs(X1 - X2);
	const int Ydist = abs(Y1 - Y2);
	const int Zdist = abs(Z1 - Z2);

	return Xdist + Ydist + Zdist; // recheck overflow
}

int testZvEndAnim(const Object *actorPtr, char *animPtr, int param) {
	int16 var_E = 0;
	// int16 var_12 = 0;
	const int16 var_10 = param;
	ZVStruct localZv;

	assert(actorPtr);
	assert(animPtr);

	const int16 var_16 = *(int16 *)animPtr;
	animPtr += 2;
	const int16 var_14 = *(int16 *)animPtr;
	animPtr += 2;

	for (int16 var_18 = 0; var_18 < var_16; var_18++) {
		animPtr += 2;
		// var_12 += *(int16 *)animPtr;
		animPtr += 2;
		animPtr += 2;
		var_E += *(int16 *)animPtr; // step depth
		animPtr += 2;

		animPtr += var_14 * 8;
	}

	copyZv(&actorPtr->zv, &localZv);

	walkStep(0, var_E, actorPtr->beta);

	localZv.ZVX1 += animMoveX;
	localZv.ZVX2 += animMoveX;
	localZv.ZVY1 += var_10;
	localZv.ZVY2 += var_10;
	localZv.ZVZ1 += animMoveZ;
	localZv.ZVZ2 += animMoveZ;

	if (asmCheckListCol(&localZv, &roomDataTable[actorPtr->room])) {
		return 0;
	}

	localZv.ZVY1 += 100;
	localZv.ZVY2 += 100;

	if (asmCheckListCol(&localZv, &roomDataTable[actorPtr->room])) {
		return 1;
	}

	return 0;
}

int evalVar(const char *name) {

	if (g_engine->getGameId() >= GID_JACK) {
		return evalVar2(name);
	}
	int var1 = *(int16 *)currentLifePtr;
	currentLifePtr += 2;

	if (var1 == -1) {
		const int temp = *(int16 *)currentLifePtr;
		currentLifePtr += 2;

		return temp;
	} else if (var1 == 0) {
		const int temp = *(int16 *)currentLifePtr;
		currentLifePtr += 2;

		return vars[temp];
	} else {
		Object *actorPtr = currentLifeActorPtr;

		if (var1 & 0x8000) {

			const int objectNumber = *(int16 *)currentLifePtr;

			int actorIdx = ListWorldObjets[objectNumber].objIndex;

			currentLifePtr += 2;
			actorPtr = &objectTable[actorIdx];

			if (actorIdx == -1) {
				switch (var1 & 0x7FFF) {
				case 0x1F: {
					return ListWorldObjets[objectNumber].room;
					break;
				}
				case 0x26: {
					return ListWorldObjets[objectNumber].stage;
					break;
				}
				default: {
					error("Unsupported evalVar %X when actor not in room !\n", var1 & 0x7FFF);
					assert(0);
				}
				}
			}
		}
		{

			var1 &= 0x7FFF;

			var1--;

			switch (var1) {
			case 0x0: {
				const int temp1 = actorPtr->COL[0];

				if (temp1 != -1) {
					return objectTable[temp1].indexInWorld;
				}
				return -1;
			}
			case 0x1: {
				return actorPtr->HARD_DEC;
			}
			case 0x2: {
				return actorPtr->HARD_COL;
			}
			case 0x3: {
				const int temp = actorPtr->HIT;

				if (temp == -1) {
					return -1;
				}
				return objectTable[temp].indexInWorld;
			}
			case 0x4: {
				const int temp = actorPtr->HIT_BY;

				if (temp == -1) {
					return -1;
				}
				return objectTable[temp].indexInWorld;
			}
			case 0x5: {
				return actorPtr->ANIM;
			}
			case 0x6: {
				return actorPtr->END_ANIM;
			}
			case 0x7: {
				return actorPtr->FRAME;
			}
			case 0x8: {
				return actorPtr->END_FRAME;
			}
			case 0x9: {
				return actorPtr->bodyNum;
			}
			case 0xA: // MARK
			{
				return actorPtr->MARK;
			}
			case 0xB: // NUM_TRACK
			{
				return actorPtr->trackNumber;
			}
			case 0xC: // CHRONO
			{
				return evalChrono(&actorPtr->CHRONO) / 60; // recheck
			}
			case 0xD: {
				return evalChrono(&actorPtr->ROOM_CHRONO) / 60; // recheck
			}
			case 0xE: // DIST
			{
				const int actorNumber = ListWorldObjets[*(int16 *)currentLifePtr].objIndex;
				currentLifePtr += 2;

				if (actorNumber == -1) {
					return 32000;
				}
				const int tempX = objectTable[actorNumber].worldX;
				const int tempY = objectTable[actorNumber].worldY;
				const int tempZ = objectTable[actorNumber].worldZ;

				return calcDist(actorPtr->worldX, actorPtr->worldY, actorPtr->worldZ, tempX, tempY, tempZ);
			}
			case 0xF: // COL_BY
			{
				if (actorPtr->COL_BY == -1)
					return -1;
				return objectTable[actorPtr->COL_BY].indexInWorld;
			}
			case 0x10: // found
			{
				if (ListWorldObjets[evalVar()].flags2 & 0x8000) {
					return 1;
				}
				return 0;
			}
			case 0x11: {
				return action;
			}
			case 0x12: // POSREL
			{

				const int objNum = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				if (ListWorldObjets[objNum].objIndex == -1) {
					return 0;
				}

				return getPosRel(actorPtr, &objectTable[ListWorldObjets[objNum].objIndex]);
			}
			case 0x13: {
				if (localJoyD & 4)
					return 4;
				if (localJoyD & 8)
					return 8;
				if (localJoyD & 1)
					return 1;
				if (localJoyD & 2)
					return 2;

				return 0;
			}
			case 0x14: {
				return localClick;
			}
			case 0x15: {
				int temp1 = actorPtr->COL[0];
				if (temp1 == -1) {
					temp1 = actorPtr->COL_BY;
					if (temp1 == -1)
						return -1;
				}

				return objectTable[temp1].indexInWorld;
			}
			case 0x16: {
				return actorPtr->alpha;
			}
			case 0x17: {
				return actorPtr->beta;
			}
			case 0x18: {
				return actorPtr->gamma;
			}
			case 0x19: {
				return inHandTable[currentInventory];
			}
			case 0x1A: {
				return actorPtr->hitForce;
			}
			case 0x1B: {
				return *(uint16 *)((currentCamera + 6) * 2 + cameraPtr);
			}
			case 0x1C: {
				const int temp = *(int16 *)currentLifePtr;
				assert(temp > 0);
				currentLifePtr += 2;
				return g_engine->getRandomNumber(temp - 1);
			}
			case 0x1D: {
				return actorPtr->falling;
			}
			case 0x1E: {
				return actorPtr->room;
			}
			case 0x1F: {
				return actorPtr->life;
			}
			case 0x20: {

				const int objNum = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				if (ListWorldObjets[objNum].flags2 & 0xC000) {
					return 1;
				}
				return 0;
			}
			case 0x21: {
				return actorPtr->roomY;
			}
			case 0x22: // TEST_ZV_END_ANIM
			{

				const int temp1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				const int temp2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				return testZvEndAnim(actorPtr, HQR_Get(listAnim, temp1), temp2);
			}
			case 0x23: // TODO: music
			{
				return currentMusic;
			}
			case 0x24: {
				const int temp = CVars[*(int16 *)currentLifePtr];
				currentLifePtr += 2;
				return temp;
			}
			case 0x25: {
				return actorPtr->stage;
			}
			case 0x26: // THROW
			{

				const int objNum = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				if (ListWorldObjets[objNum].flags2 & 0x1000) {
					return 1;
				}
				return 0;
			}
			default: {
				error("Unhandled test type %X in evalVar\n", var1);
				assert(0);
			}
			}
		}
	}
}

int evalVar2(const char *name) {

	int var1 = *(int16 *)currentLifePtr;
	currentLifePtr += 2;

	if (var1 == -1) {
		const int temp = *(int16 *)currentLifePtr;
		currentLifePtr += 2;

		// if (name)
		// 	appendFormated("%s:", name);
		// appendFormated("%d, ", temp);

		return temp;
	}
	if (var1 == 0) {
		const int temp = *(int16 *)currentLifePtr;
		currentLifePtr += 2;

		// if (name)
		// 	appendFormated("%s:", name);
		// appendFormated("vars[%d], ", temp);

		return vars[temp];
	}
	Object *actorPtr = currentLifeActorPtr;
	int actorIdx = currentLifeActorIdx;

	if (var1 & 0x8000) {

		const int objectNumber = *(int16 *)currentLifePtr;

		actorIdx = ListWorldObjets[objectNumber].objIndex;

		currentLifePtr += 2;
		actorPtr = &objectTable[actorIdx];

		if (actorIdx == -1) {
			switch (var1 & 0x7FFF) {
			case 0x1F: {
				// if (name)
				// 	appendFormated("%s:", name);
				// appendFormated("worldObjects[%d].room, ", objectNumber);

				return ListWorldObjets[objectNumber].room;
				break;
			}
			case 0x24: {
				// if (name)
				// 	appendFormated("%s:", name);
				// appendFormated("worldObjects[%d].stage, ", objectNumber);

				return ListWorldObjets[objectNumber].stage;
				break;
			}
			default: {
				debug("Unsupported evalVar2 %X when actor not in room !\n", var1 & 0x7FFF);
				// assert(0);
				return false;
			}
			}
		}
	}
	{

		var1 &= 0x7FFF;

		var1--;

		switch (var1) {
		case 0x0: {
			const int temp1 = actorPtr->COL[0];

			// if (name)
			// 	appendFormated("%s:", name);
			// appendFormated("objectTable[%d].COL, ", temp1);

			if (temp1 != -1) {
				return objectTable[temp1].indexInWorld;
			}
			return -1;
		}
		case 0x1: {
			// if (name)
			// 	appendFormated("%s:", name);
			// appendFormated("HARD_DEC, ");
			return actorPtr->HARD_DEC;
		}
		case 0x2: {
			// if (name)
			// 	appendFormated("%s:", name);
			// appendFormated("HARD_COL, ");

			return actorPtr->HARD_COL;
		}
		case 0x3: {
			// if (name)
			// 	appendFormated("%s:", name);
			// appendFormated("HIT, ");
			const int temp = actorPtr->HIT;

			if (temp == -1) {
				return -1;
			}
			return objectTable[temp].indexInWorld;
		}
		case 0x4: {
			const int temp = actorPtr->HIT_BY;

			if (temp == -1) {
				return -1;
			} else {
				return objectTable[temp].indexInWorld;
			}

		}
		case 0x5: {
			return actorPtr->ANIM;
		}
		case 0x6: {
			return actorPtr->END_ANIM;
		}
		case 0x7: {
			return actorPtr->FRAME;
		}
		case 0x8: {
			return actorPtr->END_FRAME;
		}
		case 0x9: {
			return actorPtr->bodyNum;
		}
		case 0xA: // MARK
		{
			return actorPtr->MARK;
		}
		case 0xB: // NUM_TRACK
		{
			return actorPtr->trackNumber;
		}
		case 0xC: // CHRONO
		{
			return evalChrono(&actorPtr->CHRONO) / 60; // recheck
		}
		case 0xD: {
			return evalChrono(&actorPtr->ROOM_CHRONO) / 60; // recheck
		}
		case 0xE: // DIST
		{
			const int worldObjectIdx = *(int16 *)currentLifePtr;
			currentLifePtr += 2;

			const int objectIdx = ListWorldObjets[worldObjectIdx].objIndex;

			int tempX;
			int tempY;
			int tempZ;

			if (objectIdx == -1) {
				if (ListWorldObjets[worldObjectIdx].room == currentRoom) {
					tempX = ListWorldObjets[worldObjectIdx].x;
					tempY = ListWorldObjets[worldObjectIdx].y;
					tempZ = ListWorldObjets[worldObjectIdx].z;
				} else {
					return 32000;
				}
			} else {
				tempX = objectTable[objectIdx].worldX;
				tempY = objectTable[objectIdx].worldY;
				tempZ = objectTable[objectIdx].worldZ;
			}

			return calcDist(actorPtr->worldX, actorPtr->worldY, actorPtr->worldZ, tempX, tempY, tempZ);
		}
		case 0xF: // COL_BY
		{
			if (actorPtr->COL_BY == -1)
				return -1;
			return objectTable[actorPtr->COL_BY].indexInWorld;
		}
		case 0x10: // found
		{
			if (ListWorldObjets[evalVar()].flags2 & 0x8000) {
				return 1;
			}
			return 0;
		}
		case 0x11: {
			return action;
		}
		case 0x12: // POSREL
		{

			const int objNum = *(int16 *)currentLifePtr;
			currentLifePtr += 2;

			if (ListWorldObjets[objNum].objIndex == -1) {
				return 0;
			}

			return getPosRel(actorPtr, &objectTable[ListWorldObjets[objNum].objIndex]);
		}
		case 0x13: {
			if (localJoyD & 4)
				return 4;
			if (localJoyD & 8)
				return 8;
			if (localJoyD & 1)
				return 1;
			if (localJoyD & 2)
				return 2;

			return 0;
		}
		case 0x14: {
			return localClick;
		}
		case 0x15: {
			int temp1 = actorPtr->COL[0];
			if (temp1 == -1) {
				temp1 = actorPtr->COL_BY;
				if (temp1 == -1)
					return -1;
			}

			return objectTable[temp1].indexInWorld;
		}
		case 0x16: {
			return actorPtr->alpha;
		}
		case 0x17: {
			return actorPtr->beta;
		}
		case 0x18: {
			return actorPtr->gamma;
		}
		case 0x19: {
			return inHandTable[currentInventory];
		}
		case 0x1A: {
			return actorPtr->hitForce;
		}
		case 0x1B: {
			return cameraDataTable[currentCamera] - &g_currentFloorCameraData[0];
		}
		case 0x1C: {
			const int temp = *(int16 *)currentLifePtr;
			assert(temp > 0);
			currentLifePtr += 2;
			return g_engine->getRandomNumber(temp - 1);
		}
		case 0x1D: {
			return actorPtr->falling;
		}
		case 0x1E: {
			return actorPtr->room;
		}
		case 0x1F: {
			return actorPtr->life;
		}
		case 0x20: {

			const int objNum = *(int16 *)currentLifePtr;
			currentLifePtr += 2;

			if (ListWorldObjets[objNum].flags2 & 0xC000) {
				return 1;
			}
			return 0;
		}
		case 0x21: // TODO: music
		{
			return currentMusic;
		}
		case 0x22: // c_var
		{
			const int temp = CVars[*(int16 *)currentLifePtr];
			currentLifePtr += 2;
			return temp;
		}
		case 0x23: {
			return actorPtr->stage;
		}
		case 0x24: // THROW
		{
			const int objNum = *(int16 *)currentLifePtr;
			currentLifePtr += 2;

			if (ListWorldObjets[objNum].flags2 & 0x1000) {
				return 1;
			}
			return 0;
		}
		case 0x25: // get_matrix
		{

			const int param1 = *(int16 *)currentLifePtr;
			currentLifePtr += 2;

			const int param2 = *(int16 *)currentLifePtr;
			currentLifePtr += 2;

			return getMatrix(param1, actorIdx, ListWorldObjets[param2].objIndex);
		}
		case 0x26: // hard_mat
		{
			return actorPtr->hardMat;
		}
		case 0x27: // TEST_PROTECT
		{
			return 1;
		}
		case 0x2A: // related to samples
		{
			return 1;
		}
		default: {
			debug("Unhandled test type %X in evalVar\n", var1);
			assert(0);
		}
		}
	}
	return -1;
}

} // namespace Fitd
