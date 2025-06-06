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

#define FORBIDDEN_SYMBOL_EXCEPTION_time_h
#define FORBIDDEN_SYMBOL_EXCEPTION_printf

#include <nds.h>
#include <filesystem.h>

#include "backends/platform/ds/osystem_ds.h"

#include "common/config-manager.h"
#include "common/translation.h"

#include "backends/audiocd/default/default-audiocd.h"
#include "backends/events/default/default-events.h"
#include "backends/fs/devoptab/devoptab-fs-factory.h"
#include "backends/keymapper/hardware-input.h"
#include "backends/mixer/maxmod/maxmod-mixer.h"
#include "backends/mutex/null/null-mutex.h"
#include "backends/saves/default/default-saves.h"
#include "backends/timer/default/default-timer.h"

#include <time.h>

OSystem_DS *OSystem_DS::_instance = NULL;

OSystem_DS::OSystem_DS()
	: _eventSource(NULL), _engineRunning(false), _disableCursorPalette(true),
	_currentState(), _oldState(), _hiresHack(false), _screenChangeID(0),
	_transactionMode(kTransactionNone),
	_paletteDirty(false), _cursorDirty(false), _overlayInGUI(false),
	_pfCLUT8(Graphics::PixelFormat::createFormatCLUT8()),
	_pfABGR1555(Graphics::PixelFormat(2, 5, 5, 5, 1, 0, 5, 10, 15)),
	_callbackTimer(10), _currentTimeMillis(0), _subScreenActive(true)
{
	_instance = this;

	nitroFSInit(NULL);

	DevoptabFilesystemFactory *fsFactory = new DevoptabFilesystemFactory();

	// Disable newlib's buffering, since libfat handles caching.
	fsFactory->configureBuffering(DrivePOSIXFilesystemNode::kBufferingModeDisabled, 0);

	_fsFactory = fsFactory;

}

OSystem_DS::~OSystem_DS() {
}

void timerTickHandler() {
	OSystem_DS *system = OSystem_DS::instance();
	if (system->_callbackTimer > 0) {
		system->_callbackTimer--;
	}
	system->_currentTimeMillis++;
}

void OSystem_DS::initBackend() {
	defaultExceptionHandler();

	ConfMan.setInt("autosave_period", 0);
	ConfMan.setBool("FM_medium_quality", true);

	_eventSource = new DSEventSource();
	_eventManager = new DefaultEventManager(_eventSource);

	_savefileManager = new DefaultSaveFileManager();
	_timerManager = new DefaultTimerManager();
	timerStart(0, ClockDivider_1, (u16)TIMER_FREQ(1000), timerTickHandler);

	_mixerManager = new MaxModMixerManager(11025, 32768);
	_mixerManager->init();

	initGraphics();

	BaseBackend::initBackend();
}

void OSystem_DS::addSysArchivesToSearchSet(Common::SearchSet &s, int priority) {
	s.add("nitro:/", new Common::FSDirectory("nitro:/"), priority);
}

uint32 OSystem_DS::getMillis(bool skipRecord) {
	return _currentTimeMillis;
}

void OSystem_DS::delayMillis(uint msecs) {
	int st = getMillis();

	doTimerCallback();
	if (_mixerManager)
		((MaxModMixerManager *)_mixerManager)->updateAudio();

	while (st + msecs >= getMillis());
}

void OSystem_DS::doTimerCallback(int interval) {
	DefaultTimerManager *tm = (DefaultTimerManager *)getTimerManager();
	if (_callbackTimer <= 0) {
		_callbackTimer += interval;
		tm->handler();
	}
}

void OSystem_DS::getTimeAndDate(TimeDate &td, bool skipRecord) const {
	time_t curTime = time(0);
	struct tm t = *localtime(&curTime);
	td.tm_sec = t.tm_sec;
	td.tm_min = t.tm_min;
	td.tm_hour = t.tm_hour;
	td.tm_mday = t.tm_mday;
	td.tm_mon = t.tm_mon;
	td.tm_year = t.tm_year;
	td.tm_wday = t.tm_wday;
}

Common::MutexInternal *OSystem_DS::createMutex() {
	return new NullMutexInternal();
}

void OSystem_DS::quit() {
}

