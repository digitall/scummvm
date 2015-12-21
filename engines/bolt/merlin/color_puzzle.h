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
	void init(Graphics *graphics, Boltlib &boltlib, BltId resId);
	void enter(uint32 time);
	Signal handleEvent(const BoltEvent &event);

protected:
	Signal handleButtonClick(int num, uint32 curTime);

private:
	static const int kNumPieces = 4; // XXX: this value probably comes from game data somewhere

	Graphics *_graphics;
	Scene _scene;

	struct Piece {
		int numStates;
		BltPaletteMods paletteMods;
		int state;
	};
	
	Piece _pieces[kNumPieces];

	void setPieceState(int piece, int state);
	void morphPiece(int piece, int state, uint32 curTime);

	// MORPHING

	// FIXME: morph duration is probably set in game data
	// or it may last as long as the sound
	static const uint kMorphDuration = 500;
	uint32 _morphStartTime;
	BltPaletteMods *_morphPaletteMods;
	int _morphStartState;
	int _morphEndState;
	void startMorph(BltPaletteMods *paletteMods, int startState, int endState, uint32 curTime);
	bool isMorphing() const;
	void driveMorph(uint32 curTime);
};

} // End of namespace Bolt

#endif
