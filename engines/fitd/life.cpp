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

#include "fitd/life.h"
#include "common/debug.h"
#include "fitd/aitd1.h"
#include "fitd/aitd2.h"
#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/eval_var.h"
#include "fitd/fitd.h"
#include "fitd/font.h"
#include "fitd/game_time.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/inventory.h"
#include "fitd/jack.h"
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

sequenceParamStruct sequenceParams[NUM_MAX_SEQUENCE_PARAM];

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

void resetRotateParam() {
	currentProcessedActorPtr->rotate.param = 0;
}

static void throwObj(int animThrow, int frameThrow, int arg_4, int objToThrowIdx, int throwRotated, int throwForce, int animNext) {
	if (initAnim(animThrow, 2, animNext)) {
		currentProcessedActorPtr->animActionANIM = animThrow;
		currentProcessedActorPtr->animActionFRAME = frameThrow;
		currentProcessedActorPtr->animActionType = 6;
		currentProcessedActorPtr->hotPointID = arg_4;
		currentProcessedActorPtr->animActionParam = objToThrowIdx;
		currentProcessedActorPtr->hitForce = throwForce;

		if (!throwRotated) {
			ListWorldObjets[objToThrowIdx].gamma -= 0x100;
		}

		ListWorldObjets[objToThrowIdx].flags2 |= 0x1000;
	}
}

static void put(int x, int y, int z, int room, int stage, int alpha, int beta, int gamma, int idx) {
	tWorldObject *objPtr = &ListWorldObjets[idx];

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
		currentProcessedActorPtr->animActionANIM = fireAnim;
		currentProcessedActorPtr->animActionFRAME = X;
		currentProcessedActorPtr->animActionType = 4;
		currentProcessedActorPtr->animActionParam = Z;
		currentProcessedActorPtr->hotPointID = Y;
		currentProcessedActorPtr->hitForce = hitForce;
	}
}

int randRange(int min, int max) {
	return min + g_engine->getRandomNumber(max - min);
}

int initSpecialObjet(int mode, int X, int Y, int Z, int stage, int room, int alpha, int beta, int gamma, ZVStruct *zvPtr) {
	int16 localSpecialTable[4];
	int i;
	ZVStruct *actorZvPtr = nullptr;

	memcpy(localSpecialTable, specialTable, 8);

	tObject *currentActorPtr = objectTable;

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

	if (currentRoom != room) {
		currentActorPtr->worldX -= (int16)((roomDataTable[currentRoom].worldX - roomDataTable[room].worldX) * 10);
		currentActorPtr->worldY += (int16)((roomDataTable[currentRoom].worldY - roomDataTable[room].worldY) * 10);
		currentActorPtr->worldZ += (int16)((roomDataTable[currentRoom].worldZ - roomDataTable[room].worldZ) * 10);
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
	case 0: // evaporate
	{
		int j;

		actorZvPtr->ZVX1 -= X;
		actorZvPtr->ZVX2 -= X;
		actorZvPtr->ZVY1 -= Y;
		actorZvPtr->ZVY2 -= Y;
		actorZvPtr->ZVZ1 -= Z;
		actorZvPtr->ZVZ2 -= Z;

		currentActorPtr->FRAME = HQ_Malloc(HQ_Memory, 304);

		char *flowPtr = HQ_PtrMalloc(HQ_Memory, currentActorPtr->FRAME);

		if (!flowPtr) {
			currentActorPtr->indexInWorld = -1;
			return -1;
		}

		currentActorPtr->ANIM = mode;

		*(int16 *)flowPtr = localSpecialTable[g_engine->getRandomNumber(2)]; // type ? color ?
		flowPtr += 2;
		*(int16 *)flowPtr = 30; // num of points
		flowPtr += 2;

		for (j = 0; j < 30; j++) {
			*(int16 *)flowPtr = randRange(actorZvPtr->ZVX1, actorZvPtr->ZVX2); // X
			flowPtr += 2;
			*(int16 *)flowPtr = randRange(actorZvPtr->ZVY1, actorZvPtr->ZVY2); // Y
			flowPtr += 2;
			*(int16 *)flowPtr = randRange(actorZvPtr->ZVZ1, actorZvPtr->ZVZ2); // Z
			flowPtr += 2;
		}

		for (j = 0; j < 30; j++) {
			*(int16 *)flowPtr = randRange(150, 300); // ?
			flowPtr += 2;
			*(int16 *)flowPtr = randRange(30, 80); // ?
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
	default: {
		debug("Unsupported case %d in createFlow\n", mode);
	}
	}

	actorTurnedToObj = 1;
	return i;
}

static void getHardClip() {
	const ZVStruct *zvPtr = &currentProcessedActorPtr->zv;
	char *etageData = (char *)getRoomData(currentProcessedActorPtr->room);

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
			hardClip.ZVX1 = zvCol.ZVX1;
			hardClip.ZVX2 = zvCol.ZVX2;
			hardClip.ZVY1 = zvCol.ZVY1;
			hardClip.ZVY2 = zvCol.ZVY2;
			hardClip.ZVZ1 = zvCol.ZVZ1;
			hardClip.ZVZ2 = zvCol.ZVZ2;

			return;
		}

		etageData += 16;
	}

	hardClip.ZVX1 = 32000;
	hardClip.ZVX2 = -32000;
	hardClip.ZVY1 = 32000;
	hardClip.ZVY2 = -32000;
	hardClip.ZVZ1 = 32000;
	hardClip.ZVZ2 = -32000;
}

static void animMove(int animStand, int animWalk, int animRun, int animStop, int animBackward, int animTurnRight, int animTurnLeft) {
	if (currentProcessedActorPtr->speed == 5) {
		initAnim(animRun, 1, -1);
	}

	if (currentProcessedActorPtr->speed == 4) {
		initAnim(animWalk, 1, -1);
	}

	if (currentProcessedActorPtr->speed == -1) // backward
	{
		if (currentProcessedActorPtr->ANIM == animWalk) {
			initAnim(animStand, 0, animBackward);
		} else if (currentProcessedActorPtr->ANIM == animRun) {
			initAnim(animStop, 0, animStand);
		} else {
			initAnim(animBackward, 1, -1); // walk backward
		}
	}
	if (currentProcessedActorPtr->speed == 0) {
		if (currentProcessedActorPtr->ANIM == animWalk || currentProcessedActorPtr->ANIM == animRun) {
			initAnim(animStop, 0, animStand);
		} else {
			if (currentProcessedActorPtr->direction == 0) {
				initAnim(animStand, 1, -1);
			}
			if (currentProcessedActorPtr->direction == 1) // left
			{
				initAnim(animTurnLeft, 0, animStand);
			}
			if (currentProcessedActorPtr->direction == -1) // right
			{
				initAnim(animTurnRight, 0, animStand);
			}
		}
	}
}

