
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
#include "fitd/fitd.h"
#include "fitd/floor.h"
#include "fitd/hqr.h"
#include "fitd/inventory.h"
#include "fitd/music.h"
#include "fitd/room.h"
#include "fitd/vars.h"
#include "system_menu.h"

namespace Fitd {

static void loadInterpolatedValue(interpolatedValue *pRotateStruct, Common::SeekableReadStream *in) {
	assert(sizeof(pRotateStruct->oldAngle) == 2);
	pRotateStruct->oldAngle = in->readSint16LE();

	assert(sizeof(pRotateStruct->newAngle) == 2);
	pRotateStruct->newAngle = in->readSint16LE();

	assert(sizeof(pRotateStruct->param) == 2);
	pRotateStruct->param = in->readSint16LE();

	assert(sizeof(pRotateStruct->timeOfRotate) == 4);
	pRotateStruct->timeOfRotate = in->readUint16LE();
}

static void saveInterpolatedValue(interpolatedValue *pRotateStruct, Common::WriteStream *out) {
	assert(sizeof(pRotateStruct->oldAngle) == 2);
	out->writeSint16LE(pRotateStruct->oldAngle);

	assert(sizeof(pRotateStruct->newAngle) == 2);
	out->writeSint16LE(pRotateStruct->newAngle);

	assert(sizeof(pRotateStruct->param) == 2);
	out->writeSint16LE(pRotateStruct->param);

	assert(sizeof(pRotateStruct->timeOfRotate) == 4);
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
	in->read(currentGamePalette, 768);

	in->seek(roomOffset, SEEK_SET);
	currentRoom = in->readSint16LE();
	g_currentFloor = in->readSint16LE();
	currentCamera = in->readSint16LE();
	currentWorldTarget = in->readSint16LE();
	currentCameraTargetActor = in->readSint16LE();
	maxObjects = in->readSint16LE();
	for (int i = 0; i < 300; i++) {
		ListWorldObjets[i].objIndex = in->readSint16LE();
		ListWorldObjets[i].body = in->readSint16LE();
		ListWorldObjets[i].flags = in->readSint16LE();
		ListWorldObjets[i].typeZV = in->readSint16LE();
		ListWorldObjets[i].foundBody = in->readSint16LE();
		ListWorldObjets[i].foundName = in->readSint16LE();
		ListWorldObjets[i].flags2 = in->readSint16LE();
		ListWorldObjets[i].foundLife = in->readSint16LE();
		ListWorldObjets[i].x = in->readSint16LE();
		ListWorldObjets[i].y = in->readSint16LE();
		ListWorldObjets[i].z = in->readSint16LE();
		ListWorldObjets[i].alpha = in->readSint16LE();
		ListWorldObjets[i].beta = in->readSint16LE();
		ListWorldObjets[i].gamma = in->readSint16LE();
		ListWorldObjets[i].stage = in->readSint16LE();
		ListWorldObjets[i].room = in->readSint16LE();
		ListWorldObjets[i].lifeMode = in->readSint16LE();
		ListWorldObjets[i].life = in->readSint16LE();
		ListWorldObjets[i].floorLife = in->readSint16LE();
		ListWorldObjets[i].anim = in->readSint16LE();
		ListWorldObjets[i].frame = in->readSint16LE();
		ListWorldObjets[i].animType = in->readSint16LE();
		ListWorldObjets[i].animInfo = in->readSint16LE();
		ListWorldObjets[i].trackMode = in->readSint16LE();
		ListWorldObjets[i].trackNumber = in->readSint16LE();
		ListWorldObjets[i].positionInTrack = in->readSint16LE();
		ListWorldObjets[i].mark = in->readSint16LE();
	}

	assert(CVarsSize == 15);
	for (int i = 0; i < CVarsSize; i++) {
		CVars[i] = in->readSint16LE();
	}

	inHandTable[0] = in->readSint16LE();
	in->readSint16LE(); // TODO: what is this ? always -1 ?
	numObjInInventoryTable[0] = in->readSint32LE();

	for (int i = 0; i < INVENTORY_SIZE; i++) {
		inventoryTable[0][i] = in->readSint16LE();
	}

	statusScreenAllowed = in->readSint16LE();
	giveUp = in->readSint16LE();
	lightOff = in->readSint16LE();
	shakingAmplitude = in->readSint16LE();
	shakeVar1 = in->readSint16LE();
	timer = in->readUint32LE();
	timerFreeze1 = in->readUint32LE();
	currentMusic = in->readSint16LE();

	const int var_E = currentCamera;

	loadFloor(g_currentFloor);
	currentCamera = -1;
	loadRoom(currentRoom);
	const int var_16 = currentMusic;
	currentMusic = -1;
	playMusic(var_16);

	in->seek(varsOffset, SEEK_SET);
	const uint16 tempVarSize = in->readUint16LE();
	varSize = tempVarSize;
	in->read(vars, varSize);

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		objectTable[i].indexInWorld = in->readSint16LE();
		objectTable[i].bodyNum = in->readSint16LE();
		objectTable[i]._flags = in->readUint16LE();
		objectTable[i].dynFlags = in->readSint16LE();
		objectTable[i].zv.ZVX1 = in->readSint16LE();
		objectTable[i].zv.ZVX2 = in->readSint16LE();
		objectTable[i].zv.ZVY1 = in->readSint16LE();
		objectTable[i].zv.ZVY2 = in->readSint16LE();
		objectTable[i].zv.ZVZ1 = in->readSint16LE();
		objectTable[i].zv.ZVZ2 = in->readSint16LE();
		objectTable[i].screenXMin = in->readSint16LE();
		objectTable[i].screenYMin = in->readSint16LE();
		objectTable[i].screenXMax = in->readSint16LE();
		objectTable[i].screenYMax = in->readSint16LE();
		objectTable[i].roomX = in->readSint16LE();
		objectTable[i].roomY = in->readSint16LE();
		objectTable[i].roomZ = in->readSint16LE();
		objectTable[i].worldX = in->readSint16LE();
		objectTable[i].worldY = in->readSint16LE();
		objectTable[i].worldZ = in->readSint16LE();
		objectTable[i].alpha = in->readSint16LE();
		objectTable[i].beta = in->readSint16LE();
		objectTable[i].gamma = in->readSint16LE();
		objectTable[i].stage = in->readSint16LE();
		objectTable[i].room = in->readSint16LE();
		objectTable[i].lifeMode = in->readSint16LE();
		objectTable[i].life = in->readSint16LE();
		objectTable[i].CHRONO = in->readUint32LE();
		objectTable[i].ROOM_CHRONO = in->readUint32LE();
		objectTable[i].ANIM = in->readSint16LE();
		objectTable[i].animType = in->readSint16LE();
		objectTable[i].animInfo = in->readSint16LE();
		in->readSint16LE(); // TODO: what is this ?
		in->readSint16LE(); // TODO: what is that ?
		objectTable[i].newAnim = in->readSint16LE();
		objectTable[i].newAnimType = in->readSint16LE();
		objectTable[i].newAnimInfo = in->readSint16LE();
		objectTable[i].FRAME = in->readSint16LE();
		objectTable[i].numOfFrames = in->readSint16LE();
		objectTable[i].END_FRAME = in->readSint16LE();
		objectTable[i].END_ANIM = in->readSint16LE();
		in->readSint32LE(); // TODO: and this ? a time ?
		objectTable[i].trackMode = in->readSint16LE();
		objectTable[i].trackNumber = in->readSint16LE();
		objectTable[i].MARK = in->readSint16LE();
		objectTable[i].positionInTrack = in->readSint16LE();
		objectTable[i].stepX = in->readSint16LE();
		objectTable[i].stepY = in->readSint16LE();
		objectTable[i].stepZ = in->readSint16LE();
		objectTable[i].animNegX = in->readSint16LE();
		objectTable[i].animNegY = in->readSint16LE();
		objectTable[i].animNegZ = in->readSint16LE();
		loadInterpolatedValue(&objectTable[i].YHandler, in);
		objectTable[i].falling = in->readSint16LE();
		loadInterpolatedValue(&objectTable[i].rotate, in);
		objectTable[i].direction = in->readSint16LE();
		objectTable[i].speed = in->readSint16LE();
		loadInterpolatedValue(&objectTable[i].speedChange, in);
		objectTable[i].COL[0] = in->readSint16LE();
		objectTable[i].COL[1] = in->readSint16LE();
		objectTable[i].COL[2] = in->readSint16LE();
		objectTable[i].COL_BY = in->readSint16LE();
		objectTable[i].HARD_DEC = in->readSint16LE();
		objectTable[i].HARD_COL = in->readSint16LE();
		objectTable[i].HIT = in->readSint16LE();
		objectTable[i].HIT_BY = in->readSint16LE();
		objectTable[i].animActionType = in->readSint16LE();
		objectTable[i].animActionANIM = in->readSint16LE();
		objectTable[i].animActionFRAME = in->readSint16LE();
		objectTable[i].animActionParam = in->readSint16LE();
		objectTable[i].hitForce = in->readSint16LE();
		objectTable[i].hotPointID = in->readSint16LE();
		in->readSint16LE(); // TODO: and this ?
		objectTable[i].hotPoint.x = in->readSint16LE();
		objectTable[i].hotPoint.y = in->readSint16LE();
		objectTable[i].hotPoint.z = in->readSint16LE();

		objectTable[i].hardMat = in->readSint16LE();
		in->readSint16LE(); // TODO: and this ?
	}

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		if (objectTable[i].indexInWorld != -1 && objectTable[i].bodyNum != -1) {
			char *bodyPtr = HQR_Get(listBody, objectTable[i].bodyNum);

			if (objectTable[i].ANIM != -1) {
				char *animPtr = HQR_Get(listAnim, objectTable[i].ANIM);
				setAnimObjet(objectTable[i].FRAME, animPtr, bodyPtr);
			}
		}
	}

	startGameVar1 = var_E;

	return 1;
}

