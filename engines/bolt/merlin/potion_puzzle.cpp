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

struct BltPotionPuzzleStruct { // type 59
	static const uint32 kType = kBltPotionPuzzle;
	static const uint kSize = 0x46;
	void load(const byte *src, Boltlib &boltlib) {
		bgImageId = BltId(READ_BE_UINT32(&src[4]));
		bgPaletteId = BltId(READ_BE_UINT32(&src[8]));
	}

	BltId bgImageId;
	BltId bgPaletteId;
};

typedef BltLoader<BltPotionPuzzleStruct> BltPotionPuzzle;

void PotionPuzzle::init(MerlinGame *game, Boltlib &boltlib, BltId resId) {
	_game = game;
	_graphics = _game->getGraphics();

	BltPotionPuzzle puzzle(boltlib, resId);
	_bgImage.load(boltlib, puzzle->bgImageId);
	_bgPalette.load(boltlib, puzzle->bgPaletteId);
	_progress = 0;
}

void PotionPuzzle::enter(uint32 time) {
	applyPalette(_graphics, kBack, _bgPalette);
	_bgImage.drawAt(_graphics->getPlaneSurface(kBack), 0, 0, false);
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
