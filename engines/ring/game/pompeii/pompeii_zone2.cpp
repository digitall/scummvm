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

#include "ring/game/pompeii/pompeii_zone2.h"

#include "ring/game/pompeii/pompeii_application.h"
#include "ring/game/pompeii/pompeii_shared.h"

#include "ring/debug.h"
#include "ring/ring.h"
#include "ring/helpers.h"

using namespace PompeiiGame;

namespace Ring {

Zone2Pompeii::Zone2Pompeii(ApplicationPompeii *application) : _app(application) {
	_hideBox = false;
}

Zone2Pompeii::~Zone2Pompeii() {
	// Zero-out passed pointers
	_app = nullptr;
}

void Zone2Pompeii::onInit() {
	_app->setCurrentZone(kZone2);

	_app->puzzleAdd(kPuzzle10000);
	_app->puzzleAddBackgroundImage(kPuzzle10000, "s01a01n01p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle10001);
	_app->puzzleAddBackgroundImage(kPuzzle10001, "s01a01n01p11.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle10007);
	_app->puzzleAddBackgroundImage(kPuzzle10007, "s01a01n01p02.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle10010);
	_app->puzzleAddBackgroundImage(kPuzzle10010, "s01a01n01p05.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle10013);
	_app->puzzleAddBackgroundImage(kPuzzle10013, "s01a01n01p08.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle10016);
	_app->puzzleAddBackgroundImage(kPuzzle10016, "s01a09n01p02.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle10002);
	_app->puzzleAddBackgroundImage(kPuzzle10002, "s01a09n01p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle10003);
	_app->puzzleAddBackgroundImage(kPuzzle10003, "s01a04n01p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle10017);
	_app->puzzleAddBackgroundImage(kPuzzle10017, "s01a02n02p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle10021);
	_app->puzzleAddBackgroundImage(kPuzzle10021, "s01a04n01p04.bmp", Common::Point(0, 16), true);
	_app->rotationAdd(10111, "S01A01-8", 0, 8);
	_app->rotationSetCompressionBufferLength(10111, 1100000);
	_app->rotationAdd(10112, "S01A01-7", 0, 4);
	_app->rotationAdd(10113, "S01A01-6", 0, 4);
	_app->rotationAdd(10114, "S01A01-5", 0, 2);
	_app->rotationAdd(10121, "S01A0-16", 0, 1);
	_app->rotationSetCompressionBufferLength(10121, 1200000);
	_app->rotationAdd(10122, "S01A0-14", 0, 6);
	_app->rotationSetCompressionBufferLength(10122, 1500000);
	_app->rotationAdd(10123, "S01A0-13", 0, 0);
	_app->rotationAdd(10124, "S01A0-12", 0, 1);
	_app->rotationAdd(10125, "S01A0-11", 0, 4);
	_app->rotationAdd(10126, "S01A0-10", 0, 1);
	_app->rotationAdd(10127, "S01A02-9", 0, 0);
	_app->rotationAdd(10128, "S01A0-17", 0, 0);
	_app->rotationAdd(10131, "S01A03-1", 0, 0);
	_app->rotationAdd(10141, "S01A04-2", 0, 7);
	_app->rotationAdd(10142, "S01A04-1", 0, 1);
	_app->rotationAdd(10151, "S01A05-4", 0, 0);
	_app->rotationAdd(10152, "S01A05-3", 0, 0);
	_app->rotationAdd(10153, "S01A05-2", 0, 0);
	_app->rotationAdd(10154, "S01A05-1", 0, 0);
	_app->rotationAdd(10155, "S01A05-5", 0, 0);
	_app->rotationAdd(10161, "S01A0-15", 0, 0);
	_app->rotationAdd(10162, "S01A06-9", 0, 0);
	_app->rotationAdd(10163, "S01A06-8", 0, 1);
	_app->rotationAdd(10164, "S01A06-7", 0, 0);
	_app->rotationAdd(10166, "S01A06-6", 0, 0);
	_app->rotationAdd(10171, "S01A07-6", 0, 4);
	_app->rotationAdd(10172, "S01A07-5", 0, 5);
	_app->rotationAdd(10173, "S01A07-4", 0, 5);
	_app->rotationAdd(10181, "S01A08-1", 0, 3);
	_app->rotationAdd(10191, "S01A09-1", 0, 4);
	_app->rotationAdd(10101, "S01A10-1", 0, 4);
	_app->rotationAdd(20111, "S01A01-4", 0, 0);
	_app->rotationAdd(20112, "S01A01-3", 0, 0);
	_app->rotationAdd(20113, "S01A01-2", 0, 0);
	_app->rotationAdd(20114, "S01A01-1", 0, 0);
	_app->rotationAdd(20121, "S01A02-7", 0, 0);
	_app->rotationAdd(20122, "S01A02-6", 0, 0);
	_app->rotationAdd(20123, "S01A02-5", 0, 0);
	_app->rotationAdd(20124, "S01A02-4", 0, 0);
	_app->rotationAdd(20125, "S01A02-3", 0, 0);
	_app->rotationAdd(20126, "S01A02-2", 0, 0);
	_app->rotationAdd(20127, "S01A02-1", 0, 0);
	_app->rotationAdd(20128, "S01A02-8", 0, 0);
	_app->rotationAdd(20161, "S01A06-5", 0, 0);
	_app->rotationAdd(20162, "S01A06-4", 0, 0);
	_app->rotationAdd(20163, "S01A06-3", 0, 0);
	_app->rotationAdd(20164, "S01A06-2", 0, 0);
	_app->rotationAdd(20166, "S01A06-1", 0, 0);
	_app->rotationAdd(20171, "S01A07-3", 0, 0);
	_app->rotationAdd(20172, "S01A07-2", 0, 0);
	_app->rotationAdd(20173, "S01A07-1", 0, 0);
	_app->rotationAddMovabilityToRotation(10111, 10112, "", Common::Rect(112, -212, 337, 271), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10111, 0, 333.0f, 0.30000001f, 87.0f, 0.0f, 1, 333.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10111, 10113, "", Common::Rect(502, -243, 727, 247), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10111, 1, 18.0f, 0.30000001f, 87.0f, 0.0f, 1, 18.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10112, 10111, "", Common::Rect(1938, -180, 2285, 231), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10112, 0, 153.0f, 0.30000001f, 87.0f, 0.0f, 1, 153.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10112, 10113, "", Common::Rect(1216, -206, 1522, 327), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10112, 1, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10112, 10114, "", Common::Rect(3322, -208, 3600, 324), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10112, 2, 309.0f, 0.30000001f, 87.0f, 0.0f, 1, 309.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10112, 10114, "", Common::Rect(0, -208, 166, 324), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10112, 3, 309.0f, 0.30000001f, 87.0f, 0.0f, 1, 309.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10113, 10111, "", Common::Rect(2169, -120, 2499, 129), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10113, 0, 198.0f, 0.30000001f, 87.0f, 0.0f, 1, 198.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10113, 10112, "", Common::Rect(2970, -198, 3229, 243), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10113, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10114, 10112, "", Common::Rect(1713, -176, 1921, 238), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10114, 0, 129.0f, 0.30000001f, 87.0f, 0.0f, 1, 129.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10114, 10128, "", Common::Rect(181, -317, 659, 303), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10114, 1, 354.0f, 0.30000001f, 87.0f, 0.0f, 1, 354.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10121, 10127, "", Common::Rect(2814, -294, 3234, 329), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10121, 0, 247.0f, 0.30000001f, 87.0f, 0.0f, 1, 247.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10121, 10128, "", Common::Rect(1172, -283, 1705, 280), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10121, 1, 121.0f, 0.30000001f, 87.0f, 0.0f, 1, 121.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10122, 10124, "", Common::Rect(2035, -245, 2422, 329), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10122, 0, 174.0f, 0.30000001f, 87.0f, 0.0f, 1, 174.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10122, 10125, "", Common::Rect(3005, -329, 3440, 287), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10122, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10122, 10152, "", Common::Rect(1383, -290, 1692, 301), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10122, 2, 112.0f, 0.30000001f, 87.0f, 0.0f, 1, 112.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10122, 10101, "", Common::Rect(140, -303, 699, 301), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10122, 3, 9.0f, 0.30000001f, 87.0f, 0.0f, 1, 9.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10123, 10126, "", Common::Rect(191, -219, 680, 399), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10123, 0, 356.0f, 0.30000001f, 87.0f, 0.0f, 1, 356.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10123, 10127, "", Common::Rect(2060, -276, 2406, 360), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10123, 1, 171.0f, 0.30000001f, 87.0f, 0.0f, 1, 171.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10123, 10161, "", Common::Rect(2805, -382, 3339, 98), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10123, 2, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10124, 10122, "", Common::Rect(184, -278, 631, 315), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10124, 0, 354.0f, 0.30000001f, 87.0f, 0.0f, 1, 354.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10124, 10128, "", Common::Rect(2842, -294, 3392, 311), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10124, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10124, 10151, "", Common::Rect(1316, -262, 1740, 306), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10124, 2, 79.0f, 0.30000001f, 87.0f, 0.0f, 1, 79.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10125, 10122, "", Common::Rect(1130, -320, 1499, 306), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10125, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10125, 10126, "", Common::Rect(2870, -311, 3350, 311), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10125, 1, 263.0f, 0.30000001f, 87.0f, 0.0f, 1, 263.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10125, 10191, "", Common::Rect(228, -101, 771, 271), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10125, 2, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10126, 10123, "", Common::Rect(2083, -219, 2420, 308), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10126, 0, 176.0f, 0.30000001f, 87.0f, 0.0f, 1, 176.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10126, 10125, "", Common::Rect(1117, -250, 1487, 355), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10126, 1, 83.0f, 0.30000001f, 87.0f, 0.0f, 1, 83.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10126, 10181, "", Common::Rect(646, -240, 1105, 278), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10126, 2, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10127, 10121, "", Common::Rect(917, -292, 1112, 257), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10127, 0, 67.0f, 0.30000001f, 87.0f, 0.0f, 1, 67.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10127, 10123, "", Common::Rect(216, -311, 713, 303), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10127, 1, 351.0f, 0.30000001f, 87.0f, 0.0f, 1, 351.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10127, 10128, "", Common::Rect(1211, -257, 1516, 310), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10127, 2, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10127, 10131, "", Common::Rect(1919, -194, 2394, 410), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10127, 3, 191.0f, 0.30000001f, 87.0f, 0.0f, 1, 191.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10128, 10114, "", Common::Rect(1937, -199, 2541, 397), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10128, 0, 174.0f, 0.30000001f, 87.0f, 0.0f, 1, 174.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10128, 10121, "", Common::Rect(3385, -227, 3600, 264), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10128, 1, 301.0f, 0.30000001f, 87.0f, 0.0f, 1, 301.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10128, 10121, "", Common::Rect(0, -227, 107, 264), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10128, 2, 301.0f, 0.30000001f, 87.0f, 0.0f, 1, 301.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10128, 10124, "", Common::Rect(1047, -250, 1501, 355), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10128, 3, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10128, 10127, "", Common::Rect(2958, -227, 3387, 346), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10128, 4, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10131, 10127, "", Common::Rect(219, -210, 683, 313), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10131, 0, 11.0f, 0.30000001f, 87.0f, 0.0f, 1, 11.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10131, 10142, "", Common::Rect(2531, -306, 2849, 285), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10131, 1, 265.0f, 0.30000001f, 87.0f, 0.0f, 1, 265.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10141, 10142, "", Common::Rect(1265, -243, 1687, 290), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10141, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10142, 10131, "", Common::Rect(1666, -66, 1938, 425), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10142, 0, 55.0f, 0.30000001f, 87.0f, 0.0f, 1, 55.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10142, 10141, "", Common::Rect(2898, -250, 3223, 334), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10142, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10151, 10124, "", Common::Rect(2773, -240, 3209, 331), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10151, 0, 259.0f, 0.30000001f, 87.0f, 0.0f, 1, 259.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10151, 10152, "", Common::Rect(182, -310, 493, 331), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10151, 1, 353.0f, 0.30000001f, 87.0f, 0.0f, 1, 353.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10151, 10154, "", Common::Rect(1135, -273, 1501, 318), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10151, 2, 86.0f, 0.30000001f, 87.0f, 0.0f, 1, 86.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10152, 10122, "", Common::Rect(3076, -327, 3517, 271), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10152, 0, 292.0f, 0.30000001f, 87.0f, 0.0f, 1, 292.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10152, 10151, "", Common::Rect(2026, -269, 2353, 325), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10152, 1, 173.0f, 0.30000001f, 87.0f, 0.0f, 1, 173.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10152, 10153, "", Common::Rect(1107, -278, 1418, 331), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10152, 2, 82.0f, 0.30000001f, 87.0f, 0.0f, 1, 82.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10153, 10152, "", Common::Rect(2926, -255, 3352, 313), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10153, 0, 262.0f, 0.30000001f, 87.0f, 0.0f, 1, 262.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10153, 10154, "", Common::Rect(2023, -268, 2348, 324), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10153, 1, 174.0f, 0.30000001f, 87.0f, 0.0f, 1, 174.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10154, 10151, "", Common::Rect(2902, -245, 3267, 329), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10154, 0, 266.0f, 0.30000001f, 87.0f, 0.0f, 1, 266.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10154, 10153, "", Common::Rect(242, -257, 624, 313), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10154, 1, 354.0f, 0.30000001f, 87.0f, 0.0f, 1, 354.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10155, 10151, "", Common::Rect(2318, -162, 2566, 133), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10155, 0, 220.0f, 0.30000001f, 87.0f, 0.0f, 1, 220.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10155, 10152, "", Common::Rect(3308, -261, 3600, 210), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10155, 1, 300.0f, 0.30000001f, 87.0f, 0.0f, 1, 300.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10155, 10152, "", Common::Rect(0, -261, 142, 210), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10155, 2, 300.0f, 0.30000001f, 87.0f, 0.0f, 1, 300.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10155, 10153, "", Common::Rect(601, -275, 1061, 213), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10155, 3, 25.0f, 0.30000001f, 87.0f, 0.0f, 1, 25.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10155, 10154, "", Common::Rect(1840, -185, 2088, 133), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10155, 4, 130.0f, 0.30000001f, 87.0f, 0.0f, 1, 130.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10161, 10123, "", Common::Rect(1239, -161, 1655, 453), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10161, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10161, 10162, "", Common::Rect(284, -231, 627, 336), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10161, 1, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10161, 10164, "", Common::Rect(2016, -292, 2583, 310), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10161, 2, 194.0f, 0.30000001f, 87.0f, 0.0f, 1, 194.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10162, 10161, "", Common::Rect(2097, -259, 2436, 320), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10162, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10162, 10166, "", Common::Rect(2977, -231, 3367, 313), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10162, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10163, 10166, "", Common::Rect(237, -199, 638, 329), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10163, 0, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10163, 10172, "", Common::Rect(2805, -212, 3049, 332), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10163, 1, 252.0f, 0.30000001f, 87.0f, 0.0f, 1, 252.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10163, 10173, "", Common::Rect(3118, -252, 3410, 308), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10163, 2, 288.0f, 0.30000001f, 87.0f, 0.0f, 1, 288.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10164, 10161, "", Common::Rect(520, -250, 670, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10164, 0, 14.0f, 0.30000001f, 87.0f, 0.0f, 1, 14.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10166, 10162, "", Common::Rect(1188, -220, 1546, 325), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10166, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10166, 10163, "", Common::Rect(2102, -283, 2436, 320), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10166, 1, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10171, 10172, "", Common::Rect(1378, -240, 1775, 266), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10171, 0, 113.0f, 0.30000001f, 87.0f, 0.0f, 1, 113.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10171, 10173, "", Common::Rect(921, -180, 1263, 332), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10171, 1, 67.0f, 0.30000001f, 87.0f, 0.0f, 1, 67.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10172, 10163, "", Common::Rect(776, -194, 1256, 311), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10172, 0, 72.0f, 0.30000001f, 87.0f, 0.0f, 1, 72.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10172, 10171, "", Common::Rect(3132, -169, 3396, 346), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10172, 1, 293.0f, 0.30000001f, 87.0f, 0.0f, 1, 293.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10173, 10163, "", Common::Rect(1388, -259, 1845, 315), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10173, 0, 108.0f, 0.30000001f, 87.0f, 0.0f, 1, 108.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10173, 10171, "", Common::Rect(2884, -159, 3178, 320), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10173, 1, 247.0f, 0.30000001f, 87.0f, 0.0f, 1, 247.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10181, 10126, "", Common::Rect(1961, -103, 2244, 201), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10181, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10191, 10125, "", Common::Rect(2005, -85, 2364, 322), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10191, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10101, 10122, "", Common::Rect(2248, -331, 2525, 234), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10101, 0, 189.0f, 0.30000001f, 87.0f, 0.0f, 1, 189.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20111, 20112, "", Common::Rect(110, -250, 260, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20111, 0, 333.0f, 0.30000001f, 87.0f, 0.0f, 1, 333.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20111, 20113, "", Common::Rect(560, -250, 710, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20111, 1, 18.0f, 0.30000001f, 87.0f, 0.0f, 1, 18.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20112, 20111, "", Common::Rect(1910, -250, 2060, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20112, 0, 153.0f, 0.30000001f, 87.0f, 0.0f, 1, 153.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20112, 20113, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20112, 1, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20112, 20114, "", Common::Rect(3470, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20112, 2, 309.0f, 0.30000001f, 87.0f, 0.0f, 1, 309.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20112, 20114, "", Common::Rect(0, -250, 20, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20112, 3, 309.0f, 0.30000001f, 87.0f, 0.0f, 1, 309.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20113, 20111, "", Common::Rect(2360, -250, 2510, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20113, 0, 198.0f, 0.30000001f, 87.0f, 0.0f, 1, 198.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20113, 20112, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20113, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20114, 20112, "", Common::Rect(1670, -250, 1820, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20114, 0, 129.0f, 0.30000001f, 87.0f, 0.0f, 1, 129.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20114, 20128, "", Common::Rect(320, -250, 470, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20114, 1, 354.0f, 0.30000001f, 87.0f, 0.0f, 1, 354.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20121, 20127, "", Common::Rect(2850, -250, 3000, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20121, 0, 247.0f, 0.30000001f, 87.0f, 0.0f, 1, 247.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20121, 20128, "", Common::Rect(1590, -250, 1740, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20121, 1, 121.0f, 0.30000001f, 87.0f, 0.0f, 1, 121.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20122, 20124, "", Common::Rect(2120, -250, 2270, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20122, 0, 174.0f, 0.30000001f, 87.0f, 0.0f, 1, 174.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20122, 20125, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20122, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20123, 20126, "", Common::Rect(340, -250, 490, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20123, 0, 356.0f, 0.30000001f, 87.0f, 0.0f, 1, 356.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20123, 20127, "", Common::Rect(2090, -250, 2240, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20123, 1, 171.0f, 0.30000001f, 87.0f, 0.0f, 1, 171.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20123, 20161, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20123, 2, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20124, 20122, "", Common::Rect(320, -250, 470, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20124, 0, 354.0f, 0.30000001f, 87.0f, 0.0f, 1, 354.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20124, 20128, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20124, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20125, 20122, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20125, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20125, 20126, "", Common::Rect(3010, -250, 3160, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20125, 1, 263.0f, 0.30000001f, 87.0f, 0.0f, 1, 263.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20126, 20123, "", Common::Rect(2140, -250, 2290, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20126, 0, 176.0f, 0.30000001f, 87.0f, 0.0f, 1, 176.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20126, 20125, "", Common::Rect(1210, -250, 1360, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20126, 1, 83.0f, 0.30000001f, 87.0f, 0.0f, 1, 83.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20127, 20121, "", Common::Rect(1050, -250, 1200, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20127, 0, 67.0f, 0.30000001f, 87.0f, 0.0f, 1, 67.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20127, 20123, "", Common::Rect(290, -250, 440, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20127, 1, 351.0f, 0.30000001f, 87.0f, 0.0f, 1, 351.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20127, 20128, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20127, 2, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20128, 20114, "", Common::Rect(2120, -250, 2270, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20128, 0, 174.0f, 0.30000001f, 87.0f, 0.0f, 1, 174.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20128, 20121, "", Common::Rect(3390, -250, 3540, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20128, 1, 301.0f, 0.30000001f, 87.0f, 0.0f, 1, 301.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20128, 20124, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20128, 2, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20128, 20127, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20128, 3, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20161, 20123, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20161, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20161, 20162, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20161, 1, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20161, 20163, "", Common::Rect(3200, -250, 3350, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20161, 2, 282.0f, 0.30000001f, 87.0f, 0.0f, 1, 282.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20161, 20164, "", Common::Rect(2320, -250, 2470, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20161, 3, 194.0f, 0.30000001f, 87.0f, 0.0f, 1, 194.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20161, 20166, "", Common::Rect(3570, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20161, 4, 319.0f, 0.30000001f, 87.0f, 0.0f, 1, 319.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20161, 20166, "", Common::Rect(0, -250, 120, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20161, 5, 319.0f, 0.30000001f, 87.0f, 0.0f, 1, 319.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20162, 20161, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20162, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20162, 20163, "", Common::Rect(2650, -250, 2800, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20162, 1, 227.0f, 0.30000001f, 87.0f, 0.0f, 1, 227.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20162, 20166, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20162, 2, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20163, 20161, "", Common::Rect(1400, -250, 1550, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20163, 0, 102.0f, 0.30000001f, 87.0f, 0.0f, 1, 102.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20163, 20162, "", Common::Rect(850, -250, 1000, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20163, 1, 47.0f, 0.30000001f, 87.0f, 0.0f, 1, 47.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20163, 20164, "", Common::Rect(1560, -250, 1710, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20163, 2, 118.0f, 0.30000001f, 87.0f, 0.0f, 1, 118.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20163, 20166, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20163, 3, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20163, 20172, "", Common::Rect(2900, -250, 3050, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20163, 4, 252.0f, 0.30000001f, 87.0f, 0.0f, 1, 252.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20163, 20173, "", Common::Rect(3260, -250, 3410, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20163, 5, 288.0f, 0.30000001f, 87.0f, 0.0f, 1, 288.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20164, 20161, "", Common::Rect(520, -250, 670, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20164, 0, 14.0f, 0.30000001f, 87.0f, 0.0f, 1, 14.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20164, 20163, "", Common::Rect(3360, -250, 3510, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20164, 1, 298.0f, 0.30000001f, 87.0f, 0.0f, 1, 298.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20166, 20162, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20166, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20166, 20163, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20166, 1, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20171, 20172, "", Common::Rect(1510, -250, 1660, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20171, 0, 113.0f, 0.30000001f, 87.0f, 0.0f, 1, 113.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20171, 20173, "", Common::Rect(1050, -250, 1200, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20171, 1, 67.0f, 0.30000001f, 87.0f, 0.0f, 1, 67.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20172, 20163, "", Common::Rect(1100, -250, 1250, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20172, 0, 72.0f, 0.30000001f, 87.0f, 0.0f, 1, 72.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20172, 20171, "", Common::Rect(3310, -250, 3460, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20172, 1, 293.0f, 0.30000001f, 87.0f, 0.0f, 1, 293.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20172, 20173, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20172, 2, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20173, 20163, "", Common::Rect(1460, -250, 1610, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20173, 0, 108.0f, 0.30000001f, 87.0f, 0.0f, 1, 108.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20173, 20171, "", Common::Rect(2850, -250, 3000, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20173, 1, 247.0f, 0.30000001f, 87.0f, 0.0f, 1, 247.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(20173, 20172, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(20173, 2, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->objectAdd(kObject10100, "", "", 1);
	_app->objectAddPresentation(kObject10100);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10111, 6, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10111, 7, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10112, 0, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10112, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10112, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10112, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10113, 0, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10113, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10113, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10113, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10114, 0, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10114, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10122, 4, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10122, 5, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10141, 5, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10141, 6, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10171, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10171, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10171, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10172, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10172, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10172, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10172, 4, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10173, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10173, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10173, 3, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10173, 4, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10181, 0, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10181, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10181, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10101, 0, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10101, 1, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10101, 2, 15, 12.5f, 16);
	_app->objectPresentationAddAnimationToRotation(kObject10100, 0, 10101, 3, 15, 12.5f, 16);
	_app->objectPresentationShow(kObject10100);
	_app->objectAdd(kObject10031, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10031, 10111, Common::Rect(1023, -161, 1253,  59), false, kCursorAction, 101);
	_app->objectAddRotationAccessibility(kObject10031, 10111, Common::Rect(3174, -145, 3392, 259), false, kCursorAction, 77);
	_app->objectAdd(kObject10000, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10000, 10111, Common::Rect(1033, 161, 1126, 212), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObject10000);
	_app->objectAddPresentation(kObject10000);
	_app->objectAddPresentation(kObject10000);
	_app->objectPresentationAddImageToPuzzle(kObject10000, 2, kPuzzle10000, "s01a01n01p01l02.bmp", Common::Point(150, 222), true, kDrawTypeNormal, 1000);
	_app->objectAddPresentation(kObject10000);
	_app->objectPresentationAddImageToPuzzle(kObject10000, 3, kPuzzle10000, "s01a01n01p01l01.bmp", Common::Point(247, 250), true, kDrawTypeNormal, 1000);
	_app->objectAddPresentation(kObject10000);
	_app->objectPresentationAddImageToPuzzle(kObject10000, 4, kPuzzle10000, "s01a01n01p01l03.bmp", Common::Point(382, 210), true, kDrawTypeNormal, 1000);
	_app->objectAdd(kObject10001, "", "", 1);
	_app->objectAddPuzzleAccessibility(kObject10001, kPuzzle10000, Common::Rect(0, 420, 640, 464), false, kCursorBack, 0);
	_app->objectAdd(kObject10006, "", "", 1);
	_app->objectAddPuzzleAccessibility(kObject10006, kPuzzle10001, Common::Rect(0, 420, 640, 464), false, kCursorBack, 0);
	_app->objectAdd(kObjectKey, "Key", "key", 1);
	_app->objectAddRotationAccessibility(kObjectKey, 10111, Common::Rect(1302, 357, 1422, 424), false, kCursorTake, 0);
	_app->objectAddPresentation(kObjectKey);
	_app->objectPresentationAddImageToRotation(kObjectKey, 0, 10111, 0);
	_app->objectAdd(kObjectAmulet, "Amulet", "amulet", 1);
	_app->objectAddPuzzleAccessibility(kObjectAmulet, kPuzzle10000, Common::Rect(152, 237, 253, 317), true, kCursorTake, 0);
	_app->objectAdd(kObjectContract, "Contract", "ntract", 1);
	_app->objectAddPuzzleAccessibility(kObjectContract, kPuzzle10000, Common::Rect(254, 242, 366, 342), true, kCursorZoom, 0);
	_app->objectAdd(kObjectRopes, "Ropes", "rope", 1);
	_app->objectAddPuzzleAccessibility(kObjectRopes, kPuzzle10000, Common::Rect(370, 243, 479, 345), true, kCursorTake, 0);
	_app->objectAdd(kObject10007, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10007, 10111, Common::Rect(2385, 3, 2536, 248), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject10007, 10111, Common::Rect(2385, 3, 2536, 248), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject10007);
	_app->objectPresentationAddImageToRotation(kObject10007, 0, 10111, 2);
	_app->objectPresentationAddAnimationToRotation(kObject10007, 0, 10111, 4, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject10007, 0, 0, 10000);
	_app->objectAddPresentation(kObject10007);
	_app->objectPresentationAddAnimationToPuzzle(kObject10007, 1, kPuzzle10007, "s01a01n01p02s01", kImageTypeBMP, Common::Point(320, 16), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10007, 1, 0, 10001);
	_app->objectAddPresentation(kObject10007);
	_app->objectPresentationAddAnimationToPuzzle(kObject10007, 2, kPuzzle10007, "s01a01n01p02s02", kImageTypeBMP, Common::Point(120, 72), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10007, 2, 0, 10002);
	_app->objectAddPresentation(kObject10007);
	_app->objectPresentationAddAnimationToPuzzle(kObject10007, 3, kPuzzle10007, "s01a01n01p02s03", kImageTypeBMP, Common::Point(419, 126), kDrawTypeNormal, 1000, 30, 12.5f, 4);
	_app->objectAdd(kObject10008, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10008, 10111, Common::Rect(1963, 8, 2095, 278), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject10008, 10111, Common::Rect(1963, 8, 2095, 278), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject10008);
	_app->objectPresentationAddAnimationToRotation(kObject10008, 0, 10111, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject10008, 0, 0, 10001);
	_app->objectAddPresentation(kObject10008);
	_app->objectPresentationAddAnimationToPuzzle(kObject10008, 1, kPuzzle10010, "s01a01n01p05s01", kImageTypeBMP, Common::Point(134, 171), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10008, 1, 0, 10001);
	_app->objectAdd(kObject10009, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10009, 10111, Common::Rect(1655, 8, 1778, 262), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject10009, 10111, Common::Rect(1655, 8, 1778, 262), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject10009);
	_app->objectPresentationAddImageToRotation(kObject10009, 0, 10111, 3);
	_app->objectPresentationAddAnimationToRotation(kObject10009, 0, 10111, 5, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject10009, 0, 0, 10002);
	_app->objectAddPresentation(kObject10009);
	_app->objectPresentationAddAnimationToPuzzle(kObject10009, 1, kPuzzle10013, "s01a01n01p08s01", kImageTypeBMP, Common::Point(480, 25), kDrawTypeNormal, 1000, 21, 12.5f, 16);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10009, 1, 0, 10001);
	_app->objectAddPresentation(kObject10009);
	_app->objectPresentationAddAnimationToPuzzle(kObject10009, 2, kPuzzle10013, "s01a01n01p08s02", kImageTypeBMP, Common::Point(23, 31), kDrawTypeNormal, 1000, 21, 12.5f, 16);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10009, 2, 0, 10002);
	_app->objectAdd(kObject10010, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10010, 10122, Common::Rect(933, -36, 1074, 308), false, kCursorAction, 0);
	_app->objectAddPresentation(kObject10010);
	_app->objectPresentationAddImageToRotation(kObject10010, 0, 10122, 2);
	_app->objectPresentationAddAnimationToRotation(kObject10010, 0, 10122, 3, 15, 12.5f, 4);
	_app->objectPresentationAddImageToRotation(kObject10010, 0, 10124, 0);
	_app->objectPresentationAddImageToRotation(kObject10010, 0, 10125, 0);
	_app->objectPresentationAddImageToRotation(kObject10010, 0, 10126, 0);
	_app->objectPresentationSetAnimationOnRotation(kObject10010, 0, 0, 10001);
	_app->objectAdd(kObject10011, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10011, 10191, Common::Rect(356, 49, 471, 240), false, kCursorAction, 0);
	_app->objectAddPresentation(kObject10011);
	_app->objectPresentationAddImageToRotation(kObject10011, 0, 10191, 0);
	_app->objectPresentationAddAnimationToRotation(kObject10011, 0, 10191, 1, 15, 12.5f, 4);
	_app->objectPresentationAddImageToRotation(kObject10011, 0, 10125, 1);
	_app->objectPresentationSetAnimationOnRotation(kObject10011, 0, 0, 10001);
	_app->objectAddPresentation(kObject10011);
	_app->objectPresentationAddAnimationToPuzzle(kObject10011, 1, kPuzzle10016, "s01a09n01p02s01", kImageTypeBMP, Common::Point(62, 68), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10011, 1, 0, 10001);
	_app->objectAddPresentation(kObject10011);
	_app->objectPresentationAddAnimationToPuzzle(kObject10011, 2, kPuzzle10016, "s01a09n01p02s02", kImageTypeBMP, Common::Point(396, 108), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10011, 2, 0, 10002);
	_app->objectAdd(kObject10012, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10012, 10191, Common::Rect(488, 227, 592, 313), false, kCursorAction, 0);
	_app->objectAddPuzzleAccessibility(kObject10012, kPuzzle10002, Common::Rect(0, 420, 640, 464), false, kCursorBack, 1);
	_app->objectAdd(kObjectWine, "Wine", "Wine", 1);
	_app->objectAddRotationAccessibility(kObjectWine, 10191, Common::Rect(207, 164, 288, 317), false, kCursorTake, 0);
	_app->objectAddPresentation(kObjectWine);
	_app->objectPresentationAddImageToRotation(kObjectWine, 0, 10191, 3);
	_app->objectPresentationAddImageToRotation(kObjectWine, 0, 10125, 3);
	_app->objectAdd(kObjectLamp, "Lamp", "Lamp", 1);
	_app->objectAddRotationAccessibility(kObjectLamp, 10191, Common::Rect(341, 238, 434, 317), false, kCursorTake, 0);
	_app->objectAddPresentation(kObjectLamp);
	_app->objectPresentationAddImageToRotation(kObjectLamp, 0, 10191, 2);
	_app->objectPresentationAddImageToRotation(kObjectLamp, 0, 10125, 2);
	_app->objectAdd(kObjectIncense, "incense_stick", "astick", 1);
	_app->objectAddPuzzleAccessibility(kObjectIncense, kPuzzle10002, Common::Rect(199, 212, 445, 344), false, kCursorTake, 0);
	_app->objectAddPresentation(kObjectIncense);
	_app->objectPresentationAddImageToPuzzle(kObjectIncense, 0, kPuzzle10002, "s01a09n01p01l01.bmp", Common::Point(179, 224), true, kDrawTypeNormal, 1000);
	_app->objectAdd(kObject10017, "", "", 1);
	_app->objectAddPresentation(kObject10017);
	_app->objectPresentationAddImageToRotation(kObject10017, 0, 10163, 0);
	_app->objectPresentationAddImageToRotation(kObject10017, 0, 10171, 0);
	_app->objectPresentationAddImageToRotation(kObject10017, 0, 10172, 0);
	_app->objectPresentationAddImageToRotation(kObject10017, 0, 10173, 0);
	_app->objectAdd(kObject10018, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10018, 10122, Common::Rect(933, -36, 1074, 308), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject10018, 10122, Common::Rect(933, -36, 1074, 308), false, kCursorIdle, 1);
	_app->objectAddRotationAccessibility(kObject10018, 10122, Common::Rect(933, -36, 1074, 308), false, kCursorActionObject, 2);
	_app->objectAddRotationAccessibility(kObject10018, 10122, Common::Rect(933, -36, 1074, 308), false, kCursorIdle, 3);
	_app->objectAddRotationAccessibility(kObject10018, 10122, Common::Rect(933, -36, 1074, 308), false, kCursorAction, 4);
	_app->objectAddRotationAccessibility(kObject10018, 10122, Common::Rect(933, -36, 1074, 308), false, kCursorActionObject, 5);
	_app->objectAddPresentation(kObject10018);
	_app->objectPresentationAddImageToRotation(kObject10018, 0, 10122, 0);
	_app->objectPresentationAddAnimationToRotation(kObject10018, 0, 10122, 1, 15, 12.5f, 4);
	_app->objectPresentationAddImageToRotation(kObject10018, 0, 10124, 0);
	_app->objectPresentationAddImageToRotation(kObject10018, 0, 10125, 0);
	_app->objectPresentationAddImageToRotation(kObject10018, 0, 10126, 0);
	_app->objectPresentationSetAnimationOnRotation(kObject10018, 0, 0, 10001);
	_app->objectAddPresentation(kObject10018);
	_app->objectPresentationAddAnimationToPuzzle(kObject10018, 1, kPuzzle10017, "s01a02n02p01s01", kImageTypeBMP, Common::Point(180, 114), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10018, 1, 0, 10001);
	_app->objectAddPresentation(kObject10018);
	_app->objectPresentationAddAnimationToPuzzle(kObject10018, 2, kPuzzle10017, "s01a02n02p01s02", kImageTypeBMP, Common::Point(418, 94), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10018, 2, 0, 10002);
	_app->objectAdd(kObject10020, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10020, 10141, Common::Rect(3158, -8, 3408, 254), false, kCursorAction, 0);
	_app->objectAddPuzzleAccessibility(kObject10020, kPuzzle10003, Common::Rect(201, 16, 420, 91), false, kCursorAction, 1);
	_app->objectAddPresentation(kObject10020);
	_app->objectPresentationAddImageToRotation(kObject10020, 0, 10141, 0);
	_app->objectPresentationAddImageToRotation(kObject10020, 0, 10142, 0);
	_app->objectPresentationAddAnimationToRotation(kObject10020, 0, 10141, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject10020, 0, 0, 10001);
	_app->objectAddPresentation(kObject10020);
	_app->objectPresentationAddAnimationToPuzzle(kObject10020, 1, kPuzzle10021, "s01a04n01p04s01", kImageTypeBMP, Common::Point(137, 296), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10020, 1, 0, 10001);
	_app->objectAddPresentation(kObject10020);
	_app->objectPresentationAddAnimationToPuzzle(kObject10020, 2, kPuzzle10021, "s01a04n01p04s02", kImageTypeBMP, Common::Point(312, 16), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectAdd(kObjectEggs, "Eggs", "d_eggs", 1);
	_app->objectAddPuzzleAccessibility(kObjectEggs, kPuzzle10003, Common::Rect(19, 227, 190, 372), false, kCursorTake, 0);
	_app->objectAddPuzzleAccessibility(kObjectEggs, kPuzzle10003, Common::Rect(19, 227, 190, 372), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObjectEggs);
	_app->objectPresentationAddImageToPuzzle(kObjectEggs, 0, kPuzzle10003, "s01a04n01p01l02.tga", Common::Point(3, 220), true, kDrawTypeAlpha, 1000);
	_app->objectAdd(kObjectTuna, "Tuna", "d_tuna", 1);
	_app->objectAddPuzzleAccessibility(kObjectTuna, kPuzzle10003, Common::Rect(221, 213, 389, 363), false, kCursorTake, 0);
	_app->objectAddPuzzleAccessibility(kObjectTuna, kPuzzle10003, Common::Rect(221, 213, 389, 363), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObjectTuna);
	_app->objectPresentationAddImageToPuzzle(kObjectTuna, 0, kPuzzle10003, "s01a04n01p01l03.tga", Common::Point(207, 209), true, kDrawTypeAlpha, 1000);
	_app->objectAdd(kObjectMussels, "Mussels", "ussels", 1);
	_app->objectAddPuzzleAccessibility(kObjectMussels, kPuzzle10003, Common::Rect(433, 204, 605, 351), false, kCursorTake, 0);
	_app->objectAddPuzzleAccessibility(kObjectMussels, kPuzzle10003, Common::Rect(433, 204, 605, 351), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObjectMussels);
	_app->objectPresentationAddImageToPuzzle(kObjectMussels, 0, kPuzzle10003, "s01a04n01p01l01.tga", Common::Point(420, 198), true, kDrawTypeAlpha, 1000);
	_app->objectAdd(kObjectDuck, "Duck", "urpins", 1);
	_app->objectAddPuzzleAccessibility(kObjectDuck, kPuzzle10003, Common::Rect(19, 227, 190, 372), false, kCursorTake, 0);
	_app->objectAddPuzzleAccessibility(kObjectDuck, kPuzzle10003, Common::Rect(19, 227, 190, 372), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObjectDuck);
	_app->objectPresentationAddImageToPuzzle(kObjectDuck, 0, kPuzzle10003, "s01a04n01p02l02.tga", Common::Point(3, 220), true, kDrawTypeAlpha, 1000);
	_app->objectAdd(kObjectDugs, "Dugs", "w_dugs", 1);
	_app->objectAddPuzzleAccessibility(kObjectDugs, kPuzzle10003, Common::Rect(221, 213, 389, 363), false, kCursorTake, 0);
	_app->objectAddPuzzleAccessibility(kObjectDugs, kPuzzle10003, Common::Rect(221, 213, 389, 363), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObjectDugs);
	_app->objectPresentationAddImageToPuzzle(kObjectDugs, 0, kPuzzle10003, "s01a04n01p02l01.tga", Common::Point(207, 209), true, kDrawTypeAlpha, 1000);
	_app->objectAdd(kObjectDulcia, "Dulcia", "Dulcia", 1);
	_app->objectAddPuzzleAccessibility(kObjectDulcia, kPuzzle10003, Common::Rect(19, 227, 190, 372), false, kCursorTake, 0);
	_app->objectAddPuzzleAccessibility(kObjectDulcia, kPuzzle10003, Common::Rect(19, 227, 190, 372), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObjectDulcia);
	_app->objectPresentationAddImageToPuzzle(kObjectDulcia, 0, kPuzzle10003, "s01a04n01p03l02.tga", Common::Point(3, 220), true, kDrawTypeAlpha, 1000);
	_app->objectAdd(kObjectDates2, "Dates", "adates", 1);
	_app->objectAddPuzzleAccessibility(kObjectDates2, kPuzzle10003, Common::Rect(221, 213, 389, 363), false, kCursorTake, 0);
	_app->objectAddPuzzleAccessibility(kObjectDates2, kPuzzle10003, Common::Rect(221, 213, 389, 363), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObjectDates2);
	_app->objectPresentationAddImageToPuzzle(kObjectDates2, 0, kPuzzle10003, "s01a04n01p03l03.tga", Common::Point(207, 209), true, kDrawTypeAlpha, 1000);
	_app->objectAdd(kObjectOysters, "Oisters", "isters", 1);
	_app->objectAddPuzzleAccessibility(kObjectOysters, kPuzzle10003, Common::Rect(433, 204, 605, 351), false, kCursorTake, 0);
	_app->objectAddPuzzleAccessibility(kObjectOysters, kPuzzle10003, Common::Rect(433, 204, 605, 351), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObjectOysters);
	_app->objectPresentationAddImageToPuzzle(kObjectOysters, 0, kPuzzle10003, "s01a04n01p03l01.tga", Common::Point(420, 198), true, kDrawTypeAlpha, 1000);
	_app->objectAdd(kObjectRedFish, "Red Fish", "fish", 1);
	_app->objectAddRotationAccessibility(kObjectRedFish, 10121, Common::Rect(446, 322, 551, 359), false, kCursorTake, 0);
	_app->objectAddPresentation(kObjectRedFish);
	_app->objectPresentationAddAnimationToRotation(kObjectRedFish, 0, 10121, 0, 30, 12.5f, 5);
	_app->objectAdd(kObjectNightingale, "Nightingale", "ingale", 1);
	_app->objectAdd(kObject10101, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10101, 10111, Common::Rect(2046, -289, 2337, 308), false, kCursorMove, 0);
	_app->objectAdd(kObject10102, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10102, 10171, Common::Rect(3099, -161, 3239, 162), false, kCursorMove, 0);
	_app->soundAdd(10039, kSoundTypeAmbientEffect, "01_BAG-1.WAv", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(10040, kSoundTypeAmbientEffect, "01_OPE-1.WAv", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(10041, kSoundTypeAmbientEffect, "01_TAK-4.WAv", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(10042, kSoundTypeAmbientEffect, "01_TAK-1.WAv", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(10043, kSoundTypeAmbientEffect, "01_TAK-2.WAv", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(10044, kSoundTypeAmbientEffect, "01_LOC-1.WAv", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(10045, kSoundTypeAmbientEffect, "01_CLO-1.WAv", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(10046, kSoundTypeAmbientEffect, "01_GIV-1.WAv", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(10047, kSoundTypeAmbientEffect, "01_TAK-3.WAv", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(10048, kSoundTypeAmbientEffect, "01_PUT-1.WAv", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1001, kSoundTypeDialog, "1001.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1002, kSoundTypeDialog, "1002.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1006, kSoundTypeDialog, "1006.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1007, kSoundTypeDialog, "1007.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1003, kSoundTypeDialog, "1003.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1004, kSoundTypeDialog, "1004.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1005, kSoundTypeDialog, "1005.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1008, kSoundTypeDialog, "1008.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1013, kSoundTypeDialog, "1013.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1014, kSoundTypeDialog, "1014.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1012, kSoundTypeDialog, "1012.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1009, kSoundTypeDialog, "1009.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1010, kSoundTypeDialog, "1010.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1011, kSoundTypeDialog, "1011.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1015, kSoundTypeDialog, "1015.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1016, kSoundTypeDialog, "1016.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1017, kSoundTypeDialog, "1017.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1018, kSoundTypeDialog, "1018.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1019, kSoundTypeDialog, "1019.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1020, kSoundTypeDialog, "1020.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1021, kSoundTypeDialog, "1021.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1022, kSoundTypeDialog, "1022.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1023, kSoundTypeDialog, "1023.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1024, kSoundTypeDialog, "1024.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2027, kSoundTypeDialog, "2027.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3001, kSoundTypeDialog, "3001.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4001, kSoundTypeDialog, "4001.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4002, kSoundTypeDialog, "4002.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4003, kSoundTypeDialog, "4003.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4004, kSoundTypeDialog, "4004.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4005, kSoundTypeDialog, "4005.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4006, kSoundTypeDialog, "4006.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4007, kSoundTypeDialog, "4007.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4094, kSoundTypeDialog, "4094.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4095, kSoundTypeDialog, "4095.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4096, kSoundTypeDialog, "4096.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4097, kSoundTypeDialog, "4097.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4098, kSoundTypeDialog, "4098.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4099, kSoundTypeDialog, "4099.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4100, kSoundTypeDialog, "4100.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4101, kSoundTypeDialog, "4101.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4102, kSoundTypeDialog, "4102.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4103, kSoundTypeDialog, "4103.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4104, kSoundTypeDialog, "4104.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4105, kSoundTypeDialog, "4105.WAV", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4106, kSoundTypeAmbientEffect, "4106.WAV", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundSetVolume(4106, 90);
	_app->soundAdd(1100, kSoundTypeAmbientMusic, "BGR_MU-1.WAv", _app->getConfiguration().backgroundMusic.loadFrom);
	_app->rotationAddAmbientSound(10111, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10112, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10113, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10114, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10121, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10122, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10123, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10124, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10125, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10126, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10127, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10128, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10131, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10141, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10142, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10151, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10152, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10153, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10154, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10155, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10161, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10162, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10163, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10164, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10166, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10171, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10172, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10173, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10181, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10191, 1100, 90, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10101, 1100, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle10000, 1100, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle10001, 1100, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle10002, 1100, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle10003, 1100, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle10007, 1100, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle10010, 1100, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle10013, 1100, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle10016, 1100, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle10017, 1100, 90, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle10021, 1100, 90, 0, 1, 1, 10);
	_app->varDefineByte(10000, 0);
	_app->varDefineByte(10001, 0);
	_app->varDefineByte(10002, 0);
	_app->varDefineByte(10003, 0);
	_app->varDefineByte(10004, 0);
	_app->varDefineByte(10005, 0);
	_app->varDefineByte(10006, 0);
	_app->varDefineByte(10008, 0);
	_app->varDefineByte(10007, 0);
	_app->varDefineByte(10024, 0);
	_app->varDefineByte(10025, 0);
	_app->varDefineByte(10026, 0);
	_app->varDefineByte(10009, 0);
	_app->varDefineByte(10010, 0);
	_app->varDefineByte(10011, 0);
	_app->varDefineByte(10012, 0);
	_app->varDefineByte(10013, 0);
	_app->varDefineByte(10014, 0);
	_app->varDefineByte(10015, 0);
	_app->varDefineByte(10016, 1);
	_app->varDefineByte(10017, 2);
	_app->varDefineByte(10018, 3);
	_app->varDefineByte(10019, 1);
	_app->varDefineByte(10020, 2);
	_app->varDefineByte(10021, 1);
	_app->varDefineByte(10022, 2);
	_app->varDefineByte(10023, 3);
	_app->varDefineByte(10027, 0);
	_app->varDefineByte(10028, 0);
	_app->varDefineByte(10029, 0);
	_app->varDefineByte(10030, 0);
}

void Zone2Pompeii::onSetup(SetupType type) {
	switch (type) {
	default:
		break;

	case kSetupTypeNone:
		_app->timerStopAll();
		_app->bagRemoveAll();
		_app->setSpace(kZoneSY);
		_app->varSetByte(90001, 1);
		_app->showDay();
		_app->rotationSetMovabilityOff(10011, 1, 1);
		_app->rotationSetMovabilityOff(10012, 1, 1);
		_app->rotationSetMovabilityOff(10012, 2, 2);
		_app->rotationSetMovabilityOff(10015, 1, 1);
		_app->rotationSetMovabilityOff(10211, 0, 0);
		_app->objectSetAccessibilityOff(kObject10045);
		_app->objectSetAccessibilityOff(kObject10046);
		_app->objectSetAccessibilityOff(kObject10047);
		_app->objectSetAccessibilityOff(kObject10053);
		_app->objectSetAccessibilityOff(kObject10201);
		_app->rotationSetMovabilityOff(10155);
		_app->setSpace(kZone2);
		_app->rotationSetAlp(10155, 175.0f);
		_app->rotationSetBet(10155, 0.0f);
		_app->rotationSetRan(10155, 87.0f);
		_app->rotationSetActive(10155);
		_app->objectPresentationShow(kObjectKey);
		_app->objectSetAccessibilityOn(kObjectKey);
		_app->objectSetAccessibilityOn(kObject10000, 0, 0);
		_app->objectPresentationShow(kObject1000);
		_app->objectPresentationShow(kObject1039, 0);
		_app->objectPresentationShow(kObject1039, 1);
		_app->objectPresentationShow(kObject1040, 0);
		_app->objectPresentationShow(kObject1041, 0);
		_app->objectPresentationShow(kObject1035, 0);
		_app->objectPresentationShow(kObjectBasket, 0);
		_app->objectPresentationShow(kObjectBasket2, 0);
		_app->soundPlay(1001);
		break;

	case kSetupType1:
		_app->varSetByte(90001, 0);
		_app->objectSetAccessibilityOn(kObject10041);
		_app->objectSetAccessibilityOn(kObject10042);
		_app->objectSetAccessibilityOn(kObject10043);
		_app->objectSetAccessibilityOn(kObject10044);
		_app->objectSetAccessibilityOn(kObject10045);
		_app->objectSetAccessibilityOn(kObject10046);
		_app->objectSetAccessibilityOn(kObject10047);
		_app->objectSetAccessibilityOn(kObject10048);
		_app->objectSetAccessibilityOn(kObject10049);
		_app->objectSetAccessibilityOn(kObject10050);
		_app->objectSetAccessibilityOn(kObject10051);
		_app->objectSetAccessibilityOn(kObject10052);
		_app->objectSetAccessibilityOn(kObject10053);
		_app->objectSetAccessibilityOn(kObject10054);
		_app->objectSetAccessibilityOn(kObject10101);
		_app->objectSetAccessibilityOn(kObject10102);
		_app->objectSetAccessibilityOn(kObject10201);
		_app->objectSetAccessibilityOn(kObject10202);
		_app->objectSetAccessibilityOn(kObject30301);
		_app->objectSetAccessibilityOn(kObject10405);
		_app->objectSetAccessibilityOn(kObject50501);
		_app->objectSetAccessibilityOn(kObject10601);
		_app->objectSetAccessibilityOn(kObject10701);
		_app->objectSetAccessibilityOn(kObject10801);
		_app->objectSetAccessibilityOn(kObject10901);
		_app->objectSetAccessibilityOn(kObject10902);
		_app->objectSetAccessibilityOn(kObject10903);
		_app->objectSetAccessibilityOn(kObject11001);
		_app->objectSetAccessibilityOn(kObject11101);
		_app->setSpace(kZoneSY);
		_app->bagAdd(kObjectAmulet);
		_app->rotationSetAlp(100111, 260.0f);
		_app->rotationSetBet(100111, 0.30000001f);
		_app->rotationSetRan(100111, 87.0f);
		_app->rotationSetActive(100111);
		break;

	case kSetupType200:
		_app->objectPresentationShow(kObject1000);
		_app->objectPresentationShow(kObject1039, 0);
		_app->objectPresentationShow(kObject1039, 1);
		_app->objectPresentationShow(kObject1040, 0);
		_app->objectPresentationShow(kObject1041, 0);
		_app->objectPresentationShow(kObject1035, 0);
		_app->objectPresentationShow(kObjectBasket, 0);
		_app->objectPresentationShow(kObjectBasket2, 0);
		_app->objectPresentationShow(kObject50501, 9);
		_app->onCall(2340);
		_app->onCall(200);
		break;

	case kSetupType300:
		_app->objectPresentationShow(kObject50501, 9);
		_app->onCall(2340);
		_app->onCall(300);
		break;

	case kSetupType400:
		_app->objectSetAccessibilityOff(kObject10048);
		_app->objectPresentationShow(kObject50501, 9);
		_app->onCall(2340);
		_app->onCall(400);
		break;
	}
}

void Zone2Pompeii::onButtonUp(ObjectId id, Id target, Id /*puzzleRotationId*/, uint32 /*a4*/, const Common::Point &/*point*/) {
	if (id == kObjectExitEncyclopedia)
		_app->restore();

	switch (_app->varGetByte(90001)) {
	default:
		break;

	case 0:
		switch (id) {
		default:
			break;

		case kObject10101:
			_app->onCall(10100);
			break;

		case kObject10102:
			_app->onCall(102003);
			break;
		}
		break;

	case 1:
		switch (id) {
		default:
			break;

		case kObject10000:
			if (_app->bagHasClickedObject()) {
				if (_app->bagGetClickedObject() != kObjectKey) {
					_app->cursorDelete();
					break;
				}

				_app->soundPlay(10040);
				_app->bagRemove(kObjectKey);
				_app->objectSetAccessibilityOff(kObject10000);
				_app->objectSetAccessibilityOff(kObject10000);
				_app->varSetByte(10000, 1);
				_app->objectSetAccessibilityOn(kObject10001);
				_app->puzzleSetActive(kPuzzle10000);
				_app->cursorDelete();
			}
			break;

		case kObject10001:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && _app->varGetByte(10000) == 1) {
					_app->soundPlay(10045);
					_app->objectSetAccessibilityOff(kObject10001);
					_app->rotationSetActive(10111);
					_app->soundPlay(10044);
					_app->objectPresentationHide(kObject10000, 1);
					_app->varSetByte(10000, 0);
					_app->objectSetAccessibilityOn(kObject10000);

					if (_app->varGetByte(10002) != 1 || _app->varGetByte(10003) != 1 || _app->varGetByte(10004) != 1) {
						_app->bagAdd(kObjectKey);
					} else {
						_app->objectSetAccessibilityOff(kObject10000);
						_app->bagRemove(kObjectKey);
						onVisual(101);
					}
				}
			}
			break;

		case kObjectKey:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && !_app->varGetByte(10001)) {
					_app->soundPlay(10039);
					_app->objectPresentationHide(kObjectKey);
					_app->objectSetAccessibilityOff(kObjectKey);
					_app->bagAdd(kObjectKey);
					_app->varSetByte(10001, 1);
					_app->objectSetAccessibilityOn(kObject10000);
				}
			}
			break;

		case kObjectAmulet:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && _app->varGetByte(10000) == 1 && !_app->varGetByte(10002)) {
					_app->soundPlay(10041);
					_app->objectPresentationShow(kObject10000, 2);
					_app->objectSetAccessibilityOff(kObjectAmulet, 0, 0);
					_app->bagAdd(kObjectAmulet);
					_app->varSetByte(10002, 1);
				}
			}
			break;

		case kObjectContract:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && _app->varGetByte(10000) == 1 && !_app->varGetByte(10003)) {
					_app->objectPresentationShow(kObject10000, 3);
					_app->objectSetAccessibilityOff(kObjectContract, 0, 0);
					_app->puzzleSetActive(kPuzzle10001);
					_app->objectSetAccessibilityOn(kObject10006);
					_app->soundPlay(10042);
				}
			}
			break;

		case kObjectRopes:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && _app->varGetByte(10000) == 1 && !_app->varGetByte(10004)) {
					_app->soundPlay(10043);
					_app->objectPresentationShow(kObject10000, 4);
					_app->objectSetAccessibilityOff(kObjectRopes, 0, 0);
					_app->bagAdd(kObjectRopes);
					_app->varSetByte(10004, 1);
				}
			}
			break;

		case kObject10006:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && !_app->varGetByte(10003)) {
					_app->soundPlay(10042);
					_app->objectSetAccessibilityOff(kObject10006);
					_app->bagAdd(kObjectContract);
					_app->varSetByte(10003, 1);
					_app->puzzleSetActive(kPuzzle10000);
				}
			}
			break;

		case kObject10007:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && !_app->varGetByte(10005)) {
					_app->objectSetAccessibilityOff(kObject10007, 0, 0);
					_app->puzzleSetActive(kPuzzle10007);
					_app->soundPlay(1002);
					_app->varSetByte(10005, 1);
				}
			}
			break;

		case kObject10008:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && !_app->varGetByte(10006)) {
					_app->objectSetAccessibilityOff(kObject10008, 0, 0);
					_app->puzzleSetActive(kPuzzle10010);
					_app->soundPlay(1008);
					_app->varSetByte(10006, 1);
				}
			}
			break;

		case kObject10009:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && !_app->varGetByte(10007)) {
					_app->objectSetAccessibilityOff(kObject10009, 0, 0);
					_app->puzzleSetActive(kPuzzle10013);
					_app->soundPlay(1015);
					_app->varSetByte(10007, 1);
				}
			}
			break;

		case kObject10031:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (target == 101 || target == 77)
					_app->showEncyclopedia((uint32)target);
			}
			break;

		case kObject10101:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (_app->varGetByte(90103) == 2) {
					_app->varSetByte(90103, 3);
					_app->onCall(1039);
					_app->rotationSetMovabilityOn(10111, 0, 0);
					_app->rotationSetMovabilityOn(10111, 1, 1);
				}

				_app->objectPresentationPauseAnimation(kObject10007, 0);
				_app->objectPresentationPauseAnimation(kObject10007, 1);
				_app->objectPresentationPauseAnimation(kObject10008, 0);
				_app->objectPresentationPauseAnimation(kObject10008, 1);
				_app->objectPresentationPauseAnimation(kObject10009, 1);
				_app->objectPresentationPauseAnimation(kObject10010, 0);
				_app->objectPresentationPauseAnimation(kObject10011, 0);
				_app->objectPresentationPauseAnimation(kObject10011, 1);
				_app->objectPresentationPauseAnimation(kObject10018, 0);
				_app->objectPresentationPauseAnimation(kObject10018, 1);
				_app->objectPresentationPauseAnimation(kObject10020, 0);
				_app->objectPresentationPauseAnimation(kObject10020, 1);
				_app->objectPresentationPauseAnimation(kObject10007, 2);
				_app->objectPresentationPauseAnimation(kObject10009, 0);
				_app->objectPresentationPauseAnimation(kObject10009, 2);
				_app->objectPresentationPauseAnimation(kObject10011, 2);
				_app->objectPresentationPauseAnimation(kObject10018, 2);
				_app->timerStopAll();
				_app->onCall(10100);

				if (!_app->varGetByte(90104))
					_app->onCall(104);
			}
			break;
		}
		break;

	case 2:
		switch (id) {
		default:
			break;

		case kObject10010:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && !_app->varGetByte(10009)) {
					_app->objectSetAccessibilityOff(kObject10010);
					_app->playMovie("S01A02-1");
					_app->varSetByte(10009, 1);
					_app->objectSetAccessibilityOn(kObject10101);
				}
			}
			break;

		case kObject10011:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && !_app->varGetByte(10010)) {
					_app->objectSetAccessibilityOff(kObject10011);
					_app->puzzleSetActive(kPuzzle10016);
					_app->soundPlay(2027);
					_app->varSetByte(10010, 1);
					_app->objectSetAccessibilityOn(kObject10012, 0, 0);
					_app->objectSetAccessibilityOn(kObjectWine, 0, 0);
					_app->objectSetAccessibilityOn(kObjectLamp, 0, 0);
				}
			}
			break;

		case kObject10012:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				switch (target) {
				default:
					break;

				case 0:
					_app->objectSetAccessibilityOff(kObject10012, 0, 0);
					_app->soundPlay(900013);
					_app->objectSetAccessibilityOff(kObject10012, 0, 0);
					_app->puzzleSetActive(kPuzzle10002);
					_app->objectSetAccessibilityOn(kObject10012, 1, 1);

					if (!_app->varGetByte(91209))
						_app->objectSetAccessibilityOn(kObjectIncense, 0, 0);
					break;

				case 1:
					_app->objectSetAccessibilityOff(kObject10012, 1, 1);
					_app->rotationSetActive(10191);
					_app->soundPlay(900014);

					if (!_app->varGetByte(91209))
						_app->objectSetAccessibilityOn(kObject10012, 0, 0);
					break;
				}
			}
			break;

		case kObjectWine:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && !_app->varGetByte(10011)) {
					_app->soundPlay(900010);
					_app->bagAdd(kObjectPureWine);
					_app->objectSetAccessibilityOff(kObjectWine);
					_app->objectPresentationHide(kObjectWine);
					_app->varSetByte(10011, 1);
				}
			}
			break;

		case kObjectIncense:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && !_app->varGetByte(10013)) {
					_app->soundPlay(900011);
					_app->bagAdd(kObjectIncenseStick);
					_app->objectPresentationShow(kObjectIncense, 0);
					_app->objectSetAccessibilityOff(kObjectIncense, 0, 0);
					_app->varSetByte(10013, 1);
				}
			}
			break;

		case kObjectLamp:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target && !_app->varGetByte(10012)) {
					_app->soundPlay(900012);
					_app->bagAdd(kObjectLamp2);
					_app->objectSetAccessibilityOff(kObjectLamp);
					_app->objectPresentationHide(kObjectLamp);
					_app->varSetByte(10012, 1);
				}
			}
			break;

		case kObject10101:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (_app->varGetByte(90201) == 2) {
					_app->varSetByte(90201, 3);
					_app->onCall(2009);
				} else if (_app->varGetByte(90207) == 2) {
					_app->varSetByte(90207, 3);
					_app->onCall(2079);
				}

				_app->objectPresentationPauseAnimation(kObject10007, 0);
				_app->objectPresentationPauseAnimation(kObject10007, 1);
				_app->objectPresentationPauseAnimation(kObject10008, 0);
				_app->objectPresentationPauseAnimation(kObject10008, 1);
				_app->objectPresentationPauseAnimation(kObject10009, 1);
				_app->objectPresentationPauseAnimation(kObject10010, 0);
				_app->objectPresentationPauseAnimation(kObject10011, 0);
				_app->objectPresentationPauseAnimation(kObject10011, 1);
				_app->objectPresentationPauseAnimation(kObject10018, 0);
				_app->objectPresentationPauseAnimation(kObject10018, 1);
				_app->objectPresentationPauseAnimation(kObject10020, 0);
				_app->objectPresentationPauseAnimation(kObject10020, 1);
				_app->objectPresentationPauseAnimation(kObject10007, 2);
				_app->objectPresentationPauseAnimation(kObject10009, 0);
				_app->objectPresentationPauseAnimation(kObject10009, 2);
				_app->objectPresentationPauseAnimation(kObject10011, 2);
				_app->objectPresentationPauseAnimation(kObject10018, 2);
				_app->timerStopAll();
				_app->onCall(10100);

				if (_app->varGetByte(90201) == 3) {
					_app->varSetByte(90201, 4);
					_app->onCall(202);
				}
			}
			break;
		}

		onVisual(201);
		onVisual(207);
		break;

	case 3:
		switch (id) {
		default:
			break;

		case kObject10101:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				_app->objectPresentationPauseAnimation(kObject10007, 0);
				_app->objectPresentationPauseAnimation(kObject10007, 1);
				_app->objectPresentationPauseAnimation(kObject10008, 0);
				_app->objectPresentationPauseAnimation(kObject10008, 1);
				_app->objectPresentationPauseAnimation(kObject10009, 1);
				_app->objectPresentationPauseAnimation(kObject10010, 0);
				_app->objectPresentationPauseAnimation(kObject10011, 0);
				_app->objectPresentationPauseAnimation(kObject10011, 1);
				_app->objectPresentationPauseAnimation(kObject10018, 0);
				_app->objectPresentationPauseAnimation(kObject10018, 1);
				_app->objectPresentationPauseAnimation(kObject10020, 0);
				_app->objectPresentationPauseAnimation(kObject10020, 1);
				_app->objectPresentationPauseAnimation(kObject10007, 2);
				_app->objectPresentationPauseAnimation(kObject10009, 0);
				_app->objectPresentationPauseAnimation(kObject10009, 2);
				_app->objectPresentationPauseAnimation(kObject10011, 2);
				_app->objectPresentationPauseAnimation(kObject10018, 2);
				_app->timerStopAll();
				_app->onCall(10100);
			}
			break;

		case kObject10102:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				_app->objectPresentationPauseAnimation(kObject10007, 0);
				_app->objectPresentationPauseAnimation(kObject10007, 1);
				_app->objectPresentationPauseAnimation(kObject10008, 0);
				_app->objectPresentationPauseAnimation(kObject10008, 1);
				_app->objectPresentationPauseAnimation(kObject10009, 1);
				_app->objectPresentationPauseAnimation(kObject10010, 0);
				_app->objectPresentationPauseAnimation(kObject10011, 0);
				_app->objectPresentationPauseAnimation(kObject10011, 1);
				_app->objectPresentationPauseAnimation(kObject10018, 0);
				_app->objectPresentationPauseAnimation(kObject10018, 1);
				_app->objectPresentationPauseAnimation(kObject10020, 0);
				_app->objectPresentationPauseAnimation(kObject10020, 1);
				_app->objectPresentationPauseAnimation(kObject10007, 2);
				_app->objectPresentationPauseAnimation(kObject10009, 0);
				_app->objectPresentationPauseAnimation(kObject10009, 2);
				_app->objectPresentationPauseAnimation(kObject10011, 2);
				_app->objectPresentationPauseAnimation(kObject10018, 2);
				_app->timerStopAll();
				_app->onCall(102003);

				if (!_app->varGetByte(90301)) {
					_app->varSetByte(90301, 1);
					_app->onCall(301);
				}
			}
			break;
		}
		break;

	case 4:
		switch (id) {
		default:
			break;

		case kObject10018:
			if (!_app->bagHasClickedObject()) {
				switch (target) {
				default:
					break;

				case 0:
					if (!_app->varGetByte(90401)) {
						_app->puzzleSetActive(kPuzzle10017);
						_app->soundPlay(4001);
						_app->objectSetAccessibilityOff(kObject10018);
					}
					break;

				case 4:
					if (!_app->varGetByte(90415)) {
						_app->puzzleSetActive(kPuzzle10017);
						_app->soundPlay(4103);
						_app->objectSetAccessibilityOff(kObject10018, 4, 4);
						_app->objectSetAccessibilityOn(kObjectRedFish, 0, 0);
						_app->varSetByte(10029, 1);
					}
					break;
				}
			} else {
				switch (target) {
				default:
					break;

				case 2:
					if (_app->bagGetClickedObject() == kObjectSylphium) {
						_app->soundPlay(10046);
						_app->bagRemove(kObjectSylphium);
						_app->objectSetAccessibilityOff(kObject10018, 2, 2);
						_app->puzzleSetActive(kPuzzle10017);
						_app->soundPlay(4094);
					}
					break;

				case 5:
					if (_app->bagGetClickedObject() == kObjectRedFish) {
						_app->bagRemove(kObjectRedFish);
						_app->objectSetAccessibilityOff(kObject10018, 5, 5);
						_app->playMovie("S01A02-2");
						_app->puzzleSetActive(kPuzzle10017);
						_app->soundPlay(4104);
					}
					break;
				}

				_app->cursorDelete();
			}
			break;

		case kObject10020:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				switch (target) {
				default:
					break;

				case 0:
					if (!_app->varGetByte(10015)) {
						_app->objectSetAccessibilityOff(kObject10020, 0, 0);
						_app->puzzleSetActive(kPuzzle10021);
						_app->soundPlay(4098);
						_app->varSetByte(10015, 1);
					}
					break;

				case 1:
					if (_app->varGetByte(10015) == 1) {
						_app->objectSetAccessibilityOff(kObject10020, 1, 1);
						_app->puzzleSetActive(kPuzzle10021);

						switch (_app->varGetByte(10030)) {
						default:
							break;

						case 1:
							_app->soundPlay(4099);
							break;

						case 2:
							_app->soundPlay(4100);
							break;

						case 3:
							_app->soundPlay(4101);
							break;
						}
					}
					break;
				}
			}
			break;

		case kObjectEggs:
			if (_app->bagHasClickedObject()) {
				if (_app->bagGetClickedObject() == kObjectEggs
				 && _app->varGetByte(10016) != 2
				 && _app->varGetByte(10016) != 3) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectEggs);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectEggs, 0, Common::Point(3, 220));
					_app->objectPresentationShow(kObjectEggs, 0);
					_app->varSetByte(10016, 1);
					_app->objectSetAccessibilityOff(kObjectEggs, 1, 1);
					_app->objectSetAccessibilityOn(kObjectEggs, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectTuna
				 && _app->varGetByte(10016) != 1
				 && _app->varGetByte(10016) != 3) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectTuna);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectTuna, 0, Common::Point(3, 220));
					_app->objectPresentationShow(kObjectTuna, 0);
					_app->varSetByte(10016, 2);
					_app->objectSetAccessibilityOff(kObjectEggs, 1, 1);
					_app->objectSetAccessibilityOn(kObjectEggs, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectMussels
				 && _app->varGetByte(10016) != 1
				 && _app->varGetByte(10016) != 2) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectMussels);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectMussels, 0, Common::Point(3, 220));
					_app->objectPresentationShow(kObjectMussels, 0);
					_app->varSetByte(10016, 3);
					_app->objectSetAccessibilityOff(kObjectEggs, 1, 1);
					_app->objectSetAccessibilityOn(kObjectEggs, 0, 0);
				}

				if (_app->varGetByte(10016) == 3
				 && _app->varGetByte(10017) == 1
				 && _app->varGetByte(10018) == 2) {
					_app->cursorDelete();

					handleEvents();

					_app->fadeOut(15, Color(0, 0, 0), 0);

					_app->soundPlay(4100);
					_app->objectPresentationHide(kObjectEggs, 0);
					_app->objectSetAccessibilityOff(kObjectEggs);
					_app->objectPresentationHide(kObjectTuna, 0);
					_app->objectSetAccessibilityOff(kObjectTuna);
					_app->objectPresentationHide(kObjectMussels, 0);
					_app->objectSetAccessibilityOff(kObjectMussels);
					_app->puzzleSetActive(kPuzzle10021);
					_app->soundPlay(4100);
				}

				_app->cursorDelete();
			} else {
				if (target)
					break;

				switch (_app->varGetByte(10016)) {
				default:
					break;

				case 1:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectEggs, 0);
					_app->bagAdd(kObjectEggs);
					_app->varSetByte(10016, 0);
					_app->objectSetAccessibilityOff(kObjectEggs, 0, 0);
					_app->objectSetAccessibilityOn(kObjectEggs, 1, 1);
					break;

				case 2:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectTuna, 0);
					_app->bagAdd(kObjectTuna);
					_app->varSetByte(10016, 0);
					_app->objectSetAccessibilityOff(kObjectEggs, 0, 0);
					_app->objectSetAccessibilityOn(kObjectEggs, 1, 1);
					break;

				case 3:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectMussels, 0);
					_app->bagAdd(kObjectMussels);
					_app->varSetByte(10016, 0);
					_app->objectSetAccessibilityOff(kObjectEggs, 0, 0);
					_app->objectSetAccessibilityOn(kObjectEggs, 1, 1);
					break;
				}
			}
			break;

		case kObjectTuna:
			if (_app->bagHasClickedObject()) {
				if (_app->bagGetClickedObject() == kObjectEggs
				 && _app->varGetByte(10017) != 2
				 && _app->varGetByte(10017) != 3) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectEggs);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectEggs, 0, Common::Point(207, 209));
					_app->objectPresentationShow(kObjectEggs, 0);
					_app->varSetByte(10017, 1);
					_app->objectSetAccessibilityOff(kObjectTuna, 1, 1);
					_app->objectSetAccessibilityOn(kObjectTuna, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectTuna
				 && _app->varGetByte(10017) != 1
				 && _app->varGetByte(10017) != 3) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectTuna);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectTuna, 0, Common::Point(207, 209));
					_app->objectPresentationShow(kObjectTuna, 0);
					_app->varSetByte(10017, 2);
					_app->objectSetAccessibilityOff(kObjectTuna, 1, 1);
					_app->objectSetAccessibilityOn(kObjectTuna, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectMussels
				 && _app->varGetByte(10017) != 1
				 && _app->varGetByte(10017) != 2) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectMussels);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectMussels, 0, Common::Point(207, 209));
					_app->objectPresentationShow(kObjectMussels, 0);
					_app->varSetByte(10017, 3);
					_app->objectSetAccessibilityOff(kObjectTuna, 1, 1);
					_app->objectSetAccessibilityOn(kObjectTuna, 0, 0);
				}

				if (_app->varGetByte(10016) == 3
				 && _app->varGetByte(10017) == 1
				 && _app->varGetByte(10018) == 2) {
					_app->cursorDelete();

					handleEvents();

					_app->fadeOut(15, Color(0, 0, 0), 0);
					_app->objectPresentationHide(kObjectEggs, 0);
					_app->objectSetAccessibilityOff(kObjectEggs);
					_app->objectPresentationHide(kObjectTuna, 0);
					_app->objectSetAccessibilityOff(kObjectTuna);
					_app->objectPresentationHide(kObjectMussels, 0);
					_app->objectSetAccessibilityOff(kObjectMussels);
					_app->puzzleSetActive(kPuzzle10021);
					_app->soundPlay(4100);
				}

				_app->cursorDelete();
			} else {
				if (target)
					break;

				switch (_app->varGetByte(10017)) {
				default:
					break;

				case 1:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectEggs, 0);
					_app->bagAdd(kObjectEggs);
					_app->varSetByte(10017, 0);
					_app->objectSetAccessibilityOff(kObjectTuna, 0, 0);
					_app->objectSetAccessibilityOn(kObjectTuna, 1, 1);
					break;

				case 2:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectTuna, 0);
					_app->bagAdd(kObjectTuna);
					_app->varSetByte(10017, 0);
					_app->objectSetAccessibilityOff(kObjectTuna, 0, 0);
					_app->objectSetAccessibilityOn(kObjectTuna, 1, 1);
					break;

				case 3:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectMussels, 0);
					_app->bagAdd(kObjectMussels);
					_app->varSetByte(10017, 0);
					_app->objectSetAccessibilityOff(kObjectTuna, 0, 0);
					_app->objectSetAccessibilityOn(kObjectTuna, 1, 1);
					break;
				}
			}
			break;

		case kObjectMussels:
			if (_app->bagHasClickedObject()) {
				if (_app->bagGetClickedObject() == kObjectEggs
				 && _app->varGetByte(10018) != 2
				 && _app->varGetByte(10018) != 3) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectEggs);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectEggs, 0, Common::Point(420, 198));
					_app->objectPresentationShow(kObjectEggs, 0);
					_app->varSetByte(10018, 1);
					_app->objectSetAccessibilityOff(kObjectMussels, 1, 1);
					_app->objectSetAccessibilityOn(kObjectMussels, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectTuna
				 && _app->varGetByte(10018) != 1
				 && _app->varGetByte(10018) != 3) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectTuna);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectTuna, 0, Common::Point(420, 198));
					_app->objectPresentationShow(kObjectTuna, 0);
					_app->varSetByte(10018, 2);
					_app->objectSetAccessibilityOff(kObjectMussels, 1, 1);
					_app->objectSetAccessibilityOn(kObjectMussels, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectMussels
				 && _app->varGetByte(10018) != 1
				 && _app->varGetByte(10018) != 2) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectMussels);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectMussels, 0, Common::Point(420, 198));
					_app->objectPresentationShow(kObjectMussels, 0);
					_app->varSetByte(10018, 3);
					_app->objectSetAccessibilityOff(kObjectMussels, 1, 1);
					_app->objectSetAccessibilityOn(kObjectMussels, 0, 0);
				}

				if (_app->varGetByte(10016) == 3
				 && _app->varGetByte(10017) == 1
				 && _app->varGetByte(10018) == 2) {
					_app->cursorDelete();

					handleEvents();

					_app->fadeOut(15, Color(0, 0, 0), 0);
					_app->objectPresentationHide(kObjectEggs, 0);
					_app->objectSetAccessibilityOff(kObjectEggs);
					_app->objectPresentationHide(kObjectTuna, 0);
					_app->objectSetAccessibilityOff(kObjectTuna);
					_app->objectPresentationHide(kObjectMussels, 0);
					_app->objectSetAccessibilityOff(kObjectMussels);
					_app->puzzleSetActive(kPuzzle10021);
					_app->soundPlay(4100);
				}

				_app->cursorDelete();
			} else {
				if (target)
					break;

				switch (_app->varGetByte(10018)) {
				default:
					break;

				case 1:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectEggs, 0);
					_app->bagAdd(kObjectEggs);
					_app->varSetByte(10018, 0);
					_app->objectSetAccessibilityOff(kObjectMussels, 0, 0);
					_app->objectSetAccessibilityOn(kObjectMussels, 1, 1);
					break;

				case 2:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectTuna, 0);
					_app->bagAdd(kObjectTuna);
					_app->varSetByte(10018, 0);
					_app->objectSetAccessibilityOff(kObjectMussels, 0, 0);
					_app->objectSetAccessibilityOn(kObjectMussels, 1, 1);
					break;

				case 3:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectMussels, 0);
					_app->bagAdd(kObjectMussels);
					_app->varSetByte(10018, 0);
					_app->objectSetAccessibilityOff(kObjectMussels, 0, 0);
					_app->objectSetAccessibilityOn(kObjectMussels, 1, 1);
					break;
				}
			}
			break;

		case kObjectDuck:
			if (_app->bagHasClickedObject()) {
				if (_app->bagGetClickedObject() == kObjectDuck && _app->varGetByte(10019) != 2) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectDuck);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectDuck, 0, Common::Point(3, 220));
					_app->objectPresentationShow(kObjectDuck, 0);
					_app->varSetByte(10019, 1);
					_app->objectSetAccessibilityOff(kObjectDuck, 1, 1);
					_app->objectSetAccessibilityOn(kObjectDuck, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectDugs && _app->varGetByte(10019) != 1) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectDugs);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectDugs, 0, Common::Point(3, 220));
					_app->objectPresentationShow(kObjectDugs, 0);
					_app->varSetByte(10019, 2);
					_app->objectSetAccessibilityOff(kObjectDuck, 1, 1);
					_app->objectSetAccessibilityOn(kObjectDuck, 0, 0);
				}

				if (_app->varGetByte(10019) == 2 && _app->varGetByte(10020) == 1) {
					_app->cursorDelete();

					handleEvents();

					_app->fadeOut(15, Color(0, 0, 0), 0);
					_app->objectPresentationHide(kObjectDuck, 0);
					_app->objectSetAccessibilityOff(kObjectDuck);
					_app->objectPresentationHide(kObjectDugs, 0);
					_app->objectSetAccessibilityOff(kObjectDugs);
					_app->puzzleSetActive(kPuzzle10021);
					_app->soundPlay(4101);
				}

				_app->cursorDelete();
			} else {
				if (target)
					break;

				switch (_app->varGetByte(10019)) {
				default:
					break;

				case 1:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectDuck, 0);
					_app->bagAdd(kObjectDuck);
					_app->varSetByte(10019, 0);
					_app->objectSetAccessibilityOff(kObjectDuck, 0, 0);
					_app->objectSetAccessibilityOn(kObjectDuck, 1, 1);
					break;

				case 2:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectDugs, 0);
					_app->bagAdd(kObjectDugs);
					_app->varSetByte(10019, 0);
					_app->objectSetAccessibilityOff(kObjectDuck, 0, 0);
					_app->objectSetAccessibilityOn(kObjectDuck, 1, 1);
					break;
				}
			}
			break;

		case kObjectDugs:
			if (_app->bagHasClickedObject()) {
				if (_app->bagGetClickedObject() == kObjectDuck && _app->varGetByte(10020) != 2) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectDuck);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectDuck, 0, Common::Point(207, 209));
					_app->objectPresentationShow(kObjectDuck, 0);
					_app->varSetByte(10020, 1);
					_app->objectSetAccessibilityOff(kObjectDugs, 1, 1);
					_app->objectSetAccessibilityOn(kObjectDugs, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectDugs && _app->varGetByte(10020) != 1) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectDugs);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectDugs, 0, Common::Point(207, 209));
					_app->objectPresentationShow(kObjectDugs, 0);
					_app->varSetByte(10020, 2);
					_app->objectSetAccessibilityOff(kObjectDugs, 1, 1);
					_app->objectSetAccessibilityOn(kObjectDugs, 0, 0);
				}

				if (_app->varGetByte(10019) == 2 && _app->varGetByte(10020) == 1) {
					_app->cursorDelete();

					handleEvents();

					_app->fadeOut(15, Color(0, 0, 0), 0);
					_app->objectPresentationHide(kObjectDuck, 0);
					_app->objectSetAccessibilityOff(kObjectDuck);
					_app->objectPresentationHide(kObjectDugs, 0);
					_app->objectSetAccessibilityOff(kObjectDugs);
					_app->puzzleSetActive(kPuzzle10021);
					_app->soundPlay(4101);
				}

				_app->cursorDelete();
			} else {
				if (target)
					break;

				switch (_app->varGetByte(10020)) {
				default:
					break;

				case 1:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectDuck, 0);
					_app->bagAdd(kObjectDuck);
					_app->varSetByte(10020, 0);
					_app->objectSetAccessibilityOff(kObjectDugs, 0, 0);
					_app->objectSetAccessibilityOn(kObjectDugs, 1, 1);
					break;

				case 2:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectDugs, 0);
					_app->bagAdd(kObjectDugs);
					_app->varSetByte(10020, 0);
					_app->objectSetAccessibilityOff(kObjectDugs, 0, 0);
					_app->objectSetAccessibilityOn(kObjectDugs, 1, 1);
					break;
				}
			}
			break;

		case kObjectDulcia:
			if (_app->bagHasClickedObject()) {
				if (_app->bagGetClickedObject() == kObjectDulcia
				 && _app->varGetByte(10021) != 2
				 && _app->varGetByte(10021) != 3) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectDulcia);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectDulcia, 0, Common::Point(3, 220));
					_app->objectPresentationShow(kObjectDulcia, 0);
					_app->varSetByte(10021, 1);
					_app->objectSetAccessibilityOff(kObjectDulcia, 1, 1);
					_app->objectSetAccessibilityOn(kObjectDulcia, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectDates2
				 && _app->varGetByte(10021) != 1
				 && _app->varGetByte(10021) != 3) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectDates2);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectDates2, 0, Common::Point(3, 220));
					_app->objectPresentationShow(kObjectDates2, 0);
					_app->varSetByte(10021, 2);
					_app->objectSetAccessibilityOff(kObjectDulcia, 1, 1);
					_app->objectSetAccessibilityOn(kObjectDulcia, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectOysters
				 && _app->varGetByte(10021) != 1
				 && _app->varGetByte(10021) != 2) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectOysters);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectOysters, 0, Common::Point(3, 220));
					_app->objectPresentationShow(kObjectOysters, 0);
					_app->varSetByte(10021, 3);
					_app->objectSetAccessibilityOff(kObjectDulcia, 1, 1);
					_app->objectSetAccessibilityOn(kObjectDulcia, 0, 0);
				}

				if (_app->varGetByte(10021) == 3
				 && _app->varGetByte(10022) == 1
				 && _app->varGetByte(10023) == 2) {
					_app->cursorDelete();

					handleEvents();

					_app->objectSetAccessibilityOff(kObjectDulcia);
					_app->objectSetAccessibilityOff(kObjectDates2);
					_app->objectSetAccessibilityOff(kObjectOysters);

					onVisual(414);
				}

				_app->cursorDelete();
			} else {
				if (target)
					break;

				switch (_app->varGetByte(10021)) {
				default:
					break;

				case 1:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectDulcia, 0);
					_app->bagAdd(kObjectDulcia);
					_app->varSetByte(10021, 0);
					_app->objectSetAccessibilityOff(kObjectDulcia, 0, 0);
					_app->objectSetAccessibilityOn(kObjectDulcia, 1, 1);
					break;

				case 2:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectDates2, 0);
					_app->bagAdd(kObjectDates2);
					_app->varSetByte(10021, 0);
					_app->objectSetAccessibilityOff(kObjectDulcia, 0, 0);
					_app->objectSetAccessibilityOn(kObjectDulcia, 1, 1);
					break;

				case 3:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectOysters, 0);
					_app->bagAdd(kObjectOysters);
					_app->varSetByte(10021, 0);
					_app->objectSetAccessibilityOff(kObjectDulcia, 0, 0);
					_app->objectSetAccessibilityOn(kObjectDulcia, 1, 1);
					break;
				}
				return;
			}
			break;

		case kObjectDates2:
			if (_app->bagHasClickedObject()) {
				if (_app->bagGetClickedObject() == kObjectDulcia
				 && _app->varGetByte(10022) != 2
				 && _app->varGetByte(10022) != 3) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectDulcia);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectDulcia, 0, Common::Point(207, 209));
					_app->objectPresentationShow(kObjectDulcia, 0);
					_app->varSetByte(10022, 1);
					_app->objectSetAccessibilityOff(kObjectDates2, 1, 1);
					_app->objectSetAccessibilityOn(kObjectDates2, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectDates2
				 && _app->varGetByte(10022) != 1
				 && _app->varGetByte(10022) != 3) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectDates2);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectDates2, 0, Common::Point(207, 209));
					_app->objectPresentationShow(kObjectDates2, 0);
					_app->varSetByte(10022, 2);
					_app->objectSetAccessibilityOff(kObjectDates2, 1, 1);
					_app->objectSetAccessibilityOn(kObjectDates2, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectOysters
				 && _app->varGetByte(10022) != 1
				 && _app->varGetByte(10022) != 2) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectOysters);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectOysters, 0, Common::Point(207, 209));
					_app->objectPresentationShow(kObjectOysters, 0);
					_app->varSetByte(10022, 3);
					_app->objectSetAccessibilityOff(kObjectDates2, 1, 1);
					_app->objectSetAccessibilityOn(kObjectDates2, 0, 0);
				}

				if (_app->varGetByte(10021) == 3
				 && _app->varGetByte(10022) == 1
				 && _app->varGetByte(10023) == 2) {
					_app->cursorDelete();

					handleEvents();

					_app->objectSetAccessibilityOff(kObjectDulcia);
					_app->objectSetAccessibilityOff(kObjectDates2);
					_app->objectSetAccessibilityOff(kObjectOysters);

					onVisual(414);
				}

				_app->cursorDelete();
			} else {
				if (target)
					return;

				switch (_app->varGetByte(10022)) {
				default:
					break;

				case 1:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectDulcia, 0);
					_app->bagAdd(kObjectDulcia);
					_app->varSetByte(10022, 0);
					_app->objectSetAccessibilityOff(kObjectDates2, 0, 0);
					_app->objectSetAccessibilityOn(kObjectDates2, 1, 1);
					break;

				case 2:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectDates2, 0);
					_app->bagAdd(kObjectDates2);
					_app->varSetByte(10022, 0);
					_app->objectSetAccessibilityOff(kObjectDates2, 0, 0);
					_app->objectSetAccessibilityOn(kObjectDates2, 1, 1);
					break;

				case 3:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectOysters, 0);
					_app->bagAdd(kObjectOysters);
					_app->varSetByte(10022, 0);
					_app->objectSetAccessibilityOff(kObjectDates2, 0, 0);
					_app->objectSetAccessibilityOn(kObjectDates2, 1, 1);
					break;
				}

				return;
			}
			break;

		case kObjectOysters:
			if (_app->bagHasClickedObject()) {
				if (_app->bagGetClickedObject() == kObjectDulcia
				 && _app->varGetByte(10023) != 2
				 && _app->varGetByte(10023) != 3) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectDulcia);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectDulcia, 0, Common::Point(420, 198));
					_app->objectPresentationShow(kObjectDulcia, 0);
					_app->varSetByte(10023, 1);
					_app->objectSetAccessibilityOff(kObjectOysters, 1, 1);
					_app->objectSetAccessibilityOn(kObjectOysters, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectDates2
				 && _app->varGetByte(10023) != 1
				 && _app->varGetByte(10023) != 3) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectDates2);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectDates2, 0, Common::Point(420, 198));
					_app->objectPresentationShow(kObjectDates2, 0);
					_app->varSetByte(10023, 2);
					_app->objectSetAccessibilityOff(kObjectOysters, 1, 1);
					_app->objectSetAccessibilityOn(kObjectOysters, 0, 0);
				}

				if (_app->bagGetClickedObject() == kObjectOysters
				 && _app->varGetByte(10023) != 1
				 && _app->varGetByte(10023) != 2) {
					_app->soundPlay(10048);
					_app->bagRemove(kObjectOysters);
					_app->objectPresentationSetImageCoordinatesOnPuzzle(kObjectOysters, 0, Common::Point(420, 198));
					_app->objectPresentationShow(kObjectOysters, 0);
					_app->varSetByte(10023, 3);
					_app->objectSetAccessibilityOff(kObjectOysters, 1, 1);
					_app->objectSetAccessibilityOn(kObjectOysters, 0, 0);
				}

				if (_app->varGetByte(10021) == 3
				 && _app->varGetByte(10022) == 1
				 && _app->varGetByte(10023) == 2) {
					_app->cursorDelete();

					handleEvents();

					_app->objectSetAccessibilityOff(kObjectDulcia);
					_app->objectSetAccessibilityOff(kObjectDates2);
					_app->objectSetAccessibilityOff(kObjectOysters);

					onVisual(414);
				}

				_app->cursorDelete();
			} else {
				if (target)
					break;

				switch (_app->varGetByte(10023)) {
				default:
					break;

				case 1:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectDulcia, 0);
					_app->bagAdd(kObjectDulcia);
					_app->varSetByte(10023, 0);
					_app->objectSetAccessibilityOff(kObjectOysters, 0, 0);
					_app->objectSetAccessibilityOn(kObjectOysters, 1, 1);
					break;

				case 2:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectDates2, 0);
					_app->bagAdd(kObjectDates2);
					_app->varSetByte(10023, 0);
					_app->objectSetAccessibilityOff(kObjectOysters, 0, 0);
					_app->objectSetAccessibilityOn(kObjectOysters, 1, 1);
					break;

				case 3:
					_app->soundPlay(10047);
					_app->objectPresentationHide(kObjectOysters, 0);
					_app->bagAdd(kObjectOysters);
					_app->varSetByte(10023, 0);
					_app->objectSetAccessibilityOff(kObjectOysters, 0, 0);
					_app->objectSetAccessibilityOn(kObjectOysters, 1, 1);
					break;
				}
			}
			break;

		case kObjectRedFish:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (!target) {
					_app->objectSetAccessibilityOff(kObjectRedFish, 0, 0);
					_app->objectSetAccessibilityOn(kObject10018, 5, 5);
					_app->bagAdd(kObjectRedFish);
				}
			}
			break;

		case kObject10101:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				if (_app->varGetByte(90401) == 2) {
					_app->varSetByte(90401, 3);
					_app->onCall(4019);
				}

				_app->objectPresentationPauseAnimation(kObject10007, 0);
				_app->objectPresentationPauseAnimation(kObject10007, 1);
				_app->objectPresentationPauseAnimation(kObject10008, 0);
				_app->objectPresentationPauseAnimation(kObject10008, 1);
				_app->objectPresentationPauseAnimation(kObject10009, 1);
				_app->objectPresentationPauseAnimation(kObject10010, 0);
				_app->objectPresentationPauseAnimation(kObject10011, 0);
				_app->objectPresentationPauseAnimation(kObject10011, 1);
				_app->objectPresentationPauseAnimation(kObject10018, 0);
				_app->objectPresentationPauseAnimation(kObject10018, 1);
				_app->objectPresentationPauseAnimation(kObject10020, 0);
				_app->objectPresentationPauseAnimation(kObject10020, 1);
				_app->objectPresentationPauseAnimation(kObject10007, 2);
				_app->objectPresentationPauseAnimation(kObject10009, 0);
				_app->objectPresentationPauseAnimation(kObject10009, 2);
				_app->objectPresentationPauseAnimation(kObject10011, 2);
				_app->objectPresentationPauseAnimation(kObject10018, 2);
				_app->timerStopAll();
				_app->onCall(10100);
			}
			break;

		case kObject10102:
			if (_app->bagHasClickedObject()) {
				_app->cursorDelete();
			} else {
				_app->objectPresentationPauseAnimation(kObject10007, 0);
				_app->objectPresentationPauseAnimation(kObject10007, 1);
				_app->objectPresentationPauseAnimation(kObject10008, 0);
				_app->objectPresentationPauseAnimation(kObject10008, 1);
				_app->objectPresentationPauseAnimation(kObject10009, 1);
				_app->objectPresentationPauseAnimation(kObject10010, 0);
				_app->objectPresentationPauseAnimation(kObject10011, 0);
				_app->objectPresentationPauseAnimation(kObject10011, 1);
				_app->objectPresentationPauseAnimation(kObject10018, 0);
				_app->objectPresentationPauseAnimation(kObject10018, 1);
				_app->objectPresentationPauseAnimation(kObject10020, 0);
				_app->objectPresentationPauseAnimation(kObject10020, 1);
				_app->objectPresentationPauseAnimation(kObject10007, 2);
				_app->objectPresentationPauseAnimation(kObject10009, 0);
				_app->objectPresentationPauseAnimation(kObject10009, 2);
				_app->objectPresentationPauseAnimation(kObject10011, 2);
				_app->objectPresentationPauseAnimation(kObject10018, 2);
				_app->timerStopAll();
				_app->onCall(102003);
			}
			break;
		}
		break;
	}
}

