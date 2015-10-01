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
	
struct BltMainMenuInfo {
	BltMainMenuInfo(const byte *src) {
		menuInfoId = BltLongId(READ_BE_UINT32(&src[0]));
		hotspotImageId = BltLongId(READ_BE_UINT32(&src[4]));
		hotspotPaletteId = BltLongId(READ_BE_UINT32(&src[8]));
	}

	BltLongId menuInfoId;
	BltLongId hotspotImageId; // FIXME: correct?
	BltLongId hotspotPaletteId; // FIXME: correct?
};

MainMenuCard::MainMenuCard()
	: _merlin(nullptr)
{ }

void MainMenuCard::init(MerlinEngine *merlin, BltFile &bltFile, BltLongId id) {
	_merlin = merlin;

	BltResource mainMenuRes(bltFile.loadResource(id, kBltMainMenu));
	BltMainMenuInfo info(&mainMenuRes[0]);
	MenuCard::init(merlin->_engine, bltFile, info.menuInfoId);
}

Card::Status MainMenuCard::processButtonClick(int num) {
	switch (num) {
	case -1: // No button
		return None;
	case 0: // Play
		return Ended;
	case 1: // Credits
		_merlin->startMovie(_merlin->_maPf, MKTAG('C', 'R', 'D', 'T'));
		return None;
	case 4: // Tour
		_merlin->startMovie(_merlin->_maPf, MKTAG('T', 'O', 'U', 'R'));
		return None;
	default:
		warning("unknown main menu button %d", num);
		return None;
	}
}

} // End of namespace Bolt
