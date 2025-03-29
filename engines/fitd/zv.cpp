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

#include "fitd/vars.h"
#include "fitd/zv.h"

namespace Fitd {

void getZvCube(char *bodyPtr, ZVStruct *zvPtr) {
	int16 *ptr;

	ptr = (int16 *)(bodyPtr + 2);

	zvPtr->ZVX1 = *(ptr++);
	zvPtr->ZVX2 = *(ptr++);
	zvPtr->ZVY1 = *(ptr++);
	zvPtr->ZVY2 = *(ptr++);
	zvPtr->ZVZ1 = *(ptr++);
	zvPtr->ZVZ2 = *(ptr++);

	zvPtr->ZVZ2 = zvPtr->ZVX2 = (zvPtr->ZVX2 + zvPtr->ZVZ2) / 2;
	zvPtr->ZVX1 = zvPtr->ZVZ1 = -zvPtr->ZVZ2;
}

void giveZVObjet(char *bodyPtr, ZVStruct *zvPtr) {
	int16 *ptr;

	ptr = (int16 *)(bodyPtr + 2);

	zvPtr->ZVX1 = *(ptr++);
	zvPtr->ZVX2 = *(ptr++);
	zvPtr->ZVY1 = *(ptr++);
	zvPtr->ZVY2 = *(ptr++);
	zvPtr->ZVZ1 = *(ptr++);
	zvPtr->ZVZ2 = *(ptr++);
}

void makeDefaultZV(ZVStruct *zvPtr) {
	zvPtr->ZVX1 = -100;
	zvPtr->ZVX2 = 100;

	zvPtr->ZVY1 = -2000;
	zvPtr->ZVY2 = 0;

	zvPtr->ZVZ1 = -100;
	zvPtr->ZVZ2 = 100;
}

void getZvMax(char *bodyPtr, ZVStruct *zvPtr) {
	int x1;
	int x2;
	int z1;
	int z2;

	giveZVObjet(bodyPtr, zvPtr);

	x1 = zvPtr->ZVX1;
	x2 = zvPtr->ZVX2;

	z1 = zvPtr->ZVZ1;
	z2 = zvPtr->ZVZ2;

	x2 = -x1 + x2;
	z2 = -z1 + z2;

	if (x2 < z2) {
		x2 = z2;
	}

	x2 /= 2;

	zvPtr->ZVX1 = -x2;
	zvPtr->ZVX2 = x2;

	zvPtr->ZVZ1 = -x2;
	zvPtr->ZVZ2 = x2;
}
} // namespace Fitd
