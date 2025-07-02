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

#include "audio/mixer.h"
#include "common/savefile.h"
#include "graphics/screen.h"

#include "fitd/aitd_box.h"
#include "fitd/common.h"
#include "fitd/fitd.h"
#include "fitd/font.h"
#include "fitd/game_time.h"
#include "fitd/gfx.h"
#include "fitd/life.h"
#include "fitd/pak.h"
#include "fitd/sequence.h"
#include "fitd/system_menu.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"

#define NB_OPTIONS 7
#define SELECT_COUL 0xF
#define MENU_COUL 4
#define SIZE_FONT 16

namespace Fitd {

int input5;

void AffOption(int n, int num, int selected) {
	int y = windowY1 + (windowY2 - windowY1) / 2 - NB_OPTIONS * SIZE_FONT / 2 + n * SIZE_FONT;

	if (n == selected) {
		selectedMessage(160, y, num, SELECT_COUL, MENU_COUL);
	} else {
		simpleMessage(160, y, num, MENU_COUL);
	}
}

void scaleDownImage(int16 srcWidth, int16 srcHeight, int16 x, int16 y, const byte *src, byte *out, int outWidth) {
	const int srcPitch = srcWidth;

	const int dstWidth = 80;
	const int dstHeight = 50;
	const int dstPitch = dstWidth;
	byte dstImg[dstWidth * dstHeight];
	Graphics::scaleBlit(dstImg, src, dstPitch, srcPitch, dstWidth, dstHeight, srcWidth, srcHeight, Graphics::PixelFormat::createFormatCLUT8());
	const byte *in = dstImg;
	for (int i = y; i < y + dstHeight; i++) {
		byte *o = out + x + i * outWidth;
		for (int j = x; j < x + dstWidth; j++) {
			*o++ = *in++;
		}
	}
}

static void scaleDownImage(int16 srcWidth, int16 srcHeight, int16 x, int16 y, const byte *src) {
	scaleDownImage(srcWidth, srcHeight, x, y, src, logicalScreen, 320);
}

void aitd2AffOption(int n, int num, int selected) {
	int y = 34 + n * fontHeight;
	if (n == selected) {
		selectedMessage(160, y, num, 1, MENU_COUL);
	} else {
		simpleMessage(160, y, num, MENU_COUL);
	}
}

void aitd2DisplayOptions(int selectedStringNumber) {
	pakLoad("ITD_RESS.PAK", 17, logicalScreen);
	byte lpalette[0x300];
	copyPalette(logicalScreen + 64000, lpalette);
	convertPaletteIfRequired(lpalette);
	copyPalette(lpalette, currentGamePalette);
	gfx_setPalette(lpalette);

	setClip(windowX1, windowY1, windowX2, windowY2);

	aitd2AffOption(0, 48, selectedStringNumber);
	aitd2AffOption(1, 45, selectedStringNumber);
	aitd2AffOption(2, 46, selectedStringNumber);
	aitd2AffOption(3, 41 + musicEnabled, selectedStringNumber);
	aitd2AffOption(4, 43 + soundToggle, selectedStringNumber);
	aitd2AffOption(5, 49 + detailToggle, selectedStringNumber);
	aitd2AffOption(6, 47, selectedStringNumber);
}

void AffOptionList(int selectedStringNumber) {
	if (g_engine->getGameId() == GID_AITD2) {
		aitd2DisplayOptions(selectedStringNumber);
		return;
	}

	affBigCadre(160, 100, 320, 200);

	int backupTop = windowY1;
	int backupBottom = windowY2;
	int backupLeft = windowX1;
	int backupRight = windowX2;

	affBigCadre(80, 55, 120, 70);

	scaleDownImage(320, 200, 40, 35, aux2);

	windowY1 = backupTop;
	windowY2 = backupBottom;
	windowX1 = backupLeft;
	windowX2 = backupRight;

	setClip(windowX1, windowY1, windowX2, windowY2);

	AffOption(0, 48, selectedStringNumber);
	AffOption(1, 45, selectedStringNumber);
	AffOption(2, 46, selectedStringNumber);
	AffOption(3, 41 + musicEnabled, selectedStringNumber);
	AffOption(4, 43 + soundToggle, selectedStringNumber);
	AffOption(5, 49 + detailToggle, selectedStringNumber);
	AffOption(6, 47, selectedStringNumber);
}

static void drawSavegames(int menuChoice, const SaveStateList &saveStateList, int selectedSlot) {
	int y = 55;
	extSetFont(ptrFont, 14);
	selectedMessage(160, 0, menuChoice, SELECT_COUL, MENU_COUL);

	if (saveStateList.empty()) {
		setClip(0, 0, 319, 199);
		fillBox(0, y - 2, 319, y + 18, 100);
		affBigCadre(70, y, 120, 70);
		setClip(0, 0, 319, 199);
		return;
	}

	for (int i = 0; i < MIN(6, (int)saveStateList.size()); ++i) {
		Common::String desc(saveStateList[i].getDescription().encode(Common::kASCII));
		if (i == selectedSlot) {
			setClip(0, 0, 319, 199);
			fillBox(0, y - 2, 319, y + 18, 100);
			affBigCadre(70, y, 120, 70);
			setClip(0, 0, 319, 199);
			const Graphics::Surface *s = saveStateList[i].getThumbnail();
			Graphics::Surface *d;
			if (s) {
				d = s->convertTo(Graphics::PixelFormat::createFormatCLUT8(), 0, 256, currentGamePalette, 256);
			} else {
				d = new Graphics::Surface();
				d->create(80, 50, Graphics::PixelFormat::createFormatCLUT8());
			}
			scaleDownImage(d->w, d->h, 30, y - 20, (const byte *)d->getBasePtr(0, 0));
			renderText(140, y, desc.c_str());
		} else {
			renderText(140, y, desc.c_str());
		}
		y += 20;
	}
}

static void drawEditString(const char *string, const int selectedSlot) {
	const int size = extGetSizeFont(string);
	const int y = (selectedSlot & ~0x4000) * 20 + 55;
	if (selectedSlot & 0x4000) {
		fillBox(140, y, 319, y + 16, 100);
	} else {
		fillBox(140, y, size + 160, y + 16, 100);
	}
	renderText(140, y, string);
	fillBox(size + 141, y, size + 144, y + 16, 15);
}

static int chooseSavegame(const int menuChoice, const bool save, Common::String &desc) {
	int selectedSlot = 0;
	bool edit = false;

	SaveStateList saveStateList(g_engine->listSaveFiles());
	if (save && saveStateList.size() < 6) {
		saveStateList.emplace_back(SaveStateDescriptor());
	}

	const uint maxSavegameCount = MIN(saveStateList.size(), 6U);
	if (selectedSlot < saveStateList.size()) {
		desc = saveStateList[selectedSlot].getDescription();
	}

	while (!::Engine::shouldQuit()) {
		drawSavegames(menuChoice, saveStateList, selectedSlot);
		if (save) {
			drawEditString(desc.c_str(), selectedSlot + (edit ? 0x4000 : 0));
			if (edit) {
				scaleDownImage(320, 200, 30, 35 + selectedSlot * 20, aux2);
			}
		}
		gfx_copyBlockPhys(logicalScreen, 0, 0, 320, 200);
		osystem_startFrame();
		process_events();
		flushScreen();
		osystem_drawBackground();

		if (joyD & 1) {
			// up key
			edit = false;
			selectedSlot--;
			if (selectedSlot < 0) {
				selectedSlot = maxSavegameCount - 1;
			}

			if (selectedSlot < 0) {
				selectedSlot = 0;
			}

			if (selectedSlot < saveStateList.size()) {
				desc = saveStateList[selectedSlot].getDescription();
			}

			while (!::Engine::shouldQuit() && joyD) {
				process_events();
			}
		}

		if (joyD & 2) {
			// down key
			edit = false;
			selectedSlot++;
			if (selectedSlot >= maxSavegameCount) {
				selectedSlot = 0;
			}

			while (!::Engine::shouldQuit() && joyD) {
				process_events();
			}

			if (selectedSlot < saveStateList.size()) {
				desc = saveStateList[selectedSlot].getDescription();
			}
		}

		if (key == 27) {
			return -1;
		}

		if (key == 28 || (!save && click != 0)) {
			// select current entry
			return selectedSlot;
		}

		if (save) {
			if (backspace) {
				edit = true;
				desc.deleteLastChar();

				while (!::Engine::shouldQuit() && backspace) {
					process_events();
				}
			}
			if (character >= 32 && character < 184) {
				if (!edit) {
					edit = true;
					desc.clear();
				}
				if (desc.size() < 32) {
					desc.insertChar(character, desc.size());
					if (extGetSizeFont(desc.c_str()) >= (300 - 140)) {
						desc.deleteLastChar();
					}
				}

				while (!::Engine::shouldQuit() && character) {
					process_events();
				}
			}
		}
	}
	return -1;
}

bool showLoadMenu(int menuChoice) {
	fadeInPhys(0x40, 0);

	Common::String desc;
	const int selectedSlot = chooseSavegame(menuChoice, false, desc);
	// fadeOutPhys(8, 0);
	//  freeAll();
	if (selectedSlot != -1 && g_engine->loadGameState(selectedSlot).getCode() == Common::kNoError) {
		return true;
	}
	return false;
}

static bool showSaveMenu(int menuChoice) {
	Common::String desc;
	const int selectedSlot = chooseSavegame(menuChoice, true, desc);
	if (selectedSlot == -1)
		return false;
	if (g_engine->saveGameState(selectedSlot, desc, false).getCode() == Common::kNoError) {
		makeMessage(51);
	} else {
		makeMessage(52);
	}
	return true;
}

void processSystemMenu() {
	// int entry = -1;
	int exitMenu = 0;

	freezeTime();
	saveAmbiance();

	if (lightOff) {
		makeBlackPalette();
	}

	// clearScreenSystemMenu(unkScreenVar,aux2);

	int currentSelectedEntry = 0;

	while (!exitMenu && !::Engine::shouldQuit()) {
		AffOptionList(currentSelectedEntry);
		gfx_copyBlockPhys(logicalScreen, 0, 0, 320, 200);
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
			localClick = click;
			localJoyD = joyD;

			if (!input5) {
				if (localKey == 0x1C || localClick) // enter
				{
					key &= ~0x1C;
					switch (currentSelectedEntry) {
					case 0: // exit menu
						exitMenu = 1;
						break;
					case 1: // save
						if (showSaveMenu(45)) {
							exitMenu = 1;
						}
						break;
					case 2: // load
						if (showLoadMenu(46)) {
							flagInitView = 2;
							unfreezeTime();
							restoreAmbiance();
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
						::Engine::quitGame();
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

		osystem_flip(nullptr);
	}

	// fadeOut(32,2);
	while ((key || joyD || click) && !::Engine::shouldQuit()) {
		process_events();
	}
	localKey = localClick = localJoyD = 0;
	flagInitView = 2;
	unfreezeTime();
}
} // namespace Fitd
