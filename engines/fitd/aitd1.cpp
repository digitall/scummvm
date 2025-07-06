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

#include "fitd/aitd1.h"
#include "fitd/aitd_box.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/font.h"
#include "fitd/game_time.h"
#include "fitd/gfx.h"
#include "fitd/life.h"
#include "fitd/main_loop.h"
#include "fitd/pak.h"
#include "fitd/startup_menu.h"
#include "fitd/system_menu.h"
#include "fitd/tatou.h"

namespace Fitd {

int aitd1KnownCVars[] = {
	SAMPLE_PAGE,
	BODY_FLAMME,
	MAX_WEIGHT_LOADABLE,
	TEXTE_CREDITS,
	SAMPLE_TONNERRE,
	INTRO_DETECTIVE,
	INTRO_HERITIERE,
	WORLD_NUM_PERSO,
	CHOOSE_PERSO,
	SAMPLE_CHOC,
	SAMPLE_PLOUF,
	REVERSE_OBJECT,
	KILLED_SORCERER,
	LIGHT_OBJECT,
	FOG_FLAG,
	DEAD_PERSO,
	-1};

LifeMacro aitd1LifeMacroTable[] =
	{
		LM_DO_MOVE,
		LM_ANIM_ONCE,
		LM_ANIM_ALL_ONCE,
		LM_BODY,
		LM_IF_EGAL,
		LM_IF_DIFFERENT,
		LM_IF_SUP_EGAL,
		LM_IF_SUP,
		LM_IF_INF_EGAL,
		LM_IF_INF,
		LM_GOTO,
		LM_RETURN,
		LM_END,
		LM_ANIM_REPEAT,
		LM_ANIM_MOVE,
		LM_MOVE,
		LM_HIT,
		LM_MESSAGE,
		LM_MESSAGE_VALUE,
		LM_VAR,
		LM_INC,
		LM_DEC,
		LM_ADD,
		LM_SUB,
		LM_LIFE_MODE,
		LM_SWITCH,
		LM_CASE,
		LM_CAMERA,
		LM_START_CHRONO,
		LM_MULTI_CASE,
		LM_FOUND,
		LM_LIFE,
		LM_DELETE,
		LM_TAKE,
		LM_IN_HAND,
		LM_READ,
		LM_ANIM_SAMPLE,
		LM_SPECIAL,
		LM_DO_REAL_ZV,
		LM_SAMPLE,
		LM_TYPE,
		LM_GAME_OVER,
		LM_MANUAL_ROT,
		LM_RND_FREQ,
		LM_MUSIC,
		LM_SET_BETA,
		LM_DO_ROT_ZV,
		LM_STAGE,
		LM_FOUND_NAME,
		LM_FOUND_FLAG,
		LM_FOUND_LIFE,
		LM_CAMERA_TARGET,
		LM_DROP,
		LM_FIRE,
		LM_TEST_COL,
		LM_FOUND_BODY,
		LM_SET_ALPHA,
		LM_STOP_BETA,
		LM_DO_MAX_ZV,
		LM_PUT,
		LM_C_VAR,
		LM_DO_NORMAL_ZV,
		LM_DO_CARRE_ZV,
		LM_SAMPLE_THEN,
		LM_LIGHT,
		LM_SHAKING,
		LM_INVENTORY,
		LM_FOUND_WEIGHT,
		LM_UP_COOR_Y,
		LM_SPEED,
		LM_PUT_AT,
		LM_DEF_ZV,
		LM_HIT_OBJECT,
		LM_GET_HARD_CLIP,
		LM_ANGLE,
		LM_REP_SAMPLE,
		LM_THROW,
		LM_WATER,
		LM_PICTURE,
		LM_STOP_SAMPLE,
		LM_NEXT_MUSIC,
		LM_FADE_MUSIC,
		LM_STOP_HIT_OBJECT,
		LM_COPY_ANGLE,
		LM_END_SEQUENCE,
		LM_SAMPLE_THEN_REPEAT,
		LM_WAIT_GAME_OVER,
};

static void makeSlideshow() {
	byte backupPalette[768];
	ScopedPtr image;
	uint chrono = 0;
	copyPalette(g_engine->_engine->currentGamePalette, backupPalette);
	flushScreen();
	for (int i = 0; i < 15; i++) {
		if (i == 0) {
			image.reset(pakLoad("ITD_RESS.PAK", AITD1_TITRE));
		} else {
			image.reset(pakLoad("PRESENT.PAK", i));
		}
		paletteFill(g_engine->_engine->currentGamePalette, 0, 0, 0);
		gfx_setPalette(g_engine->_engine->currentGamePalette);
		copyPalette(image.get() + 2, g_engine->_engine->currentGamePalette);
		fastCopyScreen(image.get() + 770, g_engine->_engine->frontBuffer);
		gfx_copyBlockPhys(g_engine->_engine->frontBuffer, 0, 0, 320, 200);
		fadeInPhys(8, 0);
		startChrono(&chrono);

		while (evalChrono(&chrono) < 180) {
			process_events();
		}

		fadeOutPhys(16, 0);
	}
	copyPalette(backupPalette, g_engine->_engine->currentGamePalette);
}

static int makeIntroScreens() {
	{
		ScopedPtr data(pakLoad("ITD_RESS.PAK", AITD1_TITRE));
		fastCopyScreen(data.get() + 770, g_engine->_engine->frontBuffer);
	}

	gfx_copyBlockPhys(g_engine->_engine->frontBuffer, 0, 0, 320, 200);
	fadeInPhys(8, 0);
	memcpy(g_engine->_engine->logicalScreen, g_engine->_engine->frontBuffer, 320 * 200);
	osystem_flip(nullptr);
	pakLoad("ITD_RESS.PAK", AITD1_LIVRE, g_engine->_engine->aux);
	uint chrono;
	startChrono(&chrono);

	osystem_drawBackground();

	do {

		process_events();

		const int time = evalChrono(&chrono);

		if (::Engine::shouldQuit() || time >= 0x30)
			break;

	} while (g_engine->_engine->key == 0 && g_engine->_engine->click == 0);

	playSound(g_engine->_engine->cVars[getCVarsIdx(SAMPLE_PAGE)]);
	/*  LastSample = -1;
	LastPriority = -1;
	LastSample = -1;
	LastPriority = 0; */
	g_engine->_engine->turnPageFlag = 1;
	lire(g_engine->_engine->cVars[getCVarsIdx(TEXTE_CREDITS)] + 1, 48, 2, 260, 197, 1, 26, -1);

	return 0;
}

static void copyBoxAuxLog(int x1, int y1, int x2, int y2) {

	for (int i = y1; i < y2; i++) {
		for (int j = x1; j < x2; j++) {
			*(g_engine->_engine->screenSm3 + i * 320 + j) = *(g_engine->_engine->screenSm1 + i * 320 + j);
		}
	}
}

int choosePerso() {
	int choice = 0;
	int firstTime = 1;
	int choiceMade = 0;

	initCopyBox(g_engine->_engine->aux, g_engine->_engine->logicalScreen);

	while (!::Engine::shouldQuit() && choiceMade == 0) {
		process_events();
		osystem_drawBackground();

		// TODO: missing code for music stop

		pakLoad("ITD_RESS.PAK", 10, g_engine->_engine->aux);
		fastCopyScreen(g_engine->_engine->aux, g_engine->_engine->logicalScreen);
		fastCopyScreen(g_engine->_engine->logicalScreen, g_engine->_engine->aux2);

		if (choice == 0) {
			affBigCadre(80, 100, 160, 200);
			copyBoxAuxLog(10, 10, 149, 190);
		} else {
			affBigCadre(240, 100, 160, 200);
			copyBoxAuxLog(170, 10, 309, 190);
		}

		fastCopyScreen(g_engine->_engine->logicalScreen, g_engine->_engine->frontBuffer);
		gfx_copyBlockPhys(g_engine->_engine->frontBuffer, 0, 0, 320, 200);

		if (firstTime != 0) {
			fadeInPhys(0x40, 0);

			do {
				process_events();
			} while (g_engine->_engine->click || g_engine->_engine->key);

			firstTime = 0;
		}

		while ((!::Engine::shouldQuit() && (g_engine->_engine->localKey = g_engine->_engine->key) != 28) && g_engine->_engine->click == 0) // process input
		{
			process_events();
			osystem_drawBackground();

			if (g_engine->_engine->joyD & 4) // left
			{
				choice = 0;
				fastCopyScreen(g_engine->_engine->aux2, g_engine->_engine->logicalScreen);
				affBigCadre(80, 100, 160, 200);
				copyBoxAuxLog(10, 10, 149, 190);
				gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);

				while (g_engine->_engine->joyD != 0) {
					process_events();
				}
			}

			if (g_engine->_engine->joyD & 8) // right
			{
				choice = 1;
				fastCopyScreen(g_engine->_engine->aux2, g_engine->_engine->logicalScreen);
				affBigCadre(240, 100, 160, 200);
				copyBoxAuxLog(170, 10, 309, 190);
				gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);

				while (g_engine->_engine->joyD != 0) {
					process_events();
				}
			}

			if (g_engine->_engine->localKey == 1) {
				initCopyBox(g_engine->_engine->aux2, g_engine->_engine->logicalScreen);
				fadeOutPhys(0x40, 0);
				return -1;
			}
		}

