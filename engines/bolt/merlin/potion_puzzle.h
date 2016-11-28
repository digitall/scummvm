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
	
struct BltPotionPuzzleComboTableElement {
	static const uint32 kType = kBltPotionPuzzleComboTable;
	static const uint kSize = 0x6;
	void load(const ConstSizedDataView<kSize> src, Boltlib &boltlib) {
		a = src.readInt8(0);
		b = src.readInt8(1);
		c = src.readInt8(2);
		d = src.readInt8(3);
		movie = src.readUint16BE(4);
	}

	int8 a;
	int8 b;
	int8 c;
	int8 d;
	uint16 movie;
};

typedef ScopedArray<BltPotionPuzzleComboTableElement> BltPotionPuzzleComboTable;

class PotionPuzzle : public Card {
public:
	// From Card
	virtual void init(MerlinGame *game, IBoltEventLoop *eventLoop, Boltlib &boltlib, BltId resId);
	virtual void enter();
	virtual Signal handleEvent(const BoltEvent &event);

private:
	enum DriveResult {
		kInvalidDriveResult,
		kContinue,
		kYield,
	};

	DriveResult drive();
	DriveResult driveWaitForPlayer();
	DriveResult driveChangeState();

	void draw();
	void drawIngredient(int num, Common::Point pos);
	Rect getIngredientHitbox(int num, Common::Point pos);
	void reactIngredients();

	enum Mode
	{
		kInvalidMode,
		kWaitForPlayer,
		kChangeState,
	};

	// TODO: Placing states should last as long as the "plunk" sound... I think.
	static const uint32 kPlacing1Time = 500;
	static const uint32 kPlacing2Time = 500;

	Mode _mode;
	BoltEvent _curEvent;
	Card::Signal _signal;

	MerlinGame *_game;
	Graphics *_graphics;
	BltImage _bgImage;
	BltPalette _bgPalette;
	Common::Point _origin;
	ScopedArray<BltImage> _ingredientImages;
	ScopedArray<Common::Point> _shelfPoints;
	Common::Point _basinPoints[3];
	BltPotionPuzzleComboTable _comboTable;
	ScopedArray<bool> _slotStates; // False: Empty; True: Filled
	int _bowlSlots[2]; // Ingredients in bowl

	bool _timeoutActive;
	uint32 _timeoutStart;
	uint32 _timeoutLength;

	int _requestedPiece; // 1: no requested piece
};

} // End of namespace Bolt

#endif
