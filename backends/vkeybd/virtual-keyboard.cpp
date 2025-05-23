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

#ifdef ENABLE_VKEYBD

#include "gui/gui-manager.h"
#include "backends/vkeybd/virtual-keyboard.h"

#include "backends/keymapper/keymapper.h"
#include "backends/keymapper/keymap.h"
#include "backends/vkeybd/virtual-keyboard-gui.h"
#include "backends/vkeybd/virtual-keyboard-parser.h"
#include "backends/vkeybd/keycode-descriptions.h"
#include "common/config-manager.h"
#include "common/textconsole.h"
#include "common/compression/unzip.h"

#define KEY_START_CHAR ('[')
#define KEY_END_CHAR (']')

namespace Common {

VirtualKeyboard::VirtualKeyboard() :
		_currentMode(nullptr),
		_fileArchive(nullptr, DisposeAfterUse::NO) {
	assert(g_system);
	_system = g_system;

	_parser = new VirtualKeyboardParser(this);
	_kbdGUI = new VirtualKeyboardGUI(this);
	_submitKeys = _loaded = false;
}

VirtualKeyboard::~VirtualKeyboard() {
	deleteEvents();
	delete _kbdGUI;
	delete _parser;
}

void VirtualKeyboard::deleteEvents() {
	for (auto &mode : _modes) {
		for (auto &event : mode._value.events)
			delete event._value;
	}
}

void VirtualKeyboard::reset() {
	deleteEvents();
	_modes.clear();
	_initialMode = _currentMode = 0;
	_hAlignment = kAlignCenter;
	_vAlignment = kAlignBottom;
	_keyQueue.clear();
	_loaded = false;
	_kbdGUI->reset();
}

bool VirtualKeyboard::openPack(const String &packName, Archive *searchPath, DisposeAfterUse::Flag disposeSearchPath) {
	Common::Path xmlPackName(packName), zipPackName(packName);
	xmlPackName.appendInPlace(".xml");
	zipPackName.appendInPlace(".zip");

	if (searchPath->hasFile(xmlPackName)) {
		_fileArchive.reset(searchPath, disposeSearchPath);

		// uncompressed keyboard pack
		if (!_parser->loadStream(searchPath->createReadStreamForMember(xmlPackName))) {
			_fileArchive.reset();
			return false;
		}

		return true;
	}

	if (searchPath->hasFile(zipPackName)) {
		// compressed keyboard pack
		Archive *zip = makeZipArchive(searchPath->createReadStreamForMember(zipPackName));
		_fileArchive.reset(zip, DisposeAfterUse::YES);
		if (_fileArchive && _fileArchive->hasFile(xmlPackName)) {
			if (!_parser->loadStream(_fileArchive->createReadStreamForMember(xmlPackName))) {
				_fileArchive.reset();
				return false;
			}
		} else {
			warning("Could not find %s.xml file in %s.zip virtual keyboard pack", packName.c_str(), packName.c_str());
			_fileArchive.reset();
			return false;
		}

		return true;
	}

	return false;
}

bool VirtualKeyboard::loadKeyboardPack(const String &packName) {
	_kbdGUI->initSize(_system->getOverlayWidth(), _system->getOverlayHeight());

	_fileArchive.reset();
	_loaded = false;

	bool opened = false;
	if (ConfMan.hasKey("vkeybdpath"))
		opened = openPack(packName, new FSDirectory(ConfMan.getPath("vkeybdpath")), DisposeAfterUse::YES);
	else if (ConfMan.hasKey("extrapath"))
		opened = openPack(packName, new FSDirectory(ConfMan.getPath("extrapath")), DisposeAfterUse::YES);

	// fallback to SearchMan
	if (!opened)
		opened = openPack(packName, &SearchMan, DisposeAfterUse::NO);

	if (opened) {
		_parser->setParseMode(VirtualKeyboardParser::kParseFull);
		_loaded = _parser->parse();

		if (_loaded) {
			debug("Virtual keyboard pack '%s' loaded successfully", packName.c_str());
		} else {
			warning("Error parsing the virtual keyboard pack '%s'", packName.c_str());

			_fileArchive.reset();
		}
	} else {
		warning("Virtual keyboard disabled due to missing pack file");
	}

	return _loaded;
}

bool VirtualKeyboard::checkModeResolutions() {
	_parser->setParseMode(VirtualKeyboardParser::kParseCheckResolutions);
	_loaded = _parser->parse();
	if (_currentMode)
		_kbdGUI->initMode(_currentMode);
	return _loaded;
}

String VirtualKeyboard::findArea(int16 x, int16 y) {
	return _currentMode->imageMap.findMapArea(x, y);
}

void VirtualKeyboard::processAreaClick(const String &area) {
	if (!_currentMode->events.contains(area))
		return;

	VKEvent *evt = _currentMode->events[area];

	switch (evt->type) {
	case kVKEventKey:
		// add virtual keypress to queue
		_keyQueue.insertKey(*(KeyState *)evt->data);
		break;
	case kVKEventModifier:
		_keyQueue.toggleFlags(*(byte *)(evt->data));
		break;
	case kVKEventSwitchMode:
		// switch to new mode
		switchMode((char *)evt->data);
		_keyQueue.clearFlags();
		break;
	case kVKEventSubmit:
		close(true);
		break;
	case kVKEventCancel:
		close(false);
		break;
	case kVKEventClear:
		_keyQueue.clear();
		break;
	case kVKEventDelete:
		_keyQueue.deleteKey();
		break;
	case kVKEventMoveLeft:
		_keyQueue.moveLeft();
		break;
	case kVKEventMoveRight:
		_keyQueue.moveRight();
		break;
	}
}

void VirtualKeyboard::switchMode(Mode *newMode) {
	_kbdGUI->initMode(newMode);
	_currentMode = newMode;
}

void VirtualKeyboard::switchMode(const String &newMode) {
	if (!_modes.contains(newMode)) {
		warning("Virtual keyboard mode '%s' unknown", newMode.c_str());
	} else {
		switchMode(&_modes[newMode]);
	}
}

void VirtualKeyboard::handleMouseDown(int16 x, int16 y) {
	_areaDown = findArea(x, y);
	if (_areaDown.empty())
		_kbdGUI->startDrag(x, y);
}

void VirtualKeyboard::handleMouseUp(int16 x, int16 y) {
	if (!_areaDown.empty() && _areaDown == findArea(x, y)) {
		processAreaClick(_areaDown);
		_areaDown.clear();
	}
	_kbdGUI->endDrag();
}

// If no GUI opened before the virtual keyboard, kKeymapTypeGui is not yet initialized
// Check and do it if needed
void VirtualKeyboard::initKeymap() {
	using namespace Common;

	Keymapper *mapper = _system->getEventManager()->getKeymapper();

	// Do not try to recreate same keymap over again
	if (mapper->getKeymap(kGuiKeymapName) != 0)
			return;

	Keymap *guiMap = g_gui.getKeymap();
	mapper->addGlobalKeymap(guiMap);
}

void VirtualKeyboard::show() {
	if (!_loaded) {
		debug(1, "VirtualKeyboard::show() - Virtual keyboard not loaded");
		return;
	} else {
		_kbdGUI->checkScreenChanged();
	}

	switchMode(_initialMode);

	{
		initKeymap();
		KeymapTypeEnabler guiKeymap(_system->getEventManager()->getKeymapper(), Keymap::kKeymapTypeGui);
		_kbdGUI->run();
	}

	if (_submitKeys) {
		EventManager *eventMan = _system->getEventManager();
		assert(eventMan);

		// push keydown & keyup events into the event manager
		Event evt;
		while (!_keyQueue.empty()) {
			evt.kbd = _keyQueue.pop();
			evt.type = EVENT_KEYDOWN;
			eventMan->pushEvent(evt);
			evt.type = EVENT_KEYUP;
			eventMan->pushEvent(evt);
		}
	} else {
		_keyQueue.clear();
	}
}

void VirtualKeyboard::close(bool submit) {
	_submitKeys = submit;
	_kbdGUI->close();
}

bool VirtualKeyboard::isDisplaying() {
	return _kbdGUI->isDisplaying();
}

VirtualKeyboard::KeyPressQueue::KeyPressQueue() {
	_keyPos = _keys.end();
	_strPos = 0;
	_strChanged = false;
	_flags = 0;
}

void VirtualKeyboard::KeyPressQueue::toggleFlags(byte fl) {
	_flags ^= fl;
	_flagsStr.clear();
	if (_flags) {
		_flagsStr = KEY_START_CHAR;
		if (_flags & KBD_CTRL)
			_flagsStr += "Ctrl+";
		if (_flags & KBD_ALT)
			_flagsStr += "Alt+";
		if (_flags & KBD_SHIFT)
			_flagsStr += "Shift+";
	}
	_strChanged = true;
}

void VirtualKeyboard::KeyPressQueue::clearFlags() {
	_flags = 0;
	_flagsStr.clear();
	_strChanged = true;
}

void VirtualKeyboard::KeyPressQueue::insertKey(KeyState key) {
	_strChanged = true;
	key.flags ^= _flags;
	if ((key.keycode >= KEYCODE_a) && (key.keycode <= KEYCODE_z))
		key.ascii = (key.flags & KBD_SHIFT) ? key.keycode - 32 : key.keycode;
	clearFlags();

	String keyStr;
	if (key.flags & KBD_CTRL) keyStr += "Ctrl+";
	if (key.flags & KBD_ALT) keyStr += "Alt+";

	if (key.ascii >= 32 && key.ascii <= 255) {
		if (key.flags & KBD_SHIFT && (key.ascii < 65 || key.ascii > 90))
			keyStr += "Shift+";
		keyStr += (char)key.ascii;
	} else {
		if (key.flags & KBD_SHIFT) keyStr += "Shift+";
		if (key.keycode >= 0 && key.keycode < keycodeDescTableSize)
			keyStr += keycodeDescTable[key.keycode];
	}

	if (keyStr.empty()) keyStr += "???";

	_keysStr.insertChar(KEY_START_CHAR, _strPos++);
	const char *k = keyStr.c_str();
	while (char ch = *k++)
		_keysStr.insertChar(ch, _strPos++);
	_keysStr.insertChar(KEY_END_CHAR, _strPos++);

	VirtualKeyPress kp;
	kp.key = key;
	kp.strLen = keyStr.size() + 2;
	_keys.insert(_keyPos, kp);
}

void VirtualKeyboard::KeyPressQueue::deleteKey() {
	if (_keyPos == _keys.begin())
		return;
	KeyPressList::iterator it = _keyPos;
	it--;
	_strPos -= it->strLen;
	while ((it->strLen)-- > 0)
		_keysStr.deleteChar(_strPos);
	_keys.erase(it);
	_strChanged = true;
}

void VirtualKeyboard::KeyPressQueue::moveLeft() {
	if (_keyPos == _keys.begin())
		return;
	_keyPos--;
	_strPos -= _keyPos->strLen;
	_strChanged = true;
}

void VirtualKeyboard::KeyPressQueue::moveRight() {
	if (_keyPos == _keys.end())
		return;
	_strPos += _keyPos->strLen;
	_keyPos++;
	_strChanged = true;
}

KeyState VirtualKeyboard::KeyPressQueue::pop() {
	bool front = (_keyPos == _keys.begin());
	VirtualKeyPress kp = *(_keys.begin());
	_keys.pop_front();

	if (front)
		_keyPos = _keys.begin();
	else
		_strPos -= kp.strLen;

	while (kp.strLen-- > 0)
		_keysStr.deleteChar(0);

	return kp.key;
}

void VirtualKeyboard::KeyPressQueue::clear() {
	_keys.clear();
	_keyPos = _keys.end();
	_keysStr.clear();
	_strPos = 0;
	clearFlags();
	_strChanged = true;
}

bool VirtualKeyboard::KeyPressQueue::empty() {
	return _keys.empty();
}

String VirtualKeyboard::KeyPressQueue::getString() {
	if (_keysStr.empty())
		return _flagsStr;
	if (_flagsStr.empty())
		return _keysStr;
	if (_strPos == _keysStr.size())
		return _keysStr + _flagsStr;

	uint len = _keysStr.size() + _flagsStr.size();
	char *str = new char[len];
	memcpy(str, _keysStr.c_str(), _strPos);
	memcpy(str + _strPos, _flagsStr.c_str(), _flagsStr.size());
	memcpy(str + _strPos + _flagsStr.size(), _keysStr.c_str() + _strPos, _keysStr.size() - _strPos);
	String ret(str, len);
	delete[] str;
	return ret;
}

uint VirtualKeyboard::KeyPressQueue::getInsertIndex() {
	return _strPos + _flagsStr.size();
}

bool VirtualKeyboard::KeyPressQueue::hasStringChanged() {
	bool ret = _strChanged;
	_strChanged = false;
	return ret;
}

} // End of namespace Common

#endif // #ifdef ENABLE_VKEYBD
