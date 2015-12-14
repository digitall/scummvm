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

#include "common/events.h"
#include "common/system.h"
#include "gui/message.h"

#include "bolt/bolt.h"
#include "bolt/menu_card.h"
#include "bolt/merlin/action_puzzle.h"
#include "bolt/merlin/color_puzzle.h"
#include "bolt/merlin/memory_puzzle.h"
#include "bolt/merlin/sliding_puzzle.h"
#include "bolt/merlin/synch_puzzle.h"
#include "bolt/merlin/tangram_puzzle.h"
#include "bolt/merlin/word_puzzle.h"
#include "bolt/merlin/hub.h"
#include "bolt/merlin/main_menu.h"

namespace Bolt {

MerlinEngine::MerlinEngine(OSystem *syst, const ADGameDescription *gd)
	: BoltEngine(syst, gd)
{ }

void MerlinEngine::init() {
	_boltlib.load("BOLTLIB.BLT");

	_maPf.load("MA.PF");
	_helpPf.load("HELP.PF");
	_potionPf.load("POTION.PF");
	_challdirPf.load("CHALLDIR.PF");

	_movie.setTriggerCallback(MerlinEngine::movieTrigger, this);

	// Load cursor
	initCursor();

	// Start sequence
	resetSequence();
}

void MerlinEngine::handleEvent(const BoltEvent &event) {
	// Play movie over anything else
	if (_movie.isRunning()) {
		handleEventInMovie(event);
	}
	else if (_currentCard) {
		handleEventInCard(event);
	}
	else {
		assert(false); // Unreachable; there must be an active movie or card
	}
}

void MerlinEngine::initCursor() {
	static const uint16 kCursorImageId = 0x9D00;
	static const byte kCursorPalette[3 * 2] = { 0, 0, 0, 255, 255, 255 };

	if (!_cursorImage) {
		_cursorImage.load(_boltlib, BltShortId(kCursorImageId));
	}

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

void MerlinEngine::resetSequence() {
	_sequenceCursor = -1;
	advanceSequence();
}

void MerlinEngine::advanceSequence() {
	// Advance sequence until movie or card becomes active
	_graphics.resetColorCycles(); // XXX: keeps cycles from sticking in wrong scenes; might break something?
	do {
		++_sequenceCursor;
		if (_sequenceCursor >= kSequenceSize) {
			_sequenceCursor = 0;
		}
		enterSequenceEntry();
	} while (!_movie.isRunning() && !_currentCard);
}

// Call pointer to member function.
// See <https://isocpp.org/wiki/faq/pointers-to-members>
#define CALL_MEMBER_FN(object, fn) ((object).*(fn))

void MerlinEngine::enterSequenceEntry() {
	_currentHub = nullptr;
	_currentPuzzle = nullptr;
	const Callback &callback = kSequence[_sequenceCursor];
	CALL_MEMBER_FN(*this, callback.func)(callback.param);
}

void MerlinEngine::startMainMenu(BltId id) {
	_currentCard.reset();
	MainMenu* card = new MainMenu;
	card->init(&_graphics, _boltlib, id);
	setCurrentCard(card);
}

void MerlinEngine::startMenu(BltId id) {
	_currentCard.reset();
	GenericMenuCard* menuCard = new GenericMenuCard;
	menuCard->init(&_graphics, _boltlib, id);
	setCurrentCard(menuCard);
}

void MerlinEngine::startMovie(PfFile &pfFile, uint32 name) {
	// Color cycles do NOT stop when a movie starts.
	_movie.stop();
	_movie.load(&_graphics, _mixer, pfFile, name, _eventTime);
	_movie.process(_eventTime);
}

void MerlinEngine::movieTrigger(void *param, uint16 triggerType) {
	MerlinEngine *self = reinterpret_cast<MerlinEngine*>(param);
	if (triggerType == 0x8002) {
		// Enter next card; used during win movies to transition back to hub card
		if (self->_currentCard) {
			self->enterCurrentCard(false);
		}
	}
	else {
		warning("unknown movie trigger 0x%.04X", (int)triggerType);
	}
}

void MerlinEngine::handleEventInMovie(const BoltEvent &event) {
	// Click to stop movie
	if (event.type == BoltEvent::Click) {
		_movie.stop();
	}
	else {
		_movie.process(event.time);
	}

	if (!_movie.isRunning()) {
		// When movie stops, enter current card
		if (_currentCard) {
			enterCurrentCard(true);
		}
		else {
			advanceSequence();
		}
	}
}

void MerlinEngine::handleEventInCard(const BoltEvent &event) {
	assert(_currentCard);

	Card::Signal signal = _currentCard->handleEvent(event);
	switch (signal) {
	case Card::kNull:
		break;
	case Card::kEnd:
		advanceSequence();
		break;
	case Card::kWin:
		win();
		break;
	case Card::kPlayHelp:
		// TODO: help
		break;
	case Card::kPlayTour:
		startMovie(_maPf, MKTAG('T', 'O', 'U', 'R'));
		break;
	case Card::kPlayCredits:
		startMovie(_maPf, MKTAG('C', 'R', 'D', 'T'));
		break;
	default:
		if (_currentHub &&
			signal >= Card::kEnterPuzzleX &&
			signal < Card::kEnterPuzzleX + _currentHub->numPuzzles)
		{
			int puzzleNum = signal - Card::kEnterPuzzleX;
			puzzle(&_currentHub->puzzles[puzzleNum]);
		}
		else {
			assert(false); // Unreachable; signal must be valid
		}
		break;
	}
}

void MerlinEngine::win() {
	_currentCard.reset();
	assert(_currentPuzzle);
	startMovie(_challdirPf, _currentPuzzle->winMovie);
	enterSequenceEntry(); // Return to hub
}

void MerlinEngine::puzzle(const PuzzleEntry *entry) {
	_currentCard.reset();
	_currentPuzzle = entry;
	Card *card = _currentPuzzle->puzzle(&_graphics, _boltlib, BltShortId(_currentPuzzle->resId));
	setCurrentCard(card);
}

void MerlinEngine::setCurrentCard(Card *card) {
	_currentCard.reset(card);
	if (!_movie.isRunning() && _currentCard) {
		// If there is no movie playing, enter new card now
		enterCurrentCard(true);
	}
}

void MerlinEngine::enterCurrentCard(bool cursorActive) {
	assert(_currentCard);
	_graphics.resetColorCycles();
	_currentCard->enter();
	if (cursorActive) {
		BoltEvent hoverEvent;
		hoverEvent.type = BoltEvent::Hover;
		hoverEvent.time = _eventTime;
		hoverEvent.point = getEventManager()->getMousePos();
		_currentCard->handleEvent(hoverEvent);
	}
}

void MerlinEngine::plotMovie(const void *param) {
	_currentCard.reset();
	uint32 name = *reinterpret_cast<const uint32*>(param);
	startMovie(_maPf, name);
}

void MerlinEngine::mainMenu(const void *param) {
	static const uint16 kMainMenuId = 0x0118;
	startMainMenu(BltShortId(kMainMenuId));
}

void MerlinEngine::fileMenu(const void *param) {
	static const uint16 kFileMenuId = 0x027A;
	startMenu(BltShortId(kFileMenuId));
}

void MerlinEngine::difficultyMenu(const void *param) {
	static const uint16 kDifficultyMenuId = 0x006B;
	startMenu(BltShortId(kDifficultyMenuId));
}

void MerlinEngine::hub(const void *param) {
	_currentCard.reset();
	const HubEntry *entry = reinterpret_cast<const HubEntry*>(param);
	_currentHub = entry;
	HubCard *card = new HubCard;
	card->init(&_graphics, _boltlib, BltShortId(entry->hubId));
	setCurrentCard(card);
}

void MerlinEngine::freeplayHub(const void *param) {
	_currentCard.reset();
	uint16 sceneId = *reinterpret_cast<const uint16*>(param);
	GenericMenuCard *card = new GenericMenuCard;
	card->init(&_graphics, _boltlib, BltShortId(sceneId));
	setCurrentCard(card);
}

// Hardcoded values from MERLIN.EXE:
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
//
// TODO: there are more: cursor, menus, etc.

template<class T>
static Card* makePuzzle(Graphics *graphics, Boltlib &boltlib, BltId resId) {
	T *card = new T;
	card->init(graphics, boltlib, resId);
	return card;
}

static const PuzzleEntry::PuzzleFunc makeActionPuzzle = makePuzzle<ActionPuzzle>;
static const PuzzleEntry::PuzzleFunc makeWordPuzzle = makePuzzle<WordPuzzle>;
static const PuzzleEntry::PuzzleFunc makeSlidingPuzzle = makePuzzle<SlidingPuzzle>;
static const PuzzleEntry::PuzzleFunc makeMemoryPuzzle = makePuzzle <MemoryPuzzle>;
static const PuzzleEntry::PuzzleFunc makeTangramPuzzle = makePuzzle<TangramPuzzle>;
static const PuzzleEntry::PuzzleFunc makeSynchPuzzle = makePuzzle<SynchPuzzle>;
static const PuzzleEntry::PuzzleFunc makeColorPuzzle = makePuzzle<ColorPuzzle>;

const HubEntry MerlinEngine::kStage1 = { 0x0C0B, 6, MerlinEngine::kStage1Puzzles };
const PuzzleEntry MerlinEngine::kStage1Puzzles[6] = {
	{ makeActionPuzzle,  0x4921, MKTAG('S', 'E', 'E', 'D') }, // seeds
	{ makeWordPuzzle,    0x61E3, MKTAG('G', 'R', 'A', 'V') }, // grave
	{ makeSlidingPuzzle, 0x313F, MKTAG('O', 'A', 'K', 'L') }, // oak leaf
	{ makeMemoryPuzzle,  0x865E, MKTAG('P', 'O', 'N', 'D') }, // pond
	{ makeActionPuzzle,  0x4D19, MKTAG('L', 'E', 'A', 'V') }, // leaves
	{ makeSlidingPuzzle, 0x353F, MKTAG('R', 'A', 'V', 'N') }, // raven
};

const HubEntry MerlinEngine::kStage2 = { 0x0D34, 9, MerlinEngine::kStage2Puzzles };
const PuzzleEntry MerlinEngine::kStage2Puzzles[9] = {
	{ makeSlidingPuzzle, 0x4140, MKTAG('R', 'T', 'T', 'L') }, // rattlesnake
	{ makeTangramPuzzle, 0x6D15, MKTAG('P', 'L', 'A', 'Q') }, // plaque
	{ makeActionPuzzle,  0x551C, MKTAG('S', 'N', 'O', 'W') }, // snow
	{ makeSynchPuzzle,   0x7D12, MKTAG('P', 'L', 'N', 'T') }, // planets
	{ makeWordPuzzle,    0x69E1, MKTAG('P', 'R', 'C', 'H') }, // parchment
	{ makeActionPuzzle,  0x5113, MKTAG('B', 'B', 'L', 'E') }, // bubbles
	{ makeSlidingPuzzle, 0x3D3F, MKTAG('S', 'K', 'L', 'T') }, // skeleton
	{ makeMemoryPuzzle,  0x8797, MKTAG('F', 'L', 'S', 'K') }, // flasks
	{ makeTangramPuzzle, 0x7115, MKTAG('M', 'I', 'R', 'R') }, // mirror
};

const HubEntry MerlinEngine::kStage3 = { 0x0E4F, 12, MerlinEngine::kStage3Puzzles };
const PuzzleEntry MerlinEngine::kStage3Puzzles[12] = {
	{ makeColorPuzzle,   0x8C13, MKTAG('W', 'N', 'D', 'W') }, // window
	{ makeTangramPuzzle, 0x7515, MKTAG('O', 'C', 'T', 'A') }, // octagon
	{ makeSynchPuzzle,   0x8512, MKTAG('S', 'P', 'R', 'T') }, // spirits
	{ makeColorPuzzle,   0x9014, MKTAG('S', 'T', 'A', 'R') }, // star
	{ makeSynchPuzzle,   0x8114, MKTAG('D', 'O', 'O', 'R') }, // door
	{ makeActionPuzzle,  0x5918, MKTAG('G', 'E', 'M', 'S') }, // gems
	{ makeSlidingPuzzle, 0x393F, MKTAG('C', 'S', 'T', 'L') }, // crystal
	{ makeActionPuzzle,  0x5D17, MKTAG('D', 'E', 'M', 'N') }, // demons
	{ makeTangramPuzzle, 0x7915, MKTAG('T', 'I', 'L', 'E') }, // tile
	{ makeSlidingPuzzle, 0x453F, MKTAG('S', 'P', 'I', 'D') }, // spider
	{ makeWordPuzzle,    0x65E1, MKTAG('T', 'B', 'L', 'T') }, // tablet
	{ makeMemoryPuzzle,  0x887B, MKTAG('S', 'T', 'L', 'C') }, // stalactites & stalagmites
};

static const uint32 kPlotMovieBMPR = MKTAG('B', 'M', 'P', 'R');
static const uint32 kPlotMovieINTR = MKTAG('I', 'N', 'T', 'R');
static const uint32 kPlotMoviePLOG = MKTAG('P', 'L', 'O', 'G');
static const uint32 kPlotMovieLABT = MKTAG('L', 'A', 'B', 'T');
static const uint32 kPlotMovieCAV1 = MKTAG('C', 'A', 'V', '1');
static const uint32 kPlotMovieFNLE = MKTAG('F', 'N', 'L', 'E');

static const uint16 kFreeplayScenes = 0x0600; // TODO: contains ID's for freeplay hubs
static const uint16 kFreeplayScene1 = 0x0337; // so stop hardcoding these
static const uint16 kFreeplayScene2 = 0x0446;
static const uint16 kFreeplayScene3 = 0x0555;

const MerlinEngine::Callback
MerlinEngine::kSequence[] = {
	// Pre-game menus
	{ &MerlinEngine::plotMovie, &kPlotMovieBMPR },
	{ &MerlinEngine::plotMovie, &kPlotMovieINTR },
	{ &MerlinEngine::mainMenu, nullptr },
	{ &MerlinEngine::fileMenu, nullptr },
	{ &MerlinEngine::difficultyMenu, nullptr },

	// Stage 1: Forest
	{ &MerlinEngine::plotMovie, &kPlotMoviePLOG },
	{ &MerlinEngine::hub, &MerlinEngine::kStage1 },

	// Stage 2: Laboratory
	{ &MerlinEngine::plotMovie, &kPlotMovieLABT },
	{ &MerlinEngine::hub, &MerlinEngine::kStage2 },

	// Stage 3: Cave
	{ &MerlinEngine::plotMovie, &kPlotMovieCAV1 },
	{ &MerlinEngine::hub, &MerlinEngine::kStage3 },

	// Finale movie is hidden until the game is fully implemented. 
	//{ &MerlinEngine::plotMovie, &kPlotMovieFNLE },

	{ &MerlinEngine::freeplayHub, &kFreeplayScene1 },
	{ &MerlinEngine::freeplayHub, &kFreeplayScene2 },
	{ &MerlinEngine::freeplayHub, &kFreeplayScene3 },
};

const int MerlinEngine::kSequenceSize =
	sizeof(MerlinEngine::kSequence) /
	sizeof(MerlinEngine::Callback);

} // End of namespace Bolt
