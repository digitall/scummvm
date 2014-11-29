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

#ifndef BOLT_MENU_STATE_H
#define BOLT_MENU_STATE_H

#include "bolt/bolt.h"

#include "common/array.h"
#include "common/rect.h"

namespace Common {
struct Event;
};

namespace Bolt {

class BoltEngine;

typedef Common::SharedPtr<class MenuState> MenuStatePtr;

class MenuState : public State {
public:
	static MenuStatePtr create(BoltEngine *engine, BltLongId menuId,
		StatePtr afterState);

	virtual void process(const Common::Event &event);

private:
	MenuState();

	BoltEngine *_engine;

	StatePtr _afterState;

	BltResourcePtr _menuBgInfo;
	BltResourcePtr _menuBgImageAndPalette;
	BltResourcePtr _menuBgImage;
	BltResourcePtr _menuBgPalette;
	BltResourcePtr _menuButtonInfo;

	struct MenuButton {
		enum HoverAction {
			kNone,
			kImage, // param 2: image
			kPaletteMod, // param 2: palette mod
			kDualImage, // param 2: active placed image, param 3: inactive placed image
		};
		// TODO: hitbox info
		HoverAction hoverAction;

		Rect rect;

		uint activePalStart;
		uint activePalNum;
		BltResourcePtr activePalColors;

		uint inactivePalStart;
		uint inactivePalNum;
		BltResourcePtr inactivePalColors;

		int activeImageX;
		int activeImageY;
		BltResourcePtr activeImage;

		int inactiveImageX;
		int inactiveImageY;
		BltResourcePtr inactiveImage;
	};

	Common::Array<MenuButton> _menuButtons;

	void render();
	void renderMenuButton(const MenuButton &button, bool active);
};

} // End of namespace Bolt

#endif
