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

#include "ring/game/pompeii/pompeii_zone7.h"

#include "ring/game/pompeii/pompeii_application.h"
#include "ring/game/pompeii/pompeii_shared.h"

#include "ring/debug.h"
#include "ring/ring.h"
#include "ring/helpers.h"

using namespace PompeiiGame;

namespace Ring {

Zone7Pompeii::Zone7Pompeii(ApplicationPompeii *application) : _app(application) {
	_hideBox = false;
}

Zone7Pompeii::~Zone7Pompeii() {
	// Zero-out passed pointers
	_app = nullptr;
}

void Zone7Pompeii::onInit() {
	_app->setCurrentZone(kZone7);

	_app->puzzleAdd(kPuzzle60011);
	_app->puzzleAddBackgroundImage(kPuzzle60011, "s06a01n02p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle60021);
	_app->puzzleAddBackgroundImage(kPuzzle60021, "s06a02n01p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle60022);
	_app->puzzleAddBackgroundImage(kPuzzle60022, "s06a02n01p02.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle60023);
	_app->puzzleAddBackgroundImage(kPuzzle60023, "s06a02n01p03.bmp", Common::Point(0, 16), true);
	_app->rotationAdd(10611, "S06A01-8", 0, 1);
	_app->rotationAdd(10612, "S06A01-7", 0, 2);
	_app->rotationAdd(10613, "S06A01-6", 0, 1);
	_app->rotationAdd(10614, "S06A01-5", 0, 1);
	_app->rotationAdd(10615, "S06A01-4", 0, 1);
	_app->rotationAdd(10616, "S06A01-3", 0, 0);
	_app->rotationAdd(10617, "S06A01-2", 0, 0);
	_app->rotationAdd(10618, "S06A0-10", 0, 0);
	_app->rotationAdd(10619, "S06A01-9", 0, 0);
	_app->rotationAdd(106110, "S06A01-1", 0, 0);
	_app->rotationAdd(10621, "S06A02-4", 0, 1);
	_app->rotationAdd(10622, "S06A02-3", 0, 0);
	_app->rotationAdd(10623, "S06A02-2", 0, 0);
	_app->rotationAdd(10624, "S06A02-1", 0, 0);
	_app->rotationAddMovabilityToRotation(10611, 10613, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10611, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10611, 106110, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10611, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10612, 10613, "", Common::Rect(410, -250, 560, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10612, 0, 3.0f, 0.30000001f, 87.0f, 0.0f, 1, 3.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10612, 10614, "", Common::Rect(1993, -250, 2184, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10612, 1, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10612, 10621, "", Common::Rect(2950, -250, 3100, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10612, 2, 257.0f, 0.30000001f, 87.0f, 0.0f, 1, 257.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10613, 10611, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10613, 0, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10613, 10612, "", Common::Rect(2210, -250, 2360, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10613, 1, 183.0f, 0.30000001f, 87.0f, 0.0f, 1, 183.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10614, 10612, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10614, 0, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10614, 10615, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10614, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10615, 10614, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10615, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10615, 10616, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10615, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10615, 10621, "", Common::Rect(670, -250, 820, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10615, 2, 29.0f, 0.30000001f, 87.0f, 0.0f, 1, 29.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10615, 10622, "", Common::Rect(3560, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10615, 3, 318.0f, 0.30000001f, 87.0f, 0.0f, 1, 318.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10615, 10622, "", Common::Rect(0, -250, 110, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10615, 4, 318.0f, 0.30000001f, 87.0f, 0.0f, 1, 318.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10616, 10615, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10616, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10616, 10617, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10616, 1, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10617, 10616, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10617, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10617, 10618, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10617, 1, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10617, 10622, "", Common::Rect(1390, -250, 1540, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10617, 2, 101.0f, 0.30000001f, 87.0f, 0.0f, 1, 101.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10618, 10617, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10618, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10618, 10619, "", Common::Rect(420, -250, 570, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10618, 1, 4.0f, 0.30000001f, 87.0f, 0.0f, 1, 4.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10619, 10618, "", Common::Rect(2220, -250, 2370, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10619, 0, 184.0f, 0.30000001f, 87.0f, 0.0f, 1, 184.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10619, 106110, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10619, 1, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(106110, 10611, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(106110, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(106110, 10619, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(106110, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10621, 10612, "", Common::Rect(900, -250, 1150, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10621, 0, 77.0f, 0.30000001f, 87.0f, 0.0f, 1, 77.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10621, 10615, "", Common::Rect(2470, -250, 2620, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10621, 1, 209.0f, 0.30000001f, 87.0f, 0.0f, 1, 209.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10621, 10622, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10621, 2, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10621, 10623, "", Common::Rect(3570, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10621, 3, 319.0f, 0.30000001f, 87.0f, 0.0f, 1, 319.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10621, 10623, "", Common::Rect(0, -250, 120, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10621, 4, 319.0f, 0.30000001f, 87.0f, 0.0f, 1, 319.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10622, 10615, "", Common::Rect(1760, -250, 1910, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10622, 0, 138.0f, 0.30000001f, 87.0f, 0.0f, 1, 138.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10622, 10617, "", Common::Rect(3190, -250, 3340, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10622, 1, 281.0f, 0.30000001f, 87.0f, 0.0f, 1, 281.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10622, 10621, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10622, 2, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10622, 10623, "", Common::Rect(750, -250, 900, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10622, 3, 37.0f, 0.30000001f, 87.0f, 0.0f, 1, 37.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10623, 10621, "", Common::Rect(1770, -250, 1920, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10623, 0, 139.0f, 0.30000001f, 87.0f, 0.0f, 1, 139.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10623, 10622, "", Common::Rect(2550, -250, 2700, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10623, 1, 217.0f, 0.30000001f, 87.0f, 0.0f, 1, 217.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10623, 10624, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10623, 2, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10624, 10623, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10624, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->puzzleAddMovabilityToRotation(kPuzzle60021, 10621, "", Common::Rect(0, 420, 640, 464), true, kCursorBack, 0);
	_app->puzzleSetMovabilityToRotation(kPuzzle60021, 0, 300.0f, 1.0f, 87.0f);
	_app->puzzleAddMovabilityToRotation(kPuzzle60022, 10621, "", Common::Rect(0, 420, 640, 464), true, kCursorBack, 0);
	_app->puzzleSetMovabilityToRotation(kPuzzle60022, 0, 305.0f, 0.0f, 87.0f);
	_app->puzzleAddMovabilityToRotation(kPuzzle60023, 10621, "", Common::Rect(0, 420, 640, 464), true, kCursorBack, 0);
	_app->puzzleSetMovabilityToRotation(kPuzzle60023, 0, 60.0f, 0.0f, 87.0f);
	_app->objectAdd(kObjectGlassTube, "glass tube", "s_tube", 9);
	_app->objectAddPuzzleAccessibility(kObjectGlassTube, kPuzzle60023, Common::Rect(343, 153, 447, 217), false, kCursorActionObject, 1);
	_app->objectAdd(kObjectGlassTube2, "glass tube", "s_tube", 1);
	_app->objectAdd(kObjectDropOfWater, "drop of water", "atears", 1);
	_app->objectAddRotationAccessibility(kObjectDropOfWater, 10621, Common::Rect(3420, 8, 3600, 80), false, kCursorZoom, 0);
	_app->objectAddRotationAccessibility(kObjectDropOfWater, 10621, Common::Rect(0, 8, 8, 80), false, kCursorZoom, 0);
	_app->objectAddPuzzleAccessibility(kObjectDropOfWater, kPuzzle60022, Common::Rect(168, 114, 268, 154), true, kCursorActionObject, 1);
	_app->objectAddPresentation(kObjectDropOfWater);
	_app->objectAdd(kObjectDropOfBlood, "drop of blood", "ablood", 1);
	_app->objectAddRotationAccessibility(kObjectDropOfBlood, 10621, Common::Rect(3324, 96, 3584, 280), false, kCursorZoom, 0);
	_app->objectAddPuzzleAccessibility(kObjectDropOfBlood, kPuzzle60021, Common::Rect(120, 150, 250, 260), true, kCursorActionObject, 1);
	_app->objectAddPresentation(kObjectDropOfBlood);
	_app->objectAdd(kObjectApolloTear, "Apollo's tear", "atears", 1);
	_app->objectAddRotationAccessibility(kObjectApolloTear, 10621, Common::Rect(950, -257, 1070, -22), true, kCursorZoom, 0);
	_app->objectAddPresentation(kObjectApolloTear);
	_app->objectPresentationAddAnimationToPuzzle(kObjectApolloTear, 0, kPuzzle60023, "s06a02n01p03s01", kImageTypeBMP, Common::Point(322, 173), kDrawTypeNormal, 1000, 100, 20.0f, 6);
	_app->objectPresentationSetAnimationOnPuzzle(kObjectApolloTear, 0, 0, 60000);

	for (uint32 i = 0; i < 100; i++)
		_app->objectAddPuzzleAccessibility(kObjectApolloTear, kPuzzle60023, Common::Rect(342, (int16)(i * 2.5999999f + 193.0f), 392, (int16)(i * 2.5999999f + 213.0f)), false, kCursorActionObject, i + 1);

	_app->objectAdd(kObject10601, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10601, 10611, Common::Rect(1030, -269, 1441, 247), true, kCursorMove, 0);
	_app->objectAddPresentation(kObject10601);
	_app->objectPresentationAddImageToRotation(kObject10601, 0, 10611, 0);
	_app->objectPresentationAddImageToRotation(kObject10601, 0, 10612, 0);
	_app->objectPresentationAddImageToRotation(kObject10601, 0, 10613, 0);
	_app->objectPresentationAddImageToRotation(kObject10601, 0, 10614, 0);
	_app->objectPresentationAddImageToRotation(kObject10601, 0, 10615, 0);
	_app->objectPresentationAddImageToRotation(kObject10601, 0, 10621, 0);
	_app->objectPresentationAddAnimationToRotation(kObject10601, 0, 10612, 1, 14, 12.5f, 4);
	_app->objectPresentationAddAnimationToPuzzle(kObject10601, 0, kPuzzle60011, "s06a01n02p01s01", kImageTypeBMP, Common::Point(218, 182), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject10601, 0, 0, 60001);
	_app->objectAdd(kObject60611, "", "", 1);
	_app->objectAddRotationAccessibility(kObject60611, 10612, Common::Rect(2183, -140, 2385, 227), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject60611, 10612, Common::Rect(2183, -140, 2385, 227), false, kCursorIdle, 1);
	_app->objectAddRotationAccessibility(kObject60611, 10612, Common::Rect(2183, -140, 2385, 227), false, kCursorAction, 2);
	_app->soundAdd(60001, kSoundTypeAmbientEffect, "05_BLO-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(60002, kSoundTypeAmbientEffect, "05_WAT-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(2029, kSoundTypeDialog, "2029.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2030, kSoundTypeDialog, "2030.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2031, kSoundTypeDialog, "2031.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(66001, kSoundTypeAmbientMusic, "BGR_MU-1.WAc", _app->getConfiguration().backgroundMusic.loadFrom);
	_app->rotationAddAmbientSound(10611, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10612, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10613, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10614, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10615, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10616, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10617, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10618, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10619, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(106110, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10621, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10622, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10623, 66001, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10624, 66001, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle60011, 66001, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle60021, 66001, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle60022, 66001, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle60023, 66001, 90, 0, 1, 1, 10);
	_app->varDefineByte(60000, 0);
}

void Zone7Pompeii::onButtonUp(ObjectId id, Id target, Id /*puzzleRotationId*/, uint32 /*a4*/, const Common::Point &/*point*/) {
	switch (id) {
	default:
		break;

	case kObject10601:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (_app->varGetByte(91207) + _app->varGetByte(91206) == 2)
				_app->varSetByte(90209, 2);

			_app->objectPresentationPauseAnimation(kObject10601, 0);
			_app->timerStopAll();
			_app->onCall(10600);
		}
		break;

	case kObject60611:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target) {
				_app->varSetByte(90209, 1);
				_app->objectSetAccessibilityOff(kObject60611);
				_app->puzzleSetActive(kPuzzle60011);
				_app->soundPlay(2029);
			}
		}
		break;

	case kObjectDropOfWater:
		if (_app->bagHasClickedObject()) {
			if (_app->bagGetClickedObject() == kObjectGlassTube || _app->bagGetClickedObject() == kObjectGlassTube2) {
				if (target == 1) {
					_app->soundPlay(60002);
					_app->bagRemove(kObjectGlassTube);
					_app->bagAdd(kObjectDropOfWater);
				}
			}

			_app->cursorDelete();
		} else if (!target) {
			_app->puzzleSetActive(kPuzzle60022);
		}
		break;

	case kObjectExitEncyclopedia:
		_app->restore();
		break;

	case kObjectGlassTube:
		if (_app->bagHasClickedObject() && _app->bagGetClickedObject() == kObjectDropOfWater) {
			_app->soundPlay(60002);
			_app->objectSetAccessibilityOff(kObjectGlassTube);
			_app->objectPresentationShow(kObjectApolloTear, 0);
			_app->bagRemove(kObjectDropOfWater);
			_app->bagAdd(kObjectGlassTube);
		} else {
			_app->setField74(false);
		}
		break;

	case kObjectDropOfBlood:
		if (_app->bagHasClickedObject()) {
			if ((_app->bagGetClickedObject() == kObjectGlassTube || _app->bagGetClickedObject() == kObjectGlassTube2) && target == 1) {
				_app->soundPlay(60001);
				_app->objectSetAccessibilityOff(kObjectDropOfBlood);
				_app->bagRemove(kObjectGlassTube2);
				_app->bagAdd(kObjectDropOfBlood);
			}

			_app->cursorDelete();
		} else if (!target) {
			_app->puzzleSetActive(kPuzzle60021);
		}
		break;

	case kObjectApolloTear:
		if (_app->bagHasClickedObject())
		{
			if ((_app->bagGetClickedObject() == kObjectGlassTube || _app->bagGetClickedObject() == kObjectGlassTube2) && target >= 2) {
				_app->soundPlay(60002);
				_app->objectSetAccessibilityOff(kObjectDropOfWater);
				_app->objectSetAccessibilityOff(kObjectApolloTear, 1, 100);
				_app->bagRemove(kObjectGlassTube);
				_app->bagAdd(kObjectApolloTear);
				_app->objectPresentationHide(kObjectApolloTear);
			}

			_app->cursorDelete();
		}
		else if (!target) {
			_app->puzzleSetActive(kPuzzle60023);
		}
		break;
	}
}

void Zone7Pompeii::onTimer(TimerId id) {
	if (id == kTimer0) {
		_app->timerStop(kTimer0);
		_app->objectPresentationUnpauseAnimation(kObject10601, 0);
	}
}

void Zone7Pompeii::onAnimationNextFrame(Id animationId, const Common::String &/*name*/, uint32 frame, uint32 frameCount) {
	switch (animationId) {
	default:
		break;

	case 60000:
		_app->objectSetAccessibilityOff(kObjectApolloTear, 1, 100);
		_app->objectSetAccessibilityOn(kObjectApolloTear, frame, frame);

		if (frame == frameCount) {
			_app->objectSetAccessibilityOff(kObjectApolloTear);
			_app->objectSetAccessibilityOn(kObjectApolloTear, 0, 0);
			_app->objectSetAccessibilityOn(kObjectApolloTear);
			_app->cursorDelete();
		}
		break;

	case 60001:
		if (frame == frameCount) {
			_app->objectPresentationPauseAnimation(kObject10601, 0);
			_app->timerStart(kTimer0, rnd(4000) + 2000);
		}
		break;
	}
}

void Zone7Pompeii::onSound(Id id, SoundType /*type*/, uint32 /*a3*/, bool /*process*/) {
	switch (id) {
	default:
		break;

	case 2029:
		_app->rotationSetAlp(10612, 190.0f);
		_app->rotationSetActive(10612);

		_app->objectSetAccessibilityOn(kObject60611, 1, 1);
		_hideBox = false;
		break;

	case 2030:
	case 2031:
		_app->rotationSetAlp(10612, 190.0f);
		_app->rotationSetActive(10612);
		break;
	}
}

void Zone7Pompeii::onUpdateBag(const Common::Point &/*point*/) {
	if (_hideBox) {
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;
	}
}

void Zone7Pompeii::onUpdateBefore(Id movabilityFrom, Id movabilityTo, uint32 /*movabilityIndex*/, Id /*target*/, const Common::Point &/*point*/) {
	if (movabilityFrom == 60611
	 && movabilityTo == 1
	 && !_hideBox
	 && !_app->bagHasClickedObject()) {
		_app->visualBoxSetParameters(6, kPuzzleMenu, 2029, g_system->getEventManager()->getMousePos() - Common::Point(20, 16));

		_hideBox = true;
	}
}

void Zone7Pompeii::onUpdateAfter(Id /*movabilityFrom*/, Id /*movabilityTo*/, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType /*movabilityType*/, const Common::Point &/*point*/) {
	if (_hideBox) {
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;
	}
}

void Zone7Pompeii::onVisual(int x) {
	switch (x) {
	default:
		break;

	case 2029:
		_app->objectSetAccessibilityOff(kObject60611);
		_app->objectSetAccessibilityOn(kObjectDropOfWater);
		_app->objectSetAccessibilityOn(kObjectDropOfBlood);
		_app->objectSetAccessibilityOn(kObjectGlassTube);
		break;

	case 2030:
	case 2031:
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;

		_app->puzzleSetActive(kPuzzle60011);
		break;
	}
}

} // End of namespace Ring
