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

#include "ring/game/pompeii/pompeii_zone11.h"

#include "ring/game/pompeii/pompeii_application.h"
#include "ring/game/pompeii/pompeii_shared.h"

#include "ring/debug.h"
#include "ring/ring.h"
#include "ring/helpers.h"

using namespace PompeiiGame;

namespace Ring {

Zone11Pompeii::Zone11Pompeii(ApplicationPompeii *application) : _app(application) {
	_hideBox = false;
}

Zone11Pompeii::~Zone11Pompeii() {
	// Zero-out passed pointers
	_app = nullptr;
}

void Zone11Pompeii::onInit() {
	_app->setCurrentZone(kZone11);

	_app->puzzleAdd(kPuzzle110111);
	_app->puzzleAddBackgroundImage(kPuzzle110111, "s10a01n01p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle110221);
	_app->puzzleAddBackgroundImage(kPuzzle110221, "s10a02n02p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle110222);
	_app->puzzleAddBackgroundImage(kPuzzle110222, "s10a02n02p02.bmp", Common::Point(0, 16), true);
	_app->rotationAdd(11011, "S10A01-1", 0, 2);
	_app->rotationAdd(11021, "S10A02-5", 0, 1);
	_app->rotationAdd(11022, "S10A02-4", 0, 5);
	_app->rotationAdd(11023, "S10A02-3", 0, 3);
	_app->rotationAdd(11024, "S10A02-2", 0, 0);
	_app->rotationAdd(11025, "S10A02-1", 0, 0);
	_app->rotationAddMovabilityToRotation(11011, 11025, "", Common::Rect(2950, -250, 3100, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11011, 0, 257.0f, 0.30000001f, 87.0f, 0.0f, 1, 257.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11021, 11022, "", Common::Rect(2110, -250, 2260, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11021, 0, 173.0f, 0.30000001f, 87.0f, 0.0f, 1, 173.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11021, 11023, "", Common::Rect(330, -250, 480, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11021, 1, 355.0f, 0.30000001f, 87.0f, 0.0f, 1, 355.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11022, 11021, "", Common::Rect(310, -250, 460, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11022, 0, 353.0f, 0.30000001f, 87.0f, 0.0f, 1, 353.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11022, 11024, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11022, 1, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11023, 11021, "", Common::Rect(2130, -250, 2280, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11023, 0, 175.0f, 0.30000001f, 87.0f, 0.0f, 1, 175.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11024, 11022, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11024, 0, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11024, 11025, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11024, 1, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11025, 11011, "", Common::Rect(1150, -250, 1300, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11025, 0, 77.0f, 0.30000001f, 87.0f, 0.0f, 1, 77.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11025, 11024, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11025, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->objectAdd(kObjectMuleTooth, "donkey's tooth", "atooth", 1);
	_app->objectAddRotationAccessibility(kObjectMuleTooth, 11022, Common::Rect(1214, 355, 1267, 422), false, kCursorTake, 0);
	_app->objectAdd(kObjectMuleToothPowder, "donkey's tooth powder", "powder", 1);
	_app->objectAddRotationAccessibility(kObjectMuleToothPowder, 11023, Common::Rect(3171, 413, 3480, 588), false, kCursorTake, 0);
	_app->objectAdd(kObjectFabricAndFlour, "fabric and flour", "aflour", 1);
	_app->objectAddRotationAccessibility(kObjectFabricAndFlour, 11023, Common::Rect(3171, 413, 3480, 588), false, kCursorTake, 0);
	_app->objectAdd(kObjectBread, "bread", "bread", 1);
	_app->objectAddRotationAccessibility(kObjectBread, 11022, Common::Rect(2155, 187, 2337, 343), false, kCursorAction, 0);
	_app->objectAdd(kObject11001, "", "", 1);
	_app->objectAddRotationAccessibility(kObject11001, 11011, Common::Rect(1968, -227, 2436, 353), true, kCursorMove, 0);
	_app->objectAddPresentation(kObject11001);
	_app->objectPresentationAddImageToRotation(kObject11001, 0, 11011, 0);
	_app->objectAddPresentation(kObject11001);
	_app->objectPresentationAddImageToRotation(kObject11001, 1, 11022, 0);
	_app->objectAddPresentation(kObject11001);
	_app->objectPresentationAddImageToRotation(kObject11001, 2, 11022, 2);
	_app->objectAddPresentation(kObject11001);
	_app->objectPresentationAddImageToRotation(kObject11001, 3, 11022, 3);
	_app->objectAddPresentation(kObject11001);
	_app->objectPresentationAddImageToRotation(kObject11001, 4, 11022, 3);
	_app->objectAddPresentation(kObject11001);
	_app->objectAddPresentation(kObject11001);
	_app->objectPresentationAddImageToRotation(kObject11001, 6, 11023, 0);
	_app->objectAddPresentation(kObject11001);
	_app->objectPresentationAddImageToRotation(kObject11001, 7, 11023, 2);
	_app->objectAddPresentation(kObject11001);
	_app->objectPresentationAddAnimationToRotation(kObject11001, 8, 11011, 1, 10, 12.5, 16);
	_app->objectPresentationAddAnimationToRotation(kObject11001, 8, 11023, 1, 10, 12.5, 16);
	_app->objectPresentationAddAnimationToPuzzle(kObject11001, 8, kPuzzle110111, "s10a01n01p01s01", kImageTypeBMP, Common::Point(452, 31), kDrawTypeNormal, 1000, 29, 12.5, 16);
	_app->objectPresentationSetAnimationOnRotation(11001, 8, 0, 100001);
	_app->objectAddPresentation(kObject11001);
	_app->objectPresentationAddAnimationToRotation(kObject11001, 9, 11022, 1, 15, 12.5, 16);
	_app->objectPresentationSetAnimationOnRotation(11001, 9, 0, 100002);
	_app->objectPresentationAddAnimationToPuzzle(kObject11001, 9, kPuzzle110221, "s10a02n02p01s01", kImageTypeBMP, Common::Point(276, 144), kDrawTypeNormal, 1000, 29, 12.5f, 16);
	_app->objectPresentationSetAnimationOnPuzzle(11001, 9, 0, 100002);
	_app->objectAddPresentation(kObject11001);
	_app->objectPresentationAddAnimationToRotation(kObject11001, 10, 11022, 4, 15, 12.5, 16);
	_app->objectPresentationAddAnimationToPuzzle(kObject11001, 10, kPuzzle110222, "s10a02n02p02s01", kImageTypeBMP, Common::Point(296, 37), kDrawTypeNormal, 1000, 29, 12.5f, 16);
	_app->objectPresentationSetAnimationOnRotation(11001, 10, 0, 100002);
	_app->objectAddPresentation(kObject11001);
	_app->objectPresentationAddAnimationToPuzzle(kObject11001, 11, kPuzzle110111, "s10a01n01p01s02", kImageTypeBMP, Common::Point(42, 162), kDrawTypeNormal, 1000, 29, 12.5, 4);
	_app->objectPresentationSetAnimationOnPuzzle(11001, 11, 0, 100003);
	_app->objectAddPresentation(kObject11001);
	_app->objectPresentationShow(kObject11001, 12);
	_app->objectAdd(kObject11004, "", "", 1);
	_app->objectAddRotationAccessibility(kObject11004, 11023, Common::Rect(3306,  19, 3600, 147), false, kCursorActionObject, 0);
	_app->objectAddRotationAccessibility(kObject11004, 11023, Common::Rect(   0,  19,   22, 147), false, kCursorActionObject, 0);
	_app->objectAdd(kObject11005, "", "", 1);
	_app->objectAddRotationAccessibility(kObject11005, 11023, Common::Rect(  94, 147,  221, 250), false, kCursorAction, 0);
	_app->objectAdd(kObject11006, "", "", 1);
	_app->objectAddRotationAccessibility(kObject11006, 11022, Common::Rect(3028,   5, 3130, 273), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject11006, 11022, Common::Rect(3028,   5, 3130, 273), false, kCursorAction, 1);
	_app->objectAddRotationAccessibility(kObject11006, 11022, Common::Rect(3028,   5, 3130, 273), false, kCursorIdle, 2);
	_app->objectAddRotationAccessibility(kObject11006, 11022, Common::Rect(3028,   5, 3130, 273), false, kCursorActionObject, 3);
	_app->objectAddRotationAccessibility(kObject11006, 11022, Common::Rect(3028,   5, 3130, 273), false, kCursorAction, 4);
	_app->objectAddRotationAccessibility(kObject11006, 11022, Common::Rect(3028,   5, 3130, 273), false, kCursorAction, 5);
	_app->objectAddRotationAccessibility(kObject11006, 11022, Common::Rect(3028,   5, 3130, 273), false, kCursorAction, 6);
	_app->objectAddRotationAccessibility(kObject11006, 11022, Common::Rect(3028,   5, 3130, 273), false, kCursorAction, 7);
	_app->objectAddRotationAccessibility(kObject11006, 11022, Common::Rect(3028,   5, 3130, 273), false, kCursorAction, 8);
	_app->objectAdd(kObject11002, "", "", 1);
	_app->objectAddRotationAccessibility(kObject11002, 11011, Common::Rect(3257, -10, 3410, 255), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject11002, 11011, Common::Rect(3257, -10, 3410, 255), false, kCursorAction, 1);
	_app->objectAddRotationAccessibility(kObject11002, 11011, Common::Rect(3257, -10, 3410, 255), false, kCursorIdle, 2);
	_app->soundAdd(100001, kSoundTypeAmbientEffect, "10_PUT-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(100003, kSoundTypeAmbientEffect, "10_THR-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(100004, kSoundTypeAmbientEffect, "10_THR-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(100005, kSoundTypeAmbientEffect, "10_TAK-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(100006, kSoundTypeAmbientEffect, "10_TAK-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(2062,   kSoundTypeDialog,        "2062.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4053,   kSoundTypeDialog,        "4053.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4054,   kSoundTypeDialog,        "4054.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4055,   kSoundTypeDialog,        "4055.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4056,   kSoundTypeDialog,        "4056.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4057,   kSoundTypeDialog,        "4057.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4058,   kSoundTypeDialog,        "4058.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4059,   kSoundTypeDialog,        "4059.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4060,   kSoundTypeDialog,        "4060.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4061,   kSoundTypeDialog,        "4061.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4062,   kSoundTypeDialog,        "4062.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4063,   kSoundTypeDialog,        "4063.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4064,   kSoundTypeDialog,        "4064.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4065,   kSoundTypeDialog,        "4065.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4066,   kSoundTypeDialog,        "4066.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4067,   kSoundTypeDialog,        "4067.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4068,   kSoundTypeDialog,        "4068.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4069,   kSoundTypeDialog,        "4069.wac",     _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(100002, kSoundTypeAmbientMusic,  "10_AMB-1.WAs", _app->getConfiguration().backgroundMusic.loadFrom);
	_app->rotationAddAmbientSound(11011, 100002, 80, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(11021, 100002, 80, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(11022, 100002, 85, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(11023, 100002, 75, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(11024, 100002, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(11025, 100002, 85, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle110111, 100002, 80, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle110221, 100002, 85, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle110222, 100002, 85, 0, 1, 1, 10);
	_app->varDefineByte(100001, 0);
}

void Zone11Pompeii::onButtonUp(ObjectId id, Id target, Id /*puzzleRotationId*/, uint32 /*a4*/, const Common::Point &/*point*/) {
	switch (id) {
	default:
		break;

	case kObject11001:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->objectPresentationPauseAnimation(kObject11001, 8);
			_app->objectPresentationPauseAnimation(kObject11001, 9);
			_app->objectPresentationPauseAnimation(kObject11001, 10);
			_app->objectPresentationPauseAnimation(kObject11001, 11);
			_app->timerStopAll();
			_app->onCall(10500);
		}
		break;

	case kObject11002:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			switch (target) {
			default:
				break;

			case 0:
				_app->objectSetAccessibilityOff(kObject11002);
				_app->soundPlay(2062);
				break;

			case 1:
				_app->varSetByte(90408, 1);
				_app->objectSetAccessibilityOff(kObject11002);
				_app->puzzleSetActive(kPuzzle110111);
				_app->soundPlay(4053);
				break;
			}
		}
		break;
	case kObject11006:
		if (_app->bagHasClickedObject()) {
			if (target == 3 && _app->bagGetClickedObject() == kObjectFabricAndFlour) {
				_app->bagRemove(kObjectFabricAndFlour);
				_app->rotationSetFreOn(11022);
				_app->soundPlay(100004);
				_app->objectSetAccessibilityOff(kObject11006);
				_app->objectPresentationHide(kObject11001, 2);
				_app->objectPresentationHide(kObject11001, 9);
				_app->objectPresentationShow(kObject11001, 3);
				_app->objectPresentationShow(kObject11001, 10);

				handleEvents();

				_app->puzzleSetActive(kPuzzle110222);
				_app->soundPlay(4067);
			}

			_app->cursorDelete();
		} else {
			switch (target) {
			default:
				break;

			case 0:
				_app->objectSetAccessibilityOff(kObject11006);
				_app->puzzleSetActive(kPuzzle110221);
				_app->soundPlay(4060);
				break;

			case 1:
				_app->varSetByte(90408, 2);
				_app->objectSetAccessibilityOff(kObject11006);
				_app->varSetByte(100001, 0);
				_app->puzzleSetActive(kPuzzle110221);
				_app->soundPlay(4061);
				break;
			}
		}
		break;
	case kObject11005:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->objectSetAccessibilityOff(kObject11005);
			_app->objectSetAccessibilityOn(kObjectMuleToothPowder);
		}
		break;
	case kObject11004:
		if (_app->bagHasClickedObject()) {
			if (_app->bagGetClickedObject() == kObjectMuleTooth) {
				_app->soundPlay(100001);
				_app->objectSetAccessibilityOff(kObject11004);
				_app->bagRemove(kObjectMuleTooth);
			}

			_app->cursorDelete();
		}
		break;

	case kObjectExitEncyclopedia:
		_app->restore();
		break;

	case kObjectMuleTooth:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->varSetByte(90213, 1);
			_app->soundPlay(100006);
			_app->objectSetAccessibilityOff(kObjectMuleTooth);
			_app->objectPresentationHide(kObject11001, 1);
			_app->bagAdd(kObjectMuleTooth);
		}
		break;

	case kObjectFabricAndFlour:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->soundPlay(100005);
			_app->objectSetAccessibilityOff(kObjectFabricAndFlour);
			_app->objectPresentationHide(kObject11001, 7);
			_app->bagAdd(kObjectFabricAndFlour);
		}
		break;

	case kObjectBread:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->objectSetAccessibilityOff(kObjectBread);
			_app->objectPresentationHide(kObject11001, 7);
			_app->bagAdd(kObjectBread);
			_app->soundPlay(4068);
		}
		break;

	case kObjectMuleToothPowder:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->soundPlay(100005);
			_app->objectSetAccessibilityOff(kObjectMuleToothPowder);
			_app->objectPresentationHide(kObject11001, 7);
			_app->bagAdd(kObjectMuleToothPowder);
			_app->varSetByte(90213, 2);
		}
		break;
	}
}

void Zone11Pompeii::onTimer(TimerId id) {
	switch (id) {
	default:
		break;

	case kTimer0:
		_app->timerStop(kTimer0);
		_app->objectPresentationUnpauseAnimation(kObject11001, 8);
		_app->objectPresentationUnpauseAnimation(kObject11001, 9);
		_app->objectPresentationUnpauseAnimation(kObject11001, 10);
		break;

	case kTimer1:
		_app->timerStop(kTimer1);
		_app->objectPresentationUnpauseAnimation(kObject11001, 11);
		break;
	}
}

void Zone11Pompeii::onAnimationNextFrame(Id animationId, const Common::String &/*name*/, uint32 frame, uint32 frameCount) {
	switch (animationId) {
	default:
		break;

	case 100001:
	case 100002:
		if (frame == frameCount) {
			_app->objectPresentationPauseAnimation(kObject11001, 8);
			_app->objectPresentationPauseAnimation(kObject11001, 9);
			_app->objectPresentationPauseAnimation(kObject11001, 10);
			_app->timerStart(kTimer0, rnd(3000) + 3000);
		}
		break;

	case 100003:
		if (frame == frameCount) {
			_app->objectPresentationPauseAnimation(kObject11001, 11);
			_app->timerStart(kTimer1, rnd(3000) + 3000);
		}
		break;
	}
}

void Zone11Pompeii::onSound(Id id, SoundType /*type*/, uint32 /*a3*/, bool /*process*/) {
	switch (id) {
	default:
		break;

	 case 4053:
		_app->rotationSetAlp(11011, 286.0f);
		_app->rotationSetBet(11011, 15.0f);
		_app->rotationSetActive(11011);
		_hideBox = false;
		_app->varSetByte(100001, 0);
		_app->objectSetAccessibilityOn(kObject11002, 2, 2);
		break;

	case 4054:
	case 4055:
	case 4057:
		_app->varSetByte(100001, _app->varGetByte(100001) + 1);
		// Fallback to next case

	case 4056:
	case 4058:
	case 4059:
		_app->rotationSetAlp(11011, 286.0f);
		_app->rotationSetBet(11011, 15.0f);
		_app->rotationSetActive(11011);
		if (_app->varGetByte(100001) == 3)
			_app->rotationSetMovabilityOn(11011);
		break;

	case 4060:
		_app->rotationSetActive(11022);
		break;

	case 4061:
		_hideBox = false;
		_app->rotationSetAlp(11022, 266.0f);
		_app->rotationSetBet(11022, 12.0f);
		_app->rotationSetActive(11022);
		_app->objectSetAccessibilityOn(kObject11006, 2, 2);
		break;

	case 4062:
	case 4063:
	case 4064:
		_app->varSetByte(100001, _app->varGetByte(100001) + 1);
		// Fallback to next case

	case 4065:
	case 4066:
		_app->rotationSetAlp(11022, 266.0f);
		_app->rotationSetBet(11022, 12.0f);
		_app->rotationSetActive(11022);
		if (_app->varGetByte(100001) == 3) {
			_app->objectSetAccessibilityOff(kObject11006);
			_app->objectSetAccessibilityOn(kObject11006, 3, 3);
		}
		break;

	case 4067:
		_app->soundPlay(4068);
		break;

	case 4068:
		_app->soundPlay(4069);
		break;

	case 4069:
		_app->playMovie("S10A02-1");
		_app->objectPresentationHide(kObject11001, 3);
		_app->objectPresentationHide(kObject11001, 10);
		_app->rotationSetAlp(11022, 266.0f);
		_app->rotationSetBet(11022, 12.0f);
		_app->rotationSetActive(11022);
		_app->rotationSetFreOff(11022);
		_app->varSetByte(90408, 2);
		break;

	case 100001:
		_app->playMovie("S10A02-2");
		_app->objectPresentationHide(kObject11001, 6);
		_app->objectPresentationShow(kObject11001, 7);
		_app->objectSetAccessibilityOn(kObjectMuleToothPowder);
		break;
	}
}

void Zone11Pompeii::onUpdateBag(const Common::Point &/*point*/) {
	if (_hideBox) {
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;
	}
}

void Zone11Pompeii::onUpdateBefore(Id movabilityFrom, Id movabilityTo, uint32 /*movabilityIndex*/, Id /*target*/, const Common::Point &/*point*/) {
	Common::Point mouse = g_system->getEventManager()->getMousePos() - Common::Point(20, 16);

	if (movabilityFrom == 11002
	 && movabilityTo   == 2
	 && !_hideBox
	 && !_app->bagHasClickedObject()) {
		_app->visualBoxSetParameters(6, kPuzzleMenu, 4053, mouse);
		_hideBox = true;
	}

	if (movabilityFrom == 11006
	 && (movabilityTo == 2 || movabilityTo == 3)
	 && !_hideBox
	 && !_app->bagHasClickedObject()) {
		_app->visualBoxSetParameters(6, kPuzzleMenu, 4061, mouse);
		_hideBox = true;
	}
}

void Zone11Pompeii::onUpdateAfter(Id /*movabilityFrom*/, Id /*movabilityTo*/, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType /*movabilityType*/, const Common::Point &/*point*/) {
	if (_hideBox) {
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;
	}
}

void Zone11Pompeii::onAfterRide(Id movabilityFrom, Id movabilityTo, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType movabilityType) {
	if (movabilityType == kMovabilityRotationToRotation
	 && movabilityFrom == 11022
	 && movabilityTo == 11024
	 && _app->varGetByte(90408) == 1) {
		if (_app->varGetByte(90407) == 2) {
			_app->varSetByte(90407, 3);
			_app->objectSetAccessibilityOn(kObject11006, 1, 1);
		} else if (_app->varGetByte(90407) < 2) {
			_app->objectSetAccessibilityOn(kObject11006, 0, 0);
		}
	}
}

void Zone11Pompeii::onVisual(int x) {
	switch (x) {
	default:
		break;

	case 4053:
		_app->objectSetAccessibilityOff(kObject11002);
		break;

	case 4054:
	case 4055:
	case 4056:
	case 4057:
	case 4058:
	case 4059:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle110111);
		break;

	case 4062:
	case 4063:
	case 4064:
	case 4065:
	case 4066:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle110221);
		break;
	}
}

} // End of namespace Ring
