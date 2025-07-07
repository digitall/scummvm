
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

#include "fitd/save.h"
#include "common/stream.h"
#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/floor.h"
#include "fitd/hqr.h"
#include "fitd/inventory.h"
#include "fitd/music.h"
#include "fitd/room.h"
#include "fitd/system_menu.h"
#include "fitd/vars.h"

namespace Fitd {

static void loadInterpolatedValue(InterpolatedValue *pRotateStruct, Common::SeekableReadStream *in) {
	pRotateStruct->oldValue = in->readSint16LE();
	pRotateStruct->newValue = in->readSint16LE();
	pRotateStruct->param = in->readSint16LE();
	pRotateStruct->timeOfRotate = in->readUint16LE();
}

static void saveInterpolatedValue(InterpolatedValue *pRotateStruct, Common::WriteStream *out) {
	out->writeSint16LE(pRotateStruct->oldValue);
	out->writeSint16LE(pRotateStruct->newValue);
	out->writeSint16LE(pRotateStruct->param);
	out->writeUint16LE(pRotateStruct->timeOfRotate); // TODO: check this
}

static int loadJack(Common::SeekableReadStream *in) {
	initEngine();
	initVars();

	/*const uint32 imgOffset = */ in->readUint32BE(); // offset to thumbnail start
	const uint32 palOffset = in->readUint32BE();      // offset to palette
	/*const uint32 descOffset = */ in->readUint32BE();
	const uint32 roomOffset = in->readUint32BE();
	const uint32 varsOffset = in->readUint32BE();

	in->seek(palOffset, SEEK_SET);
	in->read(g_engine->_engine->currentGamePalette, 768);

	in->seek(roomOffset, SEEK_SET);
	g_engine->_engine->currentRoom = in->readSint16LE();
	g_engine->_engine->currentFloor = in->readSint16LE();
	g_engine->_engine->currentCamera = in->readSint16LE();
	g_engine->_engine->currentWorldTarget = in->readSint16LE();
	g_engine->_engine->currentCameraTargetActor = in->readSint16LE();
	g_engine->_engine->maxObjects = in->readSint16LE();
	for (int i = 0; i < 300; i++) {
		g_engine->_engine->worldObjets[i].objIndex = in->readSint16LE();
		g_engine->_engine->worldObjets[i].body = in->readSint16LE();
		g_engine->_engine->worldObjets[i].flags = in->readSint16LE();
		g_engine->_engine->worldObjets[i].typeZV = in->readSint16LE();
		g_engine->_engine->worldObjets[i].foundBody = in->readSint16LE();
		g_engine->_engine->worldObjets[i].foundName = in->readSint16LE();
		g_engine->_engine->worldObjets[i].flags2 = in->readSint16LE();
		g_engine->_engine->worldObjets[i].foundLife = in->readSint16LE();
		g_engine->_engine->worldObjets[i].x = in->readSint16LE();
		g_engine->_engine->worldObjets[i].y = in->readSint16LE();
		g_engine->_engine->worldObjets[i].z = in->readSint16LE();
		g_engine->_engine->worldObjets[i].alpha = in->readSint16LE();
		g_engine->_engine->worldObjets[i].beta = in->readSint16LE();
		g_engine->_engine->worldObjets[i].gamma = in->readSint16LE();
		g_engine->_engine->worldObjets[i].stage = in->readSint16LE();
		g_engine->_engine->worldObjets[i].room = in->readSint16LE();
		g_engine->_engine->worldObjets[i].lifeMode = in->readSint16LE();
		g_engine->_engine->worldObjets[i].life = in->readSint16LE();
		g_engine->_engine->worldObjets[i].floorLife = in->readSint16LE();
		g_engine->_engine->worldObjets[i].anim = in->readSint16LE();
		g_engine->_engine->worldObjets[i].frame = in->readSint16LE();
		g_engine->_engine->worldObjets[i].animType = in->readSint16LE();
		g_engine->_engine->worldObjets[i].animInfo = in->readSint16LE();
		g_engine->_engine->worldObjets[i].trackMode = in->readSint16LE();
		g_engine->_engine->worldObjets[i].trackNumber = in->readSint16LE();
		g_engine->_engine->worldObjets[i].positionInTrack = in->readSint16LE();
		g_engine->_engine->worldObjets[i].mark = in->readSint16LE();
	}

	assert(g_engine->_engine->cVarsSize == 15);
	for (int i = 0; i < g_engine->_engine->cVarsSize; i++) {
		g_engine->_engine->cVars[i] = in->readSint16LE();
	}

	g_engine->_engine->inHandTable = in->readSint16LE();
	in->readSint16LE(); // TODO: what is this ? always -1 ?
	g_engine->_engine->numObjInInventoryTable = in->readSint32LE();

	for (int i = 0; i < INVENTORY_SIZE; i++) {
		g_engine->_engine->inventoryTable[i] = in->readSint16LE();
	}

	g_engine->_engine->statusScreenAllowed = in->readSint16LE();
	g_engine->_engine->giveUp = in->readSint16LE();
	g_engine->_engine->lightOff = in->readSint16LE();
	g_engine->_engine->saveShakeVar1 = in->readSint16LE();
	g_engine->_engine->saveFlagRotPal = in->readSint16LE();
	g_engine->_engine->timer = in->readUint32LE();
	g_engine->_engine->timerFreeze1 = in->readUint32LE();
	g_engine->_engine->currentMusic = in->readSint16LE();

	const int var_E = g_engine->_engine->currentCamera;

	loadFloor(g_engine->_engine->currentFloor);
	g_engine->_engine->currentCamera = -1;
	loadRoom(g_engine->_engine->currentRoom);
	const int var_16 = g_engine->_engine->currentMusic;
	g_engine->_engine->currentMusic = -1;
	playMusic(var_16);

	in->seek(varsOffset, SEEK_SET);
	const uint16 tempVarSize = in->readUint16LE();
	g_engine->_engine->varSize = tempVarSize;
	in->read(g_engine->_engine->vars, g_engine->_engine->varSize);

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		g_engine->_engine->objectTable[i].indexInWorld = in->readSint16LE();
		g_engine->_engine->objectTable[i].bodyNum = in->readSint16LE();
		g_engine->_engine->objectTable[i]._flags = in->readUint16LE();
		g_engine->_engine->objectTable[i].dynFlags = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVX1 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVX2 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVY1 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVY2 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVZ1 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVZ2 = in->readSint16LE();
		g_engine->_engine->objectTable[i].screenXMin = in->readSint16LE();
		g_engine->_engine->objectTable[i].screenYMin = in->readSint16LE();
		g_engine->_engine->objectTable[i].screenXMax = in->readSint16LE();
		g_engine->_engine->objectTable[i].screenYMax = in->readSint16LE();
		g_engine->_engine->objectTable[i].roomX = in->readSint16LE();
		g_engine->_engine->objectTable[i].roomY = in->readSint16LE();
		g_engine->_engine->objectTable[i].roomZ = in->readSint16LE();
		g_engine->_engine->objectTable[i].worldX = in->readSint16LE();
		g_engine->_engine->objectTable[i].worldY = in->readSint16LE();
		g_engine->_engine->objectTable[i].worldZ = in->readSint16LE();
		g_engine->_engine->objectTable[i].alpha = in->readSint16LE();
		g_engine->_engine->objectTable[i].beta = in->readSint16LE();
		g_engine->_engine->objectTable[i].gamma = in->readSint16LE();
		g_engine->_engine->objectTable[i].stage = in->readSint16LE();
		g_engine->_engine->objectTable[i].room = in->readSint16LE();
		g_engine->_engine->objectTable[i].lifeMode = in->readSint16LE();
		g_engine->_engine->objectTable[i].life = in->readSint16LE();
		g_engine->_engine->objectTable[i].CHRONO = in->readUint32LE();
		g_engine->_engine->objectTable[i].ROOM_CHRONO = in->readUint32LE();
		g_engine->_engine->objectTable[i].ANIM = in->readSint16LE();
		g_engine->_engine->objectTable[i].animType = in->readSint16LE();
		g_engine->_engine->objectTable[i].animInfo = in->readSint16LE();
		in->readSint16LE(); // TODO: what is this ?
		in->readSint16LE(); // TODO: what is that ?
		g_engine->_engine->objectTable[i].newAnim = in->readSint16LE();
		g_engine->_engine->objectTable[i].newAnimType = in->readSint16LE();
		g_engine->_engine->objectTable[i].newAnimInfo = in->readSint16LE();
		g_engine->_engine->objectTable[i].FRAME = in->readSint16LE();
		g_engine->_engine->objectTable[i].numOfFrames = in->readSint16LE();
		g_engine->_engine->objectTable[i].END_FRAME = in->readSint16LE();
		g_engine->_engine->objectTable[i].END_ANIM = in->readSint16LE();
		in->readSint32LE(); // TODO: and this ? a time ?
		g_engine->_engine->objectTable[i].trackMode = in->readSint16LE();
		g_engine->_engine->objectTable[i].trackNumber = in->readSint16LE();
		g_engine->_engine->objectTable[i].MARK = in->readSint16LE();
		g_engine->_engine->objectTable[i].positionInTrack = in->readSint16LE();
		g_engine->_engine->objectTable[i].stepX = in->readSint16LE();
		g_engine->_engine->objectTable[i].stepY = in->readSint16LE();
		g_engine->_engine->objectTable[i].stepZ = in->readSint16LE();
		g_engine->_engine->objectTable[i].animNegX = in->readSint16LE();
		g_engine->_engine->objectTable[i].animNegY = in->readSint16LE();
		g_engine->_engine->objectTable[i].animNegZ = in->readSint16LE();
		loadInterpolatedValue(&g_engine->_engine->objectTable[i].YHandler, in);
		g_engine->_engine->objectTable[i].falling = in->readSint16LE();
		loadInterpolatedValue(&g_engine->_engine->objectTable[i].rotate, in);
		g_engine->_engine->objectTable[i].direction = in->readSint16LE();
		g_engine->_engine->objectTable[i].speed = in->readSint16LE();
		loadInterpolatedValue(&g_engine->_engine->objectTable[i].speedChange, in);
		g_engine->_engine->objectTable[i].COL[0] = in->readSint16LE();
		g_engine->_engine->objectTable[i].COL[1] = in->readSint16LE();
		g_engine->_engine->objectTable[i].COL[2] = in->readSint16LE();
		g_engine->_engine->objectTable[i].COL_BY = in->readSint16LE();
		g_engine->_engine->objectTable[i].HARD_DEC = in->readSint16LE();
		g_engine->_engine->objectTable[i].HARD_COL = in->readSint16LE();
		g_engine->_engine->objectTable[i].HIT = in->readSint16LE();
		g_engine->_engine->objectTable[i].HIT_BY = in->readSint16LE();
		g_engine->_engine->objectTable[i].animActionType = in->readSint16LE();
		g_engine->_engine->objectTable[i].animActionANIM = in->readSint16LE();
		g_engine->_engine->objectTable[i].animActionFRAME = in->readSint16LE();
		g_engine->_engine->objectTable[i].animActionParam = in->readSint16LE();
		g_engine->_engine->objectTable[i].hitForce = in->readSint16LE();
		g_engine->_engine->objectTable[i].hotPointID = in->readSint16LE();
		in->readSint16LE(); // TODO: and this ?
		g_engine->_engine->objectTable[i].hotPoint.x = in->readSint16LE();
		g_engine->_engine->objectTable[i].hotPoint.y = in->readSint16LE();
		g_engine->_engine->objectTable[i].hotPoint.z = in->readSint16LE();

		g_engine->_engine->objectTable[i].hardMat = in->readSint16LE();
		in->readSint16LE(); // TODO: and this ?
	}

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		if (g_engine->_engine->objectTable[i].indexInWorld != -1 && g_engine->_engine->objectTable[i].bodyNum != -1) {
			byte *bodyPtr = hqrGet(g_engine->_engine->listBody, g_engine->_engine->objectTable[i].bodyNum);

			if (g_engine->_engine->objectTable[i].ANIM != -1) {
				byte *animPtr = hqrGet(g_engine->_engine->listAnim, g_engine->_engine->objectTable[i].ANIM);
				setAnimObjet(g_engine->_engine->objectTable[i].FRAME, animPtr, bodyPtr);
			}
		}
	}

	g_engine->_engine->startGameVar1 = var_E;

	return 1;
}

