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

#include "bolt/merlin/merlin.h"

#include "common/system.h"
#include "gui/message.h"

#include "bolt/bolt.h"
#include "bolt/menu_card.h"
#include "bolt/merlin/action_puzzle.h"
#include "bolt/merlin/tangram_puzzle.h"
#include "bolt/merlin/hub.h"
#include "bolt/merlin/main_menu.h"

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

	_movie.setTriggerCallback(MerlinEngine::movieTrigger, this);

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
	enterSequenceEntry();
}

void MerlinEngine::advanceSequence() {
	// advance sequence
	_sequenceCursor = (_sequenceCursor + 1) % kSequenceSize; // reset at end
	enterSequenceEntry();
}

void MerlinEngine::enterSequenceEntry() {
	_currentPuzzle = nullptr;
	kSequence[_sequenceCursor].func(this, kSequence[_sequenceCursor].param);
}

void MerlinEngine::startMainMenu(BltLongId id) {
	_currentCard.reset();
	MainMenu* card = new MainMenu;
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

void MerlinEngine::movieTrigger(void *param, uint16 triggerType) {
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

void MerlinEngine::plotMovie(MerlinEngine *self, const void *param) {
	self->_currentCard.reset();
	uint32 name = *reinterpret_cast<const uint32*>(param);
	self->startMovie(self->_maPf, name);
}

void MerlinEngine::mainMenu(MerlinEngine *self, const void *param) {
	static const uint16 kMainMenuId = 0x0118;
	self->startMainMenu(BltShortId(kMainMenuId));
}

void MerlinEngine::fileMenu(MerlinEngine *self, const void *param) {
	static const uint16 kFileMenuId = 0x027A;
	self->startMenu(BltShortId(kFileMenuId));
}

void MerlinEngine::difficultyMenu(MerlinEngine *self, const void *param) {
	static const uint16 kDifficultyMenuId = 0x006B;
	self->startMenu(BltShortId(kDifficultyMenuId));
}

class TestPuzzle : public MenuCard {
public:
	static Card* make(MerlinEngine *merlin, const PuzzleEntry &entry);
	void init(MerlinEngine *merlin, const PuzzleEntry &entry);
protected:
	Status processButtonClick(int num);

	MerlinEngine *_merlin;
	uint32 _winMovie;
};

Card* TestPuzzle::make(MerlinEngine *merlin, const PuzzleEntry &entry) {
	TestPuzzle *card = new TestPuzzle;
	card->init(merlin, entry);
	return card;
}

void TestPuzzle::init(MerlinEngine *merlin, const PuzzleEntry &entry) {
	_merlin = merlin;
	_winMovie = entry.winMovie;
	MenuCard::init(merlin->_engine, merlin->_boltlib, BltShortId(entry.resId));
}

Card::Status TestPuzzle::processButtonClick(int num) {
	// TODO: implement puzzle
	_merlin->setCardEndCallback(MerlinEngine::win, nullptr);
	return Ended;
}

// From MERLIN.EXE:
//
// Action puzzles:
//   SeedsDD    4921
//   LeavesDD   4D19
//   BubblesDD  5113
//   SnowflakDD 551C
//   GemsDD     5918
//   DemonsDD   5D17
//
// Word puzzles:
//   GraveDD  61E3
//   ParchDD  69E1
//   TabletDD 65E1
//
// Tangram puzzles:
//   MirrorDD  7115
//   PlaqueDD  6D15
//   OctagonDD 7515
//   TileDD    7915
//
// Sliding puzzles:
//   RavenDD  353F
//   LeafDD   313F
//   SnakeDD  4140
//   SkeltnDD 3D3F
//   SpiderDD 453F
//   QuartzDD 393F
//
// Synchronization puzzles:
//   PlanetDD 7D12
//   DoorDD   8114
//   SphereDD 8512
//
// Color puzzles:
//   WindowDD 8C13
//   StarDD   9014
//
// Potion puzzles:
//   ForestDD 940C
//   LabratDD 980C
//   CavernDD 9C0E
//
// Memory puzzles:
//   PondDD   865E
//   FlasksDD 8797
//   StalacDD 887B

const HubEntry MerlinEngine::kStage1 = { 0x0C0B, 6, MerlinEngine::kStage1Puzzles };
const PuzzleEntry MerlinEngine::kStage1Puzzles[6] = {
	{ ActionPuzzle::make, 0x4921, MKTAG('S', 'E', 'E', 'D') }, // seeds
	{ TestPuzzle::make, 0x6017, MKTAG('G', 'R', 'A', 'V') }, // grave
	{ TestPuzzle::make, 0x3009, MKTAG('O', 'A', 'K', 'L') }, // oak leaf
	{ TestPuzzle::make, 0x8606, MKTAG('P', 'O', 'N', 'D') }, // pond
	{ ActionPuzzle::make, 0x4D19, MKTAG('L', 'E', 'A', 'V') }, // leaves
	{ TestPuzzle::make, 0x340A, MKTAG('R', 'A', 'V', 'N') }, // raven
};

const HubEntry MerlinEngine::kStage2 = { 0x0D34, 9, MerlinEngine::kStage2Puzzles };
const PuzzleEntry MerlinEngine::kStage2Puzzles[9] = {
	{ TestPuzzle::make, 0x400B, MKTAG('R', 'T', 'T', 'L') }, // rattlesnake
	{ TangramPuzzle::make, 0x6D15, MKTAG('P', 'L', 'A', 'Q') }, // plaque
	{ ActionPuzzle::make, 0x551C, MKTAG('S', 'N', 'O', 'W') }, // snow
	{ TestPuzzle::make, 0x7D0B, MKTAG('P', 'L', 'N', 'T') }, // planets
	{ TestPuzzle::make, 0x6817, MKTAG('P', 'R', 'C', 'H') }, // parchment
	{ ActionPuzzle::make, 0x5113, MKTAG('B', 'B', 'L', 'E') }, // bubbles
	{ TestPuzzle::make, 0x3C0B, MKTAG('S', 'K', 'L', 'T') }, // skeleton
	{ TestPuzzle::make, 0x8706, MKTAG('F', 'L', 'S', 'K') }, // flasks
	{ TangramPuzzle::make, 0x7115, MKTAG('M', 'I', 'R', 'R') }, // mirror
};

const HubEntry MerlinEngine::kStage3 = { 0x0E4F, 12, MerlinEngine::kStage3Puzzles };
const PuzzleEntry MerlinEngine::kStage3Puzzles[12] = {
	{ TestPuzzle::make, 0x8C0C, MKTAG('W', 'N', 'D', 'W') }, // window
	{ TangramPuzzle::make, 0x7515, MKTAG('O', 'C', 'T', 'A') }, // octagon
	{ TestPuzzle::make, 0x850B, MKTAG('S', 'P', 'R', 'T') }, // spirits
	{ TestPuzzle::make, 0x900D, MKTAG('S', 'T', 'A', 'R') }, // star
	{ TestPuzzle::make, 0x810D, MKTAG('D', 'O', 'O', 'R') }, // door
	{ ActionPuzzle::make, 0x5918, MKTAG('G', 'E', 'M', 'S') }, // gems
	{ TestPuzzle::make, 0x3810, MKTAG('C', 'S', 'T', 'L') }, // crystal
	{ ActionPuzzle::make, 0x5D17, MKTAG('D', 'E', 'M', 'N') }, // demons
	{ TangramPuzzle::make, 0x7915, MKTAG('T', 'I', 'L', 'E') }, // tile
	{ TestPuzzle::make, 0x440D, MKTAG('S', 'P', 'I', 'D') }, // spider
	{ TestPuzzle::make, 0x640B, MKTAG('T', 'B', 'L', 'T') }, // tablet
	{ TestPuzzle::make, 0x8806, MKTAG('S', 'T', 'L', 'C') }, // stalactites & stalagmites
};

Graphics& MerlinEngine::getGraphics() {
	return _engine->_graphics;
}

void MerlinEngine::scheduleDisplayUpdate() {
	_engine->scheduleDisplayUpdate();
}

void MerlinEngine::hub(MerlinEngine *self, const void *param) {
	self->_currentCard.reset();
	const HubEntry *entry = reinterpret_cast<const HubEntry*>(param);
	HubCard *card = new HubCard;
	card->init(self, *entry);
	self->setCurrentCard(card);
}

void MerlinEngine::puzzle(MerlinEngine *self, const void *param) {
	self->_currentCard.reset();
	self->_currentPuzzle = reinterpret_cast<const PuzzleEntry*>(param);
	Card *card = self->_currentPuzzle->makeFunc(self, *self->_currentPuzzle);
	self->setCurrentCard(card);
}

void MerlinEngine::win(MerlinEngine *self, const void *param) {
	self->_currentCard.reset();
	assert(self->_currentPuzzle);
	if (self->_currentPuzzle->winMovie) {
		self->startMovie(self->_challdirPf, self->_currentPuzzle->winMovie);
	}
	self->enterSequenceEntry(); // Return to hub
}

void MerlinEngine::freeplayHub(MerlinEngine *self, const void *param) {
	self->_currentCard.reset();

	uint16 scene = *reinterpret_cast<const uint16*>(param);
	GenericMenuCard *card = new GenericMenuCard;
	card->init(self->_engine, self->_boltlib, BltShortId(scene));
	self->setCurrentCard(card);
}

static const uint32 kPlotMovieBMPR = MKTAG('B', 'M', 'P', 'R');
static const uint32 kPlotMovieINTR = MKTAG('I', 'N', 'T', 'R');
static const uint32 kPlotMoviePLOG = MKTAG('P', 'L', 'O', 'G');
static const uint32 kPlotMovieLABT = MKTAG('L', 'A', 'B', 'T');
static const uint32 kPlotMovieCAV1 = MKTAG('C', 'A', 'V', '1');
static const uint32 kPlotMovieFNLE = MKTAG('F', 'N', 'L', 'E');

static const uint16 kFreeplayScene1 = 0x0337;
static const uint16 kFreeplayScene2 = 0x0446;
static const uint16 kFreeplayScene3 = 0x0555;

const MerlinEngine::Callback
MerlinEngine::kSequence[] = {
	
	// Pre-game menus
	{ MerlinEngine::plotMovie, &kPlotMovieBMPR },
	{ MerlinEngine::plotMovie, &kPlotMovieINTR },
	{ MerlinEngine::mainMenu, nullptr },
	{ MerlinEngine::fileMenu, nullptr },
	{ MerlinEngine::difficultyMenu, nullptr },

	// Stage 1: Forest
	{ MerlinEngine::plotMovie, &kPlotMoviePLOG },
	{ MerlinEngine::hub, &MerlinEngine::kStage1 },

	// Stage 2: Laboratory
	{ MerlinEngine::plotMovie, &kPlotMovieLABT },
	{ MerlinEngine::hub, &MerlinEngine::kStage2 },

	// Stage 3: Cave
	{ MerlinEngine::plotMovie, &kPlotMovieCAV1 },
	{ MerlinEngine::hub, &MerlinEngine::kStage3 },

	// NOTE: The Finale movie is hidden until the game is fully implemented.
	//{ MerlinEngine::plotMovie, &kPlotMovieFNLE },

	{ MerlinEngine::freeplayHub, &kFreeplayScene1 },
	{ MerlinEngine::freeplayHub, &kFreeplayScene2 },
	{ MerlinEngine::freeplayHub, &kFreeplayScene3 },
};

const size_t MerlinEngine::kSequenceSize =
	sizeof(MerlinEngine::kSequence) /
	sizeof(MerlinEngine::Callback);

} // End of namespace Bolt