void Zone2Pompeii::onTimer(TimerId id) {
	switch (id) {
	default:
		break;

	case kTimer0:
		_app->timerStop(kTimer0);
		_app->objectPresentationUnpauseAnimation(kObject10007, 1);
		_app->objectPresentationUnpauseAnimation(kObject10008, 0);
		_app->objectPresentationUnpauseAnimation(kObject10008, 1);
		_app->objectPresentationUnpauseAnimation(kObject10009, 1);
		_app->objectPresentationUnpauseAnimation(kObject10010, 0);
		_app->objectPresentationUnpauseAnimation(kObject10011, 0);
		_app->objectPresentationUnpauseAnimation(kObject10011, 1);
		_app->objectPresentationUnpauseAnimation(kObject10018, 0);
		_app->objectPresentationUnpauseAnimation(kObject10018, 1);
		_app->objectPresentationUnpauseAnimation(kObject10020, 0);
		_app->objectPresentationUnpauseAnimation(kObject10020, 1);
		break;

	case kTimer1:
		_app->timerStop(kTimer1);
		_app->objectPresentationUnpauseAnimation(kObject10007, 2);
		_app->objectPresentationUnpauseAnimation(kObject10009, 0);
		_app->objectPresentationUnpauseAnimation(kObject10009, 2);
		_app->objectPresentationUnpauseAnimation(kObject10011, 2);
		_app->objectPresentationUnpauseAnimation(kObject10018, 2);
		break;

	case kTimer2:
		_app->timerStop(kTimer2);
		_app->objectPresentationUnpauseAnimation(kObject10007, 0);
		break;
	}
}