		fadeOutPhys(0x40, 0);
		g_engine->_engine->turnPageFlag = 0;

		switch (choice) {
		case 0: {
			fastCopyScreen(g_engine->_engine->frontBuffer, g_engine->_engine->logicalScreen);
			setClip(0, 0, 319, 199);
			pakLoad("ITD_RESS.PAK", AITD1_FOND_INTRO, g_engine->_engine->aux);
			copyBoxAuxLog(160, 0, 319, 199);
			fastCopyScreen(g_engine->_engine->logicalScreen, g_engine->_engine->aux);
			lire(g_engine->_engine->cVars[getCVarsIdx(INTRO_HERITIERE)] + 1, 165, 5, 314, 194, 2, 15, 1);
			g_engine->_engine->cVars[getCVarsIdx(CHOOSE_PERSO)] = 1;
			break;
		}
		case 1: {
			fastCopyScreen(g_engine->_engine->frontBuffer, g_engine->_engine->logicalScreen);
			setClip(0, 0, 319, 199);
			pakLoad("ITD_RESS.PAK", AITD1_FOND_INTRO, g_engine->_engine->aux);
			copyBoxAuxLog(0, 0, 159, 199);
			fastCopyScreen(g_engine->_engine->logicalScreen, g_engine->_engine->aux);
			lire(g_engine->_engine->cVars[getCVarsIdx(INTRO_DETECTIVE)] + 1, 5, 5, 154, 194, 2, 15, 0);
			g_engine->_engine->cVars[getCVarsIdx(CHOOSE_PERSO)] = 0;
			break;
		}
		default:
			assert(0);
		}

