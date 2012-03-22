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

#include "common/scummsys.h"

#if defined(_XBOX)

#include "backends/events/xboxsdl/xboxsdl-events.h"
#include "backends/platform/sdl/sdl.h"
#include "backends/graphics/graphics.h"
#include "common/config-manager.h"
#include "engines/engine.h"

#include "common/util.h"
#include "common/events.h"

#define JOY_DEADZONE 7200
#define JOY_ANALOG
#define JOY_XAXIS1 0
#define JOY_YAXIS1 1
#define JOY_XAXIS2 2
#define JOY_YAXIS2 3

#define JOY_BUT_A 0
#define JOY_BUT_B 1
#define JOY_BUT_X 2
#define JOY_BUT_Y 3
#define JOY_BUT_BLACK 4
#define JOY_BUT_WHITE 5
#define JOY_BUT_LTRIGGER 6
#define JOY_BUT_RTRIGGER 7
#define JOY_BUT_START 8
#define JOY_BUT_BACK 9
#define JOY_BUT_LSTICK 10
#define JOY_BUT_RSTICK 11

int _mouseSpeed = 2000;

enum
{
     BTN_UP = 0,
     BTN_DOWN = 1,
     BTN_LEFT = 2,
     BTN_RIGHT = 3,
     BTN_UP_LEFT = 4,
     BTN_UP_RIGHT = 5,
     BTN_DOWN_LEFT = 6,
     BTN_DOWN_RIGHT = 7
}; 

bool XboxSdlEventSource::handleJoyButtonDown(SDL_Event &ev, Common::Event &event) {
	Common::String gameid(ConfMan.get("gameid"));
	if (ev.jbutton.button == JOY_BUT_A) {
		event.type = Common::EVENT_LBUTTONDOWN;
		processMouseEvent(event, _km.x, _km.y);
	} else if (ev.jbutton.button == JOY_BUT_B) {
		event.type = Common::EVENT_RBUTTONDOWN;
		processMouseEvent(event, _km.x, _km.y);
	} else {
		event.type = Common::EVENT_KEYDOWN;
		switch (ev.jbutton.button) {
		case JOY_BUT_X:
			event.kbd.keycode = Common::KEYCODE_ESCAPE;
			event.kbd.ascii = mapKey(SDLK_ESCAPE, (SDLMod)ev.key.keysym.mod, 0);
			break;
		case JOY_BUT_WHITE:
			event.kbd.keycode = Common::KEYCODE_F5;
			event.kbd.ascii = mapKey(SDLK_F5, (SDLMod)ev.key.keysym.mod, 0);
			break;
		case JOY_BUT_START:
			event.kbd.keycode = Common::KEYCODE_F5;
			event.kbd.flags |= Common::KBD_CTRL;
			event.kbd.ascii = mapKey(SDLK_F5, ev.key.keysym.mod, 0);
			break;
		case JOY_BUT_BACK:
			event.kbd.keycode = Common::KEYCODE_r;
			event.kbd.ascii = mapKey(SDLK_r, ev.key.keysym.mod, 0);
			break;
		case JOY_BUT_Y:
			if ((gameid == "lol") || (gameid == "lol-cd")) {
				PushKeyEvent(SDL_PRESSED, SDLK_F1);
				PushKeyEvent(SDL_PRESSED, SDLK_F2);
				PushKeyEvent(SDL_PRESSED, SDLK_F3);
			} else {
				PushKeyEvent(SDL_PRESSED, SDLK_RETURN);
			}
			break;
		case JOY_BUT_BLACK:
			event.kbd.keycode = Common::KEYCODE_F7;
			event.kbd.ascii = mapKey(SDLK_F7, ev.key.keysym.mod, 0);
			break;
		case JOY_BUT_LTRIGGER:
			if ((gameid == "lol") || (gameid == "lol-cd")) {
				event.kbd.keycode = Common::KEYCODE_KP7;
				event.kbd.ascii = mapKey(SDLK_KP7, ev.key.keysym.mod, 0);
			} else {
				_mouseSpeed = 750;
			}
			break;
		case JOY_BUT_RTRIGGER:
			if ((gameid == "lol") || (gameid == "lol-cd")) {
				event.kbd.keycode = Common::KEYCODE_KP9;
				event.kbd.ascii = mapKey(SDLK_KP9, ev.key.keysym.mod, 0);
			} else {
				_mouseSpeed = 5000;
			}
			break;
		case JOY_BUT_LSTICK:
			break;
		case JOY_BUT_RSTICK:
			event.kbd.keycode = Common::KEYCODE_KP5;
			event.kbd.ascii = mapKey(SDLK_KP5, ev.key.keysym.mod, 0);
			break;
		}
	}
	return true;
}