void Zone2Pompeii::onAnimationNextFrame(Id animationId, const Common::String &/*name*/, uint32 frame, uint32 frameCount) {
	switch (animationId) {
	default:
		break;

	case 10000:
		if (frame == frameCount) {
			_app->objectPresentationPauseAnimation(kObject10007, 0);
			_app->timerStart(kTimer2, 1000 * (rnd(5) + 2));
		}
		break;

	case 10001:
		if (frame == frameCount) {
			_app->objectPresentationPauseAnimation(kObject10007, 1);
			_app->objectPresentationPauseAnimation(kObject10008, 0);
			_app->objectPresentationPauseAnimation(kObject10008, 1);
			_app->objectPresentationPauseAnimation(kObject10009, 1);
			_app->objectPresentationPauseAnimation(kObject10010, 0);
			_app->objectPresentationPauseAnimation(kObject10011, 0);
			_app->objectPresentationPauseAnimation(kObject10011, 1);
			_app->objectPresentationPauseAnimation(kObject10018, 0);
			_app->objectPresentationPauseAnimation(kObject10018, 1);
			_app->objectPresentationPauseAnimation(kObject10020, 0);
			_app->objectPresentationPauseAnimation(kObject10020, 1);

			_app->timerStart(kTimer0, 1000 * (rnd(5) + 2));
		}
		break;

	case 10002:
		if (frame == frameCount) {
			_app->objectPresentationPauseAnimation(kObject10007, 2);
			_app->objectPresentationPauseAnimation(kObject10009, 0);
			_app->objectPresentationPauseAnimation(kObject10009, 2);
			_app->objectPresentationPauseAnimation(kObject10011, 2);
			_app->objectPresentationPauseAnimation(kObject10018, 2);

			_app->timerStart(kTimer1, 1000 * (rnd(5) + 2));
		}
		break;
	}
}

