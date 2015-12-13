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

#include "bolt/merlin/sliding_puzzle.h"

namespace Bolt {

struct BltSlidingPuzzleStruct { // type 44
	static const uint32 kType = kBltSlidingPuzzle;
	static const uint kSize = 0xC;
	void load(const byte *src, Boltlib &bltFile) {
		unk1 = READ_BE_UINT16(&src[0]);
		difficulty1 = BltShortId(READ_BE_UINT16(&src[2]));
		unk2 = READ_BE_UINT16(&src[4]);
		difficulty2 = BltShortId(READ_BE_UINT16(&src[6]));
		unk3 = READ_BE_UINT16(&src[8]);
		difficulty3 = BltShortId(READ_BE_UINT16(&src[0xA]));
	}

	uint16 unk1;
	BltShortId difficulty1;
	uint16 unk2;
	BltShortId difficulty2;
	uint16 unk3;
	BltShortId difficulty3;
};

typedef BltLoader<BltSlidingPuzzleStruct> BltSlidingPuzzle;

void SlidingPuzzle::init(Graphics *graphics, Boltlib &boltlib, BltId resId) {
	BltResourceList resourceList(boltlib, resId);
	BltSlidingPuzzle slidingPuzzleInfo(boltlib, resourceList[1].value);
	// TODO: select proper difficulty based on player setting
	BltResourceList difficultyInfo(boltlib, slidingPuzzleInfo->difficulty1); // Ex: 3A34, 3B34, 3C34

	_scene.load(graphics, boltlib, difficultyInfo[1].value);
}

void SlidingPuzzle::enter() {
	_scene.enter();
}

Card::Signal SlidingPuzzle::handleEvent(const BoltEvent &event) {
	if (event.type == BoltEvent::Hover) {
		_scene.handleHover(event.point);
	}
	else if (event.type == BoltEvent::Click) {
		int buttonNum = _scene.getButtonAtPoint(event.point);
		return handleButtonClick(buttonNum);
	}

	return kNull;
}

Card::Signal SlidingPuzzle::handleButtonClick(int num) {
	debug(3, "Clicked button %d", num);
	// TODO: implement puzzle
	if (num != -1) {
		return kWin;
	}

	return kNull;
}

} // End of namespace Bolt
