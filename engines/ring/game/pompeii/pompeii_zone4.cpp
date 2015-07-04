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

#include "ring/game/pompeii/pompeii_zone4.h"

#include "ring/game/pompeii/pompeii_application.h"
#include "ring/game/pompeii/pompeii_shared.h"

#include "ring/debug.h"
#include "ring/ring.h"
#include "ring/helpers.h"

using namespace PompeiiGame;

namespace Ring {

Zone4Pompeii::Zone4Pompeii(ApplicationPompeii *application) : _app(application) {
	_hideBox = false;
}

Zone4Pompeii::~Zone4Pompeii() {
	// Zero-out passed pointers
	_app = nullptr;
}

void Zone4Pompeii::onInit() {
	_app->setCurrentZone(kZone4);

	_app->puzzleAdd(kPuzzle30311);
	_app->puzzleAddBackgroundImage(kPuzzle30311, "s03a01n01p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle30312);
	_app->puzzleAddBackgroundImage(kPuzzle30312, "s03a01n01p02.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle30313);
	_app->puzzleAddBackgroundImage(kPuzzle30313, "s03a01n01p03.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle30314);
	_app->puzzleAddBackgroundImage(kPuzzle30314, "s03a01n01p04.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle30317);
	_app->puzzleAddBackgroundImage(kPuzzle30317, "s03a01n01p07.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle30321);
	_app->puzzleAddBackgroundImage(kPuzzle30321, "s03a01n01p99.bmp", Common::Point(0, 16), true);
	_app->rotationAdd(10301, "S03A01-5", 0, 6);
	_app->rotationAdd(10302, "S03A01-4", 0, 3);
	_app->rotationAdd(10303, "S03A01-3", 0, 0);
	_app->rotationAdd(10304, "S03A01-2", 0, 0);
	_app->rotationAdd(10305, "S03A01-1", 0, 0);
	_app->rotationAddMovabilityToRotation(10301, 10302, "", Common::Rect(1330, -250, 1480, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10301, 0, 95.0f, 0.30000001f, 87.0f, 0.0f, 1, 95.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10302, 10301, "", Common::Rect(3130, -250, 3280, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10302, 0, 275.0f, 0.30000001f, 87.0f, 0.0f, 1, 275.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10302, 10303, "", Common::Rect(440, -250, 590, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10302, 1, 6.0f, 0.30000001f, 87.0f, 0.0f, 1, 6.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10303, 10302, "", Common::Rect(2240, -250, 2390, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10303, 0, 186.0f, 0.30000001f, 87.0f, 0.0f, 1, 186.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10303, 10304, "", Common::Rect(450, -250, 600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10303, 1, 7.0f, 0.30000001f, 87.0f, 0.0f, 1, 7.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10304, 10303, "", Common::Rect(2250, -250, 2400, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10304, 0, 187.0f, 0.30000001f, 87.0f, 0.0f, 1, 187.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10304, 10305, "", Common::Rect(3130, -250, 3280, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10304, 1, 275.0f, 0.30000001f, 87.0f, 0.0f, 1, 275.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10305, 10304, "", Common::Rect(1330, -250, 1480, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10305, 0, 95.0f, 0.30000001f, 87.0f, 0.0f, 1, 95.0f, 0.30000001f, 87.0f);
	_app->puzzleAddMovabilityToRotation(kPuzzle30311, 10301, "", Common::Rect(0, 420, 640, 464), false, kCursorBack, 0);
	_app->puzzleSetMovabilityToRotation(kPuzzle30311, 0, 270.0f, 0, 87.0f);
	_app->puzzleAddMovabilityToRotation(kPuzzle30312, 10301, "", Common::Rect(0, 420, 640, 464), false, kCursorBack, 0);
	_app->puzzleAddMovabilityToRotation(kPuzzle30313, 10301, "", Common::Rect(0, 420, 640, 464), false, kCursorBack, 0);
	_app->puzzleSetMovabilityToRotation(kPuzzle30313, 0, 270.0f, 0, 87.0f);
	_app->objectAdd(kObject30301, "", "", 1);
	_app->objectAddRotationAccessibility(kObject30301, 10301, Common::Rect(2032, -327, 2497, 262), true, kCursorMove, 0);
	_app->objectAddPresentation(kObject30301);
	_app->objectPresentationAddImageToRotation(kObject30301, 0, 10301, 0);
	_app->objectPresentationAddImageToRotation(kObject30301, 0, 10302, 0);
	_app->objectAddPresentation(kObject30301);
	_app->objectPresentationAddImageToRotation(kObject30301, 1, 10301, 2);
	_app->objectPresentationAddImageToRotation(kObject30301, 1, 10302, 1);
	_app->objectPresentationAddAnimationToPuzzle(kObject30301, 1, kPuzzle30311, "s03a01n01p01s01", kImageTypeBMP, Common::Point(119, 93), kDrawTypeNormal, 1000, 28, 12.5, 16);
	_app->objectPresentationSetAnimationOnPuzzle(30301, 1, 0, 30001);
	_app->objectAddPresentation(kObject30301);
	_app->objectPresentationAddImageToRotation(kObject30301, 2, 10301, 3);
	_app->objectPresentationAddAnimationToPuzzle(kObject30301, 2, kPuzzle30313, "s03a01n01p03s01", kImageTypeBMP, Common::Point(52, 118), kDrawTypeNormal, 1000, 28, 12.5, 16);
	_app->objectPresentationSetAnimationOnPuzzle(30301, 2, 0, 30001);
	_app->objectAddPresentation(kObject30301);
	_app->objectPresentationAddImageToRotation(kObject30301, 3, 10301, 4);
	_app->objectPresentationAddImageToRotation(kObject30301, 3, 10302, 2);
	_app->objectPresentationAddAnimationToPuzzle(kObject30301, 3, kPuzzle30314, "s03a01n01p04s01", kImageTypeBMP, Common::Point(57, 31), kDrawTypeNormal, 1000, 28, 12.5, 16);
	_app->objectPresentationSetAnimationOnPuzzle(30301, 3, 0, 30001);
	_app->objectAddPresentation(kObject30301);
	_app->objectPresentationAddImageToRotation(kObject30301, 4, 10301, 5);
	_app->objectPresentationAddAnimationToPuzzle(kObject30301, 4, kPuzzle30317, "s03a01n01p07s01", kImageTypeBMP, Common::Point(243, 108), kDrawTypeNormal, 1000, 28, 12.5, 16);
	_app->objectPresentationSetAnimationOnPuzzle(30301, 4, 0, 30001);
	_app->objectAddPresentation(kObject30301);
	_app->objectPresentationAddAnimationToPuzzle(kObject30301, 5, kPuzzle30311, "s03a01n01p01s02", kImageTypeBMP, Common::Point(329, 102), kDrawTypeNormal, 1000, 28, 12.5, 16);
	_app->objectPresentationSetAnimationOnPuzzle(30301, 5, 0, 30002);
	_app->objectAddPresentation(kObject30301);
	_app->objectPresentationAddAnimationToPuzzle(kObject30301, 6, kPuzzle30312, "s03a01n01p02s01", kImageTypeBMP, Common::Point(313, 78), kDrawTypeNormal, 1000, 28, 12.5, 16);
	_app->objectPresentationSetAnimationOnPuzzle(30301, 6, 0, 30001);
	_app->objectAddPresentation(kObject30301);
	_app->objectPresentationAddAnimationToPuzzle(kObject30301, 7, kPuzzle30313, "s03a01n01p03s02", kImageTypeBMP, Common::Point(382, 62), kDrawTypeNormal, 1000, 28, 12.5, 16);
	_app->objectPresentationSetAnimationOnPuzzle(30301, 7, 0, 30002);
	_app->objectAddPresentation(kObject30301);
	_app->objectPresentationAddAnimationToPuzzle(kObject30301, 8, kPuzzle30314, "s03a01n01p04s02", kImageTypeBMP, Common::Point(365, 103), kDrawTypeNormal, 1000, 28, 12.5, 16);
	_app->objectPresentationSetAnimationOnPuzzle(30301, 8, 0, 30002);
	_app->objectAddPresentation(kObject30301);
	_app->objectPresentationAddAnimationToPuzzle(kObject30301, 9, kPuzzle30317, "s03a01n01p07s02", kImageTypeBMP, Common::Point(354, 166), kDrawTypeNormal, 1000, 28, 12.5, 16);
	_app->objectPresentationSetAnimationOnPuzzle(30301, 9, 0, 30002);
	_app->objectAdd(kObjectKnife, "Knife", "knife", 1);
	_app->objectAdd(kObject30311, "", "", 1);
	_app->objectAddRotationAccessibility(kObject30311, 10301, Common::Rect(3004, 10, 3074, 164), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject30311, 10301, Common::Rect(3206, 42, 3287, 168), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject30311, 10301, Common::Rect(3345, 24, 3422, 201), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject30311, 10301, Common::Rect(3004, 10, 3074, 164), false, kCursorAction, 1);
	_app->objectAddRotationAccessibility(kObject30311, 10301, Common::Rect(3206, 42, 3287, 168), false, kCursorAction, 1);
	_app->objectAdd(kObject30312, "", "", 1);
	_app->objectAddRotationAccessibility(kObject30312, 10301, Common::Rect(3190, 29, 3292, 173), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject30312, 10301, Common::Rect(3190, 29, 3292, 173), false, kCursorAction, 1);
	_app->objectAdd(kObject30313, "", "", 1);
	_app->objectAddRotationAccessibility(kObject30313, 10301, Common::Rect(3345, 24, 3422, 201), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject30313, 10301, Common::Rect(3345, 24, 3422, 201), false, kCursorAction, 1);
	_app->objectAdd(kObject30315, "", "", 1);
	_app->objectAddRotationAccessibility(kObject30315, 10301, Common::Rect(3083, 52, 3206, 292), false, kCursorIdle, 0);
	_app->objectAdd(kObject30314, "", "", 1);
	_app->objectAddRotationAccessibility(kObject30314, 10301, Common::Rect(3055, 159, 3185, 311), false, kCursorActionObject, 0);
	_app->soundAdd(1044,  kSoundTypeDialog, "1044.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1045,  kSoundTypeDialog, "1045.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1049,  kSoundTypeDialog, "1049.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1050,  kSoundTypeDialog, "1050.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1051,  kSoundTypeDialog, "1051.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(10511, kSoundTypeDialog, "1051_1.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(10512, kSoundTypeDialog, "1051_2.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(10513, kSoundTypeDialog, "1051_3.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1052,  kSoundTypeDialog, "1052.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1053,  kSoundTypeDialog, "1053.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1054,  kSoundTypeDialog, "1054.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1055,  kSoundTypeDialog, "1055.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1056,  kSoundTypeDialog, "1056.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1057,  kSoundTypeDialog, "1057.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1058,  kSoundTypeDialog, "1058.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1059,  kSoundTypeDialog, "1059.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1060,  kSoundTypeDialog, "1060.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1061,  kSoundTypeDialog, "1061.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1062,  kSoundTypeDialog, "1062.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1063,  kSoundTypeDialog, "1063.wac",   _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(30000, kSoundTypeAmbientMusic, "03_BGR-1.WAc", _app->getConfiguration().backgroundMusic.loadFrom);
	_app->rotationAddAmbientSound(10301, 30000, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10302, 30000, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10303, 30000, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10304, 30000, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10305, 30000, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle30311, 30000, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle30312, 30000, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle30313, 30000, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle30314, 30000, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle30317, 30000, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle30321, 30000, 90, 0, 1, 1, 10);
	_app->varDefineByte(30001, 0);
	_app->varDefineByte(30002, 0);
	_app->varDefineByte(30003, 0);
}

void Zone4Pompeii::onSetup(SetupType type) {
	switch (type) {
	default:
		break;

	case kSetupType999:
		_app->timerStopAll();
		_app->bagRemoveAll();
		_app->setZone(kZone2, kSetupType200);
		break;
	}
}

void Zone4Pompeii::onButtonUp(ObjectId id, Id target, Id /*puzzleRotationId*/, uint32 /*a4*/, const Common::Point &/*point*/) {
	switch (id) {
	default:
		break;

	case kObject30301:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->objectPresentationPauseAnimation(kObject30301, 5);
			_app->objectPresentationPauseAnimation(kObject30301, 7);
			_app->objectPresentationPauseAnimation(kObject30301, 8);
			_app->objectPresentationPauseAnimation(kObject30301, 9);
			_app->objectPresentationPauseAnimation(kObject30301, 1);
			_app->objectPresentationPauseAnimation(kObject30301, 2);
			_app->objectPresentationPauseAnimation(kObject30301, 3);
			_app->objectPresentationPauseAnimation(kObject30301, 4);
			_app->objectPresentationPauseAnimation(kObject30301, 6);
			_app->timerStopAll();
			_app->onCall(10300);
		}
		break;

	case kObject30311:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			switch (target) {
			default:
				break;

			case 0:
				if (!_app->varGetByte(90107)) {
					_app->varSetByte(90107, 1);
					_app->puzzleSetActive(kPuzzle30313);
					_app->objectSetAccessibilityOff(kObject30311);
					_app->soundPlay(1044);
				}
				break;

			case 1:
				_app->puzzleSetActive(kPuzzle30311);
				if (!_app->varGetByte(90109)) {
					_app->varSetByte(90109, 1);
					_app->varSetByte(30002, 1);
					_app->soundPlay(1049);
					_app->objectSetAccessibilityOff(kObject30311);
					_app->objectSetAccessibilityOn(kObject30314);
				}
				break;
			}
		}
		break;

	case kObject30312:
	case kObject30315:
		if (_app->bagHasClickedObject())
			_app->cursorDelete();
		break;

	case kObject30313:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target) {
				_app->soundPlay(1045);
				_app->objectSetAccessibilityOff(kObject30313);
			}
		}
		break;

	case kObject30314:
		if (_app->bagHasClickedObject()) {

			if (_app->bagGetClickedObject() == kObjectKnife) {
				_app->objectSetAccessibilityOff(kObject30314);
				_app->soundPlay(1050);
				_app->objectSetAccessibilityOff(kObject30301);
			}

			_app->cursorDelete();
		}
		break;

	case kObjectExitEncyclopedia:
		_app->restore();
	}
}

void Zone4Pompeii::onTimer(TimerId id) {
	switch (id) {
	default:
		break;

	case kTimer0:
		_app->timerStop(kTimer0);
		_app->objectPresentationUnpauseAnimation(kObject30301, 1);
		_app->objectPresentationUnpauseAnimation(kObject30301, 2);
		_app->objectPresentationUnpauseAnimation(kObject30301, 3);
		_app->objectPresentationUnpauseAnimation(kObject30301, 4);
		_app->objectPresentationUnpauseAnimation(kObject30301, 6);
		break;

	case kTimer1:
		_app->timerStop(kTimer1);
		_app->objectPresentationUnpauseAnimation(kObject30301, 5);
		_app->objectPresentationUnpauseAnimation(kObject30301, 7);
		_app->objectPresentationUnpauseAnimation(kObject30301, 8);
		_app->objectPresentationUnpauseAnimation(kObject30301, 9);
		break;
	}
}

void Zone4Pompeii::onAnimationNextFrame(Id animationId, const Common::String &/*name*/, uint32 frame, uint32 frameCount) {
	switch (animationId) {
	default:
		break;

	case 30001:
		if (frame != frameCount)
			break;

		_app->objectPresentationPauseAnimation(kObject30301, 1);
		_app->objectPresentationPauseAnimation(kObject30301, 2);
		_app->objectPresentationPauseAnimation(kObject30301, 3);
		_app->objectPresentationPauseAnimation(kObject30301, 4);
		_app->objectPresentationPauseAnimation(kObject30301, 6);
		_app->timerStart(kTimer0, 1000 * (rnd(5) + 2));
		break;

	case 30002:
		if (frame != frameCount)
			break;

		_app->objectPresentationPauseAnimation(kObject30301, 5);
		_app->objectPresentationPauseAnimation(kObject30301, 7);
		_app->objectPresentationPauseAnimation(kObject30301, 8);
		_app->objectPresentationPauseAnimation(kObject30301, 9);
		_app->timerStart(kTimer1, 1000 * (rnd(5) + 2));
		break;
	}
}

void Zone4Pompeii::onSound(Id id, SoundType /*type*/, uint32 /*a3*/, bool /*process*/) {
	switch (id) {
	default:
		break;

	case 1044:
		_app->rotationSetActive(10301);
		_app->objectSetAccessibilityOn(kObject30313, 0, 0);
		_app->objectSetAccessibilityOn(kObject30311, 3, 4);
		break;

	case 1049:
		_app->rotationSetActive(10301);
		_app->objectSetAccessibilityOn(kObject30314);
		break;

	case 1050:
		_app->playMovie("S03A01-1");
		_app->puzzleSetActive(kPuzzle30313);
		_app->soundPlay(10511);
		break;

	case 1052:
	case 1053:
	case 1055:
	case 1056:
	case 1057:
	case 1058:
	case 1059:
	case 1060:
	case 1061:
	case 1062:
		_hideBox = false;
		_app->rotationSetAlp(10301, 270.0f);
		_app->rotationSetBet(10301, 14.0f);
		_app->rotationSetActive(10301);
		break;

	case 10512:
		_app->playMovie("S03A01-4");
		_app->puzzleSetActive(kPuzzle30314);
		_app->soundPlay(10513);
		break;

	case 1063:
		_app->objectPresentationHide(kObject30301);
		_app->playMovie("S03A01-2");
		_app->rotationSetActive(10301);
		_app->objectSetAccessibilityOn(kObject30301);
		_app->onCall(1091);
		break;

	case 10511:
		_app->rotationSetActive(10301);
		_app->objectSetAccessibilityOff(kObject30313);
		_app->onCall(109);
		_app->playMovie("S03A01-3");
		_app->puzzleSetActive(kPuzzle30321);
		_app->soundPlay(10512);
		break;

	case 10513:
		_app->rotationSetAlp(10301, 270.0f);
		_app->rotationSetBet(10301, 14.0f);
		_app->rotationSetActive(10301);
		_app->objectSetAccessibilityOn(kObject30315);
		_app->varSetByte(30003, 1);
		_hideBox = false;
		break;
	}
}

void Zone4Pompeii::onUpdateBag(const Common::Point &/*point*/) {
	if (_hideBox) {
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;
	}
}

void Zone4Pompeii::onUpdateBefore(Id movabilityFrom, Id /*movabilityTo*/, uint32 /*movabilityIndex*/, Id /*target*/, const Common::Point &/*point*/) {
	if (movabilityFrom == 30315
	 && !_hideBox
	 && !_app->bagHasClickedObject()) {
		_app->visualBoxSetParameters(6, kPuzzleMenu, 1051, g_system->getEventManager()->getMousePos() - Common::Point(20, 16));
		_hideBox = true;
	}
}

void Zone4Pompeii::onUpdateAfter(Id /*movabilityFrom*/, Id /*movabilityTo*/, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType /*movabilityType*/, const Common::Point &/*point*/) {
	if (_hideBox) {
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;
	}
}

void Zone4Pompeii::onVisual(int x) {
	switch (x) {
	default:
		break;

	case 1051:
		_app->varSetByte(90107, 3);
		break;

	case 1052:
	case 1053:
	case 1055:
	case 1056:
	case 1057:
	case 1058:
	case 1059:
	case 1060:
	case 1061:
	case 1062:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle30314);
		break;

	case 101051:
		_app->puzzleSetActive(kPuzzle30317);
		_app->soundPlay(1063);
		_app->objectSetAccessibilityOff(kObject30315);
		_app->varSetByte(90107, 3);
		break;
	}
}

} // End of namespace Ring
