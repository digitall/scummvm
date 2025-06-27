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

#include "fitd/zv.h"
#include "fitd/costable.h"
#include "fitd/room.h"
#include "fitd/vars.h"

namespace Fitd {

static bool pointRotateEnable = true;
static int pointRotateCosX;
static int pointRotateSinX;
static int pointRotateCosY;
static int pointRotateSinY;
static int pointRotateCosZ;
static int pointRotateSinZ;

void getZvCube(const char *bodyPtr, ZVStruct *zvPtr) {
	const int16 *ptr = (const int16 *)(bodyPtr + 2);

	zvPtr->ZVX1 = *ptr++;
	zvPtr->ZVX2 = *ptr++;
	zvPtr->ZVY1 = *ptr++;
	zvPtr->ZVY2 = *ptr++;
	zvPtr->ZVZ1 = *ptr++;
	zvPtr->ZVZ2 = *ptr++;

	zvPtr->ZVZ2 = zvPtr->ZVX2 = (zvPtr->ZVX2 + zvPtr->ZVZ2) / 2;
	zvPtr->ZVX1 = zvPtr->ZVZ1 = -zvPtr->ZVZ2;
}

void giveZVObjet(const char *bodyPtr, ZVStruct *zvPtr) {
	const int16 *ptr = (const int16 *)(bodyPtr + 2);

	zvPtr->ZVX1 = *ptr++;
	zvPtr->ZVX2 = *ptr++;
	zvPtr->ZVY1 = *ptr++;
	zvPtr->ZVY2 = *ptr++;
	zvPtr->ZVZ1 = *ptr++;
	zvPtr->ZVZ2 = *ptr++;
}

void makeDefaultZV(ZVStruct *zvPtr) {
	zvPtr->ZVX1 = -100;
	zvPtr->ZVX2 = 100;

	zvPtr->ZVY1 = -2000;
	zvPtr->ZVY2 = 0;

	zvPtr->ZVZ1 = -100;
	zvPtr->ZVZ2 = 100;
}

void getZvMax(char *bodyPtr, ZVStruct *zvPtr) {
	int x1;
	int x2;
	int z1;
	int z2;

	giveZVObjet(bodyPtr, zvPtr);

	x1 = zvPtr->ZVX1;
	x2 = zvPtr->ZVX2;

	z1 = zvPtr->ZVZ1;
	z2 = zvPtr->ZVZ2;

	x2 = -x1 + x2;
	z2 = -z1 + z2;

	if (x2 < z2) {
		x2 = z2;
	}

	x2 /= 2;

	zvPtr->ZVX1 = -x2;
	zvPtr->ZVX2 = x2;

	zvPtr->ZVZ1 = -x2;
	zvPtr->ZVZ2 = x2;
}

int asmCheckListCol(const ZVStruct *zvPtr, RoomData *pRoomData) {
	int hardColVar = 0;
	HardCol *pCurrentEntry = pRoomData->hardColTable.data();

	for (uint16 i = 0; i < pRoomData->numHardCol; i++) {
		if (pCurrentEntry->zv.ZVX1 < zvPtr->ZVX2 && zvPtr->ZVX1 < pCurrentEntry->zv.ZVX2) {
			if (pCurrentEntry->zv.ZVY1 < zvPtr->ZVY2 && zvPtr->ZVY1 < pCurrentEntry->zv.ZVY2) {
				if (pCurrentEntry->zv.ZVZ1 < zvPtr->ZVZ2 && zvPtr->ZVZ1 < pCurrentEntry->zv.ZVZ2) {
					assert(hardColVar < 10);
					hardColTable[hardColVar++] = pCurrentEntry;
				}
			}
		}

		pCurrentEntry++;
	}

	return hardColVar;
}

static void hardColSuB1Sub1(int flag) {
	switch (flag - 1) {
	case 0:
	case 1: {
		hardColStepZ = 0;
		break;
	}
	case 3:
	case 7: {
		hardColStepX = 0;
		break;
	}
	}
}

void handleCollision(const ZVStruct *startZv, const ZVStruct *zvPtr2, const ZVStruct *zvPtr3) {
	int32 flag = 0;
	int32 var_8;
	int32 halfX;
	int32 halfZ;
	int32 var_A;
	int32 var_6;

	if (startZv->ZVX2 > zvPtr3->ZVX1) {
		if (zvPtr3->ZVX2 <= startZv->ZVX1) {
			flag = 8;
		}
	} else {
		flag = 4;
	}

	if (startZv->ZVZ2 > zvPtr3->ZVZ1) {
		if (startZv->ZVZ1 >= zvPtr3->ZVZ2) {
			flag |= 2;
		}
	} else {
		flag |= 1;
	}

	if (flag == 5 || flag == 9 || flag == 6 || flag == 10) {
		var_8 = 2;
	} else {
		if (!flag) {
			var_8 = 0;

			hardColStepZ = 0;
			hardColStepX = 0;

			return;
		} else {
			var_8 = 1;
		}
	}

	halfX = (zvPtr2->ZVX1 + zvPtr2->ZVX2) / 2;
	halfZ = (zvPtr2->ZVZ1 + zvPtr2->ZVZ2) / 2;

	if (zvPtr3->ZVX1 > halfX) {
		var_A = 4;
	} else {
		if (zvPtr3->ZVX2 < halfX) {
			var_A = 8;
		} else {
			var_A = 0;
		}
	}

	if (zvPtr3->ZVZ1 > halfZ) {
		var_A |= 1;
	} else {
		if (zvPtr3->ZVZ2 < halfZ) {
			var_A |= 2;
		}
	}

	if (var_A == 5 || var_A == 9 || var_A == 6 || var_A == 10) {
		var_6 = 2;
	} else {
		if (!var_A) {
			var_6 = 0;
		} else {
			var_6 = 1;
		}
	}

	if (var_8 == 1) {
		hardColSuB1Sub1(flag);
		return;
	}

	if (var_6 == 1 && var_A & flag) {
		hardColSuB1Sub1(var_A);
		return;
	}

	if (var_A == flag || ((flag + var_A) != 15)) {
		const int Xmod = abs(zvPtr2->ZVX1 - startZv->ZVX1); // recheck
		const int Zmod = abs(zvPtr2->ZVZ1 - startZv->ZVZ1);

		if (Xmod > Zmod) {
			hardColStepZ = 0;
		} else {
			hardColStepX = 0;
		}
	} else {
		if (!var_6 || (var_6 == 1 && !(var_A & flag))) {
			hardColStepZ = 0;
			hardColStepX = 0;
		} else {
			hardColSuB1Sub1(flag & var_A);
		}
	}
}

void copyZv(const ZVStruct *source, ZVStruct *dest) {
	memcpy(dest, source, sizeof(ZVStruct));
}

void getZvRelativePosition(ZVStruct *zvPtr, int startRoom, int destRoom) {
	const unsigned int Xdif = 10 * (roomDataTable[destRoom].worldX - roomDataTable[startRoom].worldX);
	const unsigned int Ydif = 10 * (roomDataTable[destRoom].worldY - roomDataTable[startRoom].worldY);
	const unsigned int Zdif = 10 * (roomDataTable[destRoom].worldZ - roomDataTable[startRoom].worldZ);

	zvPtr->ZVX1 -= Xdif;
	zvPtr->ZVX2 -= Xdif;
	zvPtr->ZVY1 += Ydif;
	zvPtr->ZVY2 += Ydif;
	zvPtr->ZVZ1 += Zdif;
	zvPtr->ZVZ2 += Zdif;
}

int checkZvCollision(const ZVStruct *zvPtr1, const ZVStruct *zvPtr2) {
	if (zvPtr1->ZVX1 >= zvPtr2->ZVX2)
		return 0;

	if (zvPtr2->ZVX1 >= zvPtr1->ZVX2)
		return 0;

	if (zvPtr1->ZVY1 >= zvPtr2->ZVY2)
		return 0;

	if (zvPtr2->ZVY1 >= zvPtr1->ZVY2)
		return 0;

	if (zvPtr1->ZVZ1 >= zvPtr2->ZVZ2)
		return 0;

	if (zvPtr2->ZVZ1 >= zvPtr1->ZVZ2)
		return 0;

	return 1;
}

int checkObjectCollisions(int actorIdx, const ZVStruct *zvPtr) {
	int currentCollisionSlot = 0;
	Object *currentActor = objectTable;
	const int actorRoom = objectTable[actorIdx].room;

	for (int i = 0; i < 3; i++) {
		currentProcessedActorPtr->COL[i] = -1;
	}

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		if (currentActor->indexInWorld != -1 && i != actorIdx) {
			const ZVStruct *currentActorZv = &currentActor->zv;

			if (currentActor->room != actorRoom) {
				ZVStruct localZv;

				copyZv(zvPtr, &localZv);

				getZvRelativePosition(&localZv, actorRoom, currentActor->room);

				if (checkZvCollision(&localZv, currentActorZv)) {
					currentProcessedActorPtr->COL[currentCollisionSlot++] = i;

					if (currentCollisionSlot == 3)
						return 3;
				}
			} else {
				if (checkZvCollision(zvPtr, currentActorZv)) {
					currentProcessedActorPtr->COL[currentCollisionSlot++] = i;

					if (currentCollisionSlot == 3)
						return 3;
				}
			}
		}
		currentActor++;
	}

