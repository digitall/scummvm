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

#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/hqr.h"
#include "fitd/room.h"
#include "fitd/vars.h"

namespace Fitd {

static void initBufferAnim(int16 *buffer, byte *bodyPtr);

int setAnimObjet(int frame, byte *anim, byte *body) {

	const int flag = *(int16 *)body;

	const int16 temp = *(int16 *)anim;
	anim += 2;

	if (frame >= temp) {
		return 0;
	}

	int16 ax = *(int16 *)anim;
	anim += 2;

	int16 cx = ax;

	if (flag & INFO_OPTIMISE) {
		ax = ((ax << 4) + 8) * frame;
	} else {
		ax = ((ax + 1) << 3) * frame;
	}

	anim += ax;

	g_engine->_engine->animCurrentTime = *(int16 *)anim;
	g_engine->_engine->animKeyframeLength = g_engine->_engine->animCurrentTime;

	if (!(flag & 2)) {
		return 0;
	}

	body += 14;

	g_engine->_engine->bodyBufferMap[body + 2] = anim;
	*(uint16 *)(body + 6) = g_engine->_engine->timer;

	body += *(int16 *)body;
	body += 2;

	ax = *(int16 *)body;
	int16 bx = ax;

	body += (((ax << 1) + bx) << 1) + 2;

	bx = ax = *(int16 *)body;

	body += bx << 1;

	if (cx > ax)
		cx = ax;

	body += 10;

	byte *saveAnim = anim;

	anim += 8;

	for (int i = 0; i < cx; i++) {
		*(int16 *)body = *(int16 *)anim;
		body += 2;
		anim += 2;
		*(int16 *)body = *(int16 *)anim;
		body += 2;
		anim += 2;
		*(int16 *)body = *(int16 *)anim;
		body += 2;
		anim += 2;
		*(int16 *)body = *(int16 *)anim;
		body += 2;
		anim += 2;

		if (flag & INFO_OPTIMISE) {
			*(int16 *)body = *(int16 *)anim;
			body += 2;
			anim += 2;
			*(int16 *)body = *(int16 *)anim;
			body += 2;
			anim += 2;
			*(int16 *)body = *(int16 *)anim;
			body += 2;
			anim += 2;
			*(int16 *)body = *(int16 *)anim;
			body += 2;
			anim += 2;
		}

		body += 8;
	}

	anim = saveAnim;

	anim += 2;

	g_engine->_engine->animStepX = *(int16 *)anim;
	anim += 2;
	g_engine->_engine->animStepY = *(int16 *)anim;
	anim += 2;
	g_engine->_engine->animStepZ = *(int16 *)anim;
	anim += 2;

	return 1;
}

int initAnim(int animNum, int animType, int animInfo) {
	if (animNum == g_engine->_engine->currentProcessedActorPtr->ANIM) {
		if (!(g_engine->_engine->currentProcessedActorPtr->_flags & AF_ANIMATED)) {
			if (g_engine->_engine->currentProcessedActorPtr->_flags & AF_BOXIFY) {
				removeFromBGIncrust(g_engine->_engine->currentProcessedActorIdx);
			}

			g_engine->_engine->currentProcessedActorPtr->_flags |= AF_ANIMATED;

			setAnimObjet(g_engine->_engine->currentProcessedActorPtr->FRAME, hqrGet(g_engine->_engine->listAnim, animNum).data, hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentProcessedActorPtr->bodyNum).data);

			g_engine->_engine->currentProcessedActorPtr->animType = animType;
			g_engine->_engine->currentProcessedActorPtr->animInfo = animInfo;

			if (g_engine->getGameId() > GID_AITD1) {
				g_engine->_engine->currentProcessedActorPtr->FRAME = 0;
			}

			return 1;
		} else {
			g_engine->_engine->currentProcessedActorPtr->animType = animType;
			g_engine->_engine->currentProcessedActorPtr->animInfo = animInfo;
			return 0;
		}
	}

	if (animNum == -1) {
		g_engine->_engine->currentProcessedActorPtr->newAnim = -2;
		return 1;
	}

	if (!(g_engine->_engine->currentProcessedActorPtr->_flags & AF_ANIMATED)) {
		g_engine->_engine->currentProcessedActorPtr->_flags |= AF_ANIMATED;

		if (g_engine->_engine->currentProcessedActorPtr->_flags & AF_BOXIFY) {
			removeFromBGIncrust(g_engine->_engine->currentProcessedActorIdx);
		}

		setAnimObjet(0, hqrGet(g_engine->_engine->listAnim, animNum).data, hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentProcessedActorPtr->bodyNum).data);

		g_engine->_engine->currentProcessedActorPtr->newAnim = animNum;
		g_engine->_engine->currentProcessedActorPtr->newAnimType = animType;
		g_engine->_engine->currentProcessedActorPtr->newAnimInfo = animInfo;
		if (g_engine->getGameId() > GID_AITD1) {
			g_engine->_engine->currentProcessedActorPtr->FRAME = 0;
		}
		return 1;
	}

	if (g_engine->getGameId() == GID_AITD1) {
		if (g_engine->_engine->currentProcessedActorPtr->animType & ANIM_UNINTERRUPTABLE)
			return 0;

		if (g_engine->_engine->currentProcessedActorPtr->newAnimType & ANIM_UNINTERRUPTABLE)
			return 0;
	} else {
		if (g_engine->_engine->currentProcessedActorPtr->animType & ANIM_UNINTERRUPTABLE) {
			if (g_engine->_engine->currentProcessedActorPtr->newAnimType & ANIM_UNINTERRUPTABLE) {
				return 0;
			} else {
				g_engine->_engine->currentProcessedActorPtr->animInfo = animNum;
				return 1;
			}
		}
	}

	g_engine->_engine->currentProcessedActorPtr->newAnim = animNum;
	g_engine->_engine->currentProcessedActorPtr->newAnimType = animType;
	g_engine->_engine->currentProcessedActorPtr->newAnimInfo = animInfo;

	if (g_engine->getGameId() != GID_AITD1) {
		g_engine->_engine->currentProcessedActorPtr->FRAME = 0;
	}

	return 1;
}

