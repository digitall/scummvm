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

#ifndef RING_JERUSALEM_APPLICATION_H
#define RING_JERUSALEM_APPLICATION_H

#include "ring/base/application.h"

namespace Ring {

class ZoneSystemJerusalem;
class Zone1Jerusalem;
class Zone2Jerusalem;
class Zone3Jerusalem;
class Zone4Jerusalem;
class Zone5Jerusalem;
class Zone6Jerusalem;
class Zone7Jerusalem;
class Zone8Jerusalem;
class Zone9Jerusalem;
class Zone10Jerusalem;

class ApplicationJerusalem : public Application {
public:
	ApplicationJerusalem(RingEngine *engine);
	~ApplicationJerusalem();

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	void initFont() override;
	void setup() override;
	void initData() override;
	void initZones() override;
	void initBag() override;

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
	// Event handlers
	void onMouseLeftButtonUp(const Common::Event &evt, bool isControlPressed) override;
	void onMouseLeftButtonDown(const Common::Event &evt) override;
	void onMouseRightButtonUp(const Common::Event &evt) override;
	void onKeyDown(Common::Event &evt) override;
	void onTimer(TimerId id) override;

private:
	// Event handlers
	ZoneSystemJerusalem *_zoneSystem;
	Zone1Jerusalem      *_zone1;
	Zone2Jerusalem      *_zone2;
	Zone3Jerusalem      *_zone3;
	Zone4Jerusalem      *_zone4;
	Zone5Jerusalem      *_zone5;
	Zone6Jerusalem      *_zone6;
	Zone7Jerusalem      *_zone7;
	Zone8Jerusalem      *_zone8;
	Zone9Jerusalem      *_zone9;
	Zone10Jerusalem     *_zone10;

	friend class ZoneSystemJerusalem;
	friend class Zone1Jerusalem;
	friend class Zone2Jerusalem;
	friend class Zone3Jerusalem;
	friend class Zone4Jerusalem;
	friend class Zone5Jerusalem;
	friend class Zone6Jerusalem;
	friend class Zone7Jerusalem;
	friend class Zone8Jerusalem;
	friend class Zone9Jerusalem;
	friend class Zone10Jerusalem;
};

} // End of namespace Ring

#endif // RING_JERUSALEM_APPLICATION_H
