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
#include "fitd/file_access.h"
#include "fitd/fitd.h"
#include "fitd/gfx.h"
#include "fitd/input.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"

namespace Fitd {

static int shakeTime = 4;

static void clearScreenTatou() {
	for (int i = 0; i < 45120; i++) {
		frontBuffer[i] = 0;
	}
}

void process_events() {
	// TODO: fix this
	const uint32 timeIncrease = 2;

	osystem_flushPendingPrimitives();

	if (shakeVar1) {
		shakeTime = (shakeTime + 1) % 6;
		if (shakeTime == 0) {
			g_system->setShakePos(0, 2);
		} else {
			g_system->setShakePos(0, -2);
		}
	}

	osystem_updateScreen();
	osystem_startFrame();

	readKeyboard();
	timeGlobal += timeIncrease;
	timer = timeGlobal;
}

int make3dTatou() {
	byte paletteBackup[768];
	uint localChrono;

	char *tatou2d = checkLoadMallocPak("ITD_RESS.PAK", AITD1_TATOU_MCG);
	char *tatou3d = checkLoadMallocPak("ITD_RESS.PAK", AITD1_TATOU_3DO);
	byte *tatouPal = (byte *)checkLoadMallocPak("ITD_RESS.PAK", AITD1_TATOU_PAL);

	int time = 8920;
	int rotation = 256;

	setupCameraProjection(160, 100, 128, 500, 490);

	copyPalette(currentGamePalette, paletteBackup);

	paletteFill(currentGamePalette, 0, 0, 0);

	gfx_setPalette(currentGamePalette);

	copyPalette(tatouPal, currentGamePalette);
	fastCopyScreen(tatou2d + 770, frontBuffer);
	fastCopyScreen(frontBuffer, aux2);

	gfx_copyBlockPhys(frontBuffer, 0, 0, 320, 200);

	fadeInPhys(8, 0);

	startChrono(&localChrono);

	do {
		process_events();

		// timeGlobal++;
		timer = timeGlobal;

		if (evalChrono(&localChrono) <= 180) // avant eclair
		{
			if (key || Click) {
				break;
			}
		} else // eclair
		{
			const int unk1 = 8;
			/*  LastSample = -1;
			LastPriority = -1; */

			playSound(CVars[getCVarsIdx(SAMPLE_TONNERRE)]);

			/*     LastSample = -1;
			LastPriority = -1;*/

			paletteFill(currentGamePalette, 63, 63, 63);
			gfx_setPalette(currentGamePalette);
			/*  setClipSize(0,0,319,199);*/

			clearScreenTatou();

			setCameraTarget(0, 0, 0, unk1, rotation, 0, time);


			// blitScreenTatou();
			gfx_copyBlockPhys(frontBuffer, 0, 0, 320, 200);

			process_events();

			copyPalette(tatouPal, currentGamePalette);
			gfx_setPalette(currentGamePalette);
			gfx_copyBlockPhys(frontBuffer, 0, 0, 320, 200);

			affObjet(0, 0, 0, 0, 0, 0, tatou3d);

			while (!::Engine::shouldQuit() && key == 0 && Click == 0 && JoyD == 0) // boucle de rotation du tatou
			{
				const int deltaTime = 50;
				process_events();

				time += deltaTime - 25;

				if (time > 16000)
					break;

				rotation -= 8;

				clearScreenTatou();

				setCameraTarget(0, 0, 0, unk1, rotation, 0, time);

				affObjet(0, 0, 0, 0, 0, 0, tatou3d);

				// blitScreenTatou();

				// osystem_stopFrame();
			}

			break;
		}
	} while (!::Engine::shouldQuit());

	free(tatouPal);
	free(tatou3d);
	free(tatou2d);

	if (key || Click || JoyD) {
		while (key) {
			process_events();
		}

		fadeOutPhys(32, 0);
		copyPalette(paletteBackup, currentGamePalette);
		return 1;
	}

	fadeOutPhys(16, 0);
	copyPalette(paletteBackup, currentGamePalette);
	return 0;
}

} // namespace Fitd