int evaluateReal(InterpolatedValue *data) {
	if (!data->param)
		return data->newValue;

	if (g_engine->_engine->timer - data->timeOfRotate > static_cast<uint>(data->param)) {
		data->param = 0;
		return data->newValue;
	}

	return (data->newValue - data->oldValue) * (g_engine->_engine->timer - data->timeOfRotate) / data->param + data->oldValue;
}

int manageFall(int actorIdx, ZVStruct *zvPtr) {
	int fallResult = 0;
	const int room = g_engine->_engine->objectTable[actorIdx].room;

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		const Object *currentTestedActorPtr = &g_engine->_engine->objectTable[i];

		if (currentTestedActorPtr->indexInWorld != -1 && i != actorIdx) {
			const ZVStruct *testedZv = &currentTestedActorPtr->zv;

			if (currentTestedActorPtr->room != room) {
				ZVStruct localZv;
				copyZv(zvPtr, &localZv);
				getZvRelativePosition(&localZv, room, currentTestedActorPtr->room);

				if (checkZvCollision(&localZv, testedZv)) {
					g_engine->_engine->objectTable[i].COL_BY = actorIdx;
					fallResult++;
				}
			} else {
				if (checkZvCollision(zvPtr, testedZv)) {
					g_engine->_engine->objectTable[i].COL_BY = actorIdx;
					fallResult++;
				}
			}
		}
	}

	return fallResult;
}