		if (g_engine->_engine->localKey & 0x1C || g_engine->_engine->click) {
			choiceMade = 1;
		}
	}

	fadeOutPhys(64, 0);
	initCopyBox(g_engine->_engine->aux2, g_engine->_engine->logicalScreen);
	return choice;
}

void aitd1Start(int saveSlot) {
	g_engine->_engine->fontHeight = 16;
	gfx_setPalette(g_engine->_engine->currentGamePalette);

	if (saveSlot == -1 && !make3dTatou() && !::Engine::shouldQuit()) {
		makeIntroScreens();
	}

	while (!::Engine::shouldQuit()) {
		const int startupMenuResult = saveSlot == -1 ? processStartupMenu() : 1;
		switch (startupMenuResult) {
		case -1: // timeout
		{
			g_engine->_engine->cVars[getCVarsIdx(CHOOSE_PERSO)] = g_engine->getRandomNumber(1);
			startGame(7, 1, 0);

			if (!make3dTatou()) {
				if (!makeIntroScreens()) {
					makeSlideshow();
				}
			}

			break;
		}
		case 0: // new game
		{
			// here, original would ask for protection

			if (choosePerso() != -1) {
				process_events();
				while (g_engine->_engine->key) {
					process_events();
				}

				startGame(7, 1, 0);

				// here, original would quit if protection flag was false

				startGame(0, 0, 1);
			}
			break;
		}
		case 1: // continue
		{
			// here, original would ask for protection

			if (saveSlot == -1) {
				freezeTime();
				if (showLoadMenu(12)) {
					setupCamera();
					mainLoop(1, 1);
					//          freeScene();
					fadeOutPhys(8, 0);
				}
			} else if (g_engine->loadGameState(saveSlot).getCode() == Common::kNoError) {
				// here, original would quit if protection flag was false

				restoreAmbiance();

				g_engine->_engine->flagInitView = 2;

				setupCamera();

				mainLoop(1, 1);

				//          freeScene();

				fadeOutPhys(8, 0);
			}

			break;
		}
		case 2: // exit
		{
			::Engine::quitGame();

			break;
		}
		default:
			assert(0);
		}
	}
}

void aitd1ReadBook(int index, int type, int shadow) {
	switch (type) {
	case 0: // READ_MESSAGE
	{
		pakLoad("ITD_RESS.PAK", AITD1_LETTRE, g_engine->_engine->aux);
		g_engine->_engine->turnPageFlag = 0;
		lire(index, 60, 10, 245, 190, 0, 26, shadow);
		break;
	}
	case 1: // READ_BOOK
	{
		pakLoad("ITD_RESS.PAK", AITD1_LIVRE, g_engine->_engine->aux);
		g_engine->_engine->turnPageFlag = 1;
		lire(index, 48, 2, 260, 197, 0, 26, shadow);
		break;
	}
	case 2: // READ_CARNET
	{
		pakLoad("ITD_RESS.PAK", AITD1_CARNET, g_engine->_engine->aux);
		g_engine->_engine->turnPageFlag = 0;
		lire(index, 50, 20, 250, 199, 0, 26, shadow);
		break;
	}
	default:
		assert(0);
	}
}

} // namespace Fitd
