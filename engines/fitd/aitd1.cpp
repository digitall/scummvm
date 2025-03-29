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
#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/fitd.h"
#include "fitd/gfx.h"
#include "fitd/main_loop.h"
#include "fitd/pak.h"
#include "fitd/save.h"
#include "fitd/startup_menu.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"

namespace Fitd {

int AITD1KnownCVars[] = {
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

int makeIntroScreens(void) {
	char *data;
	unsigned int chrono;

	data = loadPak("ITD_RESS.PAK", AITD1_TITRE);
	fastCopyScreen(data + 770, frontBuffer);
	gfx_copyBlockPhys(frontBuffer, 0, 0, 320, 200);
	fadeInPhys(8, 0);
	memcpy(logicalScreen, frontBuffer, 320 * 200);
	// osystem_flip(NULL);
	free(data);
	loadPak("ITD_RESS.PAK", AITD1_LIVRE, aux);
	startChrono(&chrono);

	osystem_drawBackground();

	do {
		int time;

		process_events();

		time = evalChrono(&chrono);

		if (time >= 0x30)
			break;

	} while (key == 0 && Click == 0);

	playSound(CVars[getCVarsIdx(SAMPLE_PAGE)]);
	/*  LastSample = -1;
	LastPriority = -1;
	LastSample = -1;
	LastPriority = 0; */
	turnPageFlag = 1;
	lire(CVars[getCVarsIdx(TEXTE_CREDITS)] + 1, 48, 2, 260, 197, 1, 26, 0);

	return (0);
}

static void copyBox_Aux_Log(int x1, int y1, int x2, int y2) {
	int i;
	int j;

	for (i = y1; i < y2; i++) {
		for (j = x1; j < x2; j++) {
			*(screenSm3 + i * 320 + j) = *(screenSm1 + i * 320 + j);
		}
	}
}

int choosePerso(void) {
	int choice = 0;
	int firsttime = 1;
	int choiceMade = 0;

	initCopyBox(aux, logicalScreen);

	while (choiceMade == 0) {
		process_events();
		osystem_drawBackground();

		// TODO: missing code for music stop

		loadPak("ITD_RESS.PAK", 10, aux);
		fastCopyScreen(aux, logicalScreen);
		fastCopyScreen(logicalScreen, aux2);

		if (choice == 0) {
			affBigCadre(80, 100, 160, 200);
			copyBox_Aux_Log(10, 10, 149, 190);
		} else {
			affBigCadre(240, 100, 160, 200);
			copyBox_Aux_Log(170, 10, 309, 190);
		}

		fastCopyScreen(logicalScreen, frontBuffer);
		gfx_copyBlockPhys(frontBuffer, 0, 0, 320, 200);

		if (firsttime != 0) {
			fadeInPhys(0x40, 0);

			do {
				process_events();
			} while (Click || key);

			firsttime = 0;
		}

		while ((localKey = key) != 28 && Click == 0) // process input
		{
			process_events();
			osystem_drawBackground();

			if (JoyD & 4) // left
			{
				choice = 0;
				fastCopyScreen(aux2, logicalScreen);
				affBigCadre(80, 100, 160, 200);
				copyBox_Aux_Log(10, 10, 149, 190);
				gfx_copyBlockPhys((unsigned char *)logicalScreen, 0, 0, 320, 200);

				while (JoyD != 0) {
					process_events();
				}
			}

			if (JoyD & 8) // right
			{
				choice = 1;
				fastCopyScreen(aux2, logicalScreen);
				affBigCadre(240, 100, 160, 200);
				copyBox_Aux_Log(170, 10, 309, 190);
				gfx_copyBlockPhys((unsigned char *)logicalScreen, 0, 0, 320, 200);

				while (JoyD != 0) {
					process_events();
				}
			}

			if (localKey == 1) {
				initCopyBox(aux2, logicalScreen);
				fadeOutPhys(0x40, 0);
				return (-1);
			}
		}

		fadeOutPhys(0x40, 0);
		turnPageFlag = 0;

		switch (choice) {
		case 0: {
			fastCopyScreen(frontBuffer, logicalScreen);
			setClip(0, 0, 319, 199);
			loadPak("ITD_RESS.PAK", AITD1_FOND_INTRO, aux);
			copyBox_Aux_Log(160, 0, 319, 199);
			fastCopyScreen(logicalScreen, aux);
			lire(CVars[getCVarsIdx(INTRO_HERITIERE)] + 1, 165, 5, 314, 194, 2, 15, 0);
			CVars[getCVarsIdx(CHOOSE_PERSO)] = 1;
			break;
		}
		case 1: {
			fastCopyScreen(frontBuffer, logicalScreen);
			setClip(0, 0, 319, 199);
			loadPak("ITD_RESS.PAK", AITD1_FOND_INTRO, aux);
			copyBox_Aux_Log(0, 0, 159, 199);
			fastCopyScreen(logicalScreen, aux);
			lire(CVars[getCVarsIdx(INTRO_DETECTIVE)] + 1, 5, 5, 154, 194, 2, 15, 0);
			CVars[getCVarsIdx(CHOOSE_PERSO)] = 0;
			break;
		}
		}

		if (localKey && 0x1C) {
			choiceMade = 1;
		}
	}

	fadeOutPhys(64, 0);
	initCopyBox(aux2, logicalScreen);
	return (choice);
}

void startAITD1() {
	// fontHeight = 16;
	// g_gameUseCDA = true;
	gfx_setPalette(currentGamePalette);

	if (!make3dTatou()) {
		makeIntroScreens();
	}

	while (1) {
		int startupMenuResult = processStartupMenu();
		switch (startupMenuResult) {
		case -1: // timeout
		{
			CVars[getCVarsIdx(CHOOSE_PERSO)] = g_engine->getRandomNumber(0xFFFFFFFF) & 1;
			startGame(7, 1, 0);

			if (!make3dTatou()) {
				if (!makeIntroScreens()) {
					// makeSlideshow();
				}
			}

			break;
		}
		case 0: // new game
		{
			// here, original would ask for protection

			if (choosePerso() != -1) {
				process_events();
				while (key) {
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

			if (restoreSave(12, 0)) {
				// here, original would quit if protection flag was false

				//          updateShaking();

				flagInitView = 2;

				setupCamera();

				mainLoop(1, 1);

				//          freeScene();

				fadeOutPhys(8, 0);
			}

			break;
		}
		case 2: // exit
		{
			freeAll();
			g_engine->quitGame();

			break;
		}
		}
	}
}

} // namespace Fitd
