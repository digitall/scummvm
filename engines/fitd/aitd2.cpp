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

#include "common/scummsys.h"
#include "fitd/aitd2.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/font.h"
#include "fitd/gfx.h"
#include "fitd/inventory.h"
#include "fitd/life.h"
#include "fitd/main_loop.h"
#include "fitd/pak.h"
#include "fitd/sequence.h"
#include "fitd/startup_menu.h"
#include "fitd/tatou.h"

namespace Fitd {
static byte *pAITD2InventorySprite = nullptr;
static int TabXSprite[3] = {127, 118, 124};
static int TabYSprite[3] = {136, 104, 131};

// ITD_RESS mapping
#define AITD2_CADRE_SPF 0
#define AITD2_ITDFONT 1
#define AITD2_LETTRE 2
#define AITD2_LIVRE 3
#define AITD2_CARNET 4
#define AITD2_CYM11001 5
#define AITD2_CYM11011 6
#define AITD2_CYM11012 7
#define AITD2_CYM11013 8
#define AITD2_PRO07011 9
#define AITD2_PRO07012 10
#define AITD2_PRO07013 11
#define AITD2_PRO08001 12
#define AITD2_PRO08008 13
#define AITD2_INVENTAIRE_PIRATE 14
#define AITD2_INVENTAIRE_GANG 15
#define AITD2_INVENTAIRE_GRACE 16
#define AITD2_OPTION_SCREEN 17
#define AITD2_SPRITES_INVENTAIRE 18

int aitd2KnownCVars[] =
	{
		SAMPLE_PAGE,
		BODY_FLAMME,
		MAX_WEIGHT_LOADABLE,
		SAMPLE_CHOC,
		DEAD_PERSO,
		JET_SARBACANE,
		TIR_CANON,
		JET_SCALPEL,
		POIVRE,
		DORTOIR,
		EXT_JACK,
		NUM_MATRICE_PROTECT_1,
		NUM_MATRICE_PROTECT_2,
		NUM_PERSO,
		TYPE_INVENTAIRE,
		PROLOGUE,
		POIGNARD,
		-1};

LifeMacro aitd2LifeMacroTable[] =
	{
		LM_DO_MOVE, // 0
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
		LM_HIT, // 10
		LM_MESSAGE,
		LM_VAR,
		LM_INC,
		LM_DEC,
		LM_ADD,
		LM_SUB,
		LM_LIFE_MODE,
		LM_SWITCH,
		LM_CASE,
		LM_START_CHRONO,
		LM_MULTI_CASE,
		LM_FOUND,
		LM_LIFE,
		LM_DELETE,
		LM_TAKE,
		LM_IN_HAND, // 20
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
		LM_FOUND_LIFE, // 30
		LM_CAMERA_TARGET,
		LM_DROP,
		LM_FIRE,
		LM_TEST_COL,
		LM_FOUND_BODY,
		LM_SET_ALPHA,
		LM_DO_MAX_ZV,
		LM_PUT,
		LM_DO_NORMAL_ZV,
		LM_DO_CARRE_ZV,
		LM_SAMPLE_THEN,
		LM_LIGHT,
		LM_SHAKING,
		LM_INVENTORY,
		LM_FOUND_WEIGHT,
		LM_PUT_AT, // 40
		LM_DEF_ZV,
		LM_HIT_OBJECT,
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
		LM_SAMPLE_THEN_REPEAT,
		LM_WAIT_GAME_OVER,
		LM_GET_MATRICE,
		LM_STAGE_LIFE, // 50
		LM_CONTINUE_TRACK,
		LM_ANIM_RESET,
		LM_RESET_MOVE_MANUAL,
		LM_PLUIE,
		LM_ANIM_HYBRIDE_ONCE,
		LM_ANIM_HYBRIDE_REPEAT,
		LM_MODIF_C_VAR,
		LM_CALL_INVENTORY,
		LM_BODY_RESET,
		LM_DEL_INVENTORY,
		LM_SET_INVENTORY,
		LM_PLAY_SEQUENCE,
		LM_2D_ANIM_SAMPLE,
		LM_SET_GROUND,
		LM_PROTECT,
		LM_DEF_ABS_ZV, // 60
		LM_DEF_SEQUENCE_SAMPLE,
		LM_READ_ON_PICTURE,
		LM_FIRE_UP_DOWN, // AITD3 only

		// TIMEGATE
		LM_DO_ROT_CLUT, // 100
		LM_STOP_CLUT,   // 101
		LM_IF_IN,       // 102
		LM_IF_OUT,      // 103
		LM_INVALID,
		LM_SET_VOLUME_SAMPLE, // 105
		LM_INVALID,
		LM_INVALID,
		LM_FADE_IN_MUSIC,            // 108
		LM_SET_MUSIC_VOLUME,         // 109
		LM_MUSIC_AND_LOOP,           // 110
		LM_MUSIC_THEN,               // 111
		LM_MUSIC_THEN_LOOP,          // 112
		LM_START_FADE_IN_MUSIC,      // 113
		LM_START_FADE_IN_MUSIC_THEN, // 114
		LM_START_FADE_IN_MUSIC_LOOP, // 115
		LM_FADE_OUT_MUSIC_STOP,      // 116
		LM_MUSIC_ALTER_TEMPO,        // 117
		LM_REP_SAMPLE_N_TIME,        // 118
};

void aitd2Start(int saveSlot) {
	fontHeight = 14;
	pAITD2InventorySprite = pakLoad("ITD_RESS.PAK", AITD2_SPRITES_INVENTAIRE);
	assert(pAITD2InventorySprite);

	if (saveSlot == -1) {
		startGame(8, 0, 0); // intro
	}

	while (!::Engine::shouldQuit()) {
		const int startupMenuResult = saveSlot == -1 ? processStartupMenu() : 1;

		switch (startupMenuResult) {
		case -1: // timeout
		{
			break;
		}
		case 0: // new game
		{
			startGame(8, 7, 1);
			break;
		}
		case 1: // continue
		{
			if (g_engine->loadGameState(saveSlot != -1 ? saveSlot : 1).getCode() == Common::kNoError) {
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

void aitd2DrawInventory() {
	switch (g_engine->_engine->cVars[getCVarsIdx(TYPE_INVENTAIRE)]) {
	case 0:
		pakLoad("ITD_RESS.PAK", AITD2_INVENTAIRE_PIRATE, g_engine->_engine->logicalScreen);
		break;
	case 1:
		pakLoad("ITD_RESS.PAK", AITD2_INVENTAIRE_GANG, g_engine->_engine->logicalScreen);
		break;
	case 2:
		pakLoad("ITD_RESS.PAK", AITD2_INVENTAIRE_GRACE, g_engine->_engine->logicalScreen);
		break;
	default:
		assert(0);
	}

	statusLeft = 27;
	statusTop = 100;
	statusRight = 159;
	statusBottom = 174;

	setupCameraProjection((statusRight - statusLeft) / 2 + statusLeft, (statusBottom - statusTop) / 2 + statusTop, 128, 400, 390);
}

void aitd2RedrawInventorySprite() {
	const int inventoryType = g_engine->_engine->cVars[getCVarsIdx(TYPE_INVENTAIRE)];

	affSpfI(TabXSprite[inventoryType], TabYSprite[inventoryType], inventoryType, pAITD2InventorySprite);
}

void aitd2ReadBook(int index, int type) {
	switch (type) {
	case 0: // READ_MESSAGE
	{
		pakLoad("ITD_RESS.PAK", AITD2_LETTRE, g_engine->_engine->aux);
		byte lpalette[0x300];
		copyPalette(g_engine->_engine->aux + 64000, lpalette);
		convertPaletteIfRequired(lpalette);
		copyPalette(lpalette, currentGamePalette);
		gfx_setPalette(lpalette);
		gfx_copyBlockPhys(g_engine->_engine->aux, 0, 0, 320, 200);
		g_engine->_engine->turnPageFlag = 0;
		lire(index, 60, 10, 245, 190, 0, 124, 124);
		break;
	}
	case 1: // READ_BOOK
	{
		pakLoad("ITD_RESS.PAK", AITD2_LIVRE, g_engine->_engine->aux);
		byte lpalette[0x300];
		copyPalette(g_engine->_engine->aux + 64000, lpalette);
		convertPaletteIfRequired(lpalette);
		copyPalette(lpalette, currentGamePalette);
		gfx_setPalette(lpalette);
		gfx_copyBlockPhys(g_engine->_engine->aux, 0, 0, 320, 200);
		g_engine->_engine->turnPageFlag = 1;
		lire(index, 60, 10, 245, 190, 0, 124, 124);
		break;
	}
	case 2: // READ_CARNET
	{
		pakLoad("ITD_RESS.PAK", AITD2_CARNET, g_engine->_engine->aux);
		byte lpalette[0x300];
		copyPalette(g_engine->_engine->aux + 64000, lpalette);
		convertPaletteIfRequired(lpalette);
		copyPalette(lpalette, currentGamePalette);
		gfx_setPalette(lpalette);
		gfx_copyBlockPhys(g_engine->_engine->aux, 0, 0, 320, 200);
		g_engine->_engine->turnPageFlag = 0;
		lire(index, 60, 10, 245, 190, 0, 124, 124);
		break;
	}
	default:
		assert(0);
	}
}
} // namespace Fitd
