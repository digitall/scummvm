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
#include "fitd/vars.h"

namespace Fitd {

int musicEnabled;

int16 newRoom;

int16 newFloor;

int animMoveX;
int animMoveY;
int animMoveZ;

int animStepX;
int animStepZ;
int animStepY;

char *animVar1;
char *animVar3;
char *animVar4;

int animCurrentTime;
int animKeyframeLength;

bool cameraBackgroundChanged = false;
int flagRedraw;

char cameraBuffer[256];
char cameraBuffer2[256];
char cameraBuffer3[400];
char cameraBuffer4[400];

char *cameraBufferPtr = cameraBuffer;
char *cameraBuffer2Ptr = cameraBuffer2;
char *cameraBuffer3Ptr = cameraBuffer3;

int numActorInList;
int sortedActorTable[NUM_MAX_OBJECT];

char currentCameraVisibilityList[30];

int actorTurnedToObj = 0;

int currentProcessedActorIdx;
tObject *currentProcessedActorPtr;

int currentLifeActorIdx;
tObject *currentLifeActorPtr;
int currentLifeNum;

char *currentLifePtr;

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

float renderPointList[6400];

unsigned int timer;
unsigned int timeGlobal;

hqrEntryStruct *listBody;
hqrEntryStruct *listAnim;
hqrEntryStruct *listLife;
hqrEntryStruct *listTrack;
hqrEntryStruct *listMatrix;

int HQ_Load = 0;
int lightX = 4000;
int lightY = -2000;

hqrEntryStruct *listMus;
hqrEntryStruct *listSamp;

hqrEntryStruct *HQ_Memory;

char *aux;
char *aux2;

int hqrKeyGen = 0;

char JoyD = 0;
char Click = 0;
char key = 0;
char localKey;
char localJoyD;
char localClick;

Common::Array<int16> CVars;

int turnPageFlag;

char *logicalScreen;

int fadeState;

char *PtrPrioritySample;
char *PtrFont;
char *PtrCadre;

int LastPriority;
int LastSample;

int clipLeft = 0;
int clipTop = 0;
int clipRight = 319;
int clipBottom = 119;

const char *languageNameString = NULL;
const char *languageNameTable[LANGUAGE_NAME_SIZE] =
	{
		"FRANCAIS.PAK",
		"ITALIANO.PAK",
		"ENGLISH.PAK",
		"ESPAGNOL.PAK",
		"DEUTSCH.PAK",
};
char *systemTextes;
regularTextEntryStruct textTable[NUM_MAX_TEXT];
textEntryStruct *tabTextes;

char *currentFoundBody;
int currentFoundBodyIdx;
int statusVar1;

messageStruct messageTable[NUM_MAX_MESSAGE];

int16 currentMusic;
int action;

boxStruct genVar2[15]; // recheckSize
boxStruct genVar4[50];
boxStruct *genVar1;
boxStruct *genVar3;

char *cameraPtr;
roomDefStruct *pCurrentRoomData;

char *g_currentFloorRoomRawData = NULL;
char *g_currentFloorCameraRawData = NULL;

int changeFloor;
int16 currentCamera;
int16 g_currentFloor;
int needChangeRoom;

int16 currentRoom;
int flagInitView;
int numCameraInRoom;
int numCameraZone;
char *cameraZoneData;
int numRoomZone;
char *roomZoneData;
char *room_PtrCamera[NUM_MAX_CAMERA_IN_ROOM];
int startGameVar1;

int WindowX1;
int WindowY1;
int WindowX2;
int WindowY2;

char *screenSm1;
char *screenSm2;
char *screenSm3;
char *screenSm4;
char *screenSm5;

tObject objectTable[NUM_MAX_OBJECT];
int16 currentWorldTarget;

int16 maxObjects;

const char *listBodySelect[] = {
	"LISTBODY.PAK",
	"LISTBOD2.PAK",
};

const char *listAnimSelect[] = {
	"LISTANIM.PAK",
	"LISTANI2.PAK",
};

Common::Array<tWorldObject> ListWorldObjets;

int16 *vars;
int varSize;
int fileSize;

int genVar5;
int genVar6;
int nextSample;
int nextMusic;
int16 currentCameraTargetActor;
int16 giveUp;
int16 lightOff;
int lightVar2;
int16 statusScreenAllowed;

void sBody::sync() {
	uint8 *ptr = (uint8 *)m_raw;

	ptr += 2;                        // skip the flag
	ptr += 12;                       // skip the ZV
	ptr += READ_LE_S16(ptr) + 2;     // skip scratch buffer
	ptr += READ_LE_S16(ptr) * 6 + 2; // skip vertices
	uint16 numGroups = READ_LE_U16(ptr);
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
