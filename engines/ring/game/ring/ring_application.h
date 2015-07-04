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

#ifndef RING_RING_APPLICATION_H
#define RING_RING_APPLICATION_H

#include "ring/base/application.h"

namespace Ring {

class Accessibility;
class Movability;

class ZoneSystemRing;
class ZoneNIRing;
class ZoneRHRing;
class ZoneFORing;
class ZoneRORing;
class ZoneWARing;
class ZoneASRing;
class ZoneN2Ring;

class ApplicationRing : public Application {
public:
	ApplicationRing(RingEngine *engine);
	~ApplicationRing();

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
	void refreshPreferences() override;

	//////////////////////////////////////////////////////////////////////////
	// Drawing
	void draw() override;

	//////////////////////////////////////////////////////////////////////////
	// Setup
	void setupZone(ZoneId zone, SetupType type) override;
	void setZone(ZoneId zone, SetupType type) override;
	void setZoneAndEnableBag(ZoneId zone);

	//////////////////////////////////////////////////////////////////////////
	// Messages
	void messageInsertCd(ZoneId zone) override;

	//////////////////////////////////////////////////////////////////////////
	// Event handlers
	void onMouseLeftButtonUp(const Common::Event &evt, bool isControlPressed) override;
	void onMouseLeftButtonDown(const Common::Event &evt) override;
	void onMouseRightButtonUp(const Common::Event &evt) override;
	void onKeyDown(Common::Event &evt) override;
	void onTimer(TimerId id) override;
	void onSound(Id id, SoundType type, uint32 a3) override;
	void onSetup(ZoneId zone, SetupType type) override;
	void onBag(ObjectId id, Id target, Id puzzleRotationId, uint32 a4, DragControl *dragControl, byte type) override;
	void onBagClickedObject(ObjectId id) override;
	void onBagZoneSwitch() override;
	void onUpdateBag(const Common::Point &point) override;
	void onUpdateBefore(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, const Common::Point &point) override;
	void onUpdateAfter(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, MovabilityType movabilityType, const Common::Point &point) {}
	void onBeforeRide(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, MovabilityType movabilityType) override;
	void onAfterRide(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, MovabilityType movabilityType) override;
	void onAnimationNextFrame(Id animationId, const Common::String &name, uint32 frame, uint32 frameCount) override;
	void onAnimation(uint32 type, Id animationId, const Common::String &name, uint32 frame, uint32 a5) override;
	void onVisualList(Id id, uint32 type, const Common::Point &point) override;

protected:
	void sub_433EE0();
	void sub_445A10();

	void onSwitchZone(ZoneId zone, uint32 type);
	void onSetupLoadTimers(Common::String zoneName, Id testId1, Id puzzleRotationId, Id testId2);

private:
	// Event handlers
	ZoneSystemRing     *_zoneSystem;
	ZoneNIRing         *_zoneNI;
	ZoneRHRing         *_zoneRH;
	ZoneFORing         *_zoneFO;
	ZoneRORing         *_zoneRO;
	ZoneWARing         *_zoneWA;
	ZoneASRing         *_zoneAS;
	ZoneN2Ring         *_zoneN2;

	bool         _controlNotPressed;
	int32        _presentationIndexNI;

	void onButtonDown(ObjectId id, Id target, Id puzzleRotationId, uint32 a4, const Common::Point &point);
	void onButtonUp(ObjectId id, Id target, Id puzzleRotationId, uint32 a4, const Common::Point &point);
	void onButtonUp2(ObjectId id, uint32 index, Id puzzleRotationId, uint32 a4, const Common::Point &point);

	void onMouseLeftButtonUp(const Common::Event &evt);
	bool handleLeftButtonDown(Accessibility *accessibility, uint32 index, Id id, const Common::Point &point);
	bool handleLeftButtonUp(Accessibility *accessibility, Id id, const Common::Point &point);
	bool handleLeftButtonUp(Movability *movability, uint32 index, Id id, bool isRotation = false);
	void onKeyDownZone(const Common::KeyState &keyState);

	uint32 getCdForZone(ZoneId zone) const;
	bool isDataPresent(SetupType type);

	friend class ZoneSystemRing;
	friend class ZoneNIRing;
	friend class ZoneRHRing;
	friend class ZoneFORing;
	friend class ZoneRORing;
	friend class ZoneWARing;
	friend class ZoneASRing;
	friend class ZoneN2Ring;
};

} // End of namespace Ring

#endif // RING_RING_APPLICATION_H
