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

#include "common/system.h"
#include "fitd/aitd1.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/file_access.h"
#include "fitd/fitd.h"
#include "fitd/gfx.h"
#include "fitd/input.h"
#include "fitd/tatou.h"

namespace Fitd {

static int shakeTime = 4;

static void clearScreenTatou() {
	for (int i = 0; i < 45120; i++) {
		g_engine->_engine->frontBuffer[i] = 0;
	}
}

void process_events() {
	const uint32 timeIncrease = 1;

	flushPendingPrimitives();

	if (g_engine->_engine->shakeVar1) {
		shakeTime = (shakeTime + 1) % 6;
		if (shakeTime == 0) {
			g_system->setShakePos(0, 2);
		} else {
			g_system->setShakePos(0, -2);
		}
	}

	updateScreen();
	startFrame();

	readKeyboard();
	g_engine->_engine->timeGlobal += timeIncrease;
	g_engine->_engine->timer = g_engine->_engine->timeGlobal;
}

int make3dTatou() {
	ScopedPtr tatou2d(checkLoadMallocPak("ITD_RESS.PAK", AITD1_TATOU_MCG));
	ScopedPtr tatou3d(checkLoadMallocPak("ITD_RESS.PAK", AITD1_TATOU_3DO));
	ScopedPtr tatouPal(checkLoadMallocPak("ITD_RESS.PAK", AITD1_TATOU_PAL));

	byte paletteBackup[768];

	setupCameraProjection(160, 100, 128, 500, 490);
	copyPalette(g_engine->_engine->currentGamePalette, paletteBackup);
	paletteFill(g_engine->_engine->currentGamePalette, 0, 0, 0);
	gfx_setPalette(g_engine->_engine->currentGamePalette);

	copyPalette(tatouPal.get(), g_engine->_engine->currentGamePalette);
	fastCopyScreen(tatou2d.get() + 770, g_engine->_engine->frontBuffer);
	fastCopyScreen(g_engine->_engine->frontBuffer, g_engine->_engine->aux2);
	gfx_copyBlockPhys(g_engine->_engine->frontBuffer, 0, 0, 320, 200);

	fadeInPhys(8, 0);

	uint localChrono;
	startChrono(&localChrono);

	int time = 8920;
	int rotation = 256;
	do {
		process_events();

		g_engine->_engine->timer = g_engine->_engine->timeGlobal;

		if (evalChrono(&localChrono) <= 180) // avant eclair
		{
			if (g_engine->_engine->key || g_engine->_engine->click) {
				break;
			}
		} else // eclair
		{
			const int alpha = 8;
			g_engine->_engine->lastSample = -1;
			g_engine->_engine->lastPriority = -1;

			playSound(g_engine->_engine->cVars[getCVarsIdx(SAMPLE_TONNERRE)]);

			g_engine->_engine->lastSample = -1;
			g_engine->_engine->lastPriority = -1;

			paletteFill(g_engine->_engine->currentGamePalette, 63, 63, 63);
			gfx_setPalette(g_engine->_engine->currentGamePalette);
			clearScreenTatou();
			setCameraTarget(0, 0, 0, alpha, rotation, 0, time);
			gfx_copyBlockPhys(g_engine->_engine->frontBuffer, 0, 0, 320, 200);

			process_events();

			copyPalette(tatouPal.get(), g_engine->_engine->currentGamePalette);
			gfx_setPalette(g_engine->_engine->currentGamePalette);
			gfx_copyBlockPhys(g_engine->_engine->frontBuffer, 0, 0, 320, 200);
			affObjet(0, 0, 0, 0, 0, 0, tatou3d.get());

			while (!::Engine::shouldQuit() && g_engine->_engine->key == 0 && g_engine->_engine->click == 0 && g_engine->_engine->joyD == 0) // boucle de rotation du tatou
			{
				const int deltaTime = 50;

				process_events();
				time += deltaTime - 25;

				if (time > 16000)
					break;

				rotation -= 8;
				clearScreenTatou();
				setCameraTarget(0, 0, 0, alpha, rotation, 0, time);
				affObjet(0, 0, 0, 0, 0, 0, tatou3d.get());
			}
			break;
		}
	} while (!::Engine::shouldQuit());

	if (g_engine->_engine->key || g_engine->_engine->click || g_engine->_engine->joyD) {
		while (g_engine->_engine->key) {
			process_events();
		}

		fadeOutPhys(32, 0);
		copyPalette(paletteBackup, g_engine->_engine->currentGamePalette);
		return 1;
	}

	fadeOutPhys(16, 0);
	copyPalette(paletteBackup, g_engine->_engine->currentGamePalette);
	return 0;
}

} // namespace Fitd