void updateAnimation() {
	int oldStepZ = 0;
	int oldStepY = 0;
	int oldStepX = 0;
	int stepZ = 0;
	int stepY = 0;
	int stepX = 0;
	int16 localTable[3];
	ZVStruct zvLocal;
	ZVStruct *zvPtr;

	int newAnim = g_engine->_engine->currentProcessedActorPtr->newAnim;

	if (newAnim != -1) // next anim ?
	{
		if (newAnim == -2) // completely stop anim and add actor to background
		{
			addActorToBgInscrust(g_engine->_engine->currentProcessedActorIdx);
			g_engine->_engine->currentProcessedActorPtr->newAnim = -1;
			g_engine->_engine->currentProcessedActorPtr->newAnimType = 0;
			g_engine->_engine->currentProcessedActorPtr->newAnimInfo = -1;
			g_engine->_engine->currentProcessedActorPtr->END_ANIM = 1;

			return;
		}

		if (g_engine->_engine->currentProcessedActorPtr->END_FRAME == 0) {
			g_engine->_engine->currentProcessedActorPtr->worldX += g_engine->_engine->currentProcessedActorPtr->stepX;
			g_engine->_engine->currentProcessedActorPtr->roomX += g_engine->_engine->currentProcessedActorPtr->stepX;

			g_engine->_engine->currentProcessedActorPtr->worldZ += g_engine->_engine->currentProcessedActorPtr->stepZ;
			g_engine->_engine->currentProcessedActorPtr->roomZ += g_engine->_engine->currentProcessedActorPtr->stepZ;

			g_engine->_engine->currentProcessedActorPtr->stepX = 0;
			g_engine->_engine->currentProcessedActorPtr->stepZ = 0;

			g_engine->_engine->currentProcessedActorPtr->animNegX = 0;
			g_engine->_engine->currentProcessedActorPtr->animNegY = 0;
			g_engine->_engine->currentProcessedActorPtr->animNegZ = 0;
		}

		initBufferAnim(g_engine->_engine->bufferAnim[g_engine->_engine->bufferAnimCounter], hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentProcessedActorPtr->bodyNum).data);

		g_engine->_engine->bufferAnimCounter++;
		if (g_engine->_engine->bufferAnimCounter == NB_BUFFER_ANIM)
			g_engine->_engine->bufferAnimCounter = 0;

		g_engine->_engine->currentProcessedActorPtr->ANIM = newAnim;
		g_engine->_engine->currentProcessedActorPtr->animType = g_engine->_engine->currentProcessedActorPtr->newAnimType;
		g_engine->_engine->currentProcessedActorPtr->animInfo = g_engine->_engine->currentProcessedActorPtr->newAnimInfo;
		g_engine->_engine->currentProcessedActorPtr->newAnim = -1;
		g_engine->_engine->currentProcessedActorPtr->newAnimType = 0;
		g_engine->_engine->currentProcessedActorPtr->newAnimInfo = -1;
		g_engine->_engine->currentProcessedActorPtr->END_ANIM = 0;
		g_engine->_engine->currentProcessedActorPtr->FRAME = 0;

		g_engine->_engine->currentProcessedActorPtr->numOfFrames = getNbFramesAnim(hqrGet(g_engine->_engine->listAnim, newAnim).data);
	}

	if (g_engine->_engine->currentProcessedActorPtr->ANIM == -1) // no animation
	{
		g_engine->_engine->currentProcessedActorPtr->END_FRAME = 0;
		if (g_engine->_engine->currentProcessedActorPtr->speed == 0) {
			int numObjectCollisions = checkObjectCollisions(g_engine->_engine->currentProcessedActorIdx, &g_engine->_engine->currentProcessedActorPtr->zv);

			for (int i = 0; i < numObjectCollisions; i++) {
				g_engine->_engine->objectTable[g_engine->_engine->currentProcessedActorPtr->COL[i]].COL_BY = g_engine->_engine->currentProcessedActorIdx; // collision with current actor
			}

			oldStepY = 0;
			oldStepZ = 0;
			stepX = 0;
			stepZ = 0;
			stepY = 0;
		} else {
			oldStepX = g_engine->_engine->currentProcessedActorPtr->stepX;
			oldStepY = g_engine->_engine->currentProcessedActorPtr->stepY;
			oldStepZ = g_engine->_engine->currentProcessedActorPtr->stepZ;

			g_engine->_engine->animStepY = 0;
			g_engine->_engine->animStepX = 0;

			g_engine->_engine->animStepZ = evaluateReal(&g_engine->_engine->currentProcessedActorPtr->speedChange);

			walkStep(0, g_engine->_engine->animStepZ, g_engine->_engine->currentProcessedActorPtr->beta);

			stepX = g_engine->_engine->animMoveX - oldStepX;
			stepZ = g_engine->_engine->animMoveZ - oldStepZ;
			stepY = 0;
		}
	} else // animation
	{
		oldStepX = g_engine->_engine->currentProcessedActorPtr->stepX;
		oldStepY = g_engine->_engine->currentProcessedActorPtr->stepY;
		oldStepZ = g_engine->_engine->currentProcessedActorPtr->stepZ;

		g_engine->_engine->currentProcessedActorPtr->END_FRAME = setInterAnimObjet(g_engine->_engine->currentProcessedActorPtr->FRAME, hqrGet(g_engine->_engine->listAnim, g_engine->_engine->currentProcessedActorPtr->ANIM).data, hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentProcessedActorPtr->bodyNum).data);

		walkStep(g_engine->_engine->animStepX, g_engine->_engine->animStepZ, g_engine->_engine->currentProcessedActorPtr->beta);

		stepX = g_engine->_engine->animMoveX + g_engine->_engine->currentProcessedActorPtr->animNegX - oldStepX;
		stepZ = g_engine->_engine->animMoveZ + g_engine->_engine->currentProcessedActorPtr->animNegZ - oldStepZ;
	}

	if (g_engine->_engine->currentProcessedActorPtr->YHandler.param) // currently falling ?
	{
		if (g_engine->_engine->currentProcessedActorPtr->YHandler.param != -1) {
			stepY = evaluateReal(&g_engine->_engine->currentProcessedActorPtr->YHandler) - oldStepY;
		} else // stop falling
		{
			stepY = g_engine->_engine->currentProcessedActorPtr->YHandler.newValue - oldStepY;

			g_engine->_engine->currentProcessedActorPtr->YHandler.param = 0;
			g_engine->_engine->currentProcessedActorPtr->YHandler.newValue = 0;
			g_engine->_engine->currentProcessedActorPtr->YHandler.oldValue = 0;
		}
	} else {
		stepY = 0;
	}

	memcpy(localTable, g_engine->_engine->currentProcessedActorPtr->COL, 6);

	if (stepX || stepY || stepZ) // start of movement management
	{
		zvPtr = &g_engine->_engine->currentProcessedActorPtr->zv;
		copyZv(&g_engine->_engine->currentProcessedActorPtr->zv, &zvLocal);

		zvLocal.ZVX1 += stepX;
		zvLocal.ZVX2 += stepX;

		zvLocal.ZVY1 += stepY;
		zvLocal.ZVY2 += stepY;

		zvLocal.ZVZ1 += stepZ;
		zvLocal.ZVZ2 += stepZ;

		if (g_engine->_engine->currentProcessedActorPtr->dynFlags & 1) // hard collision enabled for actor ?
		{
			int numCol = asmCheckListCol(&zvLocal, &g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room]);

			for (int i = 0; i < numCol; i++) {
				HardCol *pHardCol = g_engine->_engine->hardColTable[i];

				if (pHardCol->type == 9) {
					g_engine->_engine->currentProcessedActorPtr->HARD_COL = static_cast<int16>(pHardCol->parameter);
				}

				if (pHardCol->type == 3) {
					g_engine->_engine->currentProcessedActorPtr->HARD_COL = 255;
				}

				if (g_engine->getGameId() == GID_AITD1 || (g_engine->getGameId() >= GID_JACK && (pHardCol->type != 10 || g_engine->_engine->currentProcessedActorIdx != g_engine->_engine->currentCameraTargetActor))) {
					if (stepX || stepZ) // move on the X or Z axis ? update to avoid entering the hard col
					{
						// ZVStruct tempZv;

						g_engine->_engine->hardColStepX = stepX;
						g_engine->_engine->hardColStepZ = stepZ;

						handleCollision(zvPtr, &zvLocal, &pHardCol->zv);

						if (g_engine->getGameId() != GID_AITD1) {
							g_engine->_engine->currentProcessedActorPtr->animNegX += g_engine->_engine->hardColStepX - stepX;
							g_engine->_engine->currentProcessedActorPtr->animNegZ += g_engine->_engine->hardColStepZ - stepZ;
						}

						zvLocal.ZVX1 += g_engine->_engine->hardColStepX - stepX;
						zvLocal.ZVX2 += g_engine->_engine->hardColStepX - stepX;
						zvLocal.ZVZ1 += g_engine->_engine->hardColStepZ - stepZ;
						zvLocal.ZVZ2 += g_engine->_engine->hardColStepZ - stepZ;

						stepX = g_engine->_engine->hardColStepX;
						stepZ = g_engine->_engine->hardColStepZ;
					}

					if (stepY) {
						// assert(0); //not implemented
					}
				}
			}
		} else // no hard collision -> just update the flag without performing the position update
		{
			if (asmCheckListCol(&zvLocal, &g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room])) {
				g_engine->_engine->currentProcessedActorPtr->HARD_COL = 1;
			} else {
				g_engine->_engine->currentProcessedActorPtr->HARD_COL = 0;
			}
		}

		int numCol = checkObjectCollisions(g_engine->_engine->currentProcessedActorIdx, &zvLocal); // get the number of actor/actor collision

		for (int j = 0; j < numCol; j++) // process the actor/actor collision
		{
			int collisionIndex = g_engine->_engine->currentProcessedActorPtr->COL[j];

			Object *actorTouchedPtr = &g_engine->_engine->objectTable[collisionIndex];

			actorTouchedPtr->COL_BY = g_engine->_engine->currentProcessedActorIdx;

			ZVStruct *touchedZv = &actorTouchedPtr->zv;

			if (actorTouchedPtr->_flags & AF_FOUNDABLE) // takable
			{
				// TODO: check if character isn't dead...
				if (g_engine->_engine->currentProcessedActorPtr->trackMode == 1 && ((g_engine->getGameId() == GID_AITD1 && g_engine->_engine->cVars[getCVarsIdx(DEAD_PERSO)] == 0)) /*|| (gameId >= JACK && defines.field_6 == 0))*/) {
					foundObject(actorTouchedPtr->indexInWorld, 0);
				}
			} else {
				// can be pushed ?
				if (actorTouchedPtr->_flags & AF_MOVABLE) {
					ZVStruct localZv2;

					bool isPushPossible = true;

					copyZv(touchedZv, &localZv2);

					localZv2.ZVX1 += stepX;
					localZv2.ZVX2 += stepX;

					localZv2.ZVZ1 += stepZ;
					localZv2.ZVZ2 += stepZ;

					if (!asmCheckListCol(&localZv2, &g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room])) {
						if (checkObjectCollisions(collisionIndex, &localZv2)) {
							isPushPossible = false;
						}
					} else {
						isPushPossible = false;
					}

					if (!isPushPossible) {
						// if we're trying to move
						if (stepX || stepZ) {
							if (actorTouchedPtr->room != g_engine->_engine->currentProcessedActorPtr->room) {
								ZVStruct localZv3;

								copyZv(touchedZv, &localZv3);

								getZvRelativePosition(&localZv3, actorTouchedPtr->room, g_engine->_engine->currentProcessedActorPtr->room);

								g_engine->_engine->hardColStepX = stepX;
								g_engine->_engine->hardColStepZ = stepZ;

								handleCollision(zvPtr, &zvLocal, &localZv3);

								stepX = g_engine->_engine->hardColStepX;
								stepZ = g_engine->_engine->hardColStepZ;
							} else {
								g_engine->_engine->hardColStepX = stepX;
								g_engine->_engine->hardColStepZ = stepZ;

								handleCollision(zvPtr, &zvLocal, touchedZv); // manage as hard collision

								stepX = g_engine->_engine->hardColStepX;
								stepZ = g_engine->_engine->hardColStepZ;
							}
						}
					} else {
						// push succeed
						if (actorTouchedPtr->_flags & AF_BOXIFY) {
							removeFromBGIncrust(collisionIndex);
						}

						actorTouchedPtr->_flags |= AF_ANIMATED;

						actorTouchedPtr->worldX += stepX; // apply push to object
						actorTouchedPtr->worldZ += stepZ;

						actorTouchedPtr->roomX += stepX;
						actorTouchedPtr->roomZ += stepZ;

						copyZv(&localZv2, touchedZv);
					}
				} else {
					// can't be pushed
					if (g_engine->_engine->currentProcessedActorPtr->dynFlags & 1) {
						// if moving
						if (stepX || stepZ) {
							if (actorTouchedPtr->room == g_engine->_engine->currentProcessedActorPtr->room) {
								// same room -> easy case
								g_engine->_engine->hardColStepX = stepX;
								g_engine->_engine->hardColStepZ = stepZ;

								handleCollision(zvPtr, &zvLocal, touchedZv); // manage as hard collision

								stepX = g_engine->_engine->hardColStepX;
								stepZ = g_engine->_engine->hardColStepZ;
							} else {
								// different room
								ZVStruct localZv3;

								copyZv(touchedZv, &localZv3);

								getZvRelativePosition(&localZv3, actorTouchedPtr->room, g_engine->_engine->currentProcessedActorPtr->room);

								g_engine->_engine->hardColStepX = stepX;
								g_engine->_engine->hardColStepZ = stepZ;

								handleCollision(zvPtr, &zvLocal, &localZv3); // manage as hard collision

								stepX = g_engine->_engine->hardColStepX;
								stepZ = g_engine->_engine->hardColStepZ;
							}
						}
					}
				}
			}
		} // end of actor/actor collision

		g_engine->_engine->currentProcessedActorPtr->stepX = stepX + oldStepX;
		g_engine->_engine->currentProcessedActorPtr->stepY = stepY + oldStepY;
		g_engine->_engine->currentProcessedActorPtr->stepZ = stepZ + oldStepZ;

		g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 += stepX;
		g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 += stepX;

		g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += stepY;
		g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += stepY;

		g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 += stepZ;
		g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 += stepZ;
	} // end of movement management

	if (!g_engine->_engine->currentProcessedActorPtr->YHandler.param) {
		// fall management ?
		g_engine->_engine->currentProcessedActorPtr->worldY += g_engine->_engine->currentProcessedActorPtr->stepY;
		g_engine->_engine->currentProcessedActorPtr->roomY += g_engine->_engine->currentProcessedActorPtr->stepY;

		g_engine->_engine->currentProcessedActorPtr->stepY = 0;

		if (g_engine->_engine->currentProcessedActorPtr->_flags & AF_FALLABLE) {
			zvPtr = &g_engine->_engine->currentProcessedActorPtr->zv;

			copyZv(zvPtr, &zvLocal);

			zvLocal.ZVY2 += 100;

			if (g_engine->_engine->currentProcessedActorPtr->roomY < -10 && !asmCheckListCol(&zvLocal, &g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room]) && !manageFall(g_engine->_engine->currentProcessedActorIdx, &zvLocal)) {
				initRealValue(0, 2000, 40, &g_engine->_engine->currentProcessedActorPtr->YHandler);
			} else {
				g_engine->_engine->currentProcessedActorPtr->falling = 0;
			}
		}
	} else {
		if (g_engine->_engine->currentProcessedActorPtr->YHandler.param != -1 && g_engine->_engine->currentProcessedActorPtr->_flags & AF_FALLABLE) {
			g_engine->_engine->currentProcessedActorPtr->falling = 1;
		}
	}

	for (int i = 0; i < 3; i++) {
		int collisionIndex = localTable[i];

		if (collisionIndex != -1) {
			Object *actorTouchedPtr = &g_engine->_engine->objectTable[collisionIndex];

			if (actorTouchedPtr->_flags & AF_MOVABLE) {
				int j;
				for (j = 0; j < 3; j++) {
					if (g_engine->_engine->currentProcessedActorPtr->COL[j] == collisionIndex)
						break;
				}

				if (j == 3) {
					actorTouchedPtr->_flags &= ~AF_ANIMATED;
					addActorToBgInscrust(collisionIndex);
				}
			}
		}
	}

	if (g_engine->_engine->currentProcessedActorPtr->END_FRAME) {
		// key frame change
		g_engine->_engine->currentProcessedActorPtr->FRAME++;

		if (g_engine->_engine->currentProcessedActorPtr->FRAME >= g_engine->_engine->currentProcessedActorPtr->numOfFrames) // end of anim ?
		{
			g_engine->_engine->currentProcessedActorPtr->END_ANIM = 1; // end of anim
			g_engine->_engine->currentProcessedActorPtr->FRAME = 0;    // restart anim

			if (!(g_engine->_engine->currentProcessedActorPtr->animType & 1) && g_engine->_engine->currentProcessedActorPtr->newAnim == -1) // is another anim waiting ?
			{
				g_engine->_engine->currentProcessedActorPtr->animType &= 0xFFFD;

				initAnim(g_engine->_engine->currentProcessedActorPtr->animInfo, 1, -1);
			}
		}
		g_engine->_engine->currentProcessedActorPtr->worldX += g_engine->_engine->currentProcessedActorPtr->stepX;
		g_engine->_engine->currentProcessedActorPtr->roomX += g_engine->_engine->currentProcessedActorPtr->stepX;

		g_engine->_engine->currentProcessedActorPtr->worldZ += g_engine->_engine->currentProcessedActorPtr->stepZ;
		g_engine->_engine->currentProcessedActorPtr->roomZ += g_engine->_engine->currentProcessedActorPtr->stepZ;

		g_engine->_engine->currentProcessedActorPtr->stepX = 0;
		g_engine->_engine->currentProcessedActorPtr->stepZ = 0;

		g_engine->_engine->currentProcessedActorPtr->animNegX = 0;
		g_engine->_engine->currentProcessedActorPtr->animNegY = 0;
		g_engine->_engine->currentProcessedActorPtr->animNegZ = 0;
	} else {
		// not the end of anim
		if (g_engine->_engine->currentProcessedActorPtr->ANIM == -1 && g_engine->_engine->currentProcessedActorPtr->speed != 0 && g_engine->_engine->currentProcessedActorPtr->speedChange.param == 0) {
			g_engine->_engine->currentProcessedActorPtr->worldX += g_engine->_engine->currentProcessedActorPtr->stepX;
			g_engine->_engine->currentProcessedActorPtr->roomX += g_engine->_engine->currentProcessedActorPtr->stepX;

			g_engine->_engine->currentProcessedActorPtr->worldZ += g_engine->_engine->currentProcessedActorPtr->stepZ;
			g_engine->_engine->currentProcessedActorPtr->roomZ += g_engine->_engine->currentProcessedActorPtr->stepZ;

			g_engine->_engine->currentProcessedActorPtr->stepX = 0;
			g_engine->_engine->currentProcessedActorPtr->stepZ = 0;

			initRealValue(0, g_engine->_engine->currentProcessedActorPtr->speed, 60, &g_engine->_engine->currentProcessedActorPtr->speedChange);
		}

		g_engine->_engine->currentProcessedActorPtr->END_ANIM = 0;
	}
}

