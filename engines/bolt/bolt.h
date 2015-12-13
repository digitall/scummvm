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

#ifndef BOLT_BOLT_H
#define BOLT_BOLT_H

#include "common/rect.h"

#include "engines/engine.h"

#include "bolt/boltlib/boltlib.h"
#include "bolt/graphics.h"
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
		left = READ_BE_INT16(&src[0]);
		right = READ_BE_INT16(&src[2]);
		top = READ_BE_INT16(&src[4]);
		bottom = READ_BE_INT16(&src[6]);
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

struct BoltEvent {
	enum Type {
		Invalid,
		Hover,
		Click,
		Tick,
		// TODO: Timer, AudioEnded
	};

	BoltEvent() : type(Invalid) { }

	Type type;
	uint32 time;
	Common::Point point;
};

class Card {
public:
	// Signal codes returned by cards after handling an event
	enum Signal {
		kInvalid = -1,
		kNull = 0,
		kEnd,
		kWin,
		kPlayHelp,
		kPlayTour,
		kPlayCredits,
		kEnterPuzzleX = 100, // 100-1xx: enter puzzle xx
	};

	virtual ~Card() { }
	virtual void enter() = 0;
	virtual Signal handleEvent(const BoltEvent &event) = 0;
};

typedef Common::ScopedPtr<Card> CardPtr;

class BoltEngine : public Engine {
public:
	// From Engine
	virtual bool hasFeature(EngineFeature f) const;

protected:
	BoltEngine(OSystem *syst, const ADGameDescription *gd);

	// From Engine
	virtual Common::Error run();

	// Provided by subclass
	virtual void init() = 0;
	virtual void handleEvent(const BoltEvent &event) = 0;

	Graphics _graphics;
	uint32 _eventTime; // time of current or last received event

private:
	void topLevelHandleEvent(const BoltEvent &event);
};

} // End of namespace Bolt

#endif
