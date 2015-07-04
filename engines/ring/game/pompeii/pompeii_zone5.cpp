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

#include "ring/game/pompeii/pompeii_zone5.h"

#include "ring/game/pompeii/pompeii_application.h"
#include "ring/game/pompeii/pompeii_shared.h"

#include "ring/debug.h"
#include "ring/ring.h"
#include "ring/helpers.h"

using namespace PompeiiGame;

namespace Ring {

Zone5Pompeii::Zone5Pompeii(ApplicationPompeii *application) : _app(application) {
}

Zone5Pompeii::~Zone5Pompeii() {
	// Zero-out passed pointers
	_app = nullptr;
}

void Zone5Pompeii::onInit() {
	_app->setCurrentZone(kZone5);

	_app->puzzleAdd(kPuzzle40411);
	_app->puzzleAddBackgroundImage(kPuzzle40411, "s04a01n01p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle40441);
	_app->puzzleAddBackgroundImage(kPuzzle40441, "s04a04n01p01.bmp", Common::Point(0, 16), true);
	_app->rotationAdd(10411, "S04A01-4", 0, 6);
	_app->rotationAdd(10412, "S04A01-3", 0, 3);
	_app->rotationAdd(10413, "S04A01-2", 0, 4);
	_app->rotationAdd(10414, "S04A01-1", 0, 3);
	_app->rotationAdd(10431, "S04A03-1", 0, 0);
	_app->rotationAdd(10441, "S04A04-2", 0, 5);
	_app->rotationAdd(10442, "S04A04-1", 0, 4);
	_app->rotationAdd(10451, "S04A05-2", 0, 0);
	_app->rotationAdd(10452, "S04A05-1", 0, 0);
	_app->rotationAddMovabilityToRotation(10411, 10412, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10411, 0, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10411, 10413, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10411, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10411, 10414, "", Common::Rect(3560, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10411, 2, 318.0f, 0.30000001f, 87.0f, 0.0f, 1, 318.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10411, 10414, "", Common::Rect(0, -250, 110, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10411, 3, 318.0f, 0.30000001f, 87.0f, 0.0f, 1, 318.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10412, 10411, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10412, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10412, 10413, "", Common::Rect(2600, -250, 2750, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10412, 1, 222.0f, 0.30000001f, 87.0f, 0.0f, 1, 222.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10412, 10414, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10412, 2, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10413, 10411, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10413, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10413, 10412, "", Common::Rect(800, -250, 950, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10413, 1, 42.0f, 0.30000001f, 87.0f, 0.0f, 1, 42.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10413, 10414, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10413, 2, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10413, 10431, "", Common::Rect(2000, -61, 2211, 304), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10413, 3, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10414, 10411, "", Common::Rect(1760, -250, 1910, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10414, 0, 138.0f, 0.30000001f, 87.0f, 0.0f, 1, 138.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10414, 10412, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10414, 1, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10414, 10413, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10414, 2, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10414, 10441, "", Common::Rect(3007, -85, 3255, 403), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10414, 3, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10431, 10413, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10431, 0, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10441, 10414, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10441, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10441, 10442, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10441, 1, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10441, 10452, "", Common::Rect(2596, -40, 2698, 222), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10441, 2, 245.0f, 0.30000001f, 87.0f, 0.0f, 1, 245.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10442, 10414, "", Common::Rect(736, -50, 840, 243), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10442, 0, 60.0f, 0.30000001f, 87.0f, 0.0f, 1, 60.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10442, 10441, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10442, 1, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10442, 10452, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10442, 2, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10451, 10442, "", Common::Rect(834, -43, 949, 236), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10451, 0, 68.0f, 0.30000001f, 87.0f, 0.0f, 1, 68.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10451, 10452, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10451, 1, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10452, 10442, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10452, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10452, 10451, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10452, 1, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->objectAdd(kObject10405, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10405, 10411, Common::Rect(2123, -103, 2343, 299), true, kCursorMove, 0);
	_app->objectAddRotationAccessibility(kObject10405, 10413, Common::Rect(1618, -68, 1710, 187), true, kCursorMove, 0);
	_app->objectAddPresentation(kObject10405);
	_app->objectPresentationAddImageToRotation(kObject10405, 0, 10411, 0);
	_app->objectAddPresentation(kObject10405);
	_app->objectPresentationAddImageToRotation(kObject10405, 1, 10441, 0);
	_app->objectAddPresentation(kObject10405);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10411, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10411, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10411, 4, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10411, 5, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10412, 0, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10412, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10412, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10413, 0, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10413, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10413, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10413, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10414, 0, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10414, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10414, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10441, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10441, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10441, 4, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10442, 0, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10442, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10442, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 2, 10442, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToPuzzle(kObject10405, 2, kPuzzle40411, "s04a01n01p01s02", kImageTypeBMP, Common::Point(235, 55), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationAddAnimationToPuzzle(kObject10405, 2, kPuzzle40441, "s04a04n01p01s02", kImageTypeBMP, Common::Point(28, 319), kDrawTypeNormal, 2000, 14, 12.5f, 4);
	_app->objectAddPresentation(kObject10405);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 3, 10411, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToPuzzle(kObject10405, 3, kPuzzle40411, "s04a01n01p01s01", kImageTypeBMP, Common::Point(342, 132), kDrawTypeNormal, 2000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject10405, 3, 0, 40002);
	_app->objectAddPresentation(kObject10405);
	_app->objectPresentationAddAnimationToRotation(kObject10405, 4, 10441, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToPuzzle(kObject10405, 4, kPuzzle40441, "s04a04n01p01s01", kImageTypeBMP, Common::Point(152, 227), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject10405, 4, 0, 40001);
	_app->objectPresentationShow(kObject10405, 2);
	_app->objectAdd(kObjectHollyWater, "Holly Water", "awater", 1);
	_app->objectAdd(kObject40411, "", "", 1);
	_app->objectAddRotationAccessibility(kObject40411, 10441, Common::Rect(2258, -1, 2420, 297), false, kCursorAction, 0);
	_app->objectAdd(kObject40412, "", "", 1);
	_app->objectAddRotationAccessibility(kObject40412, 10411, Common::Rect(163, -40, 367, 283), false, kCursorAction, 0);
	_app->soundAdd(50001, kSoundTypeAmbientMusic, "04_AMB-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundSetVolume(50001, 90);
	_app->rotationAddAmbientSound(10411, 50001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10412, 50001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10413, 50001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10414, 50001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10431, 50001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10441, 50001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10442, 50001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10451, 50001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10452, 50001, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle40411, 50001, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle40441, 50001, 90, 0, 1, 1, 10);
	_app->soundAdd(40411, kSoundTypeDialog, "1088.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(40412, kSoundTypeDialog, "1089.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(40413, kSoundTypeDialog, "1090.wac", _app->getConfiguration().dialog.loadFrom);
}

void Zone5Pompeii::onButtonUp(ObjectId id, Id /*target*/, Id /*puzzleRotationId*/, uint32 /*a4*/, const Common::Point &/*point*/) {
	switch (id) {
	default:
		break;

	case kObject40412:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->varSetByte(90112, 1);
			_app->objectSetAccessibilityOff(kObject40412);
			_app->puzzleSetActive(kPuzzle40411);
			_app->soundPlay(40411);
		}
		break;

	case kObject10405:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->objectPresentationPauseAnimation(kObject10405, 4);
			_app->objectPresentationPauseAnimation(kObject10405, 3);
			_app->timerStopAll();
			_app->soundStop(50001, 1024);
			_app->onCall(10400);
		}
		break;

	case kObject40411:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->varSetByte(90112, 2);
			_app->bagRemove(kObjectNecklace);
			_app->objectSetAccessibilityOff(kObject40411);
			_app->puzzleSetActive(kPuzzle40441);
			_app->soundPlay(40412);
		}
		break;

	case kObjectExitEncyclopedia:
		_app->restore();
		break;
	}
}

void Zone5Pompeii::onTimer(TimerId id) {
	switch (id) {
	default:
		break;

	case kTimer0:
		_app->timerStop(kTimer0);
		_app->objectPresentationUnpauseAnimation(kObject10405, 3);
		break;

	case kTimer1:
		_app->timerStop(kTimer1);
		_app->objectPresentationUnpauseAnimation(kObject10405, 4);
		break;
	}
}

void Zone5Pompeii::onAnimationNextFrame(Id animationId, const Common::String &/*name*/, uint32 frame, uint32 frameCount) {
	switch (animationId) {
	default:
		break;

	case 40001:
		if (frame == frameCount || frame == 1) {
			_app->objectPresentationPauseAnimation(kObject10405, 4);
			_app->timerStart(kTimer1, 1000 * (rnd(3) + 3));
		}
		break;

	case 40002:
		if (frame == frameCount || frame == 1) {
			_app->objectPresentationPauseAnimation(kObject10405, 3);
			_app->timerStart(kTimer0, 1000 * (rnd(3) + 4));
		}
		break;
	}
}

void Zone5Pompeii::onSound(Id id, SoundType /*type*/, uint32 /*a3*/, bool process) {
	if (!process)
		return;

	switch (id) {
	default:
		break;

	case 40411:
		_app->objectPresentationHide(kObject10405, 0);
		_app->objectPresentationHide(kObject10405, 3);
		_app->playMovie("S04A01-2");
		_app->rotationSetActive(10411);
		_app->objectSetAccessibilityOn(kObject40411);
		_app->bagAdd(kObjectHollyWater);
		_app->rotationSetMovabilityOn(10411);
		break;

	case 40412:
		_app->playMovie("S04A01-1");
		_app->soundPlay(40413);
		break;

	case 40413:
		_app->fadeOut(15, Color(0, 0, 0), 0);
		_app->soundStop(50001, 1024);
		_app->objectPresentationHide(kObject10405);
		_app->rotationSetMovabilityOn(10441);
		_app->onCall(1129);
		_app->onCall(113);
		break;
	}
}

} // End of namespace Ring
