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

#include "bolt/bolt.h"

#include "common/error.h"
#include "common/events.h"
#include "common/system.h"
#include "graphics/palette.h"

#include "bolt/merlin/merlin.h"

namespace Bolt {

BoltEngine::BoltEngine(OSystem *syst, const ADGameDescription *gd) :
	Engine(syst)
{ }

bool BoltEngine::hasFeature(EngineFeature f) const {
	return
		(f == kSupportsRTL);
}

Common::Error BoltEngine::run() {
	_eventTime = getTotalPlayTime();

	_graphics.init(_system, _eventTime);
	init();

	// Main loop
	while (!shouldQuit()) {
		_eventTime = getTotalPlayTime();

		// TODO: Instead of constantly polling for events in a loop, design a
		// function to sleep until event or time-delay occurs. This will make
		// the game more power-efficient.
		Common::Event event;
		if (!_eventMan->pollEvent(event)) {
			event.type = Common::EVENT_INVALID;
		}

		if (event.type == Common::EVENT_MOUSEMOVE) {
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::Hover;
			boltEvent.time = _eventTime;
			boltEvent.point = event.mouse;
			topLevelHandleEvent(boltEvent);
		}
		else if (event.type == Common::EVENT_LBUTTONDOWN) {
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::Click;
			boltEvent.time = _eventTime;
			boltEvent.point = event.mouse;
			topLevelHandleEvent(boltEvent);
		}
		else {
			// Emit "tick" event
			// TODO: Eliminate Tick events in favor of Timer, AudioEnded, and
			// other stuff that can be reacted to instead of polled.
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::Tick;
			boltEvent.time = _eventTime;
			topLevelHandleEvent(boltEvent);
		}
	}

	return Common::kNoError;
}

void BoltEngine::topLevelHandleEvent(const BoltEvent &event) {
	_graphics.setTime(event.time);
	if (event.type == BoltEvent::Hover) {
		// Update cursor
		// TODO: Only update if cursor is visible (there is no way to query
		// system for cursor visibility status)
		_graphics.markDirty();
	}

	handleEvent(event);
	_graphics.presentIfDirty();
}

} // End of namespace Bolt