bool XboxSdlEventSource::handleJoyButtonUp(SDL_Event &ev, Common::Event &event) {
	Common::String gameid(ConfMan.get("gameid"));
	if (ev.jbutton.button == JOY_BUT_A) {
		event.type = Common::EVENT_LBUTTONUP;
		processMouseEvent(event, _km.x, _km.y);
	} else if (ev.jbutton.button == JOY_BUT_B) {
		event.type = Common::EVENT_RBUTTONUP;
		processMouseEvent(event, _km.x, _km.y);
	} else {
		event.type = Common::EVENT_KEYUP;
		switch (ev.jbutton.button) {
		case JOY_BUT_X:
			event.kbd.keycode = Common::KEYCODE_ESCAPE;
			event.kbd.ascii = mapKey(SDLK_ESCAPE, (SDLMod)ev.key.keysym.mod, 0);
			break;
		case JOY_BUT_WHITE:
			event.kbd.keycode = Common::KEYCODE_F5;
			event.kbd.ascii = mapKey(SDLK_F5, (SDLMod)ev.key.keysym.mod, 0);
			break;
		case JOY_BUT_START:
			event.kbd.keycode = Common::KEYCODE_F5;
			event.kbd.flags |= Common::KBD_CTRL;
			event.kbd.ascii = mapKey(SDLK_F5, ev.key.keysym.mod, 0);
			break;
		case JOY_BUT_BACK:
			event.kbd.keycode = Common::KEYCODE_r;
			event.kbd.ascii = mapKey(SDLK_r, ev.key.keysym.mod, 0);
			break;
		case JOY_BUT_Y:
			if ((gameid == "lol") || (gameid == "lol-cd")) {
				PushKeyEvent(SDL_RELEASED, SDLK_F1);
				PushKeyEvent(SDL_RELEASED, SDLK_F2);
				PushKeyEvent(SDL_RELEASED, SDLK_F3);
			} else {
				PushKeyEvent(SDL_RELEASED, SDLK_RETURN);
			}
			break;
		case JOY_BUT_BLACK:
			event.kbd.keycode = Common::KEYCODE_F7;
			event.kbd.ascii = mapKey(SDLK_F7, ev.key.keysym.mod, 0);
			break;
		case JOY_BUT_LTRIGGER:
			if ((gameid == "lol") || (gameid == "lol-cd")) {
				event.kbd.keycode = Common::KEYCODE_KP7;
				event.kbd.ascii = mapKey(SDLK_KP7, ev.key.keysym.mod, 0);
			} else {
				_mouseSpeed = 2000;
			}
			break;
		case JOY_BUT_RTRIGGER:
			if ((gameid == "lol") || (gameid == "lol-cd")) {
				event.kbd.keycode = Common::KEYCODE_KP9;
				event.kbd.ascii = mapKey(SDLK_KP9, ev.key.keysym.mod, 0);
			} else {
				_mouseSpeed = 2000;
			}
			break;
		case JOY_BUT_LSTICK:
			break;
		case JOY_BUT_RSTICK:
			event.kbd.keycode = Common::KEYCODE_KP5;
			event.kbd.ascii = mapKey(SDLK_KP5, ev.key.keysym.mod, 0);
			break;
		}
	}
	return true;
}

void XboxSdlEventSource::PushKeyEvent(int state, SDLKey key) {
	SDL_Event event;
	event.type = (state)?SDL_KEYDOWN:SDL_KEYUP;
	event.key.state = (state)?SDL_PRESSED:SDL_RELEASED;
	event.key.keysym.sym = key;
	SDL_PushEvent(&event);      
} 

bool XboxSdlEventSource::handleJoyHat(SDL_Event &ev, Common::Event &event) {
	if (ev.jhat.value == SDL_HAT_UP) {
		PushKeyEvent(SDL_PRESSED, SDLK_UP);
	} else {
		PushKeyEvent(SDL_RELEASED, SDLK_UP);
	}
	if (ev.jhat.value == SDL_HAT_DOWN) {
		PushKeyEvent(SDL_PRESSED, SDLK_DOWN);
	} else {
		PushKeyEvent(SDL_RELEASED, SDLK_DOWN);
	}
	if (ev.jhat.value == SDL_HAT_LEFT) {
		PushKeyEvent(SDL_PRESSED, SDLK_LEFT);
	} else {
		PushKeyEvent(SDL_RELEASED, SDLK_LEFT);
	}
	if (ev.jhat.value == SDL_HAT_RIGHT) {
		PushKeyEvent(SDL_PRESSED, SDLK_RIGHT);
	} else {
		PushKeyEvent(SDL_RELEASED, SDLK_RIGHT);
	}
	return false;
}

