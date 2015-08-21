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

#include "bolt/menu_state.h"
#include "bolt/movie_state.h"

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
	_displayDirty = true;

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
	resetSequence();

	// Main loop
	while (!shouldQuit()) {
		Common::Event event;
		if (_eventMan->pollEvent(event)) {
			// process event
			if (event.type == Common::EVENT_MOUSEMOVE) {
				// Update cursor
				_displayDirty = true;
			}
		}
		else {
			event.type = Common::EVENT_INVALID;
		}

		// process game
		if (_state) {
			_state->process(event);
		}

		if (_displayDirty) {
			_graphics.present();
			_displayDirty = false;
		}
	}

	return Common::kNoError;
}

void BoltEngine::initCursor() {
	static const uint16 kCursorImageId = 0x9D00;
	static const byte kCursorPalette[3 * 2] = { 0, 0, 0, 0xFF, 0xFF, 0xFF };

	_cursorImage = BltImage::load(&_boltlibBltFile,
		BltLongId(BltShortId(kCursorImageId)));

	// Format is expected to be CLUT7
	Common::Array<byte> decodedImage;
	decodedImage.resize(_cursorImage->getWidth() * _cursorImage->getHeight());
	::Graphics::Surface surface;
	surface.init(_cursorImage->getWidth(), _cursorImage->getHeight(),
		_cursorImage->getWidth(), &decodedImage[0],
		::Graphics::PixelFormat::createFormatCLUT8());
	_cursorImage->draw(surface, false);

	_system->setMouseCursor(&decodedImage[0],
		_cursorImage->getWidth(), _cursorImage->getHeight(),
		-_cursorImage->getOffset().x, -_cursorImage->getOffset().y, 0);

	_system->setCursorPalette(kCursorPalette, 0, 2);

	_system->showMouse(true);
}

void BoltEngine::resetSequence() {
	_sequenceCursor = 0;
	SEQUENCE[_sequenceCursor].func(this);
}

void BoltEngine::endCard() {
	_sequenceCursor = (_sequenceCursor + 1) % SEQUENCE_SIZE; // restart at end
	SEQUENCE[_sequenceCursor].func(this);
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
	{ BoltEngine::MenuFunc, 0x8806 }, // cave puzzle
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
	self->_state.reset();
	MovieState* movieState = new MovieState;
	movieState->init(self, param);
	self->_state.reset(movieState);
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
		self->endCard();
	}
	else {
		// Reset
		self->resetSequence();
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
	_mainMenuRes = _boltlibBltFile.loadShortId(mainMenuId);
	assert(_mainMenuRes->getType() == kBltMainMenu);

	BltMainMenuInfo info(&_mainMenuRes->getData()[0]);

	startMenu(info.menuInfoId);
}

void BoltEngine::startMenu(BltLongId menuId) {
	_state.reset();
	MenuState* menuState = new MenuState;
	menuState->init(this, menuId);
	_state.reset(menuState);
}

} // End of namespace Bolt
