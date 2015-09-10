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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "bolt/merlin.h"

#include "common/system.h"
#include "gui/message.h"

#include "bolt/bolt.h"
#include "bolt/menu_card.h"

namespace Bolt {

MerlinEngine::MerlinEngine()
	: _engine(nullptr)
{ }

void MerlinEngine::init(BoltEngine *engine) {
	_engine = engine;

	// Load BOLTLIB.BLT file
	_boltlibBltFile.init("BOLTLIB.BLT"); // TODO: error handling

	// Load PF files
	_maPfFile.load("MA.PF"); // TODO: error handling
	_helpPfFile.load("HELP.PF");

	// Load cursor
	initCursor();

	// Start sequence
	resetSequence();
}

void MerlinEngine::processEvent(const BoltEvent &event) {
	// If a movie is playing, play it over anything else
	if (_movie.isRunning()) {
		// Click to stop movie
		if (event.type == BoltEvent::Click) {
			_movie.stop();
		}
		else {
			_movie.process();
		}
		if (!_movie.isRunning()) {
			// Re-enter current card
			if (_currentCard) {
				_currentCard->enter();
			}
		}
	}
	else if (_currentCard) {
		// Process current card
		if (_currentCard->processEvent(event) == Card::Ended) {
			advanceSequence();
		}
	}
	else {
		advanceSequence();
	}
}

void MerlinEngine::initCursor() {
	static const uint16 kCursorImageId = 0x9D00;
	static const byte kCursorPalette[3 * 2] = { 0, 0, 0, 255, 255, 255 };

	BltImage cursorImage;
	cursorImage.init(_boltlibBltFile, BltLongId(BltShortId(kCursorImageId)));

	// Format is expected to be CLUT7
	Common::Array<byte> decodedImage;
	decodedImage.resize(cursorImage.getWidth() * cursorImage.getHeight());
	::Graphics::Surface surface;
	surface.init(cursorImage.getWidth(), cursorImage.getHeight(),
		cursorImage.getWidth(), &decodedImage[0],
		::Graphics::PixelFormat::createFormatCLUT8());
	cursorImage.draw(surface, false);

	_engine->_system->setMouseCursor(&decodedImage[0],
		cursorImage.getWidth(), cursorImage.getHeight(),
		-cursorImage.getOffset().x, -cursorImage.getOffset().y, 0);

	_engine->_system->setCursorPalette(kCursorPalette, 0, 2);

	_engine->_system->showMouse(true);
}

void MerlinEngine::resetSequence() {
	_sequenceCursor = 0;
	SEQUENCE[_sequenceCursor].func(this, SEQUENCE[_sequenceCursor].param);
}

void MerlinEngine::advanceSequence() {
	// advance sequence
	_sequenceCursor = (_sequenceCursor + 1) % SEQUENCE_SIZE; // reset at end
	SEQUENCE[_sequenceCursor].func(this, SEQUENCE[_sequenceCursor].param);
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

class MerlinMainMenuCard : public MenuCard {
public:
	MerlinMainMenuCard();
	void init(MerlinEngine *merlin, BltFile &bltFile, BltLongId id);
protected:
	Status processButtonClick(int num);
private:
	MerlinEngine *_merlin;
};

MerlinMainMenuCard::MerlinMainMenuCard()
	: _merlin(nullptr)
{ }

void MerlinMainMenuCard::init(MerlinEngine *merlin, BltFile &bltFile, BltLongId id) {
	_merlin = merlin;

	BltResource mainMenuRes(bltFile.loadLongId(id, kBltMainMenu));
	BltMainMenuInfo info(&mainMenuRes[0]);
	MenuCard::init(merlin->_engine, bltFile, info.menuInfoId);
}

Card::Status MerlinMainMenuCard::processButtonClick(int num) {
	switch (num) {
	case -1: // No button
		return None;
	case 0: // Play
		return Ended;
	case 1: // Credits
		_merlin->startMovie(_merlin->_maPfFile, MKTAG('C', 'R', 'D', 'T'));
		return None;
	case 4: // Tour
		_merlin->startMovie(_merlin->_maPfFile, MKTAG('T', 'O', 'U', 'R'));
		return None;
	default:
		warning("unknown main menu button %d", num);
		return None;
	}
}

void MerlinEngine::startMainMenu(BltLongId id) {
	_currentCard.reset();

	MerlinMainMenuCard* card = new MerlinMainMenuCard;
	_currentCard.reset(card);
	card->init(this, _boltlibBltFile, id);
	card->enter();
}

void MerlinEngine::startMenu(BltLongId id) {
	_currentCard.reset();

	GenericMenuCard* menuCard = new GenericMenuCard;
	_currentCard.reset(menuCard);
	menuCard->init(_engine, _boltlibBltFile, id);
	menuCard->enter();
}

void MerlinEngine::startMovie(PfFile &pfFile, uint32 name) {
	_movie.stop();
	_movie.load(_engine, pfFile, name);
	_movie.process();
}

void MerlinEngine::PlayMovieFunc(MerlinEngine *self, uint32 param) {
	self->_currentCard.reset();
	self->startMovie(self->_maPfFile, param);
}

void MerlinEngine::MainMenuFunc(MerlinEngine *self, uint32 param) {
	self->startMainMenu(BltLongId(BltShortId(param)));
}

void MerlinEngine::MenuFunc(MerlinEngine *self, uint32 param) {
	self->startMenu(BltLongId(BltShortId(param)));
}

void MerlinEngine::PlotWarningFunc(MerlinEngine *self, uint32 param) {
	GUI::MessageDialog dialog(
		"Warning: Puzzles are not implemented. Continuing will spoil the plot.\n"
		"Proceed?", "Yes", "No");
	int result = dialog.runModal();

	// Reinitialize cursor because dialog clobbers it
	self->initCursor();

	if (result == GUI::kMessageOK) {
		// Continue
		self->advanceSequence();
	}
	else {
		// Reset
		self->resetSequence();
	}
}

const MerlinEngine::SequenceEntry
MerlinEngine::SEQUENCE[] = {

	// Pre-game menus
	{ MerlinEngine::PlayMovieFunc, MKTAG('B', 'M', 'P', 'R') },
	{ MerlinEngine::PlayMovieFunc, MKTAG('I', 'N', 'T', 'R') },
	{ MerlinEngine::MainMenuFunc, 0x0118 }, // main menu
	{ MerlinEngine::MenuFunc, 0x027A }, // file select
	{ MerlinEngine::MenuFunc, 0x006B }, // difficulty select

	// Stage 1: Forest
	{ MerlinEngine::PlayMovieFunc, MKTAG('P', 'L', 'O', 'G') },
	{ MerlinEngine::MenuFunc, 0x0C31 }, // stage 1 hub
	// NOTE: There are many duplicates of these puzzle scenes, probably
	// for different variants of the puzzle.
	{ MerlinEngine::MenuFunc, 0x6017 }, // grave puzzle
	{ MerlinEngine::MenuFunc, 0x8606 }, // frogs & bugs puzzle
	{ MerlinEngine::MenuFunc, 0x3009 }, // leaf puzzle
	{ MerlinEngine::MenuFunc, 0x340A }, // raven puzzle
	{ MerlinEngine::MenuFunc, 0x0337 }, // stage 1 freeplay hub

	// Plot Warning
	{ MerlinEngine::PlotWarningFunc, 0 },

	// Stage 2: Laboratory
	{ MerlinEngine::PlayMovieFunc, MKTAG('L', 'A', 'B', 'T') },
	{ MerlinEngine::MenuFunc, 0x0D29 }, // stage 2 hub
	{ MerlinEngine::MenuFunc, 0x8706 }, // pots puzzle
	{ MerlinEngine::MenuFunc, 0x7D0B }, // solar system puzzle
	{ MerlinEngine::MenuFunc, 0x3C0B }, // windowsill puzzle
	{ MerlinEngine::MenuFunc, 0x6817 }, // parchment puzzle
	{ MerlinEngine::MenuFunc, 0x400B }, // chest puzzle
	{ MerlinEngine::MenuFunc, 0x0446 }, // stage 2 freeplay hub

	// Stage 3: Cave
	{ MerlinEngine::PlayMovieFunc, MKTAG('C', 'A', 'V', '1') },
	{ MerlinEngine::MenuFunc, 0x0E41 }, // stage 3 hub
	{ MerlinEngine::MenuFunc, 0x8C0C }, // stained glass puzzle
	{ MerlinEngine::MenuFunc, 0x900D }, // purple star puzzle
	{ MerlinEngine::MenuFunc, 0x8806 }, // cave memory puzzle
	{ MerlinEngine::MenuFunc, 0x850B }, // spirits puzzle (?)
	{ MerlinEngine::MenuFunc, 0x810D }, // gate puzzle
	{ MerlinEngine::MenuFunc, 0x640B }, // cave wall puzzle
	{ MerlinEngine::MenuFunc, 0x440D }, // spiderweb puzzle
	{ MerlinEngine::MenuFunc, 0x3810 }, // pink crystal puzzle
	{ MerlinEngine::MenuFunc, 0x0555 }, // stage 3 freeplay hub

	// NOTE: I will not watch the finale until the project is done!
	//{ MerlinEngine::PlayMovieFunc, MKTAG('F', 'N', 'L', 'E') },
};

const size_t MerlinEngine::SEQUENCE_SIZE =
	sizeof(MerlinEngine::SEQUENCE) /
	sizeof(MerlinEngine::SequenceEntry);

} // End of namespace Bolt