static int loadSaveOthers(Common::SeekableReadStream *in) {
	initEngine();
	initVars();

	in->seek(8, SEEK_SET);

	const uint var28 = in->readUint32BE();

	in->seek(var28, SEEK_SET);

	g_engine->_engine->currentRoom = in->readSint16LE();
	g_engine->_engine->currentFloor = in->readSint16LE();
	g_engine->_engine->currentCamera = in->readSint16LE();
	g_engine->_engine->currentWorldTarget = in->readSint16LE();
	g_engine->_engine->currentCameraTargetActor = in->readSint16LE();
	g_engine->_engine->maxObjects = in->readSint16LE();

	for (int16 i = 0; i < g_engine->_engine->maxObjects; i++) {
		g_engine->_engine->worldObjets[i].objIndex = in->readSint16LE();
		g_engine->_engine->worldObjets[i].body = in->readSint16LE();
		g_engine->_engine->worldObjets[i].flags = in->readSint16LE();
		g_engine->_engine->worldObjets[i].typeZV = in->readSint16LE();
		g_engine->_engine->worldObjets[i].foundBody = in->readSint16LE();
		g_engine->_engine->worldObjets[i].foundName = in->readSint16LE();
		g_engine->_engine->worldObjets[i].flags2 = in->readSint16LE();
		g_engine->_engine->worldObjets[i].foundLife = in->readSint16LE();
		g_engine->_engine->worldObjets[i].x = in->readSint16LE();
		g_engine->_engine->worldObjets[i].y = in->readSint16LE();
		g_engine->_engine->worldObjets[i].z = in->readSint16LE();
		g_engine->_engine->worldObjets[i].alpha = in->readSint16LE();
		g_engine->_engine->worldObjets[i].beta = in->readSint16LE();
		g_engine->_engine->worldObjets[i].gamma = in->readSint16LE();
		g_engine->_engine->worldObjets[i].stage = in->readSint16LE();
		g_engine->_engine->worldObjets[i].room = in->readSint16LE();
		g_engine->_engine->worldObjets[i].lifeMode = in->readSint16LE();
		g_engine->_engine->worldObjets[i].life = in->readSint16LE();
		g_engine->_engine->worldObjets[i].floorLife = in->readSint16LE();
		g_engine->_engine->worldObjets[i].anim = in->readSint16LE();
		g_engine->_engine->worldObjets[i].frame = in->readSint16LE();
		g_engine->_engine->worldObjets[i].animType = in->readSint16LE();
		g_engine->_engine->worldObjets[i].animInfo = in->readSint16LE();
		g_engine->_engine->worldObjets[i].trackMode = in->readSint16LE();
		g_engine->_engine->worldObjets[i].trackNumber = in->readSint16LE();
		g_engine->_engine->worldObjets[i].positionInTrack = in->readSint16LE();
	}

	for (uint8 i = 0; i < g_engine->_engine->cVarsSize; i++) {
		g_engine->_engine->cVars[i] = in->readSint16LE();
	}

	g_engine->_engine->inHandTable = in->readSint16LE();
	g_engine->_engine->numObjInInventoryTable = in->readSint16LE();

	for (int i = 0; i < INVENTORY_SIZE; i++) {
		g_engine->_engine->inventoryTable[i] = in->readSint16LE();
	}

	g_engine->_engine->statusScreenAllowed = in->readSint16LE();
	g_engine->_engine->giveUp = in->readSint16LE();
	g_engine->_engine->lightOff = in->readSint16LE();
	g_engine->_engine->saveShakeVar1 = in->readSint16LE();
	g_engine->_engine->saveFlagRotPal = in->readSint16LE();
	g_engine->_engine->timer = in->readUint32LE();
	g_engine->_engine->timerFreeze1 = in->readUint32LE();
	g_engine->_engine->currentMusic = in->readSint16LE();

	// timerFreeze = 1;

	const int var_E = g_engine->_engine->currentCamera;

	loadFloor(g_engine->_engine->currentFloor);
	g_engine->_engine->currentCamera = -1;
	loadRoom(g_engine->_engine->currentRoom);
	const int var_16 = g_engine->_engine->currentMusic;
	g_engine->_engine->currentMusic = -1;
	playMusic(var_16);

	in->seek(12, SEEK_SET);
	const uint offsetToVars = in->readUint32BE();
	in->seek(offsetToVars, SEEK_SET);

	const uint16 tempVarSize = in->readUint16LE();
	g_engine->_engine->varSize = tempVarSize;

	in->read(g_engine->_engine->vars, g_engine->_engine->varSize);

	/*
	configureHqrHero(g_engine->_engine->listBody,0);
	configureHqrHero(g_engine->_engine->listAnim,0);
	*/

	in->seek(16, SEEK_SET);
	const uint offsetToActors = in->readUint32BE();
	in->seek(offsetToActors, SEEK_SET);

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		g_engine->_engine->objectTable[i].indexInWorld = in->readSint16LE();
		g_engine->_engine->objectTable[i].bodyNum = in->readSint16LE();
		g_engine->_engine->objectTable[i]._flags = in->readUint16LE();
		g_engine->_engine->objectTable[i].dynFlags = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVX1 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVX1 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVX1);
		g_engine->_engine->objectTable[i].zv.ZVX2 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVX2 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVX2);
		g_engine->_engine->objectTable[i].zv.ZVY1 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVY1 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVY1);
		g_engine->_engine->objectTable[i].zv.ZVY2 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVY2 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVY2);
		g_engine->_engine->objectTable[i].zv.ZVZ1 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVZ1 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVZ1);
		g_engine->_engine->objectTable[i].zv.ZVZ2 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVZ2 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVZ2);
		g_engine->_engine->objectTable[i].screenXMin = in->readSint16LE();
		g_engine->_engine->objectTable[i].screenYMin = in->readSint16LE();
		g_engine->_engine->objectTable[i].screenXMax = in->readSint16LE();
		g_engine->_engine->objectTable[i].screenYMax = in->readSint16LE();
		g_engine->_engine->objectTable[i].roomX = in->readSint16LE();
		g_engine->_engine->objectTable[i].roomY = in->readSint16LE();
		g_engine->_engine->objectTable[i].roomZ = in->readSint16LE();
		g_engine->_engine->objectTable[i].worldX = in->readSint16LE();
		g_engine->_engine->objectTable[i].worldY = in->readSint16LE();
		g_engine->_engine->objectTable[i].worldZ = in->readSint16LE();
		g_engine->_engine->objectTable[i].alpha = in->readSint16LE();
		g_engine->_engine->objectTable[i].beta = in->readSint16LE();
		g_engine->_engine->objectTable[i].gamma = in->readSint16LE();
		g_engine->_engine->objectTable[i].room = in->readSint16LE();
		g_engine->_engine->objectTable[i].stage = in->readSint16LE();
		g_engine->_engine->objectTable[i].lifeMode = in->readSint16LE();
		g_engine->_engine->objectTable[i].life = in->readSint16LE();
		g_engine->_engine->objectTable[i].CHRONO = in->readUint32LE();
		g_engine->_engine->objectTable[i].ROOM_CHRONO = in->readUint32LE();
		g_engine->_engine->objectTable[i].ANIM = in->readSint16LE();
		g_engine->_engine->objectTable[i].animType = in->readSint16LE();
		g_engine->_engine->objectTable[i].animInfo = in->readSint16LE();
		g_engine->_engine->objectTable[i].newAnim = in->readSint16LE();
		g_engine->_engine->objectTable[i].newAnimType = in->readSint16LE();
		g_engine->_engine->objectTable[i].newAnimInfo = in->readSint16LE();
		g_engine->_engine->objectTable[i].FRAME = in->readSint16LE();
		g_engine->_engine->objectTable[i].numOfFrames = in->readSint16LE();
		g_engine->_engine->objectTable[i].END_FRAME = in->readSint16LE();
		g_engine->_engine->objectTable[i].END_ANIM = in->readSint16LE();
		g_engine->_engine->objectTable[i].trackMode = in->readSint16LE();
		g_engine->_engine->objectTable[i].trackNumber = in->readSint16LE();
		g_engine->_engine->objectTable[i].MARK = in->readSint16LE();
		g_engine->_engine->objectTable[i].positionInTrack = in->readSint16LE();
		g_engine->_engine->objectTable[i].stepX = in->readSint16LE();
		g_engine->_engine->objectTable[i].stepY = in->readSint16LE();
		g_engine->_engine->objectTable[i].stepZ = in->readSint16LE();

		loadInterpolatedValue(&g_engine->_engine->objectTable[i].YHandler, in);
		g_engine->_engine->objectTable[i].falling = in->readSint16LE();

		loadInterpolatedValue(&g_engine->_engine->objectTable[i].rotate, in);
		g_engine->_engine->objectTable[i].direction = in->readSint16LE();
		g_engine->_engine->objectTable[i].speed = in->readSint16LE();

		loadInterpolatedValue(&g_engine->_engine->objectTable[i].speedChange, in);
		g_engine->_engine->objectTable[i].COL[0] = in->readSint16LE();
		g_engine->_engine->objectTable[i].COL[1] = in->readSint16LE();
		g_engine->_engine->objectTable[i].COL[2] = in->readSint16LE();
		g_engine->_engine->objectTable[i].COL_BY = in->readSint16LE();
		g_engine->_engine->objectTable[i].HARD_DEC = in->readSint16LE();
		g_engine->_engine->objectTable[i].HARD_COL = in->readSint16LE();
		g_engine->_engine->objectTable[i].HIT = in->readSint16LE();
		g_engine->_engine->objectTable[i].HIT_BY = in->readSint16LE();
		g_engine->_engine->objectTable[i].animActionType = in->readSint16LE();
		g_engine->_engine->objectTable[i].animActionANIM = in->readSint16LE();
		g_engine->_engine->objectTable[i].animActionFRAME = in->readSint16LE();
		g_engine->_engine->objectTable[i].animActionParam = in->readSint16LE();
		g_engine->_engine->objectTable[i].hitForce = in->readSint16LE();
		g_engine->_engine->objectTable[i].hotPointID = in->readSint16LE();
		g_engine->_engine->objectTable[i].hotPoint.x = in->readSint16LE();
		g_engine->_engine->objectTable[i].hotPoint.y = in->readSint16LE();
		g_engine->_engine->objectTable[i].hotPoint.z = in->readSint16LE();
	}

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		if (g_engine->_engine->objectTable[i].indexInWorld != -1 && g_engine->_engine->objectTable[i].bodyNum != -1) {
			byte *bodyPtr = hqrGet(g_engine->_engine->listBody, g_engine->_engine->objectTable[i].bodyNum);

			if (g_engine->_engine->objectTable[i].ANIM != -1) {
				byte *animPtr = hqrGet(g_engine->_engine->listAnim, g_engine->_engine->objectTable[i].ANIM);
				setAnimObjet(g_engine->_engine->objectTable[i].FRAME, animPtr, bodyPtr);
			}
		}
	}

	g_engine->_engine->startGameVar1 = var_E;

	return 1;
}

