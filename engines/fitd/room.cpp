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

#include "common/file.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/floor.h"
#include "fitd/game_time.h"
#include "fitd/pak.h"
#include "fitd/room.h"

namespace Fitd {
/*
Room data:

u16 offsetToCameraCoverZones
u16 offsetToHardCol
s16 roomPositionX
s16 roomPositionY
s16 roomPositionZ

etageVar1 -> table for camera data

*/

CameraData *cameraDataTable[NUM_MAX_CAMERA_IN_ROOM];
CameraViewedRoom *currentCameraZoneList[NUM_MAX_CAMERA_IN_ROOM];

RoomDef *getRoomData(int roomNumber) {
	return (RoomDef *)(currentFloorRoomRawData + READ_LE_U32(currentFloorRoomRawData + roomNumber * 4));
}

int getNumberOfRoom() {
	int i;
	int j = 0;

	if (currentFloorRoomRawData) {
		int numMax = READ_LE_U32(currentFloorRoomRawData) / 4;

		for (i = 0; i < numMax; i++) {
			if (g_currentFloorRoomRawDataSize >= READ_LE_U32(currentFloorRoomRawData + i * 4)) {
				j++;
			} else {
				return j;
			}
		}
		return j;
	}
	if (Common::File::exists(Common::String::format("ETAGE%02d.PAK", currentFloor).c_str())) {
		return pakGetNumFiles(Common::String::format("ETAGE%02d", currentFloor).c_str());
	}
	if (Common::File::exists(Common::String::format("SAL%02d.PAK", currentFloor).c_str())) {
		return pakGetNumFiles(Common::String::format("SAL%02d", currentFloor).c_str());
	}
	assert(0);

	return 0;
}

void loadRoom(int roomNumber) {
	int i;
	int cameraVar0 = 0;
	int cameraVar1 = 0;
	int cameraVar2 = 0;
	int oldCameraIdx;

	freezeTime();

	assert(roomNumber >= 0);

	if (currentCamera == -1) {
		oldCameraIdx = -1;
	} else {
		cameraVar0 = g_engine->_engine->roomDataTable[currentRoom].worldX;
		cameraVar1 = g_engine->_engine->roomDataTable[currentRoom].worldY;
		cameraVar2 = g_engine->_engine->roomDataTable[currentRoom].worldZ;

		oldCameraIdx = g_engine->_engine->roomDataTable[currentRoom].cameraIdxTable[currentCamera];
	}

	if (g_engine->getGameId() < GID_AITD3) {
		cameraPtr = (byte *)getRoomData(roomNumber); // TODO: obsolete
		pCurrentRoomData = getRoomData(roomNumber);
	}

	currentRoom = roomNumber;

	numCameraInRoom = g_engine->_engine->roomDataTable[roomNumber].numCameraInRoom;

	assert(numCameraInRoom < NUM_MAX_CAMERA_IN_ROOM);

	/*
	var_20 = cameraPtr + roomDataPtr->offsetToPosDef;
	numCameraZone = *(s16*)var_20;
	var_20 += 2;
	cameraZoneData = var_20;

	var_20 = cameraPtr + roomDataPtr->offsetToCameraDef;
	numRoomZone = *(s16*)var_20;
	var_20 += 2;
	roomZoneData = var_20;*/

	assert(numCameraInRoom < NUM_MAX_CAMERA_IN_ROOM);

	int newNumCamera = 0;

	// load the new camera table and try to keep the same camera (except if changing floor)
	for (i = 0; i < numCameraInRoom; i++) {
		uint currentCameraIdx = g_engine->_engine->roomDataTable[currentRoom].cameraIdxTable[i]; // indexes are between the roomDefStruct and the first zone data

		assert(currentCameraIdx <= 40);

		if ((uint)oldCameraIdx == currentCameraIdx) {
			newNumCamera = i;
			// newAbsCamera = currentCameraIdx;
		}

		if (g_engine->getGameId() < GID_AITD3) {
			roomPtrCamera[i] = currentFloorCameraRawData + READ_LE_U32(currentFloorCameraRawData + currentCameraIdx * 4);
		}

		cameraDataTable[i] = &g_engine->_engine->currentFloorCameraData[currentCameraIdx];

		currentCameraIdx = cameraDataTable[i]->numViewedRooms;

		// scan for the zone data related to the current room
		uint j;
		for (j = 0; j < currentCameraIdx; j++) {
			if (cameraDataTable[i]->viewedRoomTable[j].viewedRoomIdx == currentRoom)
				break;
		}

		assert(cameraDataTable[i]->viewedRoomTable[j].viewedRoomIdx == currentRoom);
		if (cameraDataTable[i]->viewedRoomTable[j].viewedRoomIdx == currentRoom)
			currentCameraZoneList[i] = &cameraDataTable[i]->viewedRoomTable[j];
	}

	// reajust world coordinates
	if (oldCameraIdx != -1) // if a camera was selected before loading room
	{
		int var_E = (g_engine->_engine->roomDataTable[roomNumber].worldX - cameraVar0) * 10;
		int var_C = (g_engine->_engine->roomDataTable[roomNumber].worldY - cameraVar1) * 10;
		int var_A = (g_engine->_engine->roomDataTable[roomNumber].worldZ - cameraVar2) * 10;

		for (i = 0; i < NUM_MAX_OBJECT; i++) {
			if (objectTable[i].indexInWorld != -1) {
				/*       if(gameId == AITD1) // special case. In AITD1, the load room function was always triggered just after the actor was moved in the new room.
				// it is not always the case in later games. Maybe we could generalize the AITD2 way...
				{
				actorTable[i].worldX -= var_E;
				actorTable[i].worldY += var_C;
				actorTable[i].worldZ += var_A;
				}
				else*/
				{
					if (i != currentCameraTargetActor) {
						objectTable[i].worldX -= var_E;
						objectTable[i].worldY += var_C;
						objectTable[i].worldZ += var_A;
					} else {
						objectTable[i].worldX = objectTable[i].roomX;
						objectTable[i].worldY = objectTable[i].roomY;
						objectTable[i].worldZ = objectTable[i].roomZ;
					}
				}
			}
		}
	}

	startGameVar1 = newNumCamera;
	flagInitView = 1;
	needChangeRoom = 0;
	unfreezeTime();
}

} // namespace Fitd
