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

#ifndef BOLT_MERLIN_MERLIN_H
#define BOLT_MERLIN_MERLIN_H

#include "bolt/bolt.h"
#include "bolt/movie.h"

namespace Bolt {
	
struct PuzzleEntry;

struct HubEntry {
	uint16 hubId;
	int numPuzzles;
	const PuzzleEntry *puzzles;
};

struct PuzzleEntry {
	typedef Card* (*PuzzleFunc)(Graphics *graphics, Boltlib &boltlib, BltId resId);
	PuzzleFunc puzzle;
	uint16 resId;
	uint32 winMovie;
};

class MerlinEngine : public BoltEngine {
public:
	MerlinEngine(OSystem *syst, const ADGameDescription *gd);

protected:
	// From BoltEngine
	virtual void init();
	virtual void handleEvent(const BoltEvent &event);

private:
	void initCursor();
	void resetSequence();
	void advanceSequence();
	void enterSequenceEntry();
	void startMainMenu(BltId id);
	void startMenu(BltId id);
	void startMovie(PfFile &pfFile, uint32 name);

	static void movieTrigger(void *param, uint16 triggerType);

	void handleEventInMovie(const BoltEvent &event);
	void handleEventInCard(const BoltEvent &event);
	void win();
	void puzzle(const PuzzleEntry *entry);

	Boltlib _boltlib;
	PfFile _maPf;
	PfFile _helpPf;
	PfFile _potionPf;
	PfFile _challdirPf;

	BltImage _cursorImage;

	CardPtr _currentCard;
	Movie _movie;

	void setCurrentCard(Card *card);
	void enterCurrentCard(bool cursorActive);

	int _sequenceCursor;
	const HubEntry *_currentHub;
	const PuzzleEntry *_currentPuzzle;

	typedef void (MerlinEngine::*CallbackFunc)(const void *param);
	struct Callback {
		CallbackFunc func;
		const void *param;
	};

	void plotMovie(const void *param);
	void mainMenu(const void *param);
	void fileMenu(const void *param);
	void difficultyMenu(const void *param);
	void hub(const void *param);
	void freeplayHub(const void *param);

	static const HubEntry kStage1;
	static const PuzzleEntry kStage1Puzzles[6];
	static const HubEntry kStage2;
	static const PuzzleEntry kStage2Puzzles[9];
	static const HubEntry kStage3;
	static const PuzzleEntry kStage3Puzzles[12];

	static const Callback kSequence[];
	static const int kSequenceSize;
};

} // End of namespace Bolt

#endif
