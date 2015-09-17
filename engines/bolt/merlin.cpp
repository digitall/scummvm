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
	_boltlib.load("BOLTLIB.BLT"); // TODO: error handling

	// Load PF files
	_maPf.load("MA.PF"); // TODO: error handling
	_helpPf.load("HELP.PF");
	_potionPf.load("POTION.PF");
	_challdirPf.load("CHALLDIR.PF");

	_movie.setTriggerCallback(MerlinEngine::MovieTrigger, this);

	// Load cursor
	initCursor();

	// Start sequence
	resetSequence();
}

void MerlinEngine::processEvent(const BoltEvent &event) {
	bool eventProcessed = false;
	while (!eventProcessed) {
		// TODO: clean up and simplify
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
				// When movie stops, enter current card
				if (_currentCard) {
					_currentCard->enter();
				}
			}
			eventProcessed = true;
		}
		else if (_currentCard) {
			// Process current card
			if (_currentCard->processEvent(event) == Card::Ended) {
				advanceSequence();
			}
			eventProcessed = true;
		}
		else {
			advanceSequence();
		}
	}
}

void MerlinEngine::initCursor() {
	static const uint16 kCursorImageId = 0x9D00;
	static const byte kCursorPalette[3 * 2] = { 0, 0, 0, 255, 255, 255 };

	BltImage cursorImage;
	cursorImage.init(_boltlib, BltShortId(kCursorImageId));

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
	if (!_movie.isRunning()) {
		// If there is no movie playing now, enter next card
		if (_currentCard) {
			_currentCard->enter();
		}
	}
}