static int loadSaveOthers(Common::SeekableReadStream *in) {
	int i;
	int oldNumMaxObj = 0;

	initEngine();
	initVars();

	in->seek(8, SEEK_SET);

	const unsigned int var28 = in->readUint32BE();

	in->seek(var28, SEEK_SET);

	assert(sizeof(currentRoom) == 2);
	currentRoom = in->readSint16LE();

	assert(sizeof(g_currentFloor) == 2);
	g_currentFloor = in->readSint16LE();

	assert(sizeof(currentCamera) == 2);
	currentCamera = in->readSint16LE();

	assert(sizeof(currentWorldTarget) == 2);
	currentWorldTarget = in->readSint16LE();

	assert(sizeof(currentCameraTargetActor) == 2);
	currentCameraTargetActor = in->readSint16LE();

	assert(sizeof(maxObjects) == 2);
	maxObjects = in->readSint16LE();

	if (g_engine->getGameId() == GID_AITD1) {
		oldNumMaxObj = maxObjects;
		maxObjects = 300; // fix for save engine...
	}

	for (i = 0; i < maxObjects; i++) {
		assert(sizeof(ListWorldObjets[i].objIndex) == 2);
		ListWorldObjets[i].objIndex = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].body) == 2);
		ListWorldObjets[i].body = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].flags) == 2);
		ListWorldObjets[i].flags = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].typeZV) == 2);
		ListWorldObjets[i].typeZV = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].foundBody) == 2);
		ListWorldObjets[i].foundBody = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].foundName) == 2);
		ListWorldObjets[i].foundName = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].flags2) == 2);
		ListWorldObjets[i].flags2 = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].foundLife) == 2);
		ListWorldObjets[i].foundLife = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].x) == 2);
		ListWorldObjets[i].x = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].y) == 2);
		ListWorldObjets[i].y = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].z) == 2);
		ListWorldObjets[i].z = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].alpha) == 2);
		ListWorldObjets[i].alpha = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].beta) == 2);
		ListWorldObjets[i].beta = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].gamma) == 2);
		ListWorldObjets[i].gamma = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].stage) == 2);
		ListWorldObjets[i].stage = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].room) == 2);
		ListWorldObjets[i].room = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].lifeMode) == 2);
		ListWorldObjets[i].lifeMode = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].life) == 2);
		ListWorldObjets[i].life = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].floorLife) == 2);
		ListWorldObjets[i].floorLife = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].anim) == 2);
		ListWorldObjets[i].anim = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].frame) == 2);
		ListWorldObjets[i].frame = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].animType) == 2);
		ListWorldObjets[i].animType = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].animInfo) == 2);
		ListWorldObjets[i].animInfo = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].trackMode) == 2);
		ListWorldObjets[i].trackMode = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].trackNumber) == 2);
		ListWorldObjets[i].trackNumber = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].positionInTrack) == 2);
		ListWorldObjets[i].positionInTrack = in->readSint16LE();
	}

	if (g_engine->getGameId() == GID_AITD1) {
		maxObjects = oldNumMaxObj;
	}

	if (g_engine->getGameId() == GID_AITD1) {
		assert(CVarsSize == 45);
	}

	for (i = 0; i < CVarsSize; i++) {
		assert(sizeof(CVars[i]) == 2);
		CVars[i] = in->readSint16LE();
	}

	for (int inventoryId = 0; inventoryId < NUM_MAX_INVENTORY; inventoryId++) {
		assert(sizeof(inHandTable[inventoryId]) == 2);
		inHandTable[inventoryId] = in->readSint16LE();

		assert(sizeof(numObjInInventoryTable[inventoryId]) == 2);
		numObjInInventoryTable[inventoryId] = in->readSint16LE();

		for (i = 0; i < INVENTORY_SIZE; i++) {
			assert(sizeof(inventoryTable[inventoryId][i]) == 2);
			inventoryTable[inventoryId][i] = in->readSint16LE();
		}
	}

	assert(sizeof(statusScreenAllowed) == 2);
	statusScreenAllowed = in->readSint16LE();

	assert(sizeof(giveUp) == 2);
	giveUp = in->readSint16LE();

	assert(sizeof(lightOff) == 2);
	lightOff = in->readSint16LE();

	assert(sizeof(shakingAmplitude) == 2);
	shakingAmplitude = in->readSint16LE();

	assert(sizeof(shakeVar1) == 2);
	shakeVar1 = in->readSint16LE();

	assert(sizeof(timer) == 4);
	timer = in->readUint32LE();

	assert(sizeof(timerFreeze1) == 4);
	timerFreeze1 = in->readUint32LE();

	assert(sizeof(currentMusic) == 2);
	currentMusic = in->readSint16LE();

	// timerFreeze = 1;

	const int var_E = currentCamera;

	loadFloor(g_currentFloor);
	currentCamera = -1;
	loadRoom(currentRoom);
	const int var_16 = currentMusic;
	currentMusic = -1;
	playMusic(var_16);

	in->seek(12, SEEK_SET);
	const unsigned int offsetToVars = in->readUint32BE();
	in->seek(offsetToVars, SEEK_SET);

	const uint16 tempVarSize = in->readUint16LE();
	varSize = tempVarSize;

	in->read(vars, varSize);

	if (g_engine->getGameId() == GID_AITD1) {
		// TODO ?
		// configureHqrHero(listBody, listBodySelect[CVars[getCVarsIdx(CHOOSE_PERSO)]]);
		// configureHqrHero(listAnim, listAnimSelect[CVars[getCVarsIdx(CHOOSE_PERSO)]]);
	} else {
		/*
		configureHqrHero(listBody,0);
		configureHqrHero(listAnim,0);
		*/
	}

	in->seek(16, SEEK_SET);
	const unsigned int offsetToActors = in->readUint32BE();
	in->seek(offsetToActors, SEEK_SET);

	for (i = 0; i < NUM_MAX_OBJECT; i++) {
		assert(sizeof(objectTable[i].indexInWorld) == 2);
		objectTable[i].indexInWorld = in->readSint16LE();

		assert(sizeof(objectTable[i].bodyNum) == 2);
		objectTable[i].bodyNum = in->readSint16LE();

		assert(sizeof(objectTable[i]._flags) == 2);
		objectTable[i]._flags = in->readUint16LE();

		assert(sizeof(objectTable[i].dynFlags) == 2);
		objectTable[i].dynFlags = in->readSint16LE();

		//    assert(sizeof(actorTable[i].zv.ZVX1) == 2);
		objectTable[i].zv.ZVX1 = in->readSint16LE();
		objectTable[i].zv.ZVX1 = (int16)objectTable[i].zv.ZVX1;

		//    assert(sizeof(actorTable[i].zv.ZVX2) == 2);
		objectTable[i].zv.ZVX2 = in->readSint16LE();
		objectTable[i].zv.ZVX2 = (int16)objectTable[i].zv.ZVX2;

		//    assert(sizeof(actorTable[i].zv.ZVY1) == 2);
		objectTable[i].zv.ZVY1 = in->readSint16LE();
		objectTable[i].zv.ZVY1 = (int16)objectTable[i].zv.ZVY1;

		//    assert(sizeof(actorTable[i].zv.ZVY2) == 2);
		objectTable[i].zv.ZVY2 = in->readSint16LE();
		objectTable[i].zv.ZVY2 = (int16)objectTable[i].zv.ZVY2;

		//    assert(sizeof(actorTable[i].zv.ZVZ1) == 2);
		objectTable[i].zv.ZVZ1 = in->readSint16LE();
		objectTable[i].zv.ZVZ1 = (int16)objectTable[i].zv.ZVZ1;

		//    assert(sizeof(actorTable[i].zv.ZVZ2) == 2);
		objectTable[i].zv.ZVZ2 = in->readSint16LE();
		objectTable[i].zv.ZVZ2 = (int16)objectTable[i].zv.ZVZ2;

		assert(sizeof(objectTable[i].screenXMin) == 2);
		objectTable[i].screenXMin = in->readSint16LE();

		assert(sizeof(objectTable[i].screenYMin) == 2);
		objectTable[i].screenYMin = in->readSint16LE();

		assert(sizeof(objectTable[i].screenXMax) == 2);
		objectTable[i].screenXMax = in->readSint16LE();

		assert(sizeof(objectTable[i].screenYMax) == 2);
		objectTable[i].screenYMax = in->readSint16LE();

		assert(sizeof(objectTable[i].roomX) == 2);
		objectTable[i].roomX = in->readSint16LE();

		assert(sizeof(objectTable[i].roomY) == 2);
		objectTable[i].roomY = in->readSint16LE();

		assert(sizeof(objectTable[i].roomZ) == 2);
		objectTable[i].roomZ = in->readSint16LE();

		assert(sizeof(objectTable[i].worldX) == 2);
		objectTable[i].worldX = in->readSint16LE();

		assert(sizeof(objectTable[i].worldY) == 2);
		objectTable[i].worldY = in->readSint16LE();

		assert(sizeof(objectTable[i].worldZ) == 2);
		objectTable[i].worldZ = in->readSint16LE();

		assert(sizeof(objectTable[i].alpha) == 2);
		objectTable[i].alpha = in->readSint16LE();

		assert(sizeof(objectTable[i].beta) == 2);
		objectTable[i].beta = in->readSint16LE();

		assert(sizeof(objectTable[i].gamma) == 2);
		objectTable[i].gamma = in->readSint16LE();

		assert(sizeof(objectTable[i].room) == 2);
		objectTable[i].room = in->readSint16LE();

		assert(sizeof(objectTable[i].stage) == 2);
		objectTable[i].stage = in->readSint16LE();

		assert(sizeof(objectTable[i].lifeMode) == 2);
		objectTable[i].lifeMode = in->readSint16LE();

		assert(sizeof(objectTable[i].life) == 2);
		objectTable[i].life = in->readSint16LE();

		assert(sizeof(objectTable[i].CHRONO) == 4);
		objectTable[i].CHRONO = in->readUint32LE();

		assert(sizeof(objectTable[i].ROOM_CHRONO) == 4);
		objectTable[i].ROOM_CHRONO = in->readUint32LE();

		assert(sizeof(objectTable[i].ANIM) == 2);
		objectTable[i].ANIM = in->readSint16LE();

		assert(sizeof(objectTable[i].animType) == 2);
		objectTable[i].animType = in->readSint16LE();

		assert(sizeof(objectTable[i].animInfo) == 2);
		objectTable[i].animInfo = in->readSint16LE();

		assert(sizeof(objectTable[i].newAnim) == 2);
		objectTable[i].newAnim = in->readSint16LE();

		assert(sizeof(objectTable[i].newAnimType) == 2);
		objectTable[i].newAnimType = in->readSint16LE();

		assert(sizeof(objectTable[i].newAnimInfo) == 2);
		objectTable[i].newAnimInfo = in->readSint16LE();

		assert(sizeof(objectTable[i].FRAME) == 2);
		objectTable[i].FRAME = in->readSint16LE();

		assert(sizeof(objectTable[i].numOfFrames) == 2);
		objectTable[i].numOfFrames = in->readSint16LE();

		assert(sizeof(objectTable[i].END_FRAME) == 2);
		objectTable[i].END_FRAME = in->readSint16LE();

		assert(sizeof(objectTable[i].END_ANIM) == 2);
		objectTable[i].END_ANIM = in->readSint16LE();

		assert(sizeof(objectTable[i].trackMode) == 2);
		objectTable[i].trackMode = in->readSint16LE();

		assert(sizeof(objectTable[i].trackNumber) == 2);
		objectTable[i].trackNumber = in->readSint16LE();

		assert(sizeof(objectTable[i].MARK) == 2);
		objectTable[i].MARK = in->readSint16LE();

		assert(sizeof(objectTable[i].positionInTrack) == 2);
		objectTable[i].positionInTrack = in->readSint16LE();

		assert(sizeof(objectTable[i].stepX) == 2);
		objectTable[i].stepX = in->readSint16LE();

		assert(sizeof(objectTable[i].stepY) == 2);
		objectTable[i].stepY = in->readSint16LE();

		assert(sizeof(objectTable[i].stepZ) == 2); // 45
		objectTable[i].stepZ = in->readSint16LE();

		loadInterpolatedValue(&objectTable[i].YHandler, in);

		assert(sizeof(objectTable[i].falling) == 2);
		objectTable[i].falling = in->readSint16LE();

		loadInterpolatedValue(&objectTable[i].rotate, in);

		assert(sizeof(objectTable[i].direction) == 2);
		objectTable[i].direction = in->readSint16LE();

		assert(sizeof(objectTable[i].speed) == 2);
		objectTable[i].speed = in->readSint16LE();

		loadInterpolatedValue(&objectTable[i].speedChange, in);

		assert(sizeof(objectTable[i].COL[0]) == 2);
		objectTable[i].COL[0] = in->readSint16LE();

		assert(sizeof(objectTable[i].COL[1]) == 2);
		objectTable[i].COL[1] = in->readSint16LE();

		assert(sizeof(objectTable[i].COL[2]) == 2);
		objectTable[i].COL[2] = in->readSint16LE();

		assert(sizeof(objectTable[i].COL_BY) == 2);
		objectTable[i].COL_BY = in->readSint16LE();

		assert(sizeof(objectTable[i].HARD_DEC) == 2);
		objectTable[i].HARD_DEC = in->readSint16LE();

		assert(sizeof(objectTable[i].HARD_COL) == 2);
		objectTable[i].HARD_COL = in->readSint16LE();

		assert(sizeof(objectTable[i].HIT) == 2);
		objectTable[i].HIT = in->readSint16LE();

		assert(sizeof(objectTable[i].HIT_BY) == 2);
		objectTable[i].HIT_BY = in->readSint16LE();

		assert(sizeof(objectTable[i].animActionType) == 2);
		objectTable[i].animActionType = in->readSint16LE();

		assert(sizeof(objectTable[i].animActionANIM) == 2);
		objectTable[i].animActionANIM = in->readSint16LE();

		assert(sizeof(objectTable[i].animActionFRAME) == 2);
		objectTable[i].animActionFRAME = in->readSint16LE();

		assert(sizeof(objectTable[i].animActionParam) == 2);
		objectTable[i].animActionParam = in->readSint16LE();

		assert(sizeof(objectTable[i].hitForce) == 2);
		objectTable[i].hitForce = in->readSint16LE();

		assert(sizeof(objectTable[i].hotPointID) == 2);
		objectTable[i].hotPointID = in->readSint16LE();

		assert(sizeof(objectTable[i].hotPoint.x) == 2);
		objectTable[i].hotPoint.x = in->readSint16LE();

		assert(sizeof(objectTable[i].hotPoint.y) == 2);
		objectTable[i].hotPoint.y = in->readSint16LE();

		assert(sizeof(objectTable[i].hotPoint.z) == 2);
		objectTable[i].hotPoint.z = in->readSint16LE();
	}

	for (i = 0; i < NUM_MAX_OBJECT; i++) {
		if (objectTable[i].indexInWorld != -1 && objectTable[i].bodyNum != -1) {
			char *bodyPtr = HQR_Get(listBody, objectTable[i].bodyNum);

			if (objectTable[i].ANIM != -1) {
				char *animPtr = HQR_Get(listAnim, objectTable[i].ANIM);
				setAnimObjet(objectTable[i].FRAME, animPtr, bodyPtr);
			}
		}
	}

	startGameVar1 = var_E;

	return 1;
}

