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
#include "fitd/common.h"
#include "fitd/console.h"
#include "fitd/detection.h"
#include "fitd/fitd.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/pak.h"
#include "fitd/tatou.h"
#include "fitd/unpack.h"
#include "fitd/vars.h"
#include "common/scummsys.h"
#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/events.h"
#include "common/system.h"
#include "common/file.h"
#include "engines/util.h"
#include "graphics/framelimiter.h"

namespace Fitd {

FitdEngine *g_engine;

FitdEngine::FitdEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst),
																		   _gameDescription(gameDesc), _randomSource("Fitd") {
	g_engine = this;
}

FitdEngine::~FitdEngine() {
}

uint32 FitdEngine::getFeatures() const {
	return _gameDescription->flags;
}

Common::String FitdEngine::getGameId() const {
	return _gameDescription->gameId;
}

void waitMs(uint timeMs) {
	uint time = 0;
	// Simple event handling loop
	Common::Event e;
	Graphics::FrameLimiter limiter(g_system, 25);
	while (!g_engine->shouldQuit() && time < timeMs) {
		while (g_system->getEventManager()->pollEvent(e)) {
		}

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		limiter.delayBeforeSwap();

		time += limiter.getLastFrameDuration();
		limiter.startFrame();
	}
}

static void loadPalette(void) {
	unsigned char localPalette[768];

	loadPak("ITD_RESS.PAK", 3, aux);
	copyPalette((byte *)aux, currentGamePalette);

	copyPalette(currentGamePalette, localPalette);
	//  fadeInSub1(localPalette);

	// to finish
}

int* currentCVarTable = NULL;
int getCVarsIdx(enumCVars searchedType) {
	// TODO: optimize by reversing the table....
	for (int i = 0; i < CVars.size(); i++) {
		if (currentCVarTable[i] == -1) {
			assert(0);
		}

		if (currentCVarTable[i] == searchedType)
			return i;
	}

	assert(0);
	return 0;
}

int getCVarsIdx(int searchedType) {
	return getCVarsIdx((enumCVars)searchedType);
}

Common::Error FitdEngine::run() {
	initGraphics3d(320 * 4, 200 * 4);

	CVars.resize(45);
	currentCVarTable = AITD1KnownCVars;

	gfx_init();

	// Set the engine's debugger console
	setDebugger(new Console());

	// If a savegame was selected from the launcher, load it
	int saveSlot = ConfMan.getInt("save_slot");
	if (saveSlot != -1)
		(void)loadGameState(saveSlot);

	aux = (char *)malloc(65068);
	if (!aux) {
		error("Failed to alloc Aux");
	}

	aux2 = (char *)malloc(65068);
	if (!aux2) {
		error("Failed to alloc Aux2");
	}

	// InitCopyBox(aux2,logicalScreen);

	// PtrFont = checkLoadMallocPak("ITD_RESS",5);
	//  ExtSetFont(PtrFont, 14);
	//  SetFontSpace(2,0);
	//  PtrCadre = CheckLoadMallocPak("ITD_RESS",4);
	//  ptrPrioritySample = loadFromItd("PRIORITY.ITD");

	// read cvars definitions
	{
		Common::File f;
		f.open("DEFINES.ITD");
		for (int i = 0; i < CVars.size(); i++) {
		    CVars[i] = f.readSint16BE();
		}
		f.close();
	}

	// allocTextes();
	listMus = HQR_InitRessource("LISTMUS.PAK", 110000, 40);
	listSamp = HQR_InitRessource("LISTSAMP.PAK", 64000, 30);
	HQ_Memory = HQR_Init(10000, 50);

	paletteFill(currentGamePalette, 0, 0, 0);
	loadPalette();

	startAITD1();

	return Common::kNoError;
}

Common::Error FitdEngine::syncGame(Common::Serializer &s) {
	// The Serializer has methods isLoading() and isSaving()
	// if you need to specific steps; for example setting
	// an array size after reading it's length, whereas
	// for saving it would write the existing array's length
	int dummy = 0;
	s.syncAsUint32LE(dummy);

	return Common::kNoError;
}

} // End of namespace Fitd