static int loadAitd1(Common::SeekableReadStream *in) {
	initEngine();
	initVars();

	in->readUint32BE(); // offset to thumbnail start
	in->readUint32BE(); // offset to savegame description

	in->seek(8, SEEK_SET);

	const uint32 roomOffset = in->readUint32BE();

	in->seek(roomOffset, SEEK_SET);

	g_engine->_engine->currentRoom = in->readSint16LE();
	g_engine->_engine->currentFloor = in->readSint16LE();
	g_engine->_engine->currentCamera = in->readSint16LE();
	g_engine->_engine->currentWorldTarget = in->readSint16LE();
	g_engine->_engine->currentCameraTargetActor = in->readSint16LE();
	g_engine->_engine->maxObjects = in->readSint16LE();

	int oldNumMaxObj = g_engine->_engine->maxObjects;
	g_engine->_engine->maxObjects = 300; // fix for save engine..

	for (int i = 0; i < g_engine->_engine->maxObjects; i++) {
		g_engine->_engine->worldObjets[i].objIndex = in->readSint16LE();
		g_engine->_engine->worldObjets[i].body = in->readSint16LE();
		g_engine->_engine->worldObjets[i].flags = in->readSint16LE();
		g_engine->_engine->worldObjets[i].typeZV = in->readSint16LE();
		g_engine->_engine->worldObjets[i].foundBody = in->readSint16LE();
		g_engine->_engine->worldObjets[i].foundName = in->readSint16LE();
		g_engine->_engine->worldObjets[i].flags2 = in->readSint16LE();
		g_engine->_engine->worldObjets[i].foundLife = in->readSint16LE();
		g_engine->_engine->worldObjets[i].x = in->readSint16LE();
		g_engine->_engine->worldObjets[i].y = in->readSint16LE();
		g_engine->_engine->worldObjets[i].z = in->readSint16LE();
		g_engine->_engine->worldObjets[i].alpha = in->readSint16LE();
		g_engine->_engine->worldObjets[i].beta = in->readSint16LE();
		g_engine->_engine->worldObjets[i].gamma = in->readSint16LE();
		g_engine->_engine->worldObjets[i].stage = in->readSint16LE();
		g_engine->_engine->worldObjets[i].room = in->readSint16LE();
		g_engine->_engine->worldObjets[i].lifeMode = in->readSint16LE();
		g_engine->_engine->worldObjets[i].life = in->readSint16LE();
		g_engine->_engine->worldObjets[i].floorLife = in->readSint16LE();
		g_engine->_engine->worldObjets[i].anim = in->readSint16LE();
		g_engine->_engine->worldObjets[i].frame = in->readSint16LE();
		g_engine->_engine->worldObjets[i].animType = in->readSint16LE();
		g_engine->_engine->worldObjets[i].animInfo = in->readSint16LE();
		g_engine->_engine->worldObjets[i].trackMode = in->readSint16LE();
		g_engine->_engine->worldObjets[i].trackNumber = in->readSint16LE();
		g_engine->_engine->worldObjets[i].positionInTrack = in->readSint16LE();
	}

	g_engine->_engine->maxObjects = oldNumMaxObj;

	assert(g_engine->_engine->cVarsSize == 45);
	for (int i = 0; i < g_engine->_engine->cVarsSize; i++) {
		assert(sizeof(g_engine->_engine->cVars[i]) == 2);
		g_engine->_engine->cVars[i] = in->readSint16LE();
	}

	g_engine->_engine->inHandTable = in->readSint16LE();
	g_engine->_engine->numObjInInventoryTable = in->readSint16LE();

	for (int i = 0; i < AITD1_INVENTORY_SIZE; i++) {
		g_engine->_engine->inventoryTable[i] = in->readSint16LE();
	}

	g_engine->_engine->statusScreenAllowed = in->readSint16LE();
	g_engine->_engine->giveUp = in->readSint16LE();
	g_engine->_engine->lightOff = in->readSint16LE();
	g_engine->_engine->saveShakeVar1 = in->readSint16LE();
	g_engine->_engine->saveFlagRotPal = in->readSint16LE();
	g_engine->_engine->timer = in->readUint32LE();
	g_engine->_engine->timerFreeze1 = in->readUint32LE();
	g_engine->_engine->currentMusic = in->readSint16LE();
	g_engine->_engine->timerSaved = 1;

	const int var_E = g_engine->_engine->currentCamera;

	loadFloor(g_engine->_engine->currentFloor);
	g_engine->_engine->currentCamera = -1;
	loadRoom(g_engine->_engine->currentRoom);
	const int var_16 = g_engine->_engine->currentMusic;
	g_engine->_engine->currentMusic = -1;
	playMusic(var_16);

	in->seek(12, SEEK_SET);
	const uint32 offsetToVars = in->readUint32BE();
	in->seek(offsetToVars, SEEK_SET);

	const uint16 tempVarSize = in->readUint16LE();
	g_engine->_engine->varSize = tempVarSize;

	in->read(g_engine->_engine->vars, g_engine->_engine->varSize);

	hqrName(g_engine->_engine->listBody, listBodySelect[g_engine->_engine->cVars[getCVarsIdx(CHOOSE_PERSO)]]);
	hqrName(g_engine->_engine->listAnim, listAnimSelect[g_engine->_engine->cVars[getCVarsIdx(CHOOSE_PERSO)]]);

	in->seek(16, SEEK_SET);
	const uint offsetToActors = in->readUint32BE();
	in->seek(offsetToActors, SEEK_SET);

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		g_engine->_engine->objectTable[i].indexInWorld = in->readSint16LE();
		g_engine->_engine->objectTable[i].bodyNum = in->readSint16LE();
		g_engine->_engine->objectTable[i]._flags = in->readUint16LE();
		g_engine->_engine->objectTable[i].dynFlags = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVX1 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVX1 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVX1);
		g_engine->_engine->objectTable[i].zv.ZVX2 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVX2 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVX2);
		g_engine->_engine->objectTable[i].zv.ZVY1 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVY1 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVY1);
		g_engine->_engine->objectTable[i].zv.ZVY2 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVY2 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVY2);
		g_engine->_engine->objectTable[i].zv.ZVZ1 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVZ1 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVZ1);
		g_engine->_engine->objectTable[i].zv.ZVZ2 = in->readSint16LE();
		g_engine->_engine->objectTable[i].zv.ZVZ2 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVZ2);
		g_engine->_engine->objectTable[i].screenXMin = in->readSint16LE();
		g_engine->_engine->objectTable[i].screenYMin = in->readSint16LE();
		g_engine->_engine->objectTable[i].screenXMax = in->readSint16LE();
		g_engine->_engine->objectTable[i].screenYMax = in->readSint16LE();
		g_engine->_engine->objectTable[i].roomX = in->readSint16LE();
		g_engine->_engine->objectTable[i].roomY = in->readSint16LE();
		g_engine->_engine->objectTable[i].roomZ = in->readSint16LE();
		g_engine->_engine->objectTable[i].worldX = in->readSint16LE();
		g_engine->_engine->objectTable[i].worldY = in->readSint16LE();
		g_engine->_engine->objectTable[i].worldZ = in->readSint16LE();
		g_engine->_engine->objectTable[i].alpha = in->readSint16LE();
		g_engine->_engine->objectTable[i].beta = in->readSint16LE();
		g_engine->_engine->objectTable[i].gamma = in->readSint16LE();
		g_engine->_engine->objectTable[i].stage = in->readSint16LE();
		g_engine->_engine->objectTable[i].room = in->readSint16LE();
		g_engine->_engine->objectTable[i].lifeMode = in->readSint16LE();
		g_engine->_engine->objectTable[i].life = in->readSint16LE();
		g_engine->_engine->objectTable[i].CHRONO = in->readUint32LE();
		g_engine->_engine->objectTable[i].ROOM_CHRONO = in->readUint32LE();
		g_engine->_engine->objectTable[i].ANIM = in->readSint16LE();
		g_engine->_engine->objectTable[i].animType = in->readSint16LE();
		g_engine->_engine->objectTable[i].animInfo = in->readSint16LE();
		g_engine->_engine->objectTable[i].newAnim = in->readSint16LE();
		g_engine->_engine->objectTable[i].newAnimType = in->readSint16LE();
		g_engine->_engine->objectTable[i].newAnimInfo = in->readSint16LE();
		g_engine->_engine->objectTable[i].FRAME = in->readSint16LE();
		g_engine->_engine->objectTable[i].numOfFrames = in->readSint16LE();
		g_engine->_engine->objectTable[i].END_FRAME = in->readSint16LE();
		g_engine->_engine->objectTable[i].END_ANIM = in->readSint16LE();
		g_engine->_engine->objectTable[i].trackMode = in->readSint16LE();
		g_engine->_engine->objectTable[i].trackNumber = in->readSint16LE();
		g_engine->_engine->objectTable[i].MARK = in->readSint16LE();
		g_engine->_engine->objectTable[i].positionInTrack = in->readSint16LE();
		g_engine->_engine->objectTable[i].stepX = in->readSint16LE();
		g_engine->_engine->objectTable[i].stepY = in->readSint16LE();
		g_engine->_engine->objectTable[i].stepZ = in->readSint16LE();

		loadInterpolatedValue(&g_engine->_engine->objectTable[i].YHandler, in);
		g_engine->_engine->objectTable[i].falling = in->readSint16LE();

		loadInterpolatedValue(&g_engine->_engine->objectTable[i].rotate, in);
		g_engine->_engine->objectTable[i].direction = in->readSint16LE();
		g_engine->_engine->objectTable[i].speed = in->readSint16LE();

		loadInterpolatedValue(&g_engine->_engine->objectTable[i].speedChange, in);
		g_engine->_engine->objectTable[i].COL[0] = in->readSint16LE();
		g_engine->_engine->objectTable[i].COL[1] = in->readSint16LE();
		g_engine->_engine->objectTable[i].COL[2] = in->readSint16LE();
		g_engine->_engine->objectTable[i].COL_BY = in->readSint16LE();
		g_engine->_engine->objectTable[i].HARD_DEC = in->readSint16LE();
		g_engine->_engine->objectTable[i].HARD_COL = in->readSint16LE();
		g_engine->_engine->objectTable[i].HIT = in->readSint16LE();
		g_engine->_engine->objectTable[i].HIT_BY = in->readSint16LE();
		g_engine->_engine->objectTable[i].animActionType = in->readSint16LE();
		g_engine->_engine->objectTable[i].animActionANIM = in->readSint16LE();
		g_engine->_engine->objectTable[i].animActionFRAME = in->readSint16LE();
		g_engine->_engine->objectTable[i].animActionParam = in->readSint16LE();
		g_engine->_engine->objectTable[i].hitForce = in->readSint16LE();
		g_engine->_engine->objectTable[i].hotPointID = in->readSint16LE();
		g_engine->_engine->objectTable[i].hotPoint.x = in->readSint16LE();
		g_engine->_engine->objectTable[i].hotPoint.y = in->readSint16LE();
		g_engine->_engine->objectTable[i].hotPoint.z = in->readSint16LE();
	}

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		if (g_engine->_engine->objectTable[i].indexInWorld != -1 && g_engine->_engine->objectTable[i].bodyNum != -1) {
			byte *bodyPtr = hqrGet(g_engine->_engine->listBody, g_engine->_engine->objectTable[i].bodyNum);

			if (g_engine->_engine->objectTable[i].ANIM != -1) {
				byte *animPtr = hqrGet(g_engine->_engine->listAnim, g_engine->_engine->objectTable[i].ANIM);
				setAnimObjet(g_engine->_engine->objectTable[i].FRAME, animPtr, bodyPtr);
			}
		}
	}

	g_engine->_engine->startGameVar1 = var_E;

	return 1;
}

