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

#include "bolt/merlin/main_menu.h"

#include "bolt/merlin/merlin.h"

namespace Bolt {
	
struct BltMainMenuStruct {
	static const uint32 kType = kBltMainMenu;
	static const uint32 kSize = 0xC;
	void load(const byte *src, BltFile &bltFile) {
		sceneId = BltId(READ_BE_UINT32(&src[0]));
		colorbarsImageId = BltId(READ_BE_UINT32(&src[4]));
		colorbarsPaletteId = BltId(READ_BE_UINT32(&src[8]));
	}

	BltId sceneId;
	BltId colorbarsImageId;
	BltId colorbarsPaletteId;
};

typedef BltLoader<BltMainMenuStruct> BltMainMenu;

void MainMenu::init(Graphics *graphics, BltFile &boltlib, BltId resId) {
	BltMainMenu mainMenu(boltlib, resId);
	MenuCard::init(graphics, boltlib, mainMenu->sceneId);
}

Card::Signal MainMenu::handleButtonClick(int num) {
	switch (num) {
	case -1: // No button
		return kNull;
	case 0: // Play
		return kEnd;
	case 1: // Credits
		return kPlayCredits;
	case 4: // Tour
		return kPlayTour;
	default:
		warning("unknown main menu button %d", num);
		return kNull;
	}
}

} // End of namespace Bolt