void Zone2Pompeii::onSound(Id id, SoundType /*type*/, uint32 /*a3*/, bool /*process*/) {
	switch (id) {
	default:
		break;

	case 1001:
		_app->rotationSetMovabilityOn(10155);
		break;

	case 1002:
		_app->objectSetAccessibilityOn(kObject10007, 1, 1);
		// Fallback to next case

	case 1003:
	case 1004:
	case 1005:
	case 1006:
	case 1007:
		_app->rotationSetAlp(10111, 202.0f);
		_app->rotationSetBet(10111, 15.0f);
		_app->rotationSetActive(10111);

		if (_app->varGetByte(10024) == 1) {
			_app->objectSetAccessibilityOff(kObject10007, 1, 1);

			if (_app->varGetByte(10025) == 1)
				onVisualList(0, 0, Common::Point(102, 0));
		}
		break;

	case 1008:
		_app->objectSetAccessibilityOn(kObject10008, 1, 1);
		// Fallback to next case

	case 1009:
	case 1010:
	case 1011:
	case 1012:
	case 1013:
	case 1014:
		_app->rotationSetAlp(10111, 157.0f);
		_app->rotationSetBet(10111, 15.0f);
		_app->rotationSetActive(10111);

		if (_app->varGetByte(10025) == 1) {
			_app->objectSetAccessibilityOff(kObject10008, 1, 1);

			if (_app->varGetByte(10024) == 1)
				onVisualList(0, 0, Common::Point(102, 0));
		}
		break;

	case 1015:
		_app->objectSetAccessibilityOn(kObject10009, 1, 1);
		// Fallback to next case

	case 1016:
	case 1017:
	case 1018:
	case 1019:
	case 1020:
	case 1021:
	case 1022:
	case 1023:
		_app->rotationSetAlp(10111, 127.0f);
		_app->rotationSetBet(10111, 15.0f);
		_app->rotationSetActive(10111);

		if (_app->varGetByte(10026) == 1) {
			_app->objectSetAccessibilityOff(kObject10009, 1, 1);
			_app->soundPlay(1024);
			_app->bagRemove(kObjectContract);
			onVisualList(0, 0, Common::Point(103, 0));
		}
		break;

	case 2027:
		_app->rotationSetActive(10191);
		break;

	case 3001:
		_app->objectSetAccessibilityOn(kObject10102, 0, 0);
		break;

	case 4001:
		_app->objectSetAccessibilityOn(kObject10018, 1, 1);
		// Fallback to next case

	case 4002:
	case 4003:
	case 4004:
	case 4005:
	case 4006:
	case 4007:
		_app->rotationSetAlp(10122, 60.0f);
		_app->rotationSetBet(10122, 15.0f);
		_app->rotationSetActive(10122);

		if (_app->varGetByte(10027) == 1) {
			_app->objectSetAccessibilityOff(kObject10018, 1, 1);
			_app->objectSetAccessibilityOn(kObject10101);
		}
		break;

	case 4094:
		_app->objectSetAccessibilityOn(10018, 3, 3);
		// Fallback to next case

	case 4095:
	case 4096:
	case 4097:
		_app->rotationSetAlp(10122, 60.0f);
		_app->rotationSetBet(10122, 15.0f);
		_app->rotationSetActive(10122);

		if (_app->varGetByte(10028) == 1) {
			_app->objectSetAccessibilityOff(kObject10018, 3, 3);
			onVisualList(0, 0, Common::Point(413, 0));
		}
		break;

	case 4098:
		_app->soundPlay(4099);
		break;

	case 4099:
		_app->objectSetAccessibilityOn(kObject10020, 1, 1);
		_app->puzzleSetActive(kPuzzle10003);

		if (_app->varGetByte(10030) == 0) {
			_app->varSetByte(10030, 1);
			_app->objectPresentationShow(kObjectEggs, 0);
			_app->objectSetAccessibilityOn(kObjectEggs, 0, 0);
			_app->objectPresentationShow(kObjectTuna, 0);
			_app->objectSetAccessibilityOn(kObjectTuna, 0, 0);
			_app->objectPresentationShow(kObjectMussels, 0);
			_app->objectSetAccessibilityOn(kObjectMussels, 0, 0);
		}
		break;

	case 4100:
		_app->objectSetAccessibilityOn(kObject10020, 1, 1);
		_app->puzzleSetActive(kPuzzle10003);

		if (_app->varGetByte(10030) == 1) {
			_app->varSetByte(10030, 2);
			_app->objectPresentationShow(kObjectDuck, 0);
			_app->objectSetAccessibilityOn(kObjectDuck, 0, 0);
			_app->objectPresentationShow(kObjectDugs, 0);
			_app->objectSetAccessibilityOn(kObjectDugs, 0, 0);
		}
		break;

	case 4101:
		_app->objectSetAccessibilityOn(kObject10020, 1, 1);
		_app->puzzleSetActive(kPuzzle10003);

		if (_app->varGetByte(10030) == 2) {
			_app->varSetByte(10030, 3);
			_app->objectPresentationShow(kObjectDulcia, 0);
			_app->objectSetAccessibilityOn(kObjectDulcia, 0, 0);
			_app->objectPresentationShow(kObjectDates2, 0);
			_app->objectSetAccessibilityOn(kObjectDates2, 0, 0);
			_app->objectPresentationShow(kObjectOysters, 0);
			_app->objectSetAccessibilityOn(kObjectOysters, 0, 0);
		}
		break;

	case 4103:
		_app->rotationSetActive(10122);
		break;

	case 4104:
		_app->objectPresentationHide(kObjectRedFish, 0);
		_app->objectSetAccessibilityOff(kObjectRedFish, 0, 0);

		onVisualList(0, 0, Common::Point(415, 0));
		break;
	}
}

