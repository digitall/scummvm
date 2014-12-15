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

#include "bolt/menu_state.h"

#include "common/events.h"
#include "common/system.h"
#include "graphics/surface.h"

#include "bolt/bolt.h"

namespace Bolt {

MenuStatePtr MenuState::create(BoltEngine *engine, BltLongId menuId) {
	MenuStatePtr self(new MenuState());
	self->init(engine, menuId);
	return self;
}

MenuState::MenuState()
{ }

void MenuState::init(BoltEngine *engine, BltLongId menuId) {
	_engine = engine;
	_scene.init(engine, &_engine->_boltlibBltFile, menuId);
	draw();
}

void MenuState::process(const Common::Event &event) {

	if (event.type == Common::EVENT_MOUSEMOVE) {
		draw();
	}

	// XXX: on click, leave menu
	// TODO: process buttons
	if (event.type == Common::EVENT_LBUTTONDOWN) {
		_engine->endCard();
	}
}

void MenuState::draw() {
	_scene.draw();
}

} // End of namespace Bolt
