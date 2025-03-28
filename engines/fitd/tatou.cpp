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

#include "fitd/gfx.h"
#include "fitd/tatou.h"
#include "common/events.h"
#include "graphics/framelimiter.h"

namespace Fitd {

void fadeInPhys(int step, int start) {
	byte localPalette[0x300];
	Common::Event e;

	Graphics::FrameLimiter limiter(g_system, 25);
	for (int i = 0; i < 256; i += step) {
		while (g_system->getEventManager()->pollEvent(e)) {
		}

		// computePalette(currentGamePalette,localPalette,i);
		// gfx_setPalette(localPalette);
		gfx_refreshFrontTextureBuffer();
		gfx_draw();
		g_system->updateScreen();

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		limiter.delayBeforeSwap();
		limiter.startFrame();
	}
}

}
