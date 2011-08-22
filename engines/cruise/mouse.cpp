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

#include "cruise/cruise_main.h"
#include "cruise/staticres.h"

#include "common/system.h"
#include "graphics/cursorman.h"

namespace Cruise {

const MouseCursor Mouse::mouseCursors[] = {
	{ 1, 1, mouseCursorNormal },
	{ 0, 0, mouseCursorDisk },
	{ 7, 7, mouseCursorCross },
	{ 0, 0, mouseCursorNoMouse },
	{ 10, 6, mouseCursorWalk },
	{ 10, 6, mouseCursorExit },
	{ 10, 6, mouseCursorMagnifyingGlass }
};

const byte Mouse::cursorPalette[] = {
	0, 0, 0,
	0xff, 0xff, 0xff
};

Mouse::Mouse() {
	_coordinateX = 0;
	_coordinateY = 0;
	_button = 0;
	_cursor = CURSOR_NOMOUSE;
}

void Mouse::changeCursor(CursorType eType) {
	assert(eType >= 0 && eType < CURSOR_MAX);
	if (_cursor != eType) {
		byte mouseCursor[16 * 16];
		const MouseCursor *mc = &mouseCursors[eType];
		const byte *src = mc->bitmap;
		for (int i = 0; i < 32; ++i) {
			int offs = i * 8;
			for (byte mask = 0x80; mask != 0; mask >>= 1) {
				if (src[0] & mask) {
					mouseCursor[offs] = 1;
				} else if (src[32] & mask) {
					mouseCursor[offs] = 0;
				} else {
					mouseCursor[offs] = 0xFF;
				}
				++offs;
			}
			++src;
		}
		CursorMan.replaceCursor(mouseCursor, 16, 16, mc->hotspotX, mc->hotspotY, 0xFF);
		CursorMan.replaceCursorPalette(cursorPalette, 0, 2);
		_cursor = eType;
	}
}

void mouseOff() {
	CursorMan.showMouse(false);
	g_system->updateScreen();
}

void mouseOn() {
	CursorMan.showMouse(true);
	g_system->updateScreen();
}

void Mouse::getStatus(int16 *pMouseX, int16 *pMouseButton, int16 *pMouseY) {
	*pMouseX = _coordinateX;
	*pMouseY = _coordinateY;
	*pMouseButton = _button;
}

void Mouse::manageEvent(Common::Event event) {
	switch (event.type) {
		case Common::EVENT_LBUTTONDOWN:
			_button |= CRS_MB_LEFT;
			break;
		case Common::EVENT_LBUTTONUP:
			_button &= ~CRS_MB_LEFT;
			break;
		case Common::EVENT_RBUTTONDOWN:
			_button |= CRS_MB_RIGHT;
			break;
		case Common::EVENT_RBUTTONUP:
			_button &= ~CRS_MB_RIGHT;
			break;
		case Common::EVENT_MOUSEMOVE:
			_coordinateX = event.mouse.x;
			_coordinateY = event.mouse.y;
			break;

		case Common::EVENT_KEYUP:
			switch (event.kbd.keycode) {
				case Common::KEYCODE_ESCAPE:
					_button &= ~CRS_MB_MIDDLE;
					break;
				default:
					break;
				}
			break;
		case Common::EVENT_KEYDOWN:
			if (event.kbd.keycode == Common::KEYCODE_ESCAPE) {
				_button |= CRS_MB_MIDDLE;
			}
		default:
			break;
	}
}

} // End of namespace Cruise