int loadGame(Common::SeekableReadStream *in) {
	switch (g_engine->getGameId()) {
	case GID_AITD1:
		return loadAitd1(in);
	case GID_JACK:
		return loadJack(in);
	default:
		return loadSaveOthers(in);
	}
}

static int saveAitd1(Common::WriteStream *out, const Common::String &desc) {
	// For safety, destroy special objects before mallocs
	for (uint i = 0; i < NUM_MAX_OBJECT; i++) {
		// For Special objects
		if (g_engine->_engine->objectTable[i].indexInWorld == -2) {
			g_engine->_engine->objectTable[i].indexInWorld = -1;
			if (g_engine->_engine->objectTable[i].ANIM == 4) {
				g_engine->_engine->cVars[getCVarsIdx(FOG_FLAG)] = 0;
				// HQ_Free_Malloc(HQ_Memory, g_engine->_engine->objectTable[i].FRAME);
			}
		}
	}

	out->writeUint32BE(20); // 0: image offset

	out->writeUint32BE(4020);  // 4: name offset
	out->writeUint32BE(4052);  // 8: room offset
	out->writeUint32BE(19838); // 12: offset to g_engine->_engine->vars
	out->writeUint32BE(20254); // 16: offset to objects

	// 20: image data
	byte img[4000];
	scaleDownImage(320, 200, 0, 0, g_engine->_engine->aux2, img, 80);
	out->write(img, 4000);

	// 4020: name
	memset(img, 0, 32);
	memcpy(img, desc.c_str(), desc.size());
	out->write(img, 32);

	// 4052: room offset
	out->writeSint16LE(g_engine->_engine->currentRoom);
	out->writeSint16LE(g_engine->_engine->currentFloor);
	out->writeSint16LE(g_engine->_engine->currentCamera);
	out->writeSint16LE(g_engine->_engine->currentWorldTarget);
	out->writeSint16LE(g_engine->_engine->currentCameraTargetActor);
	out->writeSint16LE(g_engine->_engine->maxObjects);

	for (int16 i = 0; i < 300; i++) {
		out->writeSint16LE(g_engine->_engine->worldObjets[i].objIndex);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].body);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].flags);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].typeZV);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].foundBody);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].foundName);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].flags2);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].foundLife);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].x);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].y);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].z);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].alpha);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].beta);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].gamma);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].stage);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].room);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].lifeMode);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].life);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].floorLife);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].anim);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].frame);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].animType);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].animInfo);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].trackMode);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].trackNumber);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].positionInTrack);
	}

	for (uint i = 0; i < g_engine->_engine->cVarsSize; i++) {
		out->writeSint16LE(g_engine->_engine->cVars[i]);
	}

	out->writeSint16LE(g_engine->_engine->inHandTable);
	out->writeSint16LE(g_engine->_engine->numObjInInventoryTable);

	for (uint i = 0; i < AITD1_INVENTORY_SIZE; i++) {
		out->writeSint16LE(g_engine->_engine->inventoryTable[i]);
	}

	out->writeSint16LE(g_engine->_engine->statusScreenAllowed);
	out->writeSint16LE(g_engine->_engine->giveUp);
	out->writeSint16LE(g_engine->_engine->lightOff);
	out->writeSint16LE(g_engine->_engine->saveShakeVar1);
	out->writeSint16LE(g_engine->_engine->saveFlagRotPal);
	out->writeUint32LE(g_engine->_engine->timer);
	out->writeUint32LE(g_engine->_engine->timerFreeze1);
	out->writeSint16LE(g_engine->_engine->currentMusic);

	// timerFreeze = 1;

	out->writeUint16LE(g_engine->_engine->varSize);
	out->write(g_engine->_engine->vars, g_engine->_engine->varSize);

	// pos = 20254

	for (uint i = 0; i < NUM_MAX_OBJECT; i++) {
		out->writeSint16LE(g_engine->_engine->objectTable[i].indexInWorld);
		out->writeSint16LE(g_engine->_engine->objectTable[i].bodyNum);
		out->writeUint16LE(g_engine->_engine->objectTable[i]._flags);
		out->writeSint16LE(g_engine->_engine->objectTable[i].dynFlags);
		out->writeSint16LE(g_engine->_engine->objectTable[i].zv.ZVX1);
		g_engine->_engine->objectTable[i].zv.ZVX1 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVX1);
		out->writeSint16LE(g_engine->_engine->objectTable[i].zv.ZVX2);
		g_engine->_engine->objectTable[i].zv.ZVX2 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVX2);
		out->writeSint16LE(g_engine->_engine->objectTable[i].zv.ZVY1);
		out->writeSint16LE(g_engine->_engine->objectTable[i].zv.ZVY2);
		out->writeSint16LE(g_engine->_engine->objectTable[i].zv.ZVZ1);
		out->writeSint16LE(g_engine->_engine->objectTable[i].zv.ZVZ2);
		out->writeSint16LE(g_engine->_engine->objectTable[i].screenXMin);
		out->writeSint16LE(g_engine->_engine->objectTable[i].screenYMin);
		out->writeSint16LE(g_engine->_engine->objectTable[i].screenXMax);
		out->writeSint16LE(g_engine->_engine->objectTable[i].screenYMax);
		out->writeSint16LE(g_engine->_engine->objectTable[i].roomX);
		out->writeSint16LE(g_engine->_engine->objectTable[i].roomY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].roomZ);
		out->writeSint16LE(g_engine->_engine->objectTable[i].worldX);
		out->writeSint16LE(g_engine->_engine->objectTable[i].worldY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].worldZ);
		out->writeSint16LE(g_engine->_engine->objectTable[i].alpha);
		out->writeSint16LE(g_engine->_engine->objectTable[i].beta);
		out->writeSint16LE(g_engine->_engine->objectTable[i].gamma);
		out->writeSint16LE(g_engine->_engine->objectTable[i].stage);
		out->writeSint16LE(g_engine->_engine->objectTable[i].room);
		out->writeSint16LE(g_engine->_engine->objectTable[i].lifeMode);
		out->writeSint16LE(g_engine->_engine->objectTable[i].life);
		out->writeUint32LE(g_engine->_engine->objectTable[i].CHRONO);
		out->writeUint32LE(g_engine->_engine->objectTable[i].ROOM_CHRONO);
		out->writeSint16LE(g_engine->_engine->objectTable[i].ANIM);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animType);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animInfo);
		out->writeSint16LE(g_engine->_engine->objectTable[i].newAnim);
		out->writeSint16LE(g_engine->_engine->objectTable[i].newAnimType);
		out->writeSint16LE(g_engine->_engine->objectTable[i].newAnimInfo);
		out->writeSint16LE(g_engine->_engine->objectTable[i].FRAME);
		out->writeSint16LE(g_engine->_engine->objectTable[i].numOfFrames);
		out->writeSint16LE(g_engine->_engine->objectTable[i].END_FRAME);
		out->writeSint16LE(g_engine->_engine->objectTable[i].END_ANIM);
		out->writeSint16LE(g_engine->_engine->objectTable[i].trackMode);
		out->writeSint16LE(g_engine->_engine->objectTable[i].trackNumber);
		out->writeSint16LE(g_engine->_engine->objectTable[i].MARK);
		out->writeSint16LE(g_engine->_engine->objectTable[i].positionInTrack);
		out->writeSint16LE(g_engine->_engine->objectTable[i].stepX);
		out->writeSint16LE(g_engine->_engine->objectTable[i].stepY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].stepZ);
		saveInterpolatedValue(&g_engine->_engine->objectTable[i].YHandler, out);

		out->writeSint16LE(g_engine->_engine->objectTable[i].falling);
		saveInterpolatedValue(&g_engine->_engine->objectTable[i].rotate, out);

		out->writeSint16LE(g_engine->_engine->objectTable[i].direction);
		out->writeSint16LE(g_engine->_engine->objectTable[i].speed);
		saveInterpolatedValue(&g_engine->_engine->objectTable[i].speedChange, out);

		out->writeSint16LE(g_engine->_engine->objectTable[i].COL[0]);
		out->writeSint16LE(g_engine->_engine->objectTable[i].COL[1]);
		out->writeSint16LE(g_engine->_engine->objectTable[i].COL[2]);
		out->writeSint16LE(g_engine->_engine->objectTable[i].COL_BY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].HARD_DEC);
		out->writeSint16LE(g_engine->_engine->objectTable[i].HARD_COL);
		out->writeSint16LE(g_engine->_engine->objectTable[i].HIT);
		out->writeSint16LE(g_engine->_engine->objectTable[i].HIT_BY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animActionType);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animActionANIM);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animActionFRAME);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animActionParam);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hitForce);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hotPointID);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hotPoint.x);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hotPoint.y);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hotPoint.z);
	}

	return 1;
}