static void initBufferAnim(int16 *buffer, byte *bodyPtr) {
	int16 *bufferIt = buffer;

	const int flag = *(int16 *)bodyPtr;
	if (flag & 2) {
		byte *source = bodyPtr + 0x10;

		*(uint16 *)(source + 4) = static_cast<uint16>(g_engine->_engine->timer);

		g_engine->_engine->bodyBufferMap[source] = (byte *)&buffer[0];

		source += *(int16 *)(source - 2);

		int16 ax = *(int16 *)source;

		ax = (ax * 2 + ax) * 2 + 2;

		source += ax;

		const int cx = *(int16 *)source;

		source += cx * 2;

		bufferIt += 4;
		source += 10;

		for (int i = 0; i < cx; i++) {
			bufferIt[0] = *(int16 *)source;
			bufferIt[1] = *(int16 *)(source + 2);
			bufferIt[2] = *(int16 *)(source + 4);
			bufferIt[3] = *(int16 *)(source + 6);

			bufferIt += 4;
			source += 8;

			if (flag & INFO_OPTIMISE) {
				bufferIt[0] = *(int16 *)source;
				bufferIt[1] = *(int16 *)(source + 2);
				bufferIt[2] = *(int16 *)(source + 4);
				bufferIt[3] = *(int16 *)(source + 6);

				bufferIt += 4;
				source += 8;
			}

			source += 8;
		}
	}
}

