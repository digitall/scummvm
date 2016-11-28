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

#include "bolt/merlin/potion_puzzle.h"

namespace Bolt {

struct BltPotionPuzzleDef {
	static const uint32 kType = kBltPotionPuzzle;
	static const uint kSize = 0x46;
	void load(const ConstSizedDataView<kSize> src, Boltlib &boltlib) {
		difficultiesId = BltId(src.readUint32BE(0));
		bgImageId = BltId(src.readUint32BE(4));
		bgPaletteId = BltId(src.readUint32BE(8));
		numShelfPoints = src.readUint16BE(0x16);
		shelfPointsId = BltId(src.readUint32BE(0x18));
		basinPointsId = BltId(src.readUint32BE(0x20));
		origin.x = src.readInt16BE(0x42);
		origin.y = src.readInt16BE(0x44);
	}

	BltId difficultiesId;
	BltId bgImageId;
	BltId bgPaletteId;
	uint16 numShelfPoints;
	BltId shelfPointsId;
	BltId basinPointsId;
	Common::Point origin;
};

struct BltPotionPuzzleSpritePointElement {
	static const uint32 kType = kBltPotionPuzzleSpritePoints;
	static const uint kSize = 0x4;
	void load(const ConstSizedDataView<kSize> src, Boltlib &boltlib) {
		pos.x = src.readInt16BE(0);
		pos.y = src.readInt16BE(2);
	}

	Common::Point pos;
};

typedef ScopedArray<BltPotionPuzzleSpritePointElement> BltPotionPuzzleSpritePoints;

struct BltPotionPuzzleDifficultyDef {
	static const uint32 kType = kBltPotionPuzzleDifficulty;
	static const uint kSize = 0xA;
	void load(const ConstSizedDataView<kSize> src, Boltlib &boltlib) {
		numIngredients = src.readUint16BE(0);
		ingredientImagesId = BltId(src.readUint32BE(2));
		comboTableListId = BltId(src.readUint32BE(6));
	}

	uint16 numIngredients;
	BltId ingredientImagesId;
	BltId comboTableListId;
};

struct BltPotionPuzzleComboTableListElement {
	static const uint32 kType = kBltPotionPuzzleComboTableList;
	static const uint kSize = 0x1E;
	void load(const ConstSizedDataView<kSize> src, Boltlib &boltlib) {
		numCombos = src.readUint16BE(0);
		comboTableId = BltId(src.readUint32BE(2));
	}

