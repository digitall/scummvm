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
	_cardEndCallback.func = nullptr;
	_cardEndCallback.param = nullptr;

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
				if (_cardEndCallback.func) {
					Callback temp = _cardEndCallback;
					_cardEndCallback.func = nullptr;
					_cardEndCallback.param = nullptr;
					temp.func(this, temp.param);
				}
				else {
					advanceSequence();
				}
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

	if (!_cursorImage) {
		_cursorImage.load(_boltlib, BltShortId(kCursorImageId));
	}

	// Format is expected to be CLUT7
	Common::Array<byte> decodedImage;
	decodedImage.resize(_cursorImage.getWidth() * _cursorImage.getHeight());
	::Graphics::Surface surface;
	surface.init(_cursorImage.getWidth(), _cursorImage.getHeight(),
		_cursorImage.getWidth(), &decodedImage[0],
		::Graphics::PixelFormat::createFormatCLUT8());
	_cursorImage.draw(surface, false);

	_engine->_system->setMouseCursor(&decodedImage[0],
		_cursorImage.getWidth(), _cursorImage.getHeight(),
		-_cursorImage.getOffset().x, -_cursorImage.getOffset().y, 0);

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
	card->init(this, _boltlib, id);
	setCurrentCard(card);
}

void MerlinEngine::startMenu(BltLongId id) {
	_currentCard.reset();
	GenericMenuCard* menuCard = new GenericMenuCard;
	menuCard->init(_engine, _boltlib, id);
	setCurrentCard(menuCard);
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

void MerlinEngine::setCurrentCard(Card *card) {
	_currentCard.reset(card);
	if (!_movie.isRunning() && _currentCard) {
		// If there is no movie playing, enter new card now
		_currentCard->enter();
	}
}

void MerlinEngine::setCardEndCallback(CallbackFunc func, const void *param) {
	_cardEndCallback.func = func;
	_cardEndCallback.param = param;
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

struct PuzzleEntry {
	uint16 sceneShortId;
	uint32 winMovieName;
};

struct HubEntry {
	uint16 hubId;
	int numPuzzles;
	const PuzzleEntry *puzzles;
};

class MerlinHubCard : public MenuCard {
public:
	static const HubEntry STAGE1;
	static const PuzzleEntry STAGE1_PUZZLES[6];
	static const HubEntry STAGE2;
	static const PuzzleEntry STAGE2_PUZZLES[9];
	static const HubEntry STAGE3;
	static const PuzzleEntry STAGE3_PUZZLES[12];

	void init(MerlinEngine *merlin, const HubEntry &entry);
	void enter();
protected:
	Status processButtonClick(int num);

private:
	MerlinEngine *_merlin;
	const HubEntry *_hubEntry;
	ScopedArray<BltImage> _itemImages;
};


const HubEntry MerlinHubCard::STAGE1 = { 0x0C0B, 6, MerlinHubCard::STAGE1_PUZZLES };
const PuzzleEntry MerlinHubCard::STAGE1_PUZZLES[6] = {
	{ 0x6017, MKTAG('S', 'E', 'E', 'D') }, // seeds (TODO: correct scene)
	{ 0x6017, MKTAG('G', 'R', 'A', 'V') }, // grave
	{ 0x3009, MKTAG('O', 'A', 'K', 'L') }, // oak leaf
	{ 0x8606, MKTAG('P', 'O', 'N', 'D') }, // pond
	{ 0x8606, MKTAG('L', 'E', 'A', 'V') }, // leaves (TODO: correct scene)
	{ 0x340A, MKTAG('R', 'A', 'V', 'N') }, // raven
};

const HubEntry MerlinHubCard::STAGE2 = { 0x0D34, 9, MerlinHubCard::STAGE2_PUZZLES };
const PuzzleEntry MerlinHubCard::STAGE2_PUZZLES[9] = {
	{ 0x400B, MKTAG('R', 'T', 'T', 'L') }, // rattlesnake
	{ 0x400B, MKTAG('P', 'L', 'A', 'Q') }, // ??? (TODO: correct scene)
	{ 0x7D0B, MKTAG('S', 'N', 'O', 'W') }, // ??? (TODO: correct scene)
	{ 0x7D0B, MKTAG('P', 'L', 'N', 'T') }, // planets
	{ 0x6817, MKTAG('P', 'R', 'C', 'H') }, // parchment
	{ 0x6817, MKTAG('B', 'B', 'L', 'E') }, // ??? (TODO: correct scene)
	{ 0x3C0B, MKTAG('S', 'K', 'L', 'T') }, // skeleton
	{ 0x8706, MKTAG('F', 'L', 'S', 'K') }, // flasks
	{ 0x8706, MKTAG('M', 'I', 'R', 'R') }, // ??? (TODO: correct scene)
};

const HubEntry MerlinHubCard::STAGE3 = { 0x0E4F, 12, MerlinHubCard::STAGE3_PUZZLES };
const PuzzleEntry MerlinHubCard::STAGE3_PUZZLES[12] = {
	{ 0x8C0C, MKTAG('W', 'N', 'D', 'W') }, // window
	{ 0x8C0C, MKTAG('O', 'C', 'T', 'A') }, // ??? (TODO: correct scene)
	{ 0x850B, MKTAG('S', 'P', 'R', 'T') }, // spirits
	{ 0x900D, MKTAG('S', 'T', 'A', 'R') }, // star
	{ 0x810D, MKTAG('D', 'O', 'O', 'R') }, // door
	{ 0x810D, MKTAG('G', 'E', 'M', 'S') }, // ??? (TODO: correct scene)
	{ 0x3810, MKTAG('C', 'S', 'T', 'L') }, // crystal
	{ 0x3810, MKTAG('D', 'E', 'M', 'N') }, // ??? (TODO: correct scene)
	{ 0x3810, MKTAG('T', 'I', 'L', 'E') }, // ??? (TODO: correct scene)
	{ 0x440D, MKTAG('S', 'P', 'I', 'D') }, // spider
	{ 0x640B, MKTAG('T', 'B', 'L', 'T') }, // tablet
	{ 0x8806, MKTAG('S', 'T', 'L', 'C') }, // stalactites & stalagmites
};

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
	_hubEntry = &entry;

	BltHub hubInfo(&BltResource(merlin->_boltlib.loadResource(BltShortId(entry.hubId), kBltHub))[0]);

	MenuCard::init(merlin->_engine, merlin->_boltlib, hubInfo.sceneId);
	_scene.setBackPlane(merlin->_boltlib, hubInfo.bgPlaneId);

	BltResourceList hubItemsList(merlin->_boltlib, hubInfo.itemListId);
	_itemImages.alloc(hubInfo.numItems);
	for (uint i = 0; i < hubInfo.numItems; ++i) {
		BltHubItem hubItem(&BltResource(merlin->_boltlib.loadResource(
			hubItemsList.get(i).value, kBltHubItem))[0]);
		_itemImages[i].load(merlin->_boltlib, hubItem.imageId);
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
	if (num >= 0 && num < _hubEntry->numPuzzles) {
		_merlin->setCardEndCallback(MerlinEngine::PuzzleFunc, &_hubEntry->puzzles[num]);
		return Ended;
	}

	// If no button was clicked, complete stage and transition to next hub.
	return Ended;
}

void MerlinEngine::HubFunc(MerlinEngine *self, const void *param) {
	self->_currentCard.reset();
	const HubEntry *entry = reinterpret_cast<const HubEntry*>(param);
	MerlinHubCard *card = new MerlinHubCard;
	card->init(self, *entry);
	self->setCurrentCard(card);
}

class MerlinPuzzleCard : public MenuCard {
public:
	void init(MerlinEngine *merlin, const PuzzleEntry &entry);
protected:
	Status processButtonClick(int num);

	MerlinEngine *_merlin;
	uint32 _winMovieName;
};

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

	// Extract current hub card entry to support returning to it
	const HubEntry *currentHub = reinterpret_cast<const HubEntry*>(SEQUENCE[self->_sequenceCursor].param);
	// Set it up so that when puzzle ends, you return to hub
	self->setCardEndCallback(HubFunc, currentHub);

	const PuzzleEntry *entry = reinterpret_cast<const PuzzleEntry*>(param);
	MerlinPuzzleCard *card = new MerlinPuzzleCard;
	card->init(self, *entry);
	self->setCurrentCard(card);
}

void MerlinEngine::FreeplayHubFunc(MerlinEngine *self, const void *param) {
	self->_currentCard.reset();

	uint16 scene = *reinterpret_cast<const uint16*>(param);
	GenericMenuCard *card = new GenericMenuCard;
	card->init(self->_engine, self->_boltlib, BltShortId(scene));
	self->setCurrentCard(card);
}

static const uint32 PLOT_MOVIE_BMPR = MKTAG('B', 'M', 'P', 'R');
static const uint32 PLOT_MOVIE_INTR = MKTAG('I', 'N', 'T', 'R');
static const uint32 PLOT_MOVIE_PLOG = MKTAG('P', 'L', 'O', 'G');
static const uint32 PLOT_MOVIE_LABT = MKTAG('L', 'A', 'B', 'T');
static const uint32 PLOT_MOVIE_CAV1 = MKTAG('C', 'A', 'V', '1');
static const uint32 PLOT_MOVIE_FNLE = MKTAG('F', 'N', 'L', 'E');

static const uint16 FREEPLAY1_SCENE = 0x0337;
static const uint16 FREEPLAY2_SCENE = 0x0446;
static const uint16 FREEPLAY3_SCENE = 0x0555;

const MerlinEngine::Callback
MerlinEngine::SEQUENCE[] = {
	
	// Pre-game menus
	{ MerlinEngine::PlotMovieFunc, &PLOT_MOVIE_BMPR },
	{ MerlinEngine::PlotMovieFunc, &PLOT_MOVIE_INTR },
	{ MerlinEngine::MainMenuFunc, nullptr },
	{ MerlinEngine::FileMenuFunc, nullptr },
	{ MerlinEngine::DifficultyMenuFunc, nullptr },

	// Stage 1: Forest
	{ MerlinEngine::PlotMovieFunc, &PLOT_MOVIE_PLOG },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE1 },

	// Stage 2: Laboratory
	{ MerlinEngine::PlotMovieFunc, &PLOT_MOVIE_LABT },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE2 },

	// Stage 3: Cave
	{ MerlinEngine::PlotMovieFunc, &PLOT_MOVIE_CAV1 },
	{ MerlinEngine::HubFunc, &MerlinHubCard::STAGE3 },

	// NOTE: The Finale movie is hidden until the game is fully implemented.
	//{ MerlinEngine::PlotMovieFunc, &PLOT_MOVIE_FNLE },

	{ MerlinEngine::FreeplayHubFunc, &FREEPLAY1_SCENE },
	{ MerlinEngine::FreeplayHubFunc, &FREEPLAY2_SCENE },
	{ MerlinEngine::FreeplayHubFunc, &FREEPLAY3_SCENE },
};

const size_t MerlinEngine::SEQUENCE_SIZE =
	sizeof(MerlinEngine::SEQUENCE) /
	sizeof(MerlinEngine::Callback);

} // End of namespace Bolt
