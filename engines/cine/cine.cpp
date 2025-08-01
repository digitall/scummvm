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

#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/events.h"

#include "backends/keymapper/keymapper.h"

#include "engines/util.h"

#include "graphics/cursorman.h"
#include "graphics/paletteman.h"

#include "image/iff.h"

#include "cine/cine.h"
#include "cine/bg_list.h"
#include "cine/main_loop.h"
#include "cine/object.h"
#include "cine/texte.h"
#include "cine/sound.h"
#include "cine/various.h"

namespace Cine {

#ifdef USE_TTS

// The color names for the copy protection screens are images, so they are transcribed here for TTS
static const char *copyProtectionColorsFWEnglish[] = {
	"White",
	"Yellow",
	"Red",
	"Orange",
	"Black",
	"Blue",
	"Brown",
	"Green"
};

static const char *copyProtectionColorsFWFrench[] = {
	"Blanc",
	"Jaune",
	"Rouge",
	"Orange",
	"Noir",
	"Bleu",
	"Marron",
	"Vert"
};

static const char *copyProtectionColorsFWGerman[] = {
	"Wei\236",
	"Gelb",
	"Rot",
	"Orange",
	"Schwarz",
	"Blau",
	"Braun",
	"Grun"
};

static const char *copyProtectionColorsFWSpanish[] = {
	"Blanco",
	"Amarillo",
	"Rojo",
	"Naranja",
	"Negro",
	"Azul",
	"Marr\242n",
	"Verde"
};

static const char *copyProtectionColorsOSEnglish[] = {
	"Black",
	"White",
	"Yellow",
	"Orange",
	"Red",
	"Brown",
	"Grey",
	"Pink",
	"Purple",
	"Light Blue",
	"Dark Blue",
	"Light Green",
	"Dark Green"
};

static const char *copyProtectionColorsOSFrench[] = {
	"Noir",
	"Blanc",
	"Jaune",
	"Orange",
	"Rouge",
	"Marron",
	"Gris",
	"Rose",
	"Violet",
	"Bleu Clair",
	"Bleu Fonc\202",
	"Vert Clair",
	"Vert Fonc\202"
};

static const char *copyProtectionColorsOSGerman[] = {
	"Schwarz",
	"Wei\236",
	"Gelb",
	"Orange",
	"Rot",
	"Braun",
	"Grau",
	"Pink",
	"Violett",
	"Hellblau",
	"Dunkelblau",
	"Hellgr\201n",
	"Dunkelgr\201n"
};

static const char *copyProtectionColorsOSSpanish[] = {
	"Negro",
	"Blanco",
	"Amarillo",
	"Naranja",
	"Rojo",
	"Marr\242n",
	"Gris",
	"Rosa",
	"Morado",
	"Azul Claro",
	"Azul Oscuro",
	"Verde Claro",
	"Verde Oscuro"
};

static const char *copyProtectionColorsOSItalian[] = {
	"Nero",
    "Bianco",
    "Giallo",
    "Arancione",
    "Rosso",
    "Marrone",
    "Grigio",
    "Rosa",
    "Viola",
    "Blu Chiaro",
    "Blu Scuro",
    "Verde Chiaro",
    "Verde Scuro"
};

static const int kNumOfFWColors = 8;
static const int kNumOfOSColors = 13;

#endif

Sound *g_sound = nullptr;

CineEngine *g_cine = nullptr;

CineEngine::CineEngine(OSystem *syst, const CINEGameDescription *gameDesc)
	: Engine(syst),
	_gameDescription(gameDesc),
	_rnd("cine") {

	// Setup mixer
	syncSoundSettings();

	setDebugger(new CineConsole(this));

	g_cine = this;

	for (int i = 0; i < NUM_FONT_CHARS; i++) {
		_textHandler.fontParamTable[i].characterIdx = 0;
		_textHandler.fontParamTable[i].characterWidth = 0;
	}
	_restartRequested = false;
	_preLoad = false;
	setDefaultGameSpeed();
}

CineEngine::~CineEngine() {
	if (getGameType() == Cine::GType_OS) {
		freeErrmessDat();
	}
}

void CineEngine::syncSoundSettings() {
	Engine::syncSoundSettings();

	bool mute = false;
	if (ConfMan.hasKey("mute"))
		mute = ConfMan.getBool("mute");

	// Use music volume for plain sound types (At least the AdLib player uses a plain sound type
	// so previously the music and sfx volume controls didn't affect it at all).
	// FIXME: Make AdLib player differentiate between playing sound effects and music and remove this.
	_mixer->setVolumeForSoundType(Audio::Mixer::kPlainSoundType,
									mute ? 0 : ConfMan.getInt("music_volume"));
}

Common::Error CineEngine::run() {
	Graphics::ModeList modes;
	modes.push_back(Graphics::Mode(320, 200));
	if (g_cine->getGameType() == GType_FW && (g_cine->getFeatures() & GF_CD)) {
		modes.push_back(Graphics::Mode(640, 480));
		initGraphicsModes(modes);
		showSplashScreen();
	} else {
		initGraphicsModes(modes);
	}

	// Initialize backend
	initGraphics(320, 200);

	if (g_cine->getGameType() == GType_FW && (g_cine->getFeatures() & GF_CD)) {
		if (!existExtractedCDAudioFiles(19)  // tracks <19 are not used
		    && !isDataAndCDAudioReadFromSameCD()) {
			warnMissingExtractedCDAudio();
		}
	}

	if (getPlatform() == Common::kPlatformDOS) {
		g_sound = new PCSound(_mixer, this);
	} else {
		// Paula chipset for Amiga and Atari versions
		g_sound = new PaulaSound(_mixer, this);
	}

	_restartRequested = false;

	Common::TextToSpeechManager *ttsMan = g_system->getTextToSpeechManager();
	if (ttsMan != nullptr) {
		ttsMan->setLanguage(ConfMan.get("language"));
		ttsMan->enable(ConfMan.getBool("tts_enabled"));
	}

	switch (getLanguage()) {
	case Common::EN_ANY:
	case Common::EN_USA:
	case Common::EN_GRB:
		_ttsLanguage = kEnglish;
		break;
	case Common::FR_FRA:
		_ttsLanguage = kFrench;
		break;
	case Common::DE_DEU:
		_ttsLanguage = kGerman;
		break;
	case Common::ES_ESP:
		_ttsLanguage = kSpanish;
		break;
	case Common::IT_ITA:
		_ttsLanguage = kItalian;
		break;
	default:
		_ttsLanguage = kEnglish;
		break;
	}

	_copyProtectionColorScreen = false;
	_copyProtectionTextScreen = false;
	_saveInputMenuOpen = false;

	do {
		initialize();

		_restartRequested = false;

		CursorMan.showMouse(true);
		mainLoop(BOOT_SCRIPT_INDEX);

		delete renderer;
		delete[] collisionPage;
		delete _scriptInfo;
	} while (_restartRequested);

	delete g_sound;

	return Common::kNoError;
}

uint32 CineEngine::getTimerDelay() const {
	return (10923000 * _timerDelayMultiplier) / 1193180;
}

/**
 * Modify game speed
 * @param speedChange Negative values slow game down, positive values speed it up, zero does nothing
 * @return Timer delay multiplier's value after the game speed change
 */
int CineEngine::modifyGameSpeed(int speedChange) {
	// If we want more speed we decrement the timer delay multiplier and vice versa.
	_timerDelayMultiplier = CLIP(_timerDelayMultiplier - speedChange, 1, 50);
	return _timerDelayMultiplier;
}

void CineEngine::setDefaultGameSpeed() {
	_timerDelayMultiplier = 12;
}

void CineEngine::initialize() {
	setTotalPlayTime(0); // Reset total play time
	_globalVars.reinit(NUM_MAX_VAR + 1);

	// Initialize all savegames' descriptions to empty strings
	memset(currentSaveName, 0, sizeof(currentSaveName));

	// Resize object table to its correct size and reset all its elements
	g_cine->_objectTable.resize(NUM_MAX_OBJECT);
	resetObjectTable();

	// Resize animation data table to its correct size and reset all its elements
	g_cine->_animDataTable.resize(NUM_MAX_ANIMDATA);
	freeAnimDataTable();

	// Resize zone data table to its correct size and reset all its elements
	g_cine->_zoneData.resize(NUM_MAX_ZONE);
	Common::fill(g_cine->_zoneData.begin(), g_cine->_zoneData.end(), 0);

	// Resize zone query table to its correct size and reset all its elements
	g_cine->_zoneQuery.resize(NUM_MAX_ZONE);
	Common::fill(g_cine->_zoneQuery.begin(), g_cine->_zoneQuery.end(), 0);

	setDefaultGameSpeed();
	_scriptInfo = setupOpcodes();

	initLanguage(getLanguage());

	if (getGameType() == Cine::GType_OS) {
		renderer = new OSRenderer;
	} else {
		renderer = new FWRenderer;
	}

	renderer->initialize();
	forbidBgPalReload = 0;
	reloadBgPalOnNextFlip = 0;
	gfxFadeOutCompleted = 0;
	gfxFadeInRequested = 0;
	safeControlsLastAccessedMs = 0;
	lastSafeControlObjIdx = -1;
	currentDisk = 1;

	collisionPage = new byte[320 * 200]();

	// Clear part buffer as there's nothing loaded into it yet.
	// Its size will change when loading data into it with the loadPart function.
	g_cine->_partBuffer.clear();

	if (getGameType() == Cine::GType_OS) {
		readVolCnf();
	}

	loadTextData("texte.dat");

	if (getGameType() == Cine::GType_OS && !(getFeatures() & GF_DEMO)) {
		loadPoldatDat("poldat.dat");
		loadErrmessDat("errmess.dat");
	}

	// in case ScummVM engines can be restarted in the future
	g_cine->_scriptTable.clear();
	g_cine->_relTable.clear();
	g_cine->_objectScripts.clear();
	g_cine->_globalScripts.clear();
	g_cine->_bgIncrustList.clear();
	freeAnimDataTable();
	g_cine->_overlayList.clear();
	g_cine->_messageTable.clear();
	resetObjectTable();
	g_cine->_seqList.clear();

	if (getGameType() == Cine::GType_OS) {
		disableSystemMenu = 1;
	} else {
		// WORKAROUND: We do not save this variable in FW's savegames.
		// Initializing this to 1, like we do it in the OS case, will
		// cause the menu disabled when loading from the launcher or
		// command line.
		// A proper fix here would be to save this variable in FW's saves.
		// Since it seems these are unversioned so far, there would be need
		// to properly add versioning to them first.
		//
		// Adding versioning to FW saves didn't solve this problem. Setting
		// disableSystemMenu according to the saved value still caused the
		// action menu (EXAMINE, TAKE, INVENTORY, ...) sometimes to be
		// disabled when it wasn't supposed to be disabled when
		// loading from the launcher or command line.
		disableSystemMenu = 0;
	}

	var8 = 0;
	bgVar0 = 0;
	var2 = var3 = var4 = lastType20OverlayBgIdx = 0;
	musicIsPlaying = 0;
	currentDatName[0] = 0;
	_keyInputList.clear();

	// Used for making sound effects work using Roland MT-32 and AdLib in
	// Operation Stealth after loading a savegame. The sound effects are loaded
	// in AUTO00.PRC using a combination of o2_loadAbs and o2_playSample(1, ...)
	// before o1_freePartRange(0, 200). In the original game AUTO00.PRC
	// was run when starting or restarting the game and one could not load a savegame
	// before passing the copy protection. Thus, we try to emulate that behaviour by
	// running at least part of AUTO00.PRC before loading a savegame.
	//
	// Confirmed that DOS and Atari ST versions do have these commands in their AUTO00.PRC files.
	// Confirmed that Amiga and demo versions do not have these commands in their AUTO00.PRC files.
	if (getGameType() == Cine::GType_OS && !(getFeatures() & GF_DEMO) &&
		(getPlatform() == Common::kPlatformDOS || getPlatform() == Common::kPlatformAtariST)) {
		loadPrc(BOOT_PRC_NAME);
		Common::strcpy_s(currentPrcName, BOOT_PRC_NAME);
		addScriptToGlobalScripts(BOOT_SCRIPT_INDEX);
		runOnlyUntilFreePartRangeFirst200 = true;
		executeGlobalScripts();
	}

	_preLoad = false;
	if (ConfMan.hasKey("save_slot") && !_restartRequested) {
		Common::Error loadError = loadGameState(ConfMan.getInt("save_slot"));

		if (loadError.getCode() == Common::kNoError)
			_preLoad = true;
	}

	if (!_preLoad) {
		loadPrc(BOOT_PRC_NAME);
		Common::strcpy_s(currentPrcName, BOOT_PRC_NAME);
		setMouseCursor(MOUSE_CURSOR_NORMAL);
	}
}

void CineEngine::showSplashScreen() {
	Common::File file;
	if (!file.open("sony.lbm"))
		return;

	Image::IFFDecoder decoder;
	if (!decoder.loadStream(file))
		return;

	const Graphics::Surface *surface = decoder.getSurface();
	if (surface->w == 640 && surface->h == 480) {
		initGraphics(640, 480);

		const Graphics::Palette &palette = decoder.getPalette();
		g_system->getPaletteManager()->setPalette(palette.data(), 0, palette.size());

		g_system->copyRectToScreen(surface->getPixels(), 640, 0, 0, 640, 480);
		g_system->updateScreen();

		Common::EventManager *eventMan = g_system->getEventManager();

		bool done = false;
		uint32 now = g_system->getMillis();

		while (!done && g_system->getMillis() - now < 2000) {
			Common::Keymapper *keymapper = _eventMan->getKeymapper();
			keymapper->getKeymap("intro-shortcuts")->setEnabled(true);

			Common::Event event;
			while (eventMan->pollEvent(event)) {
				if (event.type == Common::EVENT_CUSTOM_ENGINE_ACTION_START && event.customType == kActionExitSonyScreen) {
					done = true;
					break;
				}
				if (shouldQuit())
					done = true;
			}
			keymapper->getKeymap("intro-shortcuts")->setEnabled(false);
		}
	}

	decoder.destroy();
}

void CineEngine::sayText(const Common::String &text, Common::TextToSpeechManager::Action action) {
	Common::TextToSpeechManager *ttsMan = g_system->getTextToSpeechManager();
	// _previousSaid is used to prevent the TTS from looping when sayText is called inside a loop,
	// for example when the cursor stays on a button. Without it when the text ends it would speak
	// the same text again.
	// _previousSaid is cleared when appropriate to allow for repeat requests
	if (ttsMan != nullptr && ConfMan.getBool("tts_enabled") && text != _previousSaid) {
		if (getLanguage() == Common::DE_DEU) {
			// German translation encodes ß as 0x9e, but it's 0xe1 in codepage 850
			Common::String ttsMessage = text;
			ttsMessage.replace('\x9e', '\xe1');
			ttsMan->say(ttsMessage, action, Common::CodePage::kDos850);
		} else if (getLanguage() == Common::FR_FRA && getGameType() == GType_FW) {
			// French translation for Future Wars encodes ê as 0x97, but it's 0x88 in codepage 850
			Common::String ttsMessage = text;
			ttsMessage.replace('\x97', '\x88');
			ttsMan->say(ttsMessage, action, Common::CodePage::kDos850);
		} else {
			ttsMan->say(text, action, Common::CodePage::kDos850);
		}

		_previousSaid = text;
	}
}

void CineEngine::stopTextToSpeech() {
	Common::TextToSpeechManager *ttsMan = g_system->getTextToSpeechManager();
	if (ttsMan != nullptr && ConfMan.getBool("tts_enabled") && ttsMan->isSpeaking()) {
		ttsMan->stop();
		_previousSaid.clear();
	}
}

#ifdef USE_TTS

void CineEngine::mouseOverButton() {
	if (!_copyProtectionTextScreen && !_copyProtectionColorScreen) {
		return;
	}

	uint16 mouseX, mouseY, mouseButton;
	getMouseData(mouseUpdateStatus, &mouseButton, &mouseX, &mouseY);

	int16 objIdx = getObjectUnderCursor(mouseX, mouseY);

	if (objIdx != -1) {
		if (_copyProtectionTextScreen) {
			// Operation Stealth has other objects than just the "Ok" button in this screen
			if (getGameType() == GType_OS && _objectTable[objIdx].frame != 151) {
				return;
			}

			sayText("Ok", Common::TextToSpeechManager::INTERRUPT);
		} else if (_copyProtectionColorScreen) {
			const char **colors;

			if (getGameType() == GType_FW) {
				// The Amiga and Atari versions use a different copy protection screen from the DOS version
				// and don't have these color buttons
				// The only exception is the US Amiga version, which uses the color screen
				if (getPlatform() != Common::kPlatformDOS && getLanguage() != Common::EN_USA) {
					return;
				}

				int16 index = objIdx - 150;

				if (index < 0 || index >= kNumOfFWColors) {
					return;
				}

				switch (_ttsLanguage) {
				case kEnglish:
					colors = copyProtectionColorsFWEnglish;
					break;
				case kFrench:
					colors = copyProtectionColorsFWFrench;
					break;
				case kGerman:
					colors = copyProtectionColorsFWGerman;
					break;
				case kSpanish:
					colors = copyProtectionColorsFWSpanish;
					break;
				default:
					colors = copyProtectionColorsFWEnglish;
					break;
				}
				sayText(colors[index], Common::TextToSpeechManager::INTERRUPT);
			} else {
				int16 index = objIdx - 240;

				if (index < 0 || index >= kNumOfOSColors) {
					return;
				}

				switch (_ttsLanguage) {
				case kEnglish:
					colors = copyProtectionColorsOSEnglish;
					break;
				case kFrench:
					colors = copyProtectionColorsOSFrench;
					break;
				case kItalian:
					colors = copyProtectionColorsOSItalian;
					break;
				case kGerman:
					colors = copyProtectionColorsOSGerman;
					break;
				case kSpanish:
					colors = copyProtectionColorsOSSpanish;
					break;
				default:
					colors = copyProtectionColorsOSEnglish;
					break;
				}
				sayText(colors[index], Common::TextToSpeechManager::INTERRUPT);
			}
		}
	} else {
		_previousSaid.clear();
	}
}

#endif

} // End of namespace Cine
