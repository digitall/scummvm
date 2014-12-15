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

#ifndef BOLT_SCENE_H
#define BOLT_SCENE_H

#include "common/array.h"
#include "common/rect.h"

#include "bolt/bolt.h"
#include "bolt/blt_file.h"
#include "bolt/image.h"

namespace Bolt {

class BoltEngine;

class Scene {
public:
	void init(BoltEngine *engine, BltFile *bltFile, BltLongId sceneId);
	void draw();

private:
	BoltEngine *_engine;

	struct Plane {
		BltImagePtr image;
		BltResourcePtr palette;
		BltImagePtr hotspots;
	};

	Plane _forePlane;
	Plane _backPlane;

	void loadPlane(Plane &plane, BltFile *bltFile, BltLongId planeId);
	Plane& getScenePlane(uint16 num);
	const Plane& getScenePlane(uint16 num) const;
	Bolt::Plane& getGraphicsPlane(uint16 num);

	struct Button {
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
		Rect rect;
		uint16 plane;

		GraphicsType graphicsType;

		Common::Point defaultImagePos;
		BltImagePtr defaultImage;

		// For Palette Mods graphics
		uint hoveredPalStart;
		uint hoveredPalNum;
		BltResourcePtr hoveredColors;

		// For Images graphics
		Common::Point hoveredImagePos;
		BltImagePtr hoveredImage;

		// For Palette Mods graphics
		uint idlePalStart;
		uint idlePalNum;
		BltResourcePtr idleColors;

		// For Images graphics
		Common::Point idleImagePos;
		BltImagePtr idleImage;
	};

	Common::Array<Button> _buttons;

	void loadButton(Button &button, BltFile *bltFile, const byte *src);

	Common::Point _origin;

	bool isButtonAtPoint(const Button &button, const Common::Point &pt) const;
	void drawButton(const Button &button, bool hovered);
};

} // End of namespace Bolt

#endif
