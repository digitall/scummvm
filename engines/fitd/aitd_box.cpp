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
#include "fitd/fitd.h"
#include "fitd/gfx.h"
#include "fitd/vars.h"

namespace Fitd {

void afficheSprite(int left, int top, int index, byte *gfxData) {
	if (g_engine->getGameId() >= GID_AITD3)
		return;

	byte *outPtr = logicalScreen + top * 320 + left;
	const byte *inPtr = gfxData + READ_LE_U16(index * 2 + gfxData); // alignment unsafe

	inPtr += 4;

	const int width = READ_LE_U16(inPtr); // alignment unsafe
	inPtr += 2;
	const int height = READ_LE_U16(inPtr); // alignment unsafe
	inPtr += 2;

	const int offset = 320 - width;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			*outPtr++ = *inPtr++;
		}

		outPtr += offset;
	}
}

void affSpfI(int left, int top, int index, byte *gfxData) {
	if (g_engine->getGameId() >= GID_AITD3)
		return;

	byte *outPtr = logicalScreen + top * 320 + left;
	const byte *inPtr = gfxData + READ_LE_U16(index * 2 + gfxData); // alignment unsafe

	inPtr += 4;

	const int width = READ_LE_U16(inPtr); // alignment unsafe
	inPtr += 2;
	const int height = READ_LE_U16(inPtr); // alignment unsafe
	inPtr += 2;

	const int offset = 320 - width;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			const char color = *inPtr++;
			if (color) {
				*outPtr = color;
			}
			outPtr++;
		}

		outPtr += offset;
	}
}

void affBigCadre(int x, int y, int width, int height) {
	int top;
	int left;

	setClip(0, 0, 319, 199);

	const int halfWidth = width / 2;
	int currentLeftPosition = left = x - halfWidth;

	const int halfHeight = height / 2;
	int currentTopPosition = top = y - halfHeight;

	const int right = x + halfWidth;
	const int bottom = y + halfHeight;

	afficheSprite(currentLeftPosition, currentTopPosition, 0, ptrCadre); // draw top left corner

	while (true) // draw top bar
	{
		currentLeftPosition += 20;

		if (right - 20 <= currentLeftPosition)
			break;

		afficheSprite(currentLeftPosition, currentTopPosition, 4, ptrCadre);
	}

	afficheSprite(currentLeftPosition, currentTopPosition, 1, ptrCadre); // draw top right corner

	currentLeftPosition = left;

	while (true) // draw left bar
	{
		currentTopPosition += 20;

		if (bottom - 20 <= currentTopPosition)
			break;

		afficheSprite(currentLeftPosition, currentTopPosition, 6, ptrCadre);
	}

	currentLeftPosition = right - 8;
	currentTopPosition = top + 20;

	while (bottom - 20 > currentTopPosition) {
		afficheSprite(currentLeftPosition, currentTopPosition, 7, ptrCadre);

		currentTopPosition += 20;
	}

	currentLeftPosition = left;

	afficheSprite(currentLeftPosition, currentTopPosition, 2, ptrCadre); // draw bottom left corner

	while (true) // draw bottom bar
	{
		currentLeftPosition += 20;

		if (right - 20 <= currentLeftPosition)
			break;

		afficheSprite(currentLeftPosition, currentTopPosition + 12, 5, ptrCadre);
	}

	afficheSprite(currentLeftPosition, currentTopPosition, 3, ptrCadre); // draw bottom right corner

	afficheSprite(x - 20, currentTopPosition + 12, 8, ptrCadre); // draw "in the dark"

	windowX1 = left + 8;
	windowY1 = top + 8;
	windowX2 = right - 9;
	windowY2 = bottom - 9;

	fillBox(windowX1, windowY1, windowX2, windowY2, 0);
	setClip(windowX1, windowY1, windowX2, windowY2);
}

void affBigCadre2(int x, int y, int width, int height) {
	int top;
	int left;

	setClip(0, 0, 319, 199);

	const int halfWidth = width / 2;
	int currentLeftPosition = left = x - halfWidth;

	const int halfHeight = height / 2;
	int currentTopPosition = top = y - halfHeight;

	const int right = x + halfWidth;
	const int bottom = y + halfHeight;

	afficheSprite(currentLeftPosition, currentTopPosition, 0, ptrCadre); // draw top left corner

	// draw top bar
	while (true) {
		currentLeftPosition += 30;

		if (right - 30 <= currentLeftPosition)
			break;

		afficheSprite(currentLeftPosition, currentTopPosition, 4, ptrCadre);
	}

	afficheSprite(right - 30, currentTopPosition, 1, ptrCadre); // draw top right corner

	currentLeftPosition = left;
	currentTopPosition += 30;

	// draw left/right bars
	while (true) {
		afficheSprite(currentLeftPosition, currentTopPosition, 6, ptrCadre);
		afficheSprite(right - 17, currentTopPosition, 7, ptrCadre);
		currentTopPosition += 11;

		if (bottom - 30 <= currentTopPosition)
			break;
	}
	currentLeftPosition = left;

	currentTopPosition = bottom - 30;
	afficheSprite(currentLeftPosition, currentTopPosition, 2, ptrCadre); // draw bottom left corner

	while (true) // draw bottom bar
	{
		currentLeftPosition += 30;

		if (right - 30 <= currentLeftPosition)
			break;

		afficheSprite(currentLeftPosition, currentTopPosition + 13, 5, ptrCadre);
	}

	afficheSprite(right - 30, currentTopPosition, 3, ptrCadre); // draw bottom right corner

	windowX1 = left + 17;
	windowY1 = top + 16;
	windowX2 = right - 14;
	windowY2 = bottom - 18;

	fillBox(windowX1, windowY1, windowX2, windowY2, 0);
	setClip(windowX1, windowY1, windowX2, windowY2);
}

void initCopyBox(byte *var0, byte *var1) {
	screenSm1 = var0;
	screenSm2 = var0;

	screenSm3 = var1;
	screenSm4 = var1;
	screenSm5 = var1;
}

} // namespace Fitd