void Zone2Pompeii::onUpdateBag(const Common::Point &/*point*/) {
	if (_hideBox) {
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;
	}
}

void Zone2Pompeii::onUpdateBefore(Id movabilityFrom, Id movabilityTo, uint32 /*movabilityIndex*/, Id /*target*/, const Common::Point &/*point*/) {
	if (_hideBox || _app->bagHasClickedObject())
		return;

	Common::Point mouse = g_system->getEventManager()->getMousePos() - Common::Point(20, 16);

	switch (movabilityFrom) {
	default:
		break;

	case 10007:
		if (movabilityTo == 1) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 1002, mouse);
			_hideBox = true;
		}
		break;

	case 10008:
		if (movabilityTo == 1) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 1008, mouse);
			_hideBox = true;
		}
		break;

	case 10009:
		if (movabilityTo == 1) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 1015, mouse);
			_hideBox = true;
		}
		break;

	case 10018:
		if (movabilityTo == 1 || movabilityTo == 3) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, (movabilityTo == 1) ? 4001 : 4094, mouse);
			_hideBox = true;
		}
		break;
	}
}

void Zone2Pompeii::onUpdateAfter(Id /*movabilityFrom*/, Id /*movabilityTo*/, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType /*movabilityType*/, const Common::Point &/*point*/) {
	if (_hideBox) {
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;
	}
}

