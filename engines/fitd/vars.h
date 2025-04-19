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

#ifndef FITD_VARS_H
#define FITD_VARS_H

#include "common/scummsys.h"
#include "common/array.h"
#include "common/str.h"

namespace Fitd {

#define NUM_MAX_OBJECT       50
#define NUM_MAX_CAMERA_IN_ROOM 20

extern char* currentFoundBody;
extern int currentFoundBodyIdx;
extern int statusVar1;

struct point3dStruct
{
    int16 x;
    int16 y;
    int16 z;
};

typedef struct point3dStruct point3dStruct;

struct textEntryStruct
{
    int16 index;
    char* textPtr;
    int16 width;
};

typedef struct textEntryStruct textEntryStruct;

struct messageStruct
{
    textEntryStruct* string;
    int16 time;
};

typedef struct messageStruct messageStruct;

struct saveEntry
{
    void* ptr;
    unsigned int size;
};

typedef struct saveEntry saveEntry;

struct regularTextEntryStruct
{
    char* textPtr;
    int16 width;
};

typedef struct regularTextEntryStruct regularTextEntryStruct;

struct hqrSubEntryStruct
{
    int16 key;
    int16 size;
    unsigned int lastTimeUsed;
    char* ptr;
};

typedef struct hqrSubEntryStruct hqrSubEntryStruct;

struct hqrEntryStruct
{
    Common::String string;
    uint16 maxFreeData;
    uint16 sizeFreeData;
    uint16 numMaxEntry;
    uint16 numUsedEntry;
    hqrSubEntryStruct* entries;
};

typedef struct hqrEntryStruct hqrEntryStruct;

struct ZVStruct
{
    int32 ZVX1;
    int32 ZVX2;
    int32 ZVY1;
    int32 ZVY2;
    int32 ZVZ1;
    int32 ZVZ2;
};

typedef struct ZVStruct ZVStruct;

struct interpolatedValue
{
    int16 oldAngle;
    int16 newAngle;
    int16 param;
    unsigned int timeOfRotate;
};

typedef struct interpolatedValue interpolatedValue;

enum actorFlags
{
    AF_ANIMATED     = 0x0001,
    // 0x02
    AF_DRAWABLE     = 0x0004,
    AF_BOXIFY       = 0x0008,
    AF_MOVABLE      = 0x0010,
    AF_SPECIAL      = 0x0020,
    AF_TRIGGER      = 0x0040,
    AF_FOUNDABLE    = 0x0080,
    AF_FALLABLE     = 0x0100,

    AF_MASK         = AF_ANIMATED + AF_MOVABLE + AF_TRIGGER + AF_FOUNDABLE + AF_FALLABLE,
};

struct tObject // used to read data from file too
{
    int16 indexInWorld;
    int16 bodyNum;
    uint16 _flags;
    int16 dynFlags;
    ZVStruct zv;
    int16 screenXMin;
    int16 screenYMin;
    int16 screenXMax;
    int16 screenYMax;
    int16 roomX;
    int16 roomY;
    int16 roomZ;
    int16 worldX;
    int16 worldY;
    int16 worldZ;
    int16 alpha;
    int16 beta;
    int16 gamma;
    int16 stage;
    int16 room;
    int16 lifeMode;
    int16 life;
    unsigned int CHRONO;
    unsigned int ROOM_CHRONO;
    int16 ANIM;
    int16 animType;
    int16 animInfo;
    int16 newAnim;
    int16 newAnimType;
    int16 newAnimInfo;
    int16 FRAME;
    int16 numOfFrames;
    int16 END_FRAME;
    int16 END_ANIM;
    int16 trackMode;
    int16 trackNumber;
    int16 MARK;
    int16 positionInTrack;

    int16 stepX;
    int16 stepY;
    int16 stepZ;

	int16 animNegX;
	int16 animNegY;
	int16 animNegZ;

    interpolatedValue YHandler;
    int16 falling;
    interpolatedValue rotate;
    int16 direction;
    int16 speed;
    interpolatedValue speedChange;
    int16 COL[3];
    int16 COL_BY;
    int16 HARD_DEC;
    int16 HARD_COL;
    int16 HIT;
    int16 HIT_BY;
    int16 animActionType;
    int16 animActionANIM;
    int16 animActionFRAME;
    int16 animActionParam;
    int16 hitForce;
    int16 hotPointID;
    point3dStruct hotPoint;