static void setStage(int newStage, int newRoomLocal, int X, int Y, int Z) {

	currentProcessedActorPtr->stage = newStage;
	currentProcessedActorPtr->room = newRoomLocal;

	if (g_engine->getGameId() != GID_AITD1) {
		currentProcessedActorPtr->hardMat = -1;
	}

	const int animX = currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX;
	const int animY = currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY;
	const int animZ = currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ;

	currentProcessedActorPtr->zv.ZVX1 += X - animX;
	currentProcessedActorPtr->zv.ZVX2 += X - animX;

	currentProcessedActorPtr->zv.ZVY1 += Y - animY;
	currentProcessedActorPtr->zv.ZVY2 += Y - animY;

	currentProcessedActorPtr->zv.ZVZ1 += Z - animZ;
	currentProcessedActorPtr->zv.ZVZ2 += Z - animZ;

	currentProcessedActorPtr->roomX = X;
	currentProcessedActorPtr->roomY = Y;
	currentProcessedActorPtr->roomZ = Z;

	currentProcessedActorPtr->worldX = X;
	currentProcessedActorPtr->worldY = Y;
	currentProcessedActorPtr->worldZ = Z;

	currentProcessedActorPtr->stepX = 0;
	currentProcessedActorPtr->stepY = 0;
	currentProcessedActorPtr->stepZ = 0;

	if (currentCameraTargetActor == currentProcessedActorIdx) {
		if (newStage != g_currentFloor) {
			changeFloor = 1;
			newFloor = newStage;
			newRoom = newRoomLocal;
		} else {
			if (currentRoom != newRoomLocal) {
				needChangeRoom = 1;
				newRoom = newRoomLocal;
			}
		}
	} else {
		if (currentRoom != newRoomLocal) {
			currentProcessedActorPtr->worldX -= (int16)((roomDataTable[currentRoom].worldX - roomDataTable[newRoomLocal].worldX) * 10);
			currentProcessedActorPtr->worldY += (int16)((roomDataTable[currentRoom].worldY - roomDataTable[newRoomLocal].worldY) * 10);
			currentProcessedActorPtr->worldZ += (int16)((roomDataTable[currentRoom].worldZ - roomDataTable[newRoomLocal].worldZ) * 10);
		}

		//    FlagGenereActiveList = 1;
	}
}

