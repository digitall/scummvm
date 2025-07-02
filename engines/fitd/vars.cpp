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

#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/vars.h"

namespace Fitd {

byte *currentFoundBody;
int currentFoundBodyIdx;
int statusVar1;

HqrEntry *hqMemory;

int videoMode;
int musicConfigured;
int musicEnabled;
int soundToggle;
int detailToggle;

byte *aux;
byte *aux2;
int16 bufferAnim[NB_BUFFER_ANIM][SIZE_BUFFER_ANIM];

byte *logicalScreen;

int screenBufferSize;
int unkScreenVar2;

int16 cVars[70];
uint8 cVarsSize = 0;

byte *ptrPrioritySample;

byte *ptrFont;

byte *ptrCadre;

uint timer;
uint timeGlobal;

int windowX1;
int windowY1;
int windowX2;
int windowY2;

char *systemTextes;

byte joyD = 0;
byte click = 0;
uint16 character = 0;
bool backspace = false;
bool debugFlag = false;
char key = 0;
char localKey;
char localJoyD;
char localClick;

const char *languageNameTable[LANGUAGE_NAME_SIZE] = {
	"FRANCAIS.PAK",
	"ITALIANO.PAK",
	"ENGLISH.PAK",
	"ESPAGNOL.PAK",
	"DEUTSCH.PAK",
};

const char *languageShortNameTable[LANGUAGE_NAME_SIZE] = {
	"fr",
	"it",
	"en",
	"es",
	"de",
};

const char *languageNameString = nullptr;

#define NUM_MAX_TEXT 40

RegularTextEntry textTable[NUM_MAX_TEXT];

int turnPageFlag;

int hqrKeyGen = 0;

byte *screenSm1;
byte *screenSm2;
byte *screenSm3;
byte *screenSm4;
byte *screenSm5;

Object objectTable[NUM_MAX_OBJECT];

int16 currentWorldTarget;

int fileSize;

HqrEntry *listBody = nullptr;
HqrEntry *listAnim = nullptr;
HqrEntry *listLife = nullptr;
HqrEntry *listTrack = nullptr;
HqrEntry *listMatrix = nullptr;

int16 maxObjects;

int16 *vars;

int varSize;

Message messageTable[NUM_MAX_MESSAGE];

int16 currentMusic;
int action;

Box listBox1[50]; // recheckSize
Box listBox2[50];
Box *listPhysBox;
Box *listLogBox;

int nbPhysBoxs;
int nbLogBoxs;
int nextSample;
int nextMusic;
int16 currentCameraTargetActor;
int16 giveUp;
int16 lightOff;
int newFlagLight;
int lastPriority;
int lastSample;
int16 statusScreenAllowed;

byte *currentFloorRoomRawData = nullptr;
byte *currentFloorCameraRawData = nullptr;

int changeFloor;
int16 currentCamera;
int16 currentFloor;
int needChangeRoom;

byte *cameraPtr;
RoomDef *pCurrentRoomData;

int16 currentRoom;
int flagInitView;
int numCameraInRoom;
int numCameraZone;
byte *cameraZoneData;
int numRoomZone;
byte *roomZoneData;
byte *roomPtrCamera[NUM_MAX_CAMERA_IN_ROOM];
int startGameVar1;

int transformX;
int transformY;
int transformZ;
int transformXCos;
int transformXSin;
int transformYCos;
int transformYSin;
int transformZCos;
int transformZSin;
bool transformUseX;
bool transformUseY;
bool transformUseZ;

int translateX;
int translateY;
int translateZ;

int cameraCenterX;
int cameraCenterY;
int cameraPerspective;
int cameraFovX;
int cameraFovY;

byte currentCameraVisibilityList[30];

int actorTurnedToObj = 0;

int currentProcessedActorIdx;
Object *currentProcessedActorPtr;

int currentLifeActorIdx;
Object *currentLifeActorPtr;
int currentLifeNum;

byte *currentLifePtr;

bool cameraBackgroundChanged = false;
int flagRedraw;

int16 renderPointList[6400];

int numActorInList;
int sortedActorTable[NUM_MAX_OBJECT];

int angleCompX;
int angleCompZ;
int angleCompBeta;

int bufferAnimCounter = 0;

int animCurrentTime;
int animKeyframeLength;

int animMoveX;
int animMoveY;
int animMoveZ;

int animStepX;
int animStepZ;
int animStepY;

byte *animVar1;
byte *animVar3;
byte *animVar4;

int16 newFloor;

int fadeState;

byte cameraBuffer[256];
byte cameraBuffer2[256];
byte cameraBuffer3[400];
byte cameraBuffer4[400];

byte *cameraBufferPtr = cameraBuffer;
byte *cameraBuffer2Ptr = cameraBuffer2;
byte *cameraBuffer3Ptr = cameraBuffer3;

int overlaySize1;
int overlaySize2;

int bgOverlayVar1;

int16 newRoom;

int16 shakeVar1;
int16 saveShakeVar1;
uint timerFreeze1;
int timerSaved = 0;

int16 flagRotPal;
int16 saveFlagRotPal;
int16 waterHeight = 10000;

HardCol *hardColTable[10];

int16 hardColStepX;
int16 hardColStepZ;

ZVStruct hardClip;
const char *listBodySelect[] = {
	"LISTBODY.PAK",
	"LISTBOD2.PAK",
};

const char *listAnimSelect[] = {
	"LISTANIM.PAK",
	"LISTANI2.PAK",
};

SaveEntry saveTable[40];

int hqLoad = 0;
int lightX = 4000;
int lightY = -2000;
int ancLumiereX =  20000;
int ancLumiereY =  20000;

int clipLeft = 0;
int clipTop = 0;
int clipRight = 319;
int clipBottom = 119;

byte *maskPtr = nullptr;

HqrEntry *listMus;
HqrEntry *listSamp;

void Body::sync() {
	const uint8 *ptr = (uint8 *)m_raw;

	ptr += 2;                        // skip the flag
	ptr += 12;                       // skip the ZV
	ptr += READ_LE_S16(ptr) + 2;     // skip scratch buffer
	ptr += READ_LE_S16(ptr) * 6 + 2; // skip vertices
	const uint16 numGroups = READ_LE_U16(ptr);
	ptr += numGroups * 2 + 2; // skip group order

	assert(numGroups == m_groups.size());

	for (int i = 0; i < numGroups; i++) {
		m_groups[i].m_state.m_type = READ_LE_S16(ptr + 8);
		m_groups[i].m_state.m_delta[0] = READ_LE_S16(ptr + 10);
		m_groups[i].m_state.m_delta[1] = READ_LE_S16(ptr + 12);
		m_groups[i].m_state.m_delta[2] = READ_LE_S16(ptr + 14);
		ptr += 16;
		if (m_flags & INFO_OPTIMISE) {
			m_groups[i].m_state.m_rotateDelta[0] = READ_LE_S16(ptr + 0);
			m_groups[i].m_state.m_rotateDelta[1] = READ_LE_S16(ptr + 2);
			m_groups[i].m_state.m_rotateDelta[2] = READ_LE_S16(ptr + 4);
			ptr += 8;
		}
	}
}

} // namespace Fitd
