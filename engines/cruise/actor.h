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

#ifndef CRUISE_ACTOR_H
#define CRUISE_ACTOR_H

namespace Cruise {

enum animPhase {
	ANIM_PHASE_WAIT = 0,
	ANIM_PHASE_STATIC = 1,
	ANIM_PHASE_MOVE = 2,
	ANIM_PHASE_STATIC_END = 3,
	ANIM_PHASE_END = 4
};

enum ATP {
	ATP_MOUSE = 0,
	ATP_TRACK = 1
};

class Actor {
public:
	int16 _idx;
	int16 _type;
	int16 _overlayNumber;
	int16 _xDest;
	int16 _yDest;
	int16 _x;
	int16 _y;
	int16 _startDirection;
	int16 _nextDirection;
	int16 _endDirection;
	int16 _stepX;
	int16 _stepY;
	int16 _pathId;
	animPhase _phase;
	int16 _counter;
	int16 _poly;
	int16 _flag;
	int16 _start;
	int16 _freeze;

	Actor();
};
class ActorListNode {
public:
	ActorListNode *_next;
	ActorListNode *_prev;
	Actor *_actor;

	ActorListNode();
	~ActorListNode();
	ActorListNode *addActor(int overlay, int objIdx, int param, int param2);

};


int removeActor(ActorListNode * pHead, int overlay, int objIdx, int objType);
bool isAnimFinished(int overlayIdx, int idx, ActorListNode *pStartEntry, int objType);
ActorListNode *findActor(ActorListNode *pStartEntry, int overlayIdx, int objIdx, int type);
void processAnimation(ActorListNode *pObject);
void getPixel(int x, int y);

} // End of namespace Cruise

#endif
