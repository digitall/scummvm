/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "cruise/cruise.h"
#include "cruise/cruise_main.h"
#include "common/endian.h"
#include "common/util.h"
#include "common/rect.h"

namespace Cruise {

int computeDistance(int varX, int varY, int paramX, int paramY) {
	int diffX = ABS(paramX - varX);
	int diffY = ABS(paramY - varY);

	if (diffX > diffY) {
		diffY = diffX;
	}

	return (diffY);
}

// this function process path finding coordinates
void computeAllDistance(int16 table[][10], short int coordCount) {
	for (int i = 0; i < coordCount; i++) {
		int x1 = ctp_routeCoords[i][0];
		int y1 = ctp_routeCoords[i][1];

		for (int j = 0; j < ctp_routes[i][0]; j++) {
			int p = ctp_routes[i][j+1];

			int x2 = ctp_routeCoords[p][0];
			int y2 = ctp_routeCoords[p][1];

			table[i][p] = computeDistance(x1, y1, x2, y2);
		}
	}
}

Common::Point getWalkBoxCenter(WalkBox &walkBox) {
	int minX = 1000;
	int minY = 1000;
	int maxX = -1;
	int maxY = -1;
	Common::Point currentWalkBoxcenter;

	for (int i = 0; i < walkBox._array[0]; i++) {
		int x = walkBox._array[i*2+1];
		int y = walkBox._array[i*2+2];

		if (x < minX)
			minX = x;

		if (x > maxX)
			maxX = x;

		if (y < minY)
			minY = y;

		if (y > maxY)
			maxY = y;
	}

	currentWalkBoxcenter.x = ((maxX - minX) / 2) + minX;
	currentWalkBoxcenter.y = ((maxY - minY) / 2) + minY;
	return currentWalkBoxcenter;
}

// ax dx bx
void renderCTPWalkBox(int16 *walkboxData, int hotPointX, int hotPointY, int X, int Y, int scale) {
	int numPoints;
	int i;
	int16 *destination;
	uint8 walkboxTable[0x12];

	int startX = X - ((upscaleValue(hotPointX, scale) + 0x8000) >> 16);
	int startY = Y - ((upscaleValue(hotPointY, scale) + 0x8000) >> 16);

	numPoints = *(walkboxData++);

	destination = polyBuffer2;

	for (i = 0; i < numPoints; i++) {
		int pointX = *(walkboxData++);
		int pointY = *(walkboxData++);

		int scaledX = ((upscaleValue(pointX, scale) + 0x8000) >> 16) + startX;
		int scaledY = ((upscaleValue(pointY, scale) + 0x8000) >> 16) + startY;

		*(destination++) = scaledX;
		*(destination++) = scaledY;
	}

	m_color = 0;

	for (i = 0; i < numPoints; i++) {
		walkboxTable[i] = i;
	}

	drawPolyMode2((unsigned char *)walkboxTable, numPoints);
}

// this process the walkboxes
void makeCtStruct(Common::Array<Ct> &lst, WalkBox *pWalkBox, int num, int z) {
	int minX = 1000;
	int maxX = -1;

	if (pWalkBox->_array[0] < 1)
		return;

	Common::Point currentWalkBoxCenter = getWalkBoxCenter(*pWalkBox);

	renderCTPWalkBox(&pWalkBox->_array[0], currentWalkBoxCenter.x, currentWalkBoxCenter.y,  currentWalkBoxCenter.x, currentWalkBoxCenter.y, z + 0x200);

/*	lst.push_back(Ct());
	Ct &ct = lst[lst.size() - 1];*/

	Ct *ct = new Ct;
	int16* XArray = XMIN_XMAX;
	int minY = *XArray++;

	int i = 0;

	while (*XArray >= 0) {
		int x1 = *XArray++;
		int x2 = *XArray++;

		if (x1 < minX)
			minX = x1;

		if (x2 > maxX)
			maxX = x2;

		ct->_slices.push_back(CtEntry(x1, x2));
		i++;
	}

	ct->_num = num;
	ct->_color = pWalkBox->_color;
	ct->_bounds.left = minX;
	ct->_bounds.right = maxX;
	ct->_bounds.top = minY;
	ct->_bounds.bottom = minY + i;

	lst.push_back(*ct);
}

int getNode(int nodeIdx, int nodeResult[2]) {
	if (nodeIdx < 0 || nodeIdx >= ctp_routeCoordCount)
		return -1;

	nodeResult[0] = ctp_routeCoords[nodeIdx][0];
	nodeResult[1] = ctp_routeCoords[nodeIdx][1];

	return 0;
}

int WalkBox::setColor(int color) {
	int oldColor = _color;

	if (color == -1)
		return

	_color = color;

	return oldColor;
}

int WalkBox::setState(int state) {
	int oldState = _state;

	if (state == -1)
		return oldState;

	_state = state;

	return oldState;
}

void initWalkBoxes(short int segmentSizeTable[7], uint8 *dataPointer, bool isLoading) {

	// read polygons
	ASSERT((segmentSizeTable[2] % 80) == 0);
	for (int i = 0; i < segmentSizeTable[2] / 80; i++) {
		for (int j = 0; j < 40; j++) {
			walkboxes[i]._array[j] = (int16)READ_BE_UINT16(dataPointer);
			dataPointer += 2;
		}
	}

	if (isLoading) {
		// loading from save, ignore the initial values
		dataPointer += segmentSizeTable[3];
		dataPointer += segmentSizeTable[4];
	} else {
		// get the walkbox type
		// Type: 0x00 - non walkable, 0x01 - walkable, 0x02 - exit zone
		ASSERT((segmentSizeTable[3] % 2) == 0);
		for (int i = 0; i < segmentSizeTable[3] / 2; i++) {
			walkboxes[i]._color = (int16)READ_BE_UINT16(dataPointer);
			dataPointer += 2;
		}

		// change indicator, walkbox type can change, i.e. blocked by object (values are either 0x00 or 0x01)
		ASSERT((segmentSizeTable[4] % 2) == 0);
		for (int i = 0; i < segmentSizeTable[4] / 2; i++) {
			walkboxes[i]._state = (int16)READ_BE_UINT16(dataPointer);
			dataPointer += 2;
		}
	}

	//
	ASSERT((segmentSizeTable[5] % 2) == 0);
	for (int i = 0; i < segmentSizeTable[5] / 2; i++) {
		walkboxColorIndex[i] = (int16)READ_BE_UINT16(dataPointer);
		dataPointer += 2;
	}

	//
	ASSERT((segmentSizeTable[6] % 2) == 0);
	for (int i = 0; i < segmentSizeTable[6] / 2; i++) {
		walkboxes[i]._zoom = (int16)READ_BE_UINT16(dataPointer);
		dataPointer += 2;
	}

	numberOfWalkboxes = segmentSizeTable[6] / 2;	// get the number of walkboxes
}

int initCt(const char *ctpName, bool isLoading) {
	uint8 *dataPointer;	// ptr2
	char fileType[5];	// string2
	short int segmentSizeTable[7];	// tempTable

	if (!isLoading) {
		for (int i = 0; i < NUM_PERSONS; i++) {
			persoTable[i] = NULL;
		}
	}
	uint8* ptr = NULL;
	if (!loadFileSub1(&ptr, ctpName, 0)) {
		MemFree(ptr);
		return (-18);
	}

	dataPointer = ptr;

	fileType[4] = 0;
	memcpy(fileType, dataPointer, 4);	// get the file type, first 4 bytes of the CTP file
	dataPointer += 4;

	if (strcmp(fileType, "CTP ")) {
		MemFree(ptr);
		return (0);
	}

	ctp_routeCoordCount = (int16)READ_BE_UINT16(dataPointer); // get the number of nods
	dataPointer += 2;

	for (int i = 0; i < 7; i++) {
		segmentSizeTable[i] = (int16)READ_BE_UINT16(dataPointer);
		dataPointer += 2;
	}

	// get the path-finding coordinates
	ASSERT((segmentSizeTable[0] % 4) == 0);
	for (int i = 0; i < segmentSizeTable[0] / 4; i++) {
		ctp_routeCoords[i][0] = (int16)READ_BE_UINT16(dataPointer);
		dataPointer += 2;
		ctp_routeCoords[i][1] = (int16)READ_BE_UINT16(dataPointer);
		dataPointer += 2;
	}

	// get the path-finding line informations (indexing the routeCoords array)
	ASSERT((segmentSizeTable[1] % 20) == 0);
	for (int i = 0; i < segmentSizeTable[1] / 20; i++) {
		for (int j = 0; j < 10; j++) {
			ctp_routes[i][j] = (int16)READ_BE_UINT16(dataPointer);
			dataPointer += 2;
		}
	}

	initWalkBoxes(segmentSizeTable, dataPointer, isLoading);

	MemFree(ptr);

	if (ctpName != currentCtpName)
		strcpy(currentCtpName, ctpName);

	computeAllDistance(distanceTable, ctp_routeCoordCount);	// process path-finding stuff

	// Load the polyStructNorm list

	for (int i = numberOfWalkboxes - 1; i >= 0; i--) {
		makeCtStruct(_vm->_polyStructNorm, &walkboxes[i], i, 0);
	}

	// Load the polyStructExp list

	for (int i = numberOfWalkboxes - 1; i >= 0; i--) {
		makeCtStruct(_vm->_polyStructExp, &walkboxes[i], i, walkboxes[i]._zoom * 20);
	}

	_vm->_polyStruct = _vm->_polyStructs = &_vm->_polyStructNorm;

	return (1);
}

} // End of namespace Cruise
