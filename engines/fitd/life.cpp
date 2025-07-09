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

#include "common/debug.h"
#include "fitd/aitd1.h"
#include "fitd/aitd2.h"
#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/eval_var.h"
#include "fitd/fitd.h"
#include "fitd/font.h"
#include "fitd/game_time.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/inventory.h"
#include "fitd/jack.h"
#include "fitd/life.h"
#include "fitd/music.h"
#include "fitd/pak.h"
#include "fitd/room.h"
#include "fitd/sequence.h"
#include "fitd/tatou.h"
#include "fitd/track.h"
#include "fitd/vars.h"
#include "fitd/zv.h"

namespace Fitd {

int groundLevel;
int16 specialTable[4] = {144, 192, 48, 112};

int numSequenceParam = 0;

SequenceParam sequenceParams[NUM_MAX_SEQUENCE_PARAM];

static const char *sequenceListAITD2[] =
	{
		"BATL",
		"GRAP",
		"CLE1",
		"CLE2",
		"COOK",
		"EXPL",
		"FALA",
		"FAL2",
		"GLIS",
		"GREN",
		"JEND",
		"MANI",
		"MER",
		"TORD",
		"PANT",
		"VERE",
		"PL21",
		"PL22",
		"ENDX",
		"SORT",
		"EFER",
		"STAR",
		"MEDU",
		"PROL",
		"GRAS",
		"STRI",
		"ITRO",
		"BILL",
		"PIRA",
		"PIR2",
		"VENT",
		"FIN",
		"LAST"};

static int16 readNextArgument(const char *name = nullptr) {
	const int16 value = *(int16 *)g_engine->_engine->currentLifePtr;
	g_engine->_engine->currentLifePtr += 2;

	// if (name)
	// {
	//     appendFormated("%s:%d, ",name, value);
	// }
	// else
	// {
	//     appendFormated("%d, ", value);
	// }

	return value;
}

void resetRotateParam() {
	g_engine->_engine->currentProcessedActorPtr->rotate.param = 0;
}

static void throwObj(int animThrow, int frameThrow, int arg_4, int objToThrowIdx, int throwRotated, int throwForce, int animNext) {
	if (initAnim(animThrow, 2, animNext)) {
		g_engine->_engine->currentProcessedActorPtr->animActionANIM = animThrow;
		g_engine->_engine->currentProcessedActorPtr->animActionFRAME = frameThrow;
		g_engine->_engine->currentProcessedActorPtr->animActionType = 6;
		g_engine->_engine->currentProcessedActorPtr->hotPointID = arg_4;
		g_engine->_engine->currentProcessedActorPtr->animActionParam = objToThrowIdx;
		g_engine->_engine->currentProcessedActorPtr->hitForce = throwForce;

		if (!throwRotated) {
			g_engine->_engine->worldObjets[objToThrowIdx].gamma -= 0x100;
		}

		g_engine->_engine->worldObjets[objToThrowIdx].flags2 |= 0x1000;
	}
}

static void put(int x, int y, int z, int room, int stage, int alpha, int beta, int gamma, int idx) {
	WorldObject *objPtr = &g_engine->_engine->worldObjets[idx];

	objPtr->x = x;
	objPtr->y = y;
	objPtr->z = z;

	objPtr->room = room;
	objPtr->stage = stage;

	objPtr->alpha = alpha;
	objPtr->beta = beta;
	objPtr->gamma = gamma;

	deleteInventoryObjet(idx);

	objPtr->flags2 |= 0x4000;

	/*  FlagGenereActiveList = 1;
	FlagRefreshAux2 = 1; */
}

void drop(int worldIdx, int worldSource) {
	putAtObjet(worldIdx, worldSource);
}

static void fire(int fireAnim, int X, int Y, int Z, int hitForce, int nextAnim) {
	if (initAnim(fireAnim, 2, nextAnim)) {
		g_engine->_engine->currentProcessedActorPtr->animActionANIM = fireAnim;
		g_engine->_engine->currentProcessedActorPtr->animActionFRAME = X;
		g_engine->_engine->currentProcessedActorPtr->animActionType = 4;
		g_engine->_engine->currentProcessedActorPtr->animActionParam = Z;
		g_engine->_engine->currentProcessedActorPtr->hotPointID = Y;
		g_engine->_engine->currentProcessedActorPtr->hitForce = hitForce;
	}
}

int randRange(int min, int max) {
	return min + g_engine->getRandomNumber(max - min);
}

int initSpecialObjet(int mode, int X, int Y, int Z, int stage, int room, int alpha, int beta, int gamma, ZVStruct *zvPtr) {
	int16 localSpecialTable[4];
	ZVStruct *actorZvPtr = nullptr;

	memcpy(localSpecialTable, specialTable, 8);

	Object *currentActorPtr = g_engine->_engine->objectTable;

	int i;
	for (i = 0; i < NUM_MAX_OBJECT; i++) // count the number of active actors
	{
		if (currentActorPtr->indexInWorld == -1)
			break;
		currentActorPtr++;
	}

	if (i == NUM_MAX_OBJECT) // no free actor entry, abort
	{
		return -1;
	}

	currentActorPtr->_flags = AF_SPECIAL;
	currentActorPtr->indexInWorld = -2;
	currentActorPtr->life = -1;
	currentActorPtr->lifeMode = 2;
	currentActorPtr->bodyNum = 0;
	currentActorPtr->worldX = currentActorPtr->roomX = X;
	currentActorPtr->worldY = currentActorPtr->roomY = Y;
	currentActorPtr->worldZ = currentActorPtr->roomZ = Z;
	currentActorPtr->stage = stage;
	currentActorPtr->room = room;

	if (g_engine->_engine->currentRoom != room) {
		currentActorPtr->worldX -= static_cast<int16>((g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldX - g_engine->_engine->roomDataTable[room].worldX) * 10);
		currentActorPtr->worldY += static_cast<int16>((g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldY - g_engine->_engine->roomDataTable[room].worldY) * 10);
		currentActorPtr->worldZ += static_cast<int16>((g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldZ - g_engine->_engine->roomDataTable[room].worldZ) * 10);
	}

	currentActorPtr->alpha = alpha;
	currentActorPtr->beta = beta;
	currentActorPtr->gamma = gamma;
	currentActorPtr->stepX = 0;
	currentActorPtr->stepY = 0;
	currentActorPtr->stepZ = 0;

	if (zvPtr) {
		actorZvPtr = &currentActorPtr->zv;
		copyZv(zvPtr, actorZvPtr);
	}

	switch (mode) {
	case 0: {
		// evaporate
		actorZvPtr->ZVX1 -= X;
		actorZvPtr->ZVX2 -= X;
		actorZvPtr->ZVY1 -= Y;
		actorZvPtr->ZVY2 -= Y;
		actorZvPtr->ZVZ1 -= Z;
		actorZvPtr->ZVZ2 -= Z;

		currentActorPtr->FRAME = hqMalloc(g_engine->_engine->hqMemory, 304);

		byte *flowPtr = hqPtrMalloc(g_engine->_engine->hqMemory, currentActorPtr->FRAME);

		if (!flowPtr) {
			currentActorPtr->indexInWorld = -1;
			return -1;
		}

		currentActorPtr->ANIM = mode;

		*(int16 *)flowPtr = localSpecialTable[g_engine->getRandomNumber(2)]; // type ? color ?
		flowPtr += 2;
		*(int16 *)flowPtr = 30; // num of points
		flowPtr += 2;

		for (int j = 0; j < 30; j++) {
			*(int16 *)flowPtr = randRange(actorZvPtr->ZVX1, actorZvPtr->ZVX2); // X
			flowPtr += 2;
			*(int16 *)flowPtr = randRange(actorZvPtr->ZVY1, actorZvPtr->ZVY2); // Y
			flowPtr += 2;
			*(int16 *)flowPtr = randRange(actorZvPtr->ZVZ1, actorZvPtr->ZVZ2); // Z
			flowPtr += 2;
		}

		for (int j = 0; j < 30; j++) {
			*(int16 *)flowPtr = randRange(150, 300); // size
			flowPtr += 2;
			*(int16 *)flowPtr = randRange(30, 80); // dy
			flowPtr += 2;
		}

		actorZvPtr->ZVX1 = X - 10;
		actorZvPtr->ZVX2 = X + 10;
		actorZvPtr->ZVY1 = Y;
		actorZvPtr->ZVY2 = Y - 1;
		actorZvPtr->ZVZ1 = Z - 10;
		actorZvPtr->ZVZ2 = Z + 10;

		break;
	}
	case 1: // blood
	case 2: // debris
	{
		actorZvPtr->ZVX1 = X;
		actorZvPtr->ZVX2 = X;
		actorZvPtr->ZVY1 = 0;
		actorZvPtr->ZVY2 = 0;
		actorZvPtr->ZVZ1 = Z;
		actorZvPtr->ZVZ2 = Z;

		currentActorPtr->FRAME = hqMalloc(g_engine->_engine->hqMemory, 304);

		byte *flowPtr = hqPtrMalloc(g_engine->_engine->hqMemory, currentActorPtr->FRAME);

		if (!flowPtr) {
			currentActorPtr->indexInWorld = -1;
			return -1;
		}

		currentActorPtr->ANIM = mode;

		*(int16 *)flowPtr = mode == 1 ? 85 : 15; // color
		flowPtr += 2;
		*(int16 *)flowPtr = 30; // num of points
		flowPtr += 2;

		for (int j = 0; j < 30; j++) {
			*(int16 *)flowPtr = randRange(-100, 100); // X
			flowPtr += 2;
			*(int16 *)flowPtr = randRange(-100, 100); // Y
			flowPtr += 2;
			*(int16 *)flowPtr = randRange(-100, 100); // Z
			flowPtr += 2;
		}

		for (int j = 0; j < 30; j++) {
			*(int16 *)flowPtr = randRange(0, 1023); // gamma?
			flowPtr += 2;
			*(int16 *)flowPtr = randRange(-50, 10); // dy
			flowPtr += 2;
		}

		break;
	}
	case 3: {
		// muzzle flash
		currentActorPtr->ANIM = mode;
		actorZvPtr->ZVX1 = X;
		actorZvPtr->ZVX2 = X;
		actorZvPtr->ZVY1 = Y;
		actorZvPtr->ZVY2 = Y;
		actorZvPtr->ZVZ1 = Z;
		actorZvPtr->ZVZ2 = Z;
		break;
	}
	case 4: {
		// cigar smoke
		g_engine->_engine->cVars[getCVarsIdx(FOG_FLAG)] = 1;
		actorZvPtr->ZVX1 = X;
		actorZvPtr->ZVX2 = X;
		actorZvPtr->ZVY1 = Y;
		actorZvPtr->ZVY2 = Y;
		actorZvPtr->ZVZ1 = Z;
		actorZvPtr->ZVZ2 = Z;

		currentActorPtr->FRAME = hqMalloc(g_engine->_engine->hqMemory, 246);

		byte *flowPtr = hqPtrMalloc(g_engine->_engine->hqMemory, currentActorPtr->FRAME);
		if (!flowPtr) {
			currentActorPtr->indexInWorld = -1;
			return -1;
		}

		currentActorPtr->ANIM = 4;
		uint32 *chrono = (uint32 *)flowPtr;
		uint tmpChrono;
		startChrono(&tmpChrono);
		*chrono = tmpChrono;
		flowPtr += 4;

		for (int j = 0; j < 20; ++j) {
			*(int16 *)flowPtr = randRange(-2000, 2000);
			flowPtr += 2;
			*(int16 *)flowPtr = 0;
			flowPtr += 2;
			*(int16 *)flowPtr = randRange(-2000, 2000);
			flowPtr += 2;
		}
		actorZvPtr->ZVX1 = X - 10;
		actorZvPtr->ZVX2 = X + 10;
		actorZvPtr->ZVY1 = Y - 200;
		actorZvPtr->ZVY2 = Y - 200;
		actorZvPtr->ZVZ1 = Z - 10;
		actorZvPtr->ZVZ2 = Z + 10;
		break;
	}
	default: {
		debug("Unsupported case %d in initSpecialObjet\n", mode);
	}
	}

	g_engine->_engine->actorTurnedToObj = 1;
	return i;
}

static void getHardClip() {
	const ZVStruct *zvPtr = &g_engine->_engine->currentProcessedActorPtr->zv;
	char *etageData = (char *)getRoomData(g_engine->_engine->currentProcessedActorPtr->room);

	etageData += *(int16 *)etageData;

	const int16 numEntry = *(int16 *)etageData;
	etageData += 2;

	for (int i = 0; i < numEntry; i++) {
		ZVStruct zvCol;

		zvCol.ZVX1 = READ_LE_S16(etageData + 0x00);
		zvCol.ZVX2 = READ_LE_S16(etageData + 0x02);
		zvCol.ZVY1 = READ_LE_S16(etageData + 0x04);
		zvCol.ZVY2 = READ_LE_S16(etageData + 0x06);
		zvCol.ZVZ1 = READ_LE_S16(etageData + 0x08);
		zvCol.ZVZ2 = READ_LE_S16(etageData + 0x0A);

		if (checkZvCollision(zvPtr, &zvCol)) {
			g_engine->_engine->hardClip.ZVX1 = zvCol.ZVX1;
			g_engine->_engine->hardClip.ZVX2 = zvCol.ZVX2;
			g_engine->_engine->hardClip.ZVY1 = zvCol.ZVY1;
			g_engine->_engine->hardClip.ZVY2 = zvCol.ZVY2;
			g_engine->_engine->hardClip.ZVZ1 = zvCol.ZVZ1;
			g_engine->_engine->hardClip.ZVZ2 = zvCol.ZVZ2;

			return;
		}

		etageData += 16;
	}

	g_engine->_engine->hardClip.ZVX1 = 32000;
	g_engine->_engine->hardClip.ZVX2 = -32000;
	g_engine->_engine->hardClip.ZVY1 = 32000;
	g_engine->_engine->hardClip.ZVY2 = -32000;
	g_engine->_engine->hardClip.ZVZ1 = 32000;
	g_engine->_engine->hardClip.ZVZ2 = -32000;
}

static void animMove(int animStand, int animWalk, int animRun, int animStop, int animBackward, int animTurnRight, int animTurnLeft) {
	if (g_engine->_engine->currentProcessedActorPtr->speed == 5) {
		initAnim(animRun, 1, -1);
	}

	if (g_engine->_engine->currentProcessedActorPtr->speed == 4) {
		initAnim(animWalk, 1, -1);
	}

	if (g_engine->_engine->currentProcessedActorPtr->speed == -1) // backward
	{
		if (g_engine->_engine->currentProcessedActorPtr->ANIM == animWalk) {
			initAnim(animStand, 0, animBackward);
		} else if (g_engine->_engine->currentProcessedActorPtr->ANIM == animRun) {
			initAnim(animStop, 0, animStand);
		} else {
			initAnim(animBackward, 1, -1); // walk backward
		}
	}
	if (g_engine->_engine->currentProcessedActorPtr->speed == 0) {
		if (g_engine->_engine->currentProcessedActorPtr->ANIM == animWalk || g_engine->_engine->currentProcessedActorPtr->ANIM == animRun) {
			initAnim(animStop, 0, animStand);
		} else {
			if (g_engine->_engine->currentProcessedActorPtr->direction == 0) {
				initAnim(animStand, 1, -1);
			}
			if (g_engine->_engine->currentProcessedActorPtr->direction == 1) // left
			{
				initAnim(animTurnLeft, 0, animStand);
			}
			if (g_engine->_engine->currentProcessedActorPtr->direction == -1) // right
			{
				initAnim(animTurnRight, 0, animStand);
			}
		}
	}
}

static void setStage(int newStage, int newRoomLocal, int X, int Y, int Z) {

	g_engine->_engine->currentProcessedActorPtr->stage = newStage;
	g_engine->_engine->currentProcessedActorPtr->room = newRoomLocal;

	if (g_engine->getGameId() != GID_AITD1) {
		g_engine->_engine->currentProcessedActorPtr->hardMat = -1;
	}

	const int animX = g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX;
	const int animY = g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY;
	const int animZ = g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ;

	g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 += X - animX;
	g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 += X - animX;

	g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += Y - animY;
	g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += Y - animY;

	g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 += Z - animZ;
	g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 += Z - animZ;

	g_engine->_engine->currentProcessedActorPtr->roomX = X;
	g_engine->_engine->currentProcessedActorPtr->roomY = Y;
	g_engine->_engine->currentProcessedActorPtr->roomZ = Z;

	g_engine->_engine->currentProcessedActorPtr->worldX = X;
	g_engine->_engine->currentProcessedActorPtr->worldY = Y;
	g_engine->_engine->currentProcessedActorPtr->worldZ = Z;

	g_engine->_engine->currentProcessedActorPtr->stepX = 0;
	g_engine->_engine->currentProcessedActorPtr->stepY = 0;
	g_engine->_engine->currentProcessedActorPtr->stepZ = 0;

	if (g_engine->_engine->currentCameraTargetActor == g_engine->_engine->currentProcessedActorIdx) {
		if (newStage != g_engine->_engine->currentFloor) {
			g_engine->_engine->changeFloor = 1;
			g_engine->_engine->newFloor = newStage;
			g_engine->_engine->newRoom = newRoomLocal;
		} else {
			if (g_engine->_engine->currentRoom != newRoomLocal) {
				g_engine->_engine->needChangeRoom = 1;
				g_engine->_engine->newRoom = newRoomLocal;
			}
		}
	} else {
		if (g_engine->_engine->currentRoom != newRoomLocal) {
			g_engine->_engine->currentProcessedActorPtr->worldX -= static_cast<int16>((g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldX - g_engine->_engine->roomDataTable[newRoomLocal].worldX) * 10);
			g_engine->_engine->currentProcessedActorPtr->worldY += static_cast<int16>((g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldY - g_engine->_engine->roomDataTable[newRoomLocal].worldY) * 10);
			g_engine->_engine->currentProcessedActorPtr->worldZ += static_cast<int16>((g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldZ - g_engine->_engine->roomDataTable[newRoomLocal].worldZ) * 10);
		}

		//    FlagGenereActiveList = 1;
	}
}

void setupRealZv(ZVStruct *zvPtr) {
	const int16 *ptr = g_engine->_engine->pointBuffer;

	zvPtr->ZVX1 = 32000;
	zvPtr->ZVY1 = 32000;
	zvPtr->ZVZ1 = 32000;
	zvPtr->ZVX2 = -32000;
	zvPtr->ZVY2 = -32000;
	zvPtr->ZVZ2 = -32000;

	for (int i = 0; i < g_engine->_engine->numOfPoints; i++) {
		if (zvPtr->ZVX1 > *ptr) {
			zvPtr->ZVX1 = *ptr;
		} else {
			if (zvPtr->ZVX2 < *ptr) {
				zvPtr->ZVX2 = *ptr;
			}
		}
		ptr++;

		if (zvPtr->ZVY1 > *ptr) {
			zvPtr->ZVY1 = *ptr;
		} else {
			if (zvPtr->ZVY2 < *ptr) {
				zvPtr->ZVY2 = *ptr;
			}
		}
		ptr++;

		if (zvPtr->ZVZ1 > *ptr) {
			zvPtr->ZVZ1 = *ptr;
		} else {
			if (zvPtr->ZVZ2 < *ptr) {
				zvPtr->ZVZ2 = *ptr;
			}
		}
		ptr++;
	}
}

static void doRealZv(Object *actorPtr) {

	computeScreenBox(0, 0, 0, actorPtr->alpha, actorPtr->beta, actorPtr->gamma, hqrGet(g_engine->_engine->listBody, actorPtr->bodyNum));

	ZVStruct *zvPtr = &actorPtr->zv;

	setupRealZv(zvPtr);

	zvPtr->ZVX1 += actorPtr->roomX;
	zvPtr->ZVX2 += actorPtr->roomX;
	zvPtr->ZVY1 += actorPtr->roomY;
	zvPtr->ZVY2 += actorPtr->roomY;
	zvPtr->ZVZ1 += actorPtr->roomZ;
	zvPtr->ZVZ2 += actorPtr->roomZ;
}

static void hit(int animNumber, int arg_2, int arg_4, int arg_6, int hitForce, int arg_A) {
	if (initAnim(animNumber, 0, arg_A)) {
		g_engine->_engine->currentProcessedActorPtr->animActionANIM = animNumber;
		g_engine->_engine->currentProcessedActorPtr->animActionFRAME = arg_2;
		g_engine->_engine->currentProcessedActorPtr->animActionType = 1;
		g_engine->_engine->currentProcessedActorPtr->animActionParam = arg_6;
		g_engine->_engine->currentProcessedActorPtr->hotPointID = arg_4;
		g_engine->_engine->currentProcessedActorPtr->hitForce = hitForce;
	}
}

static void deleteObject(int objIdx) {

	WorldObject *objPtr = &g_engine->_engine->worldObjets[objIdx];
	const int actorIdx = objPtr->objIndex;

	if (actorIdx != -1) {
		Object *actorPtr = &g_engine->_engine->objectTable[actorIdx];

		actorPtr->room = -1;
		actorPtr->stage = -1;

		//    FlagGenereActiveList = 1;

		if (actorPtr->_flags & AF_BOXIFY) {
			removeFromBGIncrust(actorIdx);
		}
	}

	objPtr->room = -1;
	objPtr->stage = -1;

	deleteInventoryObjet(objIdx);
}

static void readBook(int index, int type, int shadow) {
	freezeTime();

	switch (g_engine->getGameId()) {
	case GID_AITD1:
		aitd1ReadBook(index, type, shadow);
		break;
	case GID_JACK:
		jackReadBook(index, type);
		break;
	case GID_AITD2:
		aitd2ReadBook(index, type);
		break;
	default:
		assert(0);
	}

	unfreezeTime();
}

void makeMessage(int messageIdx) {
	TextEntryStruct *messagePtr = getTextFromIdx(messageIdx);

	if (messagePtr) {
		for (int i = 0; i < 5; i++) {
			if (g_engine->_engine->messageTable[i].string == messagePtr) {
				g_engine->_engine->messageTable[i].time = 0;
				return;
			}
		}

		for (int i = 0; i < 5; i++) {
			if (g_engine->_engine->messageTable[i].string == nullptr) {
				g_engine->_engine->messageTable[i].string = messagePtr;
				g_engine->_engine->messageTable[i].time = 0;
				return;
			}
		}
	}
}

static void unpackSequenceFrame(byte *source, byte *dest) {

	byte byteCode = *source++;

	while (byteCode) {
		if (!--byteCode) // change pixel or skip pixel
		{

			const byte changeColor = *source++;

			if (changeColor) {
				*dest++ = changeColor;
			} else {
				dest++;
			}
		} else if (!--byteCode) // change 2 pixels or skip 2 pixels
		{

			const byte changeColor = *source++;

			if (changeColor) {
				*dest++ = changeColor;
				*dest++ = changeColor;
			} else {
				dest += 2;
			}
		} else if (!--byteCode) // fill or skip
		{

			const byte size = *source++;
			const byte fillColor = *source++;

			if (fillColor) {

				for (int i = 0; i < size; i++) {
					*dest++ = fillColor;
				}
			} else {
				dest += size;
			}
		} else // large fill of skip
		{

			const uint16 size = READ_LE_U16(source);
			source += 2;
			const byte fillColor = *source++;

			if (fillColor) {

				for (int i = 0; i < size; i++) {
					*dest++ = fillColor;
				}
			} else {
				dest += size;
			}
		}

		byteCode = *source++;
	}
}

static void playSequence(int sequenceIdx, int fadeStart, int fadeOutVar) {

	int quitPlayback = 0;
	int nextFrame = 1;
	byte localPalette[0x300];

	Common::String buffer;
	if (g_engine->getGameId() == GID_AITD2) {
		buffer = Common::String::format("%s.PAK", sequenceListAITD2[sequenceIdx]);
	} else if (g_engine->getGameId() == GID_AITD3) {
		buffer = Common::String::format("AN%d.PAK", sequenceIdx);
	} else {
		assert(0);
	}

	const int numMaxFrames = pakGetNumFiles(buffer.c_str());

	while (!quitPlayback) {
		int currentFrameId = 0;

		while (currentFrameId < nextFrame) {
			// frames++;

			g_engine->_engine->timer = g_engine->_engine->timeGlobal;

			if (currentFrameId >= numMaxFrames) {
				quitPlayback = 1;
				break;
			}

			if (!pakLoad(buffer.c_str(), currentFrameId, g_engine->_engine->logicalScreen)) {
				error("Error loading pak %s", buffer.c_str());
			}

			if (!currentFrameId) // first frame
			{
				const int var_4 = 1;
				memcpy(localPalette, g_engine->_engine->logicalScreen, 0x300); // copy palette
				memcpy(g_engine->_engine->aux, g_engine->_engine->logicalScreen + 0x300, 64000);
				nextFrame = READ_LE_U16(g_engine->_engine->logicalScreen + 64768);

				convertPaletteIfRequired(localPalette);

				if (var_4 != 0) {
					/*      if(fadeStart & 1)
					{
					fadeOut(0x10,0);
					}
					if(fadeStart & 4)
					{
					//memset(palette,0,0); // fade from black
					fadeInSub1(localPalette);
					flipOtherPalette(palette);
					} */

					gfx_setPalette(localPalette);
					copyPalette(localPalette, g_engine->_engine->currentGamePalette);
				}
			} else // not first frame
			{

				const uint32 frameSize = READ_LE_U32(g_engine->_engine->logicalScreen);

				if (frameSize < 64000) // g_engine->_engine->key frame
				{
					unpackSequenceFrame(g_engine->_engine->logicalScreen + 4, g_engine->_engine->aux);
				} else // delta frame
				{
					fastCopyScreen(g_engine->_engine->logicalScreen, g_engine->_engine->aux);
				}
			}

			for (int sequenceParamIdx = 0; sequenceParamIdx < numSequenceParam; sequenceParamIdx++) {
				if (sequenceParams[sequenceParamIdx].frame == static_cast<uint>(currentFrameId)) {
					playSound(sequenceParams[sequenceParamIdx].sample);
				}
			}

			// TODO: here, timing management
			// TODO: fade management

			gfx_copyBlockPhys(g_engine->_engine->aux, 0, 0, 320, 200);

			osystem_drawBackground();

			currentFrameId++;

			for (int i = 0; i < 5; i++) // display the frame 5 times (original seems to wait 5 sync)
			{
				process_events();
			}

			if (g_engine->_engine->key) {
				// stopSample();
				quitPlayback = 1;
				break;
			}
		}

		fadeOutVar--;

		if (fadeOutVar == 0) {
			quitPlayback = 1;
		}
	}

	g_engine->_engine->flagInitView = 2;
}

void setWaterHeight(int height) {
	g_engine->_engine->waterHeight = height;
}

void saveAmbiance() {
	g_engine->_engine->saveFlagRotPal = g_engine->_engine->flagRotPal;
	g_engine->_engine->saveShakeVar1 = g_engine->_engine->shakeVar1;
	g_engine->_engine->shakeVar1 = 0;
	g_engine->_engine->flagRotPal = 0;
	// TODO: clearShake();
	setWaterHeight(10000);
}

void restoreAmbiance() {
	g_engine->_engine->flagRotPal = g_engine->_engine->saveFlagRotPal;
	g_engine->_engine->shakeVar1 = g_engine->_engine->saveShakeVar1;
	if (g_engine->_engine->saveFlagRotPal) {
		setWaterHeight(-600);
	} else {
		setWaterHeight(10000);
	}
}

void stopShaking() {}

void fadeLevelDestPal(byte *pal1, byte *pal2, int coef) {
	for (int i = 0; i < 3 * 256; i++) {
		int color = *pal1 + (*pal2 - *pal1) * (coef / 256.0);
		*pal2 = color;
		pal1++;
		pal2++;
	}
}

static void setFadePalette(byte *pal1, byte *pal2, int coef) {
	fadeLevelDestPal(pal1, pal2, coef);
	gfx_setPalette(pal2);
}

static void affCbm(byte *p1, byte *p2) {
	do {
		int n = *p1;
		p1++;
		if (n == 0) {
			return;
		}
		n--;
		if (n == 0) {
			n = *p1;
			p1++;
			if (n == 0) {
				p2++;
			} else {
				*p2++ = n;
			}
		} else {
			n--;
			if (n == 0) {
				n = *p1;
				p1++;
				if (n == 0) {
					p2 += 2;
				} else {
					*p2++ = n;
					*p2++ = n;
				}
			} else {
				n--;
				if (n == 0) {
					n = *p1;
					p1++;
					int c = *p1;
					p1++;
					if (c == 0) {
						p2 += n;
					} else {
						for (int i = 0; i < n; ++i) {
							*p2++ = c;
						}
					}
				} else {
					n = *(uint16 *)p1;
					p1 += 2;
					int c = *p1;
					p1++;
					if (c == 0) {
						p2 += n;
					} else {
						for (int i = 0; i < n; ++i) {
							*p2++ = c;
						}
					}
				}
			}
		}
	} while (true);
}

static void endSequence() {
	pakLoad("CAMERA06.PAK", 7, g_engine->_engine->aux);
	gfx_copyBlockPhys(g_engine->_engine->aux, 0, 0, 320, 200);
	osystem_drawBackground();
	osystem_updateScreen();

	pakLoad("ENDSEQ.PAK", 0, g_engine->_engine->aux);
	fastCopyScreen(g_engine->_engine->aux + 770, g_engine->_engine->aux2);

	byte pal1[256 * 3];
	byte pal2[256 * 3];
	for (int i = 0; i < 256; ++i) {
		process_events();
		copyPalette(g_engine->_engine->aux + 2, pal2);
		setFadePalette(g_engine->_engine->currentGamePalette, pal2, i);
	}
	gfx_copyBlockPhys(g_engine->_engine->aux2, 0, 0, 320, 200);
	osystem_drawBackground();
	osystem_updateScreen();

	for (int i = 0; i < 2; ++i) {
		for (int j = 1; j < 12; ++j) {
			process_events();
			fastCopyScreen(g_engine->_engine->aux2, g_engine->_engine->logicalScreen);
			pakLoad("ENDSEQ.PAK", j, g_engine->_engine->aux);
			affCbm(g_engine->_engine->aux, g_engine->_engine->logicalScreen);
			gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
			osystem_drawBackground();
			osystem_updateScreen();
		}
	}
	for (int i = 0; i < 2; ++i) {
		for (int j = 12; j < 23; ++j) {
			process_events();
			fastCopyScreen(g_engine->_engine->aux2, g_engine->_engine->logicalScreen);
			pakLoad("ENDSEQ.PAK", j, g_engine->_engine->aux);
			affCbm(g_engine->_engine->aux, g_engine->_engine->logicalScreen);
			gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
			osystem_drawBackground();
			osystem_updateScreen();
		}
	}
	for (int i = 0; i < 2; ++i) {
		for (int j = 23; j < 32; ++j) {
			process_events();
			fastCopyScreen(g_engine->_engine->aux2, g_engine->_engine->logicalScreen);
			pakLoad("ENDSEQ.PAK", j, g_engine->_engine->aux);
			affCbm(g_engine->_engine->aux, g_engine->_engine->logicalScreen);
			gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
			osystem_drawBackground();
			osystem_updateScreen();
		}
	}
	for (int i = 0; i < 256; i += 32) {
		process_events();
		paletteFill(pal1, 255, 255, 0);
		setFadePalette(pal2, pal1, i);
	}
	memset(g_engine->_engine->logicalScreen, 0, 320 * 200);
	gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
	osystem_drawBackground();
	osystem_updateScreen();
	paletteFill(pal2, 255, 255, 0);
	for (int i = 0; i < 256; i += 16) {
		process_events();
		paletteFill(pal1, 255, 0, 0);
		setFadePalette(pal2, pal1, i);
	}
	g_engine->_engine->fadeState = 2;
	g_engine->_engine->flagInitView = 2;
}

void processLife(int lifeNum, bool callFoundLife) {
	int exitLife = 0;
	// int switchVal = 0;
	int var_6;
	int switchVal = 0;

	g_engine->_engine->currentLifeActorIdx = g_engine->_engine->currentProcessedActorIdx;
	g_engine->_engine->currentLifeActorPtr = g_engine->_engine->currentProcessedActorPtr;
	g_engine->_engine->currentLifeNum = lifeNum;

	g_engine->_engine->currentLifePtr = hqrGet(g_engine->_engine->listLife, lifeNum);
	assert(g_engine->_engine->currentLifePtr);

	while (!::Engine::shouldQuit() && !exitLife) {
		int lifeTempVar1;
		int lifeTempVar2;
		int16 currentOpcode;

		var_6 = -1;

		currentOpcode = *(int16 *)g_engine->_engine->currentLifePtr;
		g_engine->_engine->currentLifePtr += 2;

		// #ifdef DEBUG
		// 		strcpy(currentDebugLifeLine, "");
		// #endif
		// appendFormated("%d:opcode: %02X: ", lifeNum, currentOpcode & 0xFFFF);

		if (currentOpcode & 0x8000) {
			var_6 = *(int16 *)g_engine->_engine->currentLifePtr;
			g_engine->_engine->currentLifePtr += 2;

			if (var_6 == -1) {
				error("Unsupported newVar = -1\n");
				assert(0);
			} else {
				g_engine->_engine->currentProcessedActorIdx = g_engine->_engine->worldObjets[var_6].objIndex;

				if (g_engine->_engine->currentProcessedActorIdx != -1) {
					g_engine->_engine->currentProcessedActorPtr = &g_engine->_engine->objectTable[g_engine->_engine->currentProcessedActorIdx];

					goto processOpcode;
				} else {
					int opcodeLocated;

					if (g_engine->getGameId() == GID_AITD1) {
						opcodeLocated = aitd1LifeMacroTable[currentOpcode & 0x7FFF];
					} else {
						opcodeLocated = aitd2LifeMacroTable[currentOpcode & 0x7FFF];
					}

					switch (opcodeLocated) {
						////////////////////////////////////////////////////////////////////////
					case LM_BODY: {
						g_engine->_engine->worldObjets[var_6].body = evalVar();
						break;
					}
					case LM_BODY_RESET: {
						g_engine->_engine->worldObjets[var_6].body = evalVar();
						g_engine->_engine->worldObjets[var_6].anim = evalVar();
						break;
					}
					case LM_TYPE: {
						lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr & TYPE_MASK;
						g_engine->_engine->currentLifePtr += 2;

						lifeTempVar2 = g_engine->_engine->worldObjets[var_6].flags;

						g_engine->_engine->worldObjets[var_6].flags = (g_engine->_engine->worldObjets[var_6].flags & ~TYPE_MASK) + lifeTempVar1;
						break;
					}
					////////////////////////////////////////////////////////////////////////
					case LM_ANIM_ONCE: {
						g_engine->_engine->worldObjets[var_6].anim = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;
						g_engine->_engine->worldObjets[var_6].animInfo = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;
						g_engine->_engine->worldObjets[var_6].animType = ANIM_ONCE;
						if (g_engine->getGameId() >= GID_JACK)
							g_engine->_engine->worldObjets[var_6].frame = 0;
						break;
					}
					case LM_ANIM_REPEAT: {
						g_engine->_engine->worldObjets[var_6].anim = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;
						g_engine->_engine->worldObjets[var_6].animInfo = -1;
						g_engine->_engine->worldObjets[var_6].animType = ANIM_REPEAT;
						if (g_engine->getGameId() >= GID_JACK)
							g_engine->_engine->worldObjets[var_6].frame = 0;
						break;
					}
					case LM_ANIM_ALL_ONCE: {
						g_engine->_engine->worldObjets[var_6].anim = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;
						g_engine->_engine->worldObjets[var_6].animInfo = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;
						g_engine->_engine->worldObjets[var_6].animType = ANIM_ONCE | ANIM_UNINTERRUPTABLE;
						if (g_engine->getGameId() >= GID_JACK)
							g_engine->_engine->worldObjets[var_6].frame = 0;
						break;
					}
					case LM_ANIM_RESET: {
						assert(g_engine->getGameId() >= GID_JACK);
						g_engine->_engine->worldObjets[var_6].anim = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;
						g_engine->_engine->worldObjets[var_6].animInfo = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;
						g_engine->_engine->worldObjets[var_6].animType = ANIM_ONCE | ANIM_RESET;
						g_engine->_engine->worldObjets[var_6].frame = 0;
						break;
					}
					////////////////////////////////////////////////////////////////////////
					case LM_MOVE: // MOVE
					{
						g_engine->_engine->worldObjets[var_6].trackMode = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						g_engine->_engine->worldObjets[var_6].trackNumber = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						g_engine->_engine->worldObjets[var_6].positionInTrack = 0;

						if (g_engine->getGameId() > GID_AITD1) {
							g_engine->_engine->worldObjets[var_6].mark = -1;
						}
						break;
					}
					case LM_ANGLE: {
						g_engine->_engine->worldObjets[var_6].alpha = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						g_engine->_engine->worldObjets[var_6].beta = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						g_engine->_engine->worldObjets[var_6].gamma = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						break;
					}
					case LM_STAGE: // stage
					{
						g_engine->_engine->worldObjets[var_6].stage = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						g_engine->_engine->worldObjets[var_6].room = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						g_engine->_engine->worldObjets[var_6].x = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						g_engine->_engine->worldObjets[var_6].y = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						g_engine->_engine->worldObjets[var_6].z = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						// FlagGenereActiveList = 1;

						break;
					}
					case LM_TEST_COL: {
						if (*(int16 *)g_engine->_engine->currentLifePtr) {
							g_engine->_engine->worldObjets[var_6].flags |= 0x20;
						} else {
							g_engine->_engine->worldObjets[var_6].flags &= 0xFFDF;
						}

						g_engine->_engine->currentLifePtr += 2;

						break;
					}
					////////////////////////////////////////////////////////////////////////
					case LM_LIFE: {
						g_engine->_engine->worldObjets[var_6].life = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;
						break;
					}
					case LM_LIFE_MODE: // LIFE_MODE
					{
						lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						if (lifeTempVar1 != g_engine->_engine->worldObjets[var_6].lifeMode) {
							g_engine->_engine->worldObjets[var_6].lifeMode = lifeTempVar1;
							// FlagGenereActiveList = 1;
						}
						break;
					}
					case LM_FOUND_NAME: // FOUND_NAME
					{
						g_engine->_engine->worldObjets[var_6].foundName = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						break;
					}
					case LM_FOUND_BODY: // FOUND_BODY
					{
						g_engine->_engine->worldObjets[var_6].foundBody = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						break;
					}
					case LM_FOUND_FLAG: // FOUND_FLAG
					{
						g_engine->_engine->worldObjets[var_6].flags2 &= 0xE000;
						g_engine->_engine->worldObjets[var_6].flags2 |= *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						break;
					}
					case LM_FOUND_WEIGHT: {
						g_engine->_engine->worldObjets[var_6].positionInTrack = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						break;
					}
					case LM_START_CHRONO: // arrive in original interpreter too
					{
						break;
					}
					////////////////////////////////////////////////////////////////////////
					default: {
						error("Unsupported opcode 0x%X when actor isn't in floor\n", currentOpcode & 0x7FFF);
						break;
					}
					}
				}
			}
		} else {
			int lifeTempVar7;
			int lifeTempVar6;
			int lifeTempVar4;
			int lifeTempVar3;
			int lifeTempVar5;
			int opcodeLocated;
		processOpcode:

			if (g_engine->getGameId() == GID_AITD1) {
				opcodeLocated = aitd1LifeMacroTable[currentOpcode & 0x7FFF];
			} else {
				opcodeLocated = aitd2LifeMacroTable[currentOpcode & 0x7FFF];
			}

			switch (opcodeLocated) {
			case LM_BODY: {
				lifeTempVar1 = evalVar();

				g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].body = lifeTempVar1;

				if (g_engine->_engine->currentProcessedActorPtr->bodyNum != lifeTempVar1) {
					g_engine->_engine->currentProcessedActorPtr->bodyNum = lifeTempVar1;

					if (g_engine->_engine->currentProcessedActorPtr->_flags & AF_ANIMATED) {
						if (g_engine->_engine->currentProcessedActorPtr->ANIM != -1 && g_engine->_engine->currentProcessedActorPtr->bodyNum != -1) {
							byte *pAnim = hqrGet(g_engine->_engine->listAnim, g_engine->_engine->currentProcessedActorPtr->ANIM);
							byte *pBody;

							if (g_engine->getGameId() >= GID_JACK) {
								/*                  if (bFlagDecal)
								gereDecal(); */
							}

							pBody = hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentProcessedActorPtr->bodyNum);

							/*    if(gameId >= GID_JACK)
							{
							setInterAnimObject2(g_engine->_engine->currentProcessedActorPtr->FRAME, pAnim, pBody, TRUE, Objet->AnimDecal);
							}
							else */
							{
								setInterAnimObjet(g_engine->_engine->currentProcessedActorPtr->FRAME, pAnim, pBody);
							}
						}
					} else {
						g_engine->_engine->flagInitView = 1;
					}
				}
				break;
			}
			case LM_BODY_RESET: {
				// appendFormated("LM_BODY_RESET ");

				int param1 = evalVar("body");
				int param2 = evalVar("anim");

				g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].body = param1;
				g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].anim = param2;

				g_engine->_engine->currentProcessedActorPtr->bodyNum = param1;

				if (g_engine->_engine->currentProcessedActorPtr->_flags & AF_ANIMATED) {
					byte *pAnim = hqrGet(g_engine->_engine->listAnim, g_engine->_engine->currentProcessedActorPtr->ANIM);
					byte *pBody;

					if (g_engine->getGameId() >= GID_JACK) {
						/*                  if (bFlagDecal)
						gereDecal(); */
					}
					pBody = hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentProcessedActorPtr->bodyNum);

					setAnimObjet(0, pAnim, pBody);
					initAnim(param2, 4, -1);
				} else {
					g_engine->_engine->flagInitView = 1;
				}
				break;
			}
			case LM_DO_REAL_ZV: {
				// appendFormated("LM_DO_REAL_ZV ");
				doRealZv(g_engine->_engine->currentProcessedActorPtr);
				break;
			}
			case LM_DEF_ZV: // DEF_ZV
			{
				// appendFormated("LM_DEF_ZV ");
				g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 = g_engine->_engine->currentProcessedActorPtr->roomX + *(int16 *)g_engine->_engine->currentLifePtr + g_engine->_engine->currentProcessedActorPtr->stepX;
				g_engine->_engine->currentLifePtr += 2;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 = g_engine->_engine->currentProcessedActorPtr->roomX + *(int16 *)g_engine->_engine->currentLifePtr + g_engine->_engine->currentProcessedActorPtr->stepX;
				g_engine->_engine->currentLifePtr += 2;

				g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 = g_engine->_engine->currentProcessedActorPtr->roomY + *(int16 *)g_engine->_engine->currentLifePtr + g_engine->_engine->currentProcessedActorPtr->stepY;
				g_engine->_engine->currentLifePtr += 2;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 = g_engine->_engine->currentProcessedActorPtr->roomY + *(int16 *)g_engine->_engine->currentLifePtr + g_engine->_engine->currentProcessedActorPtr->stepY;
				g_engine->_engine->currentLifePtr += 2;

				g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 = g_engine->_engine->currentProcessedActorPtr->roomZ + *(int16 *)g_engine->_engine->currentLifePtr + g_engine->_engine->currentProcessedActorPtr->stepZ;
				g_engine->_engine->currentLifePtr += 2;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 = g_engine->_engine->currentProcessedActorPtr->roomZ + *(int16 *)g_engine->_engine->currentLifePtr + g_engine->_engine->currentProcessedActorPtr->stepZ;
				g_engine->_engine->currentLifePtr += 2;

				break;
			}
			case LM_DEF_ABS_ZV: {
				// appendFormated("LM_DEF_ABS_ZV ");
				g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				break;
			}
			case LM_DO_ROT_ZV: {
				// appendFormated("LM_DO_ROT_ZV ");
				getZvRot(hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentProcessedActorPtr->bodyNum), &g_engine->_engine->currentProcessedActorPtr->zv,
						 g_engine->_engine->currentProcessedActorPtr->alpha,
						 g_engine->_engine->currentProcessedActorPtr->beta,
						 g_engine->_engine->currentProcessedActorPtr->gamma);

				g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 += g_engine->_engine->currentProcessedActorPtr->roomX;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 += g_engine->_engine->currentProcessedActorPtr->roomX;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += g_engine->_engine->currentProcessedActorPtr->roomY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += g_engine->_engine->currentProcessedActorPtr->roomY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 += g_engine->_engine->currentProcessedActorPtr->roomZ;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 += g_engine->_engine->currentProcessedActorPtr->roomZ;

				break;
			}
			case LM_DO_MAX_ZV: {
				getZvMax(hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentProcessedActorPtr->bodyNum), &g_engine->_engine->currentProcessedActorPtr->zv);

				g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 += g_engine->_engine->currentProcessedActorPtr->roomX;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 += g_engine->_engine->currentProcessedActorPtr->roomX;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += g_engine->_engine->currentProcessedActorPtr->roomY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += g_engine->_engine->currentProcessedActorPtr->roomY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 += g_engine->_engine->currentProcessedActorPtr->roomZ;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 += g_engine->_engine->currentProcessedActorPtr->roomZ;

				break;
			}
			case LM_DO_NORMAL_ZV: {
				giveZVObjet(hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentProcessedActorPtr->bodyNum), &g_engine->_engine->currentProcessedActorPtr->zv);

				g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 += g_engine->_engine->currentProcessedActorPtr->roomX;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 += g_engine->_engine->currentProcessedActorPtr->roomX;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += g_engine->_engine->currentProcessedActorPtr->roomY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += g_engine->_engine->currentProcessedActorPtr->roomY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 += g_engine->_engine->currentProcessedActorPtr->roomZ;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 += g_engine->_engine->currentProcessedActorPtr->roomZ;

				break;
			}
			case LM_DO_CARRE_ZV: {
				// appendFormated("LM_DO_CARRE_ZV ");
				getZvCube(hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentProcessedActorPtr->bodyNum), &g_engine->_engine->currentProcessedActorPtr->zv);

				g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 += g_engine->_engine->currentProcessedActorPtr->roomX;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 += g_engine->_engine->currentProcessedActorPtr->roomX;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += g_engine->_engine->currentProcessedActorPtr->roomY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += g_engine->_engine->currentProcessedActorPtr->roomY;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 += g_engine->_engine->currentProcessedActorPtr->roomZ;
				g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 += g_engine->_engine->currentProcessedActorPtr->roomZ;

				break;
			}
			case LM_TYPE: // TYPE
			{
				// appendFormated("LM_TYPE ");

				lifeTempVar1 = readNextArgument("type") & AF_MASK;
				lifeTempVar2 = g_engine->_engine->currentProcessedActorPtr->_flags;

				g_engine->_engine->currentProcessedActorPtr->_flags = (g_engine->_engine->currentProcessedActorPtr->_flags & ~AF_MASK) + lifeTempVar1;

				if (g_engine->getGameId() > GID_AITD1) {
					if (lifeTempVar2 & 1) {
						if (!(lifeTempVar1 & 1)) {
							addActorToBgInscrust(g_engine->_engine->currentProcessedActorIdx);
						}
					}

					if (lifeTempVar1 & 1) {
						if (!(lifeTempVar2 & 8)) {
							removeFromBGIncrust(g_engine->_engine->currentProcessedActorIdx);
						}
					}
				}

				break;
			}
			case LM_GET_HARD_CLIP: // GET_HARD_CLIP
			{
				// appendFormated("LM_GET_HARD_CLIP ");
				getHardClip();
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_ANIM_ONCE: {
				// appendFormated("LM_ANIM_ONCE ");

				lifeTempVar1 = readNextArgument("Anim");
				lifeTempVar2 = readNextArgument("Flags");

				if (lifeTempVar1 == -1) {
					g_engine->_engine->currentProcessedActorPtr->ANIM = -1;
					g_engine->_engine->currentProcessedActorPtr->newAnim = -2;
				} else {
					initAnim(lifeTempVar1, 0, lifeTempVar2);
				}

				break;
			}
			case LM_ANIM_REPEAT: {
				// appendFormated("LM_ANIM_REPEAT ");
				lifeTempVar1 = readNextArgument("Anim");

				initAnim(lifeTempVar1, 1, -1);

				break;
			}
			case LM_ANIM_ALL_ONCE: {
				// appendFormated("LM_ANIM_ALL_ONCE ");
				lifeTempVar1 = readNextArgument("Anim");
				lifeTempVar2 = readNextArgument("Flags");

				initAnim(lifeTempVar1, 2, lifeTempVar2);

				break;
			}
			case LM_ANIM_RESET: {
				// appendFormated("LM_ANIM_RESET ");
				int anim = readNextArgument("Anim");
				int animFlag = readNextArgument("Flags");

				if (anim == -1) {
					g_engine->_engine->currentProcessedActorPtr->ANIM = -1;
					g_engine->_engine->currentProcessedActorPtr->newAnim = -2;
				} else {
					initAnim(anim, 4, animFlag);
				}

				break;
			}
			case LM_ANIM_HYBRIDE_ONCE: {
				// appendFormated("LM_ANIM_HYBRIDE_ONCE ");
				// TODO
				(void)readNextArgument("Anim");
				(void)readNextArgument("Body?");

				// printf("LM_ANIM_HYBRIDE_ONCE(anim:%d, body:%d)\n", anim, body);
				break;
			}
			case LM_ANIM_HYBRIDE_REPEAT: {
				// appendFormated("LM_ANIM_HYBRIDE_REPEAT ");
				// TODO
				(void)readNextArgument("Anim");
				(void)readNextArgument("Body?");

				// printf("LM_ANIM_HYBRIDE_REPEAT(anim:%d, body:%d)\n", anim, body);
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_HIT: {
				// appendFormated("LM_HIT ");

				lifeTempVar1 = readNextArgument("Anim");
				lifeTempVar2 = readNextArgument("StartFrame");
				lifeTempVar3 = readNextArgument("GroupNumber");
				lifeTempVar4 = readNextArgument("HitBoxSize");
				lifeTempVar5 = evalVar("HitForce");
				lifeTempVar6 = readNextArgument("NextAnim");

				hit(lifeTempVar1, lifeTempVar2, lifeTempVar3, lifeTempVar4, lifeTempVar5, lifeTempVar6);

				break;
			}
			case LM_FIRE: // FIRE
			{
				// appendFormated("LM_FIRE ");
				if (g_engine->getGameId() == GID_AITD1) {
					int fireAnim;
					int shootFrame;
					int emitPoint;
					int zvSize;
					int hitForce;
					int nextAnim;

					fireAnim = readNextArgument("Anim");
					shootFrame = readNextArgument("Frame");
					emitPoint = readNextArgument("EmitPoint");
					zvSize = readNextArgument("ZVSize");
					hitForce = readNextArgument("Force");
					nextAnim = readNextArgument("NextAnim");

					fire(fireAnim, shootFrame, emitPoint, zvSize, hitForce, nextAnim);
				} else // use an emitter model
				{
					int fireAnim = evalVar("Anim");
					int shootFrame = readNextArgument("Frame");
					int emitPoint = readNextArgument("EmitPoint");
					/*int emitModel = */ readNextArgument("EmitModel");
					int zvSize = readNextArgument("ZVSize");
					int hitForce = readNextArgument("Force");
					int nextAnim = evalVar();

					fire(fireAnim, shootFrame, emitPoint, zvSize, hitForce, nextAnim); // todo: implement emit model
				}

				break;
			}
			case LM_FIRE_UP_DOWN: // TODO AITD3 only
			{
				// appendFormated("LM_FIRE_UP_DOWN ");

				evalVar();
				g_engine->_engine->currentLifePtr += 12;
				evalVar();
				break;
			}
			case LM_HIT_OBJECT: // HIT_OBJECT
			{
				// appendFormated("LM_HIT_OBJECT ");

				lifeTempVar1 = readNextArgument("Flags");
				lifeTempVar2 = readNextArgument("Force");

				g_engine->_engine->currentProcessedActorPtr->animActionType = 8;
				g_engine->_engine->currentProcessedActorPtr->animActionParam = lifeTempVar1;
				g_engine->_engine->currentProcessedActorPtr->hitForce = lifeTempVar2;
				g_engine->_engine->currentProcessedActorPtr->hotPointID = -1;
				break;
			}
			case LM_STOP_HIT_OBJECT: // cancel hit obj
			{
				// appendFormated("LM_STOP_HIT_OBJECT ");
				if (g_engine->_engine->currentProcessedActorPtr->animActionType == 8) {
					g_engine->_engine->currentProcessedActorPtr->animActionType = 0;
					g_engine->_engine->currentProcessedActorPtr->animActionParam = 0;
					g_engine->_engine->currentProcessedActorPtr->hitForce = 0;
					g_engine->_engine->currentProcessedActorPtr->hotPointID = -1;
				}

				break;
			}
			case LM_THROW: // throw
			{
				// appendFormated("LM_THROW ");
				lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar3 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar4 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar5 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar6 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar7 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				throwObj(lifeTempVar1, lifeTempVar2, lifeTempVar3, lifeTempVar4, lifeTempVar5, lifeTempVar6, lifeTempVar7);

				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_MOVE: {
				// appendFormated("LM_MOVE ");
				lifeTempVar1 = readNextArgument("TrackMode");
				lifeTempVar2 = readNextArgument("TrackNumber");

				setMoveMode(lifeTempVar1, lifeTempVar2);

				break;
			}
			case LM_RESET_MOVE_MANUAL: {
				// appendFormated("LM_RESET_MOVE_MANUAL ");
				resetRotateParam();
				break;
			}
			case LM_CONTINUE_TRACK: {
				// appendFormated("LM_CONTINUE_TRACK ");
				byte *ptr = hqrGet(g_engine->_engine->listTrack, g_engine->_engine->currentProcessedActorPtr->trackNumber);

				ptr += g_engine->_engine->currentProcessedActorPtr->positionInTrack * 2;

				if (*(int16 *)ptr == 5) {
					g_engine->_engine->currentProcessedActorPtr->positionInTrack++;
				}
				break;
			}
			case LM_DO_MOVE: {
				// appendFormated("LM_DO_MOVE ");
				if (g_engine->getGameId() == GID_AITD1) {
					processTrack();
				} else {
					processTrack2();
				}
				break;
			}
			case LM_ANIM_MOVE: {
				// appendFormated("LM_ANIM_MOVE ");
				int animStand = readNextArgument("animStand");
				int animWalk = readNextArgument("animWalk");
				int animRun = readNextArgument("animRun");
				int animStop = readNextArgument("animStop");
				int animWalkBackward = readNextArgument("animWalkBackward");
				int animTurnRight = readNextArgument("animTurnRight");
				int animTurnLeft = readNextArgument("animTurnLeft");

				animMove(animStand, animWalk, animRun, animStop, animWalkBackward, animTurnRight, animTurnLeft);

				break;
			}
			case LM_MANUAL_ROT: // MANUAL_ROT
			{
				// appendFormated("LM_MANUAL_ROT ");
				if (g_engine->getGameId() == GID_AITD1) {
					gereManualRot(240);
				} else {
					gereManualRot(90);
				}
				break;
			}
			case LM_SET_BETA: // SET_BETA
			{
				// appendFormated("LM_SET_BETA ");
				lifeTempVar1 = readNextArgument("beta");
				lifeTempVar2 = readNextArgument("speed");

				if (g_engine->_engine->currentProcessedActorPtr->beta != lifeTempVar1) {
					if (g_engine->_engine->currentProcessedActorPtr->rotate.param == 0 || g_engine->_engine->currentProcessedActorPtr->rotate.newValue != lifeTempVar1) {
						initRealValue(g_engine->_engine->currentProcessedActorPtr->beta, lifeTempVar1, lifeTempVar2, &g_engine->_engine->currentProcessedActorPtr->rotate);
					}

					g_engine->_engine->currentProcessedActorPtr->beta = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
				}

				break;
			}
			case LM_SET_ALPHA: // SET_ALPHA
			{
				// appendFormated("LM_SET_ALPHA ");
				lifeTempVar1 = readNextArgument("alpha");
				lifeTempVar2 = readNextArgument("speed");

				if (g_engine->_engine->currentProcessedActorPtr->alpha != lifeTempVar1) {
					if (g_engine->_engine->currentProcessedActorPtr->rotate.param == 0 || lifeTempVar1 != g_engine->_engine->currentProcessedActorPtr->rotate.newValue) {
						initRealValue(g_engine->_engine->currentProcessedActorPtr->alpha, lifeTempVar1, lifeTempVar2, &g_engine->_engine->currentProcessedActorPtr->rotate);
					}

					g_engine->_engine->currentProcessedActorPtr->alpha = updateActorRotation(&g_engine->_engine->currentProcessedActorPtr->rotate);
				}

				break;
			}
			case LM_ANGLE: // ANGLE
			{
				// appendFormated("LM_ANGLE ");
				g_engine->_engine->currentProcessedActorPtr->alpha = readNextArgument("alpha");
				g_engine->_engine->currentProcessedActorPtr->beta = readNextArgument("beta");
				g_engine->_engine->currentProcessedActorPtr->gamma = readNextArgument("gamma");

				break;
			}
			case LM_COPY_ANGLE: {
				// appendFormated("LM_COPY_ANGLE ");
				int object = readNextArgument("object");
				int localObjectIndex = g_engine->_engine->worldObjets[object].objIndex;
				if (localObjectIndex == -1) {
					g_engine->_engine->currentProcessedActorPtr->alpha = g_engine->_engine->worldObjets[object].alpha;
					g_engine->_engine->currentProcessedActorPtr->beta = g_engine->_engine->worldObjets[object].beta;
					g_engine->_engine->currentProcessedActorPtr->gamma = g_engine->_engine->worldObjets[object].gamma;
				} else {
					g_engine->_engine->currentProcessedActorPtr->alpha = g_engine->_engine->objectTable[localObjectIndex].alpha;
					g_engine->_engine->currentProcessedActorPtr->beta = g_engine->_engine->objectTable[localObjectIndex].beta;
					g_engine->_engine->currentProcessedActorPtr->gamma = g_engine->_engine->objectTable[localObjectIndex].gamma;
				}
				break;
			}
			case LM_STAGE: // STAGE
			{
				// appendFormated("LM_STAGE ");
				lifeTempVar1 = readNextArgument("newStage");
				lifeTempVar2 = readNextArgument("g_engine->_engine->newRoom");
				lifeTempVar3 = readNextArgument("X");
				lifeTempVar4 = readNextArgument("Y");
				lifeTempVar5 = readNextArgument("Z");

				setStage(lifeTempVar1, lifeTempVar2, lifeTempVar3, lifeTempVar4, lifeTempVar5);

				break;
			}
			case LM_TEST_COL: // TEST_COL
			{
				// appendFormated("LM_TEST_COL ");
				lifeTempVar1 = readNextArgument();

				if (lifeTempVar1) {
					g_engine->_engine->currentProcessedActorPtr->dynFlags |= 1;
				} else {
					g_engine->_engine->currentProcessedActorPtr->dynFlags &= 0xFFFE;
				}

				break;
			}
			case LM_UP_COOR_Y: // UP_COOR_Y
			{
				// appendFormated("LM_UP_COOR_Y ");
				initRealValue(0, -2000, -1, &g_engine->_engine->currentProcessedActorPtr->YHandler);
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_LIFE: // LIFE
			{
				// appendFormated("LM_LIFE ");
				g_engine->_engine->currentProcessedActorPtr->life = readNextArgument("newLife");
				break;
			}
			case LM_STAGE_LIFE: {
				// appendFormated("LM_STAGE_LIFE ");
				g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].floorLife = readNextArgument("stageLife");
				break;
			}
			case LM_LIFE_MODE: // LIFE_MODE
			{
				// appendFormated("LM_LIFE_MODE ");
				lifeTempVar1 = readNextArgument("lifeMode");

				if (g_engine->getGameId() <= GID_JACK) {
					lifeTempVar2 = g_engine->_engine->currentProcessedActorPtr->lifeMode;
				} else {
					lifeTempVar2 = g_engine->_engine->currentProcessedActorPtr->lifeMode & 3;
				}

				if (lifeTempVar1 != lifeTempVar2) {
					g_engine->_engine->currentProcessedActorPtr->lifeMode = lifeTempVar1;
					// FlagGenereActiveList = 1;
				}
				break;
			}
			case LM_DELETE: // DELETE
			{
				// appendFormated("LM_DELETE ");
				if (g_engine->getGameId() <= GID_JACK) {
					lifeTempVar1 = readNextArgument("ObjectId");
				} else {
					lifeTempVar1 = evalVar("ObjectId");
				}

				deleteObject(lifeTempVar1);

				if (g_engine->_engine->worldObjets[lifeTempVar1].foundBody != -1) {
					if (g_engine->getGameId() == GID_AITD1) // TODO: check, really useful ?
					{
						g_engine->_engine->worldObjets[lifeTempVar1].flags2 &= 0x7FFF;
					}
					g_engine->_engine->worldObjets[lifeTempVar1].flags2 |= 0x4000;
				}

				break;
			}
			case LM_SPECIAL: // SPECIAL
			{
				// appendFormated("LM_SPECIAL ");
				lifeTempVar1 = readNextArgument("type");

				switch (lifeTempVar1) {
				case 0: // evaporate
				{
					initSpecialObjet(0,
									 g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX,
									 g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY,
									 g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ,
									 g_engine->_engine->currentProcessedActorPtr->stage,
									 g_engine->_engine->currentProcessedActorPtr->room,
									 g_engine->_engine->currentProcessedActorPtr->alpha,
									 g_engine->_engine->currentProcessedActorPtr->beta,
									 g_engine->_engine->currentProcessedActorPtr->gamma,
									 &g_engine->_engine->currentProcessedActorPtr->zv);
					break;
				}
				case 1: // flow
				{
					g_engine->_engine->currentProcessedActorPtr = &g_engine->_engine->objectTable[g_engine->_engine->currentProcessedActorPtr->HIT_BY];

					initSpecialObjet(1,
									 g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX + g_engine->_engine->currentProcessedActorPtr->hotPoint.x,
									 g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY + g_engine->_engine->currentProcessedActorPtr->hotPoint.y,
									 g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ + g_engine->_engine->currentProcessedActorPtr->hotPoint.z,
									 g_engine->_engine->currentProcessedActorPtr->stage,
									 g_engine->_engine->currentProcessedActorPtr->room,
									 0,
									 -g_engine->_engine->currentProcessedActorPtr->beta,
									 0,
									 &g_engine->_engine->currentProcessedActorPtr->zv);

					g_engine->_engine->currentProcessedActorPtr = g_engine->_engine->currentLifeActorPtr;

					break;
				}
				case 4: // cigar smoke
				{
					initSpecialObjet(4,
									 g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX,
									 g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY,
									 g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ,
									 g_engine->_engine->currentProcessedActorPtr->stage,
									 g_engine->_engine->currentProcessedActorPtr->room,
									 g_engine->_engine->currentProcessedActorPtr->alpha,
									 g_engine->_engine->currentProcessedActorPtr->beta,
									 g_engine->_engine->currentProcessedActorPtr->gamma,
									 &g_engine->_engine->currentProcessedActorPtr->zv);
					break;
				}
				}
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_START_CHRONO: // START_CHRONO
			{
				// appendFormated("LM_START_CHRONO ");
				startChrono(&g_engine->_engine->currentProcessedActorPtr->CHRONO);
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_FOUND: // FOUND
			{
				// appendFormated("LM_FOUND ");
				lifeTempVar1 = readNextArgument("ObjectId");

				if (g_engine->getGameId() == GID_AITD1) {
					foundObject(lifeTempVar1, 1);
				} else {
					if (callFoundLife) {
						foundObject(lifeTempVar1, 2);
					} else {
						foundObject(lifeTempVar1, 1);
					}
				}

				break;
			}
			case LM_TAKE: // TAKE
			{
				// appendFormated("LM_TAKE ");
				if (g_engine->getGameId() >= GID_TIMEGATE) {
					(void)evalVar();
					(void)readNextArgument();
					(void)readNextArgument();
					(void)readNextArgument();
				} else {
					lifeTempVar1 = readNextArgument("ObjectId");

					take(lifeTempVar1);
				}

				break;
			}
			case LM_IN_HAND: // IN_HAND
			{
				// appendFormated("LM_IN_HAND ");
				if (g_engine->getGameId() <= GID_JACK) {
					g_engine->_engine->inHandTable = *(int16 *)g_engine->_engine->currentLifePtr;
					g_engine->_engine->currentLifePtr += 2;
				} else {
					g_engine->_engine->inHandTable = evalVar();
				}
				break;
			}
			case LM_DROP: // DROP
			{
				// appendFormated("LM_DROP ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				drop(lifeTempVar1, lifeTempVar2);

				break;
			}
			case LM_PUT: {
				// appendFormated("LM_PUT ");
				int x;
				int y;
				int z;
				int room;
				int stage;
				int alpha;
				int beta;
				int gamma;
				int idx;

				idx = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				x = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				y = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				z = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				room = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				stage = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				alpha = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				beta = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				gamma = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				put(x, y, z, room, stage, alpha, beta, gamma, idx);

				break;
			}
			case LM_PUT_AT: // PUT_AT
			{
				// appendFormated("LM_PUT_AT ");
				int objIdx1;
				int objIdx2;

				objIdx1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				objIdx2 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				putAtObjet(objIdx1, objIdx2);
				break;
			}
			case LM_FOUND_NAME: // FOUND_NAME
			{
				// appendFormated("LM_FOUND_NAME ");
				g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].foundName = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				break;
			}
			case LM_FOUND_BODY: // FOUND_BODY
			{
				// appendFormated("LM_FOUND_BODY ");
				g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].foundBody = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				break;
			}
			case LM_FOUND_FLAG: // FOUND_FLAG
			{
				// appendFormated("LM_FOUND_FLAG ");
				g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].flags2 &= 0xE000;
				g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].flags2 |= *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				break;
			}
			case LM_FOUND_WEIGHT: // FOUND_WEIGHT
			{
				// appendFormated("LM_FOUND_WEIGHT ");
				g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].positionInTrack = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				break;
			}
			case LM_FOUND_LIFE: // FOUND_LIFE
			{
				// appendFormated("LM_FOUND_LIFE ");
				g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].foundLife = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				break;
			}
			case LM_READ: // READ
			{
				// appendFormated("LM_READ ");
				lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				if (g_engine->getGameId() == GID_AITD1) {
					lifeTempVar3 = *(int16 *)g_engine->_engine->currentLifePtr;
					g_engine->_engine->currentLifePtr += 2; // AITD1 CD has an extra digit, related to the VOC files to play for the text?
				} else {
					lifeTempVar3 = -1;
				}

				fadeOutPhys(0x20, 0);

				readBook(lifeTempVar2 + 1, lifeTempVar1, lifeTempVar3);

				if (g_engine->getGameId() == GID_AITD1) {
					fadeOutPhys(4, 0);
				}

				g_engine->_engine->flagInitView = 2;

				break;
			}
			case LM_READ_ON_PICTURE: // TODO
			{
				int lifeTempVar8;
				// appendFormated("LM_READ_ON_PICTURE ");
				lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar3 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar4 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar5 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar6 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar7 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar8 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				freezeTime();

				fadeOutPhys(32, 0);
				pakLoad("ITD_RESS.PAK", lifeTempVar1, g_engine->_engine->aux);
				byte lpalette[0x300];
				copyPalette(g_engine->_engine->aux + 64000, lpalette);
				convertPaletteIfRequired(lpalette);
				copyPalette(lpalette, g_engine->_engine->currentGamePalette);
				gfx_setPalette(lpalette);
				g_engine->_engine->turnPageFlag = false;
				lire(lifeTempVar2 + 1, lifeTempVar3, lifeTempVar4, lifeTempVar5, lifeTempVar6, 0, lifeTempVar7, lifeTempVar8);

				g_engine->_engine->flagInitView = 2;

				unfreezeTime();

				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_ANIM_SAMPLE: // ANIM_SAMPLE
			{
				// appendFormated("LM_ANIM_SAMPLE ");
				lifeTempVar1 = evalVar();

				if (g_engine->getGameId() == GID_TIMEGATE) {
					g_engine->_engine->currentLifePtr += 2;
				}

				lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar3 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				if (g_engine->_engine->currentProcessedActorPtr->END_FRAME != 0) {
					if (g_engine->_engine->currentProcessedActorPtr->ANIM == lifeTempVar2) {
						if (g_engine->_engine->currentProcessedActorPtr->FRAME == lifeTempVar3) {
							playSound(lifeTempVar1);
							// setSampleFreq(0);
						}
					}
				}
				break;
			}
			case LM_2D_ANIM_SAMPLE: {
				// appendFormated("LM_2D_ANIM_SAMPLE ");
				int sampleNumber = evalVar();
				int16 animNumber = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				int16 frameNumber = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				debug("LM_2D_ANIM_SAMPLE(sampleNumber %d, animNumber %d, frameNumber %d)\n", sampleNumber, animNumber, frameNumber);

				break; // TODO: implement
			}
			case LM_SAMPLE: {
				// appendFormated("LM_SAMPLE ");
				int sampleNumber;

				if (g_engine->getGameId() == GID_TIMEGATE) {
					sampleNumber = evalVar();
					readNextArgument();
				} else if (g_engine->getGameId() <= GID_JACK) {
					sampleNumber = evalVar();
				} else {
					sampleNumber = *(int16 *)g_engine->_engine->currentLifePtr;
					g_engine->_engine->currentLifePtr += 2;
				}

				playSound(sampleNumber);
				// setSampleFreq(0);
				break;
			}
			case LM_REP_SAMPLE: // sample TODO!
			{
				// appendFormated("LM_REP_SAMPLE ");
				if (g_engine->getGameId() == GID_AITD1 || g_engine->getGameId() == GID_TIMEGATE) {
					evalVar();
					g_engine->_engine->currentLifePtr += 2;
				} else {
					g_engine->_engine->currentLifePtr += 4;
				}
				// printf("LM_REP_SAMPLE\n");
				break;
			}
			case LM_STOP_SAMPLE: // todo
			{
				// appendFormated("LM_STOP_SAMPLE ");
				// printf("LM_STOP_SAMPLE\n");

				if (g_engine->getGameId() == GID_TIMEGATE) {
					readNextArgument();
				}

				break;
			}
			case LM_SAMPLE_THEN: // todo
			{
				// appendFormated("LM_SAMPLE_THEN ");
				if (g_engine->getGameId() == GID_AITD1) {
					playSound(evalVar());
					g_engine->_engine->nextSample = evalVar();
				} else {
					int newSample;

					if (g_engine->getGameId() == GID_JACK) {
						newSample = evalVar();
						g_engine->_engine->nextSample = evalVar();
					} else {
						newSample = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;

						g_engine->_engine->nextSample = *(int16 *)g_engine->_engine->currentLifePtr;
						g_engine->_engine->currentLifePtr += 2;
					}

					playSound(newSample);
				}
				// printf("LM_SAMPLE_THEN\n");
				// setSampleFreq(0);
				break;
			}
			case LM_SAMPLE_THEN_REPEAT: // todo
			{
				// appendFormated("LM_SAMPLE_THEN_REPEAT ");
				playSound(evalVar());
				g_engine->_engine->nextSample = evalVar() | 0x4000;
				// setSampleFreq(0);
				// printf("LM_SAMPLE_THEN_REPEAT\n");
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_MUSIC: // MUSIC
			{
				// appendFormated("LM_MUSIC ");
				int newMusicIdx = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				playMusic(newMusicIdx);
				break;
			}
			case LM_NEXT_MUSIC: // TODO
			{
				// appendFormated("LM_NEXT_MUSIC ");
				int musicIdx = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				if (g_engine->_engine->currentMusic == -1) {
					playMusic(musicIdx);
				} else {
					g_engine->_engine->nextMusic = musicIdx;
				}

				break;
			}
			case LM_FADE_MUSIC: // ? fade out music and play another music ?
			{
				// appendFormated("LM_FADE_MUSIC ");
				lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				if (g_engine->_engine->currentMusic != -1) {
					fadeMusic(0, 0, 0x8000);   // fade out music
					startChrono(&musicChrono); // fade out music timer
					g_engine->_engine->currentMusic = -2;         // waiting next music
					g_engine->_engine->nextMusic = lifeTempVar1;  // next music to play
				} else {
					playMusic(lifeTempVar1);
				}

				break;
			}
			case LM_RND_FREQ: // TODO
			{
				// appendFormated("LM_RND_FREQ ");
				// printf("LM_RND_FREQ\n");
				g_engine->_engine->currentLifePtr += 2;
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_LIGHT: // LIGHT
			{
				// appendFormated("LM_LIGHT ");
				lifeTempVar1 = 2 - (*(int16 *)g_engine->_engine->currentLifePtr << 1);
				g_engine->_engine->currentLifePtr += 2;

				if (g_engine->getGameId() >= GID_JACK || !g_engine->_engine->cVars[getCVarsIdx(KILLED_SORCERER)]) {
					if (g_engine->_engine->lightOff != lifeTempVar1) {
						g_engine->_engine->lightOff = lifeTempVar1;
						g_engine->_engine->newFlagLight = 1;
					}
				}

				break;
			}
			case LM_SHAKING: // SHAKING
			{
				// appendFormated("LM_SHAKING ");
				// printf("LM_SHAKING\n");
				g_engine->_engine->saveShakeVar1 = g_engine->_engine->shakeVar1 = *(int16 *)(g_engine->_engine->currentLifePtr);
				g_engine->_engine->currentLifePtr += 2;

				if (g_engine->_engine->shakeVar1 == 0) {
					stopShaking();
				}
				break;
			}
			case LM_PLUIE: {
				// appendFormated("LM_PLUIE ");
				// printf("LM_PLUIE\n");
				//  TODO
				g_engine->_engine->currentLifePtr += 2;
				break;
			}
			case LM_WATER: // ? shaking related
			{
				// appendFormated("LM_WATER ");
				// TODO: Warning, AITD1/AITD2 diff
				// printf("LM_WATER\n");
				g_engine->_engine->flagRotPal = g_engine->_engine->saveFlagRotPal = *(int16 *)(g_engine->_engine->currentLifePtr);
				g_engine->_engine->currentLifePtr += 2;

				if (g_engine->_engine->flagRotPal) {
					setWaterHeight(-600);
				} else {
					setWaterHeight(10000);
				}

				break;
			}
			case LM_CAMERA_TARGET: // CAMERA_TARGET
			{
				// appendFormated("LM_CAMERA_TARGET ");
				lifeTempVar1 = readNextArgument("Target");

				if (lifeTempVar1 != g_engine->_engine->currentWorldTarget) // same target
				{
					lifeTempVar2 = g_engine->_engine->worldObjets[lifeTempVar1].objIndex;

					if (lifeTempVar2 != -1) {
						if (g_engine->getGameId() == GID_AITD1) {
							g_engine->_engine->currentWorldTarget = lifeTempVar1;
							g_engine->_engine->currentCameraTargetActor = lifeTempVar2;

							lifeTempVar3 = g_engine->_engine->objectTable[g_engine->_engine->currentCameraTargetActor].room;

							if (lifeTempVar3 != g_engine->_engine->currentRoom) {
								g_engine->_engine->needChangeRoom = 1;
								g_engine->_engine->newRoom = lifeTempVar3;
							}
						} else {
							// security case, the target actor may be still be in list while already changed of stage
							// TODO: check if AITD1 could use the same code (quite probable as it's only security)
							if (g_engine->_engine->objectTable[lifeTempVar2].stage != g_engine->_engine->currentFloor) {
								g_engine->_engine->currentWorldTarget = lifeTempVar1;
								g_engine->_engine->changeFloor = 1;
								g_engine->_engine->newFloor = g_engine->_engine->objectTable[lifeTempVar2].stage;
								g_engine->_engine->newRoom = g_engine->_engine->objectTable[lifeTempVar2].room;
							} else {
								g_engine->_engine->currentWorldTarget = lifeTempVar1;
								g_engine->_engine->currentCameraTargetActor = lifeTempVar2;

								lifeTempVar3 = g_engine->_engine->objectTable[g_engine->_engine->currentCameraTargetActor].room;

								if (lifeTempVar3 != g_engine->_engine->currentRoom) {
									g_engine->_engine->needChangeRoom = 1;
									g_engine->_engine->newRoom = lifeTempVar3;
								}
							}
						}
					} else // different stage
					{
						g_engine->_engine->currentWorldTarget = lifeTempVar1;
						if (g_engine->_engine->worldObjets[lifeTempVar1].stage != g_engine->_engine->currentFloor) {
							g_engine->_engine->changeFloor = 1;
							g_engine->_engine->newFloor = g_engine->_engine->worldObjets[lifeTempVar1].stage;
							g_engine->_engine->newRoom = g_engine->_engine->worldObjets[lifeTempVar1].room;
						} else {
							if (g_engine->_engine->currentRoom != g_engine->_engine->worldObjets[lifeTempVar1].room) {
								g_engine->_engine->needChangeRoom = 1;
								g_engine->_engine->newRoom = g_engine->_engine->worldObjets[lifeTempVar1].room;
							}
						}
					}
				}

				break;
			}
			case LM_PICTURE: // displayScreen
			{
				// appendFormated("LM_PICTURE ");

				int pictureIndex = readNextArgument("pictureIndex");
				int delay = readNextArgument("delay");

				int sampleId;
				if (g_engine->getGameId() == GID_TIMEGATE) {
					sampleId = evalVar("sampleId");
					readNextArgument();
				} else {
					sampleId = readNextArgument("sampleId");
				}

				freezeTime();

				pakLoad("ITD_RESS.PAK", pictureIndex, g_engine->_engine->aux);

				if (g_engine->getGameId() > GID_AITD1) {
					fadeOutPhys(0x10, 0);
					byte lpalette[0x300];
					copyPalette(g_engine->_engine->aux + 64000, lpalette);
					convertPaletteIfRequired(lpalette);
					copyPalette(lpalette, g_engine->_engine->currentGamePalette);
					gfx_setPalette(lpalette);
				}

				fastCopyScreen(g_engine->_engine->aux, g_engine->_engine->frontBuffer);
				gfx_copyBlockPhys(g_engine->_engine->frontBuffer, 0, 0, 320, 200);
				osystem_drawBackground();

				uint chrono;
				startChrono(&chrono);

				playSound(sampleId);

				// soundFunc(0);

				do {
					uint time;
					process_events();

					time = evalChrono(&chrono);

					if (time > static_cast<uint>(delay))
						break;
				} while (!g_engine->_engine->key && !g_engine->_engine->click);

				unfreezeTime();

				g_engine->_engine->flagInitView = 1;

				if (g_engine->getGameId() > GID_AITD1) {
					fadeOutPhys(0x10, 0);
				}
				break;
			}
			case LM_PLAY_SEQUENCE: // sequence
			{
				// appendFormated("LM_PLAY_SEQUENCE ");
				uint16 sequenceIdx;
				uint16 fadeEntry;
				uint16 fadeOut;

				freezeTime();

				sequenceIdx = *(uint16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				fadeEntry = *(uint16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				fadeOut = *(uint16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				playSequence(sequenceIdx, fadeEntry, fadeOut);

				unfreezeTime();

				break;
			}
			case LM_DEF_SEQUENCE_SAMPLE: {
				// appendFormated("LM_DEF_SEQUENCE_SAMPLE ");
				uint16 numParams;
				int i;

				numParams = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				assert(numParams <= NUM_MAX_SEQUENCE_PARAM);

				for (i = 0; i < numParams; i++) {
					sequenceParams[i].frame = READ_LE_U16(g_engine->_engine->currentLifePtr);
					g_engine->_engine->currentLifePtr += 2;
					sequenceParams[i].sample = READ_LE_U16(g_engine->_engine->currentLifePtr);
					g_engine->_engine->currentLifePtr += 2;
				}

				numSequenceParam = numParams;
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_PROTECT: // protection opcode
			{
				assert(0);
				// assert(g_engine->getGameId() != GID_TIMEGATE);
				// appendFormated("LM_PROTECT ");
				// printf("LM_PROTECT\n");
				// protection = 1;
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_INVENTORY: // INVENTORY
			{
				// appendFormated("LM_INVENTORY ");
				g_engine->_engine->statusScreenAllowed = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				break;
			}
			case LM_SET_INVENTORY: {
				// appendFormated("LM_SET_INVENTORY ");
				// int inventoryIndex = *(int16*)(g_engine->_engine->currentLifePtr);
				g_engine->_engine->currentLifePtr += 2;

				/*          if(indeventoyIndex != currentInHand)
				{
				if(currentInHand<2)
				{
				int i;

				for(i=0;i<inventory[currentInHand];i++)
				{
				g_engine->_engine->objectTable[inventoryTable[currentInHand][i]].flags2&=0x7FFF;
				}

				currentInHand = inventoryIndex
				}
				}*/
				// printf("LM_SET_INVENTORY\n");
				break;
			}
			case LM_SET_GROUND: {
				// appendFormated("LM_SET_GROUND ");
				groundLevel = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				break;
			}
			case LM_MESSAGE: {
				// appendFormated("LM_MESSAGE ");
				lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				makeMessage(lifeTempVar1);

				break;
			}
			case LM_MESSAGE_VALUE: {
				// appendFormated("LM_MESSAGE_VALUE ");
				lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr; // unused param ?
				g_engine->_engine->currentLifePtr += 2;

				makeMessage(lifeTempVar1);

				break;
			}
			case LM_END_SEQUENCE: // ENDING
			{
				// appendFormated("LM_END_SEQUENCE ");
				freezeTime();
				endSequence();
				unfreezeTime();
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_VAR: {
				// appendFormated("LM_VAR ");
				lifeTempVar1 = readNextArgument("Index");

				g_engine->_engine->vars[lifeTempVar1] = evalVar("value");
				break;
			}
			case LM_INC: // INC_VAR
			{
				// appendFormated("LM_INC ");
				lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				g_engine->_engine->vars[lifeTempVar1]++;
				break;
			}
			case LM_DEC: // DEC_VAR
			{
				// appendFormated("LM_DEC ");
				lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				g_engine->_engine->vars[lifeTempVar1]--;
				break;
			}
			case LM_ADD: // ADD_VAR
			{
				// appendFormated("LM_ADD ");
				lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				g_engine->_engine->vars[lifeTempVar1] += evalVar();
				break;
			}
			case LM_SUB: // SUB_VAR
			{
				// appendFormated("LM_SUB ");
				lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				g_engine->_engine->vars[lifeTempVar1] -= evalVar();
				break;
			}
			case LM_MODIF_C_VAR:
			case LM_C_VAR: {
				// appendFormated("LM_C_VAR ");
				lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				g_engine->_engine->cVars[lifeTempVar1] = evalVar();
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_IF_EGAL: {
				// appendFormated("LM_IF_EGAL ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if (lifeTempVar1 == lifeTempVar2) {
					g_engine->_engine->currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
					g_engine->_engine->currentLifePtr += lifeTempVar2 * 2;
					g_engine->_engine->currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_DIFFERENT: {
				// appendFormated("LM_IF_DIFFERENT ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if (lifeTempVar1 != lifeTempVar2) {
					g_engine->_engine->currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
					g_engine->_engine->currentLifePtr += lifeTempVar2 * 2;
					g_engine->_engine->currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_SUP_EGAL: {
				// appendFormated("LM_IF_SUP_EGAL ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if (lifeTempVar1 >= lifeTempVar2) {
					g_engine->_engine->currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
					g_engine->_engine->currentLifePtr += lifeTempVar2 * 2;
					g_engine->_engine->currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_SUP: {
				// appendFormated("LM_IF_SUP ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if (lifeTempVar1 > lifeTempVar2) {
					g_engine->_engine->currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
					g_engine->_engine->currentLifePtr += lifeTempVar2 * 2;
					g_engine->_engine->currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_INF_EGAL: {
				// appendFormated("LM_IF_INF_EGAL ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if (lifeTempVar1 <= lifeTempVar2) {
					g_engine->_engine->currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
					g_engine->_engine->currentLifePtr += lifeTempVar2 * 2;
					g_engine->_engine->currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_INF: {
				// appendFormated("LM_IF_INF ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if (lifeTempVar1 < lifeTempVar2) {
					g_engine->_engine->currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
					g_engine->_engine->currentLifePtr += lifeTempVar2 * 2;
					g_engine->_engine->currentLifePtr += 2;
				}

				break;
			}
			case LM_GOTO: {
				// appendFormated("LM_GOTO ");
				lifeTempVar1 = readNextArgument("Offset");
				g_engine->_engine->currentLifePtr += lifeTempVar1 * 2;
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_SWITCH: // SWITCH
			{
				// appendFormated("LM_SWITCH ");
				switchVal = evalVar("value");
				break;
			}
			case LM_CASE: // CASE
			{
				// appendFormated("LM_CASE ");
				lifeTempVar1 = readNextArgument("Case");

				if (lifeTempVar1 == switchVal) {
					g_engine->_engine->currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
					g_engine->_engine->currentLifePtr += lifeTempVar2 * 2;
					g_engine->_engine->currentLifePtr += 2;
				}

				break;
			}
			case LM_MULTI_CASE: // MULTI_CASE
			{
				// appendFormated("LM_MULTI_CASE ");
				int i;
				lifeTempVar1 = *(int16 *)g_engine->_engine->currentLifePtr;
				g_engine->_engine->currentLifePtr += 2;

				lifeTempVar2 = 0;

				for (i = 0; i < lifeTempVar1; i++) {
					if (*(int16 *)g_engine->_engine->currentLifePtr == switchVal) {
						lifeTempVar2 = 1;
					}
					g_engine->_engine->currentLifePtr += 2;
				}

				if (!lifeTempVar2) {
					lifeTempVar2 = *(int16 *)g_engine->_engine->currentLifePtr;
					g_engine->_engine->currentLifePtr += lifeTempVar2 * 2;
					g_engine->_engine->currentLifePtr += 2;
				} else {
					g_engine->_engine->currentLifePtr += 2;
				}
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_RETURN: {
				// appendFormated("LM_RETURN ");
				exitLife = 1;
				break;
			}
			case LM_END: {
				// appendFormated("LM_END ");
				exitLife = 1;
				break;
			}
			case LM_GAME_OVER: {
				// appendFormated("LM_GAME_OVER ");
				fadeMusic(0, 0, 0x8000); // fade out music
				startChrono(&musicChrono);

				while (evalChrono(&musicChrono) < 120) {
					process_events();
				}
				g_engine->_engine->flagGameOver = 1;
				exitLife = 1;
				break;
			}
			case LM_WAIT_GAME_OVER: {
				// appendFormated("LM_WAIT_GAME_OVER ");
				while (g_engine->_engine->key || g_engine->_engine->joyD || g_engine->_engine->click) {
					process_events();
				}
				while (!g_engine->_engine->key && !g_engine->_engine->joyD && g_engine->_engine->click) {
					process_events();
				}
				g_engine->_engine->flagGameOver = 1;
				exitLife = 1;
				break;
			}
			case LM_CALL_INVENTORY: {
				// appendFormated("LM_CALL_INVENTORY ");
				processInventory();
				break;
			}
			case LM_DO_ROT_CLUT: // DO_ROT_CLUT
			{
				// appendFormated("LM_DO_ROT_CLUT ");
				assert(g_engine->getGameId() == GID_TIMEGATE);

				(void)readNextArgument();
				(void)readNextArgument();
				(void)readNextArgument();
				break;
			}
			case LM_START_FADE_IN_MUSIC_LOOP: {
				// appendFormated("LM_START_FADE_IN_MUSIC_LOOP ");
				assert(g_engine->getGameId() == GID_TIMEGATE);

				(void)readNextArgument();
				(void)readNextArgument();
				(void)readNextArgument();
				(void)readNextArgument();
				(void)readNextArgument();
				// int arg5 = readNextArgument();
				break;
			}
			default: {
				debug("Unknown opcode %X in processLife\n", currentOpcode & 0x7FFF);
				assert(0);
			}
			}
		}

#ifdef DEBUG
		if (strlen(currentDebugLifeLine)) {
			printf("%s\n", currentDebugLifeLine);
		}
#endif

		if (var_6 != -1) {
			g_engine->_engine->currentProcessedActorIdx = g_engine->_engine->currentLifeActorIdx;
			g_engine->_engine->currentProcessedActorPtr = g_engine->_engine->currentLifeActorPtr;
		}
	}

	g_engine->_engine->currentLifeNum = -1;
}
} // namespace Fitd
