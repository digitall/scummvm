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
#include "fitd/file_access.h"
#include "fitd/floor.h"
#include "fitd/hqr.h"
#include "fitd/pak.h"

namespace Fitd {

uint32 g_currentFloorRoomRawDataSize = 0;
uint32 g_currentFloorCameraRawDataSize;
uint32 g_currentFloorNumCamera = 0;
Common::Array<cameraDataStruct> g_currentFloorCameraData;

void loadFloor(int floorNumber) {
	int expectedNumberOfRoom;
	int expectedNumberOfCamera;

	if (g_currentFloorCameraRawData) {
		free(g_currentFloorCameraRawData);
		free(g_currentFloorRoomRawData);
	}

	// stopSounds();

	HQR_Reset(listBody);
	HQR_Reset(listAnim);

	g_currentFloor = floorNumber;

	Common::String floorFileName = Common::String::format("ETAGE%02d.pak", floorNumber);

	g_currentFloorRoomRawDataSize = getPakSize(floorFileName.c_str(), 0);
	g_currentFloorCameraRawDataSize = getPakSize(floorFileName.c_str(), 1);

	g_currentFloorRoomRawData = checkLoadMallocPak(floorFileName.c_str(), 0);
	g_currentFloorCameraRawData = checkLoadMallocPak(floorFileName.c_str(), 1);

	currentCamera = -1;
	needChangeRoom = 1;
	changeFloor = 0;

	//////////////////////////////////

	if (roomDataTable) {
		free(roomDataTable);
		roomDataTable = NULL;
	}

	expectedNumberOfRoom = getNumberOfRoom();

	for (int i = 0; i < expectedNumberOfRoom; i++) {
		uint32 j;
		uint8 *roomData;
		uint8 *hardColData;
		uint8 *sceZoneData;
		roomDataStruct *currentRoomDataPtr;

		if (roomDataTable) {
			roomDataTable = (roomDataStruct *)realloc(roomDataTable, sizeof(roomDataStruct) * (i + 1));
		} else {
			roomDataTable = (roomDataStruct *)malloc(sizeof(roomDataStruct));
		}

		roomData = (uint8 *)(g_currentFloorRoomRawData + READ_LE_U32(g_currentFloorRoomRawData + i * 4));
		currentRoomDataPtr = &roomDataTable[i];

		currentRoomDataPtr->worldX = READ_LE_S16(roomData + 4);
		currentRoomDataPtr->worldY = READ_LE_S16(roomData + 6);
		currentRoomDataPtr->worldZ = READ_LE_S16(roomData + 8);

		currentRoomDataPtr->numCameraInRoom = READ_LE_U16(roomData + 0xA);

		currentRoomDataPtr->cameraIdxTable = (uint16 *)malloc(currentRoomDataPtr->numCameraInRoom * sizeof(int16));

		for (j = 0; j < currentRoomDataPtr->numCameraInRoom; j++) {
			currentRoomDataPtr->cameraIdxTable[j] = READ_LE_U16(roomData + 0xC + 2 * j);
		}

		// hard col read

		hardColData = roomData + READ_LE_U16(roomData);
		currentRoomDataPtr->numHardCol = READ_LE_U16(hardColData);
		hardColData += 2;

		if (currentRoomDataPtr->numHardCol) {
			currentRoomDataPtr->hardColTable = (hardColStruct *)malloc(sizeof(hardColStruct) * currentRoomDataPtr->numHardCol);

			for (j = 0; j < currentRoomDataPtr->numHardCol; j++) {
				ZVStruct *zvData;

				zvData = &currentRoomDataPtr->hardColTable[j].zv;

				zvData->ZVX1 = READ_LE_S16(hardColData + 0x00);
				zvData->ZVX2 = READ_LE_S16(hardColData + 0x02);
				zvData->ZVY1 = READ_LE_S16(hardColData + 0x04);
				zvData->ZVY2 = READ_LE_S16(hardColData + 0x06);
				zvData->ZVZ1 = READ_LE_S16(hardColData + 0x08);
				zvData->ZVZ2 = READ_LE_S16(hardColData + 0x0A);

				currentRoomDataPtr->hardColTable[j].parameter = READ_LE_U16(hardColData + 0x0C);
				currentRoomDataPtr->hardColTable[j].type = READ_LE_U16(hardColData + 0x0E);

				hardColData += 0x10;
			}
		} else {
			currentRoomDataPtr->hardColTable = NULL;
		}

		// sce zone read

		sceZoneData = roomData + READ_LE_U16(roomData + 2);
		currentRoomDataPtr->numSceZone = READ_LE_U16(sceZoneData);
		sceZoneData += 2;

		if (currentRoomDataPtr->numSceZone) {
			currentRoomDataPtr->sceZoneTable = (sceZoneStruct *)malloc(sizeof(sceZoneStruct) * currentRoomDataPtr->numSceZone);

			for (j = 0; j < currentRoomDataPtr->numSceZone; j++) {
				ZVStruct *zvData;

				zvData = &currentRoomDataPtr->sceZoneTable[j].zv;

				zvData->ZVX1 = READ_LE_S16(sceZoneData + 0x00);
				zvData->ZVX2 = READ_LE_S16(sceZoneData + 0x02);
				zvData->ZVY1 = READ_LE_S16(sceZoneData + 0x04);
				zvData->ZVY2 = READ_LE_S16(sceZoneData + 0x06);
				zvData->ZVZ1 = READ_LE_S16(sceZoneData + 0x08);
				zvData->ZVZ2 = READ_LE_S16(sceZoneData + 0x0A);

				currentRoomDataPtr->sceZoneTable[j].parameter = READ_LE_U16(sceZoneData + 0x0C);
				currentRoomDataPtr->sceZoneTable[j].type = READ_LE_U16(sceZoneData + 0x0E);

				sceZoneData += 0x10;
			}
		} else {
			currentRoomDataPtr->sceZoneTable = NULL;
		}
	}
	///////////////////////////////////

	/////////////////////////////////////////////////
	// camera stuff

	int maxExpectedNumberOfCamera = ((READ_LE_U32(g_currentFloorCameraRawData)) / 4);

	expectedNumberOfCamera = 0;

	int minOffset = 0;

	for (int i = 0; i < maxExpectedNumberOfCamera; i++) {
		int offset = READ_LE_U32(g_currentFloorCameraRawData + i * 4);
		if (offset > minOffset) {
			minOffset = offset;
			expectedNumberOfCamera++;
		} else {
			break;
		}
	}

	g_currentFloorCameraData.clear();
	g_currentFloorCameraData.resize(expectedNumberOfCamera);

	int i;
	for (i = 0; i < expectedNumberOfCamera; i++) {
		int k;
		unsigned int offset;
		unsigned char *currentCameraData;

		offset = READ_LE_U32(g_currentFloorCameraRawData + i * 4);

		// load cameras
		if (offset < g_currentFloorCameraRawDataSize) {
			unsigned char *backupDataPtr;

			currentCameraData = (unsigned char *)(g_currentFloorCameraRawData + READ_LE_U32(g_currentFloorCameraRawData + i * 4));

			backupDataPtr = currentCameraData;

			g_currentFloorCameraData[i].alpha = READ_LE_U16(currentCameraData + 0x00);
			g_currentFloorCameraData[i].beta = READ_LE_U16(currentCameraData + 0x02);
			g_currentFloorCameraData[i].gamma = READ_LE_U16(currentCameraData + 0x04);

			g_currentFloorCameraData[i].x = READ_LE_U16(currentCameraData + 0x06);
			g_currentFloorCameraData[i].y = READ_LE_U16(currentCameraData + 0x08);
			g_currentFloorCameraData[i].z = READ_LE_U16(currentCameraData + 0x0A);

			g_currentFloorCameraData[i].focal1 = READ_LE_U16(currentCameraData + 0x0C);
			g_currentFloorCameraData[i].focal2 = READ_LE_U16(currentCameraData + 0x0E);
			g_currentFloorCameraData[i].focal3 = READ_LE_U16(currentCameraData + 0x10);

			g_currentFloorCameraData[i].numViewedRooms = READ_LE_U16(currentCameraData + 0x12);

			currentCameraData += 0x14;

			g_currentFloorCameraData[i].viewedRoomTable = (cameraViewedRoomStruct *)malloc(sizeof(cameraViewedRoomStruct) * g_currentFloorCameraData[i].numViewedRooms);
			assert(g_currentFloorCameraData[i].viewedRoomTable);
			memset(g_currentFloorCameraData[i].viewedRoomTable, 0, sizeof(cameraViewedRoomStruct) * g_currentFloorCameraData[i].numViewedRooms);

			for (k = 0; k < g_currentFloorCameraData[i].numViewedRooms; k++) {
				cameraViewedRoomStruct *pCurrentCameraViewedRoom;

				pCurrentCameraViewedRoom = &g_currentFloorCameraData[i].viewedRoomTable[k];

				pCurrentCameraViewedRoom->viewedRoomIdx = READ_LE_U16(currentCameraData + 0x00);
				pCurrentCameraViewedRoom->offsetToMask = READ_LE_U16(currentCameraData + 0x02);
				pCurrentCameraViewedRoom->offsetToCover = READ_LE_U16(currentCameraData + 0x04);

				pCurrentCameraViewedRoom->offsetToHybrids = 0;
				pCurrentCameraViewedRoom->offsetCamOptims = 0;
				pCurrentCameraViewedRoom->lightX = READ_LE_U16(currentCameraData + 0x06);
				pCurrentCameraViewedRoom->lightY = READ_LE_U16(currentCameraData + 0x08);
				pCurrentCameraViewedRoom->lightZ = READ_LE_U16(currentCameraData + 0x0A);

				// load camera mask
				unsigned char *pMaskData = NULL;
				// load camera cover
				{
					unsigned char *pZoneData;
					int numZones;
					int j;

					pZoneData = backupDataPtr + g_currentFloorCameraData[i].viewedRoomTable[k].offsetToCover;
					if (pMaskData) {
						assert(pZoneData == pMaskData);
					}
					// pZoneData = currentCameraData;

					pCurrentCameraViewedRoom->numCoverZones = numZones = READ_LE_U16(pZoneData);
					pZoneData += 2;

					pCurrentCameraViewedRoom->coverZones = (cameraZoneEntryStruct *)malloc(sizeof(cameraZoneEntryStruct) * numZones);

					assert(pCurrentCameraViewedRoom->coverZones);

					for (j = 0; j < pCurrentCameraViewedRoom->numCoverZones; j++) {
						int pointIdx;
						int numPoints;

						pCurrentCameraViewedRoom->coverZones[j].numPoints = numPoints = READ_LE_U16(pZoneData);
						pZoneData += 2;

						pCurrentCameraViewedRoom->coverZones[j].pointTable = (cameraZonePointStruct *)malloc(sizeof(cameraZonePointStruct) * (numPoints + 1));

						for (pointIdx = 0; pointIdx < pCurrentCameraViewedRoom->coverZones[j].numPoints; pointIdx++) {
							pCurrentCameraViewedRoom->coverZones[j].pointTable[pointIdx].x = READ_LE_U16(pZoneData);
							pZoneData += 2;
							pCurrentCameraViewedRoom->coverZones[j].pointTable[pointIdx].y = READ_LE_U16(pZoneData);
							pZoneData += 2;
						}

						pCurrentCameraViewedRoom->coverZones[j].pointTable[numPoints].x = pCurrentCameraViewedRoom->coverZones[j].pointTable[0].x; // copy first point to last position
						pCurrentCameraViewedRoom->coverZones[j].pointTable[numPoints].y = pCurrentCameraViewedRoom->coverZones[j].pointTable[0].y;
					}
				}

				currentCameraData += 0x0C;
			}
		} else {
			break;
		}
	}

	g_currentFloorNumCamera = i - 1;

	// globalCameraDataTable = (cameraDataStruct*)realloc(globalCameraDataTable,sizeof(cameraDataStruct)*numGlobalCamera);

	/*    roomCameraData+=0x14;

	}*/
}
} // namespace Fitd