	return currentCollisionSlot;
}

static void setupPointRotate(int alpha, int beta, int gamma) {
	pointRotateEnable = true;

	pointRotateCosX = cosTable[alpha & 0x3FF];
	pointRotateSinX = cosTable[(alpha & 0x3FF) + 0x100 & 0x3FF];

	pointRotateCosY = cosTable[beta & 0x3FF];
	pointRotateSinY = cosTable[(beta & 0x3FF) + 0x100 & 0x3FF];

	pointRotateCosZ = cosTable[gamma & 0x3FF];
	pointRotateSinZ = cosTable[(gamma & 0x3FF) + 0x100 & 0x3FF];
}

static void pointRotate(int x, int y, int z, int *destX, int *destY, int *destZ) {
	if (pointRotateEnable) {
		{
			const int tempX = x;
			const int tempY = y;
			x = ((tempX * pointRotateSinZ - tempY * pointRotateCosZ) >> 16) << 1;
			y = ((tempX * pointRotateCosZ + tempY * pointRotateSinZ) >> 16) << 1;
		}

		{
			const int tempX = x;
			const int tempZ = z;

			x = ((tempX * pointRotateSinY - tempZ * pointRotateCosY) >> 16) << 1;
			z = ((tempX * pointRotateCosY + tempZ * pointRotateSinY) >> 16) << 1;
		}

		{
			const int tempY = y;
			const int tempZ = z;
			y = ((tempY * pointRotateSinX - tempZ * pointRotateCosX) >> 16) << 1;
			z = ((tempY * pointRotateCosX + tempZ * pointRotateSinX) >> 16) << 1;
		}

		*destX = x;
		*destY = y;
		*destZ = z;
	}
}

