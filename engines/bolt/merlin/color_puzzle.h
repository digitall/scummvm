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
	// TODO: this value probably comes from Boltlib.blt somewhere.
	// All color puzzles in Merlin's Apprentice have 4 pieces.
	static const int kNumPieces = 4;
	// FIXME: morph duration is probably set in game data
	// or it may last as long as the sound
	static const uint kMorphDuration = 500;

	enum DriveResult {
		kInvalidDriveResult,
		kContinue,
		kYield,
	};

	enum Mode {
		kInvalidMode,
		kWaitForPlayer,
		kTransition,
	};

	struct Piece {
		int numStates;
		BltPaletteMods palettes;
		int currentState;
	};

	DriveResult drive();
	DriveResult driveWaitForPlayer();
	DriveResult driveTransition();
	DriveResult handleButtonClick(int num);

	void enterWaitForPlayerMode();
	void enterTransitionMode();
	void eatCurrentEvent();
	void selectPiece(int piece);
	void setPieceState(int piece, int state);
	void morphPiece(int piece, int state);

	Graphics *_graphics;
	IBoltEventLoop *_eventLoop;
	Scene _scene;

	Mode _mode;
	BoltEvent _curEvent;
	Card::Signal _signal;

	Piece _pieces[kNumPieces];

	// MORPHING

	uint32 _morphStartTime;
	BltPaletteMods *_morphPaletteMods;
	int _morphStartState;
	int _morphEndState;
	void startMorph(BltPaletteMods *paletteMods, int startState, int endState);
};

} // End of namespace Bolt

#endif
