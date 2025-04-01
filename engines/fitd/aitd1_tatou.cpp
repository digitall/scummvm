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

#include "fitd/aitd1.h"
#include "fitd/common.h"
#include "fitd/file_access.h"
#include "fitd/fitd.h"
#include "fitd/gfx.h"
#include "fitd/input.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"
#include "common/scummsys.h"
#include "common/events.h"
#include "graphics/framelimiter.h"

namespace Fitd {

static void clearScreenTatou(void) {
	for (int i = 0; i < 45120; i++) {
		frontBuffer[i] = 0;
	}
}

void process_events() {
	Common::Event e;
	while (g_system->getEventManager()->pollEvent(e)) {
	}

	// TODO: fix this
	uint32 timeIncrease = 1;

	osystem_flushPendingPrimitives();
	g_system->updateScreen();
	osystem_startFrame();

	readKeyboard();
	timeGlobal += timeIncrease;
	timer = timeGlobal;
}

int make3dTatou(void) {
	char *tatou2d;
	char *tatou3d;
	byte *tatouPal;
	int time;
	int deltaTime;
	int rotation;
	int unk1;
	byte paletteBackup[768];
	unsigned int localChrono;

	tatou2d = checkLoadMallocPak("ITD_RESS.PAK", AITD1_TATOU_MCG);
	tatou3d = checkLoadMallocPak("ITD_RESS.PAK", AITD1_TATOU_3DO);
	tatouPal = (byte *)checkLoadMallocPak("ITD_RESS.PAK", AITD1_TATOU_PAL);

	time = 8920;
	deltaTime = 50;
	rotation = 256;
	unk1 = 8;

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

			affObjet(0, 0, 0, 0, 0, 0, tatou3d);

			// blitScreenTatou();
			gfx_copyBlockPhys((byte *)frontBuffer, 0, 0, 320, 200);

			process_events();

			copyPalette(tatouPal, currentGamePalette);
			gfx_setPalette(currentGamePalette);
			gfx_copyBlockPhys((byte *)frontBuffer, 0, 0, 320, 200);

			while (key == 0 && Click == 0 && JoyD == 0) // boucle de rotation du tatou
			{
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
	} while (1);

	free(tatouPal);
	free(tatou3d);
	free(tatou2d);

	if (key || Click || JoyD) {
		while (key) {
			process_events();
		}

		fadeOutPhys(32, 0);
		copyPalette((byte *)paletteBackup, currentGamePalette);
		return (1);
	} else {
		fadeOutPhys(16, 0);
		copyPalette((byte *)paletteBackup, currentGamePalette);
		return (0);
	}

	return (0);
}

} // namespace Fitd