static void zvRotSub(int X, int Y, int Z, int alpha, int beta, int gamma) {
	if (alpha || beta || gamma) {
		setupPointRotate(alpha, beta, gamma);
		pointRotate(X, Y, Z, &animMoveX, &animMoveY, &animMoveZ);
	} else {
		animMoveX = X;
		animMoveY = Y;
		animMoveZ = Z;
	}
}

void getZvRot(char *bodyPtr, ZVStruct *zvPtr, int alpha, int beta, int gamma) {
	int X1 = 32000;
	int Y1 = 32000;
	int Z1 = 32000;

	int X2 = -32000;
	int Y2 = -32000;
	int Z2 = -32000;

	int i;
	int tempX = 0;
	int tempY = 0;
	int tempZ = 0;

	giveZVObjet(bodyPtr, zvPtr);

	for (i = 0; i < 8; i++) {
		switch (i) {
		case 0: {
			tempX = zvPtr->ZVX1;
			tempY = zvPtr->ZVY1;
			tempZ = zvPtr->ZVZ1;
			break;
		}
		case 1: {
			tempZ = zvPtr->ZVZ2;
			break;
		}
		case 2: {
			tempX = zvPtr->ZVX2;
			break;
		}
		case 3: {
			tempZ = zvPtr->ZVZ1;
			break;
		}
		case 4: {
			tempY = zvPtr->ZVY2;
			break;
		}
		case 5: {
			tempX = zvPtr->ZVX1;
			break;
		}
		case 6: {
			tempZ = zvPtr->ZVZ2;
			break;
		}
		case 7: {
			tempX = zvPtr->ZVX2;
			break;
		}
		}

		zvRotSub(tempX, tempY, tempZ, alpha, beta, gamma);

		if (animMoveX < X1)
			X1 = animMoveX;

		if (animMoveX > X2)
			X2 = animMoveX;

		if (animMoveY < Y1)
			Y1 = animMoveY;

		if (animMoveY > Y2)
			Y2 = animMoveY;

		if (animMoveZ < Z1)
			Z1 = animMoveZ;

		if (animMoveZ > Z2)
			Z2 = animMoveZ;
	}

	zvPtr->ZVX1 = X1;
	zvPtr->ZVX2 = X2;
	zvPtr->ZVY1 = Y1;
	zvPtr->ZVY2 = Y2;
	zvPtr->ZVZ1 = Z1;
	zvPtr->ZVZ2 = Z2;
}

} // namespace Fitd
