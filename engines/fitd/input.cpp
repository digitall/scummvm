/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/events.h"
#include "common/system.h"
#include "fitd/input.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"

namespace Fitd {

static void handleKeyDown(const Common::Event &event) {
	switch (event.kbd.keycode) {
	case Common::KEYCODE_RETURN:
		g_engine->_engine->key = 0x1C;
		break;
	case Common::KEYCODE_ESCAPE:
		g_engine->_engine->key = 0x1B;
		break;
	case Common::KEYCODE_UP:
		g_engine->_engine->joyD |= 1;
		break;
	case Common::KEYCODE_DOWN:
		g_engine->_engine->joyD |= 2;
		break;
	case Common::KEYCODE_RIGHT:
		g_engine->_engine->joyD |= 8;
		break;
	case Common::KEYCODE_LEFT:
		g_engine->_engine->joyD |= 4;
		break;
	case Common::KEYCODE_SPACE:
		g_engine->_engine->click = 1;
		g_engine->_engine->character = 32;
		break;
	case Common::KEYCODE_BACKSPACE:
		g_engine->_engine->backspace = true;
		break;
	default:
		g_engine->_engine->character = event.kbd.ascii;
		break;
	}
}

static void handleKeyUp(const Common::Event &event) {
	switch (event.kbd.keycode) {
	case Common::KEYCODE_RETURN:
		g_engine->_engine->key &= ~0x1C;
		break;
	case Common::KEYCODE_ESCAPE:
		g_engine->_engine->key &= ~0x1B;
		break;
	case Common::KEYCODE_UP:
		g_engine->_engine->joyD &= ~1;
		break;
	case Common::KEYCODE_DOWN:
		g_engine->_engine->joyD &= ~2;
		break;
	case Common::KEYCODE_RIGHT:
		g_engine->_engine->joyD &= ~8;
		break;
	case Common::KEYCODE_LEFT:
		g_engine->_engine->joyD &= ~4;
		break;
	case Common::KEYCODE_SPACE:
		g_engine->_engine->click &= ~1;
		g_engine->_engine->character = 0;
		break;
	case Common::KEYCODE_d:
		g_engine->_engine->debugFlag = !g_engine->_engine->debugFlag;
		g_engine->_engine->character = 0;
		break;
	case Common::KEYCODE_BACKSPACE:
		g_engine->_engine->backspace = false;
		break;
	default:
		g_engine->_engine->character = 0;
		break;
	}
}

void readKeyboard() {
	Common::Event event;
	while (!::Engine::shouldQuit() && g_system->getEventManager()->pollEvent(event)) {

		switch (event.type) {
		case Common::EventType::EVENT_KEYDOWN:
			handleKeyDown(event);
			break;
		case Common::EventType::EVENT_KEYUP:
			handleKeyUp(event);
			break;
		default:
			break;
		}
	}
}
} // namespace Fitd
