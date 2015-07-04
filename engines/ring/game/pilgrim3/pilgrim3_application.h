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
 */

#ifndef RING_PILGRIM3_APPLICATION_H
#define RING_PILGRIM3_APPLICATION_H

#include "ring/base/application.h"

namespace Ring {

class ZoneSystemPilgrim3;
class Zone6Pilgrim3;
class Zone7Pilgrim3;
class Zone8Pilgrim3;
class Zone9Pilgrim3;
class Zone10Pilgrim3;

class ApplicationPilgrim3 : public Application {
public:
	ApplicationPilgrim3(RingEngine *engine);
	~ApplicationPilgrim3();

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	void initFont() override;
	void setup() override;
	void initZones() override;
	void initBag() override;
	void initData() override;

	//////////////////////////////////////////////////////////////////////////
	// Startup & Menu
	void showStartupScreen() override;
	void startMenu(bool savegame) override;
	void showMenu(ZoneId zone, MenuAction menuAction) override;
	void showCredits() override;
	void startGame() override;

	//////////////////////////////////////////////////////////////////////////
	// Drawing
	void draw() override;

	//////////////////////////////////////////////////////////////////////////
	// Event handler
	void onMouseLeftButtonUp(const Common::Event &evt, bool isControlPressed) override;
	void onMouseLeftButtonDown(const Common::Event &evt) override;
	void onMouseRightButtonUp(const Common::Event &evt) override;
	void onKeyDown(Common::Event &evt) override;
	void onTimer(TimerId id) override;

private:
	ZoneSystemPilgrim3 *_zoneSystem;
	Zone6Pilgrim3      *_zone6;
	Zone7Pilgrim3      *_zone7;
	Zone8Pilgrim3      *_zone8;
	Zone9Pilgrim3      *_zone9;
	Zone10Pilgrim3     *_zone10;
};

} // End of namespace Ring

#endif // RING_PILGRIM3_APPLICATION_H
