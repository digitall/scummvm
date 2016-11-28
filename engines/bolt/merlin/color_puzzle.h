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

#ifndef BOLT_MERLIN_COLOR_PUZZLE_H
#define BOLT_MERLIN_COLOR_PUZZLE_H

#include "bolt/merlin/merlin.h"
#include "bolt/scene.h"

namespace Bolt {

class ColorPuzzle : public Card {
public:
	void init(Graphics *graphics, IBoltEventLoop *eventLoop, Boltlib &boltlib, BltId resId);
	void enter();
	Signal handleEvent(const BoltEvent &event);

private:
	// Action to take after drive()
	enum DriveResult {
		// Invalid; this must never occur
		kInvalidDriveResult,
		// Continue driving
		kContinue,
		// Yield and return _signal from handleEvent
		kYield,
	};

	DriveResult drive();
	DriveResult driveWaitForPlayer();
	DriveResult driveChangeState();
	DriveResult handleButtonClick(int num);

	// The current event being handled
	BoltEvent _curEvent;
	// When drive() yields, this is the signal to return from handleEvent
	Card::Signal _signal;

	static const int kNumPieces = 4; // XXX: this value probably comes from game data somewhere. All color puzzles in Merlin's Apprentice have 4 pieces.

	Graphics *_graphics;
	IBoltEventLoop *_eventLoop;
	Scene _scene;

	// Current mode of puzzle
	enum Mode {
		// Invalid mode; this must never occur
		kInvalidMode,
		// Wait for player input
		kWaitForPlayer,
		// Play animations for changing puzzle state
		kChangeState,
	};

	Mode _mode;

	struct Piece {
		int numStates;
		BltPaletteMods paletteMods;
		int state;
	};
	
	Piece _pieces[kNumPieces];

	void setPieceState(int piece, int state);
	void morphPiece(int piece, int state);

	// MORPHING

	// FIXME: morph duration is probably set in game data
	// or it may last as long as the sound
	static const uint kMorphDuration = 500;
	uint32 _morphStartTime;
	BltPaletteMods *_morphPaletteMods;
	int _morphStartState;
	int _morphEndState;
	void startMorph(BltPaletteMods *paletteMods, int startState, int endState);
};

} // End of namespace Bolt

#endif