static int loadAitd1(Common::SeekableReadStream *in) {
	int i;
	int oldNumMaxObj = 0;

	initEngine();
	initVars();

	in->readUint32BE(); // offset to thumbnail start
	in->readUint32BE(); // offset to savegame description

	in->seek(8, SEEK_SET);

	const uint32 roomOffset = in->readUint32BE();

	in->seek(roomOffset, SEEK_SET);

	assert(sizeof(currentRoom) == 2);
	currentRoom = in->readSint16LE();

	assert(sizeof(g_currentFloor) == 2);
	g_currentFloor = in->readSint16LE();

	assert(sizeof(currentCamera) == 2);
	currentCamera = in->readSint16LE();

	assert(sizeof(currentWorldTarget) == 2);
	currentWorldTarget = in->readSint16LE();

	assert(sizeof(currentCameraTargetActor) == 2);
	currentCameraTargetActor = in->readSint16LE();

	assert(sizeof(maxObjects) == 2);
	maxObjects = in->readSint16LE();

	if (g_engine->getGameId() == GID_AITD1) {
		oldNumMaxObj = maxObjects;
		maxObjects = 300; // fix for save engine..
	}

	for (i = 0; i < maxObjects; i++) {
		assert(sizeof(ListWorldObjets[i].objIndex) == 2);
		ListWorldObjets[i].objIndex = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].body) == 2);
		ListWorldObjets[i].body = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].flags) == 2);
		ListWorldObjets[i].flags = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].typeZV) == 2);
		ListWorldObjets[i].typeZV = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].foundBody) == 2);
		ListWorldObjets[i].foundBody = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].foundName) == 2);
		ListWorldObjets[i].foundName = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].flags2) == 2);
		ListWorldObjets[i].flags2 = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].foundLife) == 2);
		ListWorldObjets[i].foundLife = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].x) == 2);
		ListWorldObjets[i].x = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].y) == 2);
		ListWorldObjets[i].y = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].z) == 2);
		ListWorldObjets[i].z = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].alpha) == 2);
		ListWorldObjets[i].alpha = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].beta) == 2);
		ListWorldObjets[i].beta = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].gamma) == 2);
		ListWorldObjets[i].gamma = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].stage) == 2);
		ListWorldObjets[i].stage = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].room) == 2);
		ListWorldObjets[i].room = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].lifeMode) == 2);
		ListWorldObjets[i].lifeMode = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].life) == 2);
		ListWorldObjets[i].life = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].floorLife) == 2);
		ListWorldObjets[i].floorLife = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].anim) == 2);
		ListWorldObjets[i].anim = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].frame) == 2);
		ListWorldObjets[i].frame = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].animType) == 2);
		ListWorldObjets[i].animType = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].animInfo) == 2);
		ListWorldObjets[i].animInfo = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].trackMode) == 2);
		ListWorldObjets[i].trackMode = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].trackNumber) == 2);
		ListWorldObjets[i].trackNumber = in->readSint16LE();

		assert(sizeof(ListWorldObjets[i].positionInTrack) == 2);
		ListWorldObjets[i].positionInTrack = in->readSint16LE();
	}

	if (g_engine->getGameId() == GID_AITD1) {
		maxObjects = oldNumMaxObj;
	}

	if (g_engine->getGameId() == GID_AITD1) {
		assert(CVarsSize == 45);
	}

	for (i = 0; i < CVarsSize; i++) {
		assert(sizeof(CVars[i]) == 2);
		CVars[i] = in->readSint16LE();
	}

	const int maxInventory = g_engine->getGameId() == GID_AITD1 ? 1 : NUM_MAX_INVENTORY;
	for (int inventoryId = 0; inventoryId < maxInventory; inventoryId++) {
		assert(sizeof(inHandTable[inventoryId]) == 2);
		inHandTable[inventoryId] = in->readSint16LE();

		assert(sizeof(numObjInInventoryTable[inventoryId]) == 2);
		numObjInInventoryTable[inventoryId] = in->readSint16LE();

		for (i = 0; i < AITD1_INVENTORY_SIZE; i++) {
			assert(sizeof(inventoryTable[inventoryId][i]) == 2);
			inventoryTable[inventoryId][i] = in->readSint16LE();
		}
	}

	assert(sizeof(statusScreenAllowed) == 2);
	statusScreenAllowed = in->readSint16LE();

	assert(sizeof(giveUp) == 2);
	giveUp = in->readSint16LE();

	assert(sizeof(lightOff) == 2);
	lightOff = in->readSint16LE();

	assert(sizeof(shakingAmplitude) == 2);
	shakingAmplitude = in->readSint16LE();

	assert(sizeof(shakeVar1) == 2);
	shakeVar1 = in->readSint16LE();

	assert(sizeof(timer) == 4);
	timer = in->readUint32LE();

	assert(sizeof(timerFreeze1) == 4);
	timerFreeze1 = in->readUint32LE();

	assert(sizeof(currentMusic) == 2);
	currentMusic = in->readSint16LE();

	timerSaved = 1;

	const int var_E = currentCamera;

	loadFloor(g_currentFloor);
	currentCamera = -1;
	loadRoom(currentRoom);
	const int var_16 = currentMusic;
	currentMusic = -1;
	playMusic(var_16);

	in->seek(12, SEEK_SET);
	const uint32 offsetToVars = in->readUint32BE();
	in->seek(offsetToVars, SEEK_SET);

	const uint16 tempVarSize = in->readUint16LE();
	varSize = tempVarSize;

	in->read(vars, varSize);

	if (g_engine->getGameId() == GID_AITD1) {
		HQ_Name(listBody, listBodySelect[CVars[getCVarsIdx(CHOOSE_PERSO)]]);
		HQ_Name(listAnim, listAnimSelect[CVars[getCVarsIdx(CHOOSE_PERSO)]]);
	} else {
		/*
		HQ_Name(listBody,0);
		HQ_Name(listAnim,0);
		*/
	}

	in->seek(16, SEEK_SET);
	const unsigned int offsetToActors = in->readUint32BE();
	in->seek(offsetToActors, SEEK_SET);

	for (i = 0; i < NUM_MAX_OBJECT; i++) {
		assert(sizeof(objectTable[i].indexInWorld) == 2);
		objectTable[i].indexInWorld = in->readSint16LE();

		assert(sizeof(objectTable[i].bodyNum) == 2);
		objectTable[i].bodyNum = in->readSint16LE();

		assert(sizeof(objectTable[i]._flags) == 2);
		objectTable[i]._flags = in->readUint16LE();

		assert(sizeof(objectTable[i].dynFlags) == 2);
		objectTable[i].dynFlags = in->readSint16LE();

		//    assert(sizeof(actorTable[i].zv.ZVX1) == 2);
		objectTable[i].zv.ZVX1 = in->readSint16LE();
		objectTable[i].zv.ZVX1 = (int16)objectTable[i].zv.ZVX1;

		//    assert(sizeof(actorTable[i].zv.ZVX2) == 2);
		objectTable[i].zv.ZVX2 = in->readSint16LE();
		objectTable[i].zv.ZVX2 = (int16)objectTable[i].zv.ZVX2;

		//    assert(sizeof(actorTable[i].zv.ZVY1) == 2);
		objectTable[i].zv.ZVY1 = in->readSint16LE();
		objectTable[i].zv.ZVY1 = (int16)objectTable[i].zv.ZVY1;

		//    assert(sizeof(actorTable[i].zv.ZVY2) == 2);
		objectTable[i].zv.ZVY2 = in->readSint16LE();
		objectTable[i].zv.ZVY2 = (int16)objectTable[i].zv.ZVY2;

		//    assert(sizeof(actorTable[i].zv.ZVZ1) == 2);
		objectTable[i].zv.ZVZ1 = in->readSint16LE();
		objectTable[i].zv.ZVZ1 = (int16)objectTable[i].zv.ZVZ1;

		//    assert(sizeof(actorTable[i].zv.ZVZ2) == 2);
		objectTable[i].zv.ZVZ2 = in->readSint16LE();
		objectTable[i].zv.ZVZ2 = (int16)objectTable[i].zv.ZVZ2;

		assert(sizeof(objectTable[i].screenXMin) == 2);
		objectTable[i].screenXMin = in->readSint16LE();

		assert(sizeof(objectTable[i].screenYMin) == 2);
		objectTable[i].screenYMin = in->readSint16LE();

		assert(sizeof(objectTable[i].screenXMax) == 2);
		objectTable[i].screenXMax = in->readSint16LE();

		assert(sizeof(objectTable[i].screenYMax) == 2);
		objectTable[i].screenYMax = in->readSint16LE();

		assert(sizeof(objectTable[i].roomX) == 2);
		objectTable[i].roomX = in->readSint16LE();

		assert(sizeof(objectTable[i].roomY) == 2);
		objectTable[i].roomY = in->readSint16LE();

		assert(sizeof(objectTable[i].roomZ) == 2);
		objectTable[i].roomZ = in->readSint16LE();

		assert(sizeof(objectTable[i].worldX) == 2);
		objectTable[i].worldX = in->readSint16LE();

		assert(sizeof(objectTable[i].worldY) == 2);
		objectTable[i].worldY = in->readSint16LE();

		assert(sizeof(objectTable[i].worldZ) == 2);
		objectTable[i].worldZ = in->readSint16LE();

		assert(sizeof(objectTable[i].alpha) == 2);
		objectTable[i].alpha = in->readSint16LE();

		assert(sizeof(objectTable[i].beta) == 2);
		objectTable[i].beta = in->readSint16LE();

		assert(sizeof(objectTable[i].gamma) == 2);
		objectTable[i].gamma = in->readSint16LE();

		assert(sizeof(objectTable[i].stage) == 2);
		objectTable[i].stage = in->readSint16LE();

		assert(sizeof(objectTable[i].room) == 2);
		objectTable[i].room = in->readSint16LE();

		assert(sizeof(objectTable[i].lifeMode) == 2);
		objectTable[i].lifeMode = in->readSint16LE();

		assert(sizeof(objectTable[i].life) == 2);
		objectTable[i].life = in->readSint16LE();

		assert(sizeof(objectTable[i].CHRONO) == 4);
		objectTable[i].CHRONO = in->readUint32LE();

		assert(sizeof(objectTable[i].ROOM_CHRONO) == 4);
		objectTable[i].ROOM_CHRONO = in->readUint32LE();

		assert(sizeof(objectTable[i].ANIM) == 2);
		objectTable[i].ANIM = in->readSint16LE();

		assert(sizeof(objectTable[i].animType) == 2);
		objectTable[i].animType = in->readSint16LE();

		assert(sizeof(objectTable[i].animInfo) == 2);
		objectTable[i].animInfo = in->readSint16LE();

		assert(sizeof(objectTable[i].newAnim) == 2);
		objectTable[i].newAnim = in->readSint16LE();

		assert(sizeof(objectTable[i].newAnimType) == 2);
		objectTable[i].newAnimType = in->readSint16LE();

		assert(sizeof(objectTable[i].newAnimInfo) == 2);
		objectTable[i].newAnimInfo = in->readSint16LE();

		assert(sizeof(objectTable[i].FRAME) == 2);
		objectTable[i].FRAME = in->readSint16LE();

		assert(sizeof(objectTable[i].numOfFrames) == 2);
		objectTable[i].numOfFrames = in->readSint16LE();

		assert(sizeof(objectTable[i].END_FRAME) == 2);
		objectTable[i].END_FRAME = in->readSint16LE();

		assert(sizeof(objectTable[i].END_ANIM) == 2);
		objectTable[i].END_ANIM = in->readSint16LE();

		assert(sizeof(objectTable[i].trackMode) == 2);
		objectTable[i].trackMode = in->readSint16LE();

		assert(sizeof(objectTable[i].trackNumber) == 2);
		objectTable[i].trackNumber = in->readSint16LE();

		assert(sizeof(objectTable[i].MARK) == 2);
		objectTable[i].MARK = in->readSint16LE();

		assert(sizeof(objectTable[i].positionInTrack) == 2);
		objectTable[i].positionInTrack = in->readSint16LE();

		assert(sizeof(objectTable[i].stepX) == 2);
		objectTable[i].stepX = in->readSint16LE();

		assert(sizeof(objectTable[i].stepY) == 2);
		objectTable[i].stepY = in->readSint16LE();

		assert(sizeof(objectTable[i].stepZ) == 2); // 45
		objectTable[i].stepZ = in->readSint16LE();

		loadInterpolatedValue(&objectTable[i].YHandler, in);

		assert(sizeof(objectTable[i].falling) == 2);
		objectTable[i].falling = in->readSint16LE();

		loadInterpolatedValue(&objectTable[i].rotate, in);

		assert(sizeof(objectTable[i].direction) == 2);
		objectTable[i].direction = in->readSint16LE();

		assert(sizeof(objectTable[i].speed) == 2);
		objectTable[i].speed = in->readSint16LE();

		loadInterpolatedValue(&objectTable[i].speedChange, in);

		assert(sizeof(objectTable[i].COL[0]) == 2);
		objectTable[i].COL[0] = in->readSint16LE();

		assert(sizeof(objectTable[i].COL[1]) == 2);
		objectTable[i].COL[1] = in->readSint16LE();

		assert(sizeof(objectTable[i].COL[2]) == 2);
		objectTable[i].COL[2] = in->readSint16LE();

		assert(sizeof(objectTable[i].COL_BY) == 2);
		objectTable[i].COL_BY = in->readSint16LE();

		assert(sizeof(objectTable[i].HARD_DEC) == 2);
		objectTable[i].HARD_DEC = in->readSint16LE();

		assert(sizeof(objectTable[i].HARD_COL) == 2);
		objectTable[i].HARD_COL = in->readSint16LE();

		assert(sizeof(objectTable[i].HIT) == 2);
		objectTable[i].HIT = in->readSint16LE();

		assert(sizeof(objectTable[i].HIT_BY) == 2);
		objectTable[i].HIT_BY = in->readSint16LE();

		assert(sizeof(objectTable[i].animActionType) == 2);
		objectTable[i].animActionType = in->readSint16LE();

		assert(sizeof(objectTable[i].animActionANIM) == 2);
		objectTable[i].animActionANIM = in->readSint16LE();

		assert(sizeof(objectTable[i].animActionFRAME) == 2);
		objectTable[i].animActionFRAME = in->readSint16LE();

		assert(sizeof(objectTable[i].animActionParam) == 2);
		objectTable[i].animActionParam = in->readSint16LE();

		assert(sizeof(objectTable[i].hitForce) == 2);
		objectTable[i].hitForce = in->readSint16LE();

		assert(sizeof(objectTable[i].hotPointID) == 2);
		objectTable[i].hotPointID = in->readSint16LE();

		assert(sizeof(objectTable[i].hotPoint.x) == 2);
		objectTable[i].hotPoint.x = in->readSint16LE();

		assert(sizeof(objectTable[i].hotPoint.y) == 2);
		objectTable[i].hotPoint.y = in->readSint16LE();

		assert(sizeof(objectTable[i].hotPoint.z) == 2);
		objectTable[i].hotPoint.z = in->readSint16LE();
	}

	for (i = 0; i < NUM_MAX_OBJECT; i++) {
		if (objectTable[i].indexInWorld != -1 && objectTable[i].bodyNum != -1) {
			char *bodyPtr = HQR_Get(listBody, objectTable[i].bodyNum);

			if (objectTable[i].ANIM != -1) {
				char *animPtr = HQR_Get(listAnim, objectTable[i].ANIM);
				setAnimObjet(objectTable[i].FRAME, animPtr, bodyPtr);
			}
		}
	}

	startGameVar1 = var_E;

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

