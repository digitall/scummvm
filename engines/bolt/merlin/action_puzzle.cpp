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

#include "bolt/merlin/action_puzzle.h"

namespace Bolt {

Card* ActionPuzzle::make(Graphics *graphics, BltFile &boltlib, BltId resId) {
	ActionPuzzle *card = new ActionPuzzle;
	card->init(graphics, boltlib, resId);
	return card;
}

void ActionPuzzle::init(Graphics *graphics, BltFile &boltlib, BltId resId) {
	_graphics = graphics;

	BltResourceList resourceList(boltlib, resId);
	BltId bgImageId = resourceList[2].value;
	BltId paletteId = resourceList[3].value;

	_bgImage.load(boltlib, bgImageId);
	_palette.load(boltlib, paletteId);
}

void ActionPuzzle::enter() {
	if (_palette) {
		_palette.set(*_graphics, BltPalette::kBack);
	}
	_bgImage.drawAt(_graphics->getBackPlane().getSurface(), 0, 0, false);
	_graphics->markDirty();
}

Card::Signal ActionPuzzle::processEvent(const BoltEvent &event) {
	if (event.type == BoltEvent::Click) {
		// TODO: implement puzzle
		return kWin;
	}

	return kNull;
}

} // End of namespace Bolt