int16 getNbFramesAnim(byte *animPtr) {
	return *(int16 *)animPtr;
}

static int16 patchType(byte **bodyPtr) // local
{
	const int16 temp = *(int16 *)g_engine->_engine->animVar1;

	g_engine->_engine->animVar1 += 2;

	g_engine->_engine->animVar4 += 2;

	*(int16 *)*bodyPtr = temp;
	*bodyPtr += 2;

	return temp;
}

static void patchInterAngle(byte **bodyPtr, int bp, int bx) // local
{
	int16 oldRotation = *(int16 *)g_engine->_engine->animVar4;

	g_engine->_engine->animVar4 += 2;

	int16 newRotation = *(int16 *)g_engine->_engine->animVar1;
	g_engine->_engine->animVar1 += 2;

	const int16 diff = newRotation - oldRotation;

	if (diff == 0) {
		*(int16 *)*bodyPtr = newRotation;
	} else {
		if (diff <= 0x200) {
			if (diff >= -0x200) {
				*(int16 *)*bodyPtr = diff * bp / bx + oldRotation;
			} else {
				newRotation += 0x400;
				newRotation -= oldRotation;

				*(int16 *)*bodyPtr = newRotation * bp / bx + oldRotation;
			}
		} else {
			oldRotation += 0x400;
			newRotation -= oldRotation;

			*(int16 *)*bodyPtr = newRotation * bp / bx + oldRotation;
		}
	}

	*bodyPtr += 2;
}