static int saveJack(Common::WriteStream *out, const Common::String &desc) {
	out->writeUint32BE(20);    // image offset
	out->writeUint32BE(4020);  // pal offset
	out->writeUint32BE(4788);  // desc offset
	out->writeUint32BE(4820);  // room offset
	out->writeUint32BE(21190); // g_engine->_engine->vars offset

	byte img[4000];
	scaleDownImage(320, 200, 0, 0, g_engine->_engine->aux2, img, 80);
	out->write(img, 4000);

	out->write(g_engine->_engine->currentGamePalette, 768);

	memset(img, 0, 32);
	memcpy(img, desc.c_str(), desc.size());
	out->write(img, 32);

	out->writeSint16LE(g_engine->_engine->currentRoom);
	out->writeSint16LE(g_engine->_engine->currentFloor);
	out->writeSint16LE(g_engine->_engine->currentCamera);
	out->writeSint16LE(g_engine->_engine->currentWorldTarget);
	out->writeSint16LE(g_engine->_engine->currentCameraTargetActor);
	out->writeSint16LE(g_engine->_engine->maxObjects);

	for (int16 i = 0; i < 300; i++) {
		out->writeSint16LE(g_engine->_engine->worldObjets[i].objIndex);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].body);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].flags);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].typeZV);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].foundBody);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].foundName);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].flags2);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].foundLife);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].x);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].y);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].z);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].alpha);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].beta);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].gamma);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].stage);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].room);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].lifeMode);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].life);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].floorLife);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].anim);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].frame);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].animType);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].animInfo);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].trackMode);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].trackNumber);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].positionInTrack);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].mark);
	}

	for (uint i = 0; i < 15; i++) {
		out->writeSint16LE(g_engine->_engine->cVars[i]);
	}

	out->writeSint16LE(g_engine->_engine->inHandTable);
	out->writeSint16LE(-1); // ?
	out->writeSint32LE(g_engine->_engine->numObjInInventoryTable);
	for (uint i = 0; i < INVENTORY_SIZE; i++) {
		out->writeSint16LE(g_engine->_engine->inventoryTable[i]);
	}

	out->writeSint16LE(g_engine->_engine->statusScreenAllowed);
	out->writeSint16LE(g_engine->_engine->giveUp);
	out->writeSint16LE(g_engine->_engine->lightOff);
	out->writeSint16LE(g_engine->_engine->saveShakeVar1);
	out->writeSint16LE(g_engine->_engine->saveFlagRotPal);
	out->writeUint32LE(g_engine->_engine->timer);
	out->writeUint32LE(g_engine->_engine->timerFreeze1);
	out->writeSint16LE(g_engine->_engine->currentMusic);

	out->writeUint16LE(g_engine->_engine->varSize);
	out->write(g_engine->_engine->vars, g_engine->_engine->varSize);

	for (uint i = 0; i < NUM_MAX_OBJECT; i++) {
		out->writeSint16LE(g_engine->_engine->objectTable[i].indexInWorld);
		out->writeSint16LE(g_engine->_engine->objectTable[i].bodyNum);
		out->writeUint16LE(g_engine->_engine->objectTable[i]._flags);
		out->writeSint16LE(g_engine->_engine->objectTable[i].dynFlags);
		out->writeSint16LE(static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVX1));
		out->writeSint16LE(static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVX2));
		out->writeSint16LE(static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVY1));
		out->writeSint16LE(static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVY2));
		out->writeSint16LE(static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVZ1));
		out->writeSint16LE(static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVZ2));
		out->writeSint16LE(g_engine->_engine->objectTable[i].screenXMin);
		out->writeSint16LE(g_engine->_engine->objectTable[i].screenYMin);
		out->writeSint16LE(g_engine->_engine->objectTable[i].screenXMax);
		out->writeSint16LE(g_engine->_engine->objectTable[i].screenYMax);
		out->writeSint16LE(g_engine->_engine->objectTable[i].roomX);
		out->writeSint16LE(g_engine->_engine->objectTable[i].roomY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].roomZ);
		out->writeSint16LE(g_engine->_engine->objectTable[i].worldX);
		out->writeSint16LE(g_engine->_engine->objectTable[i].worldY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].worldZ);
		out->writeSint16LE(g_engine->_engine->objectTable[i].alpha);
		out->writeSint16LE(g_engine->_engine->objectTable[i].beta);
		out->writeSint16LE(g_engine->_engine->objectTable[i].gamma);
		out->writeSint16LE(g_engine->_engine->objectTable[i].stage);
		out->writeSint16LE(g_engine->_engine->objectTable[i].room);
		out->writeSint16LE(g_engine->_engine->objectTable[i].lifeMode);
		out->writeSint16LE(g_engine->_engine->objectTable[i].life);
		out->writeUint32LE(g_engine->_engine->objectTable[i].CHRONO);
		out->writeUint32LE(g_engine->_engine->objectTable[i].ROOM_CHRONO);
		out->writeSint16LE(g_engine->_engine->objectTable[i].ANIM);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animType);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animInfo);
		out->writeSint16LE(0); // ?
		out->writeSint16LE(0); // ?
		out->writeSint16LE(g_engine->_engine->objectTable[i].newAnim);
		out->writeSint16LE(g_engine->_engine->objectTable[i].newAnimType);
		out->writeSint16LE(g_engine->_engine->objectTable[i].newAnimInfo);
		out->writeSint16LE(g_engine->_engine->objectTable[i].FRAME);
		out->writeSint16LE(g_engine->_engine->objectTable[i].numOfFrames);
		out->writeSint16LE(g_engine->_engine->objectTable[i].END_FRAME);
		out->writeSint16LE(g_engine->_engine->objectTable[i].END_ANIM);
		out->writeSint32LE(0); // time ?
		out->writeSint16LE(g_engine->_engine->objectTable[i].trackMode);
		out->writeSint16LE(g_engine->_engine->objectTable[i].trackNumber);
		out->writeSint16LE(g_engine->_engine->objectTable[i].MARK);
		out->writeSint16LE(g_engine->_engine->objectTable[i].positionInTrack);
		out->writeSint16LE(g_engine->_engine->objectTable[i].stepX);
		out->writeSint16LE(g_engine->_engine->objectTable[i].stepY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].stepZ);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animNegX);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animNegY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animNegZ);
		saveInterpolatedValue(&g_engine->_engine->objectTable[i].YHandler, out);
		out->writeSint16LE(g_engine->_engine->objectTable[i].falling);
		saveInterpolatedValue(&g_engine->_engine->objectTable[i].rotate, out);
		out->writeSint16LE(g_engine->_engine->objectTable[i].direction);
		out->writeSint16LE(g_engine->_engine->objectTable[i].speed);
		saveInterpolatedValue(&g_engine->_engine->objectTable[i].speedChange, out);
		out->writeSint16LE(g_engine->_engine->objectTable[i].COL[0]);
		out->writeSint16LE(g_engine->_engine->objectTable[i].COL[1]);
		out->writeSint16LE(g_engine->_engine->objectTable[i].COL[2]);
		out->writeSint16LE(g_engine->_engine->objectTable[i].COL_BY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].HARD_DEC);
		out->writeSint16LE(g_engine->_engine->objectTable[i].HARD_COL);
		out->writeSint16LE(g_engine->_engine->objectTable[i].HIT);
		out->writeSint16LE(g_engine->_engine->objectTable[i].HIT_BY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animActionType);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animActionANIM);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animActionFRAME);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animActionParam);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hitForce);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hotPointID);
		out->writeSint16LE(0);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hotPoint.x);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hotPoint.y);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hotPoint.z);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hardMat);
		out->writeSint16LE(0);
	}

	return 1;
}

