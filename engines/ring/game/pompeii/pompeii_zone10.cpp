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

#include "ring/game/pompeii/pompeii_zone10.h"

#include "ring/game/pompeii/pompeii_application.h"
#include "ring/game/pompeii/pompeii_shared.h"

#include "ring/debug.h"
#include "ring/ring.h"
#include "ring/helpers.h"

using namespace PompeiiGame;

namespace Ring {

Zone10Pompeii::Zone10Pompeii(ApplicationPompeii *application) : _app(application) {
}

Zone10Pompeii::~Zone10Pompeii() {
	// Zero-out passed pointers
	_app = nullptr;
}

void Zone10Pompeii::onInit() {
	_app->setCurrentZone(kZone10);

	_app->rotationAdd(10911, "S09A01-3", 0, 4);
	_app->rotationSetCompressionBufferLength(10911, 1100000);
	_app->rotationAdd(10912, "S09A01-2", 0, 4);
	_app->rotationAdd(10913, "S09A01-1", 0, 4);
	_app->rotationAdd(10921, "S09A02-5", 0, 4);
	_app->rotationSetCompressionBufferLength(10921, 1600000);
	_app->rotationAdd(10922, "S09A02-4", 0, 6);
	_app->rotationSetCompressionBufferLength(10922, 1200000);
	_app->rotationAdd(10923, "S09A02-3", 0, 5);
	_app->rotationSetCompressionBufferLength(10923, 1500000);
	_app->rotationAdd(10924, "S09A02-2", 0, 4);
	_app->rotationSetCompressionBufferLength(10924, 1500000);
	_app->rotationAdd(10925, "S09A02-1", 0, 4);
	_app->rotationSetCompressionBufferLength(10925, 1600000);
	_app->rotationAdd(10931, "S09A03-2", 0, 4);
	_app->rotationSetCompressionBufferLength(10931, 1200000);
	_app->rotationAdd(10932, "S09A03-1", 0, 3);
	_app->rotationAddMovabilityToRotation(10911, 10913, "", Common::Rect(1500, -250, 1650, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10911, 0, 112.0f, 0.30000001f, 87.0f, 0.0f, 1, 112.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10911, 10925, "", Common::Rect(3580, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10911, 1, 320.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10911, 10925, "", Common::Rect(0, -250, 130, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10911, 2, 320.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10912, 10913, "", Common::Rect(3140, -250, 3290, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10912, 0, 276.0f, 0.30000001f, 87.0f, 0.0f, 1, 276.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10913, 10911, "", Common::Rect(3300, -250, 3450, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10913, 0, 292.0f, 0.30000001f, 87.0f, 0.0f, 1, 292.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10913, 10912, "", Common::Rect(1340, -250, 1490, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10913, 1, 96.0f, 0.30000001f, 87.0f, 0.0f, 1, 96.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10921, 10923, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10921, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10921, 10924, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10921, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10922, 10923, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10922, 0, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10922, 10932, "", Common::Rect(1280, -250, 1430, 250), false, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10922, 1, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10923, 10921, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10923, 0, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10923, 10922, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10923, 1, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10924, 10921, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10924, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10924, 10925, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10924, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10925, 10911, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10925, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 140.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10925, 10924, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10925, 1, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10931, 10932, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10931, 0, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10932, 10922, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10932, 0, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10932, 10931, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10932, 1, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->objectAdd(kObject10901, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10901, 10911, Common::Rect(2830, -50, 3322, 180), true, kCursorMove, 0);
	_app->objectAdd(kObject10902, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10902, 10911, Common::Rect(2156, -50, 2633, 180), true, kCursorMove, 0);
	_app->objectAdd(kObject10903, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10903, 10912, Common::Rect(1365, -50, 2174, 180), true, kCursorMove, 0);
	_app->objectAddPresentation(kObject10901);
	_app->objectPresentationAddImageToRotation(kObject10901, 0, 10911, 0);
	_app->objectPresentationAddImageToRotation(kObject10901, 0, 10912, 0);
	_app->objectPresentationAddImageToRotation(kObject10901, 0, 10913, 0);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 0, 10911, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 0, 10912, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 0, 10913, 1, 15, 12.5f, 16);
	_app->objectPresentationAddImageToRotation(kObject10901, 0, 10921, 2);
	_app->objectPresentationAddImageToRotation(kObject10901, 0, 10922, 0);
	_app->objectPresentationAddImageToRotation(kObject10901, 0, 10923, 2);
	_app->objectPresentationAddImageToRotation(kObject10901, 0, 10924, 2);
	_app->objectPresentationAddImageToRotation(kObject10901, 0, 10925, 2);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 0, 10921, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 0, 10922, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 0, 10923, 4, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 0, 10924, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 0, 10925, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 0, 10931, 3, 15, 12.5f, 16);
	_app->objectPresentationSetAnimationOnRotation(kObject10901, 0, 0, 90901);
	_app->objectAddPresentation(kObject10901);
	_app->objectPresentationAddImageToRotation(kObject10901, 1, 10922, 2);
	_app->objectPresentationAddImageToRotation(kObject10901, 1, 10932, 0);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 1, 10932, 1, 15, 12.5f, 16);
	_app->objectPresentationSetAnimationOnRotation(kObject10901, 0, 0, 90901);
	_app->objectAddPresentation(kObject10901);
	_app->objectAddPresentation(kObject10901);
	_app->objectAddPresentation(kObject10901);
	_app->objectPresentationAddImageToRotation(kObject10901, 4, 10911, 2);
	_app->objectPresentationAddImageToRotation(kObject10901, 4, 10912, 2);
	_app->objectPresentationAddImageToRotation(kObject10901, 4, 10913, 2);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 4, 10911, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 4, 10912, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 4, 10913, 3, 15, 12.5f, 16);
	_app->objectPresentationAddImageToRotation(kObject10901, 4, 10921, 0);
	_app->objectPresentationAddImageToRotation(kObject10901, 4, 10922, 4);
	_app->objectPresentationAddImageToRotation(kObject10901, 4, 10922, 3);
	_app->objectPresentationAddImageToRotation(kObject10901, 4, 10923, 0);
	_app->objectPresentationAddImageToRotation(kObject10901, 4, 10924, 0);
	_app->objectPresentationAddImageToRotation(kObject10901, 4, 10925, 0);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 4, 10921, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 4, 10922, 5, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 4, 10923, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 4, 10924, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 4, 10925, 1, 15, 12.5f, 16);
	_app->objectPresentationAddImageToRotation(kObject10901, 4, 10931, 0);
	_app->objectPresentationAddImageToRotation(kObject10901, 4, 10932, 2);
	_app->objectPresentationAddImageToRotation(kObject10901, 4, 10932, 0);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 4, 10931, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10901, 4, 10932, 1, 15, 12.5f, 16);
	_app->objectPresentationSetAnimationOnRotation(kObject10901, 4, 0, 90901);
	_app->objectAddPresentation(kObject10901);
	_app->objectPresentationAddImageToRotation(kObject10901, 5, 10931, 2);
	_app->objectAdd(kObjectWood, "wood", "wood", 9);
	_app->objectAddRotationAccessibility(kObjectWood, 10922, Common::Rect(1314, -248, 1615, 215), false, kCursorAction, 0);
	_app->objectAdd(kObject99902, "", "", 1);
	_app->objectAddRotationAccessibility(kObject99902, 10922, Common::Rect(3046, 5, 3176, 215), false, kCursorActionObject, 0);
	_app->objectAdd(kObject99903, "", "", 1);
	_app->objectAddRotationAccessibility(kObject99903, 10931, Common::Rect(1269, -183, 1420,   57), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject99903, 10931, Common::Rect(1153, -178, 1235, -105), false, kCursorAction, 1);
	_app->objectAddRotationAccessibility(kObject99903, 10931, Common::Rect(1466, -176, 1548,  -99), false, kCursorAction, 1);
	_app->objectAddRotationAccessibility(kObject99903, 10931, Common::Rect(1284,  159, 1404,  226), false, kCursorAction, 1);
	_app->objectAdd(kObject99904, "", "", 1);
	_app->objectAddRotationAccessibility(kObject99904, 10931, Common::Rect(1158, -22, 1233, 43), false, kCursorAction, 0);
	_app->soundAdd(90001, kSoundTypeAmbientEffect, "09_BOD-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(90002, kSoundTypeAmbientEffect, "09_CUT-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(3048, kSoundTypeDialog, "3048.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3049, kSoundTypeDialog, "3049.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3050, kSoundTypeDialog, "3050.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3051, kSoundTypeDialog, "3051.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3053, kSoundTypeDialog, "3053.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(99000, kSoundTypeAmbientMusic, "BGR_MU-1.WAc", _app->getConfiguration().backgroundMusic.loadFrom);
	_app->rotationAddAmbientSound(10911, 99000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10912, 99000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10913, 99000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10921, 99000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10922, 99000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10923, 99000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10924, 99000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10925, 99000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10931, 99000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10932, 99000, 100, 0, 1, 1, 10);
}

void Zone10Pompeii::onButtonUp(ObjectId id, Id target, Id /*puzzleRotationId*/, uint32 /*a4*/, const Common::Point &/*point*/) {
	switch (id) {
	default:
		break;

	case kObject10901:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (_app->varGetByte(90309) == 2) {
				_app->varSetByte(90309, 3);
				_app->onCall(3099);
			}

			_app->objectPresentationPauseAnimation(kObject10901, 0);
			_app->objectPresentationPauseAnimation(kObject10901, 1);
			_app->objectPresentationPauseAnimation(kObject10901, 4);
			_app->timerStopAll();
			_app->onCall(109001);
		}
		break;

	case kObject10902:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->objectPresentationPauseAnimation(kObject10901, 0);
			_app->objectPresentationPauseAnimation(kObject10901, 1);
			_app->objectPresentationPauseAnimation(kObject10901, 4);
			_app->timerStopAll();
			_app->onCall(1090012);
		}
		break;

	case kObject10903:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->objectPresentationPauseAnimation(kObject10901, 0);
			_app->objectPresentationPauseAnimation(kObject10901, 1);
			_app->objectPresentationPauseAnimation(kObject10901, 4);
			_app->timerStopAll();
			_app->onCall(109005);
		}
		break;

	case kObjectExitEncyclopedia:
		_app->restore();
		break;

	case kObject99902:
		if (_app->bagHasClickedObject()) {
			if (target == 0 && _app->bagGetClickedObject() == kObjectWood) {
				_app->objectSetAccessibilityOff(kObject99902);
				_app->objectSetAccessibilityOff(kObjectWood);
				_app->soundPlay(3049);
			}

			_app->cursorDelete();
		}
		break;

	case kObject99903:
		if (_app->bagHasClickedObject()) {
			if (_app->bagGetClickedObject() == kObjectKnife) {
				_app->soundPlay(90002);
				_app->objectSetAccessibilityOff(kObject99903);
				_app->objectPresentationHide(kObject10901, 0);
				_app->objectPresentationShow(kObject10901, 4);
				_app->varSetByte(90309, 2);
				_app->soundPlay(90001);
				_app->onCall(310);
				_app->onCall(311);
				_app->soundPlay(3048);
				_app->rotationSetMovabilityOff(10922, 1, 1);
			}

			_app->cursorDelete();
		} else {
			_app->objectSetAccessibilityOff(kObject99903);
			_app->soundPlay(3051);
			_app->rotationSetMovabilityOff(10931);
		}
		break;

	case kObject99904:
		_app->playMovie("S09A03-2");
		_app->exitToMenu(kMenuAction7);
		break;
	}
}

void Zone10Pompeii::onTimer(TimerId id) {
	if (id == kTimer0) {
		_app->timerStop(kTimer0);
		_app->objectPresentationUnpauseAnimation(kObject10901, 0);
		_app->objectPresentationUnpauseAnimation(kObject10901, 1);
		_app->objectPresentationUnpauseAnimation(kObject10901, 4);
	}
}

void Zone10Pompeii::onAnimationNextFrame(Id animationId, const Common::String &/*name*/, uint32 frame, uint32 frameCount) {
	if (animationId == 90901 && (frame == frameCount || frame == 1)) {
		_app->objectPresentationPauseAnimation(kObject10901, 0);
		_app->objectPresentationPauseAnimation(kObject10901, 1);
		_app->objectPresentationPauseAnimation(kObject10901, 4);
		_app->timerStart(kTimer0, rnd(3000) + 3000);
	}
}

void Zone10Pompeii::onSound(Id id, SoundType /*type*/, uint32 /*a3*/, bool /*process*/) {
	switch (id) {
	default:
		break;

	case 3049:
		_app->playMovie("S09A02-1");
		_app->rotationSetMovabilityOn(10922, 1, 1);
		_app->objectPresentationShow(kObject10901, 1);
		break;

	case 3050:
		_app->exitToMenu(kMenuAction7);
		break;

	case 3051:
		_app->playMovie("S09A03-1");
		_app->soundPlay(3050);
		break;
	}
}

void Zone10Pompeii::onBeforeRide(Id movabilityFrom, Id movabilityTo, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType movabilityType) {
	if (movabilityType == kMovabilityRotationToRotation) {
		if ((movabilityFrom == 10925 && movabilityTo == 10911)
		 || (movabilityFrom == 10911 && movabilityTo == 10925))
			_app->fadeOut(15, Color(0, 0, 0), 0);
	}
}

} // End of namespace Ring
