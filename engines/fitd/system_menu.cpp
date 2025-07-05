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
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/font.h"
#include "fitd/game_time.h"
#include "fitd/gfx.h"
#include "fitd/life.h"
#include "fitd/pak.h"
#include "fitd/sequence.h"
#include "fitd/system_menu.h"
#include "fitd/tatou.h"

#define NB_OPTIONS 7
#define SELECT_COUL 0xF
#define MENU_COUL 4
#define SIZE_FONT 16

namespace Fitd {

int input5;

void AffOption(int n, int num, int selected) {
	const int y = g_engine->_engine->windowY1 + (g_engine->_engine->windowY2 - g_engine->_engine->windowY1) / 2 - NB_OPTIONS * SIZE_FONT / 2 + n * SIZE_FONT;

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
	scaleDownImage(srcWidth, srcHeight, x, y, src, g_engine->_engine->logicalScreen, 320);
}

void aitd2AffOption(int n, int num, int selected) {
	const int y = 34 + n * fontHeight;
	if (n == selected) {
		selectedMessage(160, y, num, 1, MENU_COUL);
	} else {
		simpleMessage(160, y, num, MENU_COUL);
	}
}

void aitd2DisplayOptions(int selectedStringNumber) {
	pakLoad("ITD_RESS.PAK", 17, g_engine->_engine->logicalScreen);
	byte lpalette[0x300];
	copyPalette(g_engine->_engine->logicalScreen + 64000, lpalette);
	convertPaletteIfRequired(lpalette);
	copyPalette(lpalette, currentGamePalette);
	gfx_setPalette(lpalette);

	setClip(g_engine->_engine->windowX1, g_engine->_engine->windowY1, g_engine->_engine->windowX2, g_engine->_engine->windowY2);

	aitd2AffOption(0, 48, selectedStringNumber);
	aitd2AffOption(1, 45, selectedStringNumber);
	aitd2AffOption(2, 46, selectedStringNumber);
	aitd2AffOption(3, 41 + g_engine->_engine->musicEnabled, selectedStringNumber);
	aitd2AffOption(4, 43 + g_engine->_engine->soundToggle, selectedStringNumber);
	aitd2AffOption(5, 49 + g_engine->_engine->detailToggle, selectedStringNumber);
	aitd2AffOption(6, 47, selectedStringNumber);
}

void AffOptionList(int selectedStringNumber) {
	if (g_engine->getGameId() == GID_AITD2) {
		aitd2DisplayOptions(selectedStringNumber);
		return;
	}

	affBigCadre(160, 100, 320, 200);

	const int backupTop = g_engine->_engine->windowY1;
	const int backupBottom = g_engine->_engine->windowY2;
	const int backupLeft = g_engine->_engine->windowX1;
	const int backupRight = g_engine->_engine->windowX2;

	affBigCadre(80, 55, 120, 70);

	scaleDownImage(320, 200, 40, 35, g_engine->_engine->aux2);

	g_engine->_engine->windowY1 = backupTop;
	g_engine->_engine->windowY2 = backupBottom;
	g_engine->_engine->windowX1 = backupLeft;
	g_engine->_engine->windowX2 = backupRight;

	setClip(g_engine->_engine->windowX1, g_engine->_engine->windowY1, g_engine->_engine->windowX2, g_engine->_engine->windowY2);

	AffOption(0, 48, selectedStringNumber);
	AffOption(1, 45, selectedStringNumber);
	AffOption(2, 46, selectedStringNumber);
	AffOption(3, 41 + g_engine->_engine->musicEnabled, selectedStringNumber);
	AffOption(4, 43 + g_engine->_engine->soundToggle, selectedStringNumber);
	AffOption(5, 49 + g_engine->_engine->detailToggle, selectedStringNumber);
	AffOption(6, 47, selectedStringNumber);
}

static void drawSavegames(int menuChoice, const SaveStateList &saveStateList, int selectedSlot) {
	int y = 55;
	extSetFont(g_engine->_engine->ptrFont, 14);
	selectedMessage(160, 0, menuChoice, SELECT_COUL, MENU_COUL);

	if (saveStateList.empty()) {
		setClip(0, 0, 319, 199);
		fillBox(0, y - 2, 319, y + 18, 100);
		affBigCadre(70, y, 120, 70);
		setClip(0, 0, 319, 199);
		return;
	}

	for (int i = 0; i < MIN(6, static_cast<int>(saveStateList.size())); ++i) {
		Common::String desc(saveStateList[i].getDescription().encode(Common::kASCII));
		if (i == selectedSlot) {
			setClip(0, 0, 319, 199);
			fillBox(0, y - 2, 319, y + 18, 100);
			affBigCadre(70, y, 120, 70);
			setClip(0, 0, 319, 199);
			const Graphics::Surface *s = saveStateList[i].getThumbnail();
			Common::ScopedPtr<Graphics::Surface> d;
			if (s) {
				d.reset(s->convertTo(Graphics::PixelFormat::createFormatCLUT8(), nullptr, 256, currentGamePalette, 256));
			} else {
				d.reset(new Graphics::Surface());
				d->create(80, 50, Graphics::PixelFormat::createFormatCLUT8());
			}
			scaleDownImage(d->w, d->h, 30, y - 20, static_cast<const byte *>(d->getBasePtr(0, 0)));
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
	if (selectedSlot < static_cast<int>(saveStateList.size())) {
		desc = saveStateList[selectedSlot].getDescription();
	}

	while (!::Engine::shouldQuit()) {
		drawSavegames(menuChoice, saveStateList, selectedSlot);
		if (save) {
			drawEditString(desc.c_str(), selectedSlot + (edit ? 0x4000 : 0));
			if (edit) {
				scaleDownImage(320, 200, 30, 35 + selectedSlot * 20, g_engine->_engine->aux2);
			}
		}
		gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
		osystem_startFrame();
		process_events();
		flushScreen();
		osystem_drawBackground();

		if (g_engine->_engine->joyD & 1) {
			// up g_engine->_engine->key
			edit = false;
			selectedSlot--;
			if (selectedSlot < 0) {
				selectedSlot = maxSavegameCount - 1;
			}

			if (selectedSlot < 0) {
				selectedSlot = 0;
			}

			if (selectedSlot < static_cast<int>(saveStateList.size())) {
				desc = saveStateList[selectedSlot].getDescription();
			}

			while (!::Engine::shouldQuit() && g_engine->_engine->joyD) {
				process_events();
			}
		}

		if (g_engine->_engine->joyD & 2) {
			// down g_engine->_engine->key
			edit = false;
			selectedSlot++;
			if (selectedSlot >= static_cast<int>(maxSavegameCount)) {
				selectedSlot = 0;
			}

			while (!::Engine::shouldQuit() && g_engine->_engine->joyD) {
				process_events();
			}

			if (selectedSlot < static_cast<int>(saveStateList.size())) {
				desc = saveStateList[selectedSlot].getDescription();
			}
		}

		if (g_engine->_engine->key == 27) {
			return -1;
		}

		if (g_engine->_engine->key == 28 || (!save && g_engine->_engine->click != 0)) {
			// select current entry
			return selectedSlot;
		}

		if (save) {
			if (g_engine->_engine->backspace) {
				edit = true;
				desc.deleteLastChar();

				while (!::Engine::shouldQuit() && g_engine->_engine->backspace) {
					process_events();
				}
			}
			if (g_engine->_engine->character >= 32 && g_engine->_engine->character < 184) {
				if (!edit) {
					edit = true;
					desc.clear();
				}
				if (desc.size() < 32) {
					desc.insertChar(g_engine->_engine->character, desc.size());
					if (extGetSizeFont(desc.c_str()) >= (300 - 140)) {
						desc.deleteLastChar();
					}
				}

				while (!::Engine::shouldQuit() && g_engine->_engine->character) {
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

	if (g_engine->_engine->lightOff) {
		makeBlackPalette();
	}

	// clearScreenSystemMenu(unkScreenVar,g_engine->_engine->aux2);

	int currentSelectedEntry = 0;

	while (!exitMenu && !::Engine::shouldQuit()) {
		AffOptionList(currentSelectedEntry);
		gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
		osystem_startFrame();
		process_events();
		flushScreen();
		osystem_drawBackground();

		if (g_engine->_engine->lightOff) {
			fadeInPhys(0x40, 0);
		}

		//  while(!exitMenu)
		{
			g_engine->_engine->localKey = g_engine->_engine->key;
			g_engine->_engine->localClick = g_engine->_engine->click;
			g_engine->_engine->localJoyD = g_engine->_engine->joyD;

			if (!input5) {
				if (g_engine->_engine->localKey == 0x1C || g_engine->_engine->localClick) // enter
				{
					g_engine->_engine->key &= ~0x1C;
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
							g_engine->_engine->flagInitView = 2;
							unfreezeTime();
							restoreAmbiance();
							return;
						}
						break;
					case 3: // music
						g_engine->_engine->musicEnabled = g_engine->_engine->musicEnabled ? 0 : 1;
						g_engine->_mixer->muteSoundType(Audio::Mixer::kMusicSoundType, g_engine->_engine->musicEnabled ? false : true);
						break;
					case 4: // sound
						g_engine->_engine->soundToggle = g_engine->_engine->soundToggle ? 0 : 1;
						g_engine->_mixer->muteSoundType(Audio::Mixer::kSFXSoundType, g_engine->_engine->soundToggle ? false : true);
						break;
					case 5: // details
						g_engine->_engine->detailToggle = g_engine->_engine->detailToggle ? 0 : 1;
						break;
					case 6: // quit
						::Engine::quitGame();
						break;
					}
				} else {
					if (g_engine->_engine->localKey == 0x1B) {
						exitMenu = 1;
					}
					if (g_engine->_engine->localJoyD == 1) // up
					{
						currentSelectedEntry--;

						if (currentSelectedEntry < 0)
							currentSelectedEntry = 6;

						input5 = 1;
					}
					if (g_engine->_engine->localJoyD == 2) // bottom
					{
						currentSelectedEntry++;

						if (currentSelectedEntry > 6)
							currentSelectedEntry = 0;

						input5 = 1;
					}
				}
			} else {
				if (!g_engine->_engine->localKey && !g_engine->_engine->localJoyD) {
					input5 = 0;
				}
			}
		}

		osystem_flip(nullptr);
	}

	// fadeOut(32,2);
	while ((g_engine->_engine->key || g_engine->_engine->joyD || g_engine->_engine->click) && !::Engine::shouldQuit()) {
		process_events();
	}
	g_engine->_engine->localKey = g_engine->_engine->localClick = g_engine->_engine->localJoyD = 0;
	g_engine->_engine->flagInitView = 2;
	unfreezeTime();
}
} // namespace Fitd
