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

#ifndef _FITD_ROOM_H_
#define _FITD_ROOM_H_

#include "common/array.h"
#include "common/scummsys.h"
#include "fitd/vars.h"

namespace Fitd {

typedef struct HardCol {
	ZVStruct zv;
	uint32 type;
	uint32 parameter;
} HardCol;

typedef struct SceZone {
	ZVStruct zv;
	uint32 type;
	uint32 parameter;
} SceZone;

typedef struct CameraZonePoint {
	int16 x;
	int16 y;
} CameraZonePoint;

typedef struct CameraZoneEntry {
	uint16 numPoints;

	Common::Array<CameraZonePoint> pointTable;
} CameraZoneEntry;

struct RectTest {
	int16 zoneX1;
	int16 zoneZ1;
	int16 zoneX2;
	int16 zoneZ2;
};

struct CameraMask {
	uint16 numTestRect;
	Common::Array<RectTest> rectTests;
};

struct CameraViewedRoom {
	int16 viewedRoomIdx;
	int16 offsetToMask;
	int16 offsetToCover;
	int16 offsetToHybrids;
	int16 offsetCamOptims;
	int16 lightX;
	int16 lightY;
	int16 lightZ;

	uint16 numMask;
	Common::Array<CameraMask> masks;
	uint16 numCoverZones;
	Common::Array<CameraZoneEntry> coverZones;
};

typedef struct CameraData {
	int16 alpha;
	int16 beta;
	int16 gamma;

	int16 x;
	int16 y;
	int16 z;

	int16 focal1;
	int16 focal2;
	int16 focal3;

	uint16 numViewedRooms;
	Common::Array<CameraViewedRoom> viewedRoomTable;
} CameraData;

typedef struct RoomData {
	uint32 numCameraInRoom;

	uint32 numHardCol;
	Common::Array<HardCol> hardColTable;

	uint32 numSceZone;
	Common::Array<SceZone> sceZoneTable;

	int32 worldX;
	int32 worldY;
	int32 worldZ;

	Common::Array<uint16> cameraIdxTable;
} RoomData;

extern CameraData *cameraDataTable[NUM_MAX_CAMERA_IN_ROOM];
extern CameraViewedRoom *currentCameraZoneList[NUM_MAX_CAMERA_IN_ROOM];

RoomDef *getRoomData(int roomNumber);
void loadRoom(int roomNumber);
int getNumberOfRoom();
} // namespace Fitd

#endif
