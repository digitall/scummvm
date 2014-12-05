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

#include "bolt/image.h"

namespace Common {
struct Event;
};

namespace Bolt {

class BoltEngine;

typedef Common::SharedPtr<class MenuState> MenuStatePtr;

class MenuState : public State {
public:
	static MenuStatePtr create(BoltEngine *engine, BltLongId menuId);

	virtual void process(const Common::Event &event);

private:
	MenuState();

	void init(BoltEngine *engine, BltLongId menuId);

	BoltEngine *_engine;

	BltResourcePtr _menuBgInfo;
	BltResourcePtr _menuBgImageAndPalette;
	BltImagePtr _menuBgImage;
	BltImagePtr _hotspotsImage;
	BltResourcePtr _menuBgPalette;
	BltResourcePtr _menuButtonInfo;

	struct MenuButton {
		enum HotspotType {
			kHotspotNone = 0, // unused, for internal use
			kHotspotRect = 1,
			// Hotspot type 2 seems to be a normal display query (unused)
			kHotspotImageQuery = 3, // rect.left: low color, rect.right: high color
		};

		enum GraphicsType {
			kGfxNone = 0, // unused, for internal use
			kGfxPaletteMods = 1,
			kGfxImages = 2,
		};

		HotspotType hotspotType;
		GraphicsType gfxType;
		Rect rect;

		uint activePalStart;
		uint activePalNum;
		BltResourcePtr activePalColors;

		uint inactivePalStart;
		uint inactivePalNum;
		BltResourcePtr inactivePalColors;

		Common::Point hoveredImagePos;
		BltImagePtr hoveredImage;

		Common::Point idleImagePos;
		BltImagePtr idleImage;
	};

	Common::Array<MenuButton> _menuButtons;

	bool isButtonAtPoint(const MenuButton &button, const Common::Point &pt) const;
	void render();
	void renderMenuButton(const MenuButton &button, bool active);
};

} // End of namespace Bolt

#endif
