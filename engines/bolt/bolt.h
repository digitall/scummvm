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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef BOLT_BOLT_H
#define BOLT_BOLT_H

#include "common/rect.h"

#include "engines/engine.h"

#include "bolt/blt_file.h"
#include "bolt/graphics.h"
#include "bolt/image.h"
#include "bolt/pf_file.h"

struct ADGameDescription;

namespace Common {
struct Event;
};

namespace Bolt {

// A Bolt::Rect differs from a Common::Rect in the following ways:
// - Attributes are stored in left, right, top, bottom order
// - All edges are inclusive, i.e. right and bottom are included in the
//   rectangle
struct Rect {
	Rect() : left(0), right(0), top(0), bottom(0) { }
	Rect(const byte *src) {
		left = (int16)READ_BE_UINT16(&src[0]);
		right = (int16)READ_BE_UINT16(&src[2]);
		top = (int16)READ_BE_UINT16(&src[4]);
		bottom = (int16)READ_BE_UINT16(&src[6]);
	}

	operator Common::Rect() const {
		return Common::Rect(left, top, right + 1, bottom + 1);
	}

	bool contains(const Common::Point &p) const {
		return p.x >= left && p.x <= right && p.y >= top && p.y <= bottom;
	}

	void translate(int16 x, int16 y) {
		left += x;
		right += x;
		top += y;
		bottom += y;
	}

	int16 left;
	int16 right;
	int16 top;
	int16 bottom;
};

class State {
public:
	virtual ~State() { }

	// BEWARE when changing engine state within process! State may be destroyed!
	// TODO: Design better system, perhaps a "changeStateLater" function on the
	// engine, or a return code for process.
	virtual void process(const Common::Event &event) = 0;

protected:
	State() { }
};

class BoltEngine : public Engine {
	friend class MenuState;
	friend class MovieState;
	friend class Movie;
	friend class Scene;
public:
	BoltEngine(OSystem *syst, const ADGameDescription *gd);

	virtual bool hasFeature(EngineFeature f) const;

protected:
	virtual Common::Error run();

private:
	Graphics _graphics;
	bool _displayDirty;

	BltFile _boltlibBltFile;
	PfFile _maPfFile;
	BltImage _cursorImage;

	typedef Common::ScopedPtr<State> StatePtr;
	StatePtr _state;

	void scheduleDisplayUpdate();

	void initCursor();
	void resetSequence();
	void endCard();

	typedef void (*SequenceFunc)(BoltEngine *self);

	struct SequenceEntry {
		SequenceFunc func;
		uint32 param;
	};

	static const SequenceEntry MERLIN_SEQUENCE[];
	static const size_t MERLIN_SEQUENCE_SIZE;
	static const SequenceEntry LABYRINTH_SEQUENCE[];
	static const size_t LABYRINTH_SEQUENCE_SIZE;

	static const SequenceEntry *const SEQUENCE;
	static const size_t SEQUENCE_SIZE;

	static void PlayMovieFunc(BoltEngine *self);
	static void MainMenuFunc(BoltEngine *self);
	static void MenuFunc(BoltEngine *self);
	static void PlotWarningFunc(BoltEngine *self);

	int _sequenceCursor;

	// Menus

	void startMainMenu(BltShortId mainMenuId);
	void startMenu(BltLongId menuId);

	BltResource _mainMenuRes;
};

} // End of namespace Bolt

#endif
