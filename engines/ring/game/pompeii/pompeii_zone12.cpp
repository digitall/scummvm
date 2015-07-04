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

#include "ring/game/pompeii/pompeii_zone12.h"

#include "ring/game/pompeii/pompeii_application.h"
#include "ring/game/pompeii/pompeii_shared.h"

#include "ring/debug.h"
#include "ring/ring.h"
#include "ring/helpers.h"

using namespace PompeiiGame;

namespace Ring {

Zone12Pompeii::Zone12Pompeii(ApplicationPompeii *application) : _app(application) {
	_hideBox = false;
}

Zone12Pompeii::~Zone12Pompeii() {
	// Zero-out passed pointers
	_app = nullptr;
}

void Zone12Pompeii::onInit() {
	_app->setCurrentZone(kZone12);

	_app->puzzleAdd(kPuzzle111211);
	_app->puzzleAddBackgroundImage(kPuzzle111211, "s11a02n01p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle111212);
	_app->puzzleAddBackgroundImage(kPuzzle111212, "s11a02n01p04.bmp", Common::Point(0, 16), true);
	_app->rotationAdd(11111, "S11A01-4", 0, 4);
	_app->rotationAdd(11112, "S11A01-3", 0, 4);
	_app->rotationAdd(11113, "S11A01-2", 0, 4);
	_app->rotationAdd(11114, "S11A01-1", 0, 4);
	_app->rotationAdd(11121, "S11A02-4", 0, 7);
	_app->rotationAdd(11122, "S11A02-3", 0, 6);
	_app->rotationAdd(11123, "S11A02-2", 0, 7);
	_app->rotationAdd(11124, "S11A02-1", 0, 8);
	_app->rotationAddMovabilityToRotation(11111, 11112, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11111, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11111, 11113, "", Common::Rect(1910, -250, 2060, 500), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11111, 1, 153.0f, 0.30000001f, 87.0f, 0.0f, 1, 153.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11112, 11111, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11112, 0, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11112, 11114, "", Common::Rect(2450, -250, 2600, 500), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11112, 1, 207.0f, 0.30000001f, 87.0f, 0.0f, 1, 207.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11113, 11111, "", Common::Rect(110, -500, 260, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11113, 0, 333.0f, 0.30000001f, 87.0f, 0.0f, 1, 333.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11113, 11114, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11113, 1, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11113, 11124, "", Common::Rect(2010, -250, 2160, 450), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11113, 2, 163.0f, 0.30000001f, 87.0f, 0.0f, 1, 163.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11114, 11112, "", Common::Rect(650, -500, 800, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11114, 0, 27.0f, 0.30000001f, 87.0f, 0.0f, 1, 27.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11114, 11113, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11114, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11114, 11124, "", Common::Rect(2390, -250, 2540, 450), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11114, 2, 201.0f, 0.30000001f, 87.0f, 0.0f, 1, 201.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11121, 11123, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11121, 0, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 324.29999f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11122, 11123, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11122, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 54.299999f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11123, 11121, "", Common::Rect(1820, -250, 1970, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11123, 0, 144.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11123, 11122, "", Common::Rect(2720, -250, 2870, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11123, 1, 234.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11123, 11124, "", Common::Rect(610, -250, 760, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11123, 2, 23.0f, 0.30000001f, 87.0f, 0.0f, 1, 59.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11124, 11113, "", Common::Rect(210, -250, 360, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11124, 0, 343.0f, 0.30000001f, 87.0f, 0.0f, 1, 343.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11124, 11114, "", Common::Rect(590, -250, 740, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11124, 1, 21.0f, 0.30000001f, 87.0f, 0.0f, 1, 21.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(11124, 11123, "", Common::Rect(2770, -250, 2920, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(11124, 2, 239.0f, 0.30000001f, 87.0f, 0.0f, 1, 203.3f, 0.30000001f, 87.0f);
	_app->objectAdd(kObjectMaskAndCoat, "mask & coat", "k_coat", 1);
	_app->objectAddRotationAccessibility(kObjectMaskAndCoat, 11124, Common::Rect(1262, 26, 1409, 99), false, kCursorTake, 0);
	_app->objectAdd(kObjectCapsa, "capsa", "capsa", 1);
	_app->objectAddRotationAccessibility(kObjectCapsa, 11123, Common::Rect(3222, 98, 3334, 224), false, kCursorTake, 0);
	_app->objectAddRotationAccessibility(kObjectCapsa, 11124, Common::Rect(3086, 40, 3183, 138), false, kCursorTake, 0);
	_app->objectAdd(kObject11101, "", "", 1);
	_app->objectAddRotationAccessibility(kObject11101, 11122, Common::Rect(2842, -238, 3403, 273), true, kCursorMove, 0);
	_app->objectAddPresentation(kObject11101);
	_app->objectPresentationAddImageToRotation(kObject11101, 0, 11111, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 0, 11112, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 0, 11113, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 0, 11114, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 0, 11121, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 0, 11122, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 0, 11123, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 0, 11124, 0);
	_app->objectAddPresentation(kObject11101);
	_app->objectPresentationAddImageToRotation(kObject11101, 1, 11111, 1);
	_app->objectPresentationAddImageToRotation(kObject11101, 1, 11112, 1);
	_app->objectPresentationAddImageToRotation(kObject11101, 1, 11113, 1);
	_app->objectPresentationAddImageToRotation(kObject11101, 1, 11114, 1);
	_app->objectPresentationAddImageToRotation(kObject11101, 1, 11121, 2);
	_app->objectPresentationAddImageToRotation(kObject11101, 1, 11122, 1);
	_app->objectPresentationAddImageToRotation(kObject11101, 1, 11123, 2);
	_app->objectPresentationAddImageToRotation(kObject11101, 1, 11124, 2);
	_app->objectAddPresentation(kObject11101);
	_app->objectPresentationAddImageToRotation(kObject11101, 2, 11111, 2);
	_app->objectPresentationAddImageToRotation(kObject11101, 2, 11112, 2);
	_app->objectPresentationAddImageToRotation(kObject11101, 2, 11113, 2);
	_app->objectPresentationAddImageToRotation(kObject11101, 2, 11114, 2);
	_app->objectPresentationAddImageToRotation(kObject11101, 2, 11121, 3);
	_app->objectPresentationAddImageToRotation(kObject11101, 2, 11122, 2);
	_app->objectPresentationAddImageToRotation(kObject11101, 2, 11123, 3);
	_app->objectPresentationAddImageToRotation(kObject11101, 2, 11124, 3);
	_app->objectAddPresentation(kObject11101);
	_app->objectPresentationAddImageToRotation(kObject11101, 3, 11111, 3);
	_app->objectPresentationAddImageToRotation(kObject11101, 3, 11112, 3);
	_app->objectPresentationAddImageToRotation(kObject11101, 3, 11113, 3);
	_app->objectPresentationAddImageToRotation(kObject11101, 3, 11114, 3);
	_app->objectPresentationAddImageToRotation(kObject11101, 3, 11121, 4);
	_app->objectPresentationAddImageToRotation(kObject11101, 3, 11122, 3);
	_app->objectPresentationAddImageToRotation(kObject11101, 3, 11123, 4);
	_app->objectPresentationAddImageToRotation(kObject11101, 3, 11124, 4);
	_app->objectAddPresentation(kObject11101);
	_app->objectPresentationAddImageToRotation(kObject11101, 4, 11111, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 4, 11112, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 4, 11113, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 4, 11114, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 4, 11121, 5);
	_app->objectPresentationAddImageToRotation(kObject11101, 4, 11122, 4);
	_app->objectPresentationAddImageToRotation(kObject11101, 4, 11123, 5);
	_app->objectPresentationAddImageToRotation(kObject11101, 4, 11124, 5);
	_app->objectAddPresentation(kObject11101);
	_app->objectPresentationAddImageToRotation(kObject11101, 5, 11111, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 5, 11112, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 5, 11113, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 5, 11114, 0);
	_app->objectPresentationAddImageToRotation(kObject11101, 5, 11121, 6);
	_app->objectPresentationAddImageToRotation(kObject11101, 5, 11122, 5);
	_app->objectPresentationAddImageToRotation(kObject11101, 5, 11123, 6);
	_app->objectPresentationAddImageToRotation(kObject11101, 5, 11124, 6);
	_app->objectAddPresentation(kObject11101);
	_app->objectPresentationAddAnimationToRotation(kObject11101, 6, 11121, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject11101, 6, 11123, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject11101, 6, 11124, 1, 15, 12.5f, 16);
	_app->objectPresentationSetAnimationOnRotation(kObject11101, 6, 0, 11000);
	_app->objectAddPresentation(kObject11101);
	_app->objectPresentationAddAnimationToRotation(kObject11101, 7, 11124, 7, 15, 12.5f, 16);
	_app->objectPresentationSetAnimationOnRotation(kObject11101, 7, 0, 11001);
	_app->objectAdd(kObject111001, "", "", 1);
	_app->objectAddRotationAccessibility(kObject111001, 11121, Common::Rect(1001, -24, 1121, 176), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject111001, 11121, Common::Rect(1001, -24, 1121, 176), false, kCursorIdle, 1);
	_app->objectAddRotationAccessibility(kObject111001, 11121, Common::Rect(1001, -24, 1121, 176), false, kCursorAction, 2);
	_app->objectAddRotationAccessibility(kObject111001, 11121, Common::Rect(1001, -24, 1121, 176), false, kCursorIdle, 3);
	_app->objectAddRotationAccessibility(kObject111001, 11121, Common::Rect(1001, -24, 1121, 176), false, kCursorAction, 4);
	_app->objectAddRotationAccessibility(kObject111001, 11121, Common::Rect(1001, -24, 1121, 176), false, kCursorIdle, 5);
	_app->objectAddRotationAccessibility(kObject111001, 11121, Common::Rect(1001, -24, 1121, 176), false, kCursorAction, 6);
	_app->objectAddRotationAccessibility(kObject111001, 11121, Common::Rect(1001, -24, 1121, 176), false, kCursorAction, 7);
	_app->objectAddRotationAccessibility(kObject111001, 11121, Common::Rect(1001, -24, 1121, 176), false, kCursorAction, 8);
	_app->objectAddRotationAccessibility(kObject111001, 11121, Common::Rect(1001, -24, 1121, 176), false, kCursorAction, 9);
	_app->objectAddRotationAccessibility(kObject111001, 11121, Common::Rect(1001, -24, 1121, 176), false, kCursorAction, 10);
	_app->objectAddPresentation(kObject111001);
	_app->objectPresentationAddAnimationToPuzzle(kObject111001, 0, kPuzzle111211, "s11a02n01p01s01", kImageTypeBMP, Common::Point(316, 140), kDrawTypeNormal, 1000, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToPuzzle(kObject111001, 0, kPuzzle111212, "s11a02n01p04s01", kImageTypeBMP, Common::Point(456,  59), kDrawTypeNormal, 1000, 15, 12.5f, 16);
	_app->objectPresentationSetAnimationOnPuzzle(kObject111001, 0, 0, 11000);
	_app->objectPresentationShow(kObject111001, 0);
	_app->soundAdd(4048, kSoundTypeDialog, "4048.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4049, kSoundTypeDialog, "4049.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4050, kSoundTypeDialog, "4050.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4051, kSoundTypeDialog, "4051.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4052, kSoundTypeDialog, "4052.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4071, kSoundTypeDialog, "4071.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4072, kSoundTypeDialog, "4072.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4073, kSoundTypeDialog, "4073.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4074, kSoundTypeDialog, "4074.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4075, kSoundTypeDialog, "4075.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4076, kSoundTypeDialog, "4076.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4077, kSoundTypeDialog, "4077.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4078, kSoundTypeDialog, "4078.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4079, kSoundTypeDialog, "4079.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4080, kSoundTypeDialog, "4080.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4081, kSoundTypeDialog, "4081.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4082, kSoundTypeDialog, "4082.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(111111, kSoundTypeAmbientMusic, "MUS_GR-1.WAc", _app->getConfiguration().backgroundMusic.loadFrom);
	_app->rotationAddAmbientSound(11111, 111111, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(11112, 111111, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(11113, 111111, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(11114, 111111, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(11121, 111111, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(11122, 111111, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(11123, 111111, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(11124, 111111, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle111211, 111111, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle111212, 111111, 90, 0, 1, 1, 10);
	_app->varDefineByte(110001, 0);
}

void Zone12Pompeii::onButtonUp(ObjectId id, Id target, Id /*puzzleRotationId*/, uint32 /*a4*/, const Common::Point &/*point*/) {
	switch (id) {
	default:
		break;

	case kObject111001:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			switch (target) {
			default:
				break;

			case 0:
				_app->objectSetAccessibilityOff(kObject111001);
				_app->puzzleSetActive(kPuzzle111211);
				_app->soundPlay(4048);
				break;

			case 2:
				_app->objectSetAccessibilityOff(kObject111001);
					_app->puzzleSetActive(kPuzzle111212);
					_app->varSetByte(110001, 0);
					_app->soundPlay(4071);
				break;

			case 4:
				_app->objectSetAccessibilityOff(kObject111001);
						_app->puzzleSetActive(kPuzzle111212);
						_app->varSetByte(110001, 0);
						_app->soundPlay(4078);
				break;
			}
		}
		break;

	case kObject11101:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->objectPresentationPauseAnimation(kObject11101, 6);
			_app->objectPresentationPauseAnimation(kObject11101, 7);
			_app->objectPresentationPauseAnimation(kObject111001, 0);
			_app->timerStopAll();
			_app->onCall(11100);
		}
		break;

	case kObjectExitEncyclopedia:
		_app->restore();
		break;

	case kObjectMaskAndCoat:
	case kObjectCapsa:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (target == 0) {
				_app->objectSetAccessibilityOff(id);
				_app->bagAdd(id);
				_app->objectPresentationHide(kObject11101, id == kObjectMaskAndCoat ? 2 : 1);
			}
		}
		break;
	}
}

void Zone12Pompeii::onTimer(TimerId id) {
	switch (id) {
	default:
		break;

	case kTimer0:
		_app->timerStop(kTimer0);
		_app->objectPresentationUnpauseAnimation(kObject11101, 6);
		_app->objectPresentationUnpauseAnimation(kObject111001, 0);
		break;

	case kTimer1:
		_app->timerStop(kTimer1);
		_app->objectPresentationUnpauseAnimation(kObject11101, 7);
		break;
	}
}

void Zone12Pompeii::onAnimationNextFrame(Id animationId, const Common::String &/*name*/, uint32 frame, uint32 frameCount) {
	switch (animationId) {
	default:
		break;

	case 11000:
		if (frame == frameCount || frame == 1) {
			_app->objectPresentationPauseAnimation(kObject11101, 6);
			_app->objectPresentationPauseAnimation(kObject111001, 0);
			_app->timerStart(kTimer0, 1000 * (rnd(3) + 4));
		}
		break;

	case 11001:
		if (frame == frameCount || frame == 1) {
			_app->objectPresentationPauseAnimation(kObject11101, 7);
			_app->timerStart(kTimer1, 1000 * (rnd(3) + 3));
		}
		break;
	}
}

void Zone12Pompeii::onSound(Id id, SoundType /*type*/, uint32 /*a3*/, bool /*process*/) {
	switch (id) {
	default:
		break;

	case 4048:
		_app->rotationSetAlp(11121, 63.0f);
		_app->rotationSetBet(11121, 7.0f);
		_app->rotationSetActive(11121);
		_app->objectSetAccessibilityOn(kObject111001, 1, 1);
		_hideBox = false;
		break;

	case 4071:
		_app->rotationSetAlp(11121, 63.0f);
		_app->rotationSetBet(11121, 7.0f);
		_app->rotationSetActive(11121);
		_app->objectSetAccessibilityOn(kObject111001, 3, 3);
		_hideBox = false;
		break;

	case 4072:
	case 4073:
		if (_app->varGetByte(110001) != 1) {
			_app->rotationSetAlp(11121, 63.0f);
			_app->rotationSetBet(11121, 7.0f);
			_app->rotationSetActive(11121);
		} else {
			_app->objectSetAccessibilityOff(kObject111001);
			_app->soundPlay(4074);
			_app->varSetByte(110001, 0);
		}
		break;

	case 4049:
	case 4050:
	case 4051:
	case 4052:
		_app->rotationSetAlp(11121, 63.0f);
		_app->rotationSetBet(11121, 7.0f);
		_app->rotationSetActive(11121);
		break;

	case 4074:
		_app->playMovie("S11A02-1");
		_app->timerStop(kTimer0);
		_app->timerStop(kTimer1);
		_app->objectPresentationHide(kObject11101, 4);
		_app->objectPresentationHide(kObject11101, 6);
		_app->objectPresentationShow(kObject11101, 5);
		_app->rotationSetAlp(11121, 63.0f);
		_app->rotationSetBet(11121, 7.0f);
		_app->rotationSetActive(11121);
		_app->soundPlay(4077);
		_app->objectSetAccessibilityOn(kObject111001, 4, 4);
		break;

	case 4078:
		_app->rotationSetAlp(11121, 63.0f);
		_app->rotationSetBet(11121, 7.0f);
		_app->rotationSetActive(11121);
		_app->objectSetAccessibilityOn(kObject111001, 5, 5);
		_hideBox = false;
		break;

	case 4079:
	case 4080:
	case 4081:
		_app->rotationSetAlp(11121, 63.0f);
		_app->rotationSetBet(11121, 7.0f);
		_app->rotationSetActive(11121);

		if (_app->varGetByte(110001) == 1) {
			_app->soundPlay(4082);
			_app->varSetByte(90410, 2);
			_app->onCall(4109);
		}
		break;
	}
}

void Zone12Pompeii::onUpdateBag(const Common::Point &/*point*/) {
	if (_hideBox) {
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;
	}
}

void Zone12Pompeii::onUpdateBefore(Id movabilityFrom, Id movabilityTo, uint32 /*movabilityIndex*/, Id /*target*/, const Common::Point &/*point*/) {
	Common::Point mouse = g_system->getEventManager()->getMousePos() - Common::Point(20, 16);

	if (movabilityFrom == 111001
	 && !_hideBox
	 && !_app->bagHasClickedObject()) {

		switch (movabilityTo) {
		default:
			break;

		case 1:
			_app->visualBoxSetParameters(6, kPuzzleMenu, 4048, mouse);
			_hideBox = true;
			break;

		case 3:
			_app->visualBoxSetParameters(6, kPuzzleMenu, 4071, mouse);
			_hideBox = true;
			break;

		case 5:
			_app->visualBoxSetParameters(6, kPuzzleMenu, 4078, mouse);
			_hideBox = true;
			break;
		}
	}
}

void Zone12Pompeii::onUpdateAfter(Id /*movabilityFrom*/, Id /*movabilityTo*/, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType /*movabilityType*/, const Common::Point &/*point*/) {
	if (_hideBox) {
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;
	}
}

void Zone12Pompeii::onVisual(int x) {
	switch (x) {
	default:
		break;

	case 4048:
		_app->objectSetAccessibilityOff(kObject111001);
		_app->varSetByte(90407, 2);
		break;

	case 4049:
	case 4050:
	case 4051:
	case 4052:
		_hideBox = false;
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle111211);
		break;

	case 4072:
	case 4073:
	case 4079:
	case 4080:
	case 4081:
		_hideBox = false;
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle111212);
		break;

	case 4071:
	case 4078:
		_app->objectSetAccessibilityOff(kObject111001);
		_app->varSetByte(110001, 1);
		break;
	}
}

} // End of namespace Ring
