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
#include "engines/advancedDetector.h"

#include "bolt/merlin/merlin.h"

namespace Bolt {

BoltEngine::BoltEngine(OSystem *syst, const ADGameDescription *gd) :
	Engine(syst)
{
	if (Common::String("merlin").compareTo(gd->gameid) == 0) {
		_game.reset(new MerlinGame);
	} else {
		assert(false && "BoltEngine does not support this game.");
	}
}

bool BoltEngine::hasFeature(EngineFeature f) const {
	return
		(f == kSupportsRTL);
}

Common::Error BoltEngine::run() {
	assert(_game);

	_eventTime = getTotalPlayTime();
	_graphics.init(_system, this);
	_game->init(_system, &_graphics, _mixer, this);
	
	while (!shouldQuit()) {
		_eventTime = getTotalPlayTime();

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
		else if (event.type == Common::EVENT_RBUTTONDOWN) {
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::RightClick;
			boltEvent.time = _eventTime;
			boltEvent.point = event.mouse;
			topLevelHandleEvent(boltEvent);
		}
		else {
			const uint32 movieTimerDelta = _eventTime - _movieTimerStart;
			if (_movieTimerActive && movieTimerDelta >= _movieTimerInterval) {
				_movieTimerActive = false; // Event handler must set movie timer again if it wants more movie timer events.
				// FIXME: rewrite to be more robust. events with later times should never appear before events with earlier times.
				// Perhaps the "time" of timer events should be the time of handling, not the time of triggering.
				_eventTime = _movieTimerStart + _movieTimerInterval;
				BoltEvent boltEvent;
				boltEvent.type = BoltEvent::MovieTimer;
				boltEvent.time = _eventTime;
				topLevelHandleEvent(boltEvent);
			} else if (_animationFrameRequested) {
				// FIXME: animation frames are handled rapidly and use 100% of the cpu.
				// Change this so animation frames are handled at a reasonable pace.
				_animationFrameRequested = false;
				BoltEvent boltEvent;
				boltEvent.type = BoltEvent::AnimationFrame;
				boltEvent.time = _eventTime;
				topLevelHandleEvent(boltEvent);
			} else {
				// Emit "tick" event
				// TODO: Eliminate Tick events in favor of Timer, AudioEnded, and
				// other stuff that can be reacted to instead of polled.
				BoltEvent boltEvent;
				boltEvent.type = BoltEvent::Tick;
				boltEvent.time = _eventTime;
				topLevelHandleEvent(boltEvent);
			}
		}
	}

	return Common::kNoError;
}

uint32 BoltEngine::getEventTime() const {
	return _eventTime;
}

void BoltEngine::requestAnimationFrame() {
	_animationFrameRequested = true;
}

void BoltEngine::setMovieTimer(const uint32 intervalMs) {
	_movieTimerActive = true;
	_movieTimerStart = _eventTime;
	_movieTimerInterval = intervalMs;
}

void BoltEngine::topLevelHandleEvent(const BoltEvent &event) {
	_graphics.drive();
	if (event.type == BoltEvent::Hover) {
		// Update cursor
		// TODO: Only update if cursor is visible (there is no way to query
		// system for cursor visibility status)
		_graphics.markDirty();
	}
	_game->handleEvent(event);
	_graphics.presentIfDirty();
}

} // End of namespace Bolt
