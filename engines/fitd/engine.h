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

#ifndef FITD_ENGINE_H
#define FITD_ENGINE_H

#include "common/array.h"
#include "common/hashmap.h"
#include "common/scummsys.h"
#include "fitd/common.h"
#include "fitd/fitd.h"
#include "fitd/font.h"
#include "fitd/hqr.h"
#include "fitd/room.h"

#define NUM_MAX_POINT_IN_POINT_BUFFER 800
#define INVENTORY_SIZE 50

namespace Common {
template<>
struct Hash<void *> {
	uint operator()(void *s) const {
		const uint64 u = (uint64)s;
		return ((u >> 32) & 0xFFFFFFFF) ^ (u & 0xFFFFFFFF);
	}
};
} // namespace Common

namespace Fitd {

struct HardCol;
struct HqrEntry;

class Engine {
public:
	Engine() {
		logicalScreen = logicalScreenBuffer;

		listMus = hqrInitRessource("LISTMUS.PAK", 110000, 40);
		listSamp = hqrInitRessource(g_engine->getGameId() == GID_TIMEGATE ? "SAMPLES.PAK" : "LISTSAMP.PAK", 64000, 30);
		hqMemory = hqrInit(10000, 50);
	}

	~Engine() {
		hqrFree(listSamp);
		hqrFree(listMus);
		hqrFree(hqMemory);
		hqrFree(listLife);
		hqrFree(listTrack);
		hqrFree(listBody);
		hqrFree(listAnim);
		hqrFree(listMatrix);
	}

	Common::HashMap<void *, byte *> bodyBufferMap;
	Common::Array<CameraData> currentFloorCameraData;
	Common::Array<Body *> bodies;
	Common::Array<Animation *> animations;
	Common::Array<RoomData> roomDataTable;
	Common::Array<WorldObject> worldObjets;
	byte *currentFoundBody = nullptr;
	int currentFoundBodyIdx = 0;
	int statusVar1 = 0;

	HqrEntry *hqMemory = nullptr;
	HqrEntry *listMus = nullptr;
	HqrEntry *listSamp = nullptr;
	HqrEntry *listBody = nullptr;
	HqrEntry *listAnim = nullptr;
	HqrEntry *listLife = nullptr;
	HqrEntry *listTrack = nullptr;
	HqrEntry *listMatrix = nullptr;

	int videoMode = 0;
	int musicConfigured = 0;
	int musicEnabled = 0;
	int soundToggle = 0;
	int detailToggle = 0;

	byte aux[65068] = {};
	byte aux2[65068] = {};
	byte logicalScreenBuffer[64800] = {};
	byte *logicalScreen = nullptr;

	int16 bufferAnim[NB_BUFFER_ANIM][SIZE_BUFFER_ANIM] = {};

	int unkScreenVar2 = 0;

	int16 cVars[70] = {};
	uint8 cVarsSize = 0;

	byte ptrPrioritySample[247] = {};

	byte *ptrFont = nullptr;
	byte *ptrCadre = nullptr;

	uint timer = 0;
	uint timeGlobal = 0;

	int windowX1 = 0;
	int windowY1 = 0;
	int windowX2 = 0;
	int windowY2 = 0;

	ScopedPtr systemTextes;

	byte joyD = 0;
	byte click = 0;
	bool backspace = false;
	uint16 character = 0;
	bool debugFlag = false;
	char key = 0;
	char localKey = 0;
	char localJoyD = 0;
	char localClick = 0;

	const char *languageNameString = nullptr;

	RegularTextEntry textTable[40] = {};

	int turnPageFlag = 0;

	int hqrKeyGen = 0;

	byte *screenSm1 = nullptr;
	byte *screenSm2 = nullptr;
	byte *screenSm3 = nullptr;
	byte *screenSm4 = nullptr;
	byte *screenSm5 = nullptr;

	Object objectTable[NUM_MAX_OBJECT] = {};

	int16 currentWorldTarget = 0;

	int16 maxObjects = 0;

	int16 vars[482] = {};
	int varSize = 0;

	Message messageTable[5] = {};

	int16 currentMusic = 0;
	int action = 0;

	Box listBox1[50] = {}; // recheckSize
	Box listBox2[50] = {};
	Box *listPhysBox = nullptr;
	Box *listLogBox = nullptr;

	int nbPhysBoxs = 0;
	int nbLogBoxs = 0;
	int nextSample = 0;
	int nextMusic = 0;
	int16 currentCameraTargetActor = 0;
	int16 flagGameOver = 0;
	int16 lightOff = 0;
	int newFlagLight = 0;
	int lastPriority = 0;
	int lastSample = 0;
	int16 statusScreenAllowed = 0;