    // aitd2
    int16 hardMat;
};

typedef struct tObject tObject;

struct tWorldObject
{
    int16 objIndex;
    int16 body;
    union
    {
        int16 flags;
        actorFlags bitField;
    };
    int16 typeZV;
    int16 foundBody;
    int16 foundName;
    int16 flags2;
    int16 foundLife;
    int16 x;
    int16 y;
    int16 z;
    int16 alpha;
    int16 beta;
    int16 gamma;
    int16 stage;
    int16 room;
    int16 lifeMode;
    int16 life;
    int16 floorLife;
    int16 anim;
    int16 frame;
    int16 animType;
    int16 animInfo;
    int16 trackMode;
    int16 trackNumber;
    int16 positionInTrack;

    // AITD2
    int16 mark;
};

typedef struct tWorldObject tWorldObject;

struct boxStruct
{
    int16 var0;
    int16 var1;
    int16 var2;
    int16 var3;
};

typedef struct boxStruct boxStruct;

struct roomDefStruct
{
    int16 offsetToCameraDef; // 0
    int16 offsetToPosDef; // 2
    int16 worldX;//4
    int16 worldY;//6
    int16 worldZ;//8
    int16 numCameraInRoom;//0xA
};

typedef struct roomDefStruct roomDefStruct;

extern hqrEntryStruct* HQ_Memory;

extern hqrEntryStruct* listMus;
extern hqrEntryStruct* listSamp;

extern int videoMode;
extern int musicConfigured;
extern int musicEnabled;
extern int soundToggle;
extern int detailToggle;

extern char* aux;
extern char* aux2;

#define NB_BUFFER_ANIM 25 // AITD1 was  20
#define SIZE_BUFFER_ANIM (8*41) // AITD1 was 4*31

extern int16 BufferAnim[NB_BUFFER_ANIM][SIZE_BUFFER_ANIM];

extern char* logicalScreen;

// extern int screenBufferSize;
extern int unkScreenVar2;

extern int16 CVars[70];
extern uint8 CVarsSize;

extern char* PtrPrioritySample;

extern char* PtrFont;

extern char* PtrCadre;

extern unsigned char currentGamePalette[0x300];

//extern OSystem osystem;
extern unsigned char frontBuffer[320*200];
extern char rgbaBuffer[320*200*4];

extern unsigned int timer;
extern unsigned int timeGlobal;

extern int WindowX1;
extern int WindowY1;
extern int WindowX2;
extern int WindowY2;

extern textEntryStruct* tabTextes;
extern char* systemTextes;

extern "C" {
    extern char JoyD;
};
extern char Click;
extern bool Debug;
extern char key;
extern char localKey;
extern char localJoyD;
extern char localClick;

#define LANGUAGE_NAME_SIZE 5

extern const char* languageNameString;
extern const char* languageNameTable[];
extern const char* languageShortNameTable[];

extern regularTextEntryStruct textTable[40];

extern int turnPageFlag;

extern int hqrKeyGen;

extern char* screenSm1;
extern char* screenSm2;
extern char* screenSm3;
extern char* screenSm4;
extern char* screenSm5;

extern tObject objectTable[NUM_MAX_OBJECT];

extern int16 currentWorldTarget;

extern int fileSize;

extern hqrEntryStruct* listBody;
extern hqrEntryStruct* listAnim;
extern hqrEntryStruct* listLife;
extern hqrEntryStruct* listTrack;
extern hqrEntryStruct* listMatrix;

extern int16 maxObjects;

extern Common::Array<tWorldObject> ListWorldObjets; // may be less

extern int16* vars;

extern int varSize;

extern messageStruct messageTable[5];

extern int16 currentMusic;
extern int action;

extern boxStruct genVar2[15]; // recheckSize
extern boxStruct genVar4[50];
extern boxStruct *genVar1;
extern boxStruct *genVar3;

extern int genVar5;
extern int genVar6;
extern int nextSample;
extern int nextMusic;
extern int16 currentCameraTargetActor;
extern int16 giveUp;
extern int16 lightOff;
extern int lightVar2;
extern int LastPriority;
extern int LastSample;
extern int16 statusScreenAllowed;

extern char* g_currentFloorRoomRawData;
extern char* g_currentFloorCameraRawData;

extern int changeFloor;
extern int16 currentCamera;
extern int16 g_currentFloor;
extern int needChangeRoom;

extern char* cameraPtr;
extern roomDefStruct* pCurrentRoomData;
extern int16 currentRoom;
extern int flagInitView;
extern int numCameraInRoom;
extern int numCameraZone;
extern char* cameraZoneData;
extern int numRoomZone;
extern char* roomZoneData;
extern char* room_PtrCamera[NUM_MAX_CAMERA_IN_ROOM];
extern int startGameVar1;

extern int transformX;
extern int transformY;
extern int transformZ;
extern int transformXCos;
extern int transformXSin;
extern int transformYCos;
extern int transformYSin;
extern int transformZCos;
extern int transformZSin;
extern bool transformUseX;
extern bool transformUseY;
extern bool transformUseZ;

extern int translateX;
extern int translateY;
extern int translateZ;

extern int cameraCenterX;
extern int cameraCenterY;
extern int cameraPerspective;
extern int cameraFovX;
extern int cameraFovY;

extern char currentCameraVisibilityList[30];

extern int actorTurnedToObj;

extern int currentProcessedActorIdx;
extern tObject* currentProcessedActorPtr;

extern int currentLifeActorIdx;
extern tObject* currentLifeActorPtr;
extern int currentLifeNum;

extern char* currentLifePtr;

int16 readNextArgument(const char* name = NULL);

extern bool cameraBackgroundChanged;
extern int flagRedraw;

extern int16 renderPointList[6400];

extern int numActorInList;
extern int sortedActorTable[NUM_MAX_OBJECT];

extern int angleCompX;
extern int angleCompZ;
extern int angleCompBeta;

extern int bufferAnimCounter;

extern int animCurrentTime;
extern int animKeyframeLength;
extern int animMoveX;
extern int animMoveY;
extern int animMoveZ;
extern int animStepZ;
extern int animStepX;
extern int animStepY;
extern char* animVar1;
extern char* animVar3;
extern char* animVar4;

extern int16 newFloor;

extern int fadeState;

extern char cameraBuffer[256];
extern char cameraBuffer2[256];
extern char cameraBuffer3[400];
extern char cameraBuffer4[400];

extern char* cameraBufferPtr;
extern char* cameraBuffer2Ptr;
extern char* cameraBuffer3Ptr;

extern int overlaySize1;
extern int overlaySize2;

extern int bgOverlayVar1;

extern int16 newRoom;

extern const char* listBodySelect[];
extern const char* listAnimSelect[];

extern int16 shakeVar1;
extern int16 shakingAmplitude;
extern unsigned int timerFreeze1;

struct hardColStruct;
extern hardColStruct *hardColTable[10];

extern int16 hardColStepX;
extern int16 hardColStepZ;

extern ZVStruct hardClip;

extern saveEntry saveTable[];

extern int HQ_Load;
extern int lightX;
extern int lightY;

extern int clipLeft;
extern int clipTop;
extern int clipRight;
extern int clipBottom;

extern unsigned char* g_MaskPtr;

struct sGroupState
{
    int16 m_type; // 8
    int16 m_delta[3]; // A
    int16 m_rotateDelta[3]; // 10 (AITD2+) if Info_optimise
};

struct sGroup
{
    int16 m_start; // 0
    int16 m_numVertices; // 2
    int16 m_baseVertices; // 4
    int8 m_orgGroup; // 6
    int8 m_numGroup; // 7
    sGroupState m_state;
};

enum primTypeEnum
{
    primTypeEnum_Line = 0,
    primTypeEnum_Poly = 1,
    primTypeEnum_Point = 2,
    primTypeEnum_Sphere = 3,
    primTypeEnum_Disk = 4,
    primTypeEnum_Cylinder = 5,
    primTypeEnum_BigPoint = 6,
    primTypeEnum_Zixel = 7,
    processPrim_PolyTexture8 = 8,
    processPrim_PolyTexture9 = 9,
    processPrim_PolyTexture10 = 10,
};

struct sPrimitive
{
    primTypeEnum m_type;
    uint8 m_material;
    uint8 m_color;
    uint8 m_even;
    uint16 m_size;
    Common::Array<uint16> m_points;
};

struct sExtraBody
{
    uint16 m_startOfKeyframe; // 2
};

// scratch buffer:
// 4: uint16 timer


struct sBody
{
    void* m_raw;

    uint16 m_flags;
    ZVStruct m_zv;
    Common::Array<uint8> m_scratchBuffer;
    Common::Array<point3dStruct> m_vertices;
    Common::Array<uint16> m_groupOrder;
    Common::Array<sGroup> m_groups;
    Common::Array<sPrimitive> m_primitives;

    void sync();
};

struct sFrame
{
    uint16 m_timestamp;
    int16 m_animStep[3];
    Common::Array<sGroupState> m_groups;
};

struct sAnimation
{
    void* m_raw;

    uint16 m_numFrames;
    uint16 m_numGroups;
    Common::Array<sFrame> m_frames;
};

}

#endif
