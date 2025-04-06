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
#include "fitd/aitd2.h"
#include "fitd/aitd3.h"
#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/console.h"
#include "fitd/debugtools.h"
#include "fitd/detection.h"
#include "fitd/file_access.h"
#include "fitd/fitd.h"
#include "fitd/font.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/jack.h"
#include "fitd/music.h"
#include "fitd/pak.h"
#include "fitd/save.h"
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

FitdEngine::FitdEngine(OSystem *syst, const FitdGameDescription *gameDesc) : Engine(syst),
																			 _gameDescription(gameDesc), _randomSource("Fitd") {
	g_engine = this;
}

FitdEngine::~FitdEngine() {
}

FitdGameId FitdEngine::getGameId() const {
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

	if (g_engine->getGameId() == GID_AITD2) {
		loadPak("ITD_RESS.PAK", 59, aux);
	} else {
		loadPak("ITD_RESS.PAK", 3, aux);
	}
	copyPalette((byte *)aux, currentGamePalette);

	copyPalette(currentGamePalette, localPalette);
	//  fadeInSub1(localPalette);

	// to finish
}

int *currentCVarTable = NULL;
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

static void setupScreen(void) {
	logicalScreen = (char *)malloc(64800);

	// screenBufferSize = 64800;

	// unkScreenVar2 = 3;

	// TODO: remain of screen init
}

void allocTextes(void) {
	int currentIndex;
	char *currentPosInTextes;
	int textCounter;
	int stringIndex;
	char *stringPtr;
	int textLength;

	tabTextes = (textEntryStruct *)malloc(NUM_MAX_TEXT_ENTRY * sizeof(textEntryStruct)); // 2000 = 250 * 8

	assert(tabTextes);

	if (!tabTextes) {
		error("TabTextes");
	}

	// setup languageNameString
	// if (g_engine->getGameId() == GID_AITD3) {
	// 	languageNameString = "TEXTES.PAK";
	// } else
	{
		for (int i = 0; i < LANGUAGE_NAME_SIZE; i++) {
			Common::File f;
			if (f.exists(languageNameTable[i])) {
				languageNameString = languageNameTable[i];
				break;
			}
		}
	}

	if (!languageNameString) {
		error("Unable to detect language file..\n");
		assert(0);
	}

	systemTextes = (char *)checkLoadMallocPak(languageNameString, 0); // todo: use real language name
	textLength = getPakSize(languageNameString, 0);

	for (currentIndex = 0; currentIndex < NUM_MAX_TEXT_ENTRY; currentIndex++) {
		tabTextes[currentIndex].index = -1;
		tabTextes[currentIndex].textPtr = NULL;
		tabTextes[currentIndex].width = 0;
	}

	currentPosInTextes = systemTextes;

	textCounter = 0;

	while (currentPosInTextes < systemTextes + textLength) {
		currentIndex = *(currentPosInTextes++);

		if (currentIndex == 26)
			break;

		if (currentIndex == '@') // start of string marker
		{
			stringIndex = 0;

			while ((currentIndex = *(currentPosInTextes++)) >= '0' && currentIndex <= '9') // parse string number
			{
				stringIndex = stringIndex * 10 + currentIndex - 48;
			}

			if (currentIndex == ':') // start of string
			{
				stringPtr = currentPosInTextes;

				do {
					currentPosInTextes++;
				} while ((unsigned char)*(currentPosInTextes - 1) >= ' '); // detect the end of the string

				*(currentPosInTextes - 1) = 0; // add the end of string

				tabTextes[textCounter].index = stringIndex;
				tabTextes[textCounter].textPtr = stringPtr;
				tabTextes[textCounter].width = extGetSizeFont(stringPtr);

				textCounter++;
			}

			if (currentIndex == 26) {
				return;
			}
		}
	}
}

