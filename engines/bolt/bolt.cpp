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

struct BltImageHeader {

	static const int SIZE = 0x18;
	BltImageHeader(const byte *src) {
		compression = src[0];
		// FIXME: unknown fields
		offsetX = (int16)READ_BE_UINT16(&src[6]);
		offsetY = (int16)READ_BE_UINT16(&src[8]);
		width = READ_BE_UINT16(&src[0xA]);
		height = READ_BE_UINT16(&src[0xC]);
	}

	byte compression;
	int16 offsetX;
	int16 offsetY;
	uint16 width;
	uint16 height;
};

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
	_cursorImage = _boltlibBltFile.loadShortId(BltShortId(0x9D00));
	BltImageHeader cursorImageHeader(&_cursorImage->getData()[0]);
	// Format is expected to be CLUT7
	_system->setMouseCursor(&_cursorImage->getData()[BltImageHeader::SIZE],
		cursorImageHeader.width, cursorImageHeader.height,
		-cursorImageHeader.offsetX, -cursorImageHeader.offsetY, 0);
	byte cursorColors[3 * 2] = { 0, 0, 0, 0xFF, 0xFF, 0xFF };
	_system->setCursorPalette(cursorColors, 0, 2);
	_system->showMouse(true);

	// Start game
	startGameSequence();

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

void BoltEngine::startGameSequence() {
	_state = StatePtr(new GameSequenceState(this));
}

const BoltEngine::GameSequenceState::SequenceEntry
BoltEngine::GameSequenceState::MERLIN_SEQUENCE[] = {

	{ BoltEngine::GameSequenceState::PlayMovieFunc, MKTAG('B', 'M', 'P', 'R') },
	{ BoltEngine::GameSequenceState::PlayMovieFunc, MKTAG('I', 'N', 'T', 'R') },
	{ BoltEngine::GameSequenceState::MainMenuFunc, 0x0118 }, // main menu
	{ BoltEngine::GameSequenceState::MenuFunc, 0x027A }, // file select
	{ BoltEngine::GameSequenceState::MenuFunc, 0x006B }, // difficulty select
	{ BoltEngine::GameSequenceState::PlayMovieFunc, MKTAG('P', 'L', 'O', 'G') },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x0C31 }, // stage 1 hub
	{ BoltEngine::GameSequenceState::PlayMovieFunc, MKTAG('L', 'A', 'B', 'T') },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x0E41 },
	{ BoltEngine::GameSequenceState::PlayMovieFunc, MKTAG('C', 'A', 'V', '1') },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x0D29 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x0337 }, // stage 1 freeplay hub
	{ BoltEngine::GameSequenceState::MenuFunc, 0x0446 }, // stage 2 freeplay hub
	{ BoltEngine::GameSequenceState::MenuFunc, 0x0555 }, // stage 3 freeplay hub
	{ BoltEngine::GameSequenceState::MenuFunc, 0x900D }, // purple star puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x8C0C }, // stained glass puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x8806 }, // cave puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x8706 }, // ingredients puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x8606 }, // frogs & bugs puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x850B }, // spirits puzzle (?)
	{ BoltEngine::GameSequenceState::MenuFunc, 0x810D }, // gate puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x7D0B }, // solar system puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6817 }, // parchment puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6811 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x680B },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6805 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6717 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6711 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x670B },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6705 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6617 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6611 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x660B },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6605 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6417 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6411 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x640B }, // cave wall puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6405 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6317 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6311 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x630B },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6305 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6217 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6211 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x620B },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6205 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6017 }, // grave puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6011 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x600B },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x6005 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x5F17 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x5F11 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x5F0B },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x5F05 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x5E17 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x5E11 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x5E0B },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x5E05 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x440D }, // spiderweb puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x430D },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x420D },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x400B }, // chest puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x3F0B },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x3E0B },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x3C0B }, // windowsill puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x3B0B },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x3A0B },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x3810 }, // blue cave puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x3710 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x3610 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x340A }, // raven puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x330A },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x320A },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x3009 }, // leaf puzzle
	{ BoltEngine::GameSequenceState::MenuFunc, 0x2F09 },
	{ BoltEngine::GameSequenceState::MenuFunc, 0x2E09 },
	{ BoltEngine::GameSequenceState::PlayMovieFunc, MKTAG('C', 'R', 'D', 'T') },
};

