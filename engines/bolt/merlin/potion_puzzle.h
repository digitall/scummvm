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

#ifndef BOLT_MERLIN_POTION_PUZZLE_H
#define BOLT_MERLIN_POTION_PUZZLE_H

#include "bolt/merlin/merlin.h"
#include "bolt/boltlib/palette.h"

namespace Bolt {

class PotionPuzzle : public Card {
public:
	void init(MerlinGame *game, Boltlib &boltlib, BltId resId);

	// From Card
	void enter(uint32 time);
	Signal handleEvent(const BoltEvent &event);

private:
	void draw();
	void drawIngredient(int num, Common::Point pos);
	Rect getIngredientHitbox(int num, Common::Point pos);

	enum State
	{
		STATE_ACCEPTING_INPUT,
		STATE_PLACING_1,
		STATE_PLACING_2,
	};

	// TODO: Placing states should last as long as the "plunk" sound... I think.
	static const uint32 kPlacing1Time = 500;
	static const uint32 kPlacing2Time = 500;

	MerlinGame *_game;
	Graphics *_graphics;
	BltImage _bgImage;
	BltPalette _bgPalette;
	Common::Point _origin;
	ScopedArray<BltImage> _ingredientImages;
	ScopedArray<Common::Point> _shelfPoints;
	ScopedArray<bool> _slotStates; // False: Empty; True: Filled
	int _bowlSlots[2]; // Ingredients in bowl
	State _state;
	uint32 _timeoutStart;

	int _clickedPiece;
};

} // End of namespace Bolt

#endif