void OSystem_DS::logMessage(LogMessageType::Type type, const char *message) {
#ifndef DISABLE_TEXT_CONSOLE
	if (type == LogMessageType::kError) {
		printf("\x1b[41m%s\x1b[39m", message);
	} else {
		printf("%s", message);
	}
#endif
}

void OSystem_DS::messageBox(LogMessageType::Type type, const char *message) {
	if (type == LogMessageType::kError) {
#ifdef DISABLE_TEXT_CONSOLE
		consoleDemoInit();
		printf("\x1b[41m%s\x1b[39m", message);
#endif

		printf("\nPress any button to continue\n");

		while(1) {
			swiWaitForVBlank();
			scanKeys();
			if (keysDown()) break;
		}
	}
}

static const Common::HardwareInputTableEntry ndsJoystickButtons[] = {
	{ "JOY_A",              Common::JOYSTICK_BUTTON_A,              _s("A")           },
	{ "JOY_B",              Common::JOYSTICK_BUTTON_B,              _s("B")           },
	{ "JOY_X",              Common::JOYSTICK_BUTTON_X,              _s("X")           },
	{ "JOY_Y",              Common::JOYSTICK_BUTTON_Y,              _s("Y")           },
	{ "JOY_BACK",           Common::JOYSTICK_BUTTON_BACK,           _s("Select")      },
	{ "JOY_START",          Common::JOYSTICK_BUTTON_START,          _s("Start")       },
	{ "JOY_LEFT_SHOULDER",  Common::JOYSTICK_BUTTON_LEFT_SHOULDER,  _s("L")           },
	{ "JOY_RIGHT_SHOULDER", Common::JOYSTICK_BUTTON_RIGHT_SHOULDER, _s("R")           },
	{ "JOY_UP",             Common::JOYSTICK_BUTTON_DPAD_UP,        _s("D-pad Up")    },
	{ "JOY_DOWN",           Common::JOYSTICK_BUTTON_DPAD_DOWN,      _s("D-pad Down")  },
	{ "JOY_LEFT",           Common::JOYSTICK_BUTTON_DPAD_LEFT,      _s("D-pad Left")  },
	{ "JOY_RIGHT",          Common::JOYSTICK_BUTTON_DPAD_RIGHT,     _s("D-pad Right") },
	{ nullptr,              0,                                      nullptr           }
};

static const Common::AxisTableEntry ndsJoystickAxes[] = {
	{ nullptr, 0, Common::kAxisTypeFull, nullptr }
};

const Common::HardwareInputTableEntry ndsMouseButtons[] = {
	{ "MOUSE_LEFT", Common::MOUSE_BUTTON_LEFT, _s("Touch") },
	{ nullptr,      0,                         nullptr     }
};

Common::HardwareInputSet *OSystem_DS::getHardwareInputSet() {
	using namespace Common;

	CompositeHardwareInputSet *inputSet = new CompositeHardwareInputSet();
	// Touch input sends mouse events for now, so we need to declare we have a mouse...
	inputSet->addHardwareInputSet(new MouseHardwareInputSet(ndsMouseButtons));
	inputSet->addHardwareInputSet(new JoystickHardwareInputSet(ndsJoystickButtons, ndsJoystickAxes));

	return inputSet;
}

Common::String OSystem_DS::getSystemLanguage() const {
	switch (PersonalData->language) {
		case 0: return "ja_JP";
		case 1: return "en_US";
		case 2: return "fr_FR";
		case 3: return "de_DE";
		case 4: return "it_IT";
		case 5: return "es_ES";
		case 6: return "zh_CN";
		default: return "en_US";
	}
}

void OSystem_DS::engineInit() {
	_engineRunning = true;

	const Common::ConfigManager::Domain *activeDomain = ConfMan.getActiveDomain();
	assert(activeDomain);

	const Common::String engineId = activeDomain->getValOrDefault("engineid");
	const Common::String gameId = activeDomain->getValOrDefault("gameid");

	if ((engineId == "cine" && gameId == "fw")  ||
	    (engineId == "gob"  && gameId == "lit") ||
	     engineId == "supernova") {
		_hiresHack = true;
	} else {
		_hiresHack = false;
	}
}

void OSystem_DS::engineDone() {
	_engineRunning = false;
	_hiresHack = false;
}