void MerlinEngine::advanceSequence() {
	// advance sequence
	_sequenceCursor = (_sequenceCursor + 1) % SEQUENCE_SIZE; // reset at end
	SEQUENCE[_sequenceCursor].func(this, SEQUENCE[_sequenceCursor].param);
	if (!_movie.isRunning()) {
		// If there is no movie playing now, enter next card
		if (_currentCard) {
			_currentCard->enter();
		}
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

	BltResource mainMenuRes(bltFile.loadResource(id, kBltMainMenu));
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
		_merlin->startMovie(_merlin->_maPf, MKTAG('C', 'R', 'D', 'T'));
		return None;
	case 4: // Tour
		_merlin->startMovie(_merlin->_maPf, MKTAG('T', 'O', 'U', 'R'));
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
	card->init(this, _boltlib, id);
}

void MerlinEngine::startMenu(BltLongId id) {
	_currentCard.reset();

	GenericMenuCard* menuCard = new GenericMenuCard;
	_currentCard.reset(menuCard);
	menuCard->init(_engine, _boltlib, id);
}

void MerlinEngine::startMovie(PfFile &pfFile, uint32 name) {
	_movie.stop();
	_movie.load(_engine, pfFile, name);
	_movie.process();
}

void MerlinEngine::MovieTrigger(void *param, uint16 triggerType) {
	MerlinEngine *self = reinterpret_cast<MerlinEngine*>(param);
	if (triggerType == 0x8002) {
		// Enter next card; used during win movies to transition back to hub card
		if (self->_currentCard) {
			self->_currentCard->enter();
		}
	}
	else {
		warning("unknown movie trigger 0x%.04X", (int)triggerType);
	}
}

void MerlinEngine::PlotWarningFunc(MerlinEngine *self, const void *param) {
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

void MerlinEngine::PlotMovieFunc(MerlinEngine *self, const void *param) {
	self->_currentCard.reset();
	uint32 name = *reinterpret_cast<const uint32*>(param);
	self->startMovie(self->_maPf, name);
}

void MerlinEngine::MainMenuFunc(MerlinEngine *self, const void *param) {
	static const uint16 kMainMenuId = 0x0118;
	self->startMainMenu(BltShortId(kMainMenuId));
}

void MerlinEngine::FileMenuFunc(MerlinEngine *self, const void *param) {
	static const uint16 kFileMenuId = 0x027A;
	self->startMenu(BltShortId(kFileMenuId));
}

void MerlinEngine::DifficultyMenuFunc(MerlinEngine *self, const void *param) {
	static const uint16 kDifficultyMenuId = 0x006B;
	self->startMenu(BltShortId(kDifficultyMenuId));
}

struct HubEntry {
	uint16 hubId;
};

class MerlinHubCard : public MenuCard {
public:
	static const HubEntry STAGE1;
	static const HubEntry STAGE2;
	static const HubEntry STAGE3;

	void init(MerlinEngine *merlin, const HubEntry &entry);
	void enter();
protected:
	Status processButtonClick(int num);

private:
	MerlinEngine *_merlin;
	ScopedArray<BltImage> _itemImages;
};

const HubEntry MerlinHubCard::STAGE1 = { 0x0C0B };
const HubEntry MerlinHubCard::STAGE2 = { 0x0D34 };
const HubEntry MerlinHubCard::STAGE3 = { 0x0E4F };

struct BltHub { // type 40
	BltHub(const byte *src) {
		sceneId = BltLongId(READ_BE_UINT32(&src[0]));
		// FIXME: unknown field at offset 4
		bgPlaneId = BltLongId(READ_BE_UINT32(&src[6]));
		// FIXME: unknown field at offset 0xA
		numItems = src[0xB];
		itemListId = BltLongId(READ_BE_UINT32(&src[0xC]));
	}

	BltLongId sceneId;
	BltLongId bgPlaneId;
	byte numItems;
	BltLongId itemListId;
};

struct BltHubItem { // type 41
	BltHubItem(const byte *src) {
		// FIXME: unknown fields
		imageId = BltLongId(READ_BE_UINT32(&src[4]));
	}

	BltLongId imageId;
};

void MerlinHubCard::init(MerlinEngine *merlin, const HubEntry &entry) {
	_merlin = merlin;

	BltResource hubRes(merlin->_boltlib.loadResource(BltShortId(entry.hubId), kBltHub));
	BltHub hubInfo(&hubRes[0]);

	MenuCard::init(merlin->_engine, merlin->_boltlib, hubInfo.sceneId);
	_scene.setBackPlane(merlin->_boltlib, hubInfo.bgPlaneId);

	BltResource hubItemsList(merlin->_boltlib.loadResource(hubInfo.itemListId, kBltResourceList));
	_itemImages.alloc(hubInfo.numItems);
	for (uint i = 0; i < hubInfo.numItems; ++i) {
		BltLongId hubItemId(READ_BE_UINT32(&hubItemsList[i * 4]));
		BltResource hubItem(merlin->_boltlib.loadResource(hubItemId, kBltHubItem));
		BltHubItem parsedHubItem(&hubItem[0]);
		_itemImages[i].init(merlin->_boltlib, parsedHubItem.imageId);
	}
}

Graphics& MerlinEngine::getGraphics() {
	return _engine->_graphics;
}

void MerlinHubCard::enter() {
	MenuCard::enter();

	// Draw item images to back plane
	for (uint i = 0; i < _itemImages.size(); ++i) {
		_itemImages[i].drawAt(_merlin->getGraphics().getBackPlane().getSurface(), 0, 0, true);
	}
}

Card::Status MerlinHubCard::processButtonClick(int num) {
	// TODO: take action depending on button
	return Ended;
}

void MerlinEngine::HubFunc(MerlinEngine *self, const void *param) {
	self->_currentCard.reset();

	const HubEntry *entry = reinterpret_cast<const HubEntry*>(param);
	MerlinHubCard *card = new MerlinHubCard;
	self->_currentCard.reset(card);
	card->init(self, *entry);
}

struct PuzzleEntry {
	uint16 sceneShortId;
	uint32 winMovieName;
};

class MerlinPuzzleCard : public MenuCard {
public:
	static const PuzzleEntry STAGE1_PUZZLE1;
	static const PuzzleEntry STAGE1_PUZZLE2;
	static const PuzzleEntry STAGE1_PUZZLE3;
	static const PuzzleEntry STAGE1_PUZZLE4;
	static const PuzzleEntry STAGE1_PUZZLE5;
	static const PuzzleEntry STAGE1_PUZZLE6;

	static const PuzzleEntry STAGE2_PUZZLE1;
	static const PuzzleEntry STAGE2_PUZZLE2;
	static const PuzzleEntry STAGE2_PUZZLE3;
	static const PuzzleEntry STAGE2_PUZZLE4;
	static const PuzzleEntry STAGE2_PUZZLE5;
	static const PuzzleEntry STAGE2_PUZZLE6;
	static const PuzzleEntry STAGE2_PUZZLE7;
	static const PuzzleEntry STAGE2_PUZZLE8;

	static const PuzzleEntry STAGE3_PUZZLE1;
	static const PuzzleEntry STAGE3_PUZZLE2;
	static const PuzzleEntry STAGE3_PUZZLE3;
	static const PuzzleEntry STAGE3_PUZZLE4;
	static const PuzzleEntry STAGE3_PUZZLE5;
	static const PuzzleEntry STAGE3_PUZZLE6;
	static const PuzzleEntry STAGE3_PUZZLE7;
	static const PuzzleEntry STAGE3_PUZZLE8;
	static const PuzzleEntry STAGE3_PUZZLE9;
	static const PuzzleEntry STAGE3_PUZZLE10;
	static const PuzzleEntry STAGE3_PUZZLE11;
	static const PuzzleEntry STAGE3_PUZZLE12;

	void init(MerlinEngine *merlin, const PuzzleEntry &entry);
protected:
	Status processButtonClick(int num);

	MerlinEngine *_merlin;
	uint32 _winMovieName;
};

// grave
const PuzzleEntry MerlinPuzzleCard::STAGE1_PUZZLE2 = { 0x6017, MKTAG('G', 'R', 'A', 'V') };
// leaf
const PuzzleEntry MerlinPuzzleCard::STAGE1_PUZZLE3 = { 0x3009, MKTAG('O', 'A', 'K', 'L') };
// frogs and bugs
const PuzzleEntry MerlinPuzzleCard::STAGE1_PUZZLE4 = { 0x8606, MKTAG('P', 'O', 'N', 'D') };
// raven
const PuzzleEntry MerlinPuzzleCard::STAGE1_PUZZLE6 = { 0x340A, MKTAG('R', 'A', 'V', 'N') };

// solar system
const PuzzleEntry MerlinPuzzleCard::STAGE2_PUZZLE2 = { 0x7D0B, MKTAG('P', 'L', 'N', 'T') };
// parchment
const PuzzleEntry MerlinPuzzleCard::STAGE2_PUZZLE3 = { 0x6817, MKTAG('P', 'R', 'C', 'H') };
// windowsill
const PuzzleEntry MerlinPuzzleCard::STAGE2_PUZZLE5 = { 0x3C0B, MKTAG('S', 'K', 'L', 'T') };
// chest
const PuzzleEntry MerlinPuzzleCard::STAGE2_PUZZLE6 = { 0x400B, MKTAG('R', 'T', 'T', 'L') };
// pots
const PuzzleEntry MerlinPuzzleCard::STAGE2_PUZZLE8 = { 0x8706, MKTAG('F', 'L', 'S', 'K') };

// stained glass
const PuzzleEntry MerlinPuzzleCard::STAGE3_PUZZLE1 = { 0x8C0C, MKTAG('W', 'N', 'D', 'W') };
// spirits
const PuzzleEntry MerlinPuzzleCard::STAGE3_PUZZLE3 = { 0x850B, MKTAG('S', 'P', 'R', 'T') };
// purple star
const PuzzleEntry MerlinPuzzleCard::STAGE3_PUZZLE4 = { 0x900D, MKTAG('S', 'T', 'A', 'R') };
// gate
const PuzzleEntry MerlinPuzzleCard::STAGE3_PUZZLE5 = { 0x810D, MKTAG('D', 'O', 'O', 'R') };
// pink crystal
const PuzzleEntry MerlinPuzzleCard::STAGE3_PUZZLE7 = { 0x3810, MKTAG('C', 'S', 'T', 'L') };
// spiderweb
const PuzzleEntry MerlinPuzzleCard::STAGE3_PUZZLE10 = { 0x440D, MKTAG('S', 'P', 'I', 'D') };
// cave wall
const PuzzleEntry MerlinPuzzleCard::STAGE3_PUZZLE11 = { 0x640B, MKTAG('T', 'B', 'L', 'T') };
// stalactites and stalagmites
const PuzzleEntry MerlinPuzzleCard::STAGE3_PUZZLE12 = { 0x8806, MKTAG('S', 'T', 'L', 'C') };

void MerlinPuzzleCard::init(MerlinEngine *merlin, const PuzzleEntry &entry) {
	_merlin = merlin;
	_winMovieName = entry.winMovieName;
	MenuCard::init(merlin->_engine, merlin->_boltlib, BltShortId(entry.sceneShortId));
}

Card::Status MerlinPuzzleCard::processButtonClick(int num) {
	// TODO: implement puzzle
	if (_winMovieName) {
		_merlin->startMovie(_merlin->_challdirPf, _winMovieName);
	}
	return Ended;
}

void MerlinEngine::PuzzleFunc(MerlinEngine *self, const void *param) {
	self->_currentCard.reset();

	const PuzzleEntry *entry = reinterpret_cast<const PuzzleEntry*>(param);
	MerlinPuzzleCard *card = new MerlinPuzzleCard;
	self->_currentCard.reset(card);
	card->init(self, *entry);
}

static const uint32 PLOT_MOVIE_BMPR = MKTAG('B', 'M', 'P', 'R');
static const uint32 PLOT_MOVIE_INTR = MKTAG('I', 'N', 'T', 'R');
static const uint32 PLOT_MOVIE_PLOG = MKTAG('P', 'L', 'O', 'G');
static const uint32 PLOT_MOVIE_LABT = MKTAG('L', 'A', 'B', 'T');
static const uint32 PLOT_MOVIE_CAV1 = MKTAG('C', 'A', 'V', '1');
static const uint32 PLOT_MOVIE_FNLE = MKTAG('F', 'N', 'L', 'E');

const MerlinEngine::SequenceEntry
MerlinEngine::SEQUENCE[] = {
	
	// Pre-game menus
	{ MerlinEngine::PlotMovieFunc, &PLOT_MOVIE_BMPR },
	{ MerlinEngine::PlotMovieFunc, &PLOT_MOVIE_INTR },
	{ MerlinEngine::MainMenuFunc, nullptr }, // main menu
	{ MerlinEngine::FileMenuFunc, nullptr }, // file select
	{ MerlinEngine::DifficultyMenuFunc, nullptr }, // difficulty select

	// Stage 1: Forest
	{ MerlinEngine::PlotMovieFunc, &PLOT_MOVIE_PLOG },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE1 },
	{ MerlinEngine::PlotWarningFunc, nullptr },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE1_PUZZLE2 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE1 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE1_PUZZLE3 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE1 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE1_PUZZLE4 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE1 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE1_PUZZLE6 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE1 },

	// Stage 2: Laboratory
	{ MerlinEngine::PlotMovieFunc, &PLOT_MOVIE_LABT },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE2 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE2_PUZZLE2 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE2 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE2_PUZZLE3 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE2 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE2_PUZZLE5 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE2 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE2_PUZZLE6 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE2 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE2_PUZZLE8 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE2 },

	// Stage 3: Cave
	{ MerlinEngine::PlotMovieFunc, &PLOT_MOVIE_CAV1 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE3 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE3_PUZZLE1 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE3 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE3_PUZZLE3 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE3 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE3_PUZZLE4 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE3 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE3_PUZZLE5 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE3 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE3_PUZZLE7 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE3 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE3_PUZZLE10 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE3 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE3_PUZZLE11 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE3 },
	{ MerlinEngine::PuzzleFunc, &MerlinPuzzleCard::STAGE3_PUZZLE12 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE3 },

#if 0
	{ MerlinEngine::MenuFunc, 0x0337 }, // stage 1 freeplay hub
	{ MerlinEngine::MenuFunc, 0x0446 }, // stage 2 freeplay hub
	{ MerlinEngine::MenuFunc, 0x0555 }, // stage 3 freeplay hub
#endif
};

const size_t MerlinEngine::SEQUENCE_SIZE =
	sizeof(MerlinEngine::SEQUENCE) /
	sizeof(MerlinEngine::SequenceEntry);

} // End of namespace Bolt
