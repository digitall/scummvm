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

#include "fitd/aitd_box.h"
#include "fitd/common.h"
#include "fitd/gfx.h"
#include "fitd/vars.h"

namespace Fitd {

void afficheSprite(int left, int top, int index, char *gfxData) {
	char *outPtr;
	char *inPtr;

	int width;
	int height;

	int offset;

	int i;
	int j;

	// if(g_gameId >= AITD3)
	//     return;

	outPtr = logicalScreen + top * 320 + left;
	inPtr = gfxData + READ_LE_U16(index * 2 + gfxData); // alignement unsafe

	inPtr += 4;

	width = READ_LE_U16(inPtr); // alignement unsafe
	inPtr += 2;
	height = READ_LE_U16(inPtr); // alignement unsafe
	inPtr += 2;

	offset = 320 - width;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			*(outPtr++) = *(inPtr++);
		}

		outPtr += offset;
	}
}

void affSpfI(int left, int top, int index, char *gfxData) {
	char *outPtr;
	char *inPtr;

	int width;
	int height;

	int offset;

	int i;
	int j;

	// if(g_gameId >= AITD3)
	// 	return;

	outPtr = logicalScreen + top * 320 + left;
	inPtr = gfxData + READ_LE_U16(index * 2 + gfxData); // alignement unsafe

	inPtr += 4;

	width = READ_LE_U16(inPtr); // alignement unsafe
	inPtr += 2;
	height = READ_LE_U16(inPtr); // alignement unsafe
	inPtr += 2;

	offset = 320 - width;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			char color = *(inPtr++);
			if (color) {
				*(outPtr) = color;
			}
			outPtr++;
		}

		outPtr += offset;
	}
}

void affBigCadre(int x, int y, int width, int height) {
	int top;
	int right;
	int left;
	int bottom;

	int currentLeftPosition;
	int currentTopPosition;

	int halfWidth;
	int halfHeight;

	setClip(0, 0, 319, 199);

	halfWidth = width / 2;
	currentLeftPosition = left = x - halfWidth;

	halfHeight = height / 2;
	currentTopPosition = top = y - halfHeight;

	right = x + halfWidth;
	bottom = y + halfHeight;

	afficheSprite(currentLeftPosition, currentTopPosition, 0, PtrCadre); // draw top left corner

	while (1) // draw top bar
	{
		currentLeftPosition += 20;

		if (right - 20 <= currentLeftPosition)
			break;

		afficheSprite(currentLeftPosition, currentTopPosition, 4, PtrCadre);
	}

	afficheSprite(currentLeftPosition, currentTopPosition, 1, PtrCadre); // draw top right corner

	currentLeftPosition = left;

	while (1) // draw left bar
	{
		currentTopPosition += 20;

		if (bottom - 20 <= currentTopPosition)
			break;

		afficheSprite(currentLeftPosition, currentTopPosition, 6, PtrCadre);
	}

	currentLeftPosition = right - 8;
	currentTopPosition = top + 20;

	while (bottom - 20 > currentTopPosition) {
		afficheSprite(currentLeftPosition, currentTopPosition, 7, PtrCadre);

		currentTopPosition += 20;
	}

	currentLeftPosition = left;

	afficheSprite(currentLeftPosition, currentTopPosition, 2, PtrCadre); // draw bottom left corner

	while (1) // draw bottom bar
	{
		currentLeftPosition += 20;

		if (right - 20 <= currentLeftPosition)
			break;

		afficheSprite(currentLeftPosition, currentTopPosition + 12, 5, PtrCadre);
	}

	afficheSprite(currentLeftPosition, currentTopPosition, 3, PtrCadre); // draw bottom right corner

	afficheSprite(x - 20, currentTopPosition + 12, 8, PtrCadre); // draw "in the dark"

	WindowX1 = left + 8;
	WindowY1 = top + 8;
	WindowX2 = right - 9;
	WindowY2 = bottom - 9;

	fillBox(WindowX1, WindowY1, WindowX2, WindowY2, 0);
	setClip(WindowX1, WindowY1, WindowX2, WindowY2);
}

} // namespace Fitd
