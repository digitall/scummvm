/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/font.h"
#include "fitd/gfx.h"
#include "fitd/jack.h"
#include "fitd/pak.h"
#include "fitd/sequence.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"

namespace Fitd {
// ITD_RESS mapping
#define JACK_CADRE_SPF 0
#define JACK_ITDFONT 1
#define JACK_LIVRE 2
#define JACK_IM_EXT_JACK 3

void jackStart(int saveSlot) {
	fontHeight = 16; // TODO: check
	startGame(16, 1, 1);
}

void jackReadBook(int index, int type) {
	switch (type) {
	case 1: // READ_BOOK
	{
		byte *pImage = (byte *)pakLoad("ITD_RESS.PAK", JACK_LIVRE);
		memcpy(g_engine->_engine->aux, pImage, 320 * 200);
		byte *lpalette = pImage + 320 * 200;
		convertPaletteIfRequired(lpalette);
		copyPalette(lpalette, currentGamePalette);
		gfx_setPalette(lpalette);
		free(pImage);
		g_engine->_engine->turnPageFlag = 1;
		lire(index, 60, 10, 245, 190, 0, 124, 124);
		break;
	}
	default:
		assert(0);
	}
}
} // namespace Fitd