void setupRealZv(ZVStruct *zvPtr) {
	const int16 *ptr = pointBuffer;

	zvPtr->ZVX1 = 32000;
	zvPtr->ZVY1 = 32000;
	zvPtr->ZVZ1 = 32000;
	zvPtr->ZVX2 = -32000;
	zvPtr->ZVY2 = -32000;
	zvPtr->ZVZ2 = -32000;

	for (int i = 0; i < numOfPoints; i++) {
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

static void doRealZv(tObject *actorPtr) {

	computeScreenBox(0, 0, 0, actorPtr->alpha, actorPtr->beta, actorPtr->gamma, HQR_Get(listBody, actorPtr->bodyNum));

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
		currentProcessedActorPtr->animActionANIM = animNumber;
		currentProcessedActorPtr->animActionFRAME = arg_2;
		currentProcessedActorPtr->animActionType = 1;
		currentProcessedActorPtr->animActionParam = arg_6;
		currentProcessedActorPtr->hotPointID = arg_4;
		currentProcessedActorPtr->hitForce = hitForce;
	}
}

static void deleteObject(int objIdx) {

	tWorldObject *objPtr = &ListWorldObjets[objIdx];
	const int actorIdx = objPtr->objIndex;

	if (actorIdx != -1) {
		tObject *actorPtr = &objectTable[actorIdx];

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
		aitd1_readBook(index, type, shadow);
		break;
	case GID_JACK:
		JACK_ReadBook(index, type);
		break;
	case GID_AITD2:
		AITD2_ReadBook(index, type);
		break;
	default:
		assert(0);
	}

	unfreezeTime();
}

static void makeMessage(int messageIdx) {
	textEntryStruct *messagePtr = getTextFromIdx(messageIdx);

	if (messagePtr) {
		for (int i = 0; i < 5; i++) {
			if (messageTable[i].string == messagePtr) {
				messageTable[i].time = 0;
				return;
			}
		}

		for (int i = 0; i < 5; i++) {
			if (messageTable[i].string == nullptr) {
				messageTable[i].string = messagePtr;
				messageTable[i].time = 0;
				return;
			}
		}
	}
}

static void unpackSequenceFrame(unsigned char *source, unsigned char *dest) {

	unsigned char byteCode = *source++;

	while (byteCode) {
		if (!--byteCode) // change pixel or skip pixel
		{

			const unsigned char changeColor = *source++;

			if (changeColor) {
				*dest++ = changeColor;
			} else {
				dest++;
			}
		} else if (!--byteCode) // change 2 pixels or skip 2 pixels
		{

			const unsigned char changeColor = *source++;

			if (changeColor) {
				*dest++ = changeColor;
				*dest++ = changeColor;
			} else {
				dest += 2;
			}
		} else if (!--byteCode) // fill or skip
		{

			const unsigned char size = *source++;
			const unsigned char fillColor = *source++;

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
			const unsigned char fillColor = *source++;

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

	const int var_4 = 1;
	int quitPlayback = 0;
	int nextFrame = 1;
	unsigned char localPalette[0x300];

	Common::String buffer;
	if (g_engine->getGameId() == GID_AITD2) {
		buffer = Common::String::format("%s.PAK", sequenceListAITD2[sequenceIdx]);
	} else if (g_engine->getGameId() == GID_AITD3) {
		buffer = Common::String::format("AN%d.PAK", sequenceIdx);
	} else {
		assert(0);
	}

	const int numMaxFrames = PAK_getNumFiles(buffer.c_str());

	while (!quitPlayback) {
		int currentFrameId = 0;

		while (currentFrameId < nextFrame) {
			// frames++;

			timer = timeGlobal;

			if (currentFrameId >= numMaxFrames) {
				quitPlayback = 1;
				break;
			}

			if (!loadPak(buffer.c_str(), currentFrameId, logicalScreen)) {
				error("Error loading pak %s", buffer.c_str());
			}

			if (!currentFrameId) // first frame
			{
				memcpy(localPalette, logicalScreen, 0x300); // copy palette
				memcpy(aux, logicalScreen + 0x300, 64000);
				nextFrame = READ_LE_U16(logicalScreen + 64768);

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
					copyPalette(localPalette, currentGamePalette);
				}
			} else // not first frame
			{

				const uint32 frameSize = READ_LE_U32(logicalScreen);

				if (frameSize < 64000) // key frame
				{
					unpackSequenceFrame((unsigned char *)logicalScreen + 4, (unsigned char *)aux);
				} else // delta frame
				{
					fastCopyScreen(logicalScreen, aux);
				}
			}

			for (int sequenceParamIdx = 0; sequenceParamIdx < numSequenceParam; sequenceParamIdx++) {
				if (sequenceParams[sequenceParamIdx].frame == (uint)currentFrameId) {
					playSound(sequenceParams[sequenceParamIdx].sample);
				}
			}

			// TODO: here, timming management
			// TODO: fade management

			gfx_copyBlockPhys((unsigned char *)aux, 0, 0, 320, 200);

			osystem_drawBackground();

			currentFrameId++;

			for (int i = 0; i < 5; i++) // display the frame 5 times (original seems to wait 5 sync)
			{
				process_events();
			}

			if (key) {
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

	flagInitView = 2;
}

void processLife(int lifeNum, bool callFoundLife) {
	int exitLife = 0;
	// int switchVal = 0;
	int var_6;
	int switchVal = 0;

	currentLifeActorIdx = currentProcessedActorIdx;
	currentLifeActorPtr = currentProcessedActorPtr;
	currentLifeNum = lifeNum;

	currentLifePtr = HQR_Get(listLife, lifeNum);
	assert(currentLifePtr);

	while (!exitLife) {
		int lifeTempVar1;
		int lifeTempVar2;
		int lifeTempVar6;
		int lifeTempVar7;
		int lifeTempVar8;
		int16 currentOpcode;

		var_6 = -1;

		currentOpcode = *(int16 *)currentLifePtr;
		currentLifePtr += 2;

		// #ifdef DEBUG
		// 		strcpy(currentDebugLifeLine, "");
		// #endif
		// appendFormated("%d:opcode: %02X: ", lifeNum, currentOpcode & 0xFFFF);

		if (currentOpcode & 0x8000) {
			var_6 = *(int16 *)currentLifePtr;
			currentLifePtr += 2;

			if (var_6 == -1) {
				error("Unsupported newVar = -1\n");
				assert(0);
			} else {
				currentProcessedActorIdx = ListWorldObjets[var_6].objIndex;

				if (currentProcessedActorIdx != -1) {
					currentProcessedActorPtr = &objectTable[currentProcessedActorIdx];

					goto processOpcode;
				} else {
					int opcodeLocated;

					if (g_engine->getGameId() == GID_AITD1) {
						opcodeLocated = AITD1LifeMacroTable[currentOpcode & 0x7FFF];
					} else {
						opcodeLocated = AITD2LifeMacroTable[currentOpcode & 0x7FFF];
					}

					switch (opcodeLocated) {
						////////////////////////////////////////////////////////////////////////
					case LM_BODY: {
						ListWorldObjets[var_6].body = evalVar();
						break;
					}
					case LM_BODY_RESET: {
						ListWorldObjets[var_6].body = evalVar();
						ListWorldObjets[var_6].anim = evalVar();
						break;
					}
					case LM_TYPE: {
						lifeTempVar1 = *(int16 *)currentLifePtr & TYPE_MASK;
						currentLifePtr += 2;

						lifeTempVar2 = ListWorldObjets[var_6].flags;

						ListWorldObjets[var_6].flags = (ListWorldObjets[var_6].flags & ~TYPE_MASK) + lifeTempVar1;
						break;
					}
					////////////////////////////////////////////////////////////////////////
					case LM_ANIM_ONCE: {
						ListWorldObjets[var_6].anim = *(int16 *)currentLifePtr;
						currentLifePtr += 2;
						ListWorldObjets[var_6].animInfo = *(int16 *)currentLifePtr;
						currentLifePtr += 2;
						ListWorldObjets[var_6].animType = ANIM_ONCE;
						if (g_engine->getGameId() >= GID_JACK)
							ListWorldObjets[var_6].frame = 0;
						break;
					}
					case LM_ANIM_REPEAT: {
						ListWorldObjets[var_6].anim = *(int16 *)currentLifePtr;
						currentLifePtr += 2;
						ListWorldObjets[var_6].animInfo = -1;
						ListWorldObjets[var_6].animType = ANIM_REPEAT;
						if (g_engine->getGameId() >= GID_JACK)
							ListWorldObjets[var_6].frame = 0;
						break;
					}
					case LM_ANIM_ALL_ONCE: {
						ListWorldObjets[var_6].anim = *(int16 *)currentLifePtr;
						currentLifePtr += 2;
						ListWorldObjets[var_6].animInfo = *(int16 *)currentLifePtr;
						currentLifePtr += 2;
						ListWorldObjets[var_6].animType = ANIM_ONCE | ANIM_UNINTERRUPTABLE;
						if (g_engine->getGameId() >= GID_JACK)
							ListWorldObjets[var_6].frame = 0;
						break;
					}
					case LM_ANIM_RESET: {
						assert(g_engine->getGameId() >= GID_JACK);
						ListWorldObjets[var_6].anim = *(int16 *)currentLifePtr;
						currentLifePtr += 2;
						ListWorldObjets[var_6].animInfo = *(int16 *)currentLifePtr;
						currentLifePtr += 2;
						ListWorldObjets[var_6].animType = ANIM_ONCE | ANIM_RESET;
						ListWorldObjets[var_6].frame = 0;
						break;
					}
					////////////////////////////////////////////////////////////////////////
					case LM_MOVE: // MOVE
					{
						ListWorldObjets[var_6].trackMode = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						ListWorldObjets[var_6].trackNumber = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						ListWorldObjets[var_6].positionInTrack = 0;

						if (g_engine->getGameId() > GID_AITD1) {
							ListWorldObjets[var_6].mark = -1;
						}
						break;
					}
					case LM_ANGLE: {
						ListWorldObjets[var_6].alpha = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						ListWorldObjets[var_6].beta = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						ListWorldObjets[var_6].gamma = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						break;
					}
					case LM_STAGE: // stage
					{
						ListWorldObjets[var_6].stage = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						ListWorldObjets[var_6].room = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						ListWorldObjets[var_6].x = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						ListWorldObjets[var_6].y = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						ListWorldObjets[var_6].z = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						// FlagGenereActiveList = 1;

						break;
					}
					case LM_TEST_COL: {
						if (*(int16 *)currentLifePtr) {
							ListWorldObjets[var_6].flags |= 0x20;
						} else {
							ListWorldObjets[var_6].flags &= 0xFFDF;
						}

						currentLifePtr += 2;

						break;
					}
					////////////////////////////////////////////////////////////////////////
					case LM_LIFE: {
						ListWorldObjets[var_6].life = *(int16 *)currentLifePtr;
						currentLifePtr += 2;
						break;
					}
					case LM_LIFE_MODE: // LIFE_MODE
					{
						lifeTempVar1 = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						if (lifeTempVar1 != ListWorldObjets[var_6].lifeMode) {
							ListWorldObjets[var_6].lifeMode = lifeTempVar1;
							// FlagGenereActiveList = 1;
						}
						break;
					}
					case LM_FOUND_NAME: // FOUND_NAME
					{
						ListWorldObjets[var_6].foundName = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						break;
					}
					case LM_FOUND_BODY: // FOUND_BODY
					{
						ListWorldObjets[var_6].foundBody = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						break;
					}
					case LM_FOUND_FLAG: // FOUND_FLAG
					{
						ListWorldObjets[var_6].flags2 &= 0xE000;
						ListWorldObjets[var_6].flags2 |= *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						break;
					}
					case LM_FOUND_WEIGHT: {
						ListWorldObjets[var_6].positionInTrack = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						break;
					}
					case LM_START_CHRONO: // arrive in original interpreter too
					{
						break;
					}
					////////////////////////////////////////////////////////////////////////
					default: {
						error("Unsupported opcode %X when actor isn't in floor\n", currentOpcode & 0x7FFF);
						break;
					}
					}
				}
			}
		} else {
			int lifeTempVar4;
			int lifeTempVar3;
			int lifeTempVar5;
			int opcodeLocated;
		processOpcode:

			if (g_engine->getGameId() == GID_AITD1) {
				opcodeLocated = AITD1LifeMacroTable[currentOpcode & 0x7FFF];
			} else {
				opcodeLocated = AITD2LifeMacroTable[currentOpcode & 0x7FFF];
			}

			switch (opcodeLocated) {
			case LM_BODY: {
				lifeTempVar1 = evalVar();

				ListWorldObjets[currentProcessedActorPtr->indexInWorld].body = lifeTempVar1;

				if (currentProcessedActorPtr->bodyNum != lifeTempVar1) {
					currentProcessedActorPtr->bodyNum = lifeTempVar1;

					if (currentProcessedActorPtr->_flags & AF_ANIMATED) {
						if (currentProcessedActorPtr->ANIM != -1 && currentProcessedActorPtr->bodyNum != -1) {
							char *pAnim = HQR_Get(listAnim, currentProcessedActorPtr->ANIM);
							char *pBody;

							if (g_engine->getGameId() >= GID_JACK) {
								/*                  if (bFlagDecal)
								gereDecal(); */
							}

							pBody = HQR_Get(listBody, currentProcessedActorPtr->bodyNum);

							/*    if(gameId >= GID_JACK)
							{
							setInterAnimObject2(currentProcessedActorPtr->FRAME, pAnim, pBody, TRUE, Objet->AnimDecal);
							}
							else */
							{
								setInterAnimObjet(currentProcessedActorPtr->FRAME, pAnim, pBody);
							}
						}
					} else {
						flagInitView = 1;
					}
				}
				break;
			}
			case LM_BODY_RESET: {
				// appendFormated("LM_BODY_RESET ");

				int param1 = evalVar("body");
				int param2 = evalVar("anim");

				ListWorldObjets[currentProcessedActorPtr->indexInWorld].body = param1;
				ListWorldObjets[currentProcessedActorPtr->indexInWorld].anim = param2;

				currentProcessedActorPtr->bodyNum = param1;

				if (currentProcessedActorPtr->_flags & AF_ANIMATED) {
					char *pAnim = HQR_Get(listAnim, currentProcessedActorPtr->ANIM);
					char *pBody;

					if (g_engine->getGameId() >= GID_JACK) {
						/*                  if (bFlagDecal)
						gereDecal(); */
					}
					pBody = HQR_Get(listBody, currentProcessedActorPtr->bodyNum);

					setAnimObjet(0, pAnim, pBody);
					initAnim(param2, 4, -1);
				} else {
					flagInitView = 1;
				}
				break;
			}
			case LM_DO_REAL_ZV: {
				// appendFormated("LM_DO_REAL_ZV ");
				doRealZv(currentProcessedActorPtr);
				break;
			}
			case LM_DEF_ZV: // DEF_ZV
			{
				// appendFormated("LM_DEF_ZV ");
				currentProcessedActorPtr->zv.ZVX1 = currentProcessedActorPtr->roomX + *(int16 *)currentLifePtr + currentProcessedActorPtr->stepX;
				currentLifePtr += 2;
				currentProcessedActorPtr->zv.ZVX2 = currentProcessedActorPtr->roomX + *(int16 *)currentLifePtr + currentProcessedActorPtr->stepX;
				currentLifePtr += 2;

				currentProcessedActorPtr->zv.ZVY1 = currentProcessedActorPtr->roomY + *(int16 *)currentLifePtr + currentProcessedActorPtr->stepY;
				currentLifePtr += 2;
				currentProcessedActorPtr->zv.ZVY2 = currentProcessedActorPtr->roomY + *(int16 *)currentLifePtr + currentProcessedActorPtr->stepY;
				currentLifePtr += 2;

				currentProcessedActorPtr->zv.ZVZ1 = currentProcessedActorPtr->roomZ + *(int16 *)currentLifePtr + currentProcessedActorPtr->stepZ;
				currentLifePtr += 2;
				currentProcessedActorPtr->zv.ZVZ2 = currentProcessedActorPtr->roomZ + *(int16 *)currentLifePtr + currentProcessedActorPtr->stepZ;
				currentLifePtr += 2;

				break;
			}
			case LM_DEF_ABS_ZV: {
				// appendFormated("LM_DEF_ABS_ZV ");
				currentProcessedActorPtr->zv.ZVX1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				currentProcessedActorPtr->zv.ZVX2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				currentProcessedActorPtr->zv.ZVY1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				currentProcessedActorPtr->zv.ZVY2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				currentProcessedActorPtr->zv.ZVZ1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				currentProcessedActorPtr->zv.ZVZ2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				break;
			}
			case LM_DO_ROT_ZV: {
				// appendFormated("LM_DO_ROT_ZV ");
				getZvRot(HQR_Get(listBody, currentProcessedActorPtr->bodyNum), &currentProcessedActorPtr->zv,
						 currentProcessedActorPtr->alpha,
						 currentProcessedActorPtr->beta,
						 currentProcessedActorPtr->gamma);

				currentProcessedActorPtr->zv.ZVX1 += currentProcessedActorPtr->roomX;
				currentProcessedActorPtr->zv.ZVX2 += currentProcessedActorPtr->roomX;
				currentProcessedActorPtr->zv.ZVY1 += currentProcessedActorPtr->roomY;
				currentProcessedActorPtr->zv.ZVY2 += currentProcessedActorPtr->roomY;
				currentProcessedActorPtr->zv.ZVZ1 += currentProcessedActorPtr->roomZ;
				currentProcessedActorPtr->zv.ZVZ2 += currentProcessedActorPtr->roomZ;

				break;
			}
			case LM_DO_MAX_ZV: {
				getZvMax(HQR_Get(listBody, currentProcessedActorPtr->bodyNum), &currentProcessedActorPtr->zv);

				currentProcessedActorPtr->zv.ZVX1 += currentProcessedActorPtr->roomX;
				currentProcessedActorPtr->zv.ZVX2 += currentProcessedActorPtr->roomX;
				currentProcessedActorPtr->zv.ZVY1 += currentProcessedActorPtr->roomY;
				currentProcessedActorPtr->zv.ZVY2 += currentProcessedActorPtr->roomY;
				currentProcessedActorPtr->zv.ZVZ1 += currentProcessedActorPtr->roomZ;
				currentProcessedActorPtr->zv.ZVZ2 += currentProcessedActorPtr->roomZ;

				break;
			}
			case LM_DO_CARRE_ZV: // DO_CARRE_ZV
			{
				// appendFormated("LM_DO_CARRE_ZV ");
				getZvCube(HQR_Get(listBody, currentProcessedActorPtr->bodyNum), &currentProcessedActorPtr->zv);

				currentProcessedActorPtr->zv.ZVX1 += currentProcessedActorPtr->roomX;
				currentProcessedActorPtr->zv.ZVX2 += currentProcessedActorPtr->roomX;
				currentProcessedActorPtr->zv.ZVY1 += currentProcessedActorPtr->roomY;
				currentProcessedActorPtr->zv.ZVY2 += currentProcessedActorPtr->roomY;
				currentProcessedActorPtr->zv.ZVZ1 += currentProcessedActorPtr->roomZ;
				currentProcessedActorPtr->zv.ZVZ2 += currentProcessedActorPtr->roomZ;

				break;
			}
			case LM_TYPE: // TYPE
			{
				// appendFormated("LM_TYPE ");

				lifeTempVar1 = readNextArgument("type") & AF_MASK;
				lifeTempVar2 = currentProcessedActorPtr->_flags;

				currentProcessedActorPtr->_flags = (currentProcessedActorPtr->_flags & ~AF_MASK) + lifeTempVar1;

				if (g_engine->getGameId() > GID_AITD1) {
					if (lifeTempVar2 & 1) {
						if (!(lifeTempVar1 & 1)) {
							addActorToBgInscrust(currentProcessedActorIdx);
						}
					}

					if (lifeTempVar1 & 1) {
						if (!(lifeTempVar2 & 8)) {
							removeFromBGIncrust(currentProcessedActorIdx);
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
					currentProcessedActorPtr->ANIM = -1;
					currentProcessedActorPtr->newAnim = -2;
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
					currentProcessedActorPtr->ANIM = -1;
					currentProcessedActorPtr->newAnim = -2;
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
				currentLifePtr += 12;
				evalVar();
				break;
			}
			case LM_HIT_OBJECT: // HIT_OBJECT
			{
				// appendFormated("LM_HIT_OBJECT ");

				lifeTempVar1 = readNextArgument("Flags");
				lifeTempVar2 = readNextArgument("Force");

				currentProcessedActorPtr->animActionType = 8;
				currentProcessedActorPtr->animActionParam = lifeTempVar1;
				currentProcessedActorPtr->hitForce = lifeTempVar2;
				currentProcessedActorPtr->hotPointID = -1;
				break;
			}
			case LM_STOP_HIT_OBJECT: // cancel hit obj
			{
				// appendFormated("LM_STOP_HIT_OBJECT ");
				if (currentProcessedActorPtr->animActionType == 8) {
					currentProcessedActorPtr->animActionType = 0;
					currentProcessedActorPtr->animActionParam = 0;
					currentProcessedActorPtr->hitForce = 0;
					currentProcessedActorPtr->hotPointID = -1;
				}

				break;
			}
			case LM_THROW: // throw
			{
				// appendFormated("LM_THROW ");
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar3 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar4 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar5 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar6 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar7 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

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
				char *ptr;

				ptr = HQR_Get(listTrack, currentProcessedActorPtr->trackNumber);

				ptr += currentProcessedActorPtr->positionInTrack * 2;

				if (*(int16 *)ptr == 5) {
					currentProcessedActorPtr->positionInTrack++;
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

				if (currentProcessedActorPtr->beta != lifeTempVar1) {
					if (currentProcessedActorPtr->rotate.param == 0 || currentProcessedActorPtr->rotate.newAngle != lifeTempVar1) {
						initRealValue(currentProcessedActorPtr->beta, lifeTempVar1, lifeTempVar2, &currentProcessedActorPtr->rotate);
					}

					currentProcessedActorPtr->beta = updateActorRotation(&currentProcessedActorPtr->rotate);
				}

				break;
			}
			case LM_SET_ALPHA: // SET_ALPHA
			{
				// appendFormated("LM_SET_ALPHA ");
				lifeTempVar1 = readNextArgument("alpha");
				lifeTempVar2 = readNextArgument("speed");

				if (currentProcessedActorPtr->alpha != lifeTempVar1) {
					if (currentProcessedActorPtr->rotate.param == 0 || lifeTempVar1 != currentProcessedActorPtr->rotate.newAngle) {
						initRealValue(currentProcessedActorPtr->alpha, lifeTempVar1, lifeTempVar2, &currentProcessedActorPtr->rotate);
					}

					currentProcessedActorPtr->alpha = updateActorRotation(&currentProcessedActorPtr->rotate);
				}

				break;
			}
			case LM_ANGLE: // ANGLE
			{
				// appendFormated("LM_ANGLE ");
				currentProcessedActorPtr->alpha = readNextArgument("alpha");
				currentProcessedActorPtr->beta = readNextArgument("beta");
				currentProcessedActorPtr->gamma = readNextArgument("gamma");

				break;
			}
			case LM_COPY_ANGLE: {
				// appendFormated("LM_COPY_ANGLE ");
				int object = readNextArgument("object");
				int localObjectIndex = ListWorldObjets[object].objIndex;
				if (localObjectIndex == -1) {
					currentProcessedActorPtr->alpha = ListWorldObjets[object].alpha;
					currentProcessedActorPtr->beta = ListWorldObjets[object].beta;
					currentProcessedActorPtr->gamma = ListWorldObjets[object].gamma;
				} else {
					currentProcessedActorPtr->alpha = objectTable[localObjectIndex].alpha;
					currentProcessedActorPtr->beta = objectTable[localObjectIndex].beta;
					currentProcessedActorPtr->gamma = objectTable[localObjectIndex].gamma;
				}
				break;
			}
			case LM_STAGE: // STAGE
			{
				// appendFormated("LM_STAGE ");
				lifeTempVar1 = readNextArgument("newStage");
				lifeTempVar2 = readNextArgument("newRoom");
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
					currentProcessedActorPtr->dynFlags |= 1;
				} else {
					currentProcessedActorPtr->dynFlags &= 0xFFFE;
				}

				break;
			}
			case LM_UP_COOR_Y: // UP_COOR_Y
			{
				// appendFormated("LM_UP_COOR_Y ");
				initRealValue(0, -2000, -1, &currentProcessedActorPtr->YHandler);
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_LIFE: // LIFE
			{
				// appendFormated("LM_LIFE ");
				currentProcessedActorPtr->life = readNextArgument("newLife");
				break;
			}
			case LM_STAGE_LIFE: {
				// appendFormated("LM_STAGE_LIFE ");
				ListWorldObjets[currentProcessedActorPtr->indexInWorld].floorLife = readNextArgument("stageLife");
				break;
			}
			case LM_LIFE_MODE: // LIFE_MODE
			{
				// appendFormated("LM_LIFE_MODE ");
				lifeTempVar1 = readNextArgument("lifeMode");

				if (g_engine->getGameId() <= GID_JACK) {
					lifeTempVar2 = currentProcessedActorPtr->lifeMode;
				} else {
					lifeTempVar2 = currentProcessedActorPtr->lifeMode & 3;
				}

				if (lifeTempVar1 != lifeTempVar2) {
					currentProcessedActorPtr->lifeMode = lifeTempVar1;
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

				if (ListWorldObjets[lifeTempVar1].foundBody != -1) {
					if (g_engine->getGameId() == GID_AITD1) // TODO: check, really useful ?
					{
						ListWorldObjets[lifeTempVar1].flags2 &= 0x7FFF;
					}
					ListWorldObjets[lifeTempVar1].flags2 |= 0x4000;
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
									 currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
									 currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY,
									 currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
									 currentProcessedActorPtr->stage,
									 currentProcessedActorPtr->room,
									 currentProcessedActorPtr->alpha,
									 currentProcessedActorPtr->beta,
									 currentProcessedActorPtr->gamma,
									 &currentProcessedActorPtr->zv);
					break;
				}
				case 1: // flow
				{
					currentProcessedActorPtr = &objectTable[currentProcessedActorPtr->HIT_BY];

					initSpecialObjet(1,
									 currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX + currentProcessedActorPtr->hotPoint.x,
									 currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY + currentProcessedActorPtr->hotPoint.y,
									 currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ + currentProcessedActorPtr->hotPoint.z,
									 currentProcessedActorPtr->stage,
									 currentProcessedActorPtr->room,
									 0,
									 -currentProcessedActorPtr->beta,
									 0,
									 nullptr);

					currentProcessedActorPtr = currentLifeActorPtr;

					break;
				}
				case 4: // cigar smoke
				{
					initSpecialObjet(4,
									 currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
									 currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY,
									 currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
									 currentProcessedActorPtr->stage,
									 currentProcessedActorPtr->room,
									 currentProcessedActorPtr->alpha,
									 currentProcessedActorPtr->beta,
									 currentProcessedActorPtr->gamma,
									 &currentProcessedActorPtr->zv);
					break;
				}
				}
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_START_CHRONO: // START_CHRONO
			{
				// appendFormated("LM_START_CHRONO ");
				startChrono(&currentProcessedActorPtr->CHRONO);
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
					inHandTable[currentInventory] = *(int16 *)currentLifePtr;
					currentLifePtr += 2;
				} else {
					inHandTable[currentInventory] = evalVar();
				}
				break;
			}
			case LM_DROP: // DROP
			{
				// appendFormated("LM_DROP ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

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

				idx = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				x = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				y = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				z = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				room = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				stage = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				alpha = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				beta = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				gamma = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				put(x, y, z, room, stage, alpha, beta, gamma, idx);

				break;
			}
			case LM_PUT_AT: // PUT_AT
			{
				// appendFormated("LM_PUT_AT ");
				int objIdx1;
				int objIdx2;

				objIdx1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				objIdx2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				putAtObjet(objIdx1, objIdx2);
				break;
			}
			case LM_FOUND_NAME: // FOUND_NAME
			{
				// appendFormated("LM_FOUND_NAME ");
				ListWorldObjets[currentProcessedActorPtr->indexInWorld].foundName = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				break;
			}
			case LM_FOUND_BODY: // FOUND_BODY
			{
				// appendFormated("LM_FOUND_BODY ");
				ListWorldObjets[currentProcessedActorPtr->indexInWorld].foundBody = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				break;
			}
			case LM_FOUND_FLAG: // FOUND_FLAG
			{
				// appendFormated("LM_FOUND_FLAG ");
				ListWorldObjets[currentProcessedActorPtr->indexInWorld].flags2 &= 0xE000;
				ListWorldObjets[currentProcessedActorPtr->indexInWorld].flags2 |= *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				break;
			}
			case LM_FOUND_WEIGHT: // FOUND_WEIGHT
			{
				// appendFormated("LM_FOUND_WEIGHT ");
				ListWorldObjets[currentProcessedActorPtr->indexInWorld].positionInTrack = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				break;
			}
			case LM_FOUND_LIFE: // FOUND_LIFE
			{
				// appendFormated("LM_FOUND_LIFE ");
				ListWorldObjets[currentProcessedActorPtr->indexInWorld].foundLife = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				break;
			}
			case LM_READ: // READ
			{
				// appendFormated("LM_READ ");
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				if (g_engine->getGameId() == GID_AITD1) {
					lifeTempVar3 = *(int16 *)currentLifePtr;
					currentLifePtr += 2; // AITD1 CD has an extra digit, related to the VOC files to play for the text?
				}

				fadeOutPhys(0x20, 0);

				readBook(lifeTempVar2 + 1, lifeTempVar1, lifeTempVar3);

				if (g_engine->getGameId() == GID_AITD1) {
					fadeOutPhys(4, 0);
				}

				flagInitView = 2;

				break;
			}
			case LM_READ_ON_PICTURE: // TODO
			{
				// appendFormated("LM_READ_ON_PICTURE ");
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar3 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar4 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar5 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar6 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar7 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar8 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				freezeTime();

				fadeOutPhys(32, 0);
				loadPak("ITD_RESS.PAK", lifeTempVar1, aux);
				unsigned char lpalette[0x300];
				copyPalette((unsigned char *)aux + 64000, lpalette);
				convertPaletteIfRequired(lpalette);
				copyPalette(lpalette, currentGamePalette);
				gfx_setPalette(lpalette);
				turnPageFlag = false;
				lire(lifeTempVar2 + 1, lifeTempVar3, lifeTempVar4, lifeTempVar5, lifeTempVar6, 0, lifeTempVar7, lifeTempVar8);

				flagInitView = 2;

				unfreezeTime();

				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_ANIM_SAMPLE: // ANIM_SAMPLE
			{
				// appendFormated("LM_ANIM_SAMPLE ");
				lifeTempVar1 = evalVar();

				if (g_engine->getGameId() == GID_TIMEGATE) {
					currentLifePtr += 2;
				}

				lifeTempVar2 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar3 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				if (currentProcessedActorPtr->END_FRAME != 0) {
					if (currentProcessedActorPtr->ANIM == lifeTempVar2) {
						if (currentProcessedActorPtr->FRAME == lifeTempVar3) {
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
				int16 animNumber = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				int16 frameNumber = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

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
					sampleNumber = *(int16 *)currentLifePtr;
					currentLifePtr += 2;
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
					currentLifePtr += 2;
				} else {
					currentLifePtr += 4;
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
					nextSample = evalVar();
				} else {
					int newSample;

					if (g_engine->getGameId() == GID_JACK) {
						newSample = evalVar();
						nextSample = evalVar();
					} else {
						newSample = *(int16 *)currentLifePtr;
						currentLifePtr += 2;

						nextSample = *(int16 *)currentLifePtr;
						currentLifePtr += 2;
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
				nextSample = evalVar() | 0x4000;
				// setSampleFreq(0);
				// printf("LM_SAMPLE_THEN_REPEAT\n");
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_MUSIC: // MUSIC
			{
				// appendFormated("LM_MUSIC ");
				int newMusicIdx = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				playMusic(newMusicIdx);
				break;
			}
			case LM_NEXT_MUSIC: // TODO
			{
				// appendFormated("LM_NEXT_MUSIC ");
				int musicIdx = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				if (currentMusic == -1) {
					playMusic(musicIdx);
				} else {
					nextMusic = musicIdx;
				}

				break;
			}
			case LM_FADE_MUSIC: // ? fade out music and play another music ?
			{
				// appendFormated("LM_FADE_MUSIC ");
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				if (currentMusic != -1) {
					fadeMusic(0, 0, 0x8000);   // fade out music
					startChrono(&musicChrono); // fade out music timer
					currentMusic = -2;         // waiting next music
					nextMusic = lifeTempVar1;  // next music to play
				} else {
					playMusic(lifeTempVar1);
				}

				break;
			}
			case LM_RND_FREQ: // TODO
			{
				// appendFormated("LM_RND_FREQ ");
				// printf("LM_RND_FREQ\n");
				currentLifePtr += 2;
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_LIGHT: // LIGHT
			{
				// appendFormated("LM_LIGHT ");
				lifeTempVar1 = 2 - (*(int16 *)currentLifePtr << 1);
				currentLifePtr += 2;

				if (g_engine->getGameId() >= GID_JACK || !CVars[getCVarsIdx(KILLED_SORCERER)]) {
					if (lightOff != lifeTempVar1) {
						lightOff = lifeTempVar1;
						lightVar2 = 1;

						if (lightOff) {
							makeBlackPalette();
						}
					}
				}

				break;
			}
			case LM_SHAKING: // SHAKING
			{
				// appendFormated("LM_SHAKING ");
				// printf("LM_SHAKING\n");
				// shakingState = shakingAmplitude = *(int16*)(currentLifePtr);
				currentLifePtr += 2;

				/*          if(shakingState==0)
				{
				stopShaking();
				} */
				break;
			}
			case LM_PLUIE: {
				// appendFormated("LM_PLUIE ");
				// printf("LM_PLUIE\n");
				//  TODO
				currentLifePtr += 2;
				break;
			}
			case LM_WATER: // ? shaking related
			{
				// appendFormated("LM_WATER ");
				// TODO: Warning, AITD1/AITD2 diff
				// printf("LM_WATER\n");
				//          mainLoopVar1 = shakeVar1 = *(int16*)(currentLifePtr);
				currentLifePtr += 2;

				/*          if(mainLoopVar1)
				{
				//setupShaking(-600);
				}
				else
				{
				//setupShaking(1000);
				} */

				break;
			}
			case LM_CAMERA_TARGET: // CAMERA_TARGET
			{
				// appendFormated("LM_CAMERA_TARGET ");
				lifeTempVar1 = readNextArgument("Target");

				if (lifeTempVar1 != currentWorldTarget) // same target
				{
					lifeTempVar2 = ListWorldObjets[lifeTempVar1].objIndex;

					if (lifeTempVar2 != -1) {
						if (g_engine->getGameId() == GID_AITD1) {
							currentWorldTarget = lifeTempVar1;
							currentCameraTargetActor = lifeTempVar2;

							lifeTempVar3 = objectTable[currentCameraTargetActor].room;

							if (lifeTempVar3 != currentRoom) {
								needChangeRoom = 1;
								newRoom = lifeTempVar3;
							}
						} else {
							// security case, the target actor may be still be in list while already changed of stage
							// TODO: check if AITD1 could use the same code (quite probable as it's only security)
							if (objectTable[lifeTempVar2].stage != g_currentFloor) {
								currentWorldTarget = lifeTempVar1;
								changeFloor = 1;
								newFloor = objectTable[lifeTempVar2].stage;
								newRoom = objectTable[lifeTempVar2].room;
							} else {
								currentWorldTarget = lifeTempVar1;
								currentCameraTargetActor = lifeTempVar2;

								lifeTempVar3 = objectTable[currentCameraTargetActor].room;

								if (lifeTempVar3 != currentRoom) {
									needChangeRoom = 1;
									newRoom = lifeTempVar3;
								}
							}
						}
					} else // different stage
					{
						currentWorldTarget = lifeTempVar1;
						if (ListWorldObjets[lifeTempVar1].stage != g_currentFloor) {
							changeFloor = 1;
							newFloor = ListWorldObjets[lifeTempVar1].stage;
							newRoom = ListWorldObjets[lifeTempVar1].room;
						} else {
							if (currentRoom != ListWorldObjets[lifeTempVar1].room) {
								needChangeRoom = 1;
								newRoom = ListWorldObjets[lifeTempVar1].room;
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

				loadPak("ITD_RESS.PAK", pictureIndex, aux);

				if (g_engine->getGameId() > GID_AITD1) {
					fadeOutPhys(0x10, 0);
					unsigned char lpalette[0x300];
					copyPalette((unsigned char *)aux + 64000, lpalette);
					convertPaletteIfRequired(lpalette);
					copyPalette(lpalette, currentGamePalette);
					gfx_setPalette(lpalette);
				}

				fastCopyScreen(aux, frontBuffer);
				gfx_copyBlockPhys(frontBuffer, 0, 0, 320, 200);
				osystem_drawBackground();

				unsigned int chrono;
				startChrono(&chrono);

				playSound(sampleId);

				// soundFunc(0);

				do {
					unsigned int time;
					process_events();

					time = evalChrono(&chrono);

					if (time > (unsigned int)delay)
						break;
				} while (!key && !Click);

				unfreezeTime();

				flagInitView = 1;

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

				sequenceIdx = *(uint16 *)currentLifePtr;
				currentLifePtr += 2;

				fadeEntry = *(uint16 *)currentLifePtr;
				currentLifePtr += 2;

				fadeOut = *(uint16 *)currentLifePtr;
				currentLifePtr += 2;

				playSequence(sequenceIdx, fadeEntry, fadeOut);

				unfreezeTime();

				break;
			}
			case LM_DEF_SEQUENCE_SAMPLE: {
				// appendFormated("LM_DEF_SEQUENCE_SAMPLE ");
				uint16 numParams;
				int i;

				numParams = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				assert(numParams <= NUM_MAX_SEQUENCE_PARAM);

				for (i = 0; i < numParams; i++) {
					sequenceParams[i].frame = READ_LE_U16(currentLifePtr);
					currentLifePtr += 2;
					sequenceParams[i].sample = READ_LE_U16(currentLifePtr);
					currentLifePtr += 2;
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
				statusScreenAllowed = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				break;
			}
			case LM_SET_INVENTORY: {
				// appendFormated("LM_SET_INVENTORY ");
				// int inventoryIndex = *(int16*)(currentLifePtr);
				currentLifePtr += 2;

				/*          if(indeventoyIndex != currentInHand)
				{
				if(currentInHand<2)
				{
				int i;

				for(i=0;i<inventory[currentInHand];i++)
				{
				objectTable[inventoryTable[currentInHand][i]].flags2&=0x7FFF;
				}

				currentInHand = inventoryIndex
				}
				}*/
				// printf("LM_SET_INVENTORY\n");
				break;
			}
			case LM_SET_GROUND: {
				// appendFormated("LM_SET_GROUND ");
				groundLevel = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				break;
			}
			case LM_MESSAGE: {
				// appendFormated("LM_MESSAGE ");
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				makeMessage(lifeTempVar1);

				break;
			}
			case LM_MESSAGE_VALUE: {
				// appendFormated("LM_MESSAGE_VALUE ");
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;
				lifeTempVar2 = *(int16 *)currentLifePtr; // unused param ?
				currentLifePtr += 2;

				makeMessage(lifeTempVar1);

				break;
			}
			case LM_END_SEQUENCE: // ENDING
			{
				// appendFormated("LM_END_SEQUENCE ");
				// TODO!
				// printf("LM_END_SEQUENCE\n");
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_VAR: {
				// appendFormated("LM_VAR ");
				lifeTempVar1 = readNextArgument("Index");

				vars[lifeTempVar1] = evalVar("value");
				break;
			}
			case LM_INC: // INC_VAR
			{
				// appendFormated("LM_INC ");
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				vars[lifeTempVar1]++;
				break;
			}
			case LM_DEC: // DEC_VAR
			{
				// appendFormated("LM_DEC ");
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				vars[lifeTempVar1]--;
				break;
			}
			case LM_ADD: // ADD_VAR
			{
				// appendFormated("LM_ADD ");
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				vars[lifeTempVar1] += evalVar();
				break;
			}
			case LM_SUB: // SUB_VAR
			{
				// appendFormated("LM_SUB ");
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				vars[lifeTempVar1] -= evalVar();
				break;
			}
			case LM_MODIF_C_VAR:
			case LM_C_VAR: {
				// appendFormated("LM_C_VAR ");
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				CVars[lifeTempVar1] = evalVar();
				break;
			}
			////////////////////////////////////////////////////////////////////////
			case LM_IF_EGAL: {
				// appendFormated("LM_IF_EGAL ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if (lifeTempVar1 == lifeTempVar2) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)currentLifePtr;
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_DIFFERENT: {
				// appendFormated("LM_IF_DIFFERENT ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if (lifeTempVar1 != lifeTempVar2) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)currentLifePtr;
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_SUP_EGAL: {
				// appendFormated("LM_IF_SUP_EGAL ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if (lifeTempVar1 >= lifeTempVar2) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)currentLifePtr;
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_SUP: {
				// appendFormated("LM_IF_SUP ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if (lifeTempVar1 > lifeTempVar2) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)currentLifePtr;
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_INF_EGAL: {
				// appendFormated("LM_IF_INF_EGAL ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if (lifeTempVar1 <= lifeTempVar2) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)currentLifePtr;
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_IF_INF: {
				// appendFormated("LM_IF_INF ");
				lifeTempVar1 = evalVar();
				lifeTempVar2 = evalVar();

				if (lifeTempVar1 < lifeTempVar2) {
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)currentLifePtr;
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_GOTO: {
				// appendFormated("LM_GOTO ");
				lifeTempVar1 = readNextArgument("Offset");
				currentLifePtr += lifeTempVar1 * 2;
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
					currentLifePtr += 2;
				} else {
					lifeTempVar2 = *(int16 *)currentLifePtr;
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				}

				break;
			}
			case LM_MULTI_CASE: // MULTI_CASE
			{
				// appendFormated("LM_MULTI_CASE ");
				int i;
				lifeTempVar1 = *(int16 *)currentLifePtr;
				currentLifePtr += 2;

				lifeTempVar2 = 0;

				for (i = 0; i < lifeTempVar1; i++) {
					if (*(int16 *)currentLifePtr == switchVal) {
						lifeTempVar2 = 1;
					}
					currentLifePtr += 2;
				}

				if (!lifeTempVar2) {
					lifeTempVar2 = *(int16 *)currentLifePtr;
					currentLifePtr += lifeTempVar2 * 2;
					currentLifePtr += 2;
				} else {
					currentLifePtr += 2;
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
				giveUp = 1;
				exitLife = 1;
				break;
			}
			case LM_WAIT_GAME_OVER: {
				// appendFormated("LM_WAIT_GAME_OVER ");
				while (key || JoyD || Click) {
					process_events();
				}
				while (!key && !JoyD && Click) {
					process_events();
				}
				giveUp = 1;
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
			currentProcessedActorIdx = currentLifeActorIdx;
			currentProcessedActorPtr = currentLifeActorPtr;
		}
	}

	currentLifeNum = -1;
}
} // namespace Fitd