const size_t BoltEngine::GameSequenceState::MERLIN_SEQUENCE_SIZE =
	sizeof(BoltEngine::GameSequenceState::MERLIN_SEQUENCE) /
	sizeof(BoltEngine::GameSequenceState::SequenceEntry);

const BoltEngine::GameSequenceState::SequenceEntry
BoltEngine::GameSequenceState::LABYRINTH_SEQUENCE[] = {

	{ BoltEngine::GameSequenceState::PlayMovieFunc, MKTAG('B', 'M', 'P', 'R') },
	{ BoltEngine::GameSequenceState::PlayMovieFunc, MKTAG('P', 'L', 'O', 'G') },
	{ BoltEngine::GameSequenceState::PlayMovieFunc, MKTAG('C', 'R', 'D', '1') },
	{ BoltEngine::GameSequenceState::PlayMovieFunc, MKTAG('C', 'R', 'D', '2') },
};

const size_t BoltEngine::GameSequenceState::LABYRINTH_SEQUENCE_SIZE =
	sizeof(BoltEngine::GameSequenceState::LABYRINTH_SEQUENCE) /
	sizeof(BoltEngine::GameSequenceState::SequenceEntry);

#if TEST_LABYRINTH

const BoltEngine::GameSequenceState::SequenceEntry
*const BoltEngine::GameSequenceState::SEQUENCE =
BoltEngine::GameSequenceState::LABYRINTH_SEQUENCE;

const size_t BoltEngine::GameSequenceState::SEQUENCE_SIZE =
BoltEngine::GameSequenceState::LABYRINTH_SEQUENCE_SIZE;

#else

const BoltEngine::GameSequenceState::SequenceEntry
*const BoltEngine::GameSequenceState::SEQUENCE =
BoltEngine::GameSequenceState::MERLIN_SEQUENCE;

const size_t BoltEngine::GameSequenceState::SEQUENCE_SIZE =
BoltEngine::GameSequenceState::MERLIN_SEQUENCE_SIZE;

#endif

BoltEngine::GameSequenceState::GameSequenceState(BoltEngine *engine)
: _engine(engine), _cursor(0)
{ }

void BoltEngine::GameSequenceState::process(const Common::Event &event) {
	SEQUENCE[_cursor].func(this);
	_cursor = (_cursor + 1) % SEQUENCE_SIZE; // Restart at end
}

void BoltEngine::GameSequenceState::PlayMovieFunc(GameSequenceState *self) {
	uint32 param = SEQUENCE[self->_cursor].param;
	self->_engine->_state = MovieState::create(self->_engine, param,
		self->_engine->_state);
}

void BoltEngine::GameSequenceState::MainMenuFunc(GameSequenceState *self) {
	BltShortId param(SEQUENCE[self->_cursor].param);
	self->_engine->startMainMenu(param);
}

void BoltEngine::GameSequenceState::MenuFunc(GameSequenceState *self) {
	BltShortId param(SEQUENCE[self->_cursor].param);
	debug(3, "entering menu 0x%.04X", param.value);
	self->_engine->startMenu(BltLongId(param));
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
	assert(_mainMenuRes->getType() == kBltMainMenuInfo);

	BltMainMenuInfo info(&_mainMenuRes->getData()[0]);

	startMenu(info.menuInfoId);
}

void BoltEngine::startMenu(BltLongId menuId) {
	_state = MenuState::create(this, menuId, _state);
}

void BoltEngine::renderBltImageToBack(BltResourcePtr image, int x, int y,
	bool transparency) {

	assert(image->getType() == kBltImage);

	const Common::Array<byte> &data = image->getData();

	BltImageHeader header(&data[0]);

	int topLeftX = x + header.offsetX;
	int topLeftY = y + header.offsetY;

	if (header.compression) {
		_graphics.decodeRL7ToBack(topLeftX, topLeftY, header.width, header.height,
			&data[BltImageHeader::SIZE], data.size() - BltImageHeader::SIZE,
			transparency);
	}
	else {
		_graphics.decodeCLUT7ToBack(topLeftX, topLeftY, header.width, header.height,
			&data[BltImageHeader::SIZE], data.size() - BltImageHeader::SIZE,
			transparency);
	}

	_displayDirty = true;
}

} // End of namespace Bolt
