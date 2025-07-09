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

#include "common/scummsys.h"
#include "fitd/actor_list.h"
#include "fitd/anim.h"
#include "fitd/anim_action.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/floor.h"
#include "fitd/gfx.h"
#include "fitd/inventory.h"
#include "fitd/life.h"
#include "fitd/main_loop.h"
#include "fitd/room.h"
#include "fitd/system_menu.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"

namespace Fitd {

int mainLoopSwitch = 0;

void updatePendingEvents() {
	// TODO: miss pending events here

	g_engine->_engine->lastPriority = -1;
	g_engine->_engine->lastSample = -1;
	if (g_engine->_engine->nextSample != -1) {
		if (g_engine->_engine->nextSample == 0x4000) {
			playRepeatedSound(0x4000 & 0x0BFFF);
		} else {
			playSound(g_engine->_engine->nextSample);
		}
		g_engine->_engine->nextSample = -1;
	}
	//     if(currentMusic!=-1)
	//     {
	//         if(currentMusic==-2)
	//         {
	//             if(evalChrono(&musicChrono)>180)
	//             {
	//                 playMusic(nextMusic);
	//             }
	//         }
	//         else
	//         {
	// 			/*
	//             if(fadeMusic(0,0,0x10)==-1)
	//             {
	//                 currentMusic = -1;

	//                 if(nextMusic != -1)
	//                 {
	//                     playMusic(nextMusic);
	//                     nextMusic = -1;
	//                 }
	//             }
	// 			*/
	//         }
	//     }

	if (g_engine->_engine->newFlagLight != 0) {
		g_engine->_engine->newFlagLight = 0;
		if (g_engine->_engine->cVars[getCVarsIdx(KILLED_SORCERER)] != 0) {
			if (g_engine->_engine->lightOff) {
				fadeOutPhys(64, 2);
				memset(g_engine->_engine->logicalScreen, 0, 320 * 200);
				gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
				gfx_setPalette(g_engine->_engine->currentGamePalette);
			} else {
				makeBlackPalette();
				osystem_updateScreen();
				fadeInPhys(64, 0);
			}
		}
	}
}

void mainLoop(int allowSystemMenu, int deltaTime) {
	while (!::Engine::shouldQuit()) {
		process_events();

		g_engine->_engine->localKey = g_engine->_engine->key;
		g_engine->_engine->localJoyD = g_engine->_engine->joyD;
		g_engine->_engine->localClick = g_engine->_engine->click;

		if (g_engine->_engine->localKey) {
			if (g_engine->_engine->localKey == 0x1B) {
				while (g_engine->_engine->key == 0x1B) {
					process_events();
				}
				processSystemMenu();
				while ((g_engine->_engine->key == 0x1B || g_engine->_engine->key == 0x1C) && !::Engine::shouldQuit()) {
					process_events();
					g_engine->_engine->localKey = g_engine->_engine->key;
				}
			}

			if (g_engine->_engine->localKey == 0x1C || g_engine->_engine->localKey == 0x17) {
				if (allowSystemMenu == 0) {
					break;
				}

				if (g_engine->_engine->statusScreenAllowed) {
					processInventory();
				}
			}
		} else {
			//      input5 = 0;
		}

		if (g_engine->_engine->localClick) {
			if (!allowSystemMenu) {
				break;
			}

			g_engine->_engine->action = 0x2000;
		} else {
			g_engine->_engine->action = 0;
		}

		executeFoundLife(g_engine->_engine->inHandTable);

		if (g_engine->_engine->changeFloor == 0) {
			if (g_engine->getGameId() == GID_AITD1) {
				if (g_engine->_engine->cVars[getCVarsIdx(LIGHT_OBJECT)] == -1) {
					g_engine->_engine->lightX = 2000;
					g_engine->_engine->lightY = 2000;
				}
			}

			g_engine->_engine->currentProcessedActorPtr = g_engine->_engine->objectTable;

			for (g_engine->_engine->currentProcessedActorIdx = 0; g_engine->_engine->currentProcessedActorIdx < NUM_MAX_OBJECT; g_engine->_engine->currentProcessedActorIdx++) {
				if (g_engine->_engine->currentProcessedActorPtr->indexInWorld >= 0) {
					g_engine->_engine->currentProcessedActorPtr->COL_BY = -1;
					g_engine->_engine->currentProcessedActorPtr->HIT_BY = -1;
					g_engine->_engine->currentProcessedActorPtr->HIT = -1;
					g_engine->_engine->currentProcessedActorPtr->HARD_DEC = -1;
					g_engine->_engine->currentProcessedActorPtr->HARD_COL = -1;
				}

				g_engine->_engine->currentProcessedActorPtr++;
			}

			g_engine->_engine->currentProcessedActorPtr = g_engine->_engine->objectTable;
			for (g_engine->_engine->currentProcessedActorIdx = 0; g_engine->_engine->currentProcessedActorIdx < NUM_MAX_OBJECT; g_engine->_engine->currentProcessedActorIdx++) {
				if (g_engine->_engine->currentProcessedActorPtr->indexInWorld >= 0) {
					const int flag = g_engine->_engine->currentProcessedActorPtr->_flags;

					if (flag & AF_ANIMATED || (g_engine->getGameId() >= GID_AITD2 && flag & 0x200)) {
						updateAnimation();
					}
					if (flag & AF_TRIGGER) {
						processActor2();
					}

					if (g_engine->_engine->currentProcessedActorPtr->animActionType) {
						gereFrappe();
					}
				}

				g_engine->_engine->currentProcessedActorPtr++;
			}

			g_engine->_engine->currentProcessedActorPtr = g_engine->_engine->objectTable;
			for (g_engine->_engine->currentProcessedActorIdx = 0; g_engine->_engine->currentProcessedActorIdx < NUM_MAX_OBJECT; g_engine->_engine->currentProcessedActorIdx++) {
				if (g_engine->_engine->currentProcessedActorPtr->indexInWorld >= 0) {
					if (g_engine->_engine->currentProcessedActorPtr->life != -1) {
						switch (g_engine->getGameId()) {
						case GID_AITD2:
						case GID_AITD3:
						case GID_TIMEGATE: {
							if (g_engine->_engine->currentProcessedActorPtr->lifeMode & 3)
								if (!(g_engine->_engine->currentProcessedActorPtr->lifeMode & 4))
									processLife(g_engine->_engine->currentProcessedActorPtr->life, false);
							break;
						}
						case GID_JACK:
						case GID_AITD1: {
							if (g_engine->_engine->currentProcessedActorPtr->life != -1)
								if (g_engine->_engine->currentProcessedActorPtr->lifeMode != -1)
									processLife(g_engine->_engine->currentProcessedActorPtr->life, false);
							break;
						}
						default:
							break;
						}
					}
				}

				if (g_engine->_engine->changeFloor)
					break;

				g_engine->_engine->currentProcessedActorPtr++;
			}

			if (g_engine->_engine->giveUp)
				break;
		}

		if (g_engine->_engine->changeFloor) {
			loadFloor(g_engine->_engine->newFloor);
		}

		if (g_engine->_engine->needChangeRoom) {
			loadRoom(g_engine->_engine->newRoom);
			setupCamera();
		} else {
			checkIfCameraChangeIsRequired();
			if (g_engine->getGameId() >= GID_AITD2) {

				const int tempCurrentCamera = g_engine->_engine->currentCamera;

				g_engine->_engine->currentCamera = g_engine->_engine->startGameVar1;

				g_engine->_engine->currentProcessedActorPtr = g_engine->_engine->objectTable;
				for (g_engine->_engine->currentProcessedActorIdx = 0; g_engine->_engine->currentProcessedActorIdx < NUM_MAX_OBJECT; g_engine->_engine->currentProcessedActorIdx++) {
					if (g_engine->_engine->currentProcessedActorPtr->indexInWorld >= 0) {
						if (g_engine->_engine->currentProcessedActorPtr->life != -1) {
							if (g_engine->_engine->currentProcessedActorPtr->_flags & 0x200) {
								if (g_engine->_engine->currentProcessedActorPtr->lifeMode & 3)
									if (!(g_engine->_engine->currentProcessedActorPtr->lifeMode & 4)) {
										processLife(g_engine->_engine->currentProcessedActorPtr->life, false);
										g_engine->_engine->actorTurnedToObj = 1;
									}
							}
						}
					}

					if (g_engine->_engine->changeFloor)
						break;

					g_engine->_engine->currentProcessedActorPtr++;
				}

				if (g_engine->_engine->giveUp)
					break;

				g_engine->_engine->currentCamera = tempCurrentCamera;
			}
			if (g_engine->_engine->flagInitView
#ifdef FITD_DEBUGGER
				|| debuggerVar_topCamera
#endif
			) {
				setupCamera();
			}
		}

		//    if(FlagGenereActiveList)
		{
			updateAllActorAndObjects();
		}

		//    if(g_engine->_engine->actorTurnedToObj)
		{
			createActorList();
		}

		sortActorList();

		if (g_engine->_engine->flagRefreshAux2) {
			refreshAux2();
		}

		//    gereAux2();

		mainDraw(g_engine->_engine->flagRedraw);

		updatePendingEvents();
	}

	g_engine->_engine->flagRotPal = 0;
	g_engine->_engine->shakeVar1 = 0;

	//  stopShaking();
	//  stopSounds();
}
} // namespace Fitd
