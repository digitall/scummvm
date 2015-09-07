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

#ifndef BOLT_MERLIN_H
#define BOLT_MERLIN_H

#include "bolt/bolt.h"

namespace Bolt {

class MerlinEngine : public SubEngine {
public:
	MerlinEngine();

	virtual void init(BoltEngine *engine);
	virtual void processEvent(const BoltEvent &event);

private:
	void initCursor();
	void resetSequence();
	void advanceSequence();
	void startMainMenu(BltLongId id);
	void startMenu(BltLongId id);

	BoltEngine *_engine;

	BltFile _boltlibBltFile;
	PfFile _maPfFile;

	CardPtr _currentCard;

	int _sequenceCursor;

	typedef void(*SequenceFunc)(MerlinEngine *self, uint32 param);
	struct SequenceEntry {
		SequenceFunc func;
		uint32 param;
	};

	static void PlayMovieFunc(MerlinEngine *self, uint32 param);
	static void MainMenuFunc(MerlinEngine *self, uint32 param);
	static void MenuFunc(MerlinEngine *self, uint32 param);
	static void PlotWarningFunc(MerlinEngine *self, uint32 param);

	static const SequenceEntry SEQUENCE[];
	static const size_t SEQUENCE_SIZE;
};

} // End of namespace Bolt

#endif
