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
	_morphPaletteMods = nullptr;

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
	_morphPaletteMods = nullptr;

	for (int i = 0; i < kNumPieces; ++i) {
		setPieceState(i, _pieces[i].state); // Update display
	}
}

Card::Signal ColorPuzzle::handleEvent(const BoltEvent &event) {
	if (isMorphing()) {
		// NOTE: original game does not allow opening right-click menu during morph animation
		if (event.type == BoltEvent::Tick) {
			driveMorph(event.time);
		}
	}
	else {
		if (event.type == BoltEvent::Hover) {
			_scene.handleHover(event.point);
		}
		if (event.type == BoltEvent::Click) {
			int buttonNum = _scene.getButtonAtPoint(event.point);
			return handleButtonClick(buttonNum, event.time);
		}
	}

	return kNull;
}

Card::Signal ColorPuzzle::handleButtonClick(int num, uint32 curTime) {
	debug(3, "Clicked button %d", num);
	if (num >= 0 && num < kNumPieces) {
		// TODO: change states according to puzzle definition
		morphPiece(num, (_pieces[num].state + 1) % _pieces[num].numStates, curTime);
	}
	else {
		// TODO: clicking outside of pieces should show the solution
		// TODO: check win condition: all pieces must be in state 0.
		return kWin;
	}

	return kNull;
}

void ColorPuzzle::setPieceState(int piece, int state) {
	_pieces[piece].state = state;
	applyPaletteMod(_graphics, _pieces[piece].paletteMods, state, kFore);
	_graphics->markDirty();
}

void ColorPuzzle::morphPiece(int piece, int state, uint32 curTime) {
	debug(3, "morphing piece %d to state %d", piece, state);
	int oldState = _pieces[piece].state;
	_pieces[piece].state = state;
	startMorph(&_pieces[piece].paletteMods, oldState, state, curTime);
}

void ColorPuzzle::startMorph(BltPaletteMods *paletteMods, int startState, int endState, uint32 curTime) {
	_morphStartTime = curTime;
	_morphPaletteMods = paletteMods;
	_morphStartState = startState;
	_morphEndState = endState;
}

bool ColorPuzzle::isMorphing() const {
	return _morphPaletteMods != nullptr;
}

void ColorPuzzle::driveMorph(uint32 curTime) {
	uint32 progress = curTime - _morphStartTime;
	if (progress >= kMorphDuration) {
		applyPaletteMod(_graphics, *_morphPaletteMods, _morphEndState, kFore);
		// Transition back to accepting input
		_morphPaletteMods = nullptr;
	}
	else {
		applyPaletteModMorph(_graphics, *_morphPaletteMods, _morphStartState, _morphEndState, kFore,
			Common::Rational(progress, kMorphDuration));
	}

	_graphics->markDirty();
}

} // End of namespace Bolt
