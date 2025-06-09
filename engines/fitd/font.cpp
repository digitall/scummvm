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
#include "fitd/font.h"
#include "fitd/vars.h"

namespace Fitd {

textEntryStruct *tabTextes;
int fontHeight = 16;

char *fontVar1 = nullptr;
int16 fontSm1 = 0;
int16 fontSm2 = 0x1234;
char *fontVar4 = nullptr;
char *fontVar5 = nullptr;
int16 currentFontColor = 0;
int16 g_fontInterWordSpace = 2;
int16 g_fontInterLetterSpace = 1;
int16 fontSm3 = 18;
int16 fontVar6 = 0;
int16 fontSm7 = 0x1234;
int16 fontSm8 = 0x1234;
int16 fontSm9 = 0x80;

byte flagTable[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

void extSetFont(char *fontData, int color) {

	fontVar1 = fontData; // fontPtr

	const int16 tempDx = READ_LE_S16(fontData); // alignment
	fontData += 2;

	fontSm1 = *fontData++;         // character height
	fontSm2 = *(byte *)fontData++; // character size

	if (!fontSm2) {
		fontSm2 = READ_LE_S16(fontData);
	}

	fontData += 2;

	int16 tempAxFlip = READ_LE_S16(fontData);
	fontData += 2;

	tempAxFlip = (tempAxFlip & 0xFF) << 8 | (tempAxFlip & 0xFF00) >> 8;

	fontVar4 = fontData;

	fontVar5 = fontVar1 + tempAxFlip - (tempDx & 0xFF) * 2;

	currentFontColor = color;

	fontSm3 = color;
}

void setFontSpace(int interWordSpace, int interLetterSpace) {
	g_fontInterWordSpace = interWordSpace;
	g_fontInterLetterSpace = interLetterSpace;
}

int extGetSizeFont(const char *string) {
	int width = 0;
	byte character;

	while ((character = *string++)) {

		const char *dataPtr = fontVar5 + character * 2;
		uint16 data = READ_LE_S16(dataPtr);

		data >>= 4;

		data &= 0xF;

		if (!data) {
			width += g_fontInterWordSpace;
		}

		width += g_fontInterLetterSpace;
		width += data;
	}

	return width;
}

void renderText(int x, int y, const char *string) {
	byte character;

	fontVar6 = x;
	fontSm7 = y;

	while ((character = *string++)) {

		const char *dataPtr = fontVar5 + character * 2;
		uint16 data = READ_LE_U16(dataPtr);

		data = (data & 0xFF) << 8 | (data & 0xFF00) >> 8;

		uint16 dx = data;

		data >>= 12;

		if (data & 0xF) // real character (width != 0)
		{

			dx &= 0xFFF;

			const char *characterPtr = (dx >> 3) + fontVar4;

			fontSm9 = flagTable[dx & 7];

			int bp = fontSm7;

			fontSm8 = fontVar6;

			for (int ch = fontSm1; ch > 0; ch--) {
				if (bp >= 200)
					return;
				char *outPtr = logicalScreen + bp * 320 + fontSm8;

				int dh = fontSm9;
				int cl = data & 0xF;

				int al = *characterPtr;

				bp++;

				for (int bx = 0; cl > 0; cl--) {
					if (dh & al) {
						*outPtr = (char)fontSm3;
					}

					outPtr++;

					dh = ((dh >> 1) & 0x7F) | ((dh << 7) & 0x80);

					if (dh & 0x80) {
						bx++;
						al = *(characterPtr + bx);
					}
				}

				characterPtr += fontSm2;
			}

			fontVar6 += data & 0xF;
		} else // space character
		{
			fontVar6 += g_fontInterWordSpace;
		}

		fontVar6 += g_fontInterLetterSpace;
	}
}

textEntryStruct *getTextFromIdx(int index) {

	for (int currentIndex = 0; currentIndex < NUM_MAX_TEXT_ENTRY; currentIndex++) {
		if (tabTextes[currentIndex].index == index) {
			return &tabTextes[currentIndex];
		}
	}

	return nullptr;
}

void selectedMessage(int x, int y, int index, int color1, int color2) {

	const textEntryStruct *entryPtr = getTextFromIdx(index);

	if (!entryPtr)
		return;

	x -= entryPtr->width / 2; // center

	const char *textPtr = entryPtr->textPtr;

	extSetFont(PtrFont, color2);
	renderText(x, y + 1, textPtr);

	extSetFont(PtrFont, color1);
	renderText(x, y, textPtr);
}

void simpleMessage(int x, int y, int index, int color) {
	const textEntryStruct *entryPtr = getTextFromIdx(index);

	if (!entryPtr)
		return;

	x -= entryPtr->width / 2; // center

	const char *textPtr = entryPtr->textPtr;

	extSetFont(PtrFont, color);

	renderText(x, y + 1, textPtr);
}
} // namespace Fitd
