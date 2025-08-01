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
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/events.h"
#include "common/savefile.h"
#include "common/system.h"
#include "common/textconsole.h"
#include "common/text-to-speech.h"

#include "backends/audiocd/audiocd.h"

#include "engines/advancedDetector.h"
#include "engines/util.h"

#include "audio/mixer.h"
#include "audio/decoders/raw.h"

#include "graphics/cursorman.h"
#include "graphics/paletteman.h"
#include "graphics/thumbnail.h"

#include "teenagent/console.h"
#include "teenagent/dialog.h"
#include "teenagent/inventory.h"
#include "teenagent/music.h"
#include "teenagent/objects.h"
#include "teenagent/pack.h"
#include "teenagent/resources.h"
#include "teenagent/scene.h"
#include "teenagent/teenagent.h"

namespace TeenAgent {

TeenAgentEngine *g_engine = nullptr;

TeenAgentEngine::TeenAgentEngine(OSystem *system, const ADGameDescription *gd)
	: Engine(system), _action(kActionNone), _gameDescription(gd), _rnd("teenagent") {
	g_engine = this;

	music = new MusicPlayer(this);
	dialog = new Dialog(this);
	res = new Resources();

	scene = 0;
	inventory = 0;
	_sceneBusy = false;
	_dstObject = 0;
	_musicStream = 0;
	_markDelay = 0;
	_gameDelay = 0;
}

TeenAgentEngine::~TeenAgentEngine() {
	delete dialog;
	dialog = 0;
	delete scene;
	scene = 0;
	delete inventory;
	inventory = 0;
	delete music;
	music = 0;
	_mixer->stopAll();
	_useHotspots.clear();
	delete res;
	res = 0;

	CursorMan.popCursor();
}

bool TeenAgentEngine::trySelectedObject() {
	InventoryObject *inv = inventory->selectedObject();
	if (inv == NULL)
		return false;

	debugC(0, kDebugObject, "checking active object %u on %u", inv->id, _dstObject->id);

	//mouse time challenge hack:
	if ((res->dseg.get_byte(dsAddr_timedCallbackState) == 1 && inv->id == kInvItemRock && _dstObject->id == 5) ||
	    (res->dseg.get_byte(dsAddr_timedCallbackState) == 2 && inv->id == kInvItemSuperGlue && _dstObject->id == 5)) {
		//putting rock into hole or superglue on rock
		fnPutRockInHole();
		return true;
	}

	const Common::Array<UseHotspot> &hotspots = _useHotspots[scene->getId() - 1];
	for (uint i = 0; i < hotspots.size(); ++i) {
		const UseHotspot &spot = hotspots[i];
		if (spot.inventoryId == inv->id && _dstObject->id == spot.objectId) {
			debugC(0, kDebugObject, "use object on hotspot!");
			spot.dump();
			if (spot.actorX != 0xffff && spot.actorY != 0xffff)
				moveTo(spot.actorX, spot.actorY, spot.orientation);
			if (!processCallback(spot.callback))
				debugC(0, kDebugObject, "FIXME: display proper description");
			inventory->resetSelectedObject();
			return true;
		}
	}

	// error
	inventory->resetSelectedObject();
	displayMessage(res->getMessageAddr(kObjErrorMsg)); // "That's no good"
	return true;
}

void TeenAgentEngine::processObject() {
	if (_dstObject == NULL)
		return;

	switch (_action) {
	case kActionExamine: {
		if (trySelectedObject())
			break;

		byte *dcall = res->dseg.ptr(dsAddr_objExamineCallbackTablePtr);
		dcall = res->dseg.ptr(READ_LE_UINT16(dcall + scene->getId() * 2 - 2));
		dcall += 2 * _dstObject->id - 2;
		uint16 callback = READ_LE_UINT16(dcall);
		if (callback == 0 || !processCallback(callback))
			displayMessage(_dstObject->description);
	}
	break;
	case kActionUse: {
		if (trySelectedObject())
			break;

		byte *dcall = res->dseg.ptr(dsAddr_objUseCallbackTablePtr);
		dcall = res->dseg.ptr(READ_LE_UINT16(dcall + scene->getId() * 2 - 2));
		dcall += 2 * _dstObject->id - 2;
		uint16 callback = READ_LE_UINT16(dcall);
		if (!processCallback(callback))
			displayMessage(_dstObject->description);
	}
	break;

	case kActionNone:
	default:
		break;
	}
}

void TeenAgentEngine::use(Object *object) {
	if (object == NULL || scene->eventRunning())
		return;

	_dstObject = object;
	object->rect.dump();
	object->actorRect.dump();

	_action = kActionUse;
	if (object->actorRect.valid())
		scene->moveTo(Common::Point(object->actorRect.right, object->actorRect.bottom), object->actorOrientation);
	else if (object->actorOrientation > 0)
		scene->setOrientation(object->actorOrientation);
}

void TeenAgentEngine::examine(const Common::Point &point, Object *object) {
	if (scene->eventRunning())
		return;

	if (object != NULL) {
		Common::Point dst = object->actorRect.center();
		debugC(0, kDebugObject, "click %d, %d, object %d, %d", point.x, point.y, dst.x, dst.y);
		_action = kActionExamine;
		if (object->actorRect.valid())
			scene->moveTo(dst, object->actorOrientation, true); // validate examine message. Original engine does not let you into walkboxes
		_dstObject = object;
	} else if (!_sceneBusy) {
		// do not reset anything while scene is busy, but allow interrupts while walking.
		debugC(0, kDebugObject, "click %d, %d", point.x, point.y);
		_action = kActionNone;
		scene->moveTo(point, 0, true);
		_dstObject = NULL;
	}
}

void TeenAgentEngine::init() {
	_markDelay = 80;
	_gameDelay = 110;

	_useHotspots.resize(42);
	byte *sceneHotspots = res->dseg.ptr(dsAddr_sceneHotspotsPtr);
	for (byte i = 0; i < 42; ++i) {
		Common::Array<UseHotspot> & hotspots = _useHotspots[i];
		byte *hotspotsPtr = res->dseg.ptr(READ_LE_UINT16(sceneHotspots + i * 2));
		while (*hotspotsPtr) {
			UseHotspot h;
			h.load(hotspotsPtr);
			hotspotsPtr += 9;
			hotspots.push_back(h);
		}
	}
}

Common::Error TeenAgentEngine::loadGameState(int slot) {
	debug(0, "loading from slot %d", slot);
	Common::ScopedPtr<Common::InSaveFile> in(_saveFileMan->openForLoading(
		getSaveStateName(slot)));
	if (!in)
		in.reset(_saveFileMan->openForLoading(Common::String::format("teenagent.%d", slot)));

	if (!in)
		return Common::kReadPermissionDenied;

	assert(res->dseg.size() >= dsAddr_saveState + saveStateSize);

	char *data = (char *)malloc(saveStateSize);
	if (!data)
		error("[TeenAgentEngine::loadGameState] Cannot allocate buffer");

	in->seek(0);
	if (in->read(data, saveStateSize) != saveStateSize) {
		free(data);
		return Common::kReadingFailed;
	}

	memcpy(res->dseg.ptr(dsAddr_saveState), data, saveStateSize);

	free(data);

	uint32 tag = in->readUint32BE();
	if (tag == MKTAG('T', 'H', 'M', 'B')) { // Old save (before TEENAGENT_SAVEGAME_VERSION was added)
		uint16 baseAddr = dsAddr_sceneObjectTablePtr;
		// Copy scene object data in the dseg to sceneObjectsSeg
		Common::copy(res->dseg.ptr(baseAddr), res->dseg.ptr(0xb4f3), res->sceneObjectsSeg.ptr(0));

		// Set correct addresses, i.e., make them relative to dsAddr_sceneObjectTablePtr
		for (byte i = 0; i < 42; i++) {
			uint16 sceneTable = res->dseg.get_word(baseAddr + (i * 2));
			res->sceneObjectsSeg.set_word(i * 2, sceneTable - baseAddr);

			uint16 objectAddr;
			while ((objectAddr = res->dseg.get_word(sceneTable)) != 0) {
				res->sceneObjectsSeg.set_word(sceneTable - baseAddr, objectAddr - baseAddr);
				sceneTable += 2;
			}
			res->sceneObjectsSeg.set_word(sceneTable - baseAddr, 0);
		}
	} else {
		if (tag != MKTAG('T', 'N', 'G', 'T')) {
			warning("loadGameState(): Invalid save file");
			return Common::kUnknownError;
		}

		byte saveVersion = in->readByte();
		if (saveVersion != TEENAGENT_SAVEGAME_VERSION) {
			warning("loadGameState(): Failed to load %d - incorrect version", slot);
			return Common::kUnknownError;
		}

		uint32 resourceSize = in->readUint32LE();
		if (in->read(res->sceneObjectsSeg.ptr(0), resourceSize) != resourceSize) {
			warning("loadGameState(): corrupted data");
			return Common::kReadingFailed;
		}
	}

	scene->clear();
	inventory->activate(false);
	inventory->reload();

	setMusic(res->dseg.get_byte(dsAddr_currentMusic));

	int id = res->dseg.get_byte(dsAddr_currentScene);
	uint16 x = res->dseg.get_word(dsAddr_egoX), y = res->dseg.get_word(dsAddr_egoY);
	scene->loadObjectData();
	scene->init(id, Common::Point(x, y));
	scene->setPalette(4);
	scene->intro = false;
	return Common::kNoError;
}

Common::String TeenAgentEngine::getSaveStateName(int slot) const {
	return Common::String::format("%s.%02d", _targetName.c_str(), slot);
}

Common::Error TeenAgentEngine::saveGameState(int slot, const Common::String &desc, bool isAutosave) {
	debug(0, "saving to slot %d", slot);
	Common::ScopedPtr<Common::OutSaveFile> out(_saveFileMan->openForSaving(
		getSaveStateName(slot)));
	if (!out)
		return Common::kWritingFailed;

	res->dseg.set_byte(dsAddr_currentScene, scene->getId());
	Common::Point pos = scene->getPosition();
	res->dseg.set_word(dsAddr_egoX, pos.x);
	res->dseg.set_word(dsAddr_egoY, pos.y);

	assert(res->dseg.size() >= dsAddr_saveState + saveStateSize);
	// FIXME: Description string is 24 bytes and null based on detection.cpp code, not 22?
	strncpy((char *)res->dseg.ptr(dsAddr_saveState), desc.c_str(), 22);
	out->write(res->dseg.ptr(dsAddr_saveState), saveStateSize);

	// Write tag
	out->writeUint32BE(MKTAG('T', 'N', 'G', 'T'));
	// Write save version
	out->writeByte(TEENAGENT_SAVEGAME_VERSION);

	// Write scene object data
	out->writeUint32LE(res->sceneObjectsSeg.size());
	out->write(res->sceneObjectsSeg.ptr(0), res->sceneObjectsSeg.size());

	if (!Graphics::saveThumbnail(*out))
		warning("saveThumbnail failed");

	out->finalize();
	return Common::kNoError;
}

int TeenAgentEngine::skipEvents() const {
	Common::EventManager *_event = _system->getEventManager();
	Common::Event event;
	while (_event->pollEvent(event)) {
		switch (event.type) {
		case Common::EVENT_QUIT:
		case Common::EVENT_RETURN_TO_LAUNCHER:
			return -1;
		case Common::EVENT_MAINMENU:
		case Common::EVENT_LBUTTONDOWN:
		case Common::EVENT_RBUTTONDOWN:
			return 1;
		case Common::EVENT_KEYDOWN:
			if (event.kbd.ascii)
				return 1;
		default:
			break;
		}
	}
	return 0;
}

bool TeenAgentEngine::showCDLogo() {
	Common::File cdlogo;
	if (!cdlogo.exists("cdlogo.res") || !cdlogo.open("cdlogo.res"))
		return true;

	const uint bgSize = kScreenWidth * kScreenHeight;
	const uint paletteSize = 3 * 256;

	byte *bg = (byte *)malloc(bgSize);
	if (!bg)
		error("[TeenAgentEngine::showCDLogo] Cannot allocate background buffer");

	byte *palette = (byte *)malloc(paletteSize);
	if (!palette) {
		free(bg);
		error("[TeenAgentEngine::showCDLogo] Cannot allocate palette buffer");
	}

	cdlogo.read(bg, bgSize);
	cdlogo.read(palette, paletteSize);

	for (uint c = 0; c < paletteSize; ++c)
		palette[c] *= 4;

	_system->getPaletteManager()->setPalette(palette, 0, 256);
	_system->copyRectToScreen(bg, kScreenWidth, 0, 0, kScreenWidth, kScreenHeight);
	_system->updateScreen();

	free(bg);
	free(palette);

	for (uint i = 0; i < 20; ++i) {
		int r = skipEvents();
		if (r != 0)
			return r > 0 ? true : false;
		_system->delayMillis(100);
	}
	cdlogo.close();

	return true;
}

bool TeenAgentEngine::showLogo() {
	FilePack logo;
	if (!logo.open("unlogic.res"))
		return true;

	Common::ScopedPtr<Common::SeekableReadStream> frame(logo.getStream(1));
	if (!frame)
		return true;

	const uint bgSize = kScreenWidth * kScreenHeight;
	const uint paletteSize = 3 * 256;

	byte *bg = (byte *)malloc(bgSize);
	if (!bg)
		error("[TeenAgentEngine::showLogo] Cannot allocate background buffer");

	byte *palette = (byte *)malloc(paletteSize);
	if (!palette) {
		free(bg);
		error("[TeenAgentEngine::showLogo] Cannot allocate palette buffer");
	}

	frame->read(bg, bgSize);
	frame->read(palette, paletteSize);

	for (uint c = 0; c < paletteSize; ++c)
		palette[c] *= 4;

	_system->getPaletteManager()->setPalette(palette, 0, 256);

	free(palette);

	uint n = logo.fileCount();
	for (uint f = 0; f < 4; ++f)
		for (uint i = 2; i <= n; ++i) {
			{
				int r = skipEvents();
				if (r != 0) {
					free(bg);
					return r > 0 ? true : false;
				}
			}
			_system->copyRectToScreen(bg, kScreenWidth, 0, 0, kScreenWidth, kScreenHeight);

			frame.reset(logo.getStream(i));
			if (!frame) {
				free(bg);
				return true;
			}

			Surface s;
			s.load(*frame, Surface::kTypeOns);
			if (s.empty()) {
				free(bg);
				return true;
			}

			_system->copyRectToScreen(s.getPixels(), s.w, s.x, s.y, s.w, s.h);
			_system->updateScreen();

			_system->delayMillis(100);
		}

	free(bg);

	return true;
}

bool TeenAgentEngine::showMetropolis() {
	_system->fillScreen(0);
	_system->updateScreen();

	FilePack varia;
	varia.open("varia.res");

	const uint paletteSize = 3 * 256;
	byte *palette = (byte *)malloc(paletteSize);
	if (!palette)
		error("[TeenAgentEngine::showMetropolis] Cannot allocate palette buffer");

	{
		Common::ScopedPtr<Common::SeekableReadStream> s(varia.getStream(5));
		s->read(palette, paletteSize);
		for (uint c = 0; c < paletteSize; ++c)
			palette[c] *= 4;
	}

	_system->getPaletteManager()->setPalette(palette, 0, 256);

	free(palette);

	const uint varia6Size = 21760;
	const uint varia9Size = 18302;
	byte *varia6Data = (byte *)malloc(varia6Size);
	byte *varia9Data = (byte *)malloc(varia9Size);
	if (!varia6Data || !varia9Data) {
		free(varia6Data);
		free(varia9Data);

		error("[TeenAgentEngine::showMetropolis] Cannot allocate buffer");
	}

	varia.read(6, varia6Data, varia6Size);
	varia.read(9, varia9Data, varia9Size);

	const uint colorsSize = 56 * 160 * 2;
	byte *colors = (byte *)malloc(colorsSize);
	if (!colors)
		error("[TeenAgentEngine::showMetropolis] Cannot allocate colors buffer");

	memset(colors, 0, colorsSize);

	int logo_y = -56;
	for (uint f = 0; f < 300; ++f) {
		{
			int r = skipEvents();
			if (r != 0) {
				free(varia6Data);
				free(varia9Data);
				free(colors);
				return r > 0 ? true : false;
			}
		}

		if (logo_y > 0) {
			g_system->fillScreen(Common::Rect(0, 0, kScreenWidth, logo_y), 0);
		}

		Graphics::Surface *surface = _system->lockScreen();
		{
			//generate colors matrix
			memmove(colors + 320, colors + 480, 8480);
			for (uint c = 0; c < 17; ++c) {
				byte x = (_rnd.getRandomNumber(184) + 5) & 0xff;
				uint offset = 8800 + _rnd.getRandomNumber(158);
				colors[offset++] = x;
				colors[offset++] = x;
			}
			for (uint y = 1; y < 56; ++y) {
				for (uint x = 1; x < 160; ++x) {
					uint offset = y * 160 + x;
					uint v =
					    (uint)colors[offset - 161] + colors[offset - 160] + colors[offset - 159] +
					    (uint)colors[offset - 1] + colors[offset + 1] +
					    (uint)colors[offset + 161] + colors[offset + 160] + colors[offset + 159];
					v >>= 3;
					colors[offset + 8960] = v;
				}
			}
			memmove(colors, colors + 8960, 8960);
		}

		byte *dst = (byte *)surface->getBasePtr(0, 131);
		byte *src = varia6Data;
		for (uint y = 0; y < 68; ++y) {
			for (uint x = 0; x < 320; ++x) {
				if (*src++ == 1) {
					*dst++ = colors[19 * 160 + y / 2 * 160 + x / 2];
				} else
					++dst;
			}
		}
		_system->unlockScreen();

		_system->copyRectToScreen(
		    varia9Data + (logo_y < 0 ? -logo_y * 320 : 0), 320,
		    0, logo_y >= 0 ? logo_y : 0,
		    320, logo_y >= 0 ? 57 : 57 + logo_y);

		if (logo_y < 82 - 57)
			++logo_y;


		_system->updateScreen();
		_system->delayMillis(100);
	}

	free(varia6Data);
	free(varia9Data);
	free(colors);

	return true;
}

Common::Error TeenAgentEngine::run() {
	const Common::FSNode gameDataDir(ConfMan.getPath("path"));
	SearchMan.addSubDirectoryMatching(gameDataDir, "music");

	if (!res->loadArchives(_gameDescription))
		return Common::kUnknownError;

	Common::EventManager *_event = _system->getEventManager();

	initGraphics(kScreenWidth, kScreenHeight);
	setDebugger(new Console(this));

	scene = new Scene(this);
	inventory = new Inventory(this);

	init();

	CursorMan.pushCursor(res->dseg.ptr(dsAddr_cursor), 8, 12, 0, 0, 1);

	syncSoundSettings();

	Common::TextToSpeechManager *ttsMan = g_system->getTextToSpeechManager();
	if (ttsMan != nullptr) {
		// Currently, the Polish and Czech versions have all text in English
		// Therefore, the TTS language should be English to match the text
		// The Polish CD/Floppy versions also have no working voiceover, so they need full TTS
		if (_gameDescription->language == Common::PL_POL || _gameDescription->language == Common::CS_CZE) {
			ttsMan->setLanguage("en");
		} else {
			ttsMan->setLanguage(ConfMan.get("language"));
		}
		
		ttsMan->enable(ConfMan.getBool("tts_enabled"));
	}

	// Initialize CD audio
	if (_gameDescription->flags & ADGF_CD)
		g_system->getAudioCDManager()->open();

	setMusic(1);
	_mixer->playStream(Audio::Mixer::kMusicSoundType, &_musicHandle, music, -1, Audio::Mixer::kMaxChannelVolume, 0, DisposeAfterUse::NO, false);

	int loadSlot = ConfMan.getInt("save_slot");
	if (loadSlot >= 0) {
		loadGameState(loadSlot);
	} else {
		if (!showCDLogo())
			return Common::kNoError;
		if (!showLogo())
			return Common::kNoError;
		if (!showMetropolis())
			return Common::kNoError;
		scene->intro = true;
		_sceneBusy = true;
		fnIntro();
	}

	CursorMan.showMouse(true);

	uint32 gameTimer = 0;
	uint32 markTimer = 0;

	Common::Event event;
	Common::Point mouse;
	uint32 timer = _system->getMillis();

	do {
		Object *currentObject = scene->findObject(mouse);

		while (_event->pollEvent(event)) {
			if (event.type == Common::EVENT_RETURN_TO_LAUNCHER)
				return Common::kNoError;

			if ((!_sceneBusy && inventory->processEvent(event)) || scene->processEvent(event))
				continue;

			debug(5, "event");
			switch (event.type) {
			case Common::EVENT_CUSTOM_ENGINE_ACTION_START:
				if (event.customType == kActionFastMode) {
					_markDelay = _markDelay == 80 ? 40 : 80;
					debug(5, "markDelay = %u", _markDelay);
				}
				break;
			case Common::EVENT_LBUTTONDOWN:
				if (scene->getId() < 0)
					break;
				examine(event.mouse, currentObject);
				break;
			case Common::EVENT_RBUTTONDOWN:
				if (currentObject)
					debugC(0, kDebugObject, "%d, %s", currentObject->id, currentObject->name.c_str());
				if (scene->getId() < 0)
					break;

				if (currentObject == NULL)
					break;

				if (res->dseg.get_byte(dsAddr_timedCallbackState) == 3 && currentObject->id == 1) {
					fnGuardDrinking();
					break;
				}
				if (res->dseg.get_byte(dsAddr_timedCallbackState) == 4 && currentObject->id == 5) {
					fnGotAnchor();
					break;
				}
				use(currentObject);
				break;
			case Common::EVENT_MOUSEMOVE:
				mouse = event.mouse;
				break;
			default:
				;
			}
		}

		//game delays: slow 16, normal 11, fast 5, crazy 1
		//mark delays: 4 * (3 - hero_speed), normal == 1
		//game delays in 1/100th of seconds
		uint32 newTimer = _system->getMillis();
		uint32 delta = newTimer - timer;
		timer = newTimer;

		bool tickGame = gameTimer <= delta;
		if (tickGame)
			gameTimer = _gameDelay - ((delta - gameTimer) % _gameDelay);
		else
			gameTimer -= delta;

		bool tickMark = markTimer <= delta;
		if (tickMark)
			markTimer = _markDelay - ((delta - markTimer) % _markDelay);
		else
			markTimer -= delta;

		if (tickGame || tickMark) {
			bool b = scene->render(tickGame, tickMark, delta);
			if (!inventory->active() && !b && _action != kActionNone) {
				processObject();
				_action = kActionNone;
				_dstObject = NULL;
			}
			_sceneBusy = b;
		}
		_system->showMouse(scene->getMessage().empty() && !_sceneBusy);

		bool busy = inventory->active() || _sceneBusy;

		Graphics::Surface *surface = _system->lockScreen();

		if (!busy) {
			InventoryObject *selectedObject = inventory->selectedObject();
			if (currentObject || selectedObject) {
				Common::String name;
				if (selectedObject) {
					name += selectedObject->name;
					name += " & ";
				}
				if (currentObject)
					name += currentObject->name;

				sayText(name);

				uint w = res->font7.render(NULL, 0, 0, name, textColorMark);
				res->font7.render(surface, (kScreenWidth - w) / 2, 180, name, textColorMark, true);
#if 0
				if (currentObject) {
					currentObject->rect.render(surface, 0x80);
					currentObject->actorRect.render(surface, 0x81);
				}
#endif
			} else {
				_previousSaid.clear();
			}
		}

		inventory->render(surface, tickGame ? 1 : 0);

		_system->unlockScreen();

		_system->updateScreen();

		uint32 nextTick = MIN(gameTimer, markTimer);
		if (nextTick > 0) {
			_system->delayMillis(nextTick > 40 ? 40 : nextTick);
		}
	} while (!shouldQuit());

	return Common::kNoError;
}

Common::String TeenAgentEngine::parseMessage(uint16 addr) {
	Common::String message;
	for (
	    const char *str = (const char *)res->messagesSeg.ptr(addr);
	    str[0] != 0 || str[1] != 0;
	    ++str) {
		char c = str[0];
		message += c != 0 && (signed char)c != -1 ? c : '\n';
	}
	if (message.empty()) {
		warning("empty message parsed for %04x", addr);
	}
	return message;
}

void TeenAgentEngine::displayMessage(const Common::String &str, CharacterID characterID, uint16 x, uint16 y) {
	if (str.empty()) {
		return;
	}

	if (characterDialogData[characterID].textColor == textColorMark) { // mark's
		SceneEvent e(SceneEvent::kPlayAnimation);
		e.animation = 0;
		e.slot = 0x80;
		scene->push(e);
	}

	{
		SceneEvent event(SceneEvent::kMessage);
		event.message = str;
		event.color = characterDialogData[characterID].textColor;
		event.slot = 0;
		event.dst.x = x;
		event.dst.y = y;
		event.characterID = characterID;
		scene->push(event);
	}

	{
		SceneEvent e(SceneEvent::kPauseAnimation);
		e.animation = 0;
		e.slot = 0x80;
		scene->push(e);
	}
}

void TeenAgentEngine::displayMessage(uint16 addr, CharacterID characterID, uint16 x, uint16 y) {
	displayMessage(parseMessage(addr), characterID, x, y);
}

void TeenAgentEngine::displayAsyncMessage(uint16 addr, uint16 x, uint16 y, uint16 firstFrame, uint16 lastFrame, CharacterID characterID) {
	SceneEvent event(SceneEvent::kMessage);
	event.message = parseMessage(addr);
	event.slot = 0;
	event.color = characterDialogData[characterID].textColor;
	event.dst.x = x;
	event.dst.y = y;
	event.firstFrame = firstFrame;
	event.lastFrame = lastFrame;
	event.characterID = characterID;

	scene->push(event);
}

void TeenAgentEngine::displayAsyncMessageInSlot(uint16 addr, byte slot, uint16 firstFrame, uint16 lastFrame, byte color) {
	SceneEvent event(SceneEvent::kMessage);
	event.message = parseMessage(addr);
	event.slot = slot + 1;
	event.color = color;
	event.firstFrame = firstFrame;
	event.lastFrame = lastFrame;

	scene->push(event);
}

void TeenAgentEngine::displayCredits(uint16 addr, uint16 timer) {
	SceneEvent event(SceneEvent::kCreditsMessage);

	const byte *src = res->creditsSeg.ptr(addr);
	event.orientation = *src++;
	event.color = *src++;
	event.lan = 8;

	event.dst.y = *src;
	while (true) {
		++src; // skip y position
		Common::String line((const char *)src);
		event.message += line;
		src += line.size() + 1;
		if (*src == 0)
			break;
		event.message += "\n";
	}
	int w = res->font8.render(NULL, 0, 0, event.message, textColorCredits);
	event.dst.x = (kScreenWidth - w) / 2;
	event.timer = timer;
	event.characterID = kCreditsText;
	scene->push(event);
}

void TeenAgentEngine::displayCredits() {
	SceneEvent event(SceneEvent::kCredits);
	event.message = parseMessage(res->getCreditAddr(6));
	event.dst.y = kScreenHeight;

	int lines = 1;
	for (uint i = 0; i < event.message.size(); ++i)
		if (event.message[i] == '\n')
			++lines;
	event.dst.x = (kScreenWidth - res->font7.render(NULL, 0, 0, event.message, textColorCredits)) / 2;
	event.timer = 11 * lines - event.dst.y + 22;
	event.characterID = kCreditsText;
	debug(2, "credits = %s", event.message.c_str());
	scene->push(event);
}

void TeenAgentEngine::displayCutsceneMessage(uint16 addr, uint16 x, uint16 y) {
	SceneEvent event(SceneEvent::kCreditsMessage);

	event.message = parseMessage(addr);
	event.dst.x = x;
	event.dst.y = y;
	event.lan = 7;
	event.characterID = kMark;

	scene->push(event);
}

void TeenAgentEngine::moveTo(const Common::Point &dst, byte o, bool warp) {
	moveTo(dst.x, dst.y, o, warp);
}

void TeenAgentEngine::moveTo(Object *obj) {
	moveTo(obj->actorRect.right, obj->actorRect.bottom, obj->actorOrientation);
}

void TeenAgentEngine::moveTo(uint16 x, uint16 y, byte o, bool warp) {
	SceneEvent event(SceneEvent::kWalk);
	event.dst.x = x;
	event.dst.y = y;
	if (o > 4) {
		warning("invalid orientation %d", o);
		o = 0;
	}
	event.orientation = o;
	event.color = warp ? 1 : 0;
	scene->push(event);
}

void TeenAgentEngine::moveRel(int16 x, int16 y, byte o, bool warp) {
	SceneEvent event(SceneEvent::kWalk);
	event.dst.x = x;
	event.dst.y = y;
	event.orientation = o;
	event.color = (warp ? 1 : 0) | 2;
	scene->push(event);
}

void TeenAgentEngine::playAnimation(uint16 id, byte slot, bool async, bool ignore, bool loop) {
	SceneEvent event(SceneEvent::kPlayAnimation);
	event.animation = id;
	event.slot = (slot + 1) | (ignore ? 0x20 : 0) | (loop ? 0x80 : 0);
	scene->push(event);
	if (!async)
		waitAnimation();
}

void TeenAgentEngine::playActorAnimation(uint16 id, bool async, bool ignore) {
	SceneEvent event(SceneEvent::kPlayActorAnimation);
	event.animation = id;
	event.slot = ignore ? 0x20 : 0;
	scene->push(event);
	if (!async)
		waitAnimation();
}

void TeenAgentEngine::loadScene(byte id, const Common::Point &pos, byte o) {
	loadScene(id, pos.x, pos.y, o);
}

void TeenAgentEngine::loadScene(byte id, uint16 x, uint16 y, byte o) {
	if (scene->last_event_type() != SceneEvent::kCreditsMessage)
		fadeOut();

	SceneEvent event(SceneEvent::kLoadScene);
	event.scene = id;
	event.dst.x = x;
	event.dst.y = y;
	event.orientation = o;
	scene->push(event);
	fadeIn();
}

void TeenAgentEngine::enableOn(bool enable) {
	SceneEvent event(SceneEvent::kSetOn);
	event.ons = 0;
	event.color = enable ? 1 : 0;
	scene->push(event);
}

void TeenAgentEngine::setOns(byte id, byte value, byte sceneId) {
	SceneEvent event(SceneEvent::kSetOn);
	event.ons = id + 1;
	event.color = value;
	event.scene = sceneId;
	scene->push(event);
}

void TeenAgentEngine::setLan(byte id, byte value, byte sceneId) {
	if (id == 0)
		error("setting lan 0 is invalid");
	SceneEvent event(SceneEvent::kSetLan);
	event.lan = id;
	event.color = value;
	event.scene = sceneId;
	scene->push(event);
}

void TeenAgentEngine::setFlag(uint16 addr, byte value) {
	SceneEvent event(SceneEvent::kSetFlag);
	event.callback = addr;
	event.color = value;
	scene->push(event);
}

byte TeenAgentEngine::getFlag(uint16 addr) {
	return scene->peekFlagEvent(addr);
}

void TeenAgentEngine::reloadLan() {
	SceneEvent event(SceneEvent::kSetLan);
	event.lan = 0;
	scene->push(event);
}

void TeenAgentEngine::playMusic(byte id) {
	SceneEvent event(SceneEvent::kPlayMusic);
	event.music = id;
	scene->push(event);
}

void TeenAgentEngine::playSound(byte id, byte skipFrames) {
	if (skipFrames > 0)
		--skipFrames;
	SceneEvent event(SceneEvent::kPlaySound);
	event.sound = id;
	event.color = skipFrames;
	scene->push(event);
}

void TeenAgentEngine::enableObject(byte id, byte sceneId) {
	SceneEvent event(SceneEvent::kEnableObject);
	event.object = id + 1;
	event.color = 1;
	event.scene = sceneId;
	scene->push(event);
}

void TeenAgentEngine::disableObject(byte id, byte sceneId) {
	SceneEvent event(SceneEvent::kEnableObject);
	event.object = id + 1;
	event.color = 0;
	event.scene = sceneId;
	scene->push(event);
}

void TeenAgentEngine::hideActor() {
	SceneEvent event(SceneEvent::kHideActor);
	event.color = 1;
	scene->push(event);
}

void TeenAgentEngine::showActor() {
	SceneEvent event(SceneEvent::kHideActor);
	event.color = 0;
	scene->push(event);
}

void TeenAgentEngine::waitAnimation() {
	SceneEvent event(SceneEvent::kWaitForAnimation);
	scene->push(event);
}

void TeenAgentEngine::waitLanAnimationFrame(byte slot, uint16 frame) {
	SceneEvent event(SceneEvent::kWaitLanAnimationFrame);
	if (frame > 0)
		--frame;

	event.slot = slot - 1;
	event.animation = frame;
	scene->push(event);
}

void TeenAgentEngine::setTimerCallback(uint16 addr, uint16 frames) {
	SceneEvent event(SceneEvent::kTimer);
	event.callback = addr;
	event.timer = frames;
	scene->push(event);
}

void TeenAgentEngine::shakeScreen() {
	SceneEvent event(SceneEvent::kEffect);
	scene->push(event);
}

void TeenAgentEngine::fadeIn() {
	SceneEvent event(SceneEvent::kFade);
	event.orientation = 0;
	scene->push(event);
}

void TeenAgentEngine::fadeOut() {
	SceneEvent event(SceneEvent::kFade);
	event.orientation = 1;
	scene->push(event);
}

void TeenAgentEngine::wait(uint16 frames) {
	SceneEvent event(SceneEvent::kWait);
	event.timer = frames * 10;
	scene->push(event);
}

void TeenAgentEngine::playSoundNow(Pack *pack, uint32 id) {
	uint size = pack->getSize(id);
	if (size == 0) {
		warning("skipping invalid sound %u", id);
		return;
	}

	byte *data = (byte *)malloc(size);
	pack->read(id, data, size);
	debug(3, "playing %u samples...", size);

	Audio::AudioStream *stream = Audio::makeRawStream(data, size, 11025, 0);
	_mixer->playStream(Audio::Mixer::kSFXSoundType, &_soundHandle, stream); // dispose is YES by default
}

void TeenAgentEngine::setMusic(byte id) {
	debugC(0, kDebugMusic, "starting music %u", id);

	if (id != 1) // intro music
		res->dseg.set_byte(dsAddr_currentMusic, id);

	if (_gameDescription->flags & ADGF_CD) {
		byte track2cd[] = {7, 2, 0, 9, 3, 6, 8, 10, 4, 5, 11};
		if (id == 0 || id > 11 || track2cd[id - 1] == 0) {
			debugC(0, kDebugMusic, "no cd music for id %u", id);
			return;
		}
		byte track = track2cd[id - 1];
		debugC(0, kDebugMusic, "playing cd track %u", track);
		_system->getAudioCDManager()->play(track, -1, 0, 0);
	} else if (music->load(id))
		music->start();
}

bool TeenAgentEngine::hasFeature(EngineFeature f) const {
	switch (f) {
	case kSupportsReturnToLauncher:
	case kSupportsSubtitleOptions:
	case kSupportsLoadingDuringRuntime:
	case kSupportsSavingDuringRuntime:
		return true;
	default:
		return false;
	}
}

void TeenAgentEngine::sayText(const Common::String &text) {
	Common::TextToSpeechManager *ttsMan = g_system->getTextToSpeechManager();
	// _previousSaid is used to prevent the TTS from looping when sayText calls are inside loops
	if (ttsMan && ConfMan.getBool("tts_enabled") && _previousSaid != text) {
		if (_gameDescription->language == Common::RU_RUS) {
			ttsMan->say(convertCyrillic(text));
		} else {
			ttsMan->say(text, Common::CodePage::kDos850);
		}

		_previousSaid = text;
	}
}

void TeenAgentEngine::stopTextToSpeech() {
	Common::TextToSpeechManager *ttsMan = g_system->getTextToSpeechManager();
	if (ttsMan && ConfMan.getBool("tts_enabled") && ttsMan->isSpeaking()) {
		ttsMan->stop();
		_previousSaid.clear();
	}
}

void TeenAgentEngine::setTTSVoice(CharacterID characterID) const {
	Common::TextToSpeechManager *ttsMan = g_system->getTextToSpeechManager();
	if (ttsMan && ConfMan.getBool("tts_enabled")) {
		Common::Array<int> voices;
		int pitch = 0;
		Common::TTSVoice::Gender gender;

		if (characterDialogData[characterID].male) {
			voices = ttsMan->getVoiceIndicesByGender(Common::TTSVoice::MALE);
			gender = Common::TTSVoice::MALE;
		} else {
			voices = ttsMan->getVoiceIndicesByGender(Common::TTSVoice::FEMALE);
			gender = Common::TTSVoice::FEMALE;
		}

		// If no voice is available for the necessary gender, set the voice to default
		if (voices.empty()) {
			ttsMan->setVoice(0);
		} else {
			int voiceIndex = characterDialogData[characterID].voiceID % voices.size();
			ttsMan->setVoice(voices[voiceIndex]);
		}

		// If no voices are available for this gender, alter the pitch to mimic a voice
		// of the other gender
		if (ttsMan->getVoice().getGender() != gender) {
			if (gender == Common::TTSVoice::MALE) {
				pitch -= 50;
			} else {
				pitch += 50;
			}
		}

		ttsMan->setPitch(pitch);
	}
}

Common::U32String TeenAgentEngine::convertCyrillic(const Common::String &text) const {
	const byte *bytes = (const byte *)text.c_str();
	byte *convertedBytes = new byte[text.size() * 2 + 1];

	int i = 0;
	for (const byte *b = bytes; *b; ++b) {
		if (*b == 0x26) {	// & needs to be converted to и
			convertedBytes[i] = 0xd0;
			convertedBytes[i + 1] = 0xb8;
			i += 2;
			continue;
		}

		if (*b == 0x3e) {	// For ё
			convertedBytes[i] = 0xd1;
			convertedBytes[i + 1] = 0x91;
			i += 2;
			continue;
		}

		if (*b > 0x3f) {
			int translated = *b;

			if (*b > 0x70) {
				translated += 0xd10f;
			} else {
				translated += 0xd04f;
			}

			convertedBytes[i] = (translated >> 8) & 0xff;
			convertedBytes[i + 1] = translated & 0xff;
			i += 2;
		} else {
			convertedBytes[i] = *b;
			i++;
		}
	}

	convertedBytes[i] = 0;

	Common::U32String result((char *)convertedBytes);
	delete[] convertedBytes;

	return result;
}

} // End of namespace TeenAgent