static int saveAitd1(Common::WriteStream *out) {
	int oldNumMaxObj = 0;

	for (uint i = 0; i < NUM_MAX_OBJECT; i++) {
		if (objectTable[i].indexInWorld == -2) {
			objectTable[i].indexInWorld = -1;
			if (objectTable[i].ANIM == 4) {
				CVars[getCVarsIdx(FOG_FLAG)] = 0;
				// HQ_Free_Malloc(HQ_Memory, objectTable[i].FRAME);
			}
		}
	}

	out->writeUint32BE(20); // 0: image offset

	out->writeUint32BE(4020);  // 4: name offset
	out->writeUint32BE(4052);  // 8: room offset
	out->writeUint32BE(19838); // 12: offset to vars
	out->writeUint32BE(20254); // 16: offset to objects

	// 20: image data
	char img[4000];
	scaleDownImage(320, 200, 0, 0, aux2, img, 80);
	out->write(img, 4000);

	// 4020: name
	memset(img, 0, 32);
	out->write(img, 32);

	// 4052: room offset

	assert(sizeof(currentRoom) == 2);
	out->writeSint16LE(currentRoom);

	assert(sizeof(g_currentFloor) == 2);
	out->writeSint16LE(g_currentFloor);

	assert(sizeof(currentCamera) == 2);
	out->writeSint16LE(currentCamera);

	assert(sizeof(currentWorldTarget) == 2);
	out->writeSint16LE(currentWorldTarget);

	assert(sizeof(currentCameraTargetActor) == 2);
	out->writeSint16LE(currentCameraTargetActor);

	assert(sizeof(maxObjects) == 2);
	out->writeSint16LE(maxObjects);

	oldNumMaxObj = maxObjects;
	maxObjects = 300; // fix for save engine...

	for (int16 i = 0; i < maxObjects; i++) {
		assert(sizeof(ListWorldObjets[i].objIndex) == 2);
		out->writeSint16LE(ListWorldObjets[i].objIndex);

		assert(sizeof(ListWorldObjets[i].body) == 2);
		out->writeSint16LE(ListWorldObjets[i].body);

		assert(sizeof(ListWorldObjets[i].flags) == 2);
		out->writeSint16LE(ListWorldObjets[i].flags);

		assert(sizeof(ListWorldObjets[i].typeZV) == 2);
		out->writeSint16LE(ListWorldObjets[i].typeZV);

		assert(sizeof(ListWorldObjets[i].foundBody) == 2);
		out->writeSint16LE(ListWorldObjets[i].foundBody);

		assert(sizeof(ListWorldObjets[i].foundName) == 2);
		out->writeSint16LE(ListWorldObjets[i].foundName);

		assert(sizeof(ListWorldObjets[i].flags2) == 2);
		out->writeSint16LE(ListWorldObjets[i].flags2);

		assert(sizeof(ListWorldObjets[i].foundLife) == 2);
		out->writeSint16LE(ListWorldObjets[i].foundLife);

		assert(sizeof(ListWorldObjets[i].x) == 2);
		out->writeSint16LE(ListWorldObjets[i].x);

		assert(sizeof(ListWorldObjets[i].y) == 2);
		out->writeSint16LE(ListWorldObjets[i].y);

		assert(sizeof(ListWorldObjets[i].z) == 2);
		out->writeSint16LE(ListWorldObjets[i].z);

		assert(sizeof(ListWorldObjets[i].alpha) == 2);
		out->writeSint16LE(ListWorldObjets[i].alpha);

		assert(sizeof(ListWorldObjets[i].beta) == 2);
		out->writeSint16LE(ListWorldObjets[i].beta);

		assert(sizeof(ListWorldObjets[i].gamma) == 2);
		out->writeSint16LE(ListWorldObjets[i].gamma);

		assert(sizeof(ListWorldObjets[i].stage) == 2);
		out->writeSint16LE(ListWorldObjets[i].stage);

		assert(sizeof(ListWorldObjets[i].room) == 2);
		out->writeSint16LE(ListWorldObjets[i].room);

		assert(sizeof(ListWorldObjets[i].lifeMode) == 2);
		out->writeSint16LE(ListWorldObjets[i].lifeMode);

		assert(sizeof(ListWorldObjets[i].life) == 2);
		out->writeSint16LE(ListWorldObjets[i].life);

		assert(sizeof(ListWorldObjets[i].floorLife) == 2);
		out->writeSint16LE(ListWorldObjets[i].floorLife);

		assert(sizeof(ListWorldObjets[i].anim) == 2);
		out->writeSint16LE(ListWorldObjets[i].anim);

		assert(sizeof(ListWorldObjets[i].frame) == 2);
		out->writeSint16LE(ListWorldObjets[i].frame);

		assert(sizeof(ListWorldObjets[i].animType) == 2);
		out->writeSint16LE(ListWorldObjets[i].animType);

		assert(sizeof(ListWorldObjets[i].animInfo) == 2);
		out->writeSint16LE(ListWorldObjets[i].animInfo);

		assert(sizeof(ListWorldObjets[i].trackMode) == 2);
		out->writeSint16LE(ListWorldObjets[i].trackMode);

		assert(sizeof(ListWorldObjets[i].trackNumber) == 2);
		out->writeSint16LE(ListWorldObjets[i].trackNumber);

		assert(sizeof(ListWorldObjets[i].positionInTrack) == 2);
		out->writeSint16LE(ListWorldObjets[i].positionInTrack);
	}

	maxObjects = oldNumMaxObj;

	assert(CVarsSize == 45);

	for (uint i = 0; i < CVarsSize; i++) {
		assert(sizeof(CVars[i]) == 2);
		out->writeSint16LE(CVars[i]);
	}

	const int maxInventory = 1;
	for (int inventoryId = 0; inventoryId < maxInventory; inventoryId++) {
		assert(sizeof(inHandTable[inventoryId]) == 2);
		out->writeSint16LE(inHandTable[inventoryId]);

		assert(sizeof(numObjInInventoryTable[inventoryId]) == 2);
		out->writeSint16LE(numObjInInventoryTable[inventoryId]);

		for (uint i = 0; i < AITD1_INVENTORY_SIZE; i++) {
			assert(sizeof(inventoryTable[inventoryId][i]) == 2);
			out->writeSint16LE(inventoryTable[inventoryId][i]);
		}
	}

	assert(sizeof(statusScreenAllowed) == 2);
	out->writeSint16LE(statusScreenAllowed);

	assert(sizeof(giveUp) == 2);
	out->writeSint16LE(giveUp);

	assert(sizeof(lightOff) == 2);
	out->writeSint16LE(lightOff);

	assert(sizeof(shakingAmplitude) == 2);
	out->writeSint16LE(shakingAmplitude);

	assert(sizeof(shakeVar1) == 2);
	out->writeSint16LE(shakeVar1);

	assert(sizeof(timer) == 4);
	out->writeUint32LE(timer);

	assert(sizeof(timerFreeze1) == 4);
	out->writeUint32LE(timerFreeze1);

	assert(sizeof(currentMusic) == 2);
	out->writeSint16LE(currentMusic);

	// timerFreeze = 1;

	out->writeUint16LE(varSize);
	out->write(vars, varSize);

	// pos = 20254

	for (uint i = 0; i < NUM_MAX_OBJECT; i++) {
		assert(sizeof(objectTable[i].indexInWorld) == 2);
		out->writeSint16LE(objectTable[i].indexInWorld);

		assert(sizeof(objectTable[i].bodyNum) == 2);
		out->writeSint16LE(objectTable[i].bodyNum);

		assert(sizeof(objectTable[i]._flags) == 2);
		out->writeUint16LE(objectTable[i]._flags);

		assert(sizeof(objectTable[i].dynFlags) == 2);
		out->writeSint16LE(objectTable[i].dynFlags);

		//    assert(sizeof(actorTable[i].zv.ZVX1) == 2);
		out->writeSint16LE(objectTable[i].zv.ZVX1);
		objectTable[i].zv.ZVX1 = (int16)objectTable[i].zv.ZVX1;

		//    assert(sizeof(actorTable[i].zv.ZVX2) == 2);
		out->writeSint16LE(objectTable[i].zv.ZVX2);
		objectTable[i].zv.ZVX2 = (int16)objectTable[i].zv.ZVX2;

		//    assert(sizeof(actorTable[i].zv.ZVY1) == 2);
		out->writeSint16LE(objectTable[i].zv.ZVY1);

		//    assert(sizeof(actorTable[i].zv.ZVY2) == 2);
		out->writeSint16LE(objectTable[i].zv.ZVY2);

		//    assert(sizeof(actorTable[i].zv.ZVZ1) == 2);
		out->writeSint16LE(objectTable[i].zv.ZVZ1);

		//    assert(sizeof(actorTable[i].zv.ZVZ2) == 2);
		out->writeSint16LE(objectTable[i].zv.ZVZ2);

		assert(sizeof(objectTable[i].screenXMin) == 2);
		out->writeSint16LE(objectTable[i].screenXMin);

		assert(sizeof(objectTable[i].screenYMin) == 2);
		out->writeSint16LE(objectTable[i].screenYMin);

		assert(sizeof(objectTable[i].screenXMax) == 2);
		out->writeSint16LE(objectTable[i].screenXMax);

		assert(sizeof(objectTable[i].screenYMax) == 2);
		out->writeSint16LE(objectTable[i].screenYMax);

		assert(sizeof(objectTable[i].roomX) == 2);
		out->writeSint16LE(objectTable[i].roomX);

		assert(sizeof(objectTable[i].roomY) == 2);
		out->writeSint16LE(objectTable[i].roomY);

		assert(sizeof(objectTable[i].roomZ) == 2);
		out->writeSint16LE(objectTable[i].roomZ);

		assert(sizeof(objectTable[i].worldX) == 2);
		out->writeSint16LE(objectTable[i].worldX);

		assert(sizeof(objectTable[i].worldY) == 2);
		out->writeSint16LE(objectTable[i].worldY);

		assert(sizeof(objectTable[i].worldZ) == 2);
		out->writeSint16LE(objectTable[i].worldZ);

		assert(sizeof(objectTable[i].alpha) == 2);
		out->writeSint16LE(objectTable[i].alpha);

		assert(sizeof(objectTable[i].beta) == 2);
		out->writeSint16LE(objectTable[i].beta);

		assert(sizeof(objectTable[i].gamma) == 2);
		out->writeSint16LE(objectTable[i].gamma);

		assert(sizeof(objectTable[i].stage) == 2);
		out->writeSint16LE(objectTable[i].stage);

		assert(sizeof(objectTable[i].room) == 2);
		out->writeSint16LE(objectTable[i].room);

		assert(sizeof(objectTable[i].lifeMode) == 2);
		out->writeSint16LE(objectTable[i].lifeMode);

		assert(sizeof(objectTable[i].life) == 2);
		out->writeSint16LE(objectTable[i].life);

		assert(sizeof(objectTable[i].CHRONO) == 4);
		out->writeUint32LE(objectTable[i].CHRONO);

		assert(sizeof(objectTable[i].ROOM_CHRONO) == 4);
		out->writeUint32LE(objectTable[i].ROOM_CHRONO);

		assert(sizeof(objectTable[i].ANIM) == 2);
		out->writeSint16LE(objectTable[i].ANIM);

		assert(sizeof(objectTable[i].animType) == 2);
		out->writeSint16LE(objectTable[i].animType);

		assert(sizeof(objectTable[i].animInfo) == 2);
		out->writeSint16LE(objectTable[i].animInfo);

		assert(sizeof(objectTable[i].newAnim) == 2);
		out->writeSint16LE(objectTable[i].newAnim);

		assert(sizeof(objectTable[i].newAnimType) == 2);
		out->writeSint16LE(objectTable[i].newAnimType);

		assert(sizeof(objectTable[i].newAnimInfo) == 2);
		out->writeSint16LE(objectTable[i].newAnimInfo);

		assert(sizeof(objectTable[i].FRAME) == 2);
		out->writeSint16LE(objectTable[i].FRAME);

		assert(sizeof(objectTable[i].numOfFrames) == 2);
		out->writeSint16LE(objectTable[i].numOfFrames);

		assert(sizeof(objectTable[i].END_FRAME) == 2);
		out->writeSint16LE(objectTable[i].END_FRAME);

		assert(sizeof(objectTable[i].END_ANIM) == 2);
		out->writeSint16LE(objectTable[i].END_ANIM);

		assert(sizeof(objectTable[i].trackMode) == 2);
		out->writeSint16LE(objectTable[i].trackMode);

		assert(sizeof(objectTable[i].trackNumber) == 2);
		out->writeSint16LE(objectTable[i].trackNumber);

		assert(sizeof(objectTable[i].MARK) == 2);
		out->writeSint16LE(objectTable[i].MARK);

		assert(sizeof(objectTable[i].positionInTrack) == 2);
		out->writeSint16LE(objectTable[i].positionInTrack);

		assert(sizeof(objectTable[i].stepX) == 2);
		out->writeSint16LE(objectTable[i].stepX);

		assert(sizeof(objectTable[i].stepY) == 2);
		out->writeSint16LE(objectTable[i].stepY);

		assert(sizeof(objectTable[i].stepZ) == 2); // 45
		out->writeSint16LE(objectTable[i].stepZ);

		saveInterpolatedValue(&objectTable[i].YHandler, out);

		assert(sizeof(objectTable[i].falling) == 2);
		out->writeSint16LE(objectTable[i].falling);

		saveInterpolatedValue(&objectTable[i].rotate, out);

		assert(sizeof(objectTable[i].direction) == 2);
		out->writeSint16LE(objectTable[i].direction);

		assert(sizeof(objectTable[i].speed) == 2);
		out->writeSint16LE(objectTable[i].speed);

		saveInterpolatedValue(&objectTable[i].speedChange, out);

		assert(sizeof(objectTable[i].COL[0]) == 2);
		out->writeSint16LE(objectTable[i].COL[0]);

		assert(sizeof(objectTable[i].COL[1]) == 2);
		out->writeSint16LE(objectTable[i].COL[1]);

		assert(sizeof(objectTable[i].COL[2]) == 2);
		out->writeSint16LE(objectTable[i].COL[2]);

		assert(sizeof(objectTable[i].COL_BY) == 2);
		out->writeSint16LE(objectTable[i].COL_BY);

		assert(sizeof(objectTable[i].HARD_DEC) == 2);
		out->writeSint16LE(objectTable[i].HARD_DEC);

		assert(sizeof(objectTable[i].HARD_COL) == 2);
		out->writeSint16LE(objectTable[i].HARD_COL);

		assert(sizeof(objectTable[i].HIT) == 2);
		out->writeSint16LE(objectTable[i].HIT);

		assert(sizeof(objectTable[i].HIT_BY) == 2);
		out->writeSint16LE(objectTable[i].HIT_BY);

		assert(sizeof(objectTable[i].animActionType) == 2);
		out->writeSint16LE(objectTable[i].animActionType);

		assert(sizeof(objectTable[i].animActionANIM) == 2);
		out->writeSint16LE(objectTable[i].animActionANIM);

		assert(sizeof(objectTable[i].animActionFRAME) == 2);
		out->writeSint16LE(objectTable[i].animActionFRAME);

		assert(sizeof(objectTable[i].animActionParam) == 2);
		out->writeSint16LE(objectTable[i].animActionParam);

		assert(sizeof(objectTable[i].hitForce) == 2);
		out->writeSint16LE(objectTable[i].hitForce);

		assert(sizeof(objectTable[i].hotPointID) == 2);
		out->writeSint16LE(objectTable[i].hotPointID);

		assert(sizeof(objectTable[i].hotPoint.x) == 2);
		out->writeSint16LE(objectTable[i].hotPoint.x);

		assert(sizeof(objectTable[i].hotPoint.y) == 2);
		out->writeSint16LE(objectTable[i].hotPoint.y);

		assert(sizeof(objectTable[i].hotPoint.z) == 2);
		out->writeSint16LE(objectTable[i].hotPoint.z);
	}

	return 1;
}