Common::Error FitdEngine::run() {
	if (!g_system->hasFeature(OSystem::kFeatureShadersForGame)) {
		return Common::Error(Common::kUnknownError, "ALone in the Dark requires OpenGL with shaders which is not supported on your system");
	}

	initGraphics3d(320 * 4, 200 * 4);

	#ifdef USE_IMGUI
		ImGuiCallbacks callbacks;
		callbacks.init = onImGuiInit;
		callbacks.render = onImGuiRender;
		callbacks.cleanup = onImGuiCleanup;
		_system->setImGuiCallbacks(callbacks);
	#endif

	switch (getGameId()) {
	case GID_AITD1:
		CVars.resize(45);
		currentCVarTable = AITD1KnownCVars;
		break;
	case GID_JACK:
	case GID_AITD2:
	case GID_AITD3:
		CVars.resize(70);
		currentCVarTable = AITD2KnownCVars;
		break;
	default:
		break;
	}

	gfx_init();

	// Set the engine's debugger console
	setDebugger(new Console());

	setupScreen();

	if (!initMusicDriver()) {
		musicConfigured = 0;
		musicEnabled = 0;
		soundToggle = 0;
		detailToggle = 0;
	}

	aux = (char *)malloc(65068);
	if (!aux) {
		error("Failed to alloc Aux");
	}

	aux2 = (char *)malloc(65068);
	if (!aux2) {
		error("Failed to alloc Aux2");
	}

	initCopyBox(aux2, logicalScreen);
	BufferAnim.resize(NB_BUFFER_ANIM);
	for (int i = 0; i < NB_BUFFER_ANIM; i++) {
		BufferAnim[i].resize(SIZE_BUFFER_ANIM);
	}

	switch (getGameId()) {
	case GID_AITD3: {
		PtrFont = checkLoadMallocPak("ITD_RESS.PAK", 1);
		// Common::File f;
		// f.open("font.bin");
		// int fontSize = f.size();
		// PtrFont = (char *)malloc(fontSize);
		// f.read(PtrFont, fontSize);
		// f.close();
		break;
	}
	case GID_JACK:
	case GID_AITD2: {
		PtrFont = checkLoadMallocPak("ITD_RESS.PAK", 1);
		/*
		int fontSize = getPakSize("ITD_RESS",1);
		FILE* fhandle = fopen("font.bin", "wb+");
		fwrite(fontData, fontSize, 1, fhandle);
		fclose(fhandle);*/
		break;
	}
	case GID_AITD1: {
		PtrFont = checkLoadMallocPak("ITD_RESS.PAK", 5);
		break;
	}
	case GID_TIMEGATE:
		PtrFont = checkLoadMallocPak("ITD_RESS.PAK", 2);
		break;
	default:
		assert(0);
	}

	extSetFont(PtrFont, 14);

	if (getGameId() == GID_AITD1) {
		setFontSpace(2, 0);
	} else {
		setFontSpace(2, 1);
	}

	switch (getGameId()) {
	case GID_JACK:
	case GID_AITD2:
	case GID_AITD3: {
		PtrCadre = checkLoadMallocPak("ITD_RESS.PAK", 0);
		break;
	}
	case GID_AITD1: {
		PtrCadre = checkLoadMallocPak("ITD_RESS.PAK", 4);
		break;
	}
	default:
		break;
	}

	PtrPrioritySample = loadFromItd("PRIORITY.ITD");

	// read cvars definitions
	{
		Common::File f;
		f.open("DEFINES.ITD");
		for (int i = 0; i < CVars.size(); i++) {
			CVars[i] = f.readSint16BE();
		}
		f.close();
	}

	allocTextes();
	listMus = HQR_InitRessource("LISTMUS.PAK", 110000, 40);
	listSamp = HQR_InitRessource(getGameId() == GID_TIMEGATE ? "SAMPLES.PAK" : "LISTSAMP.PAK", 64000, 30);
	HQ_Memory = HQR_Init(10000, 50);

	paletteFill(currentGamePalette, 0, 0, 0);
	loadPalette();

	// If a savegame was selected from the launcher, load it
	int saveSlot = ConfMan.getInt("save_slot");

	switch (getGameId()) {
	case GID_AITD1:
		startAITD1(saveSlot);
		break;
	case GID_JACK:
		startJACK(saveSlot);
		break;
	case GID_AITD2:
		startAITD2(saveSlot);
		break;
	case GID_AITD3:
		startAITD3(saveSlot);
		break;
	// case GID_TIMEGATE:
	// 	startGame(0, 5, 1);
	// 	break;
	default:
		error("Unknown game");
		break;
	}

#ifdef USE_IMGUI
	_system->setImGuiCallbacks(ImGuiCallbacks());
#endif

	return Common::kNoError;
}

Common::Error FitdEngine::loadGameStream(Common::SeekableReadStream *stream) {
	return loadSave(stream) == 1 ? Common::kNoError : Common::kReadingFailed;
}

Common::Error FitdEngine::saveGameStream(Common::WriteStream *stream, bool isAutosave) {
	// Default to returning an error when not implemented
	return makeSave(stream) == 1 ? Common::kNoError : Common::kWritingFailed;
}

} // End of namespace Fitd
