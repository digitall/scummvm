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

#include "fitd/aitd2.h"
#include "fitd/aitd3.h"
#include "fitd/aitd_box.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/font.h"
#include "fitd/game_time.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/life.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"

namespace Fitd {

static int numInventoryActions;
static int16 inventoryActionTable[5];

static int drawListObjets(int startIdx, int selectIdx, int selectColor) {
	int y;
	const int var_6 = startIdx;
	int var_8 = 0;

	if (g_engine->getGameId() <= GID_JACK) {
		affBigCadre(160, 50, 320, 100);
		y = g_engine->_engine->windowY1 + 1;
	} else {
		setClip(27, 25, 292, 98);
		fillBox(27, 25, 292, 98, 0);

		g_engine->_engine->windowX1 = 30;
		g_engine->_engine->windowY1 = 27;
		g_engine->_engine->windowX2 = 288;
		g_engine->_engine->windowY2 = 95;

		y = 28;
	}
	for (int i = 0; i < 5; i++) {
		if (startIdx >= g_engine->_engine->numObjInInventoryTable)
			break;

		const int currentObj = g_engine->_engine->inventoryTable[startIdx];

		const WorldObject *objPtr = &g_engine->_engine->worldObjets[currentObj];

		if (startIdx == selectIdx) {
			if (g_engine->getGameId() <= GID_JACK) {
				if (selectColor == 15) {
					fillBox(0xA, y, 0x135, y + 0x10, 0x64);
				}

				selectedMessage(160, y, objPtr->foundName, selectColor, 4);
			} else {
				simpleMessage(160, y, objPtr->foundName, selectColor);
			}

			var_8 = currentObj;
		} else {
			simpleMessage(160, y, objPtr->foundName, 4);
		}

		y += g_engine->_engine->fontHeight;
		startIdx++;
	}

	if (var_6 > 0) {
		affSpfI(298, 10, 10, g_engine->_engine->ptrCadre);
	}

	if (var_6 + 5 < g_engine->_engine->numObjInInventoryTable) {
		affSpfI(298, 74, 9, g_engine->_engine->ptrCadre);
	}

	return var_8;
}

static void renderInventoryObject(int arg) {
	setClip(g_engine->_engine->statusLeft, g_engine->_engine->statusTop, g_engine->_engine->statusRight, g_engine->_engine->statusBottom);
	fillBox(g_engine->_engine->statusLeft, g_engine->_engine->statusTop, g_engine->_engine->statusRight, g_engine->_engine->statusBottom, 0);

	g_engine->_engine->statusVar1 -= 8;

	setCameraTarget(0, 0, 0, 60, g_engine->_engine->statusVar1, 0, 24000);
	affObjet(0, 0, 0, 0, 0, 0, g_engine->_engine->currentFoundBody);

	if (arg != -1) {
		Common::String buffer;
		extSetFont(g_engine->_engine->ptrFont, 4);
		buffer = Common::String::format("%d", g_engine->_engine->vars[arg]);
		renderText(g_engine->_engine->statusLeft + 4, g_engine->_engine->statusTop + 4, buffer.c_str());
	}
	switch (g_engine->getGameId()) {
	case GID_AITD2:
		aitd2RedrawInventorySprite();
		break;
	default:
		break;
	}
}

static void drawInventoryActions(int arg) {
	int y = 0;

	if (g_engine->getGameId() <= GID_JACK) {
		affBigCadre(240, 150, 160, 100);
		y = 150 - (numInventoryActions << 4) / 2;
	} else {
		setClip(162, 100, 292, 174);
		fillBox(162, 100, 292, 174, 0);

		g_engine->_engine->windowX1 = 166;
		g_engine->_engine->windowY1 = 104;
		g_engine->_engine->windowX2 = 288;
		g_engine->_engine->windowY2 = 170;

		y = 139 - numInventoryActions * g_engine->_engine->fontHeight / 2;
	}

	for (int i = 0; i < numInventoryActions; i++) {
		if (arg == i) {
			if (g_engine->getGameId() <= GID_JACK) {
				fillBox(170, y, 309, y + 16, 100);
				selectedMessage(240, y, inventoryActionTable[i], 15, 4);
			} else {
				simpleMessage(240, y, inventoryActionTable[i], 1);
			}
		} else {
			simpleMessage(240, y, inventoryActionTable[i], 4);
		}

		y += g_engine->_engine->fontHeight;
	}

	switch (g_engine->getGameId()) {
	case GID_AITD2:
		aitd2RedrawInventorySprite();
		break;
	default:
		break;
	}
}

void processInventory() {
	int exitMenu = 0;
	int choice = 0;
	int firstTime = 1;
	uint chrono;
	int selectedWorldObjectIdx = 0;
	int selectedActions = 0;

	if (!g_engine->_engine->numObjInInventoryTable)
		return;

	int firstObjectDisplayedIdx = 0;
	int lastSelectedObjectIdx = -1;
	int selectedObjectIdx = 0;
	int modeSelect = 0;
	int antiBounce = 2;

	g_engine->_engine->statusVar1 = 0;

	freezeTime();
	saveAmbiance();

	if (g_engine->_engine->lightOff != 0) {
		makeBlackPalette();
	}

	switch (g_engine->getGameId()) {
	case GID_AITD1:
	case GID_JACK:
		affBigCadre(80, 150, 160, 100);

		g_engine->_engine->statusLeft = g_engine->_engine->windowX1;
		g_engine->_engine->statusTop = g_engine->_engine->windowY1;
		g_engine->_engine->statusRight = g_engine->_engine->windowX2;
		g_engine->_engine->statusBottom = g_engine->_engine->windowY2;

		setupCameraProjection((g_engine->_engine->statusRight - g_engine->_engine->statusLeft) / 2 + g_engine->_engine->statusLeft, (g_engine->_engine->statusBottom - g_engine->_engine->statusTop) / 2 + g_engine->_engine->statusTop, 128, 400, 390);

		break;
	case GID_AITD2:
		aitd2DrawInventory();
		break;
	case GID_AITD3:
		aitd3DrawInventory();
		break;
	default:
		assert(0);
	}
	while (!exitMenu) {
		/*
		osystem_CopyBlockPhys((byte*)backbuffer,0,0,320,200);
		osystem_startFrame();
		osystem_cleanScreenKeepZBuffer();
		*/

		process_events();
		drawBackground();

		g_engine->_engine->localKey = g_engine->_engine->key;
		g_engine->_engine->localJoyD = g_engine->_engine->joyD;
		g_engine->_engine->localClick = g_engine->_engine->click;

		if (!g_engine->_engine->localKey && !g_engine->_engine->localJoyD && !g_engine->_engine->localClick) {
			antiBounce = 0;
		}

		if (g_engine->_engine->localKey == 1 || g_engine->_engine->localKey == 0x1B) {
			choice = 0;
			exitMenu = 1;
		}

		if (modeSelect == 0) {
			if (antiBounce < 1) {
				if (g_engine->_engine->localKey == 0x1C || g_engine->_engine->localClick != 0 || g_engine->_engine->localJoyD == 0xC) {
					drawListObjets(firstObjectDisplayedIdx, selectedObjectIdx, 14);
					gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
					modeSelect = 1;
					lastSelectedObjectIdx = -1;
					selectedActions = 0;
					antiBounce = 2;
					continue;
				}
				if (g_engine->_engine->localJoyD & 1 && selectedObjectIdx > 0) {
					selectedObjectIdx--;
				}

				if (g_engine->_engine->localJoyD & 2 && selectedObjectIdx < g_engine->_engine->numObjInInventoryTable - 1) {
					selectedObjectIdx++;
				}

				if (firstObjectDisplayedIdx + 5 <= selectedObjectIdx) {
					firstObjectDisplayedIdx++;
				}

				if (selectedObjectIdx < firstObjectDisplayedIdx) {
					firstObjectDisplayedIdx--;
				}

				if (g_engine->_engine->localKey || g_engine->_engine->localJoyD || g_engine->_engine->localClick) {
					if (antiBounce == 0) {
						antiBounce = 1;
						startChrono(&chrono);
					}
				}
			} else {
				if (antiBounce == 1) {
					if (evalChrono(&chrono) > 0x280000) {
						antiBounce = -1;
					}
				}
			}

			if (lastSelectedObjectIdx != selectedObjectIdx) {
				selectedWorldObjectIdx = drawListObjets(firstObjectDisplayedIdx, selectedObjectIdx, 15);

				g_engine->_engine->currentFoundBodyIdx = g_engine->_engine->worldObjets[selectedWorldObjectIdx].foundBody;

				g_engine->_engine->currentFoundBody = hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentFoundBodyIdx).data;

				const int var_C = g_engine->_engine->worldObjets[selectedWorldObjectIdx].flags2;

				numInventoryActions = 0;
				int numActionForObject = 0;

				while (numActionForObject < 11) {
					if (var_C & 1 << numActionForObject) {
						if (numInventoryActions < 5) {
							inventoryActionTable[numInventoryActions++] = numActionForObject + 23;
						}
					}
					numActionForObject++;
				}

				drawInventoryActions(-1);
				// osystem_flip(NULL);

				lastSelectedObjectIdx = selectedObjectIdx;
			}
		} else // actions
		{
			if (antiBounce < 1) {
				if (g_engine->_engine->localKey == 0x1C || g_engine->_engine->localClick) {
					selectedObjectIdx = g_engine->_engine->inventoryTable[selectedObjectIdx];
					g_engine->_engine->action = 1 << (inventoryActionTable[selectedActions] - 23);
					choice = 1;
					exitMenu = 1;
				}

				if (g_engine->_engine->localJoyD & 0xC) {
					drawInventoryActions(-1);
					modeSelect = 0;
					lastSelectedObjectIdx = -1;
					antiBounce = 2;
					continue;
				}

				if (g_engine->_engine->localJoyD & 1 && selectedActions > 0) {
					selectedActions--;
				}

				if (g_engine->_engine->localJoyD & 2 && selectedActions < numInventoryActions - 1) {
					selectedActions++;
				}

				if (g_engine->_engine->localKey || g_engine->_engine->localJoyD || g_engine->_engine->localClick) {
					if (antiBounce == 0) {
						antiBounce = 1;
						startChrono(&chrono);
					}
				}
			} else {
				if (antiBounce == 1) {
					if (evalChrono(&chrono) > 0x280000) {
						antiBounce = -1;
					}
				}

				if (lastSelectedObjectIdx != selectedActions) {
					lastSelectedObjectIdx = selectedActions;
					drawInventoryActions(lastSelectedObjectIdx);
				}
			}
		}
		renderInventoryObject(g_engine->_engine->worldObjets[selectedWorldObjectIdx].floorLife);

		if (firstTime) {
			firstTime = 0;
			if (g_engine->_engine->lightOff) {
				fadeInPhys(0x40, 0);
			}
		}

		gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
		// osystem_flip(NULL);
	}

	unfreezeTime();

	g_engine->_engine->flagInitView = 1;

	while (g_engine->_engine->click || g_engine->_engine->key || g_engine->_engine->joyD) {
		process_events();
	}

	g_engine->_engine->localJoyD = 0;
	g_engine->_engine->localKey = 0;
	g_engine->_engine->localClick = 0;

	if (choice == 1) {
		executeFoundLife(selectedObjectIdx);
	}

	restoreAmbiance();
}
} // namespace Fitd
