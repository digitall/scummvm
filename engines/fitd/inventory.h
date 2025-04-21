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

#ifndef FITD_INVENTORY_H
#define FITD_INVENTORY_H

#include "common/scummsys.h"

#define INVENTORY_SIZE 30
#define NUM_MAX_INVENTORY 2

namespace Fitd {

extern int16 currentInventory;
extern int16 numObjInInventoryTable[NUM_MAX_INVENTORY];
extern int16 inHandTable[NUM_MAX_INVENTORY];
extern int16 inventoryTable[NUM_MAX_INVENTORY][INVENTORY_SIZE];

extern int statusLeft;
extern int statusTop;
extern int statusRight;
extern int statusBottom;

void processInventory();

} // namespace Fitd

#endif
