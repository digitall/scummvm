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

#ifndef BOLT_MOVIE_STATE_H
#define BOLT_MOVIE_STATE_H

#include "bolt/bolt.h"

#include "common/ptr.h"

#include "bolt/movie.h"

namespace Common {
struct Event;
class String;
}

namespace Bolt {

class BoltEngine;

typedef Common::SharedPtr<class MovieState> MovieStatePtr;

class MovieState : public State {
public:
	static MovieStatePtr create(BoltEngine *engine, uint32 name);

	virtual void process(const Common::Event &event);

private:
	MovieState();

	BoltEngine *_engine;

	Movie _movie;
};

} // End of namespace Bolt

#endif
