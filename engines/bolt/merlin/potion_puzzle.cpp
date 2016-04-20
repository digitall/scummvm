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
		numIngredientPos = READ_BE_UINT16(&src[0x16]);
		ingredientPosId = BltId(READ_BE_UINT32(&src[0x18]));
		origin.x = READ_BE_INT16(&src[0x42]);
		origin.y = READ_BE_INT16(&src[0x44]);
	}

	BltId difficultiesId;
	BltId bgImageId;
	BltId bgPaletteId;
	uint16 numIngredientPos;
	BltId ingredientPosId;
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
	loadBltResource(puzzle, boltlib, resId);
	_bgImage.load(boltlib, puzzle.bgImageId);
	_bgPalette.load(boltlib, puzzle.bgPaletteId);
	_origin = puzzle.origin;
	_progress = 0;

	BltU16Values difficultyIds;
	loadBltResourceArray(difficultyIds, boltlib, puzzle.difficultiesId);
	BltId difficultyId = BltShortId(difficultyIds[0].value); // TODO: Use player's chosen difficulty

	BltPotionPuzzleDifficultyDef difficulty;
	loadBltResource(difficulty, boltlib, difficultyId);

	BltResourceList ingredientImagesList;
	loadBltResourceArray(ingredientImagesList, boltlib, difficulty.ingredientImagesId);
	_ingredientImages.alloc(difficulty.numIngredients);
	for (uint16 i = 0; i < difficulty.numIngredients; ++i) {
		_ingredientImages[i].load(boltlib, ingredientImagesList[i].value);
	}

	BltPotionPuzzleIngredientPos ingredientPos;
	loadBltResourceArray(ingredientPos, boltlib, puzzle.ingredientPosId);
	_ingredientPos.alloc(puzzle.numIngredientPos);
	for (uint16 i = 0; i < puzzle.numIngredientPos; ++i) {
		_ingredientPos[i] = ingredientPos[i].pos;
	}
}

void PotionPuzzle::enter(uint32 time) {
	applyPalette(_graphics, kBack, _bgPalette);
	_bgImage.drawAt(_graphics->getPlaneSurface(kBack), 0, 0, false);
	for (uint16 i = 0; i < _ingredientPos.size(); ++i) {
		Common::Point pos = _ingredientPos[i] - _origin;
		// XXX: Try anchoring images by their bottom middle end...
		pos.x -= _ingredientImages[i].getWidth() / 2;
		pos.y -= _ingredientImages[i].getHeight();
		_ingredientImages[i].drawAt(_graphics->getPlaneSurface(kBack),
			pos.x, pos.y, true);
	}
}

Card::Signal PotionPuzzle::handleEvent(const BoltEvent &event) {
	// TODO: implement
	// XXX: play all potion movies in sequence
	_game->startPotionMovie(_progress);
	++_progress;
	if (_progress >= MerlinGame::kNumPotionMovies) {
		return kEnd;
	}

	return kNull;
}

} // End of namespace Bolt
