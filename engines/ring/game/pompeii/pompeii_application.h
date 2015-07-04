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

#ifndef RING_POMPEII_APPLICATION_H
#define RING_POMPEII_APPLICATION_H

#include "ring/base/application.h"

namespace Ring {

class ZoneSystemPompeii;
class Zone1Pompeii;
class Zone2Pompeii;
class Zone3Pompeii;
class Zone4Pompeii;
class Zone5Pompeii;
class Zone6Pompeii;
class Zone7Pompeii;
class Zone8Pompeii;
class Zone9Pompeii;
class Zone10Pompeii;
class Zone11Pompeii;
class Zone12Pompeii;

class ApplicationPompeii : public Application {
public:
	ApplicationPompeii(RingEngine *engine);
	~ApplicationPompeii();

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	void initFont() override;
	void setup() override;
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
	// Messages
	void messageInsertCd(ZoneId zone) override;

	//////////////////////////////////////////////////////////////////////////
	// Event handling
	void onMouseLeftButtonUp(const Common::Event &evt, bool isControlPressed) override;
	void onMouseLeftButtonDown(const Common::Event &evt) override;
	void onMouseRightButtonUp(const Common::Event &evt) override;
	void onKeyDown(Common::Event &evt) override;
	void onTimer(TimerId id) override;

	void onSound(Id id, SoundType type, uint32 a3) override;
	void onSetup(ZoneId zone, SetupType type) override;
	void onBag(ObjectId id, Id target, Id puzzleRotationId, uint32 a4, DragControl *dragControl, byte type) override;
	void onUpdateBefore(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, const Common::Point &point) override;
	void onUpdateAfter(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, MovabilityType movabilityType, const Common::Point &point) override;
	void onUpdateBag(const Common::Point &point) override;
	void onBagClickedObject(ObjectId id) override;
	void onBeforeRide(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, MovabilityType movabilityType) override;
	void onAfterRide(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, MovabilityType movabilityType) override;
	void onAnimationNextFrame(Id animationId, const Common::String &name, uint32 frame, uint32 frameCount) override;
	void onVisualList(Id id, uint32 type, const Common::Point &point) override;

protected:
	//////////////////////////////////////////////////////////////////////////
	// Buttons
	//////////////////////////////////////////////////////////////////////////
	void onButtonUp(ObjectId id, Id target, Id puzzleRotationId, uint32 a4, const Common::Point &point);

	void onKeyDown(const Common::KeyCode &keycode, uint ascii);

	//////////////////////////////////////////////////////////////////////////
	// Misc
	//////////////////////////////////////////////////////////////////////////
	bool giveMoney(int16 amount);
	void takeMoney(int16 amount);
	void onCall(Id callType);
	void showEncyclopedia(uint32 index);
	void restore();

	void loadSaveList(uint32 userId, Id visualId, PuzzleId puzzleId);

private:
	// Event handlers
	ZoneSystemPompeii *_zoneSystem;
	Zone1Pompeii      *_zone1;
	Zone2Pompeii      *_zone2;
	Zone3Pompeii      *_zone3;
	Zone4Pompeii      *_zone4;
	Zone5Pompeii      *_zone5;
	Zone6Pompeii      *_zone6;
	Zone7Pompeii      *_zone7;
	Zone8Pompeii      *_zone8;
	Zone9Pompeii      *_zone9;
	Zone10Pompeii     *_zone10;
	Zone11Pompeii     *_zone11;
	Zone12Pompeii     *_zone12;

	// Saved zone when showing encyclopedia
	ZoneId _savedZone;

	uint32 checkBag();
	void setCoordinatesOnPuzzle6();
	void showDay();
	void setupUser(Id userId);

	friend class ZoneSystemPompeii;
	friend class Zone1Pompeii;
	friend class Zone2Pompeii;
	friend class Zone3Pompeii;
	friend class Zone4Pompeii;
	friend class Zone5Pompeii;
	friend class Zone6Pompeii;
	friend class Zone7Pompeii;
	friend class Zone8Pompeii;
	friend class Zone9Pompeii;
	friend class Zone10Pompeii;
	friend class Zone11Pompeii;
	friend class Zone12Pompeii;
};

} // End of namespace Ring

#endif // RING_POMPEII_APPLICATION_H
