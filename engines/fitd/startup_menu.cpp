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

#include "fitd/startup_menu.h"

#include "fitd/aitd_box.h"
#include "fitd/fitd.h"
#include "fitd/font.h"
#include "fitd/gfx.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"
#include "pak.h"
#include "sequence.h"

namespace Fitd {

void aitd2DrawStartupMenu(int selectedEntry) {
	int currentY = 79;
	int currentTextNum = 0;

	affBigCadre2(160, 100, 320, 80);

	while (currentTextNum < 3) {
		if (currentTextNum == selectedEntry) // highlight selected entry
		{
			fillBox(17, currentY, 303, currentY + fontHeight, 100);
			selectedMessage(160, currentY + 1, currentTextNum + 11, 15, 4);
		} else {
			simpleMessage(160, currentY, currentTextNum + 11, 4);
		}

		currentY += fontHeight; // next line
		currentTextNum++;       // next text
	}
}

static void drawStartupMenu(int selectedEntry) {
	if (g_engine->getGameId() == GID_AITD2) {
		aitd2DrawStartupMenu(selectedEntry);
		return;
	}

	if (g_engine->getGameId() == GID_AITD3) {
		pakLoad("ITD_RESS.PAK", 13, logicalScreen);
	} else {
		affBigCadre(160, 100, 320, 80);
	}

	int currentY = 76;
	for (int i = 0; i < 3; i++) {
		if (i == selectedEntry) // highlight selected entry
		{
			if (g_engine->getGameId() == GID_AITD3) {
				selectedMessage(160, currentY + 1, i + 11, 1, 4);
			} else {
				fillBox(10, currentY, 309, currentY + 16, 100);
				selectedMessage(160, currentY, i + 11, 15, 4);
			}
		} else {
			simpleMessage(160, currentY, i + 11, 4);
		}

		currentY += 16; // next line
	}
}

int processStartupMenu() {
	int currentSelectedEntry = 0;
	uint chrono;
	int selectedEntry = -1;

	flushScreen();

	if (g_engine->getGameId() == GID_AITD3) {
		pakLoad("ITD_RESS.PAK", 47, aux);
		byte lpalette[768];
		copyPalette((byte *)aux, lpalette);
		convertPaletteIfRequired(lpalette);
		copyPalette(lpalette, currentGamePalette);
		gfx_setPalette(lpalette);
	}
	drawStartupMenu(0);

	osystem_startFrame();
	// osystem_stopFrame();
	gfx_copyBlockPhys((byte *)logicalScreen, 0, 0, 320, 200);

	osystem_flip(nullptr);
	fadeInPhys(16, 0);
	startChrono(&chrono);

	while (evalChrono(&chrono) <= 0x10000) // exit loop only if time out or if choice made
	{
		if (::Engine::shouldQuit()) {
			selectedEntry = 2;
		}

		gfx_copyBlockPhys((byte *)logicalScreen, 0, 0, 320, 200);
		osystem_startFrame();

		if (selectedEntry != -1 || evalChrono(&chrono) > 0x10000) {
			break;
		}

		process_events();
		osystem_drawBackground();

		if (joyD & 1) // up key
		{
			currentSelectedEntry--;

			if (currentSelectedEntry < 0) {
				currentSelectedEntry = 2;
			}

			drawStartupMenu(currentSelectedEntry);
			osystem_flip(nullptr);
			//      menuWaitVSync();

			startChrono(&chrono);

			while (joyD) {
				process_events();
			}
		}

		if (joyD & 2) // down key
		{
			currentSelectedEntry++;

			if (currentSelectedEntry > 2) {
				currentSelectedEntry = 0;
			}

			drawStartupMenu(currentSelectedEntry);
			// menuWaitVSync();
			osystem_flip(nullptr);

			startChrono(&chrono);

			while (joyD) {
				process_events();
			}
		}

		if (key == 28 || click != 0) // select current entry
		{
			selectedEntry = currentSelectedEntry;
		}
		// osystem_stopFrame();
		osystem_flip(nullptr);
	}

	if (selectedEntry == 2) // if exit game, do not fade
	{
		fadeOutPhys(16, 0);
	}

	while (joyD) {
		process_events();
	}

	return selectedEntry;
}

} // namespace Fitd