	byte *currentFloorRoomRawData = nullptr;
	byte *currentFloorCameraRawData = nullptr;

	int changeFloor = 0;
	int16 currentCamera = 0;
	int16 currentFloor = 0;
	int needChangeRoom = 0;

	byte *cameraPtr = nullptr;
	RoomDef *pCurrentRoomData = nullptr;
	int16 currentRoom = 0;
	int flagInitView = 0;
	int numCameraInRoom = 0;
	int numCameraZone = 0;
	byte *cameraZoneData = nullptr;
	int numRoomZone = 0;
	byte *roomZoneData = nullptr;
	byte *roomPtrCamera[NUM_MAX_CAMERA_IN_ROOM] = {};
	int startGameVar1 = 0;

	int transformX = 0;
	int transformY = 0;
	int transformZ = 0;
	int transformXCos = 0;
	int transformXSin = 0;
	int transformYCos = 0;
	int transformYSin = 0;
	int transformZCos = 0;
	int transformZSin = 0;
	bool transformUseX = false;
	bool transformUseY = false;
	bool transformUseZ = false;

	int translateX = 0;
	int translateY = 0;
	int translateZ = 0;

	int cameraCenterX = 0;
	int cameraCenterY = 0;
	int cameraPerspective = 0;
	int cameraFovX = 0;
	int cameraFovY = 0;

	byte currentCameraVisibilityList[30] = {};

	int actorTurnedToObj = 0;

	int currentProcessedActorIdx = 0;
	Object *currentProcessedActorPtr = nullptr;

	int currentLifeActorIdx = 0;
	Object *currentLifeActorPtr = nullptr;
	int currentLifeNum = 0;

	byte *currentLifePtr = nullptr;

	bool cameraBackgroundChanged = false;
	int flagRedraw = 0;

	int16 renderPointList[6400] = {};

	int numActorInList = 0;
	int sortedActorTable[NUM_MAX_OBJECT] = {};

	int angleCompX = 0;
	int angleCompZ = 0;
	int angleCompBeta = 0;

	int bufferAnimCounter = 0;

	int animCurrentTime = 0;
	int animKeyframeLength = 0;
	int animMoveX = 0;
	int animMoveY = 0;
	int animMoveZ = 0;
	int animStepZ = 0;
	int animStepX = 0;
	int animStepY = 0;
	byte *animVar1 = nullptr;
	byte *animVar3 = nullptr;
	byte *animVar4 = nullptr;

	int16 newFloor = 0;

	int fadeState = 0;

	int overlaySize1 = 0;
	int overlaySize2 = 0;

	int bgOverlayVar1 = 0;

	int16 newRoom = 0;

	int16 shakeVar1 = 0;
	int16 saveShakeVar1 = 0;
	uint timerFreeze1 = 0;
	int timerSaved = 0;

	int16 flagRotPal = 0;
	int16 saveFlagRotPal = 0;
	int16 waterHeight = 10000;

	HardCol *hardColTable[10] = {};

	int16 hardColStepX = 0;
	int16 hardColStepZ = 0;

	ZVStruct hardClip = {};

	int hqLoad = 0;
	int lightX = 4000;
	int lightY = -2000;
	int ancLumiereX = 20000;
	int ancLumiereY = 20000;

	int clipLeft = 0;
	int clipTop = 0;
	int clipRight = 319;
	int clipBottom = 199;

	ScopedPtr pAITD2InventorySprite;
	TextEntryStruct tabTextes[NUM_MAX_TEXT_ENTRY] = {};

	uint32 currentFloorRoomRawDataSize = 0;

	int fontHeight = 0;

	byte frontBuffer[320 * 200] = {};
	byte currentGamePalette[256 * 3] = {};

	CameraData *cameraDataTable[NUM_MAX_CAMERA_IN_ROOM] = {};
	CameraViewedRoom *currentCameraZoneList[NUM_MAX_CAMERA_IN_ROOM] = {};

	int numOfPoints = 0;
	int16 pointBuffer[NUM_MAX_POINT_IN_POINT_BUFFER * 3] = {};

	byte *polyBackBuffer = nullptr;

	int16 numObjInInventoryTable = {};
	int16 inHandTable = {};
	int16 inventoryTable[INVENTORY_SIZE] = {};

	int BBox3D1 = 0;
	int BBox3D2 = 0;
	int BBox3D3 = 0;
	int BBox3D4 = 0;

	int flagRefreshAux2 = 0;

	int statusLeft = 0;
	int statusTop = 0;
	int statusRight = 0;
	int statusBottom = 0;
};
} // namespace Fitd

#endif // FITD_ENGINE_H
