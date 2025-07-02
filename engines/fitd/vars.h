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

#include "common/array.h"
#include "common/scummsys.h"
#include "fitd/zv.h"

namespace Fitd {

#define NUM_MAX_OBJECT 50
#define NUM_MAX_CAMERA_IN_ROOM 20
#define NB_BUFFER_ANIM 25         // AITD1 was  20
#define SIZE_BUFFER_ANIM (8 * 41) // AITD1 was 4*31
#define LANGUAGE_NAME_SIZE 5

typedef struct Point3d {
	int16 x;
	int16 y;
	int16 z;
} Point3d;

struct TextEntryStruct;

typedef struct Message {
	TextEntryStruct *string;
	int16 time;
} Message;

typedef struct SaveEntry {
	void *ptr;
	uint size;
} SaveEntry;

typedef struct RegularTextEntry {
	char *textPtr;
	int16 width;
} RegularTextEntry;

typedef struct InterpolatedValue {
	int16 oldValue;
	int16 newValue;
	int16 param;
	uint timeOfRotate;
} InterpolatedValue;

enum ActorFlags {
	AF_ANIMATED = 0x0001,
	// 0x02
	AF_DRAWABLE = 0x0004,
	AF_BOXIFY = 0x0008,
	AF_MOVABLE = 0x0010,
	AF_SPECIAL = 0x0020,
	AF_TRIGGER = 0x0040,
	AF_FOUNDABLE = 0x0080,
	AF_FALLABLE = 0x0100,

