/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "bolt/bolt.h"

#include "common/error.h"
#include "common/events.h"
#include "common/system.h"
#include "graphics/palette.h"
#include "gui/message.h"

#include "bolt/menu_card.h"
#include "bolt/movie_card.h"

namespace Bolt {

BoltEngine::BoltEngine(OSystem *syst, const ADGameDescription *gd) :
	Engine(syst) {

}

bool BoltEngine::hasFeature(EngineFeature f) const {
	return
		(f == kSupportsRTL);
}

//#define TEST_LABYRINTH 1

Common::Error BoltEngine::run() {

	_graphics.init(_system);

	// Load BOLTLIB.BLT file
	if (!_boltlibBltFile.init("BOLTLIB.BLT")) {
		return Common::kReadingFailed;
	}

	// Load PF files
#if TEST_LABYRINTH
	if (!_maPfFile.load("Crete/LC1.PF", _system, &_graphics, _mixer)) {
		return Common::kReadingFailed;
	}
#else
	if (!_maPfFile.load("MA.PF")) {
		return Common::kReadingFailed;
	}
#endif

	// Load cursor
	initCursor();

	// Start game
	_eventTime = getTotalPlayTime();
	_resetScheduled = false;
	_advanceScheduled = false;
	_sequenceCursor = 0;
	SEQUENCE[_sequenceCursor].func(this);

	// Main loop
	while (!shouldQuit()) {

		_eventTime = getTotalPlayTime();

		// TODO: Instead of constantly polling for events in a loop, design a
		// function to sleep until event or time-delay occurs. This will make
		// the game more power-efficient.
		Common::Event event;
		if (!_eventMan->pollEvent(event)) {
			event.type = Common::EVENT_INVALID;
		}

		if (event.type == Common::EVENT_MOUSEMOVE) {
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::Hover;
			boltEvent.time = _eventTime;
			boltEvent.point = event.mouse;
			processEvent(boltEvent);
		}
		else if (event.type == Common::EVENT_LBUTTONDOWN) {
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::Click;
			boltEvent.time = _eventTime;
			boltEvent.point = event.mouse;
			processEvent(boltEvent);
		}
		else {
			// Emit "tick" event
			// TODO: Eliminate Tick events in favor of Timer, AudioEnded, and
			// other stuff that can be reacted to instead of polled.
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::Tick;
			boltEvent.time = _eventTime;
			processEvent(boltEvent);
		}

	}

	return Common::kNoError;
}

void BoltEngine::processEvent(const BoltEvent &event) {
	if (event.type == BoltEvent::Hover) {
		// Update cursor
		// TODO: Only update if cursor is visible (there is no way to query
		// system for cursor visibility status)
		scheduleDisplayUpdate();
	}

	// process current card
	if (_currentCard) {
		_currentCard->process(event);
	}

	if (_resetScheduled) {
		_resetScheduled = false;
		_sequenceCursor = 0;
		SEQUENCE[_sequenceCursor].func(this);
	}

	// Use while loop to allow multiple advances before game proceeds
	while (_advanceScheduled) {
		_advanceScheduled = false;
		_sequenceCursor = (_sequenceCursor + 1) % SEQUENCE_SIZE; // restart at end
		SEQUENCE[_sequenceCursor].func(this);
	}

	if (_displayDirty) {
		_graphics.present();
		_displayDirty = false;
	}
}

void BoltEngine::scheduleDisplayUpdate() {
	_displayDirty = true;
}

void BoltEngine::scheduleResetSequence() {
	_resetScheduled = true;
}

void BoltEngine::scheduleAdvanceSequence() {
	_advanceScheduled = true;
}

void BoltEngine::initCursor() {
	static const uint16 kCursorImageId = 0x9D00; // FIXME: Merlin only, different for Crete
	static const byte kCursorPalette[3 * 2] = { 0, 0, 0, 255, 255, 255 };

	_cursorImage.init(&_boltlibBltFile, BltLongId(BltShortId(kCursorImageId)));

	// Format is expected to be CLUT7
	Common::Array<byte> decodedImage;
	decodedImage.resize(_cursorImage.getWidth() * _cursorImage.getHeight());
	::Graphics::Surface surface;
	surface.init(_cursorImage.getWidth(), _cursorImage.getHeight(),
		_cursorImage.getWidth(), &decodedImage[0],
		::Graphics::PixelFormat::createFormatCLUT8());
	_cursorImage.draw(surface, false);

	_system->setMouseCursor(&decodedImage[0],
		_cursorImage.getWidth(), _cursorImage.getHeight(),
		-_cursorImage.getOffset().x, -_cursorImage.getOffset().y, 0);

	_system->setCursorPalette(kCursorPalette, 0, 2);

	_system->showMouse(true);
}

const BoltEngine::SequenceEntry
BoltEngine::MERLIN_SEQUENCE[] = {

	// Pre-game menus
	{ BoltEngine::PlayMovieFunc, MKTAG('B', 'M', 'P', 'R') },
	{ BoltEngine::PlayMovieFunc, MKTAG('I', 'N', 'T', 'R') },
	{ BoltEngine::MainMenuFunc, 0x0118 }, // main menu
	{ BoltEngine::PlayMovieFunc, MKTAG('C', 'R', 'D', 'T') },
	{ BoltEngine::MenuFunc, 0x027A }, // file select
	{ BoltEngine::MenuFunc, 0x006B }, // difficulty select

	// Stage 1: Forest
	{ BoltEngine::PlayMovieFunc, MKTAG('P', 'L', 'O', 'G') },
	{ BoltEngine::MenuFunc, 0x0C31 }, // stage 1 hub
	// NOTE: There are many duplicates of these puzzle scenes, probably
	// for different variants of the puzzle.
	{ BoltEngine::MenuFunc, 0x6017 }, // grave puzzle
	{ BoltEngine::MenuFunc, 0x8606 }, // frogs & bugs puzzle
	{ BoltEngine::MenuFunc, 0x3009 }, // leaf puzzle
	{ BoltEngine::MenuFunc, 0x340A }, // raven puzzle
	{ BoltEngine::MenuFunc, 0x0337 }, // stage 1 freeplay hub

	// Plot Warning
	{ BoltEngine::PlotWarningFunc, 0 },

	// Stage 2: Laboratory
	{ BoltEngine::PlayMovieFunc, MKTAG('L', 'A', 'B', 'T') },
	{ BoltEngine::MenuFunc, 0x0D29 }, // stage 2 hub
	{ BoltEngine::MenuFunc, 0x8706 }, // pots puzzle
	{ BoltEngine::MenuFunc, 0x7D0B }, // solar system puzzle
	{ BoltEngine::MenuFunc, 0x3C0B }, // windowsill puzzle
	{ BoltEngine::MenuFunc, 0x6817 }, // parchment puzzle
	{ BoltEngine::MenuFunc, 0x400B }, // chest puzzle
	{ BoltEngine::MenuFunc, 0x0446 }, // stage 2 freeplay hub

	// Stage 3: Cave
	{ BoltEngine::PlayMovieFunc, MKTAG('C', 'A', 'V', '1') },
	{ BoltEngine::MenuFunc, 0x0E41 }, // stage 3 hub
	{ BoltEngine::MenuFunc, 0x8C0C }, // stained glass puzzle
	{ BoltEngine::MenuFunc, 0x900D }, // purple star puzzle
	{ BoltEngine::MenuFunc, 0x8806 }, // cave memory puzzle
	{ BoltEngine::MenuFunc, 0x850B }, // spirits puzzle (?)
	{ BoltEngine::MenuFunc, 0x810D }, // gate puzzle
	{ BoltEngine::MenuFunc, 0x640B }, // cave wall puzzle
	{ BoltEngine::MenuFunc, 0x440D }, // spiderweb puzzle
	{ BoltEngine::MenuFunc, 0x3810 }, // pink crystal puzzle
	{ BoltEngine::MenuFunc, 0x0555 }, // stage 3 freeplay hub

	// NOTE: I will not watch the finale until the project is done!
	//{ BoltEngine::PlayMovieFunc, MKTAG('F', 'N', 'L', 'E') },
};

const size_t BoltEngine::MERLIN_SEQUENCE_SIZE =
	sizeof(BoltEngine::MERLIN_SEQUENCE) /
	sizeof(BoltEngine::SequenceEntry);

const BoltEngine::SequenceEntry
BoltEngine::LABYRINTH_SEQUENCE[] = {

	{ BoltEngine::PlayMovieFunc, MKTAG('B', 'M', 'P', 'R') },
	{ BoltEngine::PlayMovieFunc, MKTAG('P', 'L', 'O', 'G') },
	{ BoltEngine::PlayMovieFunc, MKTAG('C', 'R', 'D', '1') },
	{ BoltEngine::PlayMovieFunc, MKTAG('C', 'R', 'D', '2') },
};

const size_t BoltEngine::LABYRINTH_SEQUENCE_SIZE =
	sizeof(BoltEngine::LABYRINTH_SEQUENCE) /
	sizeof(BoltEngine::SequenceEntry);

#if TEST_LABYRINTH

const BoltEngine::SequenceEntry
*const BoltEngine::SEQUENCE =
BoltEngine::LABYRINTH_SEQUENCE;

const size_t BoltEngine::SEQUENCE_SIZE =
BoltEngine::LABYRINTH_SEQUENCE_SIZE;

#else

const BoltEngine::SequenceEntry
*const BoltEngine::SEQUENCE =
BoltEngine::MERLIN_SEQUENCE;

const size_t BoltEngine::SEQUENCE_SIZE =
BoltEngine::MERLIN_SEQUENCE_SIZE;

#endif

void BoltEngine::PlayMovieFunc(BoltEngine *self) {
	uint32 param = SEQUENCE[self->_sequenceCursor].param;
	self->_currentCard.reset();
	MovieCard* movieCard = new MovieCard;
	movieCard->init(self, param);
	self->_currentCard.reset(movieCard);

	movieCard->enter();
}

void BoltEngine::MainMenuFunc(BoltEngine *self) {
	BltShortId param(SEQUENCE[self->_sequenceCursor].param);
	self->startMainMenu(param);
}

void BoltEngine::MenuFunc(BoltEngine *self) {
	BltShortId param(SEQUENCE[self->_sequenceCursor].param);
	debug(3, "entering menu 0x%.04X", param.value);
	self->startMenu(BltLongId(param));
}

void BoltEngine::PlotWarningFunc(BoltEngine *self) {
	GUI::MessageDialog dialog(
		"Warning: Puzzles are not implemented. Continuing will spoil the plot.\n"
		"Proceed?", "Yes", "No");
	int result = dialog.runModal();

	// Dialog clobbers cursor; reinitialize the cursor
	self->initCursor();

	if (result == GUI::kMessageOK) {
		// Continue
		self->scheduleAdvanceSequence();
	}
	else {
		// Reset
		self->scheduleResetSequence();
	}
}

struct BltMainMenuInfo {
	BltMainMenuInfo(const byte *src) {
		menuInfoId = BltLongId(READ_BE_UINT32(&src[0]));
		hotspotImageId = BltLongId(READ_BE_UINT32(&src[4]));
		hotspotPaletteId = BltLongId(READ_BE_UINT32(&src[8]));
	}

	BltLongId menuInfoId;
	BltLongId hotspotImageId; // FIXME: correct?
	BltLongId hotspotPaletteId; // FIXME: correct?
};

void BoltEngine::startMainMenu(BltShortId mainMenuId) {
	_mainMenuRes.reset(_boltlibBltFile.loadShortId(mainMenuId, kBltMainMenu));

	BltMainMenuInfo info(&_mainMenuRes[0]);

	startMenu(info.menuInfoId);
}

void BoltEngine::startMenu(BltLongId menuId) {
	_currentCard.reset();
	MenuCard* menuCard = new MenuCard;
	menuCard->init(this, menuId);
	_currentCard.reset(menuCard);

	menuCard->enter();
}

} // End of namespace Bolt