static int saveJack(Common::WriteStream *out) {
	out->writeUint32BE(20);    // image offset
	out->writeUint32BE(4020);  // pal offset
	out->writeUint32BE(4788);  // desc offset
	out->writeUint32BE(4820);  // room offset
	out->writeUint32BE(21190); // vars offset

	char img[4000];
	scaleDownImage(320, 200, 0, 0, aux2, img, 80);
	out->write(img, 4000);

	out->write(currentGamePalette, 768);

	// TODO: name
	memset(img, 0, 32);
	out->write(img, 32);

	out->writeSint16LE(currentRoom);
	out->writeSint16LE(g_currentFloor);
	out->writeSint16LE(currentCamera);
	out->writeSint16LE(currentWorldTarget);
	out->writeSint16LE(currentCameraTargetActor);
	out->writeSint16LE(maxObjects);

	for (int16 i = 0; i < 300; i++) {
		out->writeSint16LE(ListWorldObjets[i].objIndex);
		out->writeSint16LE(ListWorldObjets[i].body);
		out->writeSint16LE(ListWorldObjets[i].flags);
		out->writeSint16LE(ListWorldObjets[i].typeZV);
		out->writeSint16LE(ListWorldObjets[i].foundBody);
		out->writeSint16LE(ListWorldObjets[i].foundName);
		out->writeSint16LE(ListWorldObjets[i].flags2);
		out->writeSint16LE(ListWorldObjets[i].foundLife);
		out->writeSint16LE(ListWorldObjets[i].x);
		out->writeSint16LE(ListWorldObjets[i].y);
		out->writeSint16LE(ListWorldObjets[i].z);
		out->writeSint16LE(ListWorldObjets[i].alpha);
		out->writeSint16LE(ListWorldObjets[i].beta);
		out->writeSint16LE(ListWorldObjets[i].gamma);
		out->writeSint16LE(ListWorldObjets[i].stage);
		out->writeSint16LE(ListWorldObjets[i].room);
		out->writeSint16LE(ListWorldObjets[i].lifeMode);
		out->writeSint16LE(ListWorldObjets[i].life);
		out->writeSint16LE(ListWorldObjets[i].floorLife);
		out->writeSint16LE(ListWorldObjets[i].anim);
		out->writeSint16LE(ListWorldObjets[i].frame);
		out->writeSint16LE(ListWorldObjets[i].animType);
		out->writeSint16LE(ListWorldObjets[i].animInfo);
		out->writeSint16LE(ListWorldObjets[i].trackMode);
		out->writeSint16LE(ListWorldObjets[i].trackNumber);
		out->writeSint16LE(ListWorldObjets[i].positionInTrack);
		out->writeSint16LE(ListWorldObjets[i].mark);
	}

	for (uint i = 0; i < 15; i++) {
		out->writeSint16LE(CVars[i]);
	}

	out->writeSint16LE(inHandTable[0]);
	out->writeSint16LE(-1); // ?
	out->writeSint32LE(numObjInInventoryTable[0]);
	for (uint i = 0; i < INVENTORY_SIZE; i++) {
		out->writeSint16LE(inventoryTable[0][i]);
	}

	out->writeSint16LE(statusScreenAllowed);
	out->writeSint16LE(giveUp);
	out->writeSint16LE(lightOff);
	out->writeSint16LE(shakingAmplitude);
	out->writeSint16LE(shakeVar1);
	out->writeUint32LE(timer);
	out->writeUint32LE(timerFreeze1);
	out->writeSint16LE(currentMusic);

	out->writeUint16LE(varSize);
	out->write(vars, varSize);

	for (uint i = 0; i < NUM_MAX_OBJECT; i++) {
		out->writeSint16LE(objectTable[i].indexInWorld);
		out->writeSint16LE(objectTable[i].bodyNum);
		out->writeUint16LE(objectTable[i]._flags);
		out->writeSint16LE(objectTable[i].dynFlags);
		out->writeSint16LE((int16)objectTable[i].zv.ZVX1);
		out->writeSint16LE((int16)objectTable[i].zv.ZVX2);
		out->writeSint16LE((int16)objectTable[i].zv.ZVY1);
		out->writeSint16LE((int16)objectTable[i].zv.ZVY2);
		out->writeSint16LE((int16)objectTable[i].zv.ZVZ1);
		out->writeSint16LE((int16)objectTable[i].zv.ZVZ2);
		out->writeSint16LE(objectTable[i].screenXMin);
		out->writeSint16LE(objectTable[i].screenYMin);
		out->writeSint16LE(objectTable[i].screenXMax);
		out->writeSint16LE(objectTable[i].screenYMax);
		out->writeSint16LE(objectTable[i].roomX);
		out->writeSint16LE(objectTable[i].roomY);
		out->writeSint16LE(objectTable[i].roomZ);
		out->writeSint16LE(objectTable[i].worldX);
		out->writeSint16LE(objectTable[i].worldY);
		out->writeSint16LE(objectTable[i].worldZ);
		out->writeSint16LE(objectTable[i].alpha);
		out->writeSint16LE(objectTable[i].beta);
		out->writeSint16LE(objectTable[i].gamma);
		out->writeSint16LE(objectTable[i].stage);
		out->writeSint16LE(objectTable[i].room);
		out->writeSint16LE(objectTable[i].lifeMode);
		out->writeSint16LE(objectTable[i].life);
		out->writeUint32LE(objectTable[i].CHRONO);
		out->writeUint32LE(objectTable[i].ROOM_CHRONO);
		out->writeSint16LE(objectTable[i].ANIM);
		out->writeSint16LE(objectTable[i].animType);
		out->writeSint16LE(objectTable[i].animInfo);
		out->writeSint16LE(0); // ?
		out->writeSint16LE(0); // ?
		out->writeSint16LE(objectTable[i].newAnim);
		out->writeSint16LE(objectTable[i].newAnimType);
		out->writeSint16LE(objectTable[i].newAnimInfo);
		out->writeSint16LE(objectTable[i].FRAME);
		out->writeSint16LE(objectTable[i].numOfFrames);
		out->writeSint16LE(objectTable[i].END_FRAME);
		out->writeSint16LE(objectTable[i].END_ANIM);
		out->writeSint32LE(0); // time ?
		out->writeSint16LE(objectTable[i].trackMode);
		out->writeSint16LE(objectTable[i].trackNumber);
		out->writeSint16LE(objectTable[i].MARK);
		out->writeSint16LE(objectTable[i].positionInTrack);
		out->writeSint16LE(objectTable[i].stepX);
		out->writeSint16LE(objectTable[i].stepY);
		out->writeSint16LE(objectTable[i].stepZ);
		out->writeSint16LE(objectTable[i].animNegX);
		out->writeSint16LE(objectTable[i].animNegY);
		out->writeSint16LE(objectTable[i].animNegZ);
		saveInterpolatedValue(&objectTable[i].YHandler, out);
		out->writeSint16LE(objectTable[i].falling);
		saveInterpolatedValue(&objectTable[i].rotate, out);
		out->writeSint16LE(objectTable[i].direction);
		out->writeSint16LE(objectTable[i].speed);
		saveInterpolatedValue(&objectTable[i].speedChange, out);
		out->writeSint16LE(objectTable[i].COL[0]);
		out->writeSint16LE(objectTable[i].COL[1]);
		out->writeSint16LE(objectTable[i].COL[2]);
		out->writeSint16LE(objectTable[i].COL_BY);
		out->writeSint16LE(objectTable[i].HARD_DEC);
		out->writeSint16LE(objectTable[i].HARD_COL);
		out->writeSint16LE(objectTable[i].HIT);
		out->writeSint16LE(objectTable[i].HIT_BY);
		out->writeSint16LE(objectTable[i].animActionType);
		out->writeSint16LE(objectTable[i].animActionANIM);
		out->writeSint16LE(objectTable[i].animActionFRAME);
		out->writeSint16LE(objectTable[i].animActionParam);
		out->writeSint16LE(objectTable[i].hitForce);
		out->writeSint16LE(objectTable[i].hotPointID);
		out->writeSint16LE(0);
		out->writeSint16LE(objectTable[i].hotPoint.x);
		out->writeSint16LE(objectTable[i].hotPoint.y);
		out->writeSint16LE(objectTable[i].hotPoint.z);
		out->writeSint16LE(objectTable[i].hardMat);
		out->writeSint16LE(0);
	}

	return 1;
}