void Zone2Pompeii::onBeforeRide(Id movabilityFrom, Id /*movabilityTo*/, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType movabilityType) {
	if (movabilityType == kMovabilityRotationToRotation && movabilityFrom == 10111) {
		if (_app->varGetByte(90103) == 2) {
			_app->varSetByte(90103, 3);
			_app->onCall(1039);
		}
	}
}

void Zone2Pompeii::onAfterRide(Id movabilityFrom, Id movabilityTo, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType movabilityType) {
	onVisualList(0, 0, Common::Point(201, 0));
	onVisualList(0, 0, Common::Point(207, 0));

	if (movabilityType == kMovabilityRotationToRotation)
		return;

	if (_app->varGetByte(90001) == 3
	 && _app->varGetByte(90003) == 0
	 && movabilityFrom == 10171
	 && (movabilityTo == 10172 || movabilityTo == 10173)) {
		_app->soundPlay(3001);
		_app->varSetByte(90003, 1);
	}

	if (_app->varGetByte(90415) >= 2
	 && _app->varGetByte(90416) == 0
	 && movabilityTo == 10122) {
		_app->soundPlay(4105);
		_app->varSetByte(90416, 1);
	}

	if ((movabilityFrom == 10128 && movabilityTo   == 10114)
	 || (movabilityFrom == 10114 && movabilityTo   == 10128))
		_app->soundPlay(900002);
}

