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
	uint8 *m_raw;
};

Body getBodyFromPtr(void *ptr);
uint16 getFlags(const Body *body);
uint16 getVerticesSize(const Body *body);
void copyVertices(const Body *body, int16 *dst);
void getVertices(const Body *body, Common::Array<Point3d> &dst);
uint16 getGroupOrderSize(const Body *body);
void getGroupOrders(const Body *body, Common::Array<uint16> &dst);
void getGroups(const Body *body, Common::Array<Group> &dst);
void getPrimitives(const Body *body, Common::Array<Primitive> &dst);

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

} // namespace Fitd

#endif