int makeSaveOthers(Common::WriteStream *out) {
	const uint32 var28 = 0;
	int oldNumMaxObj = 0;

	if (g_engine->getGameId() == GID_AITD1) {
		for (uint i = 0; i < NUM_MAX_OBJECT; i++) {
			// if (objectTable[i].indexInWorld == -1) {
			// }

			if (objectTable[i].ANIM == 4) {
				CVars[getCVarsIdx(FOG_FLAG)] = 0;
				// HQ_Free_Malloc(HQ_Memory, objectTable[i].FRAME);
			}
		}
	}

	// Common::String buffer(Common::String::format("SAVE%d.ITD", entry));
	out->writeUint32LE(var28);
	out->writeUint32LE(var28);

	const uint32 currentRoomOffset = 20;
	out->writeUint32BE(currentRoomOffset);

	uint32 varsOffset = 0;
	uint32 objsOffset = 0;
	switch (g_engine->getGameId()) {
	case GID_AITD1:
		varsOffset = 19838;
		objsOffset = 20254;
		break;
	case GID_JACK:
		varsOffset = 2036;
		objsOffset = 2142;
		break;
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

	assert(sizeof(currentRoom) == 2);
	out->writeSint16LE(currentRoom);

	assert(sizeof(g_currentFloor) == 2);
	out->writeSint16LE(g_currentFloor);

	assert(sizeof(currentCamera) == 2);
	out->writeSint16LE(currentCamera);

	assert(sizeof(currentWorldTarget) == 2);
	out->writeSint16LE(currentWorldTarget);

	assert(sizeof(currentCameraTargetActor) == 2);
	out->writeSint16LE(currentCameraTargetActor);

	assert(sizeof(maxObjects) == 2);
	out->writeSint16LE(maxObjects);

	if (g_engine->getGameId() == GID_AITD1) {
		oldNumMaxObj = maxObjects;
		maxObjects = 300; // fix for save engine..
	}

	for (int16 i = 0; i < maxObjects; i++) {
		assert(sizeof(ListWorldObjets[i].objIndex) == 2);
		out->writeSint16LE(ListWorldObjets[i].objIndex);

		assert(sizeof(ListWorldObjets[i].body) == 2);
		out->writeSint16LE(ListWorldObjets[i].body);

		assert(sizeof(ListWorldObjets[i].flags) == 2);
		out->writeSint16LE(ListWorldObjets[i].flags);

		assert(sizeof(ListWorldObjets[i].typeZV) == 2);
		out->writeSint16LE(ListWorldObjets[i].typeZV);

		assert(sizeof(ListWorldObjets[i].foundBody) == 2);
		out->writeSint16LE(ListWorldObjets[i].foundBody);

		assert(sizeof(ListWorldObjets[i].foundName) == 2);
		out->writeSint16LE(ListWorldObjets[i].foundName);

		assert(sizeof(ListWorldObjets[i].flags2) == 2);
		out->writeSint16LE(ListWorldObjets[i].flags2);

		assert(sizeof(ListWorldObjets[i].foundLife) == 2);
		out->writeSint16LE(ListWorldObjets[i].foundLife);

		assert(sizeof(ListWorldObjets[i].x) == 2);
		out->writeSint16LE(ListWorldObjets[i].x);

		assert(sizeof(ListWorldObjets[i].y) == 2);
		out->writeSint16LE(ListWorldObjets[i].y);

		assert(sizeof(ListWorldObjets[i].z) == 2);
		out->writeSint16LE(ListWorldObjets[i].z);

		assert(sizeof(ListWorldObjets[i].alpha) == 2);
		out->writeSint16LE(ListWorldObjets[i].alpha);

		assert(sizeof(ListWorldObjets[i].beta) == 2);
		out->writeSint16LE(ListWorldObjets[i].beta);

		assert(sizeof(ListWorldObjets[i].gamma) == 2);
		out->writeSint16LE(ListWorldObjets[i].gamma);

		assert(sizeof(ListWorldObjets[i].stage) == 2);
		out->writeSint16LE(ListWorldObjets[i].stage);

		assert(sizeof(ListWorldObjets[i].room) == 2);
		out->writeSint16LE(ListWorldObjets[i].room);

		assert(sizeof(ListWorldObjets[i].lifeMode) == 2);
		out->writeSint16LE(ListWorldObjets[i].lifeMode);

		assert(sizeof(ListWorldObjets[i].life) == 2);
		out->writeSint16LE(ListWorldObjets[i].life);

		assert(sizeof(ListWorldObjets[i].floorLife) == 2);
		out->writeSint16LE(ListWorldObjets[i].floorLife);

		assert(sizeof(ListWorldObjets[i].anim) == 2);
		out->writeSint16LE(ListWorldObjets[i].anim);

		assert(sizeof(ListWorldObjets[i].frame) == 2);
		out->writeSint16LE(ListWorldObjets[i].frame);

		assert(sizeof(ListWorldObjets[i].animType) == 2);
		out->writeSint16LE(ListWorldObjets[i].animType);

		assert(sizeof(ListWorldObjets[i].animInfo) == 2);
		out->writeSint16LE(ListWorldObjets[i].animInfo);

		assert(sizeof(ListWorldObjets[i].trackMode) == 2);
		out->writeSint16LE(ListWorldObjets[i].trackMode);

		assert(sizeof(ListWorldObjets[i].trackNumber) == 2);
		out->writeSint16LE(ListWorldObjets[i].trackNumber);

		assert(sizeof(ListWorldObjets[i].positionInTrack) == 2);
		out->writeSint16LE(ListWorldObjets[i].positionInTrack);
	}

	if (g_engine->getGameId() == GID_AITD1) {
		maxObjects = oldNumMaxObj;
	}

	if (g_engine->getGameId() == GID_AITD1) {
		assert(CVarsSize == 45);
	}

	for (uint i = 0; i < CVarsSize; i++) {
		assert(sizeof(CVars[i]) == 2);
		out->writeSint16LE(CVars[i]);
	}

	for (int inventoryId = 0; inventoryId < NUM_MAX_INVENTORY; inventoryId++) {
		assert(sizeof(inHandTable[inventoryId]) == 2);
		out->writeSint16LE(inHandTable[inventoryId]);

		assert(sizeof(numObjInInventoryTable[inventoryId]) == 2);
		out->writeSint16LE(numObjInInventoryTable[inventoryId]);

		for (uint i = 0; i < INVENTORY_SIZE; i++) {
			assert(sizeof(inventoryTable[inventoryId][i]) == 2);
			out->writeSint16LE(inventoryTable[inventoryId][i]);
		}
	}

	assert(sizeof(statusScreenAllowed) == 2);
	out->writeSint16LE(statusScreenAllowed);

	assert(sizeof(giveUp) == 2);
	out->writeSint16LE(giveUp);

	assert(sizeof(lightOff) == 2);
	out->writeSint16LE(lightOff);

	assert(sizeof(shakingAmplitude) == 2);
	out->writeSint16LE(shakingAmplitude);

	assert(sizeof(shakeVar1) == 2);
	out->writeSint16LE(shakeVar1);

	assert(sizeof(timer) == 4);
	out->writeUint32LE(timer);

	assert(sizeof(timerFreeze1) == 4);
	out->writeUint32LE(timerFreeze1);

	assert(sizeof(currentMusic) == 2);
	out->writeSint16LE(currentMusic);

	// timerFreeze = 1;

	const uint32 pos = out->pos();
	for (uint i = 0; i < varsOffset - pos; i++) {
		out->writeByte(0);
	}

	out->writeUint16LE(varSize);
	out->write(vars, varSize);

	// pos = 20254

	for (uint i = 0; i < NUM_MAX_OBJECT; i++) {
		assert(sizeof(objectTable[i].indexInWorld) == 2);
		out->writeSint16LE(objectTable[i].indexInWorld);

		assert(sizeof(objectTable[i].bodyNum) == 2);
		out->writeSint16LE(objectTable[i].bodyNum);

		assert(sizeof(objectTable[i]._flags) == 2);
		out->writeUint16LE(objectTable[i]._flags);

		assert(sizeof(objectTable[i].dynFlags) == 2);
		out->writeSint16LE(objectTable[i].dynFlags);

		//    assert(sizeof(actorTable[i].zv.ZVX1) == 2);
		out->writeSint16LE(objectTable[i].zv.ZVX1);
		objectTable[i].zv.ZVX1 = (int16)objectTable[i].zv.ZVX1;

		//    assert(sizeof(actorTable[i].zv.ZVX2) == 2);
		out->writeSint16LE(objectTable[i].zv.ZVX2);
		objectTable[i].zv.ZVX2 = (int16)objectTable[i].zv.ZVX2;

		//    assert(sizeof(actorTable[i].zv.ZVY1) == 2);
		out->writeSint16LE(objectTable[i].zv.ZVY1);

		//    assert(sizeof(actorTable[i].zv.ZVY2) == 2);
		out->writeSint16LE(objectTable[i].zv.ZVY2);

		//    assert(sizeof(actorTable[i].zv.ZVZ1) == 2);
		out->writeSint16LE(objectTable[i].zv.ZVZ1);

		//    assert(sizeof(actorTable[i].zv.ZVZ2) == 2);
		out->writeSint16LE(objectTable[i].zv.ZVZ2);

		assert(sizeof(objectTable[i].screenXMin) == 2);
		out->writeSint16LE(objectTable[i].screenXMin);

		assert(sizeof(objectTable[i].screenYMin) == 2);
		out->writeSint16LE(objectTable[i].screenYMin);

		assert(sizeof(objectTable[i].screenXMax) == 2);
		out->writeSint16LE(objectTable[i].screenXMax);

		assert(sizeof(objectTable[i].screenYMax) == 2);
		out->writeSint16LE(objectTable[i].screenYMax);

		assert(sizeof(objectTable[i].roomX) == 2);
		out->writeSint16LE(objectTable[i].roomX);

		assert(sizeof(objectTable[i].roomY) == 2);
		out->writeSint16LE(objectTable[i].roomY);

		assert(sizeof(objectTable[i].roomZ) == 2);
		out->writeSint16LE(objectTable[i].roomZ);

		assert(sizeof(objectTable[i].worldX) == 2);
		out->writeSint16LE(objectTable[i].worldX);

		assert(sizeof(objectTable[i].worldY) == 2);
		out->writeSint16LE(objectTable[i].worldY);

		assert(sizeof(objectTable[i].worldZ) == 2);
		out->writeSint16LE(objectTable[i].worldZ);

		assert(sizeof(objectTable[i].alpha) == 2);
		out->writeSint16LE(objectTable[i].alpha);

		assert(sizeof(objectTable[i].beta) == 2);
		out->writeSint16LE(objectTable[i].beta);

		assert(sizeof(objectTable[i].gamma) == 2);
		out->writeSint16LE(objectTable[i].gamma);

		assert(sizeof(objectTable[i].stage) == 2);
		out->writeSint16LE(objectTable[i].stage);

		assert(sizeof(objectTable[i].room) == 2);
		out->writeSint16LE(objectTable[i].room);

		assert(sizeof(objectTable[i].lifeMode) == 2);
		out->writeSint16LE(objectTable[i].lifeMode);

		assert(sizeof(objectTable[i].life) == 2);
		out->writeSint16LE(objectTable[i].life);

		assert(sizeof(objectTable[i].CHRONO) == 4);
		out->writeUint32LE(objectTable[i].CHRONO);

		assert(sizeof(objectTable[i].ROOM_CHRONO) == 4);
		out->writeUint32LE(objectTable[i].ROOM_CHRONO);

		assert(sizeof(objectTable[i].ANIM) == 2);
		out->writeSint16LE(objectTable[i].ANIM);

		assert(sizeof(objectTable[i].animType) == 2);
		out->writeSint16LE(objectTable[i].animType);

		assert(sizeof(objectTable[i].animInfo) == 2);
		out->writeSint16LE(objectTable[i].animInfo);

		assert(sizeof(objectTable[i].newAnim) == 2);
		out->writeSint16LE(objectTable[i].newAnim);

		assert(sizeof(objectTable[i].newAnimType) == 2);
		out->writeSint16LE(objectTable[i].newAnimType);

		assert(sizeof(objectTable[i].newAnimInfo) == 2);
		out->writeSint16LE(objectTable[i].newAnimInfo);

		assert(sizeof(objectTable[i].FRAME) == 2);
		out->writeSint16LE(objectTable[i].FRAME);

		assert(sizeof(objectTable[i].numOfFrames) == 2);
		out->writeSint16LE(objectTable[i].numOfFrames);

		assert(sizeof(objectTable[i].END_FRAME) == 2);
		out->writeSint16LE(objectTable[i].END_FRAME);

		assert(sizeof(objectTable[i].END_ANIM) == 2);
		out->writeSint16LE(objectTable[i].END_ANIM);

		assert(sizeof(objectTable[i].trackMode) == 2);
		out->writeSint16LE(objectTable[i].trackMode);

		assert(sizeof(objectTable[i].trackNumber) == 2);
		out->writeSint16LE(objectTable[i].trackNumber);

		assert(sizeof(objectTable[i].MARK) == 2);
		out->writeSint16LE(objectTable[i].MARK);

		assert(sizeof(objectTable[i].positionInTrack) == 2);
		out->writeSint16LE(objectTable[i].positionInTrack);

		assert(sizeof(objectTable[i].stepX) == 2);
		out->writeSint16LE(objectTable[i].stepX);

		assert(sizeof(objectTable[i].stepY) == 2);
		out->writeSint16LE(objectTable[i].stepY);

		assert(sizeof(objectTable[i].stepZ) == 2); // 45
		out->writeSint16LE(objectTable[i].stepZ);

		saveInterpolatedValue(&objectTable[i].YHandler, out);

		assert(sizeof(objectTable[i].falling) == 2);
		out->writeSint16LE(objectTable[i].falling);

		saveInterpolatedValue(&objectTable[i].rotate, out);

		assert(sizeof(objectTable[i].direction) == 2);
		out->writeSint16LE(objectTable[i].direction);

		assert(sizeof(objectTable[i].speed) == 2);
		out->writeSint16LE(objectTable[i].speed);

		saveInterpolatedValue(&objectTable[i].speedChange, out);

		assert(sizeof(objectTable[i].COL[0]) == 2);
		out->writeSint16LE(objectTable[i].COL[0]);

		assert(sizeof(objectTable[i].COL[1]) == 2);
		out->writeSint16LE(objectTable[i].COL[1]);

		assert(sizeof(objectTable[i].COL[2]) == 2);
		out->writeSint16LE(objectTable[i].COL[2]);

		assert(sizeof(objectTable[i].COL_BY) == 2);
		out->writeSint16LE(objectTable[i].COL_BY);

		assert(sizeof(objectTable[i].HARD_DEC) == 2);
		out->writeSint16LE(objectTable[i].HARD_DEC);

		assert(sizeof(objectTable[i].HARD_COL) == 2);
		out->writeSint16LE(objectTable[i].HARD_COL);

		assert(sizeof(objectTable[i].HIT) == 2);
		out->writeSint16LE(objectTable[i].HIT);

		assert(sizeof(objectTable[i].HIT_BY) == 2);
		out->writeSint16LE(objectTable[i].HIT_BY);

		assert(sizeof(objectTable[i].animActionType) == 2);
		out->writeSint16LE(objectTable[i].animActionType);

		assert(sizeof(objectTable[i].animActionANIM) == 2);
		out->writeSint16LE(objectTable[i].animActionANIM);

		assert(sizeof(objectTable[i].animActionFRAME) == 2);
		out->writeSint16LE(objectTable[i].animActionFRAME);

		assert(sizeof(objectTable[i].animActionParam) == 2);
		out->writeSint16LE(objectTable[i].animActionParam);

		assert(sizeof(objectTable[i].hitForce) == 2);
		out->writeSint16LE(objectTable[i].hitForce);

		assert(sizeof(objectTable[i].hotPointID) == 2);
		out->writeSint16LE(objectTable[i].hotPointID);

		assert(sizeof(objectTable[i].hotPoint.x) == 2);
		out->writeSint16LE(objectTable[i].hotPoint.x);

		assert(sizeof(objectTable[i].hotPoint.y) == 2);
		out->writeSint16LE(objectTable[i].hotPoint.y);

		assert(sizeof(objectTable[i].hotPoint.z) == 2);
		out->writeSint16LE(objectTable[i].hotPoint.z);
	}

	return 1;
}

int saveGame(Common::WriteStream *out) {
	switch (g_engine->getGameId()) {
	case GID_AITD1:
		return saveAitd1(out);
	case GID_JACK:
		return saveJack(out);
	default:
		return makeSaveOthers(out);
	}
}
} // namespace Fitd
