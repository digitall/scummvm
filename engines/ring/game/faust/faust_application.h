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

#ifndef RING_FAUST_APPLICATION_H
#define RING_FAUST_APPLICATION_H

#include "ring/base/application.h"

namespace Ring {

class ZoneSystemFaust;
class Zone2Faust;
class Zone3Faust;
class Zone4Faust;
class Zone5Faust;
class Zone6Faust;
class Zone7Faust;
class Zone8Faust;
class Zone9Faust;
class Zone10Faust;
class Zone11Faust;
class Zone12Faust;
class Zone13Faust;
class Zone14Faust;
class Zone15Faust;
class Zone16Faust;
class Zone17Faust;

class ApplicationFaust : public Application {
public:
	ApplicationFaust(RingEngine *engine);
	~ApplicationFaust();

	//////////////////////////////////////////////////////////////////////////
	// Initialization
	void initLanguages() override;
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
	// Zone
	void setupZone(ZoneId zone, SetupType type) override;

	//////////////////////////////////////////////////////////////////////////
	// Event handler
	void onMouseLeftButtonUp(const Common::Event &evt, bool isControlPressed) override;
	void onMouseLeftButtonDown(const Common::Event &evt) override;
	void onMouseRightButtonUp(const Common::Event &evt) override;
	void onKeyDown(Common::Event &evt) override;
	void onTimer(TimerId id) override;

	void onSound(Id id, SoundType type, uint32 a3) override;
	void onSetup(ZoneId zone, SetupType type) override;
	void onBag(ObjectId id, Id target, Id puzzleRotationId, uint32 a4, DragControl *dragControl, byte type) override;
	void onBagClickedObject(ObjectId id) override;
	void onUpdateBag(const Common::Point &point) override;
	void onUpdateBefore(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, const Common::Point &point) override;
	void onBeforeRide(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, MovabilityType movabilityType) override;
	void onAfterRide(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, MovabilityType movabilityType) override;
	void onAnimationNextFrame(Id animationId, const Common::String &name, uint32 frame, uint32 frameCount) override;
	void onVisualList(Id id, uint32 type, const Common::Point &point) override;

protected:
	//////////////////////////////////////////////////////////////////////////
	// Buttons
	//////////////////////////////////////////////////////////////////////////
	void onButtonDown(ObjectId id, Id target, Id puzzleRotationId, uint32 a4, const Common::Point &point);
	void onButtonUp(ObjectId id, Id target, Id puzzleRotationId, uint32 a4, const Common::Point &point);

	//////////////////////////////////////////////////////////////////////////
	// Zone switching
	//////////////////////////////////////////////////////////////////////////
	// FIXME Implement zone switch
	//void onSwitchZone(ZoneId zone, uint32 type);

	//////////////////////////////////////////////////////////////////////////
	// Helper functions
	//////////////////////////////////////////////////////////////////////////
	void setupRotation(uint32 type);
	void sub_45FF30();
	void sub_468290(uint32 a1);
	void sub_4A2CF0();

private:
	// Event handlers
	ZoneSystemFaust *_zoneSystem;
	Zone2Faust      *_zone2;
	Zone3Faust      *_zone3;
	Zone4Faust      *_zone4;
	Zone5Faust      *_zone5;
	Zone6Faust      *_zone6;
	Zone7Faust      *_zone7;
	Zone8Faust      *_zone8;
	Zone9Faust      *_zone9;
	Zone10Faust     *_zone10;
	Zone11Faust     *_zone11;
	Zone12Faust     *_zone12;
	Zone13Faust     *_zone13;
	Zone14Faust     *_zone14;
	Zone15Faust     *_zone15;
	Zone16Faust     *_zone16;
	Zone17Faust     *_zone17;

	int32 _slot;
	ZoneId _zone;

	//////////////////////////////////////////////////////////////////////////
	// Start menu
	//////////////////////////////////////////////////////////////////////////
	void setProgressAndShowMenu(ProgressState progress);
	void loadAndStartLogin();
	void loadAndInitZone();
	void initZone();
	void initMenuSave(bool savegame);
	void initMenu2();
	void initMenu3();

	friend class ZoneSystemFaust;
	friend class Zone2Faust;
	friend class Zone3Faust;
	friend class Zone4Faust;
	friend class Zone5Faust;
	friend class Zone6Faust;
	friend class Zone7Faust;
	friend class Zone8Faust;
	friend class Zone9Faust;
	friend class Zone10Faust;
	friend class Zone11Faust;
	friend class Zone12Faust;
	friend class Zone13Faust;
	friend class Zone14Faust;
	friend class Zone15Faust;
	friend class Zone16Faust;
	friend class Zone17Faust;
};

} // End of namespace Ring

#endif // RING_FAUST_APPLICATION_H
