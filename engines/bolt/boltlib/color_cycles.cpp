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

#include "bolt/boltlib/color_cycles.h"

#include "bolt/graphics.h"

namespace Bolt {

void applyColorCycles(Graphics *graphics, BltColorCycles *cycles) {
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

} // End of namespace Bolt
