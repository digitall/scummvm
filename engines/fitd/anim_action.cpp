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

#include "common/debug.h"
#include "fitd/anim_action.h"
#include "fitd/common.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/hqr.h"
#include "fitd/life.h"
#include "fitd/room.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"
#include "fitd/zv.h"

namespace Fitd {

#define NO_FRAPPE 0
#define WAIT_FRAPPE_ANIM 1
#define FRAPPE_OK 2
#define DONE_FRAPPE 3
#define WAIT_TIR_ANIM 4
#define DO_TIR 5
#define WAIT_ANIM_THROW 6
#define WAIT_FRAME_THROW 7
#define HIT_OBJECT 8
#define THROW_OBJECT 9
#define WAIT_FRAPPE_FRAME 10

static int getCVarsIdx(enumCVars searchedType) {
	// TODO: optimize by reversing the table....
	for (int i = 0; i < g_engine->_engine->cVarsSize; i++) {
		if (currentCVarTable[i] == -1) {
			assert(0);
		}

		if (currentCVarTable[i] == searchedType)
			return i;
	}

	assert(0);
	return 0;
}

int getCVarsIdx(int searchedType) {
	return getCVarsIdx(static_cast<enumCVars>(searchedType));
}

static SceZone *processActor2Sub(int x, int y, int z, RoomData *pRoomData) {

	SceZone *pCurrentZone = pRoomData->sceZoneTable.data();

	for (uint32 i = 0; i < pRoomData->numSceZone; i++) {
		if (pCurrentZone->zv.ZVX1 <= x && pCurrentZone->zv.ZVX2 >= x) {
			if (pCurrentZone->zv.ZVY1 <= y && pCurrentZone->zv.ZVY2 >= y) {
				if (pCurrentZone->zv.ZVZ1 <= z && pCurrentZone->zv.ZVZ2 >= z) {
					return pCurrentZone;
				}
			}
		}

		pCurrentZone++;
	}

	return nullptr;
}

void gereFrappe() {
	switch (g_engine->_engine->currentProcessedActorPtr->animActionType) {
	case WAIT_FRAPPE_ANIM:
	case WAIT_FRAPPE_FRAME:
		if (g_engine->_engine->currentProcessedActorPtr->animActionType == WAIT_FRAPPE_ANIM && g_engine->_engine->currentProcessedActorPtr->ANIM == g_engine->_engine->currentProcessedActorPtr->animActionANIM) {
			g_engine->_engine->currentProcessedActorPtr->animActionType = WAIT_FRAPPE_FRAME;
		}

		if (g_engine->_engine->currentProcessedActorPtr->ANIM != g_engine->_engine->currentProcessedActorPtr->animActionANIM) {
			g_engine->_engine->currentProcessedActorPtr->animActionType = NO_FRAPPE;
			return;
		}

		if (g_engine->_engine->currentProcessedActorPtr->FRAME == g_engine->_engine->currentProcessedActorPtr->animActionFRAME) {
			g_engine->_engine->currentProcessedActorPtr->animActionType = FRAPPE_OK;
		}
		return;

	case FRAPPE_OK: {
		if (g_engine->_engine->currentProcessedActorPtr->ANIM != g_engine->_engine->currentProcessedActorPtr->animActionANIM) {
			g_engine->_engine->currentProcessedActorPtr->animActionType = 0;
		}

		int x = g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->hotPoint.x + g_engine->_engine->currentProcessedActorPtr->stepX;
		int y = g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->hotPoint.y + g_engine->_engine->currentProcessedActorPtr->stepY;
		int z = g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->hotPoint.z + g_engine->_engine->currentProcessedActorPtr->stepZ;

		int range = g_engine->_engine->currentProcessedActorPtr->animActionParam;

		ZVStruct rangeZv;
		rangeZv.ZVX1 = x - range;
		rangeZv.ZVX2 = x + range;
		rangeZv.ZVY1 = y - range;
		rangeZv.ZVY2 = y + range;
		rangeZv.ZVZ1 = z - range;
		rangeZv.ZVZ2 = z + range;

		// drawProjectedBox(rangeZv.ZVX1,rangeZv.ZVX2,rangeZv.ZVY1,rangeZv.ZVY2,rangeZv.ZVZ1,rangeZv.ZVZ2,60,255);

		int collision = checkObjectCollisions(g_engine->_engine->currentProcessedActorIdx, &rangeZv);

		for (int i = 0; i < collision; i++) {
			Object *actorPtr2;

			g_engine->_engine->currentProcessedActorPtr->HIT = g_engine->_engine->currentProcessedActorPtr->COL[i];
			actorPtr2 = &g_engine->_engine->objectTable[g_engine->_engine->currentProcessedActorPtr->COL[i]];

			actorPtr2->HIT_BY = g_engine->_engine->currentProcessedActorIdx;
			actorPtr2->hitForce = g_engine->_engine->currentProcessedActorPtr->hitForce;

			if (actorPtr2->_flags & AF_ANIMATED) {
				g_engine->_engine->currentProcessedActorPtr->animActionType = 0;
				return;
			}
		}
		break;
	}
	case 4: // WAIT_TIR_ANIM
	{
		if (g_engine->_engine->currentProcessedActorPtr->ANIM != g_engine->_engine->currentProcessedActorPtr->animActionANIM)
			return;

		if (g_engine->_engine->currentProcessedActorPtr->FRAME != g_engine->_engine->currentProcessedActorPtr->animActionFRAME)
			return;

		g_engine->_engine->currentProcessedActorPtr->animActionType = 5;

		break;
	}
	case 5: // DO_TIR
	{
		int touchedActor;

		initSpecialObjet(3,
						 g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->hotPoint.x,
						 g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->hotPoint.y,
						 g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->hotPoint.z,
						 g_engine->_engine->currentProcessedActorPtr->stage,
						 g_engine->_engine->currentProcessedActorPtr->room,
						 0,
						 g_engine->_engine->currentProcessedActorPtr->beta,
						 0,
						 &g_engine->_engine->currentProcessedActorPtr->zv);

		touchedActor = checkLineProjectionWithActors(
			g_engine->_engine->currentProcessedActorIdx,
			g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->hotPoint.x,
			g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->hotPoint.y,
			g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->hotPoint.z,
			g_engine->_engine->currentProcessedActorPtr->beta - 0x100,
			g_engine->_engine->currentProcessedActorPtr->room,
			g_engine->_engine->currentProcessedActorPtr->animActionParam);

		if (touchedActor == -1) // no one has been touched
		{
			initSpecialObjet(2, g_engine->_engine->animMoveX, g_engine->_engine->animMoveY, g_engine->_engine->animMoveZ, g_engine->_engine->currentProcessedActorPtr->stage, g_engine->_engine->currentProcessedActorPtr->room, 0, -g_engine->_engine->currentProcessedActorPtr->beta, 0, &g_engine->_engine->currentProcessedActorPtr->zv);

			g_engine->_engine->currentProcessedActorPtr->animActionType = 0;
		} else {
			initSpecialObjet(2, g_engine->_engine->animMoveX, g_engine->_engine->animMoveY, g_engine->_engine->animMoveZ, g_engine->_engine->currentProcessedActorPtr->stage, g_engine->_engine->currentProcessedActorPtr->room, 0, -g_engine->_engine->currentProcessedActorPtr->beta, 0, &g_engine->_engine->currentProcessedActorPtr->zv);

			g_engine->_engine->currentProcessedActorPtr->hotPoint.x = g_engine->_engine->animMoveX - g_engine->_engine->currentProcessedActorPtr->roomX;
			g_engine->_engine->currentProcessedActorPtr->hotPoint.y = g_engine->_engine->animMoveY - g_engine->_engine->currentProcessedActorPtr->roomY;
			g_engine->_engine->currentProcessedActorPtr->hotPoint.z = g_engine->_engine->animMoveZ - g_engine->_engine->currentProcessedActorPtr->roomZ;

			g_engine->_engine->currentProcessedActorPtr->HIT = touchedActor;

			g_engine->_engine->objectTable[touchedActor].HIT_BY = g_engine->_engine->currentProcessedActorIdx;
			g_engine->_engine->objectTable[touchedActor].hitForce = g_engine->_engine->currentProcessedActorPtr->hitForce;

			g_engine->_engine->currentProcessedActorPtr->animActionType = 0;
		}
		break;
	}
	case 6: // WAIT_ANIM_THROW
	{
		if (g_engine->_engine->currentProcessedActorPtr->ANIM == g_engine->_engine->currentProcessedActorPtr->animActionANIM) {
			int objIdx = g_engine->_engine->currentProcessedActorPtr->animActionParam;

			WorldObject *objPtr = &g_engine->_engine->worldObjets[objIdx];

			int x = g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->hotPoint.x + g_engine->_engine->currentProcessedActorPtr->stepX;
			int y = g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->hotPoint.y + g_engine->_engine->currentProcessedActorPtr->stepY;
			int z = g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->hotPoint.z + g_engine->_engine->currentProcessedActorPtr->stepZ;

			ZVStruct rangeZv;

			giveZVObjet(hqrGet(g_engine->_engine->listBody, objPtr->body).data, &rangeZv);

			rangeZv.ZVX1 += x;
			rangeZv.ZVX2 += x;
			rangeZv.ZVY1 += y;
			rangeZv.ZVY2 += y;
			rangeZv.ZVZ1 += z;
			rangeZv.ZVZ2 += z;

			if (asmCheckListCol(&rangeZv, &g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room])) {
				g_engine->_engine->currentProcessedActorPtr->animActionType = 0;
				putAtObjet(objIdx, g_engine->_engine->currentProcessedActorPtr->indexInWorld);
			} else {
				if (g_engine->_engine->currentProcessedActorPtr->FRAME == g_engine->_engine->currentProcessedActorPtr->animActionFRAME) {
					g_engine->_engine->currentProcessedActorPtr->animActionType = 7;

					x = g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->hotPoint.x + g_engine->_engine->currentProcessedActorPtr->stepX;
					y = g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->hotPoint.y + g_engine->_engine->currentProcessedActorPtr->stepY;
					z = g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->hotPoint.z + g_engine->_engine->currentProcessedActorPtr->stepZ;

					deleteInventoryObjet(objIdx);

					objPtr->x = x;
					objPtr->y = y;
					objPtr->z = z;

					objPtr->room = g_engine->_engine->currentProcessedActorPtr->room;
					objPtr->stage = g_engine->_engine->currentProcessedActorPtr->stage;
					objPtr->alpha = g_engine->_engine->currentProcessedActorPtr->alpha;
					objPtr->beta = g_engine->_engine->currentProcessedActorPtr->beta + 0x200;

					objPtr->flags2 &= 0xBFFF;
					objPtr->flags |= 0x85;
					objPtr->flags &= 0xFFDF;

					// FlagGenereActiveList = 1;
				}
			}
		}
		break;
	}
	case 7: // THROW
	{
		int objIdx;
		int actorIdx;
		Object *actorPtr;

		g_engine->_engine->currentProcessedActorPtr->animActionType = 0;

		int x = g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->hotPoint.x + g_engine->_engine->currentProcessedActorPtr->stepX;
		int y = g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->hotPoint.y + g_engine->_engine->currentProcessedActorPtr->stepY;
		int z = g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->hotPoint.z + g_engine->_engine->currentProcessedActorPtr->stepZ;

		objIdx = g_engine->_engine->currentProcessedActorPtr->animActionParam;

		actorIdx = g_engine->_engine->worldObjets[objIdx].objIndex;

		if (actorIdx == -1)
			return;

		actorPtr = &g_engine->_engine->objectTable[actorIdx];

		actorPtr->roomX = x;
		actorPtr->roomY = y;
		actorPtr->roomZ = z;

		giveZVObjet(hqrGet(g_engine->_engine->listBody, actorPtr->bodyNum).data, &actorPtr->zv);

		actorPtr->zv.ZVX1 += x;
		actorPtr->zv.ZVX2 += x;
		actorPtr->zv.ZVY1 += y;
		actorPtr->zv.ZVY2 += y;
		actorPtr->zv.ZVZ1 += z;
		actorPtr->zv.ZVZ2 += z;

		actorPtr->_flags |= AF_ANIMATED;
		actorPtr->_flags &= ~AF_BOXIFY;

		g_engine->_engine->worldObjets[objIdx].x = x;
		g_engine->_engine->worldObjets[objIdx].y = y;
		g_engine->_engine->worldObjets[objIdx].z = z;

		g_engine->_engine->worldObjets[objIdx].alpha = g_engine->_engine->currentProcessedActorPtr->indexInWorld; // original thrower

		actorPtr->dynFlags = 0;
		actorPtr->animActionType = 9;
		actorPtr->animActionParam = 100;
		actorPtr->hitForce = g_engine->_engine->currentProcessedActorPtr->hitForce;
		actorPtr->hotPointID = -1;
		actorPtr->speed = 3000;

		initRealValue(0, actorPtr->speed, 60, &actorPtr->speedChange);

		break;
	}
	case 8: // HIT_OBJ
	{
		ZVStruct zv;
		copyZv(&g_engine->_engine->currentProcessedActorPtr->zv, &zv);
		zv.ZVX1 -= 10;
		zv.ZVX2 += 10;
		zv.ZVY1 -= 10;
		zv.ZVY2 += 10;
		zv.ZVZ1 -= 10;
		zv.ZVZ2 += 10;
		const int numCol = checkObjectCollisions(g_engine->_engine->currentProcessedActorIdx, &zv);
		if (numCol) {
			for (int i = 0; i < numCol; ++i) {
				int hitObjIndex = g_engine->_engine->currentProcessedActorPtr->COL[i];
				g_engine->_engine->currentProcessedActorPtr->hotPoint.x = 0;
				g_engine->_engine->currentProcessedActorPtr->hotPoint.y = 0;
				g_engine->_engine->currentProcessedActorPtr->hotPoint.z = 0;
				g_engine->_engine->currentProcessedActorPtr->HIT = hitObjIndex;
				Object *hitObj = &g_engine->_engine->objectTable[hitObjIndex];
				hitObj->HIT_BY = g_engine->_engine->currentProcessedActorIdx;
				hitObj->hitForce = g_engine->_engine->currentProcessedActorPtr->hitForce;
			}
		}
		break;
	}
	case 9: // during throw
	{
		WorldObject *objPtr = &g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld];

		ZVStruct rangeZv;
		ZVStruct rangeZv2;
		int xtemp;
		int ytemp;
		int ztemp;
		int x1;
		int x2;
		int x3;
		int y1;
		int y2;
		int z1;
		int z2;
		int z3;
		int step;

		copyZv(&g_engine->_engine->currentProcessedActorPtr->zv, &rangeZv);
		copyZv(&g_engine->_engine->currentProcessedActorPtr->zv, &rangeZv2);

		xtemp = g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX;
		ytemp = g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY;
		ztemp = g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ;

		rangeZv2.ZVX1 -= xtemp;
		rangeZv2.ZVX2 -= xtemp;
		rangeZv2.ZVY1 -= ytemp;
		rangeZv2.ZVY2 -= ytemp;
		rangeZv2.ZVZ1 -= ztemp;
		rangeZv2.ZVZ2 -= ztemp;

		x1 = objPtr->x;
		x2 = objPtr->x;
		x3 = objPtr->x;

		y1 = objPtr->y;
		y2 = objPtr->y;

		z1 = objPtr->z;
		z2 = objPtr->z;
		z3 = objPtr->z;

		step = 0;

		g_engine->_engine->animMoveZ = 0;
		g_engine->_engine->animMoveX = 0;

		do {
			int collision;
			SceZone *ptr;

			walkStep(0, -step, g_engine->_engine->currentProcessedActorPtr->beta);
			step += 100;
			x2 = x1 + g_engine->_engine->animMoveX;
			y2 = y1;
			z2 = z1 + g_engine->_engine->animMoveZ;

			copyZv(&rangeZv2, &rangeZv);

			rangeZv.ZVX1 = x2 - 200;
			rangeZv.ZVX2 = x2 + 200;
			rangeZv.ZVY1 = y2 - 200;
			rangeZv.ZVY2 = y2 + 200;
			rangeZv.ZVZ1 = z2 - 200;
			rangeZv.ZVZ2 = z2 + 200;

			collision = checkObjectCollisions(g_engine->_engine->currentProcessedActorIdx, &rangeZv);

			if (collision) {
				int collision2 = collision;
				int i;

				g_engine->_engine->currentProcessedActorPtr->hotPoint.x = 0;
				g_engine->_engine->currentProcessedActorPtr->hotPoint.y = 0;
				g_engine->_engine->currentProcessedActorPtr->hotPoint.z = 0;

				for (i = 0; i < collision; i++) {
					int currentActorCol = g_engine->_engine->currentProcessedActorPtr->COL[i];

					if (g_engine->_engine->objectTable[currentActorCol].indexInWorld == objPtr->alpha) {
						collision2--;
						objPtr->x = xtemp;
						objPtr->y = ytemp;
						objPtr->z = ztemp;

						return;
					}

					if (g_engine->_engine->objectTable[currentActorCol].indexInWorld == g_engine->_engine->cVars[getCVarsIdx(static_cast<enumCVars>(REVERSE_OBJECT))]) {
						objPtr->alpha = g_engine->_engine->cVars[getCVarsIdx(static_cast<enumCVars>(REVERSE_OBJECT))];
						g_engine->_engine->currentProcessedActorPtr->beta += 0x200;
						xtemp = x3;
						ztemp = z3;

						g_engine->_engine->currentProcessedActorPtr->worldX = g_engine->_engine->currentProcessedActorPtr->roomX = x3;
						g_engine->_engine->currentProcessedActorPtr->worldY = g_engine->_engine->currentProcessedActorPtr->roomY = y1;
						g_engine->_engine->currentProcessedActorPtr->worldZ = g_engine->_engine->currentProcessedActorPtr->roomZ = z3;

						g_engine->_engine->currentProcessedActorPtr->stepX = 0;
						g_engine->_engine->currentProcessedActorPtr->stepZ = 0;

						copyZv(&rangeZv2, &rangeZv);

						rangeZv.ZVX1 += x3;
						rangeZv.ZVX2 += x3;
						rangeZv.ZVY1 += y1;
						rangeZv.ZVY2 += y1;
						rangeZv.ZVZ1 += z3;
						rangeZv.ZVZ2 += z3;

						copyZv(&rangeZv, &g_engine->_engine->currentProcessedActorPtr->zv);

						objPtr->x = xtemp;
						objPtr->y = ytemp;
						objPtr->z = ztemp;

						return;

					} else {
						Object *actorPtr;

						g_engine->_engine->currentProcessedActorPtr->HIT = currentActorCol;
						actorPtr = &g_engine->_engine->objectTable[currentActorCol];
						actorPtr->HIT_BY = g_engine->_engine->currentProcessedActorIdx;
						actorPtr->hitForce = g_engine->_engine->currentProcessedActorPtr->hitForce;
					}
				}

				if (collision2) {
					playSound(g_engine->_engine->cVars[getCVarsIdx(static_cast<enumCVars>(SAMPLE_CHOC))]);
					throwStoppedAt(x3, z3);
					return;
				}
			}

			ptr = processActor2Sub(x2, y2, z2, &g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room]);

			if (ptr) {
				if (ptr->type == 0 || ptr->type == 10) {
					playSound(g_engine->_engine->cVars[getCVarsIdx(static_cast<enumCVars>(SAMPLE_CHOC))]);
					throwStoppedAt(x3, z3);
					return;
				}
			}

			if (asmCheckListCol(&rangeZv, &g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room])) {
				g_engine->_engine->currentProcessedActorPtr->hotPoint.x = 0;
				g_engine->_engine->currentProcessedActorPtr->hotPoint.y = 0;
				g_engine->_engine->currentProcessedActorPtr->hotPoint.z = 0;

				playSound(g_engine->_engine->cVars[getCVarsIdx(static_cast<enumCVars>(SAMPLE_CHOC))]);
				throwStoppedAt(x3, z3);
				return;
			}
		} while (g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 - 100 > x2 ||
				 g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 + 100 < x2 ||
				 g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 - 100 > z2 ||
				 g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 + 100 < z2);

		objPtr->x = xtemp;
		objPtr->y = ytemp;
		objPtr->z = ztemp;

		break;
	}
	default: {
		debug("Unsupported processAnimAction type %d\n", g_engine->_engine->currentProcessedActorPtr->animActionType);
		break;
	}
	}
}
} // namespace Fitd