	uint16 numCombos;
	BltId comboTableId;
};

typedef ScopedArray<BltPotionPuzzleComboTableListElement> BltPotionPuzzleComboTableList;

void PotionPuzzle::init(MerlinGame *game, IBoltEventLoop *eventLoop, Boltlib &boltlib, BltId resId) {
	_game = game;
	_graphics = _game->getGraphics();

	BltPotionPuzzleDef puzzle;
	BltU16Values difficultyIds;
	BltPotionPuzzleDifficultyDef difficulty;
	BltResourceList ingredientImagesList;
	BltPotionPuzzleSpritePoints shelfPoints;
	BltPotionPuzzleSpritePoints basinPoints;
	BltPotionPuzzleComboTableList comboTableList;

	loadBltResource(puzzle, boltlib, resId);
	loadBltResourceArray(difficultyIds, boltlib, puzzle.difficultiesId);
	_bgImage.load(boltlib, puzzle.bgImageId);
	_bgPalette.load(boltlib, puzzle.bgPaletteId);
	
	BltId difficultyId = BltShortId(difficultyIds[0].value); // TODO: Use player's chosen difficulty
	loadBltResource(difficulty, boltlib, difficultyId);
	loadBltResourceArray(ingredientImagesList, boltlib, difficulty.ingredientImagesId);
	loadBltResourceArray(shelfPoints, boltlib, puzzle.shelfPointsId);
	loadBltResourceArray(basinPoints, boltlib, puzzle.basinPointsId);
	loadBltResourceArray(comboTableList, boltlib, difficulty.comboTableListId); // TODO: which combo table should we choose?
	loadBltResourceArray(_comboTable, boltlib, comboTableList[0].comboTableId);

	_origin = puzzle.origin;
	_bowlSlots[0] = -1;
	_bowlSlots[1] = -1;
	_mode = kWaitForPlayer;
	
	_slotStates.alloc(puzzle.numShelfPoints);
	for (int i = 0; i < puzzle.numShelfPoints; ++i) {
		_slotStates[i] = true;
	}

	_ingredientImages.alloc(difficulty.numIngredients);
	for (uint16 i = 0; i < difficulty.numIngredients; ++i) {
		_ingredientImages[i].load(boltlib, ingredientImagesList[i].value);
	}

	_shelfPoints.alloc(puzzle.numShelfPoints);
	for (uint16 i = 0; i < puzzle.numShelfPoints; ++i) {
		_shelfPoints[i] = shelfPoints[i].pos;
	}

	if (basinPoints.size() != 3) {
		error("Invalid number of basin points %d", basinPoints.size());
	}
	for (int i = 0; i < 3; ++i) {
		_basinPoints[i] = basinPoints[i].pos;
	}
}

void PotionPuzzle::enter() {
	draw();
}

Card::Signal PotionPuzzle::handleEvent(const BoltEvent &event) {
	_curEvent = event;
	_signal = kNull;

	bool yield = false;
	while (!yield) {
		DriveResult result = drive();

		switch (result) {
		case kContinue:
			break;
		case kYield:
			yield = true;
			break;
		default:
			assert(false && "Invalid drive result");
			return Card::Signal::kInvalid;
		}
	}

	return _signal;
}

PotionPuzzle::DriveResult PotionPuzzle::drive() {
	switch (_mode) {
	case kWaitForPlayer:
		return driveWaitForPlayer();
	case kChangeState:
		return driveChangeState();
	default:
		assert(false && "Invalid potion puzzle state");
		return kInvalidDriveResult;
	}
}

PotionPuzzle::DriveResult PotionPuzzle::driveWaitForPlayer() {
	if (_curEvent.type == BoltEvent::kClick) {
		// Determine which piece was clicked
		for (uint i = 0; i < _shelfPoints.size(); ++i) {
			if (getIngredientHitbox(i, _shelfPoints[i]).contains(_curEvent.point) && _slotStates[i]) {
				debug(3, "clicked piece %d", i);
				_requestedPiece = i;
				// TODO: play select piece sound
				_timeoutStart = _curEvent.eventTime;
				_timeoutLength = kPlacing1Time;
				_timeoutActive = true;
				_mode = kChangeState;
				_curEvent = BoltEvent(); // eat event
				// Continue to ChangeState mode
				return kContinue;
			}
		}
	} else if (_curEvent.type == BoltEvent::kRightClick) {
		_curEvent = BoltEvent(); // eat event
		_signal = kEnd;
		return kYield;
	}

	return kYield;
}

PotionPuzzle::DriveResult PotionPuzzle::driveChangeState() {
	// TODO: use timer events, eliminate Drive events
	if (_curEvent.type == BoltEvent::kDrive) {
		// Wait for timeout
		if (_timeoutActive) {
			uint32 delta = _curEvent.eventTime - _timeoutStart;
			if (delta >= _timeoutLength) {
				// Timeout finished; disable timeout and continue driving
				_timeoutActive = false;
				return kContinue;
			}

			// Timeout active; yield
			return kYield;
		}

		// Examine state and decide what action to take
		if (_bowlSlots[0] >= 0 && _bowlSlots[1] >= 0) {
			// Both bowl slots occupied, cause reaction
			reactIngredients();
		} else if (_requestedPiece >= 0) {
			// Piece selected; move piece to bowl
			_slotStates[_requestedPiece] = false;
			if (_bowlSlots[0] < 0) {
				_bowlSlots[0] = _requestedPiece;
			} else if (_bowlSlots[1] < 0) {
				_bowlSlots[1] = _requestedPiece;
			} else {
				assert(false && "Could not place potion ingredient");
			}
			_requestedPiece = -1;
			draw();

			// TODO: Play "plunk" sound
			_timeoutStart = _curEvent.eventTime;
			_timeoutLength = kPlacing2Time;
			_timeoutActive = true;

			// Continue into ChangeState mode with active timeout
			return kContinue;
		} else {
			// No action to take; change to WaitForPlayer mode
			_mode = kWaitForPlayer;
			return kContinue;
		}
	}

	return kYield;
}

void PotionPuzzle::draw() {
	applyPalette(_graphics, kBack, _bgPalette);
	_bgImage.drawAt(_graphics->getPlaneSurface(kBack), 0, 0, false);
	if (!_game->isInMovie()) {
		// Do not clear a movie's foreground.
		_graphics->clearPlane(kFore);
	}

	for (uint i = 0; i < _shelfPoints.size(); ++i) {
		if (_slotStates[i]) {
			drawIngredient(i, _shelfPoints[i]);
		}
	}

	if (_bowlSlots[0] >= 0 && _bowlSlots[1] < 0) {
		// Draw one ingredient in basin
		drawIngredient(_bowlSlots[0], _basinPoints[1]);
	} else if (_bowlSlots[0] < 0 && _bowlSlots[1] >= 0) {
		// Draw one ingredient in basin (slot 1)
		drawIngredient(_bowlSlots[1], _basinPoints[1]);
	} else if (_bowlSlots[0] >= 0 && _bowlSlots[1] >= 0) {
		// Draw two ingredients in basin
		// FIXME: basin points are wrong
		drawIngredient(_bowlSlots[0], _basinPoints[0]);
		drawIngredient(_bowlSlots[1], _basinPoints[2]);
	}

	_graphics->markDirty();
}

void PotionPuzzle::drawIngredient(int num, Common::Point pos) {
	pos -= _origin;
	// Anchor images by their south end
	pos.x -= _ingredientImages[num].getWidth() / 2;
	pos.y -= _ingredientImages[num].getHeight();
	_ingredientImages[num].drawAt(_graphics->getPlaneSurface(kBack),
		pos.x, pos.y, true);
}

Rect PotionPuzzle::getIngredientHitbox(int num, Common::Point pos) {
	pos -= _origin;
	// Anchor images by their south end
	pos.x -= _ingredientImages[num].getWidth() / 2;
	pos.y -= _ingredientImages[num].getHeight();
	Rect rc(0, 0, _ingredientImages[num].getWidth(), _ingredientImages[num].getHeight());
	rc.translate(pos.x, pos.y);
	return rc;
}

void PotionPuzzle::reactIngredients() {
	// Play potion movie and return to WaitForPlayer mode
	if (_bowlSlots[0] >= 0 && _bowlSlots[1] >= 0) {
		// FIXME: how does combo lookup actually work?
		// why are there multiple combo tables?
		// Find reaction
		uint reactionNum = 0;
		for (reactionNum = 0; reactionNum < _comboTable.size(); ++reactionNum) {
			const BltPotionPuzzleComboTableElement &combo = _comboTable[reactionNum];
			debug(3, "checking combo %d, %d, %d, %d, %d",
				(int)combo.a, (int)combo.b, (int)combo.c, (int)combo.d, (int)combo.movie);
			// -1 is "default" - if -1 is found in the combo table, it matches any ingredient.
			// (FIXME: The above might be true for the a field, but is it true for b?)
			// TODO: I believe c=-1, d=-1 marks the Win condition.
			if ((_bowlSlots[0] == combo.a || combo.a == -1) && _bowlSlots[1] == combo.b) {
				// Cause reaction NOW
				_bowlSlots[0] = combo.c;
				_bowlSlots[1] = combo.d;
				_game->startPotionMovie(combo.movie);
				break;
			}
		}
		if (reactionNum >= _comboTable.size()) {
			warning("No reaction found for ingredients %d, %d", _bowlSlots[0], _bowlSlots[1]);
			// Empty the bowl. This should never happen.
			_bowlSlots[0] = -1;
			_bowlSlots[1] = -1;
		}
	}
}

} // End of namespace Bolt
