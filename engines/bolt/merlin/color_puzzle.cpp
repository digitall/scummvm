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

void ColorPuzzle::init(Graphics *graphics, IBoltEventLoop *eventLoop, Boltlib &boltlib, BltId resId) {
	_graphics = graphics;
	_eventLoop = eventLoop;

	_morphPaletteMods = nullptr;

	BltResourceList resourceList;
	loadBltResourceArray(resourceList, boltlib, resId);
	BltId difficultiesId = resourceList[0].value;
	BltId sceneId = resourceList[3].value;

	_scene.load(graphics, boltlib, sceneId);

	BltU16Values difficultyIds;
	loadBltResourceArray(difficultyIds, boltlib, difficultiesId);
	// TODO: Load player's chosen difficulty
	BltResourceList difficulty;
	loadBltResourceArray(difficulty, boltlib, BltShortId(difficultyIds[0].value));
	BltId numStatesId = difficulty[0].value;
	BltId statePaletteModsId = difficulty[1].value;

	BltU8Values numStates;
	loadBltResourceArray(numStates, boltlib, numStatesId);
	BltResourceList statePaletteMods;
	loadBltResourceArray(statePaletteMods, boltlib, statePaletteModsId);

	for (int i = 0; i < kNumPieces; ++i) {
		Piece &p = _pieces[i];
		p.numStates = numStates[i].value;
		loadBltResourceArray(p.paletteMods, boltlib, statePaletteMods[i].value);
		// FIXME: What is initial state? Is it random or chosen from a predefined set?
		p.state = 1;
	}
}

void ColorPuzzle::enter() {
	_mode = kWaitForPlayer;
	_scene.enter();
	_morphPaletteMods = nullptr;

	for (int i = 0; i < kNumPieces; ++i) {
		setPieceState(i, _pieces[i].state); // Update display
	}
}

Card::Signal ColorPuzzle::handleEvent(const BoltEvent &event) {
	_curEvent = event;
	_signal = kNull;

	bool yield = false;
	while (!yield) {
		DriveResult result = drive();

		switch (result) {
		case kContinue:
			break;
		case kYield:
			yield = true;
			break;
		default:
			assert(false && "Invalid drive result");
			return Card::Signal::kInvalid;
		}
	}

	return _signal;
}

ColorPuzzle::DriveResult ColorPuzzle::handleButtonClick(int num) {
	debug(3, "Clicked button %d", num);
	if (num >= 0 && num < kNumPieces) {
		// TODO: change states according to puzzle definition
		morphPiece(num, (_pieces[num].state + 1) % _pieces[num].numStates);
		_mode = kChangeState;
		// Proceed to morphing mode; do not yield
		return kContinue;
	}
	else {
		// TODO: clicking outside of pieces should show the solution
		// TODO: check win condition: all pieces must be in state 0.
		_signal = kWin;
		return kYield;
	}

	return kYield;
}

ColorPuzzle::DriveResult ColorPuzzle::drive() {
	switch (_mode) {
	case kWaitForPlayer:
		return driveWaitForPlayer();
	case kChangeState:
		return driveChangeState();
	default:
		assert(false && "Invalid color puzzle mode");
		return kInvalidDriveResult;
	}
}

ColorPuzzle::DriveResult ColorPuzzle::driveWaitForPlayer() {
	if (_curEvent.type == BoltEvent::kHover) {
		_scene.handleHover(_curEvent.point);
		_curEvent = BoltEvent(); // eat event
	} else if (_curEvent.type == BoltEvent::kClick) {
		int buttonNum = _scene.getButtonAtPoint(_curEvent.point);
		DriveResult result = handleButtonClick(buttonNum);
		_curEvent = BoltEvent(); // eat event
		return result;
	}

	return DriveResult::kYield;
}

ColorPuzzle::DriveResult ColorPuzzle::driveChangeState() {
	// TODO: eliminate kDrive events in favor of smooth animation, timers, etc.
	if (_curEvent.type == BoltEvent::kDrive) {
		uint32 progress = _curEvent.eventTime - _morphStartTime;
		if (progress >= kMorphDuration) {
			applyPaletteMod(_graphics, kFore, *_morphPaletteMods, _morphEndState);
			// Transition back to accepting input
			_morphPaletteMods = nullptr;
			_mode = kWaitForPlayer;
			_graphics->markDirty();
			// Proceed to WaitForPlayer mode. Do NOT yield here.
			return DriveResult::kContinue;
		}
		else {
			applyPaletteModBlended(_graphics, kFore, *_morphPaletteMods,
				_morphStartState, _morphEndState,
				Common::Rational(progress, kMorphDuration));
			_graphics->markDirty();
			return DriveResult::kYield;
		}
	}

	return DriveResult::kYield;
}

void ColorPuzzle::setPieceState(int piece, int state) {
	_pieces[piece].state = state;
	applyPaletteMod(_graphics, kFore, _pieces[piece].paletteMods, state);
	_graphics->markDirty();
}

void ColorPuzzle::morphPiece(int piece, int state) {
	debug(3, "morphing piece %d to state %d", piece, state);
	int oldState = _pieces[piece].state;
	_pieces[piece].state = state;
	startMorph(&_pieces[piece].paletteMods, oldState, state);
}

void ColorPuzzle::startMorph(BltPaletteMods *paletteMods, int startState, int endState) {
	_morphStartTime = _eventLoop->getEventTime();
	_morphPaletteMods = paletteMods;
	_morphStartState = startState;
	_morphEndState = endState;
}

} // End of namespace Bolt
