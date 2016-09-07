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

void BoltEventLoop::init(BoltEngine *engine, IBoltEventHandler *handler) {
	_engine = engine;
	_handler = handler;
	_eventTime = _engine->getTotalPlayTime();
}

void BoltEventLoop::run() {
	bool breakout = false;
	while (!breakout && !_engine->shouldQuit())
	{
		_eventTime = _engine->getTotalPlayTime();

		Common::Event event;
		if (!_engine->getEventManager()->pollEvent(event)) {
			event.type = Common::EVENT_INVALID;
		}

		if (event.type == Common::EVENT_MOUSEMOVE) {
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::Hover;
			boltEvent.time = _eventTime;
			boltEvent.point = event.mouse;
			_handler->handleEvent(boltEvent);
		}
		else if (event.type == Common::EVENT_LBUTTONDOWN) {
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::Click;
			boltEvent.time = _eventTime;
			boltEvent.point = event.mouse;
			_handler->handleEvent(boltEvent);
		}
		else if (event.type == Common::EVENT_RBUTTONDOWN) {
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::RightClick;
			boltEvent.time = _eventTime;
			boltEvent.point = event.mouse;
			_handler->handleEvent(boltEvent);
		}
		else {
			// Emit "tick" event
			// TODO: Eliminate Tick events in favor of Timer, AudioEnded, and
			// other stuff that can be reacted to instead of polled.
			BoltEvent boltEvent;
			boltEvent.type = BoltEvent::Tick;
			boltEvent.time = _eventTime;
			_handler->handleEvent(boltEvent);
		}
	}
}

uint32 BoltEventLoop::getEventTime() const {
	return _eventTime;
}

BoltEngine::BoltEngine(OSystem *syst, const ADGameDescription *gd) :
	Engine(syst)
{
	if (Common::String("merlin").compareTo(gd->gameid) == 0) {
		_game.reset(new MerlinGame);
	}
}

bool BoltEngine::hasFeature(EngineFeature f) const {
	return
		(f == kSupportsRTL);
}

Common::Error BoltEngine::run() {
	class Handler : public IBoltEventHandler
	{
	public:
		Handler(BoltEngine *engine) : _engine(engine) { }
		virtual void handleEvent(const BoltEvent &event) {
			_engine->topLevelHandleEvent(event);
		}
	private:
		BoltEngine *_engine;
	};

	Handler handler(this);
	_eventLoop.init(this, &handler);

	_graphics.init(_system, _eventLoop.getEventTime());
	_game->init(_system, &_graphics, _mixer, &_eventLoop);

	_eventLoop.run();

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
	_game->handleEvent(event);
	_graphics.presentIfDirty();
}

} // End of namespace Bolt