int makeSaveOthers(Common::WriteStream *out, const Common::String &desc) {
	const uint32 var28 = 0;

	out->writeUint32LE(var28);
	out->writeUint32LE(var28);

	const uint32 currentRoomOffset = 20;
	out->writeUint32BE(currentRoomOffset);

	uint32 varsOffset = 0;
	uint32 objsOffset = 0;
	switch (g_engine->getGameId()) {
	case GID_AITD2:
		varsOffset = 19144;
		objsOffset = 20110;
		break;
	default:
		// TODO:
		assert(0);
		break;
	}
	out->writeUint32BE(varsOffset);
	out->writeUint32BE(objsOffset);

	out->writeSint16LE(g_engine->_engine->currentRoom);
	out->writeSint16LE(g_engine->_engine->currentFloor);
	out->writeSint16LE(g_engine->_engine->currentCamera);
	out->writeSint16LE(g_engine->_engine->currentWorldTarget);
	out->writeSint16LE(g_engine->_engine->currentCameraTargetActor);
	out->writeSint16LE(g_engine->_engine->maxObjects);

	for (int16 i = 0; i < g_engine->_engine->maxObjects; i++) {
		out->writeSint16LE(g_engine->_engine->worldObjets[i].objIndex);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].body);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].flags);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].typeZV);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].foundBody);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].foundName);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].flags2);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].foundLife);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].x);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].y);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].z);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].alpha);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].beta);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].gamma);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].stage);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].room);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].lifeMode);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].life);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].floorLife);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].anim);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].frame);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].animType);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].animInfo);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].trackMode);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].trackNumber);
		out->writeSint16LE(g_engine->_engine->worldObjets[i].positionInTrack);
	}

	for (uint i = 0; i < g_engine->_engine->cVarsSize; i++) {
		out->writeSint16LE(g_engine->_engine->cVars[i]);
	}

	out->writeSint16LE(g_engine->_engine->inHandTable);
	out->writeSint16LE(g_engine->_engine->numObjInInventoryTable);

	for (uint i = 0; i < INVENTORY_SIZE; i++) {
		out->writeSint16LE(g_engine->_engine->inventoryTable[i]);
	}

	out->writeSint16LE(g_engine->_engine->statusScreenAllowed);
	out->writeSint16LE(g_engine->_engine->giveUp);
	out->writeSint16LE(g_engine->_engine->lightOff);
	out->writeSint16LE(g_engine->_engine->saveShakeVar1);
	out->writeSint16LE(g_engine->_engine->saveFlagRotPal);
	out->writeUint32LE(g_engine->_engine->timer);
	out->writeUint32LE(g_engine->_engine->timerFreeze1);
	out->writeSint16LE(g_engine->_engine->currentMusic);

	// timerFreeze = 1;

	const uint32 pos = out->pos();
	for (uint i = 0; i < varsOffset - pos; i++) {
		out->writeByte(0);
	}

	out->writeUint16LE(g_engine->_engine->varSize);
	out->write(g_engine->_engine->vars, g_engine->_engine->varSize);

	// pos = 20254

	for (uint i = 0; i < NUM_MAX_OBJECT; i++) {
		out->writeSint16LE(g_engine->_engine->objectTable[i].indexInWorld);
		out->writeSint16LE(g_engine->_engine->objectTable[i].bodyNum);
		out->writeUint16LE(g_engine->_engine->objectTable[i]._flags);
		out->writeSint16LE(g_engine->_engine->objectTable[i].dynFlags);
		out->writeSint16LE(g_engine->_engine->objectTable[i].zv.ZVX1);
		g_engine->_engine->objectTable[i].zv.ZVX1 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVX1);
		out->writeSint16LE(g_engine->_engine->objectTable[i].zv.ZVX2);
		g_engine->_engine->objectTable[i].zv.ZVX2 = static_cast<int16>(g_engine->_engine->objectTable[i].zv.ZVX2);
		out->writeSint16LE(g_engine->_engine->objectTable[i].zv.ZVY1);
		out->writeSint16LE(g_engine->_engine->objectTable[i].zv.ZVY2);
		out->writeSint16LE(g_engine->_engine->objectTable[i].zv.ZVZ1);
		out->writeSint16LE(g_engine->_engine->objectTable[i].zv.ZVZ2);
		out->writeSint16LE(g_engine->_engine->objectTable[i].screenXMin);
		out->writeSint16LE(g_engine->_engine->objectTable[i].screenYMin);
		out->writeSint16LE(g_engine->_engine->objectTable[i].screenXMax);
		out->writeSint16LE(g_engine->_engine->objectTable[i].screenYMax);
		out->writeSint16LE(g_engine->_engine->objectTable[i].roomX);
		out->writeSint16LE(g_engine->_engine->objectTable[i].roomY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].roomZ);
		out->writeSint16LE(g_engine->_engine->objectTable[i].worldX);
		out->writeSint16LE(g_engine->_engine->objectTable[i].worldY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].worldZ);
		out->writeSint16LE(g_engine->_engine->objectTable[i].alpha);
		out->writeSint16LE(g_engine->_engine->objectTable[i].beta);
		out->writeSint16LE(g_engine->_engine->objectTable[i].gamma);
		out->writeSint16LE(g_engine->_engine->objectTable[i].stage);
		out->writeSint16LE(g_engine->_engine->objectTable[i].room);
		out->writeSint16LE(g_engine->_engine->objectTable[i].lifeMode);
		out->writeSint16LE(g_engine->_engine->objectTable[i].life);
		out->writeUint32LE(g_engine->_engine->objectTable[i].CHRONO);
		out->writeUint32LE(g_engine->_engine->objectTable[i].ROOM_CHRONO);
		out->writeSint16LE(g_engine->_engine->objectTable[i].ANIM);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animType);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animInfo);
		out->writeSint16LE(g_engine->_engine->objectTable[i].newAnim);
		out->writeSint16LE(g_engine->_engine->objectTable[i].newAnimType);
		out->writeSint16LE(g_engine->_engine->objectTable[i].newAnimInfo);
		out->writeSint16LE(g_engine->_engine->objectTable[i].FRAME);
		out->writeSint16LE(g_engine->_engine->objectTable[i].numOfFrames);
		out->writeSint16LE(g_engine->_engine->objectTable[i].END_FRAME);
		out->writeSint16LE(g_engine->_engine->objectTable[i].END_ANIM);
		out->writeSint16LE(g_engine->_engine->objectTable[i].trackMode);
		out->writeSint16LE(g_engine->_engine->objectTable[i].trackNumber);
		out->writeSint16LE(g_engine->_engine->objectTable[i].MARK);
		out->writeSint16LE(g_engine->_engine->objectTable[i].positionInTrack);
		out->writeSint16LE(g_engine->_engine->objectTable[i].stepX);
		out->writeSint16LE(g_engine->_engine->objectTable[i].stepY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].stepZ);

		saveInterpolatedValue(&g_engine->_engine->objectTable[i].YHandler, out);
		out->writeSint16LE(g_engine->_engine->objectTable[i].falling);

		saveInterpolatedValue(&g_engine->_engine->objectTable[i].rotate, out);
		out->writeSint16LE(g_engine->_engine->objectTable[i].direction);
		out->writeSint16LE(g_engine->_engine->objectTable[i].speed);

		saveInterpolatedValue(&g_engine->_engine->objectTable[i].speedChange, out);
		out->writeSint16LE(g_engine->_engine->objectTable[i].COL[0]);
		out->writeSint16LE(g_engine->_engine->objectTable[i].COL[1]);
		out->writeSint16LE(g_engine->_engine->objectTable[i].COL[2]);
		out->writeSint16LE(g_engine->_engine->objectTable[i].COL_BY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].HARD_DEC);
		out->writeSint16LE(g_engine->_engine->objectTable[i].HARD_COL);
		out->writeSint16LE(g_engine->_engine->objectTable[i].HIT);
		out->writeSint16LE(g_engine->_engine->objectTable[i].HIT_BY);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animActionType);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animActionANIM);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animActionFRAME);
		out->writeSint16LE(g_engine->_engine->objectTable[i].animActionParam);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hitForce);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hotPointID);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hotPoint.x);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hotPoint.y);
		out->writeSint16LE(g_engine->_engine->objectTable[i].hotPoint.z);
	}

	return 1;
}

int saveGame(Common::WriteStream *out, const Common::String &desc) {
	switch (g_engine->getGameId()) {
	case GID_AITD1:
		return saveAitd1(out, desc);
	case GID_JACK:
		return saveJack(out, desc);
	default:
		return makeSaveOthers(out, desc);
	}
}
} // namespace Fitd
