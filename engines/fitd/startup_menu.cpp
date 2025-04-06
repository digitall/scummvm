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
#include "fitd/fitd.h"
#include "fitd/font.h"
#include "fitd/gfx.h"
#include "fitd/startup_menu.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"

namespace Fitd {

void drawStartupMenu(int selectedEntry) {
	int currentY = 76;
	int currentTextNum = 0;

	affBigCadre(160, 100, 320, 80);

	while (currentTextNum < 3) {
		if (currentTextNum == selectedEntry) // hilight selected entry
		{
			fillBox(10, currentY, 309, currentY + 16, 100);
			selectedMessage(160, currentY, currentTextNum + 11, 15, 4);
		} else {
			simpleMessage(160, currentY, currentTextNum + 11, 4);
		}

		currentY += 16;   // next line
		currentTextNum++; // next text
	}
}

int processStartupMenu(void) {
	int currentSelectedEntry = 0;
	unsigned int chrono;
	int selectedEntry = -1;

	flushScreen();

	drawStartupMenu(0);

	osystem_startFrame();
	// osystem_stopFrame();
	gfx_copyBlockPhys((unsigned char *)logicalScreen, 0, 0, 320, 200);

	osystem_flip(NULL);
	fadeInPhys(16, 0);
	startChrono(&chrono);

	while (evalChrono(&chrono) <= 0x10000) // exit loop only if time out or if choice made
	{
		if(g_engine->shouldQuit()) {
			selectedEntry = 2;
		}

		gfx_copyBlockPhys((unsigned char *)logicalScreen, 0, 0, 320, 200);
		osystem_startFrame();

		if (selectedEntry != -1 || evalChrono(&chrono) > 0x10000) {
			break;
		}

		process_events();
		osystem_drawBackground();

		if (JoyD & 1) // up key
		{
			currentSelectedEntry--;

			if (currentSelectedEntry < 0) {
				currentSelectedEntry = 2;
			}

			drawStartupMenu(currentSelectedEntry);
			osystem_flip(NULL);
			//      menuWaitVSync();

			startChrono(&chrono);

			while (JoyD) {
				process_events();
			}
		}

		if (JoyD & 2) // down key
		{
			currentSelectedEntry++;

			if (currentSelectedEntry > 2) {
				currentSelectedEntry = 0;
			}

			drawStartupMenu(currentSelectedEntry);
			// menuWaitVSync();
			osystem_flip(NULL);

			startChrono(&chrono);

			while (JoyD) {
				process_events();
			}
		}

		if ((key == 28) || (Click != 0)) // select current entry
		{
			selectedEntry = currentSelectedEntry;
		}
		// osystem_stopFrame();
		osystem_flip(NULL);
	}

	if (selectedEntry == 2) // if exit game, do not fade
	{
		fadeOutPhys(16, 0);
	}

	while (JoyD) {
		process_events();
	}

	return (selectedEntry);
}

} // namespace Fitd
