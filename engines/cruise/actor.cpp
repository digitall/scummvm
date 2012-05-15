/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "common/textconsole.h"

#include "cruise/cruise.h"
#include "cruise/staticres.h"
#include "common/list.h"
#include "common/list_intern.h"

namespace Cruise {

enum AnimPathIds {
	ANIM_WAIT	= -1,
	ANIM_FINISH = -2,
	ANIM_STATIC = -3
};

Actor::Actor() {
	_idx = 0;
	_type = 0;
	_overlayNumber = 0;
	_xDest = 0;
	_yDest = 0;
	_x = 0;
	_y = 0;
	_startDirection = 0;
	_nextDirection = 0;
	_endDirection = 0;
	_stepX = 0;
	_stepY = 0;
	_pathId = 0;
	_counter = 0;
	_poly = 0;
	_flag = 0;
	_start = 0;
	_freeze = 0;
}

bool Actor::isAnimFinished() {
	if (_pathId == ANIM_FINISH)
		return true;
	else
		return false;
}

bool ActorList::isAnimFinished(int overlayIdx, int idx, int objType) {
	Actor *pCurrentEntry = findActor(overlayIdx, idx, objType);
	if (!pCurrentEntry)
		return true;
	return pCurrentEntry->isAnimFinished();

}

Actor *ActorList::findActor(int overlayIdx, int objIdx, int type) {
	Common::List<Actor>::iterator iter = begin();

	while (iter != end()) {
		if ((iter->_overlayNumber == overlayIdx || overlayIdx == -1) &&
			(iter->_idx == objIdx || objIdx == -1) &&
			(iter->_type == type || type == -1)) {
			return &(*iter);
		}

		iter++;
	}
	return NULL;
}

int nclick_noeud;
int flag_aff_chemin;

void getPixel(int x, int y) {

	for (uint i = 0; i < _vm->_polyStructs->size(); ++i) {
		Ct &ct = (*_vm->_polyStructs)[i];
		numPoly = ct._num;

		if (walkboxes[numPoly]._state == 0 && ct._bounds.contains(x, y)) {
			// click was in given box
			int u = y - ct._bounds.top;
			CtEntry &cte = ct._slices[u];

			if ((x >= cte.minX && x <= cte.maxX)) {
				flag_obstacle = walkboxes[numPoly]._color;

				return;
			}
		}
	}

	flag_obstacle = 0;
}

int x_mouse;
int y_mouse;

int point_select;

int table_ptselect[2][2];

int X;
int Y;

int modelVar9;
int modelVar10;

void polydroite(int x1, int y1, int x2, int y2) {
	int dx;
	int dy;

	int mD0;
	int mD1;

	int mA0;
	int mA1;

	int bp;
	int cx;
	int si;

	int ax;
	int bx;

	modelVar9 = x1;
	modelVar10 = y1;

	dx = x2 - x1;
	dy = y2 - y1;

	mD0 = mD1 = 1;

	if (dx < 0) {
		dx = -dx;
		mD0 = -1;
	}

	if (dy < 0) {
		dy = -dy;
		mD1 = -1;
	}

	if (dx < dy) {
		mA0 = 0;
		bp = dx;
		cx = dy;

		mA1 = mD1;
	} else {
		mA1 = 0;
		bp = dy;
		cx = dx;

		mA0 = mD0;
	}

	bp = bp * 2;
	dx = bp - cx;
	si = dx - cx;

	ax = modelVar9;
	bx = modelVar10;

	getPixel(modelVar9, modelVar10);

	X = modelVar9;
	Y = modelVar10;

	if ((flag_obstacle == 0) || (cx == 0)) {
		flag_obstacle = 1;
		return;
	}

	while (--cx >= 0) {
		if (dx > 0) {
			ax += mD0;
			bx += mD1;
			dx += si;
		} else {
			ax += mA0;
			bx += mA1;
			dx += bp;
		}

		getPixel(ax, bx);

		X = ax;
		Y = bx;

		if (flag_obstacle == 0) {
			flag_obstacle = 1;
			return;
		}
	}

	flag_obstacle = 0;
}

void poly2(int x1, int y1, int x2, int y2) {
	int dx;
	int dy;

	int mD0;
	int mD1;

	int mA0;
	int mA1;

	int bp;
	int cx;
	int si;

	int ax;
	int bx;

	modelVar9 = x1;
	modelVar10 = y1;

	dx = x2 - x1;
	dy = y2 - y1;

	mD0 = mD1 = 1;

	if (dx < 0) {
		dx = -dx;
		mD0 = -1;
	}

	if (dy < 0) {
		dy = -dy;
		mD1 = -1;
	}

	if (dx < dy) {
		mA0 = 0;
		bp = dx;
		cx = dy;

		mA1 = mD1;
	} else {
		mA1 = 0;
		bp = dy;
		cx = dx;

		mA0 = mD0;
	}

	bp = bp * 2;
	dx = bp - cx;
	si = dx - cx;

	ax = modelVar9;
	bx = modelVar10;

	getPixel(modelVar9, modelVar10);

	X = modelVar9;
	Y = modelVar10;

	if ((flag_obstacle != 0) || (cx == 0)) {
		flag_obstacle = 1;
		return;
	}

	while (--cx >= 0) {
		if (dx > 0) {
			ax += mD0;
			bx += mD1;
			dx += si;
		} else {
			ax += mA0;
			bx += mA1;
			dx += bp;
		}

		getPixel(ax, bx);

		X = ax;
		Y = bx;

		if (flag_obstacle != 0) {
			flag_obstacle = 1;
			return;
		}
	}

	flag_obstacle = 0;
}

int point_proche(CtpRoute ctpRoutes[]) {
	int x1, y1, i, x, y, p;
	int d1 = 1000;

	_vm->_polyStructs = &_vm->_polyStructNorm;

	if (nclick_noeud == 1) {
		x = x_mouse;
		y = y_mouse;
		x1 = table_ptselect[0][0];
		y1 = table_ptselect[0][1];

		_vm->_polyStructs = &_vm->_polyStructExp;

		getPixel(x, y);

		if (!flag_obstacle) {
			_vm->_polyStructs = &_vm->_polyStructNorm;

			getPixel(x, y);

			if (flag_obstacle) {
				polydroite(x1, y1, x, y);
			}
			_vm->_polyStructs = &_vm->_polyStructExp;
		}
		if (!flag_obstacle) {	/* dans flag_obstacle --> couleur du point */
			x1 = table_ptselect[0][0];
			y1 = table_ptselect[0][1];

			poly2(x, y, x1, y1);

			x_mouse = X;
			y_mouse = Y;
		}
	}
	_vm->_polyStructs = &_vm->_polyStructNorm;

	p = -1;
	for (i = 0; i < routeCount; i++) {
		x = ctpRoutes[i]._coords[0];
		y = ctpRoutes[i]._coords[1];

		int pointDistance = computeDistance(x_mouse, y_mouse, x, y);
		if (pointDistance < d1) {
			polydroite(x_mouse, y_mouse, x, y);

			if (!flag_obstacle && routes[i]._route[0] > 0) {
				d1 = pointDistance;
				p = i;
			}
		}
	}

	return (p);
}

#define NBNOEUD 20

int16 select_noeud[3];
int8 solution[20 + 1];

int prem;
int prem2;
int dist_chemin;
int idsol;
int solmax;

int8 fl[NBNOEUD + 1];
int8 sol[NBNOEUD + 1];
int8 Fsol[NBNOEUD + 1];

int D;

void explore(int depart, int arrivee) {
	int id1, id2, i;

	id1 = depart;

	fl[id1]++;
	sol[idsol++] = (char)id1;

	if (idsol > solmax) {
		fl[id1] = -1;
		idsol--;

		return;
	}

	while ((i = fl[id1]) < 20) {
		id2 = routes[id1]._route[i + 1];

		if (id2 == arrivee) {
			if (idsol < solmax) {
				sol[idsol] = (char)arrivee;
				D = 0;
				for (i = 0; i < idsol; i++) {
					D = D + routes[(int)sol[i]]._distance[(int)sol[i + 1]];
					Fsol[i] = sol[i];
				}
				prem2 = 0;
				if (!prem) {
					dist_chemin = D;
					prem = 1;
					for (i = 0; i <= idsol; i++) {
						solution[i] = sol[i];
					}
					solution[i++] = -1;
					solution[i] = -1;
				} else if (D < dist_chemin) {
					dist_chemin = D;
					for (i = 0; i <= idsol; i++) {
						solution[i] = sol[i];
					}
					solution[i++] = -1;
					solution[i] = -1;
				}
			}
			fl[id1] = -1;
			idsol--;

			return;
		} else if ((id2 != -1) && ((int)fl[id2] == -1))
			explore(id2, arrivee);
		else if (id2 == -1) {
			fl[id1] = -1;
			idsol--;

			return;
		}
		fl[id1]++;
	}

	fl[id1] = -1;
	idsol--;
}

void chemin0(int depart, int arrivee) {
	int i;
	//int   y=30;

	prem = 0;
	prem2 = 0;
	dist_chemin = 0;
	idsol = 0;
	solmax = 999;

	for (i = 0; i < 20 + 1; i++)
		fl[i] = -1;

	X = 0, Y = 30;

	explore(depart, arrivee);
}

void valide_noeud(int16 table[], int16 p, int *nclick, int16 solution0[20 + 3][2]) {
	int a, b, d, i, p1, x1, x2, y1, y2;
	//int   y=30;

	table[*nclick] = p;
	table[(*nclick) + 1] = -1;
	table_ptselect[*nclick][0] = x_mouse;
	table_ptselect[*nclick][1] = y_mouse;
	(*nclick)++;
	_vm->_polyStructs = &_vm->_polyStructNorm;

	if (*nclick == 2) {	// second point
		x1 = table_ptselect[0][0];
		y1 = table_ptselect[0][1];
		x2 = table_ptselect[1][0];
		y2 = table_ptselect[1][1];
		if ((x1 == x2) && (y1 == y2)) {
			return;
		}
		flag_aff_chemin = 1;
		_vm->_polyStructs = &_vm->_polyStructExp;

		// can we go there directly ?
		polydroite(x1, y1, x2, y2);

		if (!flag_obstacle) {
			solution0[0][0] = x1;
			solution0[0][1] = y1;
			_vm->_polyStructs = &_vm->_polyStructExp;

			poly2(x2, y2, routes[select_noeud[1]]._coords[0],
			      routes[select_noeud[1]]._coords[1]);

			solution0[1][0] = table_ptselect[1][0] = X;
			solution0[1][1] = table_ptselect[1][1] = Y;
			solution0[2][0] = -1;

			if ((x1 == X) && (y1 == Y)) {
				flag_aff_chemin = 0;
				return;
			}
		} else {
			// no, we take the fastest way
			solution[0] = -1;
			if (routes[select_noeud[0]]._route[0] > 0)
				chemin0(table[0], table[1]);

			if (solution[0] == -1) {
				x1 = table_ptselect[0][0];
				y1 = table_ptselect[0][1];
				polydroite(x1, y1, x_mouse, y_mouse);
				solution0[0][0] = x1;
				solution0[0][1] = y1;
				solution0[1][0] = X;
				solution0[1][1] = Y;

				solution0[2][0] = -1;
				if ((x1 == X) && (y1 == Y)) {
					flag_aff_chemin = 0;
					return;
				}
			} else {
				solution0[0][0] = x1;
				solution0[0][1] = y1;
				i = 0;
				while (solution[i] != -1) {
					p1 = solution[i];
					solution0[i + 1][0] =
					    routes[p1]._coords[0];
					solution0[++i][1] =
					    routes[p1]._coords[1];
				}
				_vm->_polyStructs = &_vm->_polyStructExp;
				poly2(x2, y2,
				      routes[select_noeud[1]]._coords[0],
				      routes[select_noeud[1]]._coords[1]);
				solution0[i + 1][0] = table_ptselect[1][0] = X;
				solution0[i + 1][1] = table_ptselect[1][1] = Y;
				solution0[i + 2][0] = -1;
				if ((x1 == X) && (y1 == Y)) {
					flag_aff_chemin = 0;
					return;
				}

				/****** Trim down any un-necessary walk points ******/

				i++;
				d = 0;
				a = i;
				flag_obstacle = 1;
				while (d != a) {
					x1 = solution0[d][0];
					y1 = solution0[d][1];

					while (flag_obstacle && i != d) {
						x2 = solution0[i][0];
						y2 = solution0[i][1];
						_vm->_polyStructs = &_vm->_polyStructExp;
						polydroite(x1, y1, x2, y2);
						i--;
					}
					flag_obstacle = 1;
					if (d != i) {
						i++;
						for (b = d + 1; b < i; b++) {
							solution0[b][0] = -2;
						}
					} else
						i++;
					d = i;
					i = a;
				}
				flag_obstacle = 0;
			}
		}
	}
}

/**
 * Computes a path for an actor to walk between a given source and destination position
 */
int16 computePathfinding(MovementEntry &moveInfo, int16 x, int16 y, int16 destX, int16 destY, int16 stepX, int16 stepY, int16 oldPathId) {
	Perso *perso;
	int num;

	if (!_vm->_polyStruct) {
		moveInfo.x = -1;
		moveInfo.y = -1;

		return -1;
	}

	if (oldPathId >= 0) {
		if (persoTable[oldPathId]) {
			freePerso(oldPathId);
		}
	}

	if (!flagCt) {
		int i;
		int16 *ptr;

		for (i = 0; i < NUM_PERSONS; i++) {	// 10 = num perso
			if (!persoTable[i]) {
				break;
			}
		}

		if (i == NUM_PERSONS) {
			moveInfo.x = -1;
			moveInfo.y = -1;

			return -1;
		}

		perso = persoTable[i] = new Perso();

		ptr = perso->solution[0];

		perso->inc_jo1 = stepX;
		perso->inc_jo2 = stepY;

		*(ptr++) = x;
		*(ptr++) = y;
		*(ptr++) = moveInfo.x = destX;
		*(ptr++) = moveInfo.y = destY;
		*(ptr++) = -1;

		moveInfo.poly = numPoly;

		perso->inc_droite = 0;
		perso->inc_chemin = 0;

		return i;
	}

	nclick_noeud = 0;
	_vm->_polyStructs = &_vm->_polyStructNorm;
	flag_aff_chemin = 0;

	if (x == destX && y == destY) {
		moveInfo.x = -1;
		moveInfo.y = -1;

		return (-1);
	}

	/******* cherche le premier noeud ******/

	getPixel(x, y);

	moveInfo.poly = numPoly;

	x_mouse = x;
	y_mouse = y;

	if (!flag_obstacle || (point_select = point_proche(routes)) == -1) {
		moveInfo.x = -1;
		moveInfo.y = -1;

		return (-1);
	}

	valide_noeud(select_noeud, point_select, &nclick_noeud, NULL);

	flag_aff_chemin = 0;

	/******* cherche le deuxieme noeud ******/

	num = 0;
	while (num < NUM_PERSONS && persoTable[num] != NULL)
		num++;

	if (num == NUM_PERSONS) {
		moveInfo.x = -1;
		moveInfo.y = -1;
		return (-1);
	}

	perso = persoTable[num] = new Perso();

	perso->inc_jo1 = stepX;
	perso->inc_jo2 = stepY;

	x_mouse = destX;
	y_mouse = destY;

	if ((point_select = point_proche(routes)) != -1)
		valide_noeud(select_noeud, point_select, &nclick_noeud, perso->solution);

	if ((!flag_aff_chemin) || ((table_ptselect[0][0] == table_ptselect[1][0]) && (table_ptselect[0][1] == table_ptselect[1][1]))) {
		moveInfo.x = -1;
		moveInfo.y = -1;
		freePerso(num);

		return (-1);
	}

	moveInfo.x = table_ptselect[1][0];
	moveInfo.y = table_ptselect[1][1];
	moveInfo.poly = numPoly;
	perso->inc_chemin = 0;
	perso->inc_droite = 0;

	return (num);
}

void set_anim(int ovl, int obj, int start, int x, int y, int mat, int state) {
	int newf, zoom;

	newf = ABS(mat) - 1;

	zoom = computeZoom(y);
	if (mat < 0)
		zoom = -zoom;

	setObjectPosition(ovl, obj, 0, x);
	setObjectPosition(ovl, obj, 1, y);
	setObjectPosition(ovl, obj, 2, y);
	setObjectPosition(ovl, obj, 4, zoom);
	setObjectPosition(ovl, obj, 3, newf + start);
	setObjectPosition(ovl, obj, 5, state);
}

/**
 * Handles the processing of any active actors to allow for handling movement
 */
void ActorList::processAnimation() {
	objectParamsQuery params;
	MovementEntry moveInfo;
	Common::List<Actor>::iterator iter = begin();

	while (iter != end()) {
		if (!iter->_freeze && ((iter->_type == ATP_MOUSE) || (iter->_type == 1))) {
			getMultipleObjectParam(iter->_overlayNumber, iter->_idx, &params);

			if (((animationStart && !iter->_flag) || (!animationStart && iter->_xDest != -1
					&& iter->_yDest != -1)) && (iter->_type == ATP_MOUSE)) {
				// mouse animation
				if (!animationStart) {
					aniX = iter->_xDest;
					aniY = iter->_yDest;
					iter->_xDest = -1;
					iter->_yDest = -1;

					iter->_flag = 1;
				}

				iter->_pathId = computePathfinding(moveInfo, params.X, params.Y,
					aniX, aniY, iter->_stepX, iter->_stepY, iter->_pathId);

				if (iter->_pathId == ANIM_WAIT) {
					if ((iter->_endDirection != -1) && (iter->_endDirection != iter->_startDirection)) {
						iter->_phase = ANIM_PHASE_STATIC_END;
						iter->_nextDirection = iter->_endDirection;
						iter->_endDirection = -1;
						iter->_counter = 0;
					} else {
						iter->_pathId = ANIM_FINISH;
						iter->_flag = 0;
						iter->_endDirection = -1;
						iter->_phase = ANIM_PHASE_WAIT;
					}
				} else {
					iter->_phase = ANIM_PHASE_STATIC;
					iter->_counter = -1;
				}
			} else
				if ((iter->_type == 1) && (iter->_xDest != -1) && (iter->_yDest != -1)) {
					// track animation
					iter->_pathId = computePathfinding(moveInfo, params.X, params.Y, iter->_xDest, iter->_yDest, iter->_stepX, iter->_stepY, iter->_pathId);

					iter->_xDest = -1;
					iter->_yDest = -1;

					if (iter->_pathId == ANIM_WAIT) {
						if ((iter->_endDirection != -1) && (iter->_endDirection != iter->_startDirection)) {
							iter->_phase = ANIM_PHASE_STATIC_END;
							iter->_nextDirection = iter->_endDirection;
							iter->_endDirection = -1;
							iter->_counter = 0;
						} else {
							iter->_pathId = -2;
							iter->_flag = 0;
							iter->_endDirection = -1;
							iter->_phase = ANIM_PHASE_WAIT;
						}
					} else {
						iter->_phase = ANIM_PHASE_STATIC;
						iter->_counter = -1;
					}
				}

			animationStart = false;

			if ((iter->_pathId >= 0) || (iter->_phase == ANIM_PHASE_STATIC_END)) {

				// Main switch statement for handling various phases of movement
				// IMPORTANT: This switch relies on falling through cases in certain circumstances
				// , so 'break' statements should *not* be used at the end of case areas
				switch (iter->_phase) {
				case ANIM_PHASE_STATIC_END:
				case ANIM_PHASE_STATIC:
				{
					// In-place (on the spot) animationos

					if ((iter->_counter == -1) && (iter->_phase == ANIM_PHASE_STATIC)) {
						Perso *perso = persoTable[iter->_pathId];
						perso->processActorWalk(iter->_pathId, moveInfo);

						if (moveInfo.x == -1) {
							iter->_pathId = ANIM_FINISH;
							iter->_flag = 0;
							iter->_endDirection = -1;
							iter->_phase = ANIM_PHASE_WAIT;
							break;
						}

						iter->_x = moveInfo.x;
						iter->_y = moveInfo.y;
						iter->_nextDirection = moveInfo.direction;
						iter->_poly = moveInfo.poly;
						iter->_counter = 0;

						if (iter->_startDirection == iter->_nextDirection)
							iter->_phase = ANIM_PHASE_MOVE;
					}

					if ((iter->_counter >= 0)
					        && ((iter->_phase == ANIM_PHASE_STATIC_END)
					            || (iter->_phase == ANIM_PHASE_STATIC))) {
						int newA;
						int inc = 1;
						int t_inc = iter->_startDirection - 1;

						if (t_inc < 0)
							t_inc = 3;

						if (iter->_nextDirection == t_inc)
							inc = -1;

						if (inc > 0)
							newA = actor_stat[iter->_startDirection][iter->_counter++];
						else
							newA = actor_invstat[iter->_startDirection][iter->_counter++];

						if (newA == 0) {
							iter->_startDirection = iter->_startDirection + inc;

							if (iter->_startDirection > 3)
								iter->_startDirection = 0;

							if (iter->_startDirection < 0)
								iter-> _startDirection = 3;

							iter->_counter = 0;

							if (iter->_startDirection == iter->_nextDirection) {
								if (iter->_phase == ANIM_PHASE_STATIC)
									iter->_phase = ANIM_PHASE_MOVE;
								else
									iter->_phase = ANIM_PHASE_END;
							} else {
								newA = actor_stat[iter->_startDirection][iter->_counter++];

								if (inc == -1)
									newA = -newA;

								set_anim(iter->_overlayNumber, iter->_idx,
									iter->_start, params.X, params.Y, newA, iter->_poly);
								break;
							}
						} else {
							set_anim(iter->_overlayNumber,iter->_idx, iter->_start,
								params.X, params.Y, newA, iter->_poly);
							break;
						}
					}
				}

				case ANIM_PHASE_MOVE:
				{
					// Walk animations

					if (iter->_counter >= 1) {
						Perso *perso = persoTable[iter->_pathId];
						perso->processActorWalk(iter->_pathId, moveInfo);

						if (moveInfo.x == -1) {
							if ((iter->_endDirection == -1) || (iter->_endDirection == iter->_nextDirection)) {
								iter->_phase = ANIM_PHASE_END;
							} else {
								iter->_phase = ANIM_PHASE_STATIC_END;
								iter->_nextDirection = iter->_endDirection;
							}
							iter->_counter = 0;
							break;
						} else {
							iter->_x = moveInfo.x;
							iter->_y = moveInfo.y;
							iter->_nextDirection = moveInfo.direction;
							iter->_poly = moveInfo.poly;
						}
					}

					if (iter->_phase == ANIM_PHASE_MOVE) {
						int newA;

						iter->_startDirection = iter->_nextDirection;

						newA = actor_move[iter->_startDirection][iter->_counter++];
						if (!newA) {
							iter->_counter = 0;
							newA = actor_move[iter->_startDirection][iter->_counter++];
						}
						set_anim(iter->_overlayNumber, iter->_idx, iter->_start,
							iter->_x, iter->_y, newA, iter->_poly);
						break;
					}
				}

				case ANIM_PHASE_END:
				{
					// End of walk animation

					int newA = actor_end[iter->_startDirection][0];

					set_anim(iter->_overlayNumber, iter->_idx, iter->_start,
						iter->_x, iter->_y, newA, iter->_poly);

					iter->_pathId = ANIM_FINISH;
					iter->_phase = ANIM_PHASE_WAIT;
					iter->_flag = 0;
					iter->_endDirection = -1;
					break;
				}
				default: {
					warning("Unimplemented currentActor->phase=%d in processAnimation()", iter->_phase);
					// exit(1);
				}
				}
			}
		}

		iter++;
	}
}

Actor *ActorList::add(int overlay, int objIdx, int param, int param2) {
	Actor newActor;

	newActor._idx = objIdx;
	newActor._type = param2;
	newActor._pathId = -1;
	newActor._overlayNumber = overlay;
	newActor._startDirection = param;
	newActor._nextDirection = -1;
	newActor._stepX = 5;
	newActor._stepY = 2;
	newActor._phase = ANIM_PHASE_WAIT;
	newActor._flag = 0;
	newActor._freeze = 0;

	push_back(newActor);

	Common::List<Actor>:: iterator iter = reverse_begin();
	return &(*iter);
}

void ActorList::add(Actor actor) {
	push_back(actor);
}

int ActorList::remove(int overlay, int objIdx, int objType) {

	int dir = 0;
	Common::List<Actor>::iterator iter = begin();

	while(iter != end()) {
		if (((iter->_overlayNumber == overlay) || (overlay == -1)) &&
				((iter->_idx == objIdx) || (objIdx == -1)) &&
				((iter->_type == objType) || (objType == -1))) {
				dir = iter->_startDirection;
				if (iter->_pathId >= 0)
					freePerso(iter->_pathId);

				iter = erase(iter);

		}
		// the old remove was first changing matching elements _type to -1 then removed.
		// I am adding this control in chase -1 assigned _type some where else.
		if ( iter->_type == -1)
			iter = erase(iter);
		iter++;
	}

	return dir;
}

Common::List<Actor>::iterator ActorList::begin() {

	return Common::List<Actor>::begin();
}

uint ActorList::size() {

	return  Common::List<Actor>::size();
}

void ActorList::clear() {
	//Common::List<Actor>::clear();
	remove(-1, -1, -1);
}

} // End of namespace Cruise