static void patchInterStep(byte **bodyPtr, int bp, int bx) // local
{
	const int16 cx = *(int16 *)g_engine->_engine->animVar4;
	g_engine->_engine->animVar4 += 2;

	const int16 ax = *(int16 *)g_engine->_engine->animVar1;
	g_engine->_engine->animVar1 += 2;

	if (ax == cx) {
		*(int16 *)*bodyPtr = ax;
	} else {
		*(int16 *)*bodyPtr = (ax - cx) * bp / bx + cx;
	}

	*bodyPtr += 2;
}

uint16 getFlags(const Body *body) { return READ_LE_U16(body->m_raw); }

uint16 getVerticesSize(const Body *body) {
	uint16 scratchBufferSize = READ_LE_U16(body->m_raw + 14);
	uint16 numVertices = READ_LE_U16(body->m_raw + 16 + scratchBufferSize);
	return numVertices;
}

uint16 getGroupOrderSize(const Body *body) {
	uint16 groupOrderSize = 0;
	uint16 flags = getFlags(body);

	if (flags & INFO_ANIM) {
		uint8 *ptr = body->m_raw + 14;
		uint16 scratchBufferSize = READ_LE_U16(ptr);
		ptr += 2 + scratchBufferSize;
		uint16 numVertices = READ_LE_U16(ptr);
		ptr += 2 + (numVertices * 6);
		groupOrderSize = READ_LE_U16(ptr);
	}
	return groupOrderSize;
}

void getGroupOrders(const Body *body, Common::Array<uint16> &dst) {
	uint16 flags = getFlags(body);

	if (flags & INFO_ANIM) {
		uint16 scratchBufferSize = READ_LE_U16(body->m_raw + 14);
		uint16 numVertices = READ_LE_U16(body->m_raw + 16 + scratchBufferSize);
		uint16 numGroupOrders = READ_LE_U16(body->m_raw + 18 + scratchBufferSize + (numVertices * 6));
		uint16 *pGroupOrders = (uint16 *)(body->m_raw + 20 + scratchBufferSize + (numVertices * 6));
		dst.resize(numGroupOrders);
		uint16 groupOrderSize = (flags & INFO_OPTIMISE) ? 24 : 16;
		for (uint16 i = 0; i < numGroupOrders; i++) {
			dst[i] = READ_LE_U16(pGroupOrders) / groupOrderSize;
			pGroupOrders++;
		}
	}
}

void getGroups(const Body *body, Common::Array<Group> &dst) {
	uint16 flags = getFlags(body);

	if (flags & INFO_ANIM) {
		uint8 *ptr = body->m_raw + 14;
		uint16 scratchBufferSize = READ_LE_U16(ptr);
		ptr += 2 + scratchBufferSize;
		uint16 numVertices = READ_LE_U16(ptr);
		ptr += 2 + (numVertices * 6);
		uint16 numGroups = READ_LE_U16(ptr);
		dst.resize(numGroups);
		uint8 *pGroup = (uint8 *)(ptr + 2);
		if (flags & INFO_OPTIMISE) {
			// AITD2+
			for (int i = 0; i < numGroups; i++) {
				dst[i].m_start = READ_LE_S16(pGroup) / 6;
				pGroup += 2;
				dst[i].m_numVertices = READ_LE_S16(pGroup);
				pGroup += 2;
				dst[i].m_baseVertices = READ_LE_S16(pGroup) / 6;
				pGroup += 2;
				dst[i].m_orgGroup = READ_LE_S8(pGroup);
				pGroup += 1;
				dst[i].m_numGroup = READ_LE_S8(pGroup);
				pGroup += 1;
				dst[i].m_state.m_type = READ_LE_S16(pGroup);
				pGroup += 2;
				dst[i].m_state.m_delta[0] = READ_LE_S16(pGroup);
				pGroup += 2;
				dst[i].m_state.m_delta[1] = READ_LE_S16(pGroup);
				pGroup += 2;
				dst[i].m_state.m_delta[2] = READ_LE_S16(pGroup);
				pGroup += 2;
				dst[i].m_state.m_rotateDelta[0] = READ_LE_S16(pGroup);
				pGroup += 2;
				dst[i].m_state.m_rotateDelta[1] = READ_LE_S16(pGroup);
				pGroup += 2;
				dst[i].m_state.m_rotateDelta[2] = READ_LE_S16(pGroup);
				pGroup += 2;
				pGroup += 2; // padding?
			}
		} else {
			pGroup += numGroups * 2;
			for (int i = 0; i < numGroups; i++) {
				dst[i].m_start = READ_LE_S16(pGroup) / 6;
				pGroup += 2;
				dst[i].m_numVertices = READ_LE_S16(pGroup);
				pGroup += 2;
				dst[i].m_baseVertices = READ_LE_S16(pGroup) / 6;
				pGroup += 2;
				dst[i].m_orgGroup = READ_LE_S8(pGroup);
				pGroup += 1;
				dst[i].m_numGroup = READ_LE_S8(pGroup);
				pGroup += 1;
				dst[i].m_state.m_type = READ_LE_S16(pGroup);
				pGroup += 2;
				dst[i].m_state.m_delta[0] = READ_LE_S16(pGroup);
				pGroup += 2;
				dst[i].m_state.m_delta[1] = READ_LE_S16(pGroup);
				pGroup += 2;
				dst[i].m_state.m_delta[2] = READ_LE_S16(pGroup);
				pGroup += 2;
			}
		}
	}
}