bool XboxSdlEventSource::handleJoyAxisMotion(SDL_Event &ev, Common::Event &event) {
	int axis = ev.jaxis.value;
	int buttonsNow = 0;
	static int buttonsPrev = 0;
	int joy_x = 0;
	int joy_y = 0;

	if ( axis > JOY_DEADZONE) {
		axis -= JOY_DEADZONE;
		event.type = Common::EVENT_MOUSEMOVE;
	} else if ( axis < -JOY_DEADZONE ) {
		axis += JOY_DEADZONE;
		event.type = Common::EVENT_MOUSEMOVE;
	} else
		axis = 0;

	if ( ev.jaxis.axis == JOY_XAXIS1) {
		_km.x_vel = axis / _mouseSpeed;
		_km.x_down_count = 0;

	} else if (ev.jaxis.axis == JOY_YAXIS1) {
#ifndef JOY_INVERT_Y
		axis = -axis;
#endif
		_km.y_vel = -axis / _mouseSpeed;
		_km.y_down_count = 0;
	} else {
		joy_x = SDL_JoystickGetAxis(_joystick, JOY_XAXIS2);
		joy_y = SDL_JoystickGetAxis(_joystick, JOY_YAXIS2);

		if (joy_x < -30000) buttonsNow |= (1 << BTN_LEFT);
		if (joy_x >  30000) buttonsNow |= (1 << BTN_RIGHT);
		if (joy_y < -30000) buttonsNow |= (1 << BTN_UP);
		if (joy_y >  30000) buttonsNow |= (1 << BTN_DOWN); 

		if (joy_x > 12000 && joy_y < -12000)
			buttonsNow = (1 << BTN_UP_RIGHT);
		if (joy_x > 12000 && joy_y > 12000)
			buttonsNow = (1 << BTN_DOWN_RIGHT);
		if (joy_x < -12000 && joy_y > 12000)
			buttonsNow = (1 << BTN_DOWN_LEFT);
		if (joy_x < -12000 && joy_y < -12000)
			buttonsNow = (1 << BTN_UP_LEFT); 

		if (buttonsNow != buttonsPrev)
		{
			if ((buttonsNow & (1 << BTN_LEFT)) != (buttonsPrev & (1 << BTN_LEFT)))
			{
				PushKeyEvent((buttonsNow & (1 << BTN_LEFT)), SDLK_KP4);
			}
			if ((buttonsNow & (1 << BTN_RIGHT)) != (buttonsPrev & (1 << BTN_RIGHT)))
			{
				PushKeyEvent((buttonsNow & (1 << BTN_RIGHT)), SDLK_KP6);
			}
			if ((buttonsNow & (1 << BTN_UP)) != (buttonsPrev & (1 << BTN_UP)))
			{
				PushKeyEvent((buttonsNow & (1 << BTN_UP)), SDLK_KP8);
			}
			if ((buttonsNow & (1 << BTN_DOWN)) != (buttonsPrev & (1 << BTN_DOWN)))
			{
				PushKeyEvent((buttonsNow & (1 << BTN_DOWN)), SDLK_KP2);
			} 
		
			if ((buttonsNow & (1 << BTN_UP_RIGHT)) != (buttonsPrev & (1<< BTN_UP_RIGHT)))
			{
				PushKeyEvent((buttonsNow & (1 << BTN_UP_RIGHT)), SDLK_KP9);
			}
			if ((buttonsNow & (1 << BTN_DOWN_RIGHT)) != (buttonsPrev & (1<< BTN_DOWN_RIGHT)))
			{
				PushKeyEvent((buttonsNow & (1 << BTN_DOWN_RIGHT)), SDLK_KP3);
			}
			if ((buttonsNow & (1 << BTN_DOWN_LEFT)) != (buttonsPrev & (1<< BTN_DOWN_LEFT)))
			{
				PushKeyEvent((buttonsNow & (1 << BTN_DOWN_LEFT)), SDLK_KP1);
			}
			if ((buttonsNow & (1 << BTN_UP_LEFT)) != (buttonsPrev & (1<< BTN_UP_LEFT)))
			{
				PushKeyEvent((buttonsNow & (1 << BTN_UP_LEFT)), SDLK_KP7);
			}
		} 
		buttonsPrev = buttonsNow;
	}

	processMouseEvent(event, _km.x, _km.y);

	return true;
}

#endif
