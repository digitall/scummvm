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
	void load(const byte *src, Boltlib &boltlib) {
		difficultiesId = BltId(READ_BE_UINT32(&src[0]));
		bgImageId = BltId(READ_BE_UINT32(&src[4]));
		bgPaletteId = BltId(READ_BE_UINT32(&src[8]));
		numShelfPoints = READ_BE_UINT16(&src[0x16]);
		shelfPointsId = BltId(READ_BE_UINT32(&src[0x18]));
		origin.x = READ_BE_INT16(&src[0x42]);
		origin.y = READ_BE_INT16(&src[0x44]);
	}

	BltId difficultiesId;
	BltId bgImageId;
	BltId bgPaletteId;
	uint16 numShelfPoints;
	BltId shelfPointsId;
	Common::Point origin;
};

struct BltPotionPuzzleIngredientPosElement {
	static const uint32 kType = kBltPotionPuzzleIngredientPos;
	static const uint kSize = 0x4;
	void load(const byte *src, Boltlib &boltlib) {
		pos.x = READ_BE_INT16(&src[0]);
		pos.y = READ_BE_INT16(&src[2]);
	}

	Common::Point pos;
};

typedef ScopedArray<BltPotionPuzzleIngredientPosElement> BltPotionPuzzleIngredientPos;

struct BltPotionPuzzleDifficultyDef {
	static const uint32 kType = kBltPotionPuzzleDifficulty;
	static const uint kSize = 0xA;
	void load(const byte *src, Boltlib &boltlib) {
		numIngredients = READ_BE_UINT16(&src[0]);
		ingredientImagesId = BltId(READ_BE_UINT32(&src[2]));
	}

	uint16 numIngredients;
	BltId ingredientImagesId;
};

void PotionPuzzle::init(MerlinGame *game, Boltlib &boltlib, BltId resId) {
	_game = game;
	_graphics = _game->getGraphics();

	BltPotionPuzzleDef puzzle;
	BltU16Values difficultyIds;
	BltPotionPuzzleDifficultyDef difficulty;
	BltResourceList ingredientImagesList;
	BltPotionPuzzleIngredientPos ingredientPos;

	loadBltResource(puzzle, boltlib, resId);
	loadBltResourceArray(difficultyIds, boltlib, puzzle.difficultiesId);
	_bgImage.load(boltlib, puzzle.bgImageId);
	_bgPalette.load(boltlib, puzzle.bgPaletteId);
	
	BltId difficultyId = BltShortId(difficultyIds[0].value); // TODO: Use player's chosen difficulty
	loadBltResource(difficulty, boltlib, difficultyId);
	loadBltResourceArray(ingredientImagesList, boltlib, difficulty.ingredientImagesId);
	loadBltResourceArray(ingredientPos, boltlib, puzzle.shelfPointsId);

	_origin = puzzle.origin;
	_bowlSlots[0] = -1;
	_bowlSlots[1] = -1;
	_state = STATE_ACCEPTING_INPUT;
	
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
		_shelfPoints[i] = ingredientPos[i].pos;
	}
}

void PotionPuzzle::enter(uint32 time) {
	draw();
}

Card::Signal PotionPuzzle::handleEvent(const BoltEvent &event) {
	// TODO: implement
	// XXX: play all potion movies in sequence
	switch (_state)
	{
	case STATE_ACCEPTING_INPUT:
		if (event.type == BoltEvent::Click) {
			// Determine which piece was clicked
			for (int i = 0; i < _shelfPoints.size(); ++i) {
				if (getIngredientHitbox(i, _shelfPoints[i]).contains(event.point)) {
					// Enter STATE_PLACING_1
					debug(3, "clicked piece %d", i);
					_clickedPiece = i;
					_timeoutStart = event.time;
					_state = STATE_PLACING_1;
				}
			}
		}
		return kNull;
	case STATE_PLACING_1:
	{
		uint32 delta = event.time - _timeoutStart;
		if (delta >= kPlacing1Time) {
			// Enter STATE_PLACING_2
			_slotStates[_clickedPiece] = false;
			_bowlSlots[0] = _clickedPiece;
			draw();
			_timeoutStart = event.time;
			_state = STATE_PLACING_2;
		}
		return kNull;
	}
	case STATE_PLACING_2:
	{
		uint32 delta = event.time - _timeoutStart;
		if (delta >= kPlacing2Time) {
			// Play potion movie and return to STATE_ACCEPTING_INPUT
			// TODO: Play correct potion movie. Movie is determined by looking up ingredients in a table.
			_bowlSlots[0] = -1;
			_game->startPotionMovie(0);
			_state = STATE_ACCEPTING_INPUT;
		}
		return kNull;
	}
	}

	assert(false); // Unreachable
	return kEnd;
}

void PotionPuzzle::draw() {
	applyPalette(_graphics, kBack, _bgPalette);
	_bgImage.drawAt(_graphics->getPlaneSurface(kBack), 0, 0, false);
	if (!_game->isInMovie()) {
		// Do not clear a movie's foreground.
		_graphics->clearPlane(kFore);
	}

	for (int i = 0; i < _shelfPoints.size(); ++i) {
		if (_slotStates[i]) {
			drawIngredient(i, _shelfPoints[i]);
		}
	}

	for (int i = 0; i < 2; ++i) {
		if (_bowlSlots[i] != -1) {
			int ingredient = _bowlSlots[i];
			// TODO: Draw pieces in bowl at correct position.
			drawIngredient(ingredient, Common::Point(160, 100));
		}
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

} // End of namespace Bolt
