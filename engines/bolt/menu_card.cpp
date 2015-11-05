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

#include "bolt/menu_card.h"

#include "common/events.h"
#include "common/system.h"
#include "graphics/surface.h"

#include "bolt/bolt.h"

namespace Bolt {

void MenuCard::init(BoltEngine *engine, BltFile &bltFile, BltId menuId) {
	_engine = engine;
	_scene.load(engine, bltFile, menuId);
}

void MenuCard::enter() {
	_scene.enter();
}

Card::Status MenuCard::processEvent(const BoltEvent &event) {
	if (event.type == BoltEvent::Click) {
		int buttonNum = _scene.getButtonAtPoint(event.point);
		debug(3, "Clicked button %d", buttonNum);
		return processButtonClick(buttonNum);
	}
	else {
		_scene.process();
	}

	return None;
}

void GenericMenuCard::init(BoltEngine *engine, BltFile &bltFile, BltId id) {
	MenuCard::init(engine, bltFile, id);
}

Card::Status GenericMenuCard::processButtonClick(int num) {
	// Generic behavior: Just end the card.
	return Ended;
}

} // End of namespace Bolt
