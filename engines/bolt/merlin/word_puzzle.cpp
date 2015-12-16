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

#include "bolt/merlin/word_puzzle.h"

namespace Bolt {

void WordPuzzle::init(Graphics *graphics, Boltlib &boltlib, BltId resId) {
	BltResourceList resourceList(boltlib, resId);
	Blt16BitValues difficulties(boltlib, resourceList[0].value);
	// There are three difficulties, choose one here
	BltResourceList difficulty(boltlib, BltShortId(difficulties[0].value)); // Difficulty 0
	_scene.load(graphics, boltlib, difficulty[19].value);
}

void WordPuzzle::enter(uint32 time) {
	_scene.enter();
}

Card::Signal WordPuzzle::handleEvent(const BoltEvent &event) {
	if (event.type == BoltEvent::Hover) {
		_scene.handleHover(event.point);
	}
	else if (event.type == BoltEvent::Click) {
		int buttonNum = _scene.getButtonAtPoint(event.point);
		return handleButtonClick(buttonNum);
	}

	return kNull;
}

Card::Signal WordPuzzle::handleButtonClick(int num) {
	debug(3, "Clicked button %d", num);
	// TODO: implement puzzle
	if (num != -1) {
		return kWin;
	}

	return kNull;
}

} // End of namespace Bolt
