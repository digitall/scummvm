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

struct hardColStruct;
typedef struct hardColStruct hardColStruct;

struct hardColStruct {
	ZVStruct zv;
	uint32 type;
	uint32 parameter;
};

struct sceZoneStruct {
	ZVStruct zv;
	uint32 type;
	uint32 parameter;
};

typedef struct sceZoneStruct sceZoneStruct;

struct cameraZonePointStruct {
	int16 x;
	int16 y;
};

typedef struct cameraZonePointStruct cameraZonePointStruct;

struct cameraZoneEntryStruct {
	uint16 numPoints;

	cameraZonePointStruct *pointTable;
};

typedef struct cameraZoneEntryStruct cameraZoneEntryStruct;

struct rectTestStruct {
	int16 zoneX1;
	int16 zoneZ1;
	int16 zoneX2;
	int16 zoneZ2;
};

struct cameraMaskStruct {
	uint16 numTestRect;
	rectTestStruct *rectTests;
};

struct cameraViewedRoomStruct {
	int16 viewedRoomIdx;
	int16 offsetToMask;
	int16 offsetToCover;
	int16 offsetToHybrids;
	int16 offsetCamOptims;
	int16 lightX;
	int16 lightY;
	int16 lightZ;

	uint16 numMask;
	cameraMaskStruct *masks;
	uint16 numCoverZones;
	cameraZoneEntryStruct *coverZones;
};

struct cameraDataStruct {
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
	cameraViewedRoomStruct *viewedRoomTable;
};

struct roomDataStruct {
	uint32 numCameraInRoom;

	uint32 numHardCol;
	hardColStruct *hardColTable;

	uint32 numSceZone;
	sceZoneStruct *sceZoneTable;

	int32 worldX;
	int32 worldY;
	int32 worldZ;

	uint16 *cameraIdxTable;
};
typedef struct roomDataStruct roomDataStruct;

extern cameraDataStruct *cameraDataTable[NUM_MAX_CAMERA_IN_ROOM];
extern cameraViewedRoomStruct *currentCameraZoneList[NUM_MAX_CAMERA_IN_ROOM];
extern roomDataStruct *roomDataTable;

roomDefStruct *getRoomData(int roomNumber);
void loadRoom(int roomNumber);
int getNumberOfRoom();
} // namespace Fitd

#endif
