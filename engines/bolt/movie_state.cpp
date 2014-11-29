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

#include "bolt/movie_state.h"

#include "bolt/bolt.h"

#include "common/endian.h"
#include "common/events.h"

namespace Bolt {

MovieStatePtr MovieState::create(BoltEngine *engine, uint32 name, StatePtr afterState) {

	MovieStatePtr self(new MovieState());

	self->_engine = engine;
	self->_afterState = afterState;

	self->_movie = Movie::create(engine, &engine->_maPfFile, name);

	return self;
}

MovieState::MovieState()
{ }

void MovieState::process(const Common::Event &event) {
	if (event.type == Common::EVENT_LBUTTONDOWN) {
		// Clicked, go to after-movie state
		_movie->stop();
		_engine->_state = _afterState;
	}
	else if (!_movie->process()) {
		// Movie done; go to after-movie state
		_engine->_state = _afterState;
	}
}

} // End of namespace Bolt