void copyVertices(const Body *body, int16 *dst) {
	uint8 *ptr = body->m_raw + 14;
	uint16 scratchBufferSize = READ_LE_U16(ptr);
	ptr += 2 + scratchBufferSize;
	uint16 numVertices = READ_LE_U16(ptr);
	int16 *pVerts = (int16 *)(ptr + 2);
	assert(numVertices < NUM_MAX_POINT_IN_POINT_BUFFER);
	for (uint16 i = 0; i < numVertices; i++) {
		*dst = READ_LE_S16(pVerts);
		dst++;
		pVerts++;
		*dst = READ_LE_S16(pVerts);
		dst++;
		pVerts++;
		*dst = READ_LE_S16(pVerts);
		dst++;
		pVerts++;
	}
}

void getVertices(const Body *body, Common::Array<Point3d> &dst) {
	uint size = getVerticesSize(body);
	if (size == 0) {
		dst.clear();
		return;
	}
	dst.resize(size);
	copyVertices(body, (int16 *)&dst[0]);
}

void getPrimitives(const Body *body, Common::Array<Primitive> &dst) {
	uint16 scratchBufferSize = READ_LE_U16(body->m_raw + 14);
	uint16 numVertices = READ_LE_U16(body->m_raw + 16 + scratchBufferSize);
	uint8 *pPrimitive = body->m_raw + 18 + scratchBufferSize + (numVertices * 6);
	uint16 flags = getFlags(body);
	if (flags & INFO_ANIM) {
		uint16 numGroups = READ_LE_U16(pPrimitive);
		if (flags & INFO_OPTIMISE) {
			// AITD2+
			pPrimitive += 2 + numGroups * 2 + numGroups * 24;
		} else {
			pPrimitive += 2 + numGroups * 2 + numGroups * 16;
		}
	}
	uint16 numPrimitives = READ_LE_U16(pPrimitive);
	pPrimitive += 2;
	dst.resize(numPrimitives);
	for (uint16 i = 0; i < numPrimitives; i++) {
		dst[i].m_type = static_cast<PrimType>(READ_LE_U8(pPrimitive));
		pPrimitive += 1;
		switch (dst[i].m_type) {
		case primTypeEnum_Line:
			dst[i].m_material = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			dst[i].m_color = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			dst[i].m_even = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			dst[i].m_points.resize(2);
			for (uint j = 0; j < dst[i].m_points.size(); j++) {
				dst[i].m_points[j] = READ_LE_U16(pPrimitive) / 6;
				pPrimitive += 2;
			}
			break;
		case primTypeEnum_Poly:
			dst[i].m_points.resize(READ_LE_U8(pPrimitive));
			pPrimitive += 1;
			dst[i].m_material = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			dst[i].m_color = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			for (uint j = 0; j < dst[i].m_points.size(); j++) {
				dst[i].m_points[j] = READ_LE_U16(pPrimitive) / 6;
				pPrimitive += 2;
			}
			break;
		case primTypeEnum_Point:
		case primTypeEnum_BigPoint:
		case primTypeEnum_Zixel:
			dst[i].m_material = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			dst[i].m_color = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			dst[i].m_even = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			dst[i].m_points.resize(1);
			for (uint j = 0; j < dst[i].m_points.size(); j++) {
				dst[i].m_points[j] = READ_LE_U16(pPrimitive) / 6;
				pPrimitive += 2;
			}
			break;
		case primTypeEnum_Sphere:
			dst[i].m_material = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			dst[i].m_color = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			dst[i].m_even = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			dst[i].m_size = READ_LE_U16(pPrimitive);
			pPrimitive += 2;
			dst[i].m_points.resize(1);
			for (uint j = 0; j < dst[i].m_points.size(); j++) {
				dst[i].m_points[j] = READ_LE_U16(pPrimitive) / 6;
				pPrimitive += 2;
			}
			break;
		case processPrim_PolyTexture8:
			dst[i].m_points.resize(READ_LE_U8(pPrimitive));
			pPrimitive += 1;
			dst[i].m_material = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			dst[i].m_color = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			for (uint j = 0; j < dst[i].m_points.size(); j++) {
				dst[i].m_points[j] = READ_LE_U16(pPrimitive) / 6;
				pPrimitive += 2;
			}
			break;
		case processPrim_PolyTexture9:
		case processPrim_PolyTexture10:
			dst[i].m_points.resize(READ_LE_U8(pPrimitive));
			pPrimitive += 1;
			dst[i].m_material = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			dst[i].m_color = READ_LE_U8(pPrimitive);
			pPrimitive += 1;
			for (uint j = 0; j < dst[i].m_points.size(); j++) {
				dst[i].m_points[j] = READ_LE_U16(pPrimitive) / 6;
				pPrimitive += 2;
			}
			// load UVS?
			for (uint j = 0; j < dst[i].m_points.size(); j++) {
				READ_LE_U8(pPrimitive);
				pPrimitive += 1;
				READ_LE_U8(pPrimitive);
				pPrimitive += 1;
			}
			break;
		default:
			assert(0);
		}
	}
}