void Zone2Pompeii::onVisual(int x) {
	switch (x) {
	default:
		break;

	case 101:
		if (_app->varGetByte(90101) < 2
		 && _app->varGetByte(10002) == 1
		 && _app->varGetByte(10003) == 1
		 && _app->varGetByte(10004) == 1) {
			_app->varSetByte(90101, 2);
			_app->onCall(102);
		}
		break;

	case 102:
		if (_app->varGetByte(90102) < 2
		 && _app->varGetByte(10024) == 1
		 && _app->varGetByte(10025) == 1) {
			_app->objectSetAccessibilityOff(kObject10000);
			_app->varSetByte(90102, 2);
			_app->onCall(103);
		}
		break;
	case 103:
		if (_app->varGetByte(90103) < 2
		 && _app->varGetByte(10026) == 1) {
			_app->varSetByte(90103, 2);
			_app->objectSetAccessibilityOn(kObject10101);
		}
		break;

	case 201:
		if (_app->varGetByte(90201) < 2
		 && _app->varGetByte(10009) == 1)
			_app->varSetByte(90201, 2);
		break;

	case 207:
		if (_app->varGetByte(90207) < 2
		 && _app->varGetByte(10010) == 1
		 && _app->varGetByte(10011) == 1
		 && _app->varGetByte(10012) == 1
		 && _app->varGetByte(10013) == 1)
			_app->varSetByte(90207, 2);
		break;

	case 401:
		if (_app->varGetByte(90401) < 2) {
			_app->varSetByte(90401, 2);
			_app->onCall(402);
			_app->onCall(403);
			_app->onCall(404);
		}
		break;

	case 413:
		if (_app->varGetByte(90413) < 2) {
			_app->varSetByte(90413, 2);
			_app->onCall(4139);
			_app->onCall(414);
		}
		break;

	case 414:
		if (_app->varGetByte(90414) < 2) {
			_app->varSetByte(90414, 2);

			checkEvents();

			_app->fadeOut(15, Color(0, 0, 0), 0);

			_app->rotationSetActive(10122);

			_app->varSetByte(10030, 4);
			_app->objectSetAccessibilityOff(kObject10020);
			_app->objectPresentationHide(kObjectDulcia, 0);
			_app->objectPresentationHide(kObjectDates2, 0);
			_app->objectPresentationHide(kObjectOysters, 0);
			_app->onCall(4149);
			_app->onCall(415);
		}
		break;

	case 415:
		if (_app->varGetByte(90415) < 2) {
			_app->varSetByte(90415, 2);
			_app->fadeOut(15, Color(0, 0, 0), 0);
			_app->onCall(4159);
			_app->onCall(416);
		}
		break;

	case 1002:
		_app->varSetByte(10024, 1);
		break;

	case 1003:
	case 1004:
	case 1005:
	case 1006:
	case 1007:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle10007);
		break;

	case 1008:
		_app->varSetByte(10025, 1);
		break;

	case 1009:
	case 1010:
	case 1011:
	case 1012:
	case 1013:
	case 1014:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle10010);
		break;

	case 1015:
		_app->varSetByte(10026, 1);
		break;

	case 1016:
	case 1017:
	case 1018:
	case 1019:
	case 1020:
	case 1021:
	case 1022:
	case 1023:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle10013);
		break;

	case 4001:
		_app->varSetByte(10027, 1);
		if (_app->varGetByte(90401) < 2) {
			_app->varSetByte(90401, 2);
			_app->onCall(402);
			_app->onCall(403);
			_app->onCall(404);
		}
		break;

	case 4094:
		_app->varSetByte(10028, 1);
		break;

	case 4002:
	case 4003:
	case 4004:
	case 4005:
	case 4006:
	case 4007:
	case 4095:
	case 4096:
	case 4097:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle10017);
		break;
	}
}

} // End of namespace Ring
