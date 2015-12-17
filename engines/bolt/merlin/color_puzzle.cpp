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

#include "bolt/merlin/color_puzzle.h"

namespace Bolt {

void ColorPuzzle::init(Graphics *graphics, Boltlib &boltlib, BltId resId) {
	_graphics = graphics;

	BltResourceList resourceList(boltlib, resId);
	BltId difficultiesId = resourceList[0].value;
	BltId sceneId = resourceList[3].value;

	_scene.load(graphics, boltlib, sceneId);

	BltU16Values difficultyIds(boltlib, difficultiesId);
	// TODO: Load player's chosen difficulty
	BltResourceList difficulty(boltlib, BltShortId(difficultyIds[0].value));
	BltId numStatesId = difficulty[0].value;
	BltId statePaletteModsId = difficulty[1].value;

	BltU8Values numStates(boltlib, numStatesId);
	BltResourceList statePaletteMods(boltlib, statePaletteModsId);

	for (int i = 0; i < kNumPieces; ++i) {
		Piece &p = _pieces[i];
		p.numStates = numStates[i].value;
		p.paletteMods.load(boltlib, statePaletteMods[i].value);
		// FIXME: What is initial state? Is it random or chosen from a predefined set?
		p.state = 1;
	}
}

void ColorPuzzle::enter(uint32 time) {
	_scene.enter();

	for (int i = 0; i < kNumPieces; ++i) {
		setPieceState(i, _pieces[i].state); // Update display
	}
}

Card::Signal ColorPuzzle::handleEvent(const BoltEvent &event) {
	if (event.type == BoltEvent::Hover) {
		_scene.handleHover(event.point);
	}
	if (event.type == BoltEvent::Click) {
		int buttonNum = _scene.getButtonAtPoint(event.point);
		return handleButtonClick(buttonNum);
	}

	return kNull;
}

Card::Signal ColorPuzzle::handleButtonClick(int num) {
	debug(3, "Clicked button %d", num);
	if (num >= 0 && num < kNumPieces) {
		// TODO: implement color morphing animation
		// TODO: change states according to puzzle definition
		setPieceState(num, (_pieces[num].state + 1) % _pieces[num].numStates);
	}
	else {
		// TODO: check win condition: all pieces must be in state 0.
		return kWin;
	}

	return kNull;
}

void ColorPuzzle::setPieceState(int piece, int state) {
	debug(3, "setting piece %d to state %d", piece, state);
	_pieces[piece].state = state;
	applyPaletteMod(_graphics, _pieces[piece].paletteMods, state, kFore);
	_graphics->markDirty();
}

} // End of namespace Bolt
