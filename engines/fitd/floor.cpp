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
#include "fitd/fitd.h"
#include "fitd/file_access.h"
#include "fitd/floor.h"
#include "fitd/hqr.h"
#include "fitd/pak.h"

namespace Fitd {

uint32 g_currentFloorRoomRawDataSize = 0;
uint32 g_currentFloorCameraRawDataSize;
uint32 g_currentFloorNumCamera = 0;
cameraDataStruct g_currentFloorCameraData[40];

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

	if (g_engine->getGameId() < GID_AITD3) {
		Common::String floorFileName = Common::String::format("ETAGE%02d.pak", floorNumber);

		g_currentFloorRoomRawDataSize = getPakSize(floorFileName.c_str(), 0);
		g_currentFloorCameraRawDataSize = getPakSize(floorFileName.c_str(), 1);

		g_currentFloorRoomRawData = checkLoadMallocPak(floorFileName.c_str(), 0);
		g_currentFloorCameraRawData = checkLoadMallocPak(floorFileName.c_str(), 1);
	}

	currentCamera = -1;
	needChangeRoom = 1;
	changeFloor = 0;

	//////////////////////////////////

	if (roomDataTable) {
		free(roomDataTable);
		roomDataTable = nullptr;
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

		if (g_engine->getGameId() >= GID_AITD3) {
			Common::String buffer;
			if (g_engine->getGameId() > GID_AITD3) {
				buffer = Common::String::format("SAL%02d.PAK", floorNumber);
			} else {
				buffer = Common::String::format("ETAGE%02d.PAK", floorNumber);
			}

			roomData = (uint8 *)checkLoadMallocPak(buffer.c_str(), i);
		} else {
			roomData = (uint8 *)(g_currentFloorRoomRawData + READ_LE_U32(g_currentFloorRoomRawData + i * 4));
		}
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
			currentRoomDataPtr->hardColTable = nullptr;
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
			currentRoomDataPtr->sceZoneTable = nullptr;
		}
	}
	///////////////////////////////////

	/////////////////////////////////////////////////
	// camera stuff

	if (g_engine->getGameId() >= GID_AITD3) {
		Common::String buffer;

		if (g_engine->getGameId() == GID_AITD3) {
			buffer = Common::String::format("CAMERA%02d.PAK", floorNumber);
		} else {
			buffer = Common::String::format("CAMSAL%02d.PAK", floorNumber);
		}

		expectedNumberOfCamera = PAK_getNumFiles(buffer.c_str());
	} else {
		int maxExpectedNumberOfCamera = READ_LE_U32(g_currentFloorCameraRawData) / 4;

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
	}

	assert(expectedNumberOfCamera < 40);

	int i;
	for (i = 0; i < expectedNumberOfCamera; i++) {
		unsigned int offset;
		unsigned char *currentCameraData = nullptr;

		if (g_engine->getGameId() >= GID_AITD3) {
			Common::String buffer;

			if (g_engine->getGameId() == GID_AITD3) {
				buffer = Common::String::format("CAMERA%02d.PAK", floorNumber);
			} else {
				buffer = Common::String::format("CAMSAL%02d.PAK", floorNumber);
			}

			offset = 0;
			g_currentFloorCameraRawDataSize = 1;
			currentCameraData = (unsigned char *)checkLoadMallocPak(buffer.c_str(), i);
		} else {
			offset = READ_LE_U32(g_currentFloorCameraRawData + i * 4);
		}

		// load cameras
		if (offset < g_currentFloorCameraRawDataSize) {
			int k;
			unsigned char *backupDataPtr;

			if (g_engine->getGameId() < GID_AITD3) {
				currentCameraData = (unsigned char *)(g_currentFloorCameraRawData + READ_LE_U32(g_currentFloorCameraRawData + i * 4));
			}

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

				if (g_engine->getGameId() == GID_AITD1) {
					pCurrentCameraViewedRoom->offsetToHybrids = 0;
					pCurrentCameraViewedRoom->offsetCamOptims = 0;
					pCurrentCameraViewedRoom->lightX = READ_LE_U16(currentCameraData + 0x06);
					pCurrentCameraViewedRoom->lightY = READ_LE_U16(currentCameraData + 0x08);
					pCurrentCameraViewedRoom->lightZ = READ_LE_U16(currentCameraData + 0x0A);
				} else {
					pCurrentCameraViewedRoom->offsetToHybrids = READ_LE_U16(currentCameraData + 0x06);
					pCurrentCameraViewedRoom->offsetCamOptims = READ_LE_U16(currentCameraData + 0x08);
					pCurrentCameraViewedRoom->lightX = READ_LE_U16(currentCameraData + 0x0A);
					pCurrentCameraViewedRoom->lightY = READ_LE_U16(currentCameraData + 0x0C);
					pCurrentCameraViewedRoom->lightZ = READ_LE_U16(currentCameraData + 0x0E);
				}

				// load camera mask
				unsigned char *pMaskData = nullptr;
				if (g_engine->getGameId() >= GID_JACK) {
					pMaskData = backupDataPtr + g_currentFloorCameraData[i].viewedRoomTable[k].offsetToMask;

					// for this camera, how many masks zone
					pCurrentCameraViewedRoom->numMask = READ_LE_U16(pMaskData);
					pMaskData += 2;

					pCurrentCameraViewedRoom->masks = (cameraMaskStruct *)malloc(sizeof(cameraMaskStruct) * pCurrentCameraViewedRoom->numMask);
					memset(pCurrentCameraViewedRoom->masks, 0, sizeof(cameraMaskStruct) * pCurrentCameraViewedRoom->numMask);

					for (int l = 0; l < pCurrentCameraViewedRoom->numMask; l++) {
						cameraMaskStruct *pCurrentCameraMask = &pCurrentCameraViewedRoom->masks[l];

						// for this overlay zone, how many
						pCurrentCameraMask->numTestRect = READ_LE_U16(pMaskData);
						pMaskData += 2;

						pCurrentCameraMask->rectTests = (rectTestStruct *)malloc(sizeof(rectTestStruct) * pCurrentCameraMask->numTestRect);
						memset(pCurrentCameraMask->rectTests, 0, sizeof(rectTestStruct) * pCurrentCameraMask->numTestRect);

						for (int j = 0; j < pCurrentCameraMask->numTestRect; j++) {
							rectTestStruct *pCurrentRectTest = &pCurrentCameraMask->rectTests[j];

							pCurrentRectTest->zoneX1 = READ_LE_S16(pMaskData);
							pCurrentRectTest->zoneZ1 = READ_LE_S16(pMaskData + 2);
							pCurrentRectTest->zoneX2 = READ_LE_S16(pMaskData + 4);
							pCurrentRectTest->zoneZ2 = READ_LE_S16(pMaskData + 6);
							pMaskData += 8;
						}
					}
				}
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

				if (g_engine->getGameId() == GID_AITD1) {
					currentCameraData += 0x0C;
				} else {
					currentCameraData += 0x10;
				}
				if (g_engine->getGameId() == GID_TIMEGATE) {
					currentCameraData += 6;
				}
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
