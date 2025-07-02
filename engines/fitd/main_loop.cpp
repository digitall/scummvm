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

	LastPriority = -1;
	LastSample = -1;
	if (nextSample != -1) {
		if (nextSample == 0x4000) {
			playRepeatedSound(0x4000 & 0x0BFFF);
		} else {
			playSound(nextSample);
		}
		nextSample = -1;
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
}

void mainLoop(int allowSystemMenu, int deltaTime) {
	while (!::Engine::shouldQuit()) {
		process_events();

		localKey = key;
		localJoyD = JoyD;
		localClick = Click;

		if (localKey) {
			if (localKey == 0x1B) {
				while (key == 0x1B) {
					process_events();
				}
				processSystemMenu();
				while ((key == 0x1B || key == 0x1C) && !::Engine::shouldQuit()) {
					process_events();
					localKey = key;
				}
			}

			if (localKey == 0x1C || localKey == 0x17) {
				if (allowSystemMenu == 0) {
					break;
				}

				if (statusScreenAllowed) {
					processInventory();
				}
			}
		} else {
			//      input5 = 0;
		}

		if (localClick) {
			if (!allowSystemMenu) {
				break;
			}

			action = 0x2000;
		} else {
			action = 0;
		}

		executeFoundLife(inHandTable[currentInventory]);

		if (changeFloor == 0) {
			if (g_engine->getGameId() == GID_AITD1) {
				if (CVars[getCVarsIdx(LIGHT_OBJECT)] == -1) {
					lightX = 2000;
					lightY = 2000;
				}
			}

			currentProcessedActorPtr = objectTable;

			for (currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_OBJECT; currentProcessedActorIdx++) {
				if (currentProcessedActorPtr->indexInWorld >= 0) {
					currentProcessedActorPtr->COL_BY = -1;
					currentProcessedActorPtr->HIT_BY = -1;
					currentProcessedActorPtr->HIT = -1;
					currentProcessedActorPtr->HARD_DEC = -1;
					currentProcessedActorPtr->HARD_COL = -1;
				}

				currentProcessedActorPtr++;
			}

			currentProcessedActorPtr = objectTable;
			for (currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_OBJECT; currentProcessedActorIdx++) {
				if (currentProcessedActorPtr->indexInWorld >= 0) {
					const int flag = currentProcessedActorPtr->_flags;

					if (flag & AF_ANIMATED || (g_engine->getGameId() >= GID_AITD2 && flag & 0x200)) {
						updateAnimation();
					}
					if (flag & AF_TRIGGER) {
						processActor2();
					}

					if (currentProcessedActorPtr->animActionType) {
						gereFrappe();
					}
				}

				currentProcessedActorPtr++;
			}

			currentProcessedActorPtr = objectTable;
			for (currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_OBJECT; currentProcessedActorIdx++) {
				if (currentProcessedActorPtr->indexInWorld >= 0) {
					if (currentProcessedActorPtr->life != -1) {
						switch (g_engine->getGameId()) {
						case GID_AITD2:
						case GID_AITD3:
						case GID_TIMEGATE: {
							if (currentProcessedActorPtr->lifeMode & 3)
								if (!(currentProcessedActorPtr->lifeMode & 4))
									processLife(currentProcessedActorPtr->life, false);
							break;
						}
						case GID_JACK:
						case GID_AITD1: {
							if (currentProcessedActorPtr->life != -1)
								if (currentProcessedActorPtr->lifeMode != -1)
									processLife(currentProcessedActorPtr->life, false);
							break;
						}
						default:
							break;
						}
					}
				}

				if (changeFloor)
					break;

				currentProcessedActorPtr++;
			}

			if (giveUp)
				break;
		}

		if (changeFloor) {
			loadFloor(newFloor);
		}

		if (needChangeRoom) {
			loadRoom(newRoom);
			setupCamera();
		} else {
			checkIfCameraChangeIsRequired();
			if (g_engine->getGameId() >= GID_AITD2) {

				const int tempCurrentCamera = currentCamera;

				currentCamera = startGameVar1;

				currentProcessedActorPtr = objectTable;
				for (currentProcessedActorIdx = 0; currentProcessedActorIdx < NUM_MAX_OBJECT; currentProcessedActorIdx++) {
					if (currentProcessedActorPtr->indexInWorld >= 0) {
						if (currentProcessedActorPtr->life != -1) {
							if (currentProcessedActorPtr->_flags & 0x200) {
								if (currentProcessedActorPtr->lifeMode & 3)
									if (!(currentProcessedActorPtr->lifeMode & 4)) {
										processLife(currentProcessedActorPtr->life, false);
										actorTurnedToObj = 1;
									}
							}
						}
					}

					if (changeFloor)
						break;

					currentProcessedActorPtr++;
				}

				if (giveUp)
					break;

				currentCamera = tempCurrentCamera;
			}
			if (flagInitView
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

		//    if(actorTurnedToObj)
		{
			createActorList();
		}

		sortActorList();

		//    if(FlagRefreshAux2)
		{
			//      setupCameraSub4();
		}

		//    mainLoopSub1();

		// osystem_delay(100);

		mainDraw(flagRedraw);

		updatePendingEvents();
	}

	flagRotPal = 0;
	//  shakingState = 0;

	//  stopShaking();
	//  stopSounds();
}
} // namespace Fitd
