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

#include "bolt/boltlib/palette.h"

#include "bolt/graphics.h"

namespace Bolt {

void applyColorCycles(Graphics *graphics, const BltColorCycles *cycles) {
	graphics->resetColorCycles();
	if (cycles) {
		for (int i = 0; i < 4; ++i) {
			BltColorCycleSlot *slot = (*cycles)->slots[i].get();
			if ((*cycles)->numSlots[i] == 1 && slot) {
				if ((*slot)->frames <= 0) {
					warning("Invalid color cycle frames");
				}
				else {
					if ((*slot)->plane != 0) {
						warning("Color cycle plane was not 0");
					}
					graphics->setColorCycle(i, (*slot)->start, (*slot)->end,
						(*slot)->frames * 1000 / 60);
				}
			}
		}
	}
}

struct BltPaletteHeader {
	static const uint32 kSize = 6;
	BltPaletteHeader(const byte *src) {
		target = READ_BE_UINT16(&src[0]);
		bottom = READ_BE_UINT16(&src[2]);
		top = READ_BE_UINT16(&src[4]);
	}

	uint16 target; // ??? (usually 2)
	uint16 bottom; // ??? (usually 0)
	uint16 top; // ??? (usually 127)
};

void BltPalette::load(Boltlib &boltlib, BltId id) {
	data.reset(boltlib.loadResource(id, kBltPalette));
}

void applyPalette(Graphics *graphics, const BltPalette &palette, PaletteTarget target) {
	if (palette.data) {
		BltPaletteHeader header(&palette.data[0]);
		if (header.target == 0) {
			debug(3, "setting palette with target type 0 to both planes");
			// Both fore and back planes
			if (header.bottom != 0) {
				warning("palette target 0 bottom color is not 0");
			}
			if (header.top != 255) {
				warning("palette target 0 top color is not 255");
			}

			graphics->getBackPlane().setPalette(&palette.data[BltPaletteHeader::kSize], 0, 128);
			graphics->getForePlane().setPalette(&palette.data[BltPaletteHeader::kSize + 128 * 3], 0, 128);
		}
		else if (header.target == 2) {
			// Auto? Back or fore not specified in palette resource.
			Plane *plane = nullptr;
			if (target == kBack) {
				debug(3, "setting palette with target type 2 to back");
				plane = &graphics->getBackPlane();
			}
			else if (target == kFore) {
				debug(3, "setting palette with target type 2 to fore");
				plane = &graphics->getForePlane();
			}
			else {
				assert(false); // Unreachable, target must be valid
			}

			uint num = header.top - header.bottom + 1;
			plane->setPalette(&palette.data[BltPaletteHeader::kSize],
				header.bottom, num);
		}
		else {
			warning("Unknown palette target %d", (int)header.target);
		}
	}
}

void applyPaletteMod(Graphics *graphics, const BltPaletteMods &mod, int num, PaletteTarget target) {
	Plane *plane = nullptr;
	if (target == kBack) {
		plane = &graphics->getBackPlane();
	}
	else if (target == kFore) {
		plane = &graphics->getForePlane();
	}
	else {
		assert(false); // Unreachable, target must be valid
	}

	plane->setPalette(&mod[num].colors[0], mod[num].start, mod[num].num);
}

} // End of namespace Bolt
