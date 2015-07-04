/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 7
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 07110-1301, USA.
 */

#ifndef RING_POMPEII_ZONE_4_H
#define RING_POMPEII_ZONE_4_H

#include "ring/base/event.h"

#include "ring/shared.h"

namespace Ring {

class ApplicationPompeii;

class Zone4Pompeii : public EventHandlerZone {
public:
	Zone4Pompeii(ApplicationPompeii *application);
	~Zone4Pompeii();

	void onInit() override;
	void onSetup(SetupType type) override;
	void onButtonUp(ObjectId id, Id target, Id puzzleRotationId, uint32 a4, const Common::Point &point) override;

	void onTimer(TimerId id) override;
	void onAnimationNextFrame(Id animationId, const Common::String &name, uint32 frame, uint32 frameCount) override;
	void onSound(Id id, SoundType type, uint32 a3, bool process) override;
	void onUpdateBag(const Common::Point &point) override;
	void onUpdateBefore(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, const Common::Point &point) override;
	void onUpdateAfter(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, MovabilityType movabilityType, const Common::Point &point) override;

	void onVisual(int x);

private:
	ApplicationPompeii *_app;

	bool _hideBox;
};

} // End of namespace Ring

#endif // RING_POMPEII_ZONE_4_H
