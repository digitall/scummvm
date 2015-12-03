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

#include "bolt/merlin/memory_puzzle.h"

#include "bolt/blt_file.h"

namespace Bolt {

void MemoryPuzzle::init(Graphics *graphics, BltFile &boltlib, BltId resId) {
	BltResourceList resourceList(boltlib, resId);
	BltId sceneId = resourceList[1].value;
	_scene.load(graphics, boltlib, sceneId);
}

void MemoryPuzzle::enter() {
	_scene.enter();
}

Card::Signal MemoryPuzzle::processEvent(const BoltEvent &event) {
	if (event.type == BoltEvent::Hover) {
		_scene.handleHover(event.point);
	}
	else if (event.type == BoltEvent::Click) {
		int buttonNum = _scene.getButtonAtPoint(event.point);
		return processButtonClick(buttonNum);
	}

	return kNull;
}

Card::Signal MemoryPuzzle::processButtonClick(int num) {
	debug(3, "Clicked button %d", num);
	// TODO: implement puzzle
	if (num != -1) {
		return kWin;
	}

	return kNull;
}

} // End of namespace Bolt
