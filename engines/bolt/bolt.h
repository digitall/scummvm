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

#include "bolt/blt_file.h"
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

class BoltEngine;

class Card {
public:
	enum Status {
		Invalid,
		None,
		Ended,
	};

	virtual ~Card() { }
	virtual void enter() = 0;
	virtual Status processEvent(const BoltEvent &event) = 0;
};

typedef Common::ScopedPtr<Card> CardPtr;

class SubEngine {
public:
	virtual ~SubEngine() { }
	virtual void init(BoltEngine *engine) = 0;
	virtual void processEvent(const BoltEvent &event) = 0;
};

class BoltEngine : public Engine {
	friend class Movie;
	friend class Scene;
	friend class MerlinEngine;
public:
	BoltEngine(OSystem *syst, const ADGameDescription *gd);

	virtual bool hasFeature(EngineFeature f) const;

protected:
	virtual Common::Error run();

private:
	void processEvent(const BoltEvent &event);
	void scheduleDisplayUpdate();

	Graphics _graphics;
	bool _displayDirty;

	uint32 _eventTime; // time of current or last received event

	typedef Common::ScopedPtr<SubEngine> SubEnginePtr;
	SubEnginePtr _subEngine;
};

} // End of namespace Bolt

#endif
