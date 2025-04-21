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

char *currentFoundBody;
int currentFoundBodyIdx;
int statusVar1;

hqrEntryStruct *HQ_Memory;

int videoMode;
int musicConfigured;
int musicEnabled;
int soundToggle;
int detailToggle;

char *aux;
char *aux2;
int16 BufferAnim[NB_BUFFER_ANIM][SIZE_BUFFER_ANIM];

char *logicalScreen;

int screenBufferSize;
int unkScreenVar2;

int16 CVars[70];
uint8 CVarsSize = 0;

char *PtrPrioritySample;

char *PtrFont;

char *PtrCadre;

unsigned char currentGamePalette[256 * 3];

// OSystem osystem;

char rgbaBuffer[320 * 200 * 4];

uint timer;
uint timeGlobal;

int WindowX1;
int WindowY1;
int WindowX2;
int WindowY2;

char *systemTextes;

char JoyD = 0;
char Click = 0;
bool Debug = false;
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

regularTextEntryStruct textTable[NUM_MAX_TEXT];

int turnPageFlag;

int hqrKeyGen = 0;

char *screenSm1;
char *screenSm2;
char *screenSm3;
char *screenSm4;
char *screenSm5;

tObject objectTable[NUM_MAX_OBJECT];

int16 currentWorldTarget;

int fileSize;

hqrEntryStruct *listBody = nullptr;
hqrEntryStruct *listAnim = nullptr;
hqrEntryStruct *listLife = nullptr;
hqrEntryStruct *listTrack = nullptr;
hqrEntryStruct *listMatrix = nullptr;

int16 maxObjects;

Common::Array<tWorldObject> ListWorldObjets;

int16 *vars;

int varSize;

messageStruct messageTable[NUM_MAX_MESSAGE];

int16 currentMusic;
int action;

boxStruct genVar2[15]; // recheckSize
boxStruct genVar4[50];
boxStruct *genVar1;
boxStruct *genVar3;

int genVar5;
int genVar6;
int nextSample;
int nextMusic;
int16 currentCameraTargetActor;
int16 giveUp;
int16 lightOff;
int lightVar2;
int LastPriority;
int LastSample;
int16 statusScreenAllowed;

char *g_currentFloorRoomRawData = nullptr;
char *g_currentFloorCameraRawData = nullptr;

int changeFloor;
int16 currentCamera;
int16 g_currentFloor;
int needChangeRoom;

char *cameraPtr;
roomDefStruct *pCurrentRoomData;

int16 currentRoom;
int flagInitView;
int numCameraInRoom;
int numCameraZone;
char *cameraZoneData;
int numRoomZone;
char *roomZoneData;
char *room_PtrCamera[NUM_MAX_CAMERA_IN_ROOM];
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

char currentCameraVisibilityList[30];

int actorTurnedToObj = 0;

int currentProcessedActorIdx;
tObject *currentProcessedActorPtr;

int currentLifeActorIdx;
tObject *currentLifeActorPtr;
int currentLifeNum;

char *currentLifePtr;

int16 readNextArgument(const char *name) {
	const int16 value = *(int16 *)currentLifePtr;
	currentLifePtr += 2;

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

char *animVar1;
char *animVar3;
char *animVar4;

int16 newFloor;

int fadeState;

char cameraBuffer[256];
char cameraBuffer2[256];
char cameraBuffer3[400];
char cameraBuffer4[400];

char *cameraBufferPtr = cameraBuffer;
char *cameraBuffer2Ptr = cameraBuffer2;
char *cameraBuffer3Ptr = cameraBuffer3;

int overlaySize1;
int overlaySize2;

int bgOverlayVar1;

int16 newRoom;

int16 shakeVar1;
int16 shakingAmplitude;
unsigned int timerFreeze1;

hardColStruct *hardColTable[10];

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

saveEntry saveTable[40];

int HQ_Load = 0;
int lightX = 4000;
int lightY = -2000;

int clipLeft = 0;
int clipTop = 0;
int clipRight = 319;
int clipBottom = 119;

unsigned char *g_MaskPtr = nullptr;

hqrEntryStruct *listMus;
hqrEntryStruct *listSamp;

void sBody::sync() {
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
