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

BoltEngine::BoltEngine(OSystem *const syst, const ADGameDescription *const gd) :
	Engine(syst)
{
	if (Common::String("merlin").compareTo(gd->gameId) == 0) {
		_game.reset(new MerlinGame);
	} else {
		assert(false && "BoltEngine does not support this game.");
	}
}

bool BoltEngine::hasFeature(const EngineFeature f) const {
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
			boltEvent.type = BoltEvent::kHover;
			boltEvent.eventTime = _eventTime;
			boltEvent.point = event.mouse;
			topLevelHandleEvent(boltEvent);
		}
		else if (event.type == Common::EVENT_LBUTTONDOWN) {
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::kClick;
			boltEvent.eventTime = _eventTime;
			boltEvent.point = event.mouse;
			topLevelHandleEvent(boltEvent);
		}
		else if (event.type == Common::EVENT_RBUTTONDOWN) {
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::kRightClick;
			boltEvent.eventTime = _eventTime;
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
				boltEvent.type = BoltEvent::kMovieTimer;
				boltEvent.eventTime = _eventTime;
				topLevelHandleEvent(boltEvent);
			} else if (_smoothAnimationRequested) {
				// FIXME: smooth animation events are handled rapidly and use 100% of the cpu.
				// Change this so smooth animation events are handled at a reasonable rate.
				_smoothAnimationRequested = false;
				BoltEvent boltEvent;
				boltEvent.type = BoltEvent::kSmoothAnimation;
				boltEvent.eventTime = _eventTime;
				topLevelHandleEvent(boltEvent);
			} else {
				// Emit Drive event
				// TODO: Eliminate Drive events in favor of Timers, SmoothAnimation and AudioEnded.
				// Generally, events signify things that are reacted to instead of polled.
				BoltEvent boltEvent;
				boltEvent.type = BoltEvent::kDrive;
				boltEvent.eventTime = _eventTime;
				topLevelHandleEvent(boltEvent);
			}
		}
	}

	return Common::kNoError;
}

uint32 BoltEngine::getEventTime() const {
	return _eventTime;
}

void BoltEngine::requestSmoothAnimation() {
	_smoothAnimationRequested = true;
}

void BoltEngine::setMovieTimer(const uint32 intervalMs) {
	_movieTimerActive = true;
	_movieTimerStart = _eventTime;
	_movieTimerInterval = intervalMs;
}

void BoltEngine::topLevelHandleEvent(const BoltEvent &event) {
	_game->handleEvent(event);
	_graphics.handleEvent(event);
	// TODO: Present after all pending events have been handled.
	_graphics.presentIfDirty();
}

} // End of namespace Bolt