	AF_MASK = AF_ANIMATED + AF_MOVABLE + AF_TRIGGER + AF_FOUNDABLE + AF_FALLABLE,
};

// used to read data from file too
typedef struct Object {
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
	uint CHRONO;
	uint ROOM_CHRONO;
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

	InterpolatedValue YHandler;
	int16 falling;
	InterpolatedValue rotate;
	int16 direction;
	int16 speed;
	InterpolatedValue speedChange;
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
	Point3d hotPoint;

	// aitd2
	int16 hardMat;
	int16 field_A8;
} Object;

typedef struct WorldObject {
	int16 objIndex;
	int16 body;
	union {
		int16 flags;
		ActorFlags bitField;
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
} WorldObject;

typedef struct Box {
	int16 var0;
	int16 var1;
	int16 var2;
	int16 var3;
} Box;

typedef struct RoomDef {
	int16 offsetToCameraDef; // 0
	int16 offsetToPosDef;    // 2
	int16 worldX;            // 4
	int16 worldY;            // 6
	int16 worldZ;            // 8
	int16 numCameraInRoom;   // 0xA
} RoomDef;

struct GroupState {
	int16 m_type;           // 8
	int16 m_delta[3];       // A
	int16 m_rotateDelta[3]; // 10 (AITD2+) if Info_optimise
};

struct Group {
	int16 m_start;        // 0
	int16 m_numVertices;  // 2
	int16 m_baseVertices; // 4
	int8 m_orgGroup;      // 6
	int8 m_numGroup;      // 7
	GroupState m_state;
};

enum PrimType {
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

struct Primitive {
	PrimType m_type;
	uint8 m_material;
	uint8 m_color;
	uint8 m_even;
	uint16 m_size;
	Common::Array<uint16> m_points;
};

struct Body {
	void *m_raw;

	uint16 m_flags;
	ZVStruct m_zv;
	Common::Array<uint8> m_scratchBuffer;
	Common::Array<Point3d> m_vertices;
	Common::Array<uint16> m_groupOrder;
	Common::Array<Group> m_groups;
	Common::Array<Primitive> m_primitives;

	void sync();
};

struct Frame {
	uint16 m_timestamp;
	int16 m_animStep[3];
	Common::Array<GroupState> m_groups;
};

struct Animation {
	void *m_raw;

	uint16 m_numFrames;
	uint16 m_numGroups;
	Common::Array<Frame> m_frames;
};

extern byte *currentFoundBody;
extern int currentFoundBodyIdx;
extern int statusVar1;

struct HqrEntry;

extern HqrEntry *HQ_Memory;
extern HqrEntry *listMus;
extern HqrEntry *listSamp;

extern int videoMode;
extern int musicConfigured;
extern int musicEnabled;
extern int soundToggle;
extern int detailToggle;

extern char *aux;
extern char *aux2;

extern int16 BufferAnim[NB_BUFFER_ANIM][SIZE_BUFFER_ANIM];

extern char *logicalScreen;

extern int unkScreenVar2;

extern int16 CVars[70];
extern uint8 CVarsSize;

extern char *PtrPrioritySample;

extern char *PtrFont;

extern char *PtrCadre;

extern uint timer;
extern uint timeGlobal;

extern int WindowX1;
extern int WindowY1;
extern int WindowX2;
extern int WindowY2;

extern char *systemTextes;

extern char JoyD;
extern char Click;
extern bool Backspace;
extern uint16 Character;
extern bool Debug;
extern char key;
extern char localKey;
extern char localJoyD;
extern char localClick;

extern const char *languageNameString;
extern const char *languageNameTable[];
extern const char *languageShortNameTable[];

extern RegularTextEntry textTable[40];

extern int turnPageFlag;

extern int hqrKeyGen;

extern char *screenSm1;
extern char *screenSm2;
extern char *screenSm3;
extern char *screenSm4;
extern char *screenSm5;

extern Object objectTable[NUM_MAX_OBJECT];

extern int16 currentWorldTarget;

extern int fileSize;

extern HqrEntry *listBody;
extern HqrEntry *listAnim;
extern HqrEntry *listLife;
extern HqrEntry *listTrack;
extern HqrEntry *listMatrix;

extern int16 maxObjects;

extern int16 *vars;

extern int varSize;

extern Message messageTable[5];

extern int16 currentMusic;
extern int action;

extern Box listBox1[50]; // recheckSize
extern Box listBox2[50];
extern Box *listPhysBox;
extern Box *listLogBox;

extern int nbPhysBoxs;
extern int nbLogBoxs;
extern int nextSample;
extern int nextMusic;
extern int16 currentCameraTargetActor;
extern int16 giveUp;
extern int16 lightOff;
extern int newFlagLight;
extern int LastPriority;
extern int LastSample;
extern int16 statusScreenAllowed;

extern char *g_currentFloorRoomRawData;
extern char *g_currentFloorCameraRawData;

extern int changeFloor;
extern int16 currentCamera;
extern int16 g_currentFloor;
extern int needChangeRoom;

extern char *cameraPtr;
extern RoomDef *pCurrentRoomData;
extern int16 currentRoom;
extern int flagInitView;
extern int numCameraInRoom;
extern int numCameraZone;
extern char *cameraZoneData;
extern int numRoomZone;
extern char *roomZoneData;
extern char *roomPtrCamera[NUM_MAX_CAMERA_IN_ROOM];
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
extern Object *currentProcessedActorPtr;

extern int currentLifeActorIdx;
extern Object *currentLifeActorPtr;
extern int currentLifeNum;

extern byte *currentLifePtr;

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
extern byte *animVar1;
extern byte *animVar3;
extern byte *animVar4;

extern int16 newFloor;

extern int fadeState;

extern char cameraBuffer[256];
extern char cameraBuffer2[256];
extern char cameraBuffer3[400];
extern char cameraBuffer4[400];

extern char *cameraBufferPtr;
extern char *cameraBuffer2Ptr;
extern char *cameraBuffer3Ptr;

extern int overlaySize1;
extern int overlaySize2;

extern int bgOverlayVar1;

extern int16 newRoom;

extern const char *listBodySelect[];
extern const char *listAnimSelect[];

extern int16 shakeVar1;
extern int16 saveShakeVar1;
extern uint timerFreeze1;
extern int timerSaved;

extern int16 flagRotPal;
extern int16 saveFlagRotPal;
extern int16 waterHeight;

struct HardCol;
extern HardCol *hardColTable[10];

extern int16 hardColStepX;
extern int16 hardColStepZ;

extern ZVStruct hardClip;

extern SaveEntry saveTable[];

extern int HQ_Load;
extern int lightX;
extern int lightY;
extern int ancLumiereX;
extern int ancLumiereY;

extern int clipLeft;
extern int clipTop;
extern int clipRight;
extern int clipBottom;

extern byte *g_MaskPtr;

} // namespace Fitd

#endif
