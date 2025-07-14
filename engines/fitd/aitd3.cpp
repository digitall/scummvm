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

#include "fitd/aitd3.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/gfx.h"
#include "fitd/main_loop.h"
#include "fitd/pak.h"
#include "fitd/startup_menu.h"
#include "fitd/tatou.h"

namespace Fitd {
#define AITD3_CADRE_SPF 0
#define AITD3_ITDFONT 1
#define AITD3_LETTRE 2
#define AITD3_LIVRE 3
#define AITD3_CARNET 4
#define AITD3_CYM00001 5
#define AITD3_CYM00007 6
#define AITD3_CYM00013 7
#define AITD3_CYM00014 8
#define AITD3_CYM01017 9
#define AITD3_CYM03011 10
#define AITD3_CYM13005 11
#define AITD3_CYM13013 12
#define AITD3_MENU3 13
#define AITD3_INVENTAIRE_CAVERNE 14
#define AITD3_INVENTAIRE_COWBOY 15
#define AITD3_INVENTAIRE_COUGUAR 16
#define AITD3_OPTION_SCREEN 17
#define AITD3_SPRITES_INVENTAIRE_CAVERNE 18
#define AITD3_SPRITES_INVENTAIRE_COWBOY 19
#define AITD3_SPRITES_INVENTAIRE_COUGUAR 20

#define MENU_PAL 47
#define MAP 48

#define NBSPRITES 3 /* amount of sprites by inventory */
#define NBDISPLAY 3 /* amount of inventory screens */

#define MENU_MESSAGES 11
#define MESSAGE_LOAD_GAME 46
#define MESSAGE_SAVE_GAME 45
#define START_OPTIONS_INVENTAIRE 23 /* num message.eng */

void aitd3Start(int saveSlot) {
	if (saveSlot == -1) {
		startGame(1, 0, 0);
	}

	while (!::Engine::shouldQuit()) {
		const int startupMenuResult = saveSlot == -1 ? processStartupMenu() : 1;

		switch (startupMenuResult) {
		case -1: // timeout
		{
			startGame(1, 0, 0);

			break;
		}
		case 0: // new game
		{
			startGame(0, 12, 1);

			break;
		}
		case 1: // continue
		{
			if (g_engine->loadGameState(saveSlot != -1 ? saveSlot : 1).getCode() == Common::kNoError) {

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
		default:assert(0);
		}
	}
}

void aitd3DrawInventory() {
	switch (g_engine->_engine->cVars[getCVarsIdx(TYPE_INVENTAIRE)]) {
	case 0:
		pakLoad("ITD_RESS.PAK", AITD3_INVENTAIRE_CAVERNE, g_engine->_engine->logicalScreen);
		break;
	case 1:
		pakLoad("ITD_RESS.PAK", AITD3_INVENTAIRE_COWBOY, g_engine->_engine->logicalScreen);
		break;
	case 2:
		pakLoad("ITD_RESS.PAK", AITD3_INVENTAIRE_COUGUAR, g_engine->_engine->logicalScreen);
		break;
	default:
		assert(0);
	}

	g_engine->_engine->statusLeft = 27;
	g_engine->_engine->statusTop = 100;
	g_engine->_engine->statusRight = 159;
	g_engine->_engine->statusBottom = 174;

	setupCameraProjection((g_engine->_engine->statusRight - g_engine->_engine->statusLeft) / 2 + g_engine->_engine->statusLeft, (g_engine->_engine->statusBottom - g_engine->_engine->statusTop) / 2 + g_engine->_engine->statusTop, 128, 400, 390);
}
} // namespace Fitd
