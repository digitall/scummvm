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
#include "fitd/font.h"
#include "fitd/game_time.h"
#include "fitd/gfx.h"
#include "fitd/save.h"
#include "fitd/system_menu.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"
#include "audio/mixer.h"

#define NB_OPTIONS 7
#define SELECT_COUL 0xF
#define MENU_COUL 4
#define SIZE_FONT 16

namespace Fitd {

int input5;
Graphics::Surface *savedSurface = NULL;

void AffOption(int n, int num, int selected) {
	int y = WindowY1 + ((WindowY2 - WindowY1) / 2) - (NB_OPTIONS * SIZE_FONT) / 2 + (n * SIZE_FONT);

	if (n == selected) {
		selectedMessage(160, y, num, SELECT_COUL, MENU_COUL);
	} else {
		simpleMessage(160, y, num, MENU_COUL);
	}
}

void AffOptionList(int selectedStringNumber) {
	affBigCadre(160, 100, 320, 200);

	int backupTop = WindowY1;
	int backupBottom = WindowY2;
	int backupLeft = WindowX1;
	int backupRight = WindowX2;

	affBigCadre(80, 55, 120, 70);

	// scaleDownImage(40,35,aux2);

	WindowY1 = backupTop;
	WindowY2 = backupBottom;
	WindowX1 = backupLeft;
	WindowX2 = backupRight;

	setClip(WindowX1, WindowY1, WindowX2, WindowY2);

	AffOption(0, 48, selectedStringNumber);
	AffOption(1, 45, selectedStringNumber);
	AffOption(2, 46, selectedStringNumber);
	AffOption(3, 41 + musicEnabled, selectedStringNumber);
	AffOption(4, 43 + soundToggle, selectedStringNumber);
	AffOption(5, 49 + detailToggle, selectedStringNumber);
	AffOption(6, 47, selectedStringNumber);

	menuWaitVSync();
}

void processSystemMenu(void) {
	// int entry = -1;
	int exitMenu = 0;
	int currentSelectedEntry;

	freezeTime();
	savedSurface = gfx_capture();
	// pauseShaking();

	if (lightOff) {
		// makeBlackPalette();
	}

	// clearScreenSystemMenu(unkScreenVar,aux2);

	currentSelectedEntry = 0;

	while (!exitMenu && !g_engine->shouldQuit()) {
		AffOptionList(currentSelectedEntry);
		gfx_copyBlockPhys((unsigned char *)logicalScreen, 0, 0, 320, 200);
		osystem_startFrame();
		process_events();
		flushScreen();
		osystem_drawBackground();

		if (lightOff) {
			fadeInPhys(0x40, 0);
		}

		//  while(!exitMenu)
		{
			localKey = key;
			localClick = Click;
			localJoyD = JoyD;

			if (!input5) {
				if (localKey == 0x1C || localClick) // enter
				{
					key &= ~0x1C;
					switch (currentSelectedEntry) {
					case 0: // exit menu
						exitMenu = 1;
						break;
					case 1: // save
						g_engine->saveGameState(1, "", false);
						break;
					case 2: // load
						if (g_engine->loadGameState(1).getCode() == Common::kNoError) {
							flagInitView = 2;
							unfreezeTime();
							// updateShaking();
							return;
						}
						break;
					case 3: // music
						musicEnabled = musicEnabled ? 0 : 1;
						g_engine->_mixer->muteSoundType(Audio::Mixer::kMusicSoundType, musicEnabled ? false : true);
						break;
					case 4: // sound
						soundToggle = soundToggle ? 0 : 1;
						g_engine->_mixer->muteSoundType(Audio::Mixer::kSFXSoundType, soundToggle ? false : true);
						break;
					case 5: // details
						detailToggle = detailToggle ? 0 : 1;
						break;
					case 6: // quit
						g_engine->quitGame();
						break;
					}
				} else {
					if (localKey == 0x1B) {
						exitMenu = 1;
					}
					if (localJoyD == 1) // up
					{
						currentSelectedEntry--;

						if (currentSelectedEntry < 0)
							currentSelectedEntry = 6;

						input5 = 1;
					}
					if (localJoyD == 2) // bottom
					{
						currentSelectedEntry++;

						if (currentSelectedEntry > 6)
							currentSelectedEntry = 0;

						input5 = 1;
					}
				}
			} else {
				if (!localKey && !localJoyD) {
					input5 = 0;
				}
			}
		}

		osystem_flip(NULL);
	}

	// fadeOut(32,2);
	while ((key || JoyD || Click) && !g_engine->shouldQuit()) {
		process_events();
	}
	localKey = localClick = localJoyD = 0;
	flagInitView = 2;
	unfreezeTime();
}
} // namespace Fitd