int16 setInterAnimObjet(int frame, byte *animPtr, byte *bodyPtr) {
	int numOfBonesInAnim = *(int16 *)(animPtr + 2);

	const Body body = getBodyFromPtr(bodyPtr);

	const int flag = getFlags(&body);

	animPtr += 4;

	if (flag & INFO_OPTIMISE) {
		animPtr += ((numOfBonesInAnim << 4) + 8) * frame; // seek to keyframe
	} else {
		animPtr += (numOfBonesInAnim + 1) * 8 * frame; // seek to keyframe
	}

	// g_engine->_engine->animVar1 = ptr to the current keyFrame
	g_engine->_engine->animVar1 = animPtr;

	const uint16 keyframeLength = *(uint16 *)animPtr; // keyframe length

	if (!(getFlags(&body) & INFO_ANIM)) // do not anim if the model can't be animated
	{
		return 0;
	}

	bodyPtr += 16; // skip the flags, ZV, scratch buffer size

	g_engine->_engine->animVar3 = bodyPtr; // this is the scratch buffer

	const uint16 timeOfKeyframeStart = *(uint16 *)(bodyPtr + 4); // time of start of keyframe

	byte *animBufferPtr = g_engine->_engine->bodyBufferMap[bodyPtr];

	if (!animBufferPtr) {
		animBufferPtr = g_engine->_engine->animVar1;
	}

	// g_engine->_engine->animVar4 = ptr to previous key frame
	g_engine->_engine->animVar4 = animBufferPtr;

	bodyPtr += *(int16 *)(bodyPtr - 2); // skip over scratch buffer

	int ax = *(int16 *)bodyPtr; // num vertices
	ax = ax * 6 + 2;
	bodyPtr += ax; // skip the vertices

	ax = *(int16 *)bodyPtr; // num of group order
	uint16 bx = ax;
	bodyPtr += bx * 2; // skip group order table

	uint16 groupOrderSize = getGroupOrderSize(&body);
	if (static_cast<uint>(numOfBonesInAnim) > groupOrderSize) {
		numOfBonesInAnim = groupOrderSize;
	}

	bodyPtr += 10; // skip bone 0

	const uint16 time = static_cast<uint16>(g_engine->_engine->timer) - timeOfKeyframeStart;

	bx = keyframeLength;
	const int bp = time;

	if (time < keyframeLength) // interpolate keyframe
	{
		byte *animVar1Backup = g_engine->_engine->animVar1;
		// skip bone 0 anim
		g_engine->_engine->animVar4 += 8; // anim buffer
		g_engine->_engine->animVar1 += 8; // current keyframe ptr

		if (!(flag & INFO_OPTIMISE)) {
			do {
				switch (patchType(&bodyPtr)) {
				case 0: // rotate
					patchInterAngle(&bodyPtr, bp, bx);
					patchInterAngle(&bodyPtr, bp, bx);
					patchInterAngle(&bodyPtr, bp, bx);
					break;
				case 1: // translate
				case 2: // zoom
					patchInterStep(&bodyPtr, bp, bx);
					patchInterStep(&bodyPtr, bp, bx);
					patchInterStep(&bodyPtr, bp, bx);
					break;
				}

				bodyPtr += 8;
			} while (--numOfBonesInAnim);
		} else {
			do {
				switch (patchType(&bodyPtr)) {
				case 0: {
					g_engine->_engine->animVar4 += 6;
					g_engine->_engine->animVar1 += 6;
					bodyPtr += 6;
					break;
				}
				case 1:
				case 2: {
					patchInterStep(&bodyPtr, bp, bx);
					patchInterStep(&bodyPtr, bp, bx);
					patchInterStep(&bodyPtr, bp, bx);
					break;
				}
				}

				patchInterAngle(&bodyPtr, bp, bx);
				patchInterAngle(&bodyPtr, bp, bx);
				patchInterAngle(&bodyPtr, bp, bx);

				g_engine->_engine->animVar4 += 2;
				g_engine->_engine->animVar1 += 2;
				bodyPtr += 10;

			} while (--numOfBonesInAnim);
		}

		g_engine->_engine->animVar1 = animVar1Backup;

		g_engine->_engine->animVar1 += 2;

		g_engine->_engine->animStepX = *(int16 *)g_engine->_engine->animVar1 * bp / bx;       // X
		g_engine->_engine->animStepY = *(int16 *)(g_engine->_engine->animVar1 + 2) * bp / bx; // Y
		g_engine->_engine->animStepZ = *(int16 *)(g_engine->_engine->animVar1 + 4) * bp / bx; // Z

		g_engine->_engine->animVar1 += 6;

		g_engine->_engine->animCurrentTime = bx;
		g_engine->_engine->animKeyframeLength = bp;
		return 0;
	} else // change keyframe
	{
		byte *tempBx = g_engine->_engine->animVar1;
		byte *si = g_engine->_engine->animVar1;

		si += 8;

		do {
			*(int16 *)bodyPtr = *(int16 *)si;
			*(int16 *)(bodyPtr + 2) = *(int16 *)(si + 2);
			*(int16 *)(bodyPtr + 4) = *(int16 *)(si + 4);
			*(int16 *)(bodyPtr + 6) = *(int16 *)(si + 6);

			bodyPtr += 8;
			si += 8;

			if (flag & INFO_OPTIMISE) {
				*(int16 *)bodyPtr = *(int16 *)si;
				*(int16 *)(bodyPtr + 2) = *(int16 *)(si + 2);
				*(int16 *)(bodyPtr + 4) = *(int16 *)(si + 4);
				*(int16 *)(bodyPtr + 6) = *(int16 *)(si + 6);
				bodyPtr += 8;
				si += 8;
			}

			bodyPtr += 8;

		} while (--numOfBonesInAnim);

		g_engine->_engine->bodyBufferMap[g_engine->_engine->animVar3] = g_engine->_engine->animVar1;
		//*(char**)g_engine->_engine->animVar3 = g_engine->_engine->animVar1;

		*(uint16 *)(g_engine->_engine->animVar3 + 4) = static_cast<uint16>(g_engine->_engine->timer);

		tempBx += 2;

		g_engine->_engine->animCurrentTime = bx;
		g_engine->_engine->animKeyframeLength = bx;

		g_engine->_engine->animStepX = *(int16 *)tempBx;
		g_engine->_engine->animStepY = *(int16 *)(tempBx + 2);
		g_engine->_engine->animStepZ = *(int16 *)(tempBx + 4);

		tempBx += 6;
		return 1;
	}
}
} // namespace Fitd
