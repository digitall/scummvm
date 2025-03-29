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

#ifndef FITD_ANIM_H
#define FITD_ANIM_H

#define INFO_TRI 1
#define INFO_ANIM 2
#define INFO_TORTUE 4
#define INFO_OPTIMISE 8

#include "common/scummsys.h"
#include "common/array.h"

namespace Fitd {

int initAnim(int animNum,int animType, int animInfo);
int setAnimObjet(int frame, char* anim, char* body);
int16 setInterAnimObjet(int frame, char* animPtr, char* bodyPtr);
int16 getNbFramesAnim(char* animPtr);
void initBufferAnim(Common::Array<int16>& animBuffer, char* bodyPtr);
void updateAnimation(void);
void initCopyBox(char* var0, char* var1);

}

#endif
