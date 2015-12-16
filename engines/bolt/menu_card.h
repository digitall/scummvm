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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef BOLT_MENU_STATE_H
#define BOLT_MENU_STATE_H

#include "bolt/bolt.h"
#include "bolt/scene.h"

namespace Common {
struct Event;
};

namespace Bolt {

class BoltEngine;

class MenuCard : public Card {
public:
	virtual void enter(uint32 time);
	virtual Signal handleEvent(const BoltEvent &event);

protected:
	void init(Graphics *graphics, Boltlib &boltlib, BltId resId);
	virtual Signal handleButtonClick(int num) = 0;

	BoltEngine *_engine;
	Scene _scene;
};

class GenericMenuCard : public MenuCard {
public:
	void init(Graphics *graphics, Boltlib &boltlib, BltId resId);
protected:
	virtual Signal handleButtonClick(int num);
};

} // End of namespace Bolt

#endif
