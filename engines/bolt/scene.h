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
#include "bolt/boltlib/boltlib.h"
#include "bolt/boltlib/palette.h"

namespace Bolt {

class BoltEngine;

class Scene {
public:
	void load(Graphics *graphics, Boltlib &boltlib, BltId sceneId);
	void enter();
	void handleHover(const Common::Point &pt);
	// Return number of button at a point, or -1 if there is no button.
	int getButtonAtPoint(const Common::Point &pt);

	void setBackPlane(Boltlib &bltFile, BltId id);


private:
	Graphics *_graphics;

	struct BltPlaneStruct { // type 26
		static const uint32 kType = kBltPlane;
		static const uint kSize = 0x10;
		void load(const byte *src, Boltlib &bltFile) {
			BltId imageId(READ_BE_UINT32(&src[0]));
			image.load(bltFile, imageId);
			BltId paletteId(READ_BE_UINT32(&src[4]));
			palette.load(bltFile, paletteId);
			BltId hotspotsId(READ_BE_UINT32(&src[8]));
			hotspots.load(bltFile, hotspotsId);
		}

		BltImage image;
		BltPalette palette;
		BltImage hotspots;
	};

	typedef BltLoader<BltPlaneStruct> BltPlane;

	BltPlane _forePlane;
	BltPlane _backPlane;

	struct BltSpriteStruct { // type 27
		static const uint32 kType = kBltSpriteList;
		static const uint kSize = 0x8;
		void load(const byte *src, Boltlib &bltFile) {
			pos.x = READ_BE_INT16(&src[0]);
			pos.y = READ_BE_INT16(&src[2]);
			BltId imageId(READ_BE_UINT32(&src[4]));
			image.load(bltFile, imageId);
		}

		Common::Point pos;
		BltImage image;
	};

	typedef BltArrayLoader<BltSpriteStruct> BltSpriteList;

	BltSpriteList _sprites;

	Common::ScopedPtr<BltColorCycles> _colorCycles;

	struct BltButtonGraphicsStruct { // type 30
		static const uint32 kType = kBltButtonGraphicsList;
		static const uint kSize = 0xC;
		enum GraphicsType {
			PaletteMods = 1,
			Sprites = 2,
		};

		void load(const byte *src, Boltlib &boltlib) {
			type = READ_BE_UINT16(&src[0]);
			// FIXME: unknown field at 2. It is used in the buttons on sliding
			// and points to an image.
			BltId hoveredId(READ_BE_UINT32(&src[6]));
			BltId idleId(READ_BE_UINT32(&src[0xA]));
			if (type == PaletteMods) {
				hoveredPaletteMod.load(boltlib, hoveredId);
				idlePaletteMod.load(boltlib, idleId);
			}
			else if (type == Sprites) {
				hoveredSprites.load(boltlib, hoveredId);
				idleSprites.load(boltlib, idleId);
			}
		}

		uint16 type;

		// For palette mod type graphics
		BltPaletteMods hoveredPaletteMod;
		BltPaletteMods idlePaletteMod;

		// For image type graphics
		BltSpriteList hoveredSprites;
		BltSpriteList idleSprites;
	};

	typedef BltArrayLoader<BltButtonGraphicsStruct> BltButtonGraphicsList;

	struct BltButtonStruct { // type 31
		static const uint32 kType = kBltButtonList;
		static const uint kSize = 0x14;
		void load(const byte *src, Boltlib &bltFile) {
			type = READ_BE_UINT16(&src[0]);
			rect = Rect(&src[2]);
			plane = READ_BE_UINT16(&src[0xA]);
			numGraphics = READ_BE_UINT16(&src[0xC]);
			// FIXME: unknown field at 0xE. Always 0 in game data.
			graphics.load(bltFile, BltId(READ_BE_UINT32(&src[0x10])));
		}

		enum HotspotType {
			Rectangle = 1,
			// 2 is regular display query (unused)
			HotspotQuery = 3,
		};

		uint16 type;
		Rect rect;
		uint16 plane;
		uint16 numGraphics;
		BltButtonGraphicsList graphics;
	};

	typedef BltArrayLoader<BltButtonStruct> BltButtonList;

	BltButtonList _buttons;

	Common::Point _origin;

	void drawButton(const BltButtonStruct &button, bool hovered);
};

} // End of namespace Bolt

#endif
