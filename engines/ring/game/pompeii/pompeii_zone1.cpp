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

#include "ring/game/pompeii/pompeii_zone1.h"

#include "ring/game/pompeii/pompeii_application.h"
#include "ring/game/pompeii/pompeii_shared.h"

#include "ring/debug.h"
#include "ring/helpers.h"
#include "ring/ring.h"

using namespace PompeiiGame;

namespace Ring {

Zone1Pompeii::Zone1Pompeii(ApplicationPompeii *application) : _app(application) {
	_hideBox = false;
}

Zone1Pompeii::~Zone1Pompeii() {
	// Zero-out passed pointers
	_app = nullptr;
}

void Zone1Pompeii::onInit() {
	_app->setCurrentZone(kZone1);

	_app->puzzleAdd(kPuzzle1000);
	_app->puzzleAddBackgroundImage(kPuzzle1000, "s00a01n02p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1001);
	_app->puzzleAddBackgroundImage(kPuzzle1001, "s00a01n02p02.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1002);
	_app->puzzleAddBackgroundImage(kPuzzle1002, "s00a01n02p05.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1004);
	_app->puzzleAddBackgroundImage(kPuzzle1004, "s00a02n02p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1007);
	_app->puzzleAddBackgroundImage(kPuzzle1007, "s00a02n02p04.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1010);
	_app->puzzleAddBackgroundImage(kPuzzle1010, "s00a02n01p03.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1011);
	_app->puzzleAddBackgroundImage(kPuzzle1011, "s00a02n03p02.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1013);
	_app->puzzleAddBackgroundImage(kPuzzle1013, "s00a02n03p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1015);
	_app->puzzleAddBackgroundImage(kPuzzle1015, "s00a01n01p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1016);
	_app->puzzleAddBackgroundImage(kPuzzle1016, "s00a03n01p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1017);
	_app->puzzleAddBackgroundImage(kPuzzle1017, "s00a03n02p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1020);
	_app->puzzleAddBackgroundImage(kPuzzle1020, "s00a01n08p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1023);
	_app->puzzleAddBackgroundImage(kPuzzle1023, "s00a01n10p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1026);
	_app->puzzleAddBackgroundImage(kPuzzle1026, "s00a02n04p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1027);
	_app->puzzleAddBackgroundImage(kPuzzle1027, "s00a01n03p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1030);
	_app->puzzleAddBackgroundImage(kPuzzle1030, "s00a01n03p04.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1033);
	_app->puzzleAddBackgroundImage(kPuzzle1033, "s00a01n03p07.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1035);
	_app->puzzleAddBackgroundImage(kPuzzle1035, "s00a01n03p12.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1036);
	_app->puzzleAddBackgroundImage(kPuzzle1036, "s00a01n03p13.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1038);
	_app->puzzleAddBackgroundImage(kPuzzle1038, "s00a01n04p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1037);
	_app->puzzleAddBackgroundImage(kPuzzle1037, "s00a01n04p02.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1039);
	_app->puzzleAddBackgroundImage(kPuzzle1039, "s00a01n05p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1064);
	_app->puzzleAddBackgroundImage(kPuzzle1064, "s00a01n14p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1040);
	_app->puzzleAddBackgroundImage(kPuzzle1040, "s00a01n03p10.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1044);
	_app->puzzleAddBackgroundImage(kPuzzle1044, "s00a01n03p11.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1045);
	_app->puzzleAddBackgroundImage(kPuzzle1045, "s00a04n03p05.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1046);
	_app->puzzleAddBackgroundImage(kPuzzle1046, "s00a04n06p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1048);
	_app->puzzleAddBackgroundImage(kPuzzle1048, "s00a04n13p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1049);
	_app->puzzleAddBackgroundImage(kPuzzle1049, "s00a04n03p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1051);
	_app->puzzleAddBackgroundImage(kPuzzle1051, "s00a04n03p03.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1053);
	_app->puzzleAddBackgroundImage(kPuzzle1053, "s00a05n03p01.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1056);
	_app->puzzleAddBackgroundImage(kPuzzle1056, "s00a05n02p02.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1060);
	_app->puzzleAddBackgroundImage(kPuzzle1060, "s00a01n02p06.bmp", Common::Point(0, 16), true);
	_app->puzzleAdd(kPuzzle1063);
	_app->puzzleAddBackgroundImage(kPuzzle1063, "s00a01n11p01.bmp", Common::Point(0, 16), true);
	_app->rotationAdd(10011, "S00A0-69", 0, 0);
	_app->rotationAdd(10012, "S00A0-65", 0, 7);
	_app->rotationAdd(10013, "S00A0-61", 0, 14);
	_app->rotationAdd(10014, "S00A0-58", 0, 2);
	_app->rotationAdd(10015, "S00A0-55", 0, 2);
	_app->rotationAdd(10016, "S00A0-52", 0, 0);
	_app->rotationAdd(10017, "S00A0-49", 0, 0);
	_app->rotationAdd(10018, "S00A0-77", 0, 2);
	_app->rotationAdd(10019, "S00A0-73", 0, 0);
	_app->rotationAdd(100110, "S00A0-43", 0, 3);
	_app->rotationAdd(100111, "S00A0-41", 0, 1);
	_app->rotationAdd(100112, "S00A0-39", 0, 1);
	_app->rotationAdd(100113, "S00A0-37", 0, 1);
	_app->rotationAdd(100114, "S00A0-35", 0, 2);
	_app->rotationSetCompressionBufferLength(100114, 1100000);
	_app->rotationAdd(10021, "S00A0-68", 0, 4);
	_app->rotationSetCompressionBufferLength(10021, 1200000);
	_app->rotationAdd(10022, "S00A0-64", 0, 4);
	_app->rotationAdd(10023, "S00A0-60", 0, 3);
	_app->rotationAdd(10024, "S00A0-57", 0, 2);
	_app->rotationAdd(10025, "S00A0-54", 0, 0);
	_app->rotationAdd(10026, "S00A0-51", 0, 0);
	_app->rotationAdd(10027, "S00A0-48", 0, 0);
	_app->rotationAdd(10028, "S00A0-76", 0, 0);
	_app->rotationAdd(10029, "S00A0-72", 0, 0);
	_app->rotationAdd(100210, "S00A02-8", 0, 0);
	_app->rotationAdd(100211, "S00A02-7", 0, 0);
	_app->rotationAdd(100212, "S00A02-6", 0, 0);
	_app->rotationAdd(100213, "S00A02-5", 0, 0);
	_app->rotationAdd(100214, "S00A02-4", 0, 0);
	_app->rotationAdd(100215, "S00A02-3", 0, 0);
	_app->rotationAdd(100216, "S00A02-2", 0, 0);
	_app->rotationAdd(100217, "S00A02-1", 0, 0);
	_app->rotationAdd(100218, "S00A0-46", 0, 0);
	_app->rotationAdd(100219, "S00A02-9", 0, 0);
	_app->rotationAdd(100220, "S00A0-82", 0, 0);
	_app->rotationAdd(100221, "S00A0-80", 0, 0);
	_app->rotationAdd(10031, "S00A03-4", 0, 4);
	_app->rotationAdd(10032, "S00A03-3", 0, 3);
	_app->rotationAdd(10041, "S00A0-66", 0, 2);
	_app->rotationAdd(10042, "S00A0-62", 0, 0);
	_app->rotationAdd(10043, "S00A04-9", 0, 6);
	_app->rotationAdd(10044, "S00A04-8", 0, 0);
	_app->rotationAdd(10045, "S00A04-7", 0, 0);
	_app->rotationAdd(10046, "S00A04-6", 0, 4);
	_app->rotationAdd(10047, "S00A04-5", 0, 0);
	_app->rotationAdd(10048, "S00A0-74", 0, 0);
	_app->rotationAdd(100410, "S00A04-4", 0, 0);
	_app->rotationAdd(100411, "S00A04-3", 0, 0);
	_app->rotationAdd(100412, "S00A04-2", 0, 0);
	_app->rotationAdd(100413, "S00A04-1", 0, 2);
	_app->rotationAdd(10051, "S00A0-67", 0, 0);
	_app->rotationAdd(10052, "S00A0-63", 0, 6);
	_app->rotationSetCompressionBufferLength(10052, 1400000);
	_app->rotationAdd(10053, "S00A0-59", 0, 6);
	_app->rotationAdd(10054, "S00A0-56", 0, 0);
	_app->rotationAdd(10055, "S00A0-53", 0, 0);
	_app->rotationAdd(10056, "S00A0-50", 0, 0);
	_app->rotationAdd(10057, "S00A0-47", 0, 0);
	_app->rotationAdd(10058, "S00A0-75", 0, 0);
	_app->rotationAdd(10059, "S00A0-71", 0, 0);
	_app->rotationAdd(100510, "S00A0-42", 0, 0);
	_app->rotationAdd(100511, "S00A0-40", 0, 0);
	_app->rotationAdd(100512, "S00A0-38", 0, 0);
	_app->rotationAdd(100513, "S00A0-36", 0, 0);
	_app->rotationAdd(100514, "S00A0-34", 0, 0);
	_app->rotationAdd(100515, "S00A0-33", 0, 0);
	_app->rotationAdd(100516, "S00A0-32", 0, 1);
	_app->rotationAdd(100517, "S00A0-31", 0, 1);
	_app->rotationAdd(100518, "S00A0-45", 0, 1);
	_app->rotationAdd(100519, "S00A0-44", 0, 1);
	_app->rotationAdd(100520, "S00A0-81", 0, 0);
	_app->rotationAdd(100521, "S00A0-79", 0, 0);
	_app->rotationAdd(100522, "S00A0-78", 0, 0);
	_app->rotationAdd(1001102, "S00A0-23", 0, 1);
	_app->rotationAdd(1001202, "S00A0-21", 0, 4);
	_app->rotationAdd(1001402, "S00A0-18", 0, 1);
	_app->rotationAdd(1001502, "S00A0-16", 0, 1);
	_app->rotationAdd(1001602, "S00A0-14", 0, 1);
	_app->rotationAdd(1001702, "S00A0-12", 0, 1);
	_app->rotationAdd(1001802, "S00A0-27", 0, 0);
	_app->rotationAdd(1001902, "S00A0-25", 0, 1);
	_app->rotationAdd(10011002, "S00A01-9", 0, 1);
	_app->rotationAdd(10011102, "S00A01-8", 0, 0);
	_app->rotationAdd(10011202, "S00A01-7", 0, 0);
	_app->rotationAdd(10011302, "S00A01-6", 0, 0);
	_app->rotationAdd(10011402, "S00A01-5", 0, 0);
	_app->rotationAdd(100312, "S00A03-2", 0, 0);
	_app->rotationAdd(100322, "S00A03-1", 0, 0);
	_app->rotationAdd(1005102, "S00A0-22", 0, 0);
	_app->rotationAdd(1005202, "S00A0-20", 0, 0);
	_app->rotationAdd(1005302, "S00A0-19", 0, 0);
	_app->rotationAdd(1005402, "S00A0-17", 0, 4);
	_app->rotationAdd(1005502, "S00A0-15", 0, 0);
	_app->rotationAdd(1005602, "S00A0-13", 0, 0);
	_app->rotationAdd(1005702, "S00A0-11", 0, 0);
	_app->rotationAdd(1005802, "S00A0-26", 0, 0);
	_app->rotationAdd(1005902, "S00A0-24", 0, 0);
	_app->rotationAdd(10051002, "S00A05-8", 0, 0);
	_app->rotationAdd(10051102, "S00A05-7", 0, 0);
	_app->rotationAdd(10051202, "S00A05-6", 0, 0);
	_app->rotationAdd(10051302, "S00A05-5", 0, 0);
	_app->rotationAdd(10051402, "S00A05-4", 0, 0);
	_app->rotationAdd(10051502, "S00A05-3", 0, 0);
	_app->rotationAdd(10051602, "S00A05-2", 0, 0);
	_app->rotationAdd(10051702, "S00A05-1", 0, 0);
	_app->rotationAdd(10051802, "S00A0-10", 0, 0);
	_app->rotationAdd(10051902, "S00A05-9", 0, 0);
	_app->rotationAdd(10052002, "S00A0-30", 0, 0);
	_app->rotationAdd(10052102, "S00A0-29", 0, 0);
	_app->rotationAdd(10052202, "S00A0-28", 0, 0);
	_app->rotationAdd(1001103, "S00A01-4", 0, 2);
	_app->rotationSetCompressionBufferLength(1001103, 1400000);
	_app->rotationAdd(10011103, "S00A01-3", 0, 0);
	_app->rotationAdd(10011203, "S00A01-2", 0, 2);
	_app->rotationAdd(10011303, "S00A01-1", 0, 2);
	_app->rotationAdd(1001104, "S00A01N01R04", 0, 0);
	_app->rotationAdd(1002502, "S00A02N05R02", 0, 0);
	_app->rotationAddMovabilityToRotation(10011, 10012, "", Common::Rect(2858, -266, 3179, 96), true, kCursorMove, 99);
	_app->rotationSetMovabilityToRotation(10011, 0, 270.0f, 0.0f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011, 100113, "", Common::Rect(1156, -68, 1413, 57), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(10011, 1, 70.0f, 0.0f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10012, 10011, "", Common::Rect(1211, -80, 1411, 70), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10012, 0, 42.0f, 0.0f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10012, 10013, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(10012, 1, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10012, 100114, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 2);
	_app->rotationSetMovabilityToRotation(10012, 2, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10013, 10012, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10013, 0, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10013, 10027, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(10013, 1, 180.0f, 0.0f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10014, 10015, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(10014, 0, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10014, 100114, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10014, 1, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10015, 10014, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10015, 0, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10015, 10016, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(10015, 1, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10016, 10015, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10016, 0, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10016, 10017, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(10016, 1, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10017, 10016, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10017, 0, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10017, 10018, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(10017, 1, 0.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10017, 10019, "", Common::Rect(1190, -92, 1501, 108), true, kCursorMove, 2);
	_app->rotationSetMovabilityToRotation(10017, 2, 42.0f, 0.0f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10018, 10017, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10018, 0, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10019, 10017, "", Common::Rect(2858, -266, 3179, 96), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10019, 0, 270.0f, 0.0f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10019, 100110, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(10019, 1, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100110, 10019, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100110, 0, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100110, 10041, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(100110, 1, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100111, 100112, "", Common::Rect(2944, -220, 3283, 115), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100111, 0, 258.0f, 0.0f, 87.0f, 0.0f, 1, 258.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100112, 100111, "", Common::Rect(1119, -61, 1432, 119), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100112, 0, 70.0f, 0.0f, 87.0f, 0.0f, 1, 70.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100112, 100113, "", Common::Rect(2944, -220, 3283, 115), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100112, 1, 258.0f, 0.0f, 87.0f, 0.0f, 1, 258.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100113, 10011, "", Common::Rect(2944, -220, 3283, 115), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100113, 0, 258.0f, 0.0f, 87.0f, 0.0f, 1, 258.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100113, 100112, "", Common::Rect(1142, -84, 1353, 87), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100113, 1, 80.0f, 0.0f, 87.0f, 0.0f, 1, 80.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100113, 10031, "", Common::Rect(2128, -54, 2330, 31), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100113, 2, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100114, 10012, "", Common::Rect(2130, -71, 2378, 68), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100114, 0, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100114, 10014, "", Common::Rect(342, -66, 558, 50), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100114, 1, 0.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001102, 1001202, "", Common::Rect(2858, -266, 3179, 96), true, kCursorMove, 99);
	_app->rotationSetMovabilityToRotation(1001102, 0, 270.0f, 0.0f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001102, 10011302, "", Common::Rect(1156, -68, 1413, 57), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(1001102, 1, 70.0f, 0.0f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001202, 1001102, "", Common::Rect(1211, -80, 1411, 70), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1001202, 0, 42.0f, 0.0f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001202, 10011402, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 2);
	_app->rotationSetMovabilityToRotation(1001202, 1, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001402, 1001502, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(1001402, 0, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001402, 10011402, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1001402, 1, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001502, 1001402, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1001502, 0, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001502, 1001602, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(1001502, 1, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001602, 1001502, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1001602, 0, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001602, 1001702, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(1001602, 1, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001702, 1001602, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1001702, 0, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001702, 1001802, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(1001702, 1, 0.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001702, 1001902, "", Common::Rect(1190, -92, 1501, 108), true, kCursorMove, 2);
	_app->rotationSetMovabilityToRotation(1001702, 2, 42.0f, 0.0f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001802, 1001702, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1001802, 0, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001902, 1001702, "", Common::Rect(2858, -266, 3179, 96), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1001902, 0, 270.0f, 0.0f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001902, 10011002, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(1001902, 1, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011002, 1001902, "", Common::Rect(2111, -131, 2300, 120), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011002, 0, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011102, 10011202, "", Common::Rect(2944, -220, 3283, 115), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011102, 0, 258.0f, 0.0f, 87.0f, 0.0f, 1, 258.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011202, 10011102, "", Common::Rect(1119, -61, 1432, 119), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011202, 0, 70.0f, 0.0f, 87.0f, 0.0f, 1, 70.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011202, 10011302, "", Common::Rect(2944, -220, 3283, 115), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011202, 1, 258.0f, 0.0f, 87.0f, 0.0f, 1, 258.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011302, 1001102, "", Common::Rect(2944, -220, 3283, 115), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011302, 0, 258.0f, 0.0f, 87.0f, 0.0f, 1, 258.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011302, 10011202, "", Common::Rect(1142, -84, 1353, 87), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011302, 1, 80.0f, 0.0f, 87.0f, 0.0f, 1, 80.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011302, 100312, "", Common::Rect(2128, -54, 2330, 31), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011302, 2, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011402, 1001202, "", Common::Rect(2130, -71, 2378, 68), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011402, 0, 180.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011402, 1001402, "", Common::Rect(342, -66, 558, 50), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011402, 1, 0.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001103, 10011303, "", Common::Rect(1156, -68, 1413, 57), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(1001103, 0, 70.0f, 0.0f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011103, 10011203, "", Common::Rect(2944, -220, 3283, 115), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011103, 0, 258.0f, 0.0f, 87.0f, 0.0f, 1, 258.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011203, 10011103, "", Common::Rect(1119, -61, 1432, 119), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011203, 0, 70.0f, 0.0f, 87.0f, 0.0f, 1, 70.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011203, 10011303, "", Common::Rect(2944, -220, 3283, 115), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011203, 1, 258.0f, 0.0f, 87.0f, 0.0f, 1, 258.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011303, 1001103, "", Common::Rect(2944, -220, 3283, 115), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011303, 0, 258.0f, 0.0f, 87.0f, 0.0f, 1, 258.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10011303, 10011203, "", Common::Rect(1142, -84, 1353, 87), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10011303, 1, 80.0f, 0.0f, 87.0f, 0.0f, 1, 80.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1001104, 10012, "", Common::Rect(2858, -266, 3179, 96), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1001104, 0, 270.0f, 0.0f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10021, 10023, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10021, 0, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10021, 10027, "", Common::Rect(1320, -250, 1470, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10021, 1, 94.0f, 0.30000001f, 87.0f, 0.0f, 1, 94.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10021, 10028, "", Common::Rect(2120, -250, 2270, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10021, 2, 174.0f, 0.30000001f, 87.0f, 0.0f, 1, 174.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10021, 100218, "", Common::Rect(3350, -250, 3500, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10021, 3, 297.0f, 0.30000001f, 87.0f, 0.0f, 1, 297.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10022, 10028, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10022, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10022, 10029, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10022, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10022, 100218, "", Common::Rect(480, -250, 630, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10022, 2, 10.0f, 0.30000001f, 87.0f, 0.0f, 1, 10.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10023, 10021, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10023, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10023, 100219, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10023, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10023, 100221, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10023, 2, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10024, 100211, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10024, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10024, 100212, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10024, 1, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10024, 100214, "", Common::Rect(1900, -250, 2050, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10024, 2, 152.0f, 0.30000001f, 87.0f, 0.0f, 1, 152.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10024, 100215, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10024, 3, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10024, 100216, "", Common::Rect(830, -250, 980, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10024, 4, 45.0f, 0.30000001f, 87.0f, 0.0f, 1, 45.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10025, 100215, "", Common::Rect(2590, -250, 2740, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10025, 0, 221.0f, 0.30000001f, 87.0f, 0.0f, 1, 221.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10025, 100216, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10025, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10025, 100217, "", Common::Rect(511, -108, 694, 113), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10025, 2, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10025, 100220, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10025, 3, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10026, 100217, "", Common::Rect(2160, -250, 2310, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10026, 0, 178.0f, 0.30000001f, 87.0f, 0.0f, 1, 178.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10027, 10021, "", Common::Rect(3120, -250, 3270, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10027, 0, 274.0f, 0.30000001f, 87.0f, 0.0f, 1, 274.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10027, 10013, "", Common::Rect(1156, -241, 1824, 143), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10027, 1, 90.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10028, 10021, "", Common::Rect(320, -250, 470, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10028, 0, 354.0f, 0.30000001f, 87.0f, 0.0f, 1, 354.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10028, 10022, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10028, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10029, 10022, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10029, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10029, 100210, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10029, 1, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100210, 10029, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100210, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100210, 100211, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100210, 1, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100210, 100213, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100210, 2, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100210, 100214, "", Common::Rect(830, -250, 980, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100210, 3, 45.0f, 0.30000001f, 87.0f, 0.0f, 1, 45.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100211, 10024, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100211, 0, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100211, 100210, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100211, 1, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100211, 100213, "", Common::Rect(1730, -250, 1880, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100211, 2, 135.0f, 0.30000001f, 87.0f, 0.0f, 1, 135.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100211, 100214, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100211, 3, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100211, 100215, "", Common::Rect(660, -250, 810, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100211, 4, 28.0f, 0.30000001f, 87.0f, 0.0f, 1, 28.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100212, 10024, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100212, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100212, 100215, "", Common::Rect(1730, -250, 1880, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100212, 1, 135.0f, 0.30000001f, 87.0f, 0.0f, 1, 135.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100212, 100216, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100212, 2, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100213, 100210, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100213, 0, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100213, 100211, "", Common::Rect(3530, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100213, 1, 315.0f, 0.30000001f, 87.0f, 0.0f, 1, 315.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100213, 100211, "", Common::Rect(0, -250, 80, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100213, 2, 315.0f, 0.30000001f, 87.0f, 0.0f, 1, 315.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100213, 100214, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100213, 3, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100213, 100218, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100213, 4, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100213, 100219, "", Common::Rect(790, -250, 940, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100213, 5, 41.0f, 0.30000001f, 87.0f, 0.0f, 1, 41.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100214, 10024, "", Common::Rect(100, -250, 250, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100214, 0, 332.0f, 0.30000001f, 87.0f, 0.0f, 1, 332.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100214, 100210, "", Common::Rect(2630, -250, 2780, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100214, 1, 225.0f, 0.30000001f, 87.0f, 0.0f, 1, 225.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100214, 100211, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100214, 2, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100214, 100213, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100214, 3, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100214, 100215, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100214, 4, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100214, 100218, "", Common::Rect(1770, -250, 1920, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100214, 5, 139.0f, 0.30000001f, 87.0f, 0.0f, 1, 139.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100214, 100219, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100214, 6, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100214, 100220, "", Common::Rect(630, -250, 780, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100214, 7, 25.0f, 0.30000001f, 87.0f, 0.0f, 1, 25.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100215, 10024, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100215, 0, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100215, 10025, "", Common::Rect(790, -250, 940, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100215, 1, 41.0f, 0.30000001f, 87.0f, 0.0f, 1, 41.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100215, 100211, "", Common::Rect(2460, -250, 2610, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100215, 2, 208.0f, 0.30000001f, 87.0f, 0.0f, 1, 208.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100215, 100212, "", Common::Rect(3530, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100215, 3, 315.0f, 0.30000001f, 87.0f, 0.0f, 1, 315.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100215, 100212, "", Common::Rect(0, -250, 80, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100215, 4, 315.0f, 0.30000001f, 87.0f, 0.0f, 1, 315.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100215, 100214, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100215, 5, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100215, 100216, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100215, 6, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100215, 100219, "", Common::Rect(1930, -250, 2080, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100215, 7, 155.0f, 0.30000001f, 87.0f, 0.0f, 1, 155.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100215, 100220, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100215, 8, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100216, 10024, "", Common::Rect(2630, -250, 2780, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100216, 0, 225.0f, 0.30000001f, 87.0f, 0.0f, 1, 225.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100216, 10025, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100216, 1, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100216, 100212, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100216, 2, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100216, 100215, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100216, 3, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100216, 100220, "", Common::Rect(1770, -250, 1920, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100216, 4, 139.0f, 0.30000001f, 87.0f, 0.0f, 1, 139.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100217, 10025, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100217, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100217, 10026, "", Common::Rect(360, -250, 510, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100217, 1, 358.0f, 0.30000001f, 87.0f, 0.0f, 1, 358.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100218, 10021, "", Common::Rect(1550, -250, 1700, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100218, 0, 117.0f, 0.30000001f, 87.0f, 0.0f, 1, 117.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100218, 10022, "", Common::Rect(2280, -250, 2430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100218, 1, 190.0f, 0.30000001f, 87.0f, 0.0f, 1, 190.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100218, 100213, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100218, 2, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100218, 100214, "", Common::Rect(3570, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100218, 3, 319.0f, 0.30000001f, 87.0f, 0.0f, 1, 319.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100218, 100214, "", Common::Rect(0, -250, 120, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100218, 4, 319.0f, 0.30000001f, 87.0f, 0.0f, 1, 319.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100218, 100219, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100218, 5, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100219, 10023, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100219, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100219, 100213, "", Common::Rect(2590, -250, 2740, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100219, 1, 221.0f, 0.30000001f, 87.0f, 0.0f, 1, 221.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100219, 100214, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100219, 2, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100219, 100215, "", Common::Rect(130, -250, 280, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100219, 3, 335.0f, 0.30000001f, 87.0f, 0.0f, 1, 335.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100219, 100218, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100219, 4, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100219, 100220, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100219, 5, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100220, 10025, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100220, 0, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100220, 100214, "", Common::Rect(2430, -250, 2580, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100220, 1, 205.0f, 0.30000001f, 87.0f, 0.0f, 1, 205.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100220, 100215, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100220, 2, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100220, 100216, "", Common::Rect(3570, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100220, 3, 319.0f, 0.30000001f, 87.0f, 0.0f, 1, 319.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100220, 100216, "", Common::Rect(0, -250, 120, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100220, 4, 319.0f, 0.30000001f, 87.0f, 0.0f, 1, 319.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100220, 100219, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100220, 5, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100220, 100221, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100220, 6, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100221, 10023, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100221, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100221, 100220, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100221, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1002502, 100217, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1002502, 0, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10031, 100113, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10031, 0, 350.0f, 0.0f, 87.0f, 0.0f, 1, 350.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10031, 10032, "", Common::Rect(2083, -161, 2455, 136), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(10031, 1, 181.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10032, 10031, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10032, 0, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10032, 100515, "", Common::Rect(1529, -113, 1641, 71), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(10032, 1, 105.0f, 0.0f, 87.0f, 0.0f, 1, 105.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100312, 10011302, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100312, 0, 350.0f, 0.0f, 87.0f, 0.0f, 1, 350.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100312, 100322, "", Common::Rect(2083, -161, 2455, 136), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(100312, 1, 181.0f, 0.0f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100322, 100312, "", Common::Rect(395, -94, 583, 119), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100322, 0, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100322, 10051502, "", Common::Rect(1529, -113, 1641, 71), true, kCursorMove, 1);
	_app->rotationSetMovabilityToRotation(100322, 1, 105.0f, 0.0f, 87.0f, 0.0f, 1, 105.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10041, 10043, "", Common::Rect(650, -250, 800, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10041, 0, 27.0f, 0.30000001f, 87.0f, 0.0f, 1, 27.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10041, 100411, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10041, 1, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10041, 100110, "", Common::Rect(2181, -59, 2320, 31), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10041, 2, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10042, 10044, "", Common::Rect(110, -250, 260, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10042, 0, 333.0f, 0.30000001f, 87.0f, 0.0f, 1, 333.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10042, 10045, "", Common::Rect(1030, -250, 1180, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10042, 1, 65.0f, 0.30000001f, 87.0f, 0.0f, 1, 65.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10042, 10048, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10042, 2, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10042, 100410, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10042, 3, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10042, 100412, "", Common::Rect(3300, -250, 3450, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10042, 4, 292.0f, 0.30000001f, 87.0f, 0.0f, 1, 292.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10043, 10041, "", Common::Rect(2450, -250, 2600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10043, 0, 207.0f, 0.30000001f, 87.0f, 0.0f, 1, 207.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10043, 100411, "", Common::Rect(2150, -250, 2300, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10043, 1, 177.0f, 0.30000001f, 87.0f, 0.0f, 1, 177.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10043, 100412, "", Common::Rect(1773, -101, 1891, 78), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10043, 2, 128.0f, 0.30000001f, 87.0f, 0.0f, 1, 128.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10044, 10042, "", Common::Rect(1910, -250, 2060, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10044, 0, 153.0f, 0.30000001f, 87.0f, 0.0f, 1, 153.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10044, 10045, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10044, 1, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10044, 10048, "", Common::Rect(1460, -250, 1610, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10044, 2, 108.0f, 0.30000001f, 87.0f, 0.0f, 1, 108.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10044, 100410, "", Common::Rect(2810, -250, 2960, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10044, 3, 243.0f, 0.30000001f, 87.0f, 0.0f, 1, 243.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10044, 100412, "", Common::Rect(3150, -250, 3300, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10044, 4, 277.0f, 0.30000001f, 87.0f, 0.0f, 1, 277.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10045, 10042, "", Common::Rect(2830, -250, 2980, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10045, 0, 245.0f, 0.30000001f, 87.0f, 0.0f, 1, 245.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10045, 10044, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10045, 1, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10045, 10046, "", Common::Rect(1190, -250, 1340, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10045, 2, 81.0f, 0.30000001f, 87.0f, 0.0f, 1, 81.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10045, 10047, "", Common::Rect(1450, -250, 1600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10045, 3, 107.0f, 0.30000001f, 87.0f, 0.0f, 1, 107.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10045, 10048, "", Common::Rect(1910, -250, 2060, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10045, 4, 153.0f, 0.30000001f, 87.0f, 0.0f, 1, 153.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10046, 10045, "", Common::Rect(2990, -250, 3140, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10046, 0, 261.0f, 0.30000001f, 87.0f, 0.0f, 1, 261.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10046, 10047, "", Common::Rect(1730, -250, 1880, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10046, 1, 135.0f, 0.30000001f, 87.0f, 0.0f, 1, 135.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10046, 10048, "", Common::Rect(2660, -250, 2810, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10046, 2, 228.0f, 0.30000001f, 87.0f, 0.0f, 1, 228.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10046, 100413, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10046, 3, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10047, 10045, "", Common::Rect(3250, -250, 3400, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10047, 0, 287.0f, 0.30000001f, 87.0f, 0.0f, 1, 287.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10047, 10046, "", Common::Rect(3530, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10047, 1, 315.0f, 0.30000001f, 87.0f, 0.0f, 1, 315.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10047, 10046, "", Common::Rect(0, -250, 80, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10047, 2, 315.0f, 0.30000001f, 87.0f, 0.0f, 1, 315.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10047, 10048, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10047, 3, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10048, 10042, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10048, 0, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10048, 10044, "", Common::Rect(3260, -250, 3410, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10048, 1, 288.0f, 0.30000001f, 87.0f, 0.0f, 1, 288.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10048, 10045, "", Common::Rect(110, -250, 260, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10048, 2, 333.0f, 0.30000001f, 87.0f, 0.0f, 1, 333.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10048, 10046, "", Common::Rect(860, -250, 1010, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10048, 3, 48.0f, 0.30000001f, 87.0f, 0.0f, 1, 48.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10048, 10047, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10048, 4, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100410, 10042, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100410, 0, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100410, 10044, "", Common::Rect(1010, -250, 1160, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100410, 1, 63.0f, 0.30000001f, 87.0f, 0.0f, 1, 63.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100410, 100411, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100410, 2, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100410, 100412, "", Common::Rect(60, -250, 210, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100410, 3, 328.0f, 0.30000001f, 87.0f, 0.0f, 1, 328.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100411, 10041, "", Common::Rect(3080, -250, 3230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100411, 0, 270.0f, 0.30000001f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100411, 10043, "", Common::Rect(350, -250, 500, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100411, 1, 357.0f, 0.30000001f, 87.0f, 0.0f, 1, 357.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100411, 100410, "", Common::Rect(1280, -250, 1430, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100411, 2, 90.0f, 0.30000001f, 87.0f, 0.0f, 1, 90.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100411, 100412, "", Common::Rect(940, -250, 1090, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100411, 3, 56.0f, 0.30000001f, 87.0f, 0.0f, 1, 56.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100412, 10042, "", Common::Rect(1500, -250, 1650, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100412, 0, 112.0f, 0.30000001f, 87.0f, 0.0f, 1, 112.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100412, 10043, "", Common::Rect(3460, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100412, 1, 308.0f, 0.30000001f, 87.0f, 0.0f, 1, 308.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100412, 10043, "", Common::Rect(0, -250, 10, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100412, 2, 308.0f, 0.30000001f, 87.0f, 0.0f, 1, 308.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100412, 10044, "", Common::Rect(1350, -250, 1500, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100412, 3, 97.0f, 0.30000001f, 87.0f, 0.0f, 1, 97.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100412, 100410, "", Common::Rect(1860, -250, 2010, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100412, 4, 148.0f, 0.30000001f, 87.0f, 0.0f, 1, 148.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100412, 100411, "", Common::Rect(2740, -250, 2890, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100412, 5, 236.0f, 0.30000001f, 87.0f, 0.0f, 1, 236.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100413, 10046, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100413, 0, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051, 100515, "", Common::Rect(3160, -250, 3310, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051, 0, 278.0f, 0.30000001f, 87.0f, 0.0f, 1, 278.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051, 100519, "", Common::Rect(1230, -250, 1380, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051, 1, 85.0f, 0.30000001f, 87.0f, 0.0f, 1, 85.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051, 100522, "", Common::Rect(1380, -250, 1530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051, 2, 100.0f, 0.30000001f, 87.0f, 0.0f, 1, 100.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052, 100516, "", Common::Rect(3030, -250, 3180, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052, 0, 265.0f, 0.30000001f, 87.0f, 0.0f, 1, 265.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052, 100517, "", Common::Rect(1230, -250, 1380, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052, 1, 85.0f, 0.30000001f, 87.0f, 0.0f, 1, 85.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052, 100519, "", Common::Rect(1730, -250, 1880, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052, 2, 135.0f, 0.30000001f, 87.0f, 0.0f, 1, 135.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10053, 10059, "", Common::Rect(954, -208, 1309, 89), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10053, 0, 74.0f, 0.30000001f, 87.0f, 0.0f, 1, 74.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10053, 100511, "", Common::Rect(1840, -250, 1990, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10053, 1, 146.0f, 0.30000001f, 87.0f, 0.0f, 1, 146.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10053, 100512, "", Common::Rect(2580, -250, 2730, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10053, 2, 220.0f, 0.30000001f, 87.0f, 0.0f, 1, 220.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10053, 100513, "", Common::Rect(2940, -250, 3090, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10053, 3, 256.0f, 0.30000001f, 87.0f, 0.0f, 1, 256.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10053, 100519, "", Common::Rect(3430, -250, 3580, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10053, 4, 305.0f, 0.30000001f, 87.0f, 0.0f, 1, 305.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10053, 100520, "", Common::Rect(310, -250, 460, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10053, 5, 353.0f, 0.30000001f, 87.0f, 0.0f, 1, 353.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10053, 100522, "", Common::Rect(3290, -250, 3440, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10053, 6, 291.0f, 0.30000001f, 87.0f, 0.0f, 1, 291.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10054, 10056, "", Common::Rect(80, -250, 230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10054, 0, 330.0f, 0.30000001f, 87.0f, 0.0f, 1, 330.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10054, 10058, "", Common::Rect(2900, -250, 3050, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10054, 1, 252.0f, 0.30000001f, 87.0f, 0.0f, 1, 252.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10055, 10056, "", Common::Rect(2810, -250, 2960, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10055, 0, 243.0f, 0.30000001f, 87.0f, 0.0f, 1, 243.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10055, 100521, "", Common::Rect(3240, -250, 3390, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10055, 1, 286.0f, 0.30000001f, 87.0f, 0.0f, 1, 286.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10056, 10054, "", Common::Rect(1880, -250, 2030, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10056, 0, 150.0f, 0.30000001f, 87.0f, 0.0f, 1, 150.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10056, 10055, "", Common::Rect(1010, -250, 1160, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10056, 1, 63.0f, 0.30000001f, 87.0f, 0.0f, 1, 63.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10056, 10057, "", Common::Rect(2900, -250, 3050, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10056, 2, 252.0f, 0.30000001f, 87.0f, 0.0f, 1, 252.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10056, 100521, "", Common::Rect(10, -250, 160, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10056, 3, 323.0f, 0.30000001f, 87.0f, 0.0f, 1, 323.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10057, 10056, "", Common::Rect(1100, -250, 1250, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10057, 0, 72.0f, 0.30000001f, 87.0f, 0.0f, 1, 72.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10057, 10058, "", Common::Rect(1880, -250, 2030, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10057, 1, 150.0f, 0.30000001f, 87.0f, 0.0f, 1, 150.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10057, 10059, "", Common::Rect(2840, -250, 2990, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10057, 2, 246.0f, 0.30000001f, 87.0f, 0.0f, 1, 246.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10058, 10054, "", Common::Rect(1100, -250, 1250, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10058, 0, 72.0f, 0.30000001f, 87.0f, 0.0f, 1, 72.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10058, 10057, "", Common::Rect(80, -250, 230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10058, 1, 330.0f, 0.30000001f, 87.0f, 0.0f, 1, 330.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10058, 100510, "", Common::Rect(2830, -250, 2980, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10058, 2, 245.0f, 0.30000001f, 87.0f, 0.0f, 1, 245.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10059, 10053, "", Common::Rect(2920, -250, 3070, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10059, 0, 254.0f, 0.30000001f, 87.0f, 0.0f, 1, 254.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10059, 10057, "", Common::Rect(1040, -250, 1190, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10059, 1, 66.0f, 0.30000001f, 87.0f, 0.0f, 1, 66.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10059, 100510, "", Common::Rect(1820, -250, 1970, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10059, 2, 144.0f, 0.30000001f, 87.0f, 0.0f, 1, 144.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100510, 10058, "", Common::Rect(1030, -250, 1180, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100510, 0, 65.0f, 0.30000001f, 87.0f, 0.0f, 1, 65.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100510, 10059, "", Common::Rect(20, -250, 170, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100510, 1, 324.0f, 0.30000001f, 87.0f, 0.0f, 1, 324.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100510, 100511, "", Common::Rect(2740, -250, 2890, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100510, 2, 236.0f, 0.30000001f, 87.0f, 0.0f, 1, 236.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100511, 10053, "", Common::Rect(40, -250, 190, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100511, 0, 326.0f, 0.30000001f, 87.0f, 0.0f, 1, 326.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100511, 100510, "", Common::Rect(940, -250, 1090, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100511, 1, 56.0f, 0.30000001f, 87.0f, 0.0f, 1, 56.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100511, 100512, "", Common::Rect(2970, -250, 3120, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100511, 2, 259.0f, 0.30000001f, 87.0f, 0.0f, 1, 259.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100512, 10053, "", Common::Rect(780, -250, 930, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100512, 0, 40.0f, 0.30000001f, 87.0f, 0.0f, 1, 40.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100512, 100511, "", Common::Rect(1170, -250, 1320, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100512, 1, 79.0f, 0.30000001f, 87.0f, 0.0f, 1, 79.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100512, 100513, "", Common::Rect(3310, -250, 3460, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100512, 2, 293.0f, 0.30000001f, 87.0f, 0.0f, 1, 293.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100513, 10053, "", Common::Rect(1140, -250, 1290, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100513, 0, 76.0f, 0.30000001f, 87.0f, 0.0f, 1, 76.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100513, 100512, "", Common::Rect(1510, -250, 1660, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100513, 1, 113.0f, 0.30000001f, 87.0f, 0.0f, 1, 113.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100513, 100514, "", Common::Rect(3350, -250, 3500, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100513, 2, 297.0f, 0.30000001f, 87.0f, 0.0f, 1, 297.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100513, 100522, "", Common::Rect(500, -250, 650, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100513, 3, 12.0f, 0.30000001f, 87.0f, 0.0f, 1, 12.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100514, 100513, "", Common::Rect(1550, -250, 1700, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100514, 0, 117.0f, 0.30000001f, 87.0f, 0.0f, 1, 117.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100514, 100515, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100514, 1, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100515, 10032, "", Common::Rect(3104, -257, 3599, 136), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100515, 0, 270.0f, 0.0f, 87.0f, 0.0f, 1, 270.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100515, 10051, "", Common::Rect(1360, -250, 1510, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100515, 1, 98.0f, 0.30000001f, 87.0f, 0.0f, 1, 98.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100515, 100514, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100515, 2, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100515, 100516, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100515, 3, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100516, 10052, "", Common::Rect(1230, -250, 1380, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100516, 0, 85.0f, 0.30000001f, 87.0f, 0.0f, 1, 85.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100516, 100515, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100516, 1, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100517, 10052, "", Common::Rect(3030, -250, 3180, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100517, 0, 265.0f, 0.30000001f, 87.0f, 0.0f, 1, 265.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100517, 100518, "", Common::Rect(1230, -250, 1380, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100517, 1, 85.0f, 0.30000001f, 87.0f, 0.0f, 1, 85.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100517, 100520, "", Common::Rect(1950, -250, 2100, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100517, 2, 157.0f, 0.30000001f, 87.0f, 0.0f, 1, 157.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100518, 100517, "", Common::Rect(3030, -250, 3180, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100518, 0, 265.0f, 0.30000001f, 87.0f, 0.0f, 1, 265.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100518, 100521, "", Common::Rect(1910, -250, 2060, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100518, 1, 153.0f, 0.30000001f, 87.0f, 0.0f, 1, 153.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100519, 10051, "", Common::Rect(3030, -250, 3180, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100519, 0, 265.0f, 0.30000001f, 87.0f, 0.0f, 1, 265.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100519, 10052, "", Common::Rect(3530, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100519, 1, 315.0f, 0.30000001f, 87.0f, 0.0f, 1, 315.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100519, 10052, "", Common::Rect(0, -250, 80, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100519, 2, 315.0f, 0.30000001f, 87.0f, 0.0f, 1, 315.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100519, 10053, "", Common::Rect(1630, -250, 1780, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100519, 3, 125.0f, 0.30000001f, 87.0f, 0.0f, 1, 125.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100519, 100520, "", Common::Rect(1220, -250, 1370, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100519, 4, 84.0f, 0.30000001f, 87.0f, 0.0f, 1, 84.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100519, 100522, "", Common::Rect(2270, -250, 2420, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100519, 5, 189.0f, 0.30000001f, 87.0f, 0.0f, 1, 189.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100520, 10053, "", Common::Rect(2110, -250, 2260, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100520, 0, 173.0f, 0.30000001f, 87.0f, 0.0f, 1, 173.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100520, 100517, "", Common::Rect(150, -250, 300, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100520, 1, 337.0f, 0.30000001f, 87.0f, 0.0f, 1, 337.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100520, 100519, "", Common::Rect(3020, -250, 3170, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100520, 2, 264.0f, 0.30000001f, 87.0f, 0.0f, 1, 264.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100520, 100521, "", Common::Rect(1210, -250, 1360, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100520, 3, 83.0f, 0.30000001f, 87.0f, 0.0f, 1, 83.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100520, 100522, "", Common::Rect(2850, -250, 3000, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100520, 4, 247.0f, 0.30000001f, 87.0f, 0.0f, 1, 247.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100521, 10055, "", Common::Rect(1440, -250, 1590, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100521, 0, 106.0f, 0.30000001f, 87.0f, 0.0f, 1, 106.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100521, 10056, "", Common::Rect(1810, -250, 1960, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100521, 1, 143.0f, 0.30000001f, 87.0f, 0.0f, 1, 143.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100521, 100518, "", Common::Rect(110, -250, 260, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100521, 2, 333.0f, 0.30000001f, 87.0f, 0.0f, 1, 333.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100521, 100520, "", Common::Rect(3010, -250, 3160, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100521, 3, 263.0f, 0.30000001f, 87.0f, 0.0f, 1, 263.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100522, 10051, "", Common::Rect(3180, -250, 3330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100522, 0, 280.0f, 0.30000001f, 87.0f, 0.0f, 1, 280.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100522, 10053, "", Common::Rect(1490, -250, 1640, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100522, 1, 111.0f, 0.30000001f, 87.0f, 0.0f, 1, 111.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100522, 100513, "", Common::Rect(2300, -250, 2450, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100522, 2, 192.0f, 0.30000001f, 87.0f, 0.0f, 1, 192.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100522, 100519, "", Common::Rect(470, -250, 620, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100522, 3, 9.0f, 0.30000001f, 87.0f, 0.0f, 1, 9.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(100522, 100520, "", Common::Rect(1050, -250, 1200, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(100522, 4, 67.0f, 0.30000001f, 87.0f, 0.0f, 1, 67.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005102, 10051502, "", Common::Rect(3160, -250, 3310, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005102, 0, 278.0f, 0.30000001f, 87.0f, 0.0f, 1, 278.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005102, 10051902, "", Common::Rect(1230, -250, 1380, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005102, 1, 85.0f, 0.30000001f, 87.0f, 0.0f, 1, 85.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005102, 10052202, "", Common::Rect(1380, -250, 1530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005102, 2, 100.0f, 0.30000001f, 87.0f, 0.0f, 1, 100.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005202, 10051602, "", Common::Rect(3030, -250, 3180, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005202, 0, 265.0f, 0.30000001f, 87.0f, 0.0f, 1, 265.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005202, 10051702, "", Common::Rect(1230, -250, 1380, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005202, 1, 85.0f, 0.30000001f, 87.0f, 0.0f, 1, 85.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005202, 10051902, "", Common::Rect(1730, -250, 1880, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005202, 2, 135.0f, 0.30000001f, 87.0f, 0.0f, 1, 135.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005302, 1005902, "", Common::Rect(1120, -250, 1270, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005302, 0, 74.0f, 0.30000001f, 87.0f, 0.0f, 1, 74.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005302, 10051102, "", Common::Rect(1840, -250, 1990, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005302, 1, 146.0f, 0.30000001f, 87.0f, 0.0f, 1, 146.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005302, 10051202, "", Common::Rect(2580, -250, 2730, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005302, 2, 220.0f, 0.30000001f, 87.0f, 0.0f, 1, 220.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005302, 10051302, "", Common::Rect(2940, -250, 3090, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005302, 3, 256.0f, 0.30000001f, 87.0f, 0.0f, 1, 256.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005302, 10051902, "", Common::Rect(3430, -250, 3580, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005302, 4, 305.0f, 0.30000001f, 87.0f, 0.0f, 1, 305.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005302, 10052002, "", Common::Rect(310, -250, 460, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005302, 5, 353.0f, 0.30000001f, 87.0f, 0.0f, 1, 353.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005302, 10052202, "", Common::Rect(3290, -250, 3440, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005302, 6, 291.0f, 0.30000001f, 87.0f, 0.0f, 1, 291.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005402, 1005602, "", Common::Rect(80, -250, 230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005402, 0, 330.0f, 0.30000001f, 87.0f, 0.0f, 1, 330.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005402, 1005802, "", Common::Rect(2900, -250, 3050, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005402, 1, 252.0f, 0.30000001f, 87.0f, 0.0f, 1, 252.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005502, 1005602, "", Common::Rect(2810, -250, 2960, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005502, 0, 243.0f, 0.30000001f, 87.0f, 0.0f, 1, 243.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005502, 10052102, "", Common::Rect(3240, -250, 3390, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005502, 1, 286.0f, 0.30000001f, 87.0f, 0.0f, 1, 286.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005602, 1005402, "", Common::Rect(1880, -250, 2030, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005602, 0, 150.0f, 0.30000001f, 87.0f, 0.0f, 1, 150.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005602, 1005502, "", Common::Rect(1010, -250, 1160, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005602, 1, 63.0f, 0.30000001f, 87.0f, 0.0f, 1, 63.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005602, 1005702, "", Common::Rect(2900, -250, 3050, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005602, 2, 252.0f, 0.30000001f, 87.0f, 0.0f, 1, 252.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005602, 10052102, "", Common::Rect(10, -250, 160, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005602, 3, 323.0f, 0.30000001f, 87.0f, 0.0f, 1, 323.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005702, 1005602, "", Common::Rect(1100, -250, 1250, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005702, 0, 72.0f, 0.30000001f, 87.0f, 0.0f, 1, 72.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005702, 1005802, "", Common::Rect(1880, -250, 2030, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005702, 1, 150.0f, 0.30000001f, 87.0f, 0.0f, 1, 150.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005702, 1005902, "", Common::Rect(2840, -250, 2990, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005702, 2, 246.0f, 0.30000001f, 87.0f, 0.0f, 1, 246.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005802, 1005402, "", Common::Rect(1100, -250, 1250, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005802, 0, 72.0f, 0.30000001f, 87.0f, 0.0f, 1, 72.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005802, 1005702, "", Common::Rect(80, -250, 230, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005802, 1, 330.0f, 0.30000001f, 87.0f, 0.0f, 1, 330.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005802, 10051002, "", Common::Rect(2830, -250, 2980, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005802, 2, 245.0f, 0.30000001f, 87.0f, 0.0f, 1, 245.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005902, 1005302, "", Common::Rect(2920, -250, 3070, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005902, 0, 254.0f, 0.30000001f, 87.0f, 0.0f, 1, 254.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005902, 1005702, "", Common::Rect(1040, -250, 1190, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005902, 1, 66.0f, 0.30000001f, 87.0f, 0.0f, 1, 66.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(1005902, 10051002, "", Common::Rect(1820, -250, 1970, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(1005902, 2, 144.0f, 0.30000001f, 87.0f, 0.0f, 1, 144.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051002, 1005802, "", Common::Rect(1030, -250, 1180, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051002, 0, 65.0f, 0.30000001f, 87.0f, 0.0f, 1, 65.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051002, 1005902, "", Common::Rect(20, -250, 170, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051002, 1, 324.0f, 0.30000001f, 87.0f, 0.0f, 1, 324.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051002, 10051102, "", Common::Rect(2740, -250, 2890, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051002, 2, 236.0f, 0.30000001f, 87.0f, 0.0f, 1, 236.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051102, 1005302, "", Common::Rect(40, -250, 190, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051102, 0, 326.0f, 0.30000001f, 87.0f, 0.0f, 1, 326.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051102, 10051002, "", Common::Rect(940, -250, 1090, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051102, 1, 56.0f, 0.30000001f, 87.0f, 0.0f, 1, 56.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051102, 10051202, "", Common::Rect(2970, -250, 3120, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051102, 2, 259.0f, 0.30000001f, 87.0f, 0.0f, 1, 259.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051202, 1005302, "", Common::Rect(780, -250, 930, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051202, 0, 40.0f, 0.30000001f, 87.0f, 0.0f, 1, 40.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051202, 10051102, "", Common::Rect(1170, -250, 1320, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051202, 1, 79.0f, 0.30000001f, 87.0f, 0.0f, 1, 79.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051202, 10051302, "", Common::Rect(3310, -250, 3460, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051202, 2, 293.0f, 0.30000001f, 87.0f, 0.0f, 1, 293.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051302, 1005302, "", Common::Rect(1140, -250, 1290, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051302, 0, 76.0f, 0.30000001f, 87.0f, 0.0f, 1, 76.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051302, 10051202, "", Common::Rect(1510, -250, 1660, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051302, 1, 113.0f, 0.30000001f, 87.0f, 0.0f, 1, 113.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051302, 10051402, "", Common::Rect(3350, -250, 3500, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051302, 2, 297.0f, 0.30000001f, 87.0f, 0.0f, 1, 297.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051302, 10052202, "", Common::Rect(500, -250, 650, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051302, 3, 12.0f, 0.30000001f, 87.0f, 0.0f, 1, 12.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051402, 10051302, "", Common::Rect(1550, -250, 1700, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051402, 0, 117.0f, 0.30000001f, 87.0f, 0.0f, 1, 117.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051402, 10051502, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051402, 1, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051502, 100322, "", Common::Rect(3104, -257, 3599, 136), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051502, 0, 359.0f, 0.0f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051502, 1005102, "", Common::Rect(1360, -250, 1510, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051502, 1, 98.0f, 0.30000001f, 87.0f, 0.0f, 1, 98.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051502, 10051402, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051502, 2, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051502, 10051602, "", Common::Rect(380, -250, 530, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051502, 3, 0.0f, 0.30000001f, 87.0f, 0.0f, 1, 0.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051602, 1005202, "", Common::Rect(1230, -250, 1380, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051602, 0, 85.0f, 0.30000001f, 87.0f, 0.0f, 1, 85.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051602, 10051502, "", Common::Rect(2180, -250, 2330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051602, 1, 180.0f, 0.30000001f, 87.0f, 0.0f, 1, 180.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051702, 1005202, "", Common::Rect(3030, -250, 3180, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051702, 0, 265.0f, 0.30000001f, 87.0f, 0.0f, 1, 265.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051702, 10051802, "", Common::Rect(1230, -250, 1380, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051702, 1, 85.0f, 0.30000001f, 87.0f, 0.0f, 1, 85.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051702, 10052002, "", Common::Rect(1950, -250, 2100, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051702, 2, 157.0f, 0.30000001f, 87.0f, 0.0f, 1, 157.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051802, 10051702, "", Common::Rect(3030, -250, 3180, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051802, 0, 265.0f, 0.30000001f, 87.0f, 0.0f, 1, 265.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051802, 10052102, "", Common::Rect(1910, -250, 2060, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051802, 1, 153.0f, 0.30000001f, 87.0f, 0.0f, 1, 153.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051902, 1005102, "", Common::Rect(3030, -250, 3180, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051902, 0, 265.0f, 0.30000001f, 87.0f, 0.0f, 1, 265.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051902, 1005202, "", Common::Rect(3530, -250, 3600, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051902, 1, 315.0f, 0.30000001f, 87.0f, 0.0f, 1, 315.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051902, 1005202, "", Common::Rect(0, -250, 80, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051902, 2, 315.0f, 0.30000001f, 87.0f, 0.0f, 1, 315.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051902, 1005302, "", Common::Rect(1630, -250, 1780, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051902, 3, 125.0f, 0.30000001f, 87.0f, 0.0f, 1, 125.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051902, 10052002, "", Common::Rect(1220, -250, 1370, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051902, 4, 84.0f, 0.30000001f, 87.0f, 0.0f, 1, 84.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10051902, 10052202, "", Common::Rect(2270, -250, 2420, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10051902, 5, 189.0f, 0.30000001f, 87.0f, 0.0f, 1, 189.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052002, 1005302, "", Common::Rect(2110, -250, 2260, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052002, 0, 173.0f, 0.30000001f, 87.0f, 0.0f, 1, 173.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052002, 10051702, "", Common::Rect(150, -250, 300, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052002, 1, 337.0f, 0.30000001f, 87.0f, 0.0f, 1, 337.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052002, 10051902, "", Common::Rect(3020, -250, 3170, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052002, 2, 264.0f, 0.30000001f, 87.0f, 0.0f, 1, 264.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052002, 10052102, "", Common::Rect(1210, -250, 1360, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052002, 3, 83.0f, 0.30000001f, 87.0f, 0.0f, 1, 83.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052002, 10052202, "", Common::Rect(2850, -250, 3000, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052002, 4, 247.0f, 0.30000001f, 87.0f, 0.0f, 1, 247.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052102, 1005502, "", Common::Rect(1440, -250, 1590, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052102, 0, 106.0f, 0.30000001f, 87.0f, 0.0f, 1, 106.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052102, 1005602, "", Common::Rect(1810, -250, 1960, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052102, 1, 143.0f, 0.30000001f, 87.0f, 0.0f, 1, 143.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052102, 10051802, "", Common::Rect(110, -250, 260, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052102, 2, 333.0f, 0.30000001f, 87.0f, 0.0f, 1, 333.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052102, 10052002, "", Common::Rect(3010, -250, 3160, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052102, 3, 263.0f, 0.30000001f, 87.0f, 0.0f, 1, 263.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052202, 1005102, "", Common::Rect(3180, -250, 3330, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052202, 0, 280.0f, 0.30000001f, 87.0f, 0.0f, 1, 280.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052202, 1005302, "", Common::Rect(1490, -250, 1640, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052202, 1, 111.0f, 0.30000001f, 87.0f, 0.0f, 1, 111.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052202, 10051302, "", Common::Rect(2300, -250, 2450, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052202, 2, 192.0f, 0.30000001f, 87.0f, 0.0f, 1, 192.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052202, 10051902, "", Common::Rect(470, -250, 620, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052202, 3, 9.0f, 0.30000001f, 87.0f, 0.0f, 1, 9.0f, 0.30000001f, 87.0f);
	_app->rotationAddMovabilityToRotation(10052202, 10052002, "", Common::Rect(1050, -250, 1200, 250), true, kCursorMove, 0);
	_app->rotationSetMovabilityToRotation(10052202, 4, 67.0f, 0.30000001f, 87.0f, 0.0f, 1, 67.0f, 0.30000001f, 87.0f);
	_app->objectAdd(kObject1094, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1094, 10012, Common::Rect(3086, -148, 3315,  201), false, kCursorAction, 90);
	_app->objectAddRotationAccessibility(kObject1094, 10022, Common::Rect(2123, -175, 2322,  -15), false, kCursorAction, 89);
	_app->objectAddRotationAccessibility(kObject1094, 10021, Common::Rect(1030,  -98, 1109,   82), false, kCursorAction, 105);
	_app->objectAdd(kObject1000, "", "", 1);
	_app->objectAddPresentation(kObject1000);
	_app->objectPresentationAddImageToRotation(kObject1000, 0, 10014, 1);
	_app->objectAdd(kObject1001, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1001, 10012, Common::Rect(1105, -31, 1228, 382), false, kCursorAction, 0);
	_app->objectAddPresentation(kObject1001);
	_app->objectPresentationAddImageToRotation(kObject1001, 0, 10012, 0);
	_app->objectPresentationAddAnimationToPuzzle(kObject1001, 0, kPuzzle1000, "s00a01n02p01s01", kImageTypeBMP, Common::Point(151, 34), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1001, 0, 0, 1001);
	_app->objectAddPresentation(kObject1001);
	_app->objectPresentationAddAnimationToPuzzle(kObject1001, 1, kPuzzle1000, "s00a01n02p01s02", kImageTypeBMP, Common::Point(219, 172), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1001, 1, 0, 1001);
	_app->objectAddPresentation(kObject1001);
	_app->objectPresentationAddAnimationToPuzzle(kObject1001, 2, kPuzzle1000, "s00a01n02p01s03", kImageTypeBMP, Common::Point(421, 44), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1001, 2, 0, 1002);
	_app->objectAddPresentation(kObject1001);
	_app->objectPresentationAddImageToRotation(kObject1001, 3, 10012, 2);
	_app->objectPresentationAddAnimationToPuzzle(kObject1001, 3, kPuzzle1001, "s00a01n02p02s01", kImageTypeBMP, Common::Point(59, 20), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1001, 3, 0, 1001);
	_app->objectAddPresentation(kObject1001);
	_app->objectPresentationAddAnimationToPuzzle(kObject1001, 4, kPuzzle1001, "s00a01n02p02s02", kImageTypeBMP, Common::Point(349, 16), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1001, 4, 0, 1002);
	_app->objectAdd(kObject1005, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1005, 10012, Common::Rect(1364, -31, 1582, 350), false, kCursorAction, 0);
	_app->objectAddPresentation(kObject1005);
	_app->objectPresentationAddImageToRotation(kObject1005, 0, 10012, 3);
	_app->objectAdd(kObjectStick, "Stick", "stick", 9);
	_app->objectAddRotationAccessibility(kObjectStick, 10012, Common::Rect(1144, -68, 1225, 168), false, kCursorTake, 0);
	_app->objectAddRotationAccessibility(kObjectStick, 10012, Common::Rect(1916, -28, 1956,  17), false, kCursorActionObject, 1);
	_app->objectAddPresentation(kObjectStick);
	_app->objectPresentationAddImageToRotation(kObjectStick, 0, 10012, 4);
	_app->objectAdd(kObjectRope, "Rope", "rope", 1);
	_app->objectAddRotationAccessibility(kObjectRope, 10012, Common::Rect(1896, 141, 1947, 178), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObjectRope);
	_app->objectPresentationAddImageToRotation(kObjectRope, 0, 10012, 5);
	_app->objectAdd(kObjectSalad, "Salad", "salad", 9);
	_app->objectAddRotationAccessibility(kObjectSalad, 10012, Common::Rect(511, 152, 639, 252), false, kCursorTake, 0);
	_app->objectAdd(kObject1006, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1006, 10012, Common::Rect(2105, 82, 2293, 238), false, kCursorActionObject, 0);
	_app->objectAddRotationAccessibility(kObject1006, 10012, Common::Rect(1963, 12, 2074, 178), false, kCursorActionObject, 1);
	_app->objectAddPresentation(kObject1006);
	_app->objectPresentationAddAnimationToRotation(kObject1006, 0, 10012, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1006, 0, 0, 1001);
	_app->objectAdd(kObjectDates, "Dates", "dates", 1);
	_app->objectAdd(kObject10071, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10071, 10022, Common::Rect(1917, 14, 2021, 380), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject10071, 10022, Common::Rect(1917, 14, 2021, 380), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject10071);
	_app->objectPresentationAddImageToRotation(kObject10071, 0, 10022, 0);
	_app->objectPresentationAddAnimationToRotation(kObject10071, 0, 10022, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject10071, 0, 0, 1001);
	_app->objectAddPresentation(kObject10071);
	_app->objectPresentationAddAnimationToPuzzle(kObject10071, 1, kPuzzle1004, "s00a02n02p02s01", kImageTypeBMP, Common::Point(134, 35), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10071, 1, 0, 1001);
	_app->objectAddPresentation(kObject10071);
	_app->objectPresentationAddAnimationToPuzzle(kObject10071, 2, kPuzzle1004, "s00a02n02p02s02", kImageTypeBMP, Common::Point(447, 141), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10071, 2, 0, 1002);
	_app->objectAdd(kObject1008, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1008, 10022, Common::Rect(2046, -231, 2336, 171), false, kCursorAction, 0);
	_app->objectAdd(kObject1009, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1009, 10022, Common::Rect(2151,    8, 2218, 213), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1009, 10022, Common::Rect(2151,    8, 2218, 213), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1009);
	_app->objectPresentationAddImageToRotation(kObject1009, 0, 10022, 2);
	_app->objectPresentationAddAnimationToRotation(kObject1009, 0, 10022, 3, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1009, 0, 0, 1001);
	_app->objectAddPresentation(kObject1009);
	_app->objectPresentationAddAnimationToPuzzle(kObject1009, 1, kPuzzle1007, "s00a02n02p04s01", kImageTypeBMP, Common::Point(471, 153), kDrawTypeNormal, 1000, 28, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1009, 1, 0, 1001);
	_app->objectAdd(kObject1010, "", "", 1);
	_app->objectAddPresentation(kObject1010);
	_app->objectPresentationAddImageToRotation(kObject1010, 0, 10021, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1010, 0, 10021, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1010, 0, 0, 1002);
	_app->objectAdd(kObject1011, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1011, 10021, Common::Rect(1429, -10, 1548, 392), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1011, 10021, Common::Rect(1429, -10, 1548, 392), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1011);
	_app->objectPresentationAddImageToRotation(kObject1011, 0, 10021, 2);
	_app->objectPresentationAddAnimationToRotation(kObject1011, 0, 10021, 3, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1011, 0, 0, 1001);
	_app->objectAddPresentation(kObject1011);
	_app->objectPresentationAddAnimationToPuzzle(kObject1011, 1, kPuzzle1010, "s00a02n01p03s01", kImageTypeBMP, Common::Point(161, 43), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1011, 1, 0, 1001);
	_app->objectAddPresentation(kObject1011);
	_app->objectPresentationAddAnimationToPuzzle(kObject1011, 2, kPuzzle1010, "s00a02n01p03s02", kImageTypeBMP, Common::Point(340, 26), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1011, 2, 0, 1002);
	_app->objectAdd(kObject1012, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1012, 10023, Common::Rect(1226, -5, 1288, 168), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1012, 10023, Common::Rect(1226, -5, 1288, 168), false, kCursorActionObject, 1);
	_app->objectAddRotationAccessibility(kObject1012, 10023, Common::Rect(1226, -5, 1288, 168), false, kCursorIdle, 2);
	_app->objectAddPresentation(kObject1012);
	_app->objectPresentationAddImageToRotation(kObject1012, 0, 10023, 2);
	_app->objectAdd(kObject1013, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1013, 10023, Common::Rect(1226, -5, 1288, 168), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1013, 10023, Common::Rect(1226, -5, 1288, 168), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1013);
	_app->objectPresentationAddImageToRotation(kObject1013, 0, 10023, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1013, 0, 10023, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1013, 0, 0, 1001);
	_app->objectAddPresentation(kObject1013);
	_app->objectPresentationAddAnimationToPuzzle(kObject1013, 1, kPuzzle1013, "s00a02n03p01s01", kImageTypeBMP, Common::Point(183, 55), kDrawTypeNormal, 1000, 15, 12.5f, 16);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1013, 1, 0, 1001);
	_app->objectAddPresentation(kObject1013);
	_app->objectPresentationAddAnimationToPuzzle(kObject1013, 2, kPuzzle1013, "s00a02n03p01s02", kImageTypeBMP, Common::Point(372, 133), kDrawTypeNormal, 1000, 15, 12.5f, 16);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1013, 2, 0, 1002);
	_app->objectAdd(kObject1016, "", "", 1);
	_app->objectAddPresentation(kObject1016);
	_app->objectPresentationAddAnimationToPuzzle(kObject1016, 0, kPuzzle1015, "s00a01n01p01s01", kImageTypeBMP, Common::Point(170, 191), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1016, 0, 0, 1001);
	_app->objectAddPresentation(kObject1016);
	_app->objectPresentationAddAnimationToPuzzle(kObject1016, 1, kPuzzle1015, "s00a01n01p01s02", kImageTypeBMP, Common::Point(324, 43), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1016, 1, 0, 1002);
	_app->objectAdd(kObject1017, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1017, 10031, Common::Rect(1645, 0, 1734, 192), false, kCursorAction, 0);
	_app->objectAddPresentation(kObject1017);
	_app->objectPresentationAddImageToRotation(kObject1017, 0, 10031, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1017, 0, 10031, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1017, 0, 0, 1001);
	_app->objectAddPresentation(kObject1017);
	_app->objectPresentationAddAnimationToPuzzle(kObject1017, 1, kPuzzle1016, "s00a03n01p01s01", kImageTypeBMP, Common::Point(60, 185), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1017, 1, 0, 1001);
	_app->objectAdd(kObjectBracelet, "Bracelet", "acelet", 1);
	_app->objectAddRotationAccessibility(kObjectBracelet, 10032, Common::Rect(689, 311, 752, 346), false, kCursorTake, 0);
	_app->objectAddPresentation(kObjectBracelet);
	_app->objectPresentationAddImageToRotation(kObjectBracelet, 0, 10032, 0);
	_app->objectAdd(kObject1019, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1019, 10032, Common::Rect(2002, 66, 2090, 266), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1019, 10032, Common::Rect(2002, 66, 2090, 266), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1019);
	_app->objectPresentationAddImageToRotation(kObject1019, 0, 10032, 2);
	_app->objectPresentationAddAnimationToRotation(kObject1019, 0, 10032, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1019, 0, 0, 1001);
	_app->objectAddPresentation(kObject1019);
	_app->objectPresentationAddAnimationToPuzzle(kObject1019, 1, kPuzzle1017, "s00a03n02p01s01", kImageTypeBMP, Common::Point(217, 103), kDrawTypeNormal, 1000, 28, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1019, 1, 0, 1001);
	_app->objectAddPresentation(kObject1019);
	_app->objectPresentationAddAnimationToPuzzle(kObject1019, 2, kPuzzle1017, "s00a03n02p01s02", kImageTypeBMP, Common::Point(422, 90), kDrawTypeNormal, 1000, 28, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1019, 2, 0, 1002);
	_app->objectAdd(kObject1020, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1020, 10018, Common::Rect(1272, -15, 1346, 182), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1020, 10018, Common::Rect(1272, -15, 1346, 182), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1020);
	_app->objectPresentationAddImageToRotation(kObject1020, 0, 10018, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1020, 0, 10018, 1, 14, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1020, 0, 0, 1001);
	_app->objectAddPresentation(kObject1020);
	_app->objectPresentationAddAnimationToPuzzle(kObject1020, 1, kPuzzle1020, "s00a01n08p01s01", kImageTypeBMP, Common::Point(140, 67), kDrawTypeNormal, 1000, 29, 12.5f, 16);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1020, 1, 0, 1001);
	_app->objectAdd(kObject1021, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1021, 100110, Common::Rect(3375, 49, 3480, 287), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1021, 100110, Common::Rect(3375, 49, 3480, 287), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1021);
	_app->objectPresentationAddImageToRotation(kObject1021, 0, 100110, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1021, 0, 100110, 1, 13, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1021, 0, 0, 1001);
	_app->objectAddPresentation(kObject1021);
	_app->objectPresentationAddImageToRotation(kObject1021, 1, 100110, 2);
	_app->objectAddPresentation(kObject1021);
	_app->objectPresentationAddAnimationToPuzzle(kObject1021, 2, kPuzzle1023, "s00a01n10p01s01", kImageTypeBMP, Common::Point(160, 85), kDrawTypeNormal, 1000, 30, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1021, 2, 0, 1001);
	_app->objectAdd(kObject1023, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1023, 10024, Common::Rect(3258, -12, 3436, 422), false, kCursorAction, 0);
	_app->objectAddPresentation(kObject1023);
	_app->objectPresentationAddImageToRotation(kObject1023, 0, 10024, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1023, 0, 10024, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1023, 0, 0, 1001);
	_app->objectAddPresentation(kObject1023);
	_app->objectPresentationAddAnimationToPuzzle(kObject1023, 1, kPuzzle1026, "s00a02n04p01s01", kImageTypeBMP, Common::Point(165, 53), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1023, 1, 0, 1001);
	_app->objectAdd(kObject1039, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1039, 10013, Common::Rect(3382, -47, 3434, 40), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1039, 10013, Common::Rect(3382, -47, 3434, 40), false, kCursorIdle, 1);
	_app->objectAddRotationAccessibility(kObject1039, 10013, Common::Rect(3382, -47, 3434, 40), false, kCursorAction, 2);
	_app->objectAddRotationAccessibility(kObject1039, 10013, Common::Rect(3382, -47, 3434, 40), false, kCursorIdle, 3);
	_app->objectAddPresentation(kObject1039);
	_app->objectPresentationAddImageToRotation(kObject1039, 0, 10013, 2);
	_app->objectPresentationAddAnimationToPuzzle(kObject1039, 0, kPuzzle1027, "s00a01n03p01s01", kImageTypeBMP, Common::Point(161, 224), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1039, 0, 0, 1001);
	_app->objectAddPresentation(kObject1039);
	_app->objectPresentationAddAnimationToPuzzle(kObject1039, 1, kPuzzle1027, "s00a01n03p01s02", kImageTypeBMP, Common::Point(366, 204), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1039, 1, 0, 1001);
	_app->objectAdd(kObject1040, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1040, 10013, Common::Rect(2763, 35, 2949, 133), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1040, 10013, Common::Rect(2763, 35, 2949, 133), false, kCursorActionObject, 1);
	_app->objectAddPresentation(kObject1040);
	_app->objectPresentationAddImageToRotation(kObject1040, 0, 10013, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1040, 0, 10013, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1040, 0, 0, 1001);
	_app->objectAdd(kObject1035, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1035, 10013, Common::Rect(1137, -84, 1241, 189), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1035, 10013, Common::Rect(1137, -84, 1241, 189), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1035);
	_app->objectPresentationAddImageToRotation(kObject1035, 0, 10013, 3);
	_app->objectPresentationAddAnimationToPuzzle(kObject1035, 0, kPuzzle1030, "s00a01n03p04s01", kImageTypeBMP, Common::Point(175, 45), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1035, 0, 0, 1001);
	_app->objectAdd(kObject1041, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1041, 10013, Common::Rect(1715, -57, 1771, 105), false, kCursorAction, 0);
	_app->objectAddPresentation(kObject1041);
	_app->objectPresentationAddImageToRotation(kObject1041, 0, 10013, 4);
	_app->objectPresentationAddAnimationToPuzzle(kObject1041, 0, kPuzzle1033, "s00a01n03p07s01", kImageTypeBMP, Common::Point(221, 77), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1041, 0, 0, 1001);
	_app->objectAdd(kObjectCommercialAdd, "Commercial Add", "al_add", 1);
	_app->objectAddPresentation(kObjectCommercialAdd);
	_app->objectPresentationAddImageToRotation(kObjectCommercialAdd, 0, 10013, 5);
	_app->objectAdd(kObjectBasket, "Basket", "bakset", 1);
	_app->objectAddRotationAccessibility(kObjectBasket, 10013, Common::Rect(3088, 113, 3188, 212), false, kCursorTake, 0);
	_app->objectAddPresentation(kObjectBasket);
	_app->objectPresentationAddImageToRotation(kObjectBasket, 0, 10013, 7);
	_app->objectAddPresentation(kObjectBasket);
	_app->objectPresentationAddImageToRotation(kObjectBasket, 1, 10013, 9);
	_app->objectAdd(kObjectBasket2, "Basket", "bakset", 1);
	_app->objectAddRotationAccessibility(kObjectBasket2, 10013, Common::Rect(3, 66, 93, 152), false, kCursorTake, 0);
	_app->objectAddPresentation(kObjectBasket2);
	_app->objectPresentationAddImageToRotation(kObjectBasket2, 0, 10013, 6);
	_app->objectAddPresentation(kObjectBasket2);
	_app->objectPresentationAddImageToRotation(kObjectBasket2, 1, 10013, 8);
	_app->objectAdd(kObjectFlask, "Flask", "flask", 1);
	_app->objectAdd(kObjectPerfumedOil, "Perfumed Oil", "ed_oil", 1);
	_app->objectAdd(kObject1044, "", "", 1);
	_app->objectAddPuzzleAccessibility(kObject1044, kPuzzle1036, Common::Rect(452, 128, 548, 162), false, kCursorActionObject, 0);
	_app->objectAddPuzzleAccessibility(kObject1044, kPuzzle1036, Common::Rect(  0, 420, 640, 464), false, kCursorBack, 1);
	_app->objectAdd(kObject10441, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10441, 10013, Common::Rect(3150, -94, 3290, 1), true, kCursorAction, 0);
	_app->objectAdd(kObject1045, "", "", 1);
	_app->objectAddPresentation(kObject1045);
	_app->objectPresentationAddAnimationToPuzzle(kObject1045, 0, kPuzzle1035, "s00a01n03p12s01", kImageTypeBMP, Common::Point(166, 35), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1045, 0, 0, 1001);
	_app->objectAdd(kObjectRose, "Rose", "rose", 1);
	_app->objectAddRotationAccessibility(kObjectRose, 10014, Common::Rect(2831,  -14, 3011, 127), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObjectRose, 10014, Common::Rect(2747, -136, 2974, -40), false, kCursorAction, 1);
	_app->objectAddRotationAccessibility(kObjectRose, 10014, Common::Rect(   1, -173,   91,  -3), false, kCursorAction, 2);
	_app->objectAddPuzzleAccessibility(kObjectRose, kPuzzle1037, Common::Rect(0, 420, 640, 464), false, kCursorBack, 3);
	_app->objectAddPuzzleAccessibility(kObjectRose, kPuzzle1038, Common::Rect(0, 420, 640, 464), false, kCursorBack, 4);
	_app->objectAdd(kObjectDoveFeather, "Dove¦s Feather", "eather", 1);
	_app->objectAddRotationAccessibility(kObjectDoveFeather, 10014, Common::Rect(2782, 273, 2875, 324), false, kCursorTake, 0);
	_app->objectAddPresentation(kObjectDoveFeather);
	_app->objectPresentationAddImageToRotation(kObjectDoveFeather, 0, 10014, 0);
	_app->objectAdd(kObject1029, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1029, 10022, Common::Rect(2042, -233, 2339, 175), false, kCursorAction, 0);
	_app->objectAdd(kObject1028, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1028, 10022, Common::Rect(2151, 8, 2218, 213), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1028, 10022, Common::Rect(2151, 8, 2218, 213), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1028);
	_app->objectPresentationAddImageToRotation(kObject1028, 0, 10022, 2);
	_app->objectPresentationAddAnimationToRotation(kObject1028, 0, 10022, 3, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1028, 0, 0, 1001);
	_app->objectAddPresentation(kObject1028);
	_app->objectPresentationAddAnimationToPuzzle(kObject1028, 1, kPuzzle1007, "s00a02n02p04s01", kImageTypeBMP, Common::Point(471, 153), kDrawTypeNormal, 1000, 28, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1028, 1, 0, 1001);
	_app->objectAdd(kObject1031, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1031, 10015, Common::Rect(1283, 56, 1416, 269), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1031, 10015, Common::Rect(1283, 56, 1416, 269), false, kCursorActionObject, 1);
	_app->objectAddPresentation(kObject1031);
	_app->objectPresentationAddImageToRotation(kObject1031, 0, 10015, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1031, 0, 10015, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(1031, 0, 0, 1001);
	_app->objectAddPresentation(kObject1031);
	_app->objectPresentationAddAnimationToPuzzle(kObject1031, 1, kPuzzle1039, "s00a01n05p01s01", kImageTypeBMP, Common::Point(384, 177), kDrawTypeNormal, 1000, 29, 12.5f, 16);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1031, 1, 0, 1001);
	_app->objectAdd(kObject1033, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1033, 10018, Common::Rect(1272, -15, 1346, 182), false, kCursorAction, 0);
	_app->objectAddPresentation(kObject1033);
	_app->objectPresentationAddImageToRotation(kObject1033, 0, 10018, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1033, 0, 10018, 1, 14, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1033, 0, 0, 1001);
	_app->objectAddPresentation(kObject1033);
	_app->objectPresentationAddAnimationToPuzzle(kObject1033, 1, kPuzzle1020, "s00a01n08p01s01", kImageTypeBMP, Common::Point(140, 67), kDrawTypeNormal, 1000, 29, 12.5f, 16);
	_app->objectPresentationSetAnimationOnPuzzle(1033, 1, 0, 1001);
	_app->objectAdd(kObject1046, "", "", 1);
	_app->objectAddPresentation(kObject1046);
	_app->objectPresentationAddImageToRotation(kObject1046, 0, 100114, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1046, 0, 100114, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1046, 0, 0, 1001);
	_app->objectAddPresentation(kObject1046);
	_app->objectPresentationAddAnimationToPuzzle(kObject1046, 1, kPuzzle1064, "s00a01n14p01s01", kImageTypeBMP, Common::Point(414, 119), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1046, 1, 0, 1001);
	_app->objectAdd(kObject1048, "", "", 1);
	_app->objectAddPresentation(kObject1048);
	_app->objectPresentationAddImageToRotation(kObject1048, 0, 10012, 6);
	_app->objectAdd(kObject1050, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1050, 10013, Common::Rect(1137, -84, 1241, 189), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1050, 10013, Common::Rect(1137, -84, 1241, 189), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1050);
	_app->objectPresentationAddImageToRotation(kObject1050, 0, 10013, 3);
	_app->objectPresentationAddAnimationToPuzzle(kObject1050, 0, kPuzzle1030, "s00a01n03p04s01", kImageTypeBMP, Common::Point(175, 45), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1050, 0, 0, 1001);
	_app->objectAdd(kObject1051, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1051, 10013, Common::Rect(1715, -57, 1771, 105), false, kCursorAction, 0);
	_app->objectAddPresentation(kObject1051);
	_app->objectPresentationAddImageToRotation(kObject1051, 0, 10013, 4);
	_app->objectPresentationAddAnimationToPuzzle(kObject1051, 0, kPuzzle1033, "s00a01n03p07s01", kImageTypeBMP, Common::Point(221, 77), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1051, 0, 0, 1001);
	_app->objectAdd(kObject1052, "", "", 1);
	_app->objectAddPresentation(kObject1052);
	_app->objectPresentationAddImageToRotation(kObject1052, 0, 10013, 11);
	_app->objectAdd(kObject1053, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1053, 10013, Common::Rect(3382, -47, 3434, 40), false, kCursorAction, 0);
	_app->objectAddPresentation(kObject1053);
	_app->objectPresentationAddImageToRotation(kObject1053, 0, 10013, 2);
	_app->objectPresentationAddAnimationToPuzzle(kObject1053, 0, kPuzzle1027, "s00a01n03p01s01", kImageTypeBMP, Common::Point(161, 224), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(1053, 0, 0, 1001);
	_app->objectAddPresentation(kObject1053);
	_app->objectPresentationAddAnimationToPuzzle(kObject1053, 1, kPuzzle1027, "s00a01n03p01s02", kImageTypeBMP, Common::Point(366, 204), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1053, 1, 0, 1002);
	_app->objectAdd(kObject1054, "", "", 1);
	_app->objectAddPresentation(kObject1054);
	_app->objectPresentationAddAnimationToPuzzle(kObject1054, 0, kPuzzle1044, "s00a01n03p11s01", kImageTypeBMP, Common::Point(252, 98), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1054, 0, 0, 1001);
	_app->objectAddPresentation(kObject1054);
	_app->objectPresentationAddAnimationToPuzzle(kObject1054, 1, kPuzzle1044, "s00a01n03p11s02", kImageTypeBMP, Common::Point(392, 81), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1054, 1, 0, 1002);
	_app->objectAdd(kObject1055, "", "", 1);
	_app->objectAddPresentation(kObject1055);
	_app->objectPresentationAddImageToRotation(kObject1055, 0, 10013, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1055, 0, 10013, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1055, 0, 0, 1001);
	_app->objectAdd(kObject1049, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1049, 10013, Common::Rect(543, -28, 733, 410), false, kCursorAction, 0);
	_app->objectAddPresentation(kObject1049);
	_app->objectPresentationAddImageToRotation(kObject1049, 0, 10013, 13);
	_app->objectPresentationAddAnimationToPuzzle(kObject1049, 0, kPuzzle1040, "s00a01n03p10s01", kImageTypeBMP, Common::Point(206, 43), kDrawTypeNormal, 1000, 28, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1049, 0, 0, 1001);
	_app->objectAdd(kObjectRoses, "Roses", "rose", 1);
	_app->objectAdd(kObject1057, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1057, 10043, Common::Rect(1713, 29, 1756, 106), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1057, 10043, Common::Rect(1713, 29, 1756, 106), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1057);
	_app->objectPresentationAddImageToRotation(kObject1057, 0, 10043, 3);
	_app->objectPresentationAddAnimationToRotation(kObject1057, 0, 10043, 5, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1057, 0, 0, 1001);
	_app->objectAddPresentation(kObject1057);
	_app->objectPresentationAddAnimationToPuzzle(kObject1057, 1, kPuzzle1045, "s00a04n03p05s01", kImageTypeBMP, Common::Point(379, 60), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1057, 1, 0, 1001);
	_app->objectAddPresentation(kObject1057);
	_app->objectPresentationAddAnimationToPuzzle(kObject1057, 2, kPuzzle1045, "s00a04n03p05s02", kImageTypeBMP, Common::Point(138, 17), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1057, 2, 0, 1002);
	_app->objectAdd(kObjectPhiltre1, "Philtre1", "hiltre", 1);
	_app->objectAdd(kObjectPhiltre2, "Philtre2", "hiltre", 1);
	_app->objectAdd(kObject1059, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1059, 10046, Common::Rect(657, -8, 775, 410), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1059, 10046, Common::Rect(657, -8, 775, 410), false, kCursorIdle, 1);
	_app->objectAddRotationAccessibility(kObject1059, 10046, Common::Rect(657, -8, 775, 410), false, kCursorIdle, 2);
	_app->objectAddPresentation(kObject1059);
	_app->objectPresentationAddImageToRotation(kObject1059, 0, 10046, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1059, 0, 10046, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1059, 0, 0, 1001);
	_app->objectAddPresentation(kObject1059);
	_app->objectPresentationAddAnimationToPuzzle(kObject1059, 1, kPuzzle1046, "s00a04n06p01s01", kImageTypeBMP, Common::Point(388, 59), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1059, 1, 0, 1001);
	_app->objectAddPresentation(kObject1059);
	_app->objectPresentationAddAnimationToPuzzle(kObject1059, 2, kPuzzle1046, "s00a04n06p01s02", kImageTypeBMP, Common::Point(176, 35), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1059, 2, 0, 1002);
	_app->objectAdd(kObject10591, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10591, 100413, Common::Rect(687, -31, 769, 164), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject10591, 100413, Common::Rect(687, -31, 769, 164), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject10591);
	_app->objectPresentationAddImageToRotation(kObject10591, 0, 100413, 0);
	_app->objectPresentationAddAnimationToRotation(kObject10591, 0, 100413, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject10591, 0, 0, 1001);
	_app->objectAddPresentation(kObject10591);
	_app->objectPresentationAddAnimationToPuzzle(kObject10591, 1, kPuzzle1048, "s00a04n13p01s01", kImageTypeBMP, Common::Point(136, 19), kDrawTypeNormal, 1000, 29, 12.5f, 16);
	_app->objectPresentationSetAnimationOnPuzzle(kObject10591, 1, 0, 1001);
	_app->objectAdd(kObject1062, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1062, 10043, Common::Rect(333, -1, 428, 240), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1062, 10043, Common::Rect(333, -1, 428, 240), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1062);
	_app->objectPresentationAddImageToRotation(kObject1062, 0, 10043, 2);
	_app->objectPresentationAddAnimationToRotation(kObject1062, 0, 10043, 4, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1062, 0, 0, 1001);
	_app->objectAddPresentation(kObject1062);
	_app->objectPresentationAddAnimationToPuzzle(kObject1062, 1, kPuzzle1051, "s00a04n03p03s01", kImageTypeBMP, Common::Point(45, 50), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1062, 1, 0, 1001);
	_app->objectAdd(kObject1061, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1061, 10043, Common::Rect(527, 33, 587, 217), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1061, 10043, Common::Rect(527, 33, 587, 217), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1061);
	_app->objectPresentationAddImageToRotation(kObject1061, 0, 10043, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1061, 0, 10043, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1061, 0, 0, 1002);
	_app->objectAddPresentation(kObject1061);
	_app->objectPresentationAddImageToRotation(kObject1061, 1, 10046, 2);
	_app->objectPresentationAddAnimationToRotation(kObject1061, 1, 10046, 3, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1061, 1, 0, 1002);
	_app->objectAddPresentation(kObject1061);
	_app->objectPresentationAddAnimationToPuzzle(kObject1061, 2, kPuzzle1049, "s00a04n03p01s01", kImageTypeBMP, Common::Point(176, 57), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1061, 2, 0, 1001);
	_app->objectAddPresentation(kObject1061);
	_app->objectPresentationAddAnimationToPuzzle(kObject1061, 3, kPuzzle1049, "s00a04n03p01s02", kImageTypeBMP, Common::Point(485, 107), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1061, 3, 0, 1002);
	_app->objectAdd(kObject1064, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1064, 10012, Common::Rect(1364, -31, 1582, 350), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1064, 10012, Common::Rect(1364, -31, 1582, 350), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1064);
	_app->objectPresentationAddImageToRotation(kObject1064, 0, 10012, 3);
	_app->objectPresentationAddAnimationToPuzzle(kObject1064, 0, kPuzzle1002, "s00a01n02p05s01", kImageTypeBMP, Common::Point(171, 26), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1064, 0, 0, 1001);
	_app->objectAddPresentation(kObject1064);
	_app->objectPresentationAddAnimationToPuzzle(kObject1064, 1, kPuzzle1002, "s00a01n02p05s02", kImageTypeBMP, Common::Point(315, 24), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1064, 1, 0, 1002);
	_app->objectAdd(kObject1066, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1066, 10013, Common::Rect(3382, -47, 3434, 40), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1066, 10013, Common::Rect(3382, -47, 3434, 40), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1066);
	_app->objectPresentationAddImageToRotation(kObject1066, 0, 10013, 2);
	_app->objectPresentationAddAnimationToPuzzle(kObject1066, 0, kPuzzle1027, "s00a01n03p01s01", kImageTypeBMP, Common::Point(161, 224), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1066, 0, 0, 1001);
	_app->objectAddPresentation(kObject1066);
	_app->objectPresentationAddAnimationToPuzzle(kObject1066, 1, kPuzzle1027, "s00a01n03p01s02", kImageTypeBMP, Common::Point(366, 204), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1066, 1, 0, 1002);
	_app->objectAdd(kObject1067, "", "", 1);
	_app->objectAddPresentation(kObject1067);
	_app->objectPresentationAddImageToRotation(kObject1067, 0, 10013, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1067, 0, 10013, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1067, 0, 0, 1001);
	_app->objectAdd(kObject1068, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1068, 10053, Common::Rect(1165, 77, 1341, 345), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1068, 10053, Common::Rect(1165, 77, 1341, 345), false, kCursorIdle, 1);
	_app->objectAddRotationAccessibility(kObject1068, 10053, Common::Rect(1165, 77, 1341, 345), false, kCursorActionObject, 2);
	_app->objectAddPresentation(kObject1068);
	_app->objectPresentationAddImageToRotation(kObject1068, 0, 10053, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1068, 0, 10053, 5, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1068, 0, 0, 1001);
	_app->objectAddPresentation(kObject1068);
	_app->objectPresentationAddAnimationToPuzzle(kObject1068, 1, kPuzzle1053, "s00a05n03p01s01", kImageTypeBMP, Common::Point(204, 91), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1068, 1, 0, 1001);
	_app->objectAddPresentation(kObject1068);
	_app->objectPresentationAddAnimationToPuzzle(kObject1068, 2, kPuzzle1053, "s00a05n03p01s02", kImageTypeBMP, Common::Point(321, 16), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1068, 2, 0, 1002);
	_app->objectAdd(kObjectStone, "Stone", "stone", 9);
	_app->objectAddRotationAccessibility(kObjectStone, 10053, Common::Rect(1436, 71, 1546, 185), false, kCursorTake, 0);
	_app->objectAdd(kObjectBranch, "Branch", "branch", 9);
	_app->objectAddRotationAccessibility(kObjectBranch, 10053, Common::Rect(2453, 411, 2610, 466), false, kCursorTake, 0);
	_app->objectAddRotationAccessibility(kObjectBranch, 10053, Common::Rect(1436,  71, 1546, 185), false, kCursorActionObject, 1);
	_app->objectAddPresentation(kObjectBranch);
	_app->objectPresentationAddImageToRotation(kObjectBranch, 0, 10053, 4);
	_app->objectAdd(kObject1071, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1071, 10053, Common::Rect(2197, -166, 2425, -120), false, kCursorActionObject, 0);
	_app->objectAdd(kObject1072, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1072, 10053, Common::Rect(2351, -658, 2466, -345), false, kCursorActionObject, 0);
	_app->objectAdd(kObject1073, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1073, 10053, Common::Rect(2492, -248, 2865, -126), false, kCursorActionObject, 0);
	_app->objectAdd(kObject1075, "", "", 1);
	_app->objectAddPresentation(kObject1075);
	_app->objectPresentationAddAnimationToRotation(kObject1075, 0, 10053, 1, 14, 12.5f, 5);
	_app->objectPresentationSetAnimationOnRotation(kObject1075, 0, 0, 1000);
	_app->objectAdd(kObject1076, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1076, 10053, Common::Rect(1713, -78, 2411, 297), false, kCursorActionObject, 0);
	_app->objectAdd(kObject1077, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1077, 10052, Common::Rect(1212, 7, 1311, 234), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1077, 10052, Common::Rect(1212, 7, 1311, 234), false, kCursorIdle, 1);
	_app->objectAddPresentation(kObject1077);
	_app->objectPresentationAddImageToRotation(kObject1077, 0, 10052, 3);
	_app->objectPresentationAddAnimationToRotation(kObject1077, 0, 10052, 5, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1077, 0, 0, 1001);
	_app->objectAddPresentation(kObject1077);
	_app->objectPresentationAddAnimationToPuzzle(kObject1077, 1, kPuzzle1056, "s00a05n02p02s01", kImageTypeBMP, Common::Point(101, 59), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1077, 1, 0, 1001);
	_app->objectAddPresentation(kObject1077);
	_app->objectPresentationAddAnimationToPuzzle(kObject1077, 2, kPuzzle1056, "s00a05n02p02s02", kImageTypeBMP, Common::Point(396, 63), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1077, 2, 0, 1002);
	_app->objectAdd(kObject1079, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1079, 10052, Common::Rect(258, 14, 776, 413), false, kCursorAction, 0);
	_app->objectAddPresentation(kObject1079);
	_app->objectPresentationAddImageToRotation(kObject1079, 0, 10052, 0);
	_app->objectPresentationAddImageToRotation(kObject1079, 0, 100516, 0);
	_app->objectPresentationAddImageToRotation(kObject1079, 0, 100517, 0);
	_app->objectPresentationAddImageToRotation(kObject1079, 0, 100518, 0);
	_app->objectPresentationAddImageToRotation(kObject1079, 0, 100519, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1079, 0, 10052, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1079, 0, 0, 1001);
	_app->objectAdd(kObject1078, "", "", 1);
	_app->objectAddPresentation(kObject1078);
	_app->objectPresentationAddImageToRotation(kObject1078, 0, 10052, 1);
	_app->objectPresentationAddAnimationToRotation(kObject1078, 0, 10052, 4, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1078, 0, 0, 1002);
	_app->objectAdd(kObject1080, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1080, 1001102, Common::Rect(3062, -64, 3091, -21), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObject1080);
	_app->objectPresentationAddImageToRotation(kObject1080, 0, 1001102, 0);
	_app->objectAddPresentation(kObject1080);
	_app->objectPresentationAddImageToRotation(kObject1080, 1, 1001202, 3);
	_app->objectAddPresentation(kObject1080);
	_app->objectPresentationAddImageToRotation(kObject1080, 2, 1001402, 0);
	_app->objectAddPresentation(kObject1080);
	_app->objectPresentationAddImageToRotation(kObject1080, 3, 1001502, 0);
	_app->objectAddPresentation(kObject1080);
	_app->objectPresentationAddImageToRotation(kObject1080, 4, 1001602, 0);
	_app->objectAddPresentation(kObject1080);
	_app->objectPresentationAddImageToRotation(kObject1080, 5, 1001702, 0);
	_app->objectAddPresentation(kObject1080);
	_app->objectPresentationAddImageToRotation(kObject1080, 6, 1001902, 0);
	_app->objectAdd(kObjectStone2, "Stone", "stone", 1);
	_app->objectAddRotationAccessibility(kObjectStone2, 10011002, Common::Rect(2564, 599, 2724, 674), false, kCursorTake, 0);
	_app->objectAddPresentation(kObjectStone2);
	_app->objectPresentationAddImageToRotation(kObjectStone2, 0, 10011002, 0);
	_app->objectAdd(kObject1081, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1081, 1001202, Common::Rect(945, -47, 1008, 108), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1081, 1001202, Common::Rect(945, -47, 1008, 108), false, kCursorIdle, 1);
	_app->objectAddRotationAccessibility(kObject1081, 1001202, Common::Rect(945, -47, 1008, 108), false, kCursorActionObject, 2);
	_app->objectAddPresentation(kObject1081);
	_app->objectPresentationAddImageToRotation(kObject1081, 0, 1001202, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1081, 0, 1001202, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1081, 0, 0, 1001);
	_app->objectAddPresentation(kObject1081);
	_app->objectPresentationAddImageToRotation(kObject1081, 1, 1001202, 2);
	_app->objectAddPresentation(kObject1081);
	_app->objectPresentationAddAnimationToPuzzle(kObject1081, 2, kPuzzle1060, "s00a01n02p06s01", kImageTypeBMP, Common::Point(436, 49), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1081, 2, 0, 1001);
	_app->objectAdd(kObject1084, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1084, 1005402, Common::Rect(2881, -71, 2974, 131), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObject1084);
	_app->objectPresentationAddImageToRotation(kObject1084, 0, 1005402, 0);
	_app->objectPresentationAddAnimationToRotation(kObject1084, 0, 1005402, 1, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1084, 0, 0, 1001);
	_app->objectAdd(kObject1085, "", "", 1);
	_app->objectAddPresentation(kObject1085);
	_app->objectPresentationAddImageToRotation(kObject1085, 0, 1005402, 2);
	_app->objectPresentationAddAnimationToRotation(kObject1085, 0, 1005402, 3, 15, 12.5f, 4);
	_app->objectPresentationSetAnimationOnRotation(kObject1085, 0, 0, 1001);
	_app->objectAdd(kObject1089, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1089, 1001103, Common::Rect(112, -45, 265, 378), false, kCursorActionObject, 0);
	_app->objectAddPresentation(kObject1089);
	_app->objectPresentationAddImageToRotation(kObject1089, 0, 1001103, 0);
	_app->objectPresentationAddImageToRotation(kObject1089, 0, 10011303, 0);
	_app->objectPresentationAddImageToRotation(kObject1089, 0, 10011203, 0);
	_app->objectPresentationAddImageToRotation(kObject1089, 0, 10011303, 1);
	_app->objectPresentationAddImageToRotation(kObject1089, 0, 10011203, 1);
	_app->objectAdd(kObject1093, "", "", 1);
	_app->objectAddRotationAccessibility(kObject1093, 10011103, Common::Rect(1300, -82, 1397, 112), false, kCursorAction, 0);
	_app->objectAddRotationAccessibility(kObject1093, 10011103, Common::Rect(1300, -82, 1397, 112), false, kCursorAction, 1);
	_app->objectAddRotationAccessibility(kObject1093, 10011103, Common::Rect(1300, -82, 1397, 112), false, kCursorActionObject, 2);
	_app->objectAddPresentation(kObject1093);
	_app->objectPresentationAddAnimationToPuzzle(kObject1093, 0, kPuzzle1063, "s00a01n11p01s01", kImageTypeBMP, Common::Point(208, 22), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1093, 0, 0, 1001);
	_app->objectAddPresentation(kObject1093);
	_app->objectPresentationAddAnimationToPuzzle(kObject1093, 1, kPuzzle1063, "s00a01n11p01s02", kImageTypeBMP, Common::Point(348, 101), kDrawTypeNormal, 1000, 29, 12.5f, 4);
	_app->objectPresentationSetAnimationOnPuzzle(kObject1093, 1, 0, 1002);
	_app->objectAdd(kObject10041, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10041, 10011, Common::Rect(42, -311, 402, 296), true, kCursorMove, 0);
	_app->objectAdd(kObject10042, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10042, 10018, Common::Rect(1202, -262, 1385, 178), true, kCursorMove, 0);
	_app->objectAdd(kObject10043, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10043, 100110, Common::Rect(3141, -257, 3533, 343), true, kCursorMove, 0);
	_app->objectAdd(kObject10044, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10044, 10023, Common::Rect(1316, -182, 1518, 92), true, kCursorMove, 0);
	_app->objectAdd(kObject10045, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10045, 10026, Common::Rect(3171, -308, 3529, 257), true, kCursorMove, 0);
	_app->objectAdd(kObject10046, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10046, 10015, Common::Rect(1125, -127, 1601, 155), true, kCursorMove, 0);
	_app->objectAdd(kObject10047, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10047, 10024, Common::Rect(3058, -196, 3306, 131), true, kCursorMove, 0);
	_app->objectAdd(kObject10048, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10048, 10031, Common::Rect(1030, -203, 1334, 213), true, kCursorMove, 0);
	_app->objectAdd(kObject10049, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10049, 10042, Common::Rect(1981, -175, 2181, 183), true, kCursorMove, 0);
	_app->objectAdd(kObject10050, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10050, 10043, Common::Rect(1378, -113, 1655, 155), true, kCursorMove, 0);
	_app->objectAdd(kObject10051, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10051, 100412, Common::Rect(73, -206, 469, 227), true, kCursorMove, 0);
	_app->objectAdd(kObject10052, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10052, 10044, Common::Rect(840, -73, 1107, 215), true, kCursorMove, 0);
	_app->objectAddRotationAccessibility(kObject10052, 10045, Common::Rect(3411, -187, 3600, 215), true, kCursorMove, 0);
	_app->objectAddRotationAccessibility(kObject10052, 10045, Common::Rect(0, -187, 219, 215), true, kCursorMove, 0);
	_app->objectAdd(kObject10053, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10053, 10016, Common::Rect(1381, -131, 1705, 119), true, kCursorMove, 0);
	_app->objectAdd(kObject10054, "", "", 1);
	_app->objectAddRotationAccessibility(kObject10054, 10052, Common::Rect(131, -280, 537, 278), true, kCursorMove, 0);
	_app->soundAdd(1153, kSoundTypeAmbientEffect, "00_DON-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1154, kSoundTypeAmbientEffect, "00_DON-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1157, kSoundTypeAmbientEffect, "00_TAK-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1158, kSoundTypeAmbientEffect, "00_TAK-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1159, kSoundTypeAmbientEffect, "00_TAK-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1161, kSoundTypeAmbientEffect, "00_TAK-4.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1162, kSoundTypeAmbientEffect, "00_PUT-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1163, kSoundTypeAmbientEffect, "00_PUT-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1164, kSoundTypeAmbientEffect, "00_BUS-3.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1165, kSoundTypeAmbientEffect, "00_BUS-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1166, kSoundTypeAmbientEffect, "00_BUS-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1167, kSoundTypeAmbientEffect, "00_AMB-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1168, kSoundTypeAmbientEffect, "00_HOO-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1169, kSoundTypeAmbientEffect, "00_HOO-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1170, kSoundTypeAmbientEffect, "00_TAK-3.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1171, kSoundTypeAmbientEffect, "00_THR-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1172, kSoundTypeAmbientEffect, "00_THR-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1173, kSoundTypeAmbientEffect, "00_TAK-6.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1174, kSoundTypeAmbientEffect, "00_MOV-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1175, kSoundTypeAmbientEffect, "00_MOV-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1176, kSoundTypeAmbientEffect, "00_MOV-3.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1177, kSoundTypeAmbientEffect, "00_HIS-3.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1178, kSoundTypeAmbientEffect, "00_HIS-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1179, kSoundTypeAmbientEffect, "00_HIS-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1180, kSoundTypeAmbientEffect, "00_POK-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1181, kSoundTypeAmbientEffect, "00_POK-3.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1182, kSoundTypeAmbientEffect, "00_POK-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1183, kSoundTypeAmbientEffect, "01_BOD-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1184, kSoundTypeAmbientEffect, "01_BOD-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1185, kSoundTypeAmbientEffect, "01_PUT-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1186, kSoundTypeAmbientEffect, "00_CRI-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1187, kSoundTypeAmbientEffect, "00_BIR-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1188, kSoundTypeAmbientEffect, "00_BIR-2.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundAdd(1190, kSoundTypeAmbientEffect, "00_ROC-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->soundSetVolume(1190, 95);
	_app->soundAdd(1025,  kSoundTypeDialog,  "1025.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1026,  kSoundTypeDialog,  "1026.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1027,  kSoundTypeDialog,  "1027.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1028,  kSoundTypeDialog,  "1028.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1029,  kSoundTypeDialog,  "1029.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1030,  kSoundTypeDialog,  "1030.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1031,  kSoundTypeDialog,  "1031.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1032,  kSoundTypeDialog,  "1032.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1033,  kSoundTypeDialog,  "1033.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1034,  kSoundTypeDialog,  "1034.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1035,  kSoundTypeDialog,  "1035.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1036,  kSoundTypeDialog,  "1036.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1037,  kSoundTypeDialog,  "1037.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1038,  kSoundTypeDialog,  "1038.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1039,  kSoundTypeDialog,  "1039.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1040,  kSoundTypeDialog,  "1040.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1041,  kSoundTypeDialog,  "1041.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1042,  kSoundTypeDialog,  "1042.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1043,  kSoundTypeDialog,  "1043.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1046,  kSoundTypeDialog,  "1046.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1047,  kSoundTypeDialog,  "1047.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1048,  kSoundTypeDialog,  "1048.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1064,  kSoundTypeDialog,  "1064.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1065,  kSoundTypeDialog,  "1065.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1066,  kSoundTypeDialog,  "1066.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1067,  kSoundTypeDialog,  "1067.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2001,  kSoundTypeDialog,  "2001.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2002,  kSoundTypeDialog,  "2002.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2003,  kSoundTypeDialog,  "2003.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2004,  kSoundTypeDialog,  "2004.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2005,  kSoundTypeDialog,  "2005.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2006,  kSoundTypeDialog,  "2006.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2007,  kSoundTypeDialog,  "2007.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2008,  kSoundTypeDialog,  "2008.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(20061, kSoundTypeDialog,  "2006.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(20071, kSoundTypeDialog,  "2007.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(20081, kSoundTypeDialog,  "2008.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2009,  kSoundTypeDialog,  "2009.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(20091, kSoundTypeDialog, "20091.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(20092, kSoundTypeDialog, "20092.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2010,  kSoundTypeDialog,  "2010.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2011,  kSoundTypeDialog,  "2011.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2012,  kSoundTypeDialog,  "2012.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(20121, kSoundTypeDialog,  "2012.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2013,  kSoundTypeDialog,  "2013.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2014,  kSoundTypeDialog,  "2014.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2015,  kSoundTypeDialog,  "2015.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2016,  kSoundTypeDialog,  "2016.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(20141, kSoundTypeDialog,  "2014.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(20151, kSoundTypeDialog,  "2015.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(20161, kSoundTypeDialog,  "2016.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2017,  kSoundTypeDialog,  "2017.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2018,  kSoundTypeDialog,  "2018.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2019,  kSoundTypeDialog,  "2019.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2020,  kSoundTypeDialog,  "2020.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2021,  kSoundTypeDialog,  "2021.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2022,  kSoundTypeDialog,  "2022.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2028,  kSoundTypeDialog,  "2028.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2032,  kSoundTypeDialog,  "2032.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2033,  kSoundTypeDialog,  "2033.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2034,  kSoundTypeDialog,  "2034.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2035,  kSoundTypeDialog,  "2035.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2036,  kSoundTypeDialog,  "2036.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2037,  kSoundTypeDialog,  "2037.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2038,  kSoundTypeDialog,  "2038.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2039,  kSoundTypeDialog,  "2039.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2040,  kSoundTypeDialog,  "2040.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2041,  kSoundTypeDialog,  "2041.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2042,  kSoundTypeDialog,  "2042.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2043,  kSoundTypeDialog,  "2043.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2044,  kSoundTypeDialog,  "2044.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2045,  kSoundTypeDialog,  "2045.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2046,  kSoundTypeDialog,  "2046.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2047,  kSoundTypeDialog,  "2047.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2048,  kSoundTypeDialog,  "2048.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2049,  kSoundTypeDialog,  "2049.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2050,  kSoundTypeDialog,  "2050.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2051,  kSoundTypeDialog,  "2051.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2052,  kSoundTypeDialog,  "2052.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2053,  kSoundTypeDialog,  "2053.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2054,  kSoundTypeDialog,  "2054.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2055,  kSoundTypeDialog,  "2055.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2063,  kSoundTypeDialog,  "2063.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2064,  kSoundTypeDialog,  "2064.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2065,  kSoundTypeDialog,  "2065.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2066,  kSoundTypeDialog,  "2066.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2067,  kSoundTypeDialog,  "2067.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(2076,  kSoundTypeDialog,  "2076.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3002,  kSoundTypeDialog,  "3002.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3003,  kSoundTypeDialog,  "3003.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3004,  kSoundTypeDialog,  "3004.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3005,  kSoundTypeDialog,  "3005.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3006,  kSoundTypeDialog,  "3006.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3007,  kSoundTypeDialog,  "3007.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3008,  kSoundTypeDialog,  "3008.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3009,  kSoundTypeDialog,  "3009.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3010,  kSoundTypeDialog,  "3010.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3011,  kSoundTypeDialog,  "3011.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3012,  kSoundTypeDialog,  "3012.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3013,  kSoundTypeDialog,  "3013.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3014,  kSoundTypeDialog,  "3014.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3015,  kSoundTypeDialog,  "3015.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3021,  kSoundTypeDialog,  "3021.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3022,  kSoundTypeDialog,  "3022.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3023,  kSoundTypeDialog,  "3023.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3024,  kSoundTypeDialog,  "3024.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3025,  kSoundTypeDialog,  "3025.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3026,  kSoundTypeDialog,  "3026.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3027,  kSoundTypeDialog,  "3027.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3028,  kSoundTypeDialog,  "3028.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3029,  kSoundTypeDialog,  "3029.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3030,  kSoundTypeDialog,  "3030.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3031,  kSoundTypeDialog,  "3031.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3032,  kSoundTypeDialog,  "3032.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3033,  kSoundTypeDialog,  "3033.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3034,  kSoundTypeDialog,  "3034.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3035,  kSoundTypeDialog,  "3035.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3036,  kSoundTypeDialog,  "3036.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3037,  kSoundTypeDialog,  "3037.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3038,  kSoundTypeDialog,  "3038.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3039,  kSoundTypeDialog,  "3039.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3040,  kSoundTypeDialog,  "3040.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3041,  kSoundTypeDialog,  "3041.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3042,  kSoundTypeDialog,  "3042.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3043,  kSoundTypeDialog,  "3043.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3044,  kSoundTypeDialog,  "3044.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3045,  kSoundTypeDialog,  "3045.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3046,  kSoundTypeDialog,  "3046.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(3047,  kSoundTypeDialog,  "3047.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4008,  kSoundTypeDialog,  "4008.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4009,  kSoundTypeDialog,  "4009.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4010,  kSoundTypeDialog,  "4010.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4011,  kSoundTypeDialog,  "4011.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4012,  kSoundTypeDialog,  "4012.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4013,  kSoundTypeDialog,  "4013.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4014,  kSoundTypeDialog,  "4014.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4015,  kSoundTypeDialog,  "4015.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4016,  kSoundTypeDialog,  "4016.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4017,  kSoundTypeDialog,  "4017.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4018,  kSoundTypeDialog,  "4018.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4019,  kSoundTypeDialog,  "4019.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4020,  kSoundTypeDialog,  "4020.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4021,  kSoundTypeDialog,  "4021.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4031,  kSoundTypeDialog,  "4031.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4032,  kSoundTypeDialog,  "4032.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4033,  kSoundTypeDialog,  "4033.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4034,  kSoundTypeDialog,  "4034.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4035,  kSoundTypeDialog,  "4035.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4036,  kSoundTypeDialog,  "4036.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4037,  kSoundTypeDialog,  "4037.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4038,  kSoundTypeDialog,  "4038.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4039,  kSoundTypeDialog,  "4039.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4040,  kSoundTypeDialog,  "4040.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4041,  kSoundTypeDialog,  "4041.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4042,  kSoundTypeDialog,  "4042.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4043,  kSoundTypeDialog,  "4043.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4044,  kSoundTypeDialog,  "4044.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4045,  kSoundTypeDialog,  "4045.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4046,  kSoundTypeDialog,  "4046.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4047,  kSoundTypeDialog,  "4047.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4070,  kSoundTypeDialog,  "4070.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4107,  kSoundTypeDialog,  "4107.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4108,  kSoundTypeDialog,  "4108.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4109,  kSoundTypeDialog,  "4109.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4110,  kSoundTypeDialog,  "4110.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4111,  kSoundTypeDialog,  "4111.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4112,  kSoundTypeDialog,  "4112.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4113,  kSoundTypeDialog,  "4113.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4114,  kSoundTypeDialog,  "4114.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4115,  kSoundTypeDialog,  "4115.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4116,  kSoundTypeDialog,  "4116.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4117,  kSoundTypeDialog,  "4117.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4118,  kSoundTypeDialog,  "4118.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4119,  kSoundTypeDialog,  "4119.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4120,  kSoundTypeDialog,  "4120.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4121,  kSoundTypeDialog,  "4121.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4122,  kSoundTypeDialog,  "4122.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4123,  kSoundTypeDialog,  "4123.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4124,  kSoundTypeDialog,  "4124.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4125,  kSoundTypeDialog,  "4125.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4126,  kSoundTypeDialog,  "4126.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(4127,  kSoundTypeDialog,  "4127.wac", _app->getConfiguration().dialog.loadFrom);
	_app->soundAdd(1191,  kSoundTypeAmbientEffect, "00_RUN-1.WAs", _app->getConfiguration().ambientEffect.loadFrom);
	_app->rotationAdd3DSound(10011002, 1191, 1, 1, 5, 100, 180.0f, 10);
	_app->rotationAdd3DSound(1001902,  1191, 1, 1, 5, 100, 260.0f, 10);
	_app->rotationAdd3DSound(1001702,  1191, 1, 1, 5, 100, 180.0f, 10);
	_app->rotationAdd3DSound(1001602,  1191, 1, 1, 5, 100, 180.0f, 10);
	_app->rotationAdd3DSound(1001502,  1191, 1, 1, 5, 100, 180.0f, 10);
	_app->rotationAdd3DSound(1001402,  1191, 1, 1, 5, 100, 180.0f, 10);
	_app->rotationAdd3DSound(10011402, 1191, 1, 1, 5, 100, 180.0f, 10);
	_app->rotationAdd3DSound(1001202,  1191, 1, 1, 5, 100,  50.0f, 10);
	_app->rotationAdd3DSound(1001102,  1191, 1, 1, 5,  93, 260.0f, 10);
	_app->rotationSet3DSoundOff(10011002, 1191);
	_app->rotationSet3DSoundOff(1001902, 1191);
	_app->rotationSet3DSoundOff(1001702, 1191);
	_app->rotationSet3DSoundOff(1001602, 1191);
	_app->rotationSet3DSoundOff(1001502, 1191);
	_app->rotationSet3DSoundOff(1001402, 1191);
	_app->rotationSet3DSoundOff(10011402, 1191);
	_app->rotationSet3DSoundOff(1001202, 1191);
	_app->rotationSet3DSoundOff(1001102, 1191);
	_app->soundAdd(5001, kSoundTypeAmbientMusic, "BGR_MU-5.WAc", _app->getConfiguration().backgroundMusic.loadFrom);
	_app->soundAdd(5002, kSoundTypeAmbientMusic, "BGR_MU-2.WAc", _app->getConfiguration().backgroundMusic.loadFrom);
	_app->soundAdd(5003, kSoundTypeAmbientMusic, "BGR_MU-3.WAc", _app->getConfiguration().backgroundMusic.loadFrom);
	_app->soundAdd(5004, kSoundTypeAmbientMusic, "BGR_MU-4.WAc", _app->getConfiguration().backgroundMusic.loadFrom);
	_app->soundAdd(5005, kSoundTypeAmbientMusic, "00_AMB-1.WAs", _app->getConfiguration().backgroundMusic.loadFrom);
	_app->rotationAddAmbientSound(1001102, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1001104, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1001202, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1001402, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1001502, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1001602, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1001702, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1001802, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1001902, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10011002, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10011102, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10011202, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10011302, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10011402, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1002502, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100312, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100322, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1005102, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1005202, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1005302, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1005402, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1005502, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1005602, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1005702, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1005802, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(1005902, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10051002, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10051102, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10051202, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10051302, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10051402, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10051502, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10051602, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10051702, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10051802, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10051902, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10052002, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10052102, 5005, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10052202, 5005, 100, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle1060, 5005, 100, 0, 1, 1, 10);
	_app->soundAdd(5000, kSoundTypeAmbientMusic, "BGR_MU-1.WAc", _app->getConfiguration().backgroundMusic.loadFrom);
	_app->rotationAddAmbientSound(10051, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10052, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10053, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10054, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10055, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10056, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10057, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10058, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(10059, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100510, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100511, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100512, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100513, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100514, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100515, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100516, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100517, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100518, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100519, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100520, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100521, 5000, 100, 0, 1, 1, 10);
	_app->rotationAddAmbientSound(100522, 5000, 100, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle1053, 5000, 100, 0, 1, 1, 10);
	_app->puzzleAddAmbientSound(kPuzzle1056, 5000, 100, 0, 1, 1, 10);
	_app->varDefineByte(1000, 0);
	_app->varDefineByte(1001, 0);
	_app->varDefineByte(1002, 0);
	_app->varDefineByte(1003, 0);
	_app->varDefineByte(1004, 0);
	_app->varDefineByte(1005, 0);
	_app->varDefineByte(1006, 0);
	_app->varDefineByte(10071, 0);
	_app->varDefineByte(1054, 0);
	_app->varDefineByte(1055, 0);
	_app->varDefineByte(1056, 0);
	_app->varDefineByte(1057, 0);
	_app->varDefineByte(1058, 0);
	_app->varDefineByte(1008, 0);
	_app->varDefineByte(1009, 0);
	_app->varDefineByte(1010, 0);
	_app->varDefineByte(1011, 0);
	_app->varDefineByte(1012, 0);
	_app->varDefineByte(1013, 0);
	_app->varDefineByte(1014, 0);
	_app->varDefineByte(1015, 0);
	_app->varDefineByte(1016, 0);
	_app->varDefineByte(1017, 0);
	_app->varDefineByte(1018, 0);
	_app->varDefineByte(1019, 0);
	_app->varDefineByte(1020, 0);
	_app->varDefineByte(1021, 0);
	_app->varDefineByte(1022, 0);
	_app->varDefineByte(1023, 0);
	_app->varDefineByte(1024, 0);
	_app->varDefineByte(1025, 0);
	_app->varDefineByte(1026, 0);
	_app->varDefineByte(1027, 0);
	_app->varDefineByte(1028, 0);
	_app->varDefineByte(10281, 0);
	_app->varDefineByte(10191, 0);
	_app->varDefineByte(1059, 0);
	_app->varDefineByte(1060, 0);
	_app->varDefineByte(1061, 0);
	_app->varDefineByte(1062, 0);
	_app->varDefineByte(1063, 0);
	_app->varDefineByte(1064, 0);
	_app->varDefineByte(1065, 0);
	_app->varDefineByte(10651, 0);
	_app->varDefineByte(1066, 0);
	_app->varDefineByte(1029, 0);
	_app->varDefineByte(1031, 0);
	_app->varDefineByte(1032, 0);
	_app->varDefineByte(1033, 0);
	_app->varDefineByte(1034, 0);
	_app->varDefineByte(1030, 0);
	_app->varDefineByte(1035, 0);
	_app->varDefineByte(1036, 0);
	_app->varDefineByte(10361, 0);
	_app->varDefineByte(1037, 0);
	_app->varDefineByte(1038, 0);
	_app->varDefineByte(1067, 0);
	_app->varDefineByte(1068, 0);
	_app->varDefineByte(1069, 0);
	_app->varDefineByte(1070, 0);
	_app->varDefineByte(1071, 0);
	_app->varDefineByte(1072, 0);
	_app->varDefineByte(1073, 0);
	_app->varDefineByte(1040, 0);
	_app->varDefineByte(1041, 0);
	_app->varDefineByte(1042, 0);
	_app->varDefineByte(1043, 0);
	_app->varDefineByte(1044, 0);
	_app->varDefineByte(1045, 0);
	_app->varDefineByte(1046, 0);
	_app->varDefineByte(1047, 0);
	_app->varDefineByte(1048, 0);
	_app->varDefineByte(1049, 0);
	_app->varDefineByte(1050, 0);
	_app->varDefineByte(1051, 0);
	_app->varDefineByte(1052, 0);
	_app->varDefineByte(1053, 0);
	_app->varDefineByte(1074, 0);
	_app->varDefineByte(1075, 0);
	_app->varDefineByte(1076, 0);
	_app->varDefineByte(1077, 0);
	_app->varDefineByte(1078, 0);
	_app->varDefineByte(1099, 0);
	_app->varDefineByte(1098, 0);
	_app->varDefineByte(1097, 0);
}

void Zone1Pompeii::onButtonUp(ObjectId id, Id target, Id /*puzzleRotationId*/, uint32 /*a4*/, const Common::Point &/*point*/) {
	switch (id) {
	default:
		break;

	case kObject1001:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1000)) {
				_app->rotationSetMovabilityOff(10012, 0, 0);
				_app->objectSetAccessibilityOff(kObject1001, 0, 0);
				_app->objectSetAccessibilityOff(kObject1005, 0, 0);
				_app->objectPresentationHide(kObject1006);
				_app->puzzleSetActive(kPuzzle1000);
				_app->soundPlay(1026);
				_app->varSetByte(1000, 1);
			}
		}
		break;

	case kObjectStick:
		if (_app->bagHasClickedObject()) {
			if (target == 1 && _app->bagGetClickedObject() == kObjectRopes && _app->varGetByte(1001) == 1) {
				_app->objectSetAccessibilityOff(kObjectStick, 1, 1);
				_app->objectPresentationShow(kObjectRope, 0);
				_app->objectSetAccessibilityOn(kObjectRope, 0, 0);
				_app->varSetByte(1002, 1);
			}

			_app->setField74(false);
			_app->cursorDelete();
		} else {
			if (!target) {
				_app->objectPresentationShow(kObject1001, 3);
				_app->objectPresentationShow(kObject1001, 4);
				_app->soundPlay(1159);
				_app->bagAdd(kObjectStick);
				_app->objectSetAccessibilityOff(kObjectStick, 0, 0);
			}
		}
		break;

	case kObjectRope:
		if (_app->bagHasClickedObject()) {
			if (_app->bagGetClickedObject() == kObjectSalad && _app->varGetByte(1002) == 1) {
				_app->bagRemove(kObjectSalad);
				_app->objectSetAccessibilityOff(kObjectRope, 0, 0);
				_app->soundPlay(1153 + rnd(2));
				_app->timerStop(kTimer0);
				_app->timerStop(kTimer1);
				_app->timerStop(kTimer2);
				_app->objectSetAccessibilityOff(kObject1006, 0, 1);
				_app->playMovie("s00a0-13");
				_app->puzzleSetActive(kPuzzle1001);
				_app->soundPlay(1030);
			}

			_app->cursorDelete();
		}
		break;

	case kObjectSalad:
		if (_app->bagHasClickedObject()) {
			_app->setField74(false);
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1003)) {
				_app->soundPlay(1157);
				_app->bagAdd(kObjectSalad);
				_app->objectSetAccessibilityOff(kObjectSalad, 0, 0);
				_app->varSetByte(1003, 1);
			}
		}
		break;

	case kObject1005:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1000)) {
				_app->rotationSetMovabilityOff(10012, 0, 0);
				_app->objectSetAccessibilityOff(kObject1005, 0, 0);
				_app->objectSetAccessibilityOff(kObject1001, 0, 0);
				_app->objectPresentationHide(kObject1006);
				_app->puzzleSetActive(kPuzzle1000);
				_app->soundPlay(1026);
				_app->varSetByte(1000, 1);
			}
		}
		break;

	case kObject1006:
		if (_app->bagHasClickedObject()) {
			switch (target) {
			default:
				break;

			case 0:
				if (_app->bagGetClickedObject() == kObjectRope) {
					_app->soundPlay(1153 + rnd(2));
					_app->cursorDelete();
				}

				if (_app->bagGetClickedObject() == kObjectStick) {
					_app->playMovie("S00A0-19");
					_app->exitToMenu(kMenuAction2);
					return;
				}
				break;

			case 1:
				if (_app->bagGetClickedObject() == kObjectSalad) {
					_app->bagRemove(kObjectSalad);
					_app->objectSetAccessibilityOn(kObjectSalad, 0, 0);
					_app->varSetByte(1003, 0);
					_app->soundPlay(1168 + rnd(2));
					_app->cursorDelete();
				}

				if (_app->bagGetClickedObject() == kObjectStick) {
					_app->bagRemove(kObjectStick);
					_app->objectPresentationShow(kObjectStick, 0);
					_app->objectSetAccessibilityOn(kObjectStick, 1, 1);
					_app->varSetByte(1001, 1);
				}
				break;
			}
		} else {
			switch (target) {
			default:
				break;

			case 0:
				if (!_app->soundIsPlaying(1168) && !_app->soundIsPlaying(1169))
					_app->soundPlay(1168 + rnd(2));
				break;

			case 1:
				if (!_app->soundIsPlaying(1153) && !_app->soundIsPlaying(1154))
					_app->soundPlay(1153 + rnd(2));
				break;
			}
		}
		break;

	case kObject1008:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && _app->varGetByte(1004) == 1) {
				_app->soundPlay(900009);
				_app->objectSetAccessibilityOff(kObject1008, 0, 0);
				_app->objectPresentationShow(kObject1009);
				_app->objectSetAccessibilityOn(kObject1009, 0, 0);

				switch (_app->varGetByte(1005)) {
				default:
					break;

				case 0:
					_app->objectSetAccessibilityOff(kObject1009, 0, 0);
					_app->puzzleSetActive(kPuzzle1007);
					_app->soundPlay(1036);
					_app->varSetByte(1005, 1);
					break;

				case 1:
					_app->puzzleSetActive(kPuzzle1007);
					_app->soundPlay(1038);
					_app->varSetByte(1005, 2);
					_app->objectSetAccessibilityOff(kObject1009, 0, 0);
					break;
				}
			}
		}
		break;

	case kObject1009:
	case kObject1019:
	case kObject1041:
	case kObjectFlask:
	case kObject1075:
		if (_app->bagHasClickedObject())
			_app->cursorDelete();
		break;

	case kObject1011:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1006)) {
				_app->objectSetAccessibilityOff(kObject1011, 0, 0);
				_app->playMovie("S00A02-2");
				_app->objectPresentationHide(kObject1010);
				_app->objectPresentationShow(kObject1011);
				_app->puzzleSetActive(kPuzzle1010);
				_app->soundPlay(1039);
				_app->varSetByte(1006, 1);
			}
		}
		break;

	case kObject1012:
		if (_app->bagHasClickedObject()) {
			if (target == 1 && _app->bagGetClickedObject() == kObjectContract) {
				_app->bagRemove(kObjectContract);
				_app->objectSetAccessibilityOff(kObject1012, 1, 1);
				_app->puzzleSetActive(kPuzzle1011);
				_app->soundPlay(1047);
			}

			_app->cursorDelete();
		} else {
			if (!target) {
				_app->objectSetAccessibilityOff(kObject1012, 0, 0);
				_app->varSetByte(90108, 1);
				_app->puzzleSetActive(kPuzzle1011);
				_app->soundPlay(1046);
			}
		}
		break;

	case kObject1013:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(10071) && !_app->soundIsPlaying(1064)) {
				_app->objectSetAccessibilityOff(kObject1013, 0, 0);
				_app->puzzleSetActive(kPuzzle1013);
				_app->soundPlay(1065);
				_app->varSetByte(10071, 1);
			}
		}
		break;

	case kObjectBracelet:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1009)) {
				_app->soundPlay(1161);
				_app->objectPresentationHide(kObjectBracelet, 0);
				_app->objectSetAccessibilityOff(kObjectBracelet, 0, 0);
				_app->bagAdd(kObjectBracelet);
				_app->varSetByte(1009, 1);
				_app->objectSetAccessibilityOn(kObject1019, 1, 1);
			}
		}
		break;

	case kObject1020:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1011)) {
				_app->puzzleSetActive(kPuzzle1020);
				_app->objectSetAccessibilityOff(kObject1020, 0, 0);
				_app->soundPlay(2018);
				_app->varSetByte(1011, 1);
				_app->onCall(2061);
			}
		}
		break;

	case kObject1021:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1012)) {
				_app->puzzleSetActive(kPuzzle1023);
				_app->objectSetAccessibilityOff(kObject1021, 0, 0);
				_app->soundPlay(2019);
				_app->varSetByte(1012, 1);
			}
		}
		break;

	case kObject1023:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1013)) {
				_app->puzzleSetActive(kPuzzle1026);
				_app->objectSetAccessibilityOff(kObject1023, 0, 0);
				_app->varSetByte(1013, 1);
				_app->soundPlay(2028);
			}
		}
		break;

	case kObjectDoveFeather:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1015)) {
				_app->objectPresentationHide(kObjectDoveFeather, 0);
				_app->objectSetAccessibilityOff(kObjectDoveFeather, 0, 0);
				_app->bagAdd(kObjectDoveFeather);
				_app->rotationSetAlp(10014, 110.0f);
				_app->rotationSetBet(10014, 0.0f);
				_app->soundPlay(2055);
				_app->varSetByte(1015, 1);
			}
		}
		break;

	case kObject1028:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1016)) {
				_app->objectSetAccessibilityOff(kObject1028, 0, 0);
				_app->puzzleSetActive(kPuzzle1007);
				_app->soundPlay(2063);
				_app->varSetByte(1016, 1);
			}
		}
		break;

	case kObject1029:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1017)) {
				_app->soundPlay(900009);
				_app->objectSetAccessibilityOff(kObject1029, 0, 0);
				_app->varSetByte(1017, 1);
				_app->objectPresentationShow(kObject1028);
				_app->objectSetAccessibilityOn(kObject1028, 0, 0);
			}
		}
		break;

	case kObject1031:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1018)) {
				_app->objectSetAccessibilityOff(kObject1031, 0, 0);
				_app->puzzleSetActive(kPuzzle1039);
				_app->soundPlay(2066);
				_app->varSetByte(1018, 1);
			}
		}
		break;

	case kObject1033:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1019)) {
				_app->puzzleSetActive(kPuzzle1020);
				_app->objectSetAccessibilityOff(kObject1033, 0, 0);
				_app->soundPlay(2076);
				_app->varSetByte(1019, 1);
				_app->onCall(2161);
			}
		}
		break;

	case kObject1035:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1022)) {
				_app->objectSetAccessibilityOff(kObject1035, 0, 0);
				_app->puzzleSetActive(kPuzzle1030);
				_app->soundPlay(2040);
				_app->varSetByte(1022, 1);
			}
		}
		break;

	case kObjectBasket:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1025)) {
				_app->objectSetAccessibilityOff(kObjectBasket, 0, 0);
				_app->objectPresentationHide(kObjectBasket, 0);
				_app->bagAdd(kObjectBasket);
				_app->varSetByte(1025, 1);
			}
		}
		break;

	case kObjectBasket2:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1026)) {
				_app->objectSetAccessibilityOff(kObjectBasket2, 0, 0);
				_app->objectPresentationHide(kObjectBasket2, 0);
				_app->bagAdd(kObjectBasket2);
				_app->varSetByte(1026, 1);
			}
		}
		break;

	case kObject1039:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1020)) {
				_app->objectSetAccessibilityOff(kObject1039, 0, 0);
				_app->puzzleSetActive(kPuzzle1027);
				_app->soundPlay(2032);
				_app->varSetByte(1020, 1);
				_app->objectSetAccessibilityOn(kObject1040, 0, 0);
			}
		}
		break;

	case kObject1040:
		if (_app->bagHasClickedObject()) {
			if (target == 1) {

				if (_app->bagGetClickedObject() == kObjectCommercialAdd) {
					_app->soundPlay(1162);
					_app->bagRemove(kObjectCommercialAdd);
					_app->objectPresentationShow(kObjectCommercialAdd, 0);
					_app->varSetByte(10281, 1);
				}

				if (_app->bagGetClickedObject() == kObjectBasket && _app->varGetByte(10281) == 1) {
					_app->soundPlay(1163);
					_app->bagRemove(kObjectBasket);
					_app->objectPresentationShow(kObjectBasket, 1);
					_app->varSetByte(1027, 1);
				}

				if (_app->bagGetClickedObject() == kObjectBasket2 && _app->varGetByte(10281) == 1) {
					_app->soundPlay(1163);
					_app->bagRemove(kObjectBasket2);
					_app->objectPresentationShow(kObjectBasket2, 1);
					_app->varSetByte(1028, 1);
				}
			}

			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1021)) {
				_app->objectSetAccessibilityOff(kObject1040, 0, 0);
				_app->objectSetAccessibilityOn(kObject1040, 1, 1);
				_app->playMovie("S00A01-9");
				_app->puzzleSetActive(kPuzzle1027);
				_app->soundPlay(2033);
				_app->varSetByte(1021, 1);
			}

			if (!_app->soundIsPlaying(1168) && !_app->soundIsPlaying(1169))
				_app->soundPlay(1168 + rnd(2));
		}

		if (_app->varGetByte(1027)  == 1
		 && _app->varGetByte(1028)  == 1
		 && _app->varGetByte(10281) == 1
		 && _app->varGetByte(1021)  == 1) {
			_app->objectSetAccessibilityOff(kObject1040, 1, 1);
			_app->playMovie("S00A0-18");
			_app->objectPresentationShow(kObject1045);
			_app->puzzleSetActive(kPuzzle1035);
			_app->soundPlay(2049);
		}
		break;

	case kObject1044:
		if (_app->bagHasClickedObject()) {
			if (_app->bagGetClickedObject() == kObjectFlask && _app->varGetByte(10651) == 1) {
				_app->objectSetAccessibilityOff(kObject1044, 0, 0);
				_app->objectSetAccessibilityOff(kObject1044, 1, 1);
				_app->objectSetAccessibilityOff(kObject10441, 0, 0);
				_app->rotationSetActive(10013);
				_app->bagRemove(kObjectFlask);
				_app->bagAdd(kObjectPerfumedOil);
				_app->soundPlay(2054);
				onVisual(210);
			}

			_app->cursorDelete();
		} else {
			if (target == 1) {
				_app->objectSetAccessibilityOff(kObject1044, 1, 1);
				_app->rotationSetActive(10013);
				_app->objectSetAccessibilityOn(kObject10441, 0, 0);
			}
		}
		break;

	case kObject1049:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1030)) {
				_app->objectSetAccessibilityOff(kObject1049, 0, 0);
				_app->puzzleSetActive(kPuzzle1040);
				_app->soundPlay(3006);
				_app->varSetByte(1030, 1);
			}
		}
		break;

	case kObject1050:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1032)) {
				_app->objectSetAccessibilityOff(kObject1050, 0, 0);
				_app->puzzleSetActive(kPuzzle1030);
				_app->soundPlay(3008);
				_app->varSetByte(1032, 1);
			}
		}
		break;

	case kObject1051:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1033)) {
				_app->objectSetAccessibilityOff(kObject1051, 0, 0);
				_app->varSetByte(1033, 1);
				_app->puzzleSetActive(kPuzzle1033);
				_app->soundPlay(3011);
			}
		}
		break;

	case kObject1053:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1031)) {
				_app->objectSetAccessibilityOff(kObject1053, 0, 0);
				_app->puzzleSetActive(kPuzzle1027);
				_app->soundPlay(3007);
				_app->varSetByte(1031, 1);
			}
		}
		break;

	case kObject1057:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1035)) {
				_app->objectSetAccessibilityOff(kObject1057, 0, 0);
				_app->puzzleSetActive(kPuzzle1045);
				_app->soundPlay(3023);
				_app->varSetByte(1035, 1);
				_app->varSetByte(90307, 1);
			}
		}
		break;

	case kObject1059:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target) {
				switch (_app->varGetByte(1036)) {
				default:
					break;

				case 0:
					_app->objectSetAccessibilityOff(kObject1059, 0, 0);
					_app->varSetByte(1036, 1);
					_app->puzzleSetActive(kPuzzle1046);
					_app->soundPlay(3028);
					break;

				case 1:
					_app->objectSetAccessibilityOff(kObject1059, 0, 0);
					_app->puzzleSetActive(kPuzzle1046);
					_app->soundPlay(3038);
					break;
				}
			}
		}
		break;

	case kObject1061:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1037)) {
				_app->objectSetAccessibilityOff(kObject1061, 0, 0);
				_app->varSetByte(1037, 1);
				_app->puzzleSetActive(kPuzzle1049);
				_app->soundPlay(3041);
			}
		}
		break;

	case kObject1062:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1038)) {
				_app->objectSetAccessibilityOff(kObject1062, 0, 0);
				_app->varSetByte(1038, 1);
				_app->puzzleSetActive(kPuzzle1051);
				_app->soundPlay(3045);
			}
		}
		break;

	case kObject1064:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1040) && !_app->soundIsPlaying(4008)) {
				_app->objectSetAccessibilityOff(kObject1064, 0, 0);
				_app->varSetByte(1040, 1);
				_app->puzzleSetActive(kPuzzle1002);
				_app->soundPlay(4009);
			}
		}
		break;

	case kObject1066:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1041)) {
				_app->objectSetAccessibilityOff(kObject1066, 0, 0);
				_app->varSetByte(1041, 1);
				_app->puzzleSetActive(kPuzzle1027);
				_app->soundPlay(4015);
			}
		}
		break;

	case kObject1068:
		if (_app->bagHasClickedObject()) {
			if (target == 2
			 && _app->bagGetClickedObject() == kObjectBranch
			 && _app->varGetByte(1047) == 1
			 && !_app->varGetByte(1046)) {
				_app->bagRemove(kObjectBranch);
				_app->objectSetAccessibilityOff(kObject1068, 2, 2);
				_app->puzzleSetActive(kPuzzle1053);
				_app->soundPlay(4032);
				_app->varSetByte(1046, 1);
			}

			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1042)) {
				_app->objectSetAccessibilityOn(kObjectStone, 0, 0);
				_app->objectSetAccessibilityOn(kObject1072, 0, 0);
				_app->objectSetAccessibilityOff(kObject1068, 0, 0);
				_app->varSetByte(1042, 1);
				_app->puzzleSetActive(kPuzzle1053);
				_app->soundPlay(4031);
			}
		}
		break;

	case kObjectStone:
		if (_app->bagHasClickedObject()) {
			_app->setField74(false);
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1044)) {
				_app->soundPlay(1170);
				_app->bagAdd(kObjectStone);
				_app->soundPlay(1174 + rnd(3));
				_app->soundPlay(1177 + rnd(3), kSoundLoop);
				_app->objectSetAccessibilityOff(kObjectStone, 0, 0);
				_app->varSetByte(1044, 1);
				_app->objectPresentationShow(kObject1075, 0);
				_app->objectSetAccessibilityOn(kObjectBranch, 1, 1);
				_app->timerStart(kTimer4, 20000);
			}
		}
		break;

	case kObjectBranch:
		if (_app->bagHasClickedObject()) {
			if (target != 1 || _app->bagGetClickedObject() != kObjectBranch) {
				_app->setField74(false);
				_app->cursorDelete();
			} else {
				if (_app->soundIsPlaying(1177))
					_app->soundStop(1177, 1024);

				if (_app->soundIsPlaying(1178))
					_app->soundStop(1178, 1024);

				if (_app->soundIsPlaying(1179))
					_app->soundStop(1179, 1024);

				_app->timerStop(kTimer4);
				_app->playMovie("S00A05-8");
				_app->objectPresentationHide(kObject1075, 0);
				_app->objectSetAccessibilityOff(kObjectBranch, 1, 1);
				_app->timerStart(kTimer5, 10000);
				_app->objectSetAccessibilityOn(kObject1076, 0, 0);
				_app->cursorDelete();
			}
		} else {
			if (target || _app->varGetByte(1045)) {
				_app->setField74(false);
			} else {
				_app->soundPlay(1173);
				_app->objectPresentationHide(kObjectBranch, 0);
				_app->objectSetAccessibilityOff(kObjectBranch, 0, 0);
				_app->varSetByte(1045, 1);
				_app->bagAdd(kObjectBranch);
			}
		}
		break;

	case kObject1072:
		if (_app->bagHasClickedObject()) {
			if (_app->bagGetClickedObject() == kObjectStone) {
				_app->playMovie("S00A05-3");
				_app->soundPlay(1172);
				_app->bagRemove(kObjectStone);
				_app->objectPresentationShow(kObjectBranch, 0);
				_app->objectSetAccessibilityOff(kObject1072, 0, 0);
				_app->objectSetAccessibilityOn(kObjectBranch, 0, 0);
			}

			_app->cursorDelete();
		}
		break;

	case kObject1076:
		if (_app->bagHasClickedObject()) {
			if (_app->bagGetClickedObject() == kObjectBranch && !target && !_app->varGetByte(1047)) {
					if (_app->soundIsPlaying(1177))
						_app->soundStop(1177, 1024);

					if (_app->soundIsPlaying(1178))
						_app->soundStop(1178, 1024);

					if (!_app->soundIsPlaying(1179))
						_app->soundStop(1179, 1024);

					_app->soundPlay(1180 + rnd(3));
					_app->objectSetAccessibilityOff(kObject1076, 0, 0);
					_app->timerStop(kTimer5);
					_app->playMovie("S00A05-2");
					_app->varSetByte(1047, 1);
					_app->objectSetAccessibilityOn(kObject1068, 2, 2);
			}

			_app->cursorDelete();
		}
		break;

	case kObject1077:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1048)) {
				_app->objectSetAccessibilityOff(kObject1077, 0, 0);
				_app->varSetByte(1048, 1);
				_app->puzzleSetActive(kPuzzle1056);
				_app->soundPlay(4042);
			}
		}
		break;

	case kObject1080:
		if (_app->bagHasClickedObject()) {
			if (target || (_app->bagGetClickedObject() != kObjectKnife && _app->bagGetClickedObject() != kObjectStone2)) {
				_app->cursorDelete();
			} else {
				_app->exitToMenu(kMenuAction10);
				return;
			}
		}
		break;

	case kObject1081:
		if (_app->bagHasClickedObject()) {
			if (target == 2) {

				if (_app->bagGetClickedObject() == kObjectPhiltre2) {
					_app->objectSetAccessibilityOff(kObject1081, 2, 2);
					_app->soundPlay(1183 + rnd(2));
					_app->objectPresentationHide(kObject1081, 0);
					_app->objectPresentationHide(kObject1081, 2);
					_app->objectPresentationShow(kObject1081, 1);
					_app->soundPlay(4113);
				}

				if (_app->bagGetClickedObject() == kObjectStone2) {
					_app->exitToMenu(kMenuAction12);
					return;
				}

				if (_app->bagGetClickedObject() == kObjectKnife) {
					_app->exitToMenu(kMenuAction10);
					return;
				}
			}

			_app->cursorDelete();
		}
		break;

	case kObject1084:
		if (_app->bagHasClickedObject()) {
			if (_app->bagGetClickedObject() == kObjectKnife && !_app->varGetByte(1051)) {
				_app->rotationSetMovabilityOff(1005402);
				_app->soundPlay(900015);
				_app->playMovie("S00A05-5");
				_app->varSetByte(1051, 1);
				_app->objectPresentationHide(kObject1084);
				_app->objectSetAccessibilityOff(kObject1084, 0, 0);
				_app->objectPresentationShow(kObject1085);
				_app->soundPlay(4115);
			}

			_app->cursorDelete();
		}
		break;

	case kObject1089:
		if (_app->bagHasClickedObject()) {
			if (_app->bagGetClickedObject() == kObjectMaskAndCoat) {
				_app->soundPlay(1185);
				_app->bagRemove(kObjectMaskAndCoat);
				_app->objectSetAccessibilityOff(kObject1089, 0, 0);
				_app->varSetByte(1052, 1);
				_app->soundPlay(4117);
				_app->objectPresentationShow(kObject1089);
				_app->rotationSetMovabilityOn(10011203, 0, 0);
			}

			_app->cursorDelete();
		}
		break;

	case kObject1093:
		if (!_app->bagHasClickedObject()) {
			switch (target) {
			default:
				break;

			case 0:
				if (_app->varGetByte(1052) == 0) {
					_app->objectSetAccessibilityOff(kObject1093, 0, 0);
					_app->puzzleSetActive(kPuzzle1063);
					_app->soundPlay(4116);
				}

				if (_app->varGetByte(1052) == 1) {
					if (!_app->varGetByte(1053)) {
						_app->objectSetAccessibilityOff(kObject1093, 0, 0);
						_app->puzzleSetActive(kPuzzle1063);
						_app->soundPlay(4118);
						_app->varSetByte(1053, 1);
					}
				}
				break;

			case 1:
				if (_app->varGetByte(1053) == 1) {
					_app->objectSetAccessibilityOff(kObject1093, 1, 1);
					_app->puzzleSetActive(kPuzzle1063);
					_app->soundPlay(4119);
					_app->varSetByte(1053, 2);
				}
				break;
			}
		} else {
			if (target == 2) {

				if (_app->bagGetClickedObject() == kObjectPhiltre2 && _app->varGetByte(1053) == 2) {
					_app->objectSetAccessibilityOff(kObject1093, 2, 2);
					_app->puzzleSetActive(kPuzzle1063);
					_app->soundPlay(4123);
				}

				if (_app->bagGetClickedObject() == kObjectStone2 && _app->varGetByte(1053) == 2) {
					_app->objectSetAccessibilityOff(kObject1093, 2, 2);
					_app->soundPlay(4122);
				}

				if (_app->bagGetClickedObject() == kObjectCapsa && _app->varGetByte(1053) == 2) {
					_app->objectSetAccessibilityOff(kObject1093, 2, 2);
					_app->puzzleSetActive(kPuzzle1063);
					_app->soundPlay(4120);
				}

				if (_app->bagGetClickedObject() == kObjectKnife && _app->varGetByte(1053) == 2) {
					_app->exitToMenu(kMenuAction10);
					return;
				}
			}

			_app->cursorDelete();
		}
		break;

	case kObject1094:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (target == 90 || target == 89 || target == 105)
				_app->showEncyclopedia((uint32)target);
		}
		break;

	case kObjectStone2:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target) {
				_app->bagAdd(kObjectStone2);
				_app->objectPresentationHide(kObjectStone2);
				_app->objectSetAccessibilityOff(kObjectStone2);
			}
		}
		break;

	case kObject10041:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			// Original sest an unused variable to 0
			_app->onCall(10001);
		}
		break;

	case kObject10042:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			_app->onCall(100021);
		}
		break;

	case kObject10043:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			_app->onCall(100022);
		}
		break;

	case kObject10044:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			_app->onCall(10003);
		}
		break;

	case kObject10045:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			_app->onCall(10004);
		}
		break;

	case kObject10046:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			_app->onCall(10005);
		}
		break;

	case kObject10047:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);

			if (_app->varGetByte(90208) == 2) {
				_app->varSetByte(90208, 3);
				_app->onCall(2089);
			}

			onVisual(0);
			_app->timerStopAll();
			_app->onCall(10006);
		}
		break;

	case kObject10048:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			_app->onCall(10007);
		}
		break;

	case kObject10049:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			_app->onCall(10008);
		}
		break;

	case kObject10050:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			_app->onCall(100091);
		}
		break;

	case kObject10051:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			_app->onCall(100092);
		}
		break;

	case kObject10052:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			_app->onCall(100093);
		}
		break;

	case kObject10053:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			_app->onCall(10010);
		}
		break;

	case kObject10054:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			_app->onCall(30);

			if (_app->varGetByte(90408) == 2 && !_app->varGetByte(1049)) {
				_app->rotationSetMovabilityOff(10052);
				_app->varSetByte(90408, 3);
				_app->varSetByte(1049, 1);
				_app->soundPlay(4070);

				uint32 startTick = g_system->getMillis();
				while (g_system->getMillis() - startTick <= 1500)
					handleEvents();

				_app->rotationSetMovabilityOn(10052);
				_app->onCall(4099);
				_app->onCall(410);
			}

			onVisual(0);
			_app->timerStopAll();
			_app->onCall(10011);
		}
		break;

	case kObject10071:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(1004)) {
				_app->objectSetAccessibilityOff(kObject10071, 0, 0);
				_app->puzzleSetActive(kPuzzle1004);
				_app->soundPlay(1031);
				_app->varSetByte(1004, 1);
			}
		}
		break;

	case kObject10441:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target) {
				_app->objectSetAccessibilityOff(kObject10441, 0, 0);
				_app->puzzleSetActive(kPuzzle1036);

				if (_app->varGetByte(10651) == 1)
					_app->objectSetAccessibilityOn(kObject1044, 0, 0);

				_app->objectSetAccessibilityOn(kObject1044, 1, 1);
			}
		}
		break;

	case kObject10591:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			if (!target && !_app->varGetByte(10361)) {
				_app->objectSetAccessibilityOff(kObject10591, 0, 0);
				_app->varSetByte(10361, 1);
				_app->puzzleSetActive(kPuzzle1048);
				_app->soundPlay(3036);
			}
		}
		break;

	case kObjectExitEncyclopedia:
		_app->restore();
		break;

	case kObjectRose:
		if (_app->bagHasClickedObject()) {
			_app->cursorDelete();
		} else {
			switch (target) {
			default:
				break;

			case 0:
				if (_app->varGetByte(1014)) {
					_app->soundPlay(1164 + rnd(3));
					_app->objectSetAccessibilityOff(kObjectRose, 0, 0);
					_app->bagAdd(kObjectRose);
					_app->playMovie("S00A01-7");
					_app->varSetByte(1014, 1);
					_app->objectPresentationShow(kObjectDoveFeather, 0);
					_app->objectSetAccessibilityOn(kObjectDoveFeather, 0, 0);
				}
				break;

			case 1:
				_app->puzzleSetActive(kPuzzle1037);
				_app->objectSetAccessibilityOn(kObjectRose, 3, 3);
				break;

			case 2:
				_app->puzzleSetActive(kPuzzle1038);
				_app->objectSetAccessibilityOn(kObjectRose, 4, 4);
				break;

			case 3:
				_app->objectSetAccessibilityOff(kObjectRose, 3, 3);
				_app->rotationSetActive(10014);
				break;

			case 4:
				_app->objectSetAccessibilityOff(kObjectRose, 4, 4);
				_app->rotationSetActive(10014);
				break;
			}
		}
		break;
	}

	if (_app->varGetByte(1024) == 1
	 && _app->varGetByte(1026) == 1
	 && _app->varGetByte(1022) == 1
	 && _app->varGetByte(1062) == 1
	 && _app->varGetByte(1065) == 0)
		_app->objectSetAccessibilityOn(kObject1039, 2, 2);

	onVisual(204);
	onVisual(206);
	onVisual(211);
	onVisual(214);
	onVisual(402);
	onVisual(403);
	onVisual(405);
	onVisual(406);
	onVisual(409);
	onVisual(420);
}

void Zone1Pompeii::onTimer(TimerId id) {
	switch (id) {
	default:
		break;

	case kTimer0:
		_app->timerStop(kTimer0);
		_app->soundPlay(1027);
		_app->timerStart(kTimer1, 40000);
		break;

	case kTimer1:
		_app->timerStop(kTimer1);
		_app->soundPlay(1028);
		_app->timerStart(kTimer2, 700000);
		break;

	case kTimer2:
		_app->timerStop(kTimer2);
		_app->objectSetAccessibilityOff(kObjectRope, 0, 0);
		_app->objectSetAccessibilityOff(kObject1006);
		_app->soundPlay(1029);
		break;

	case kTimer3:
		_app->timerStop(kTimer3);
		if (!_app->varGetByte(1029)) {
			_app->playMovie("S00A0-17");
			_app->exitToMenu(kMenuAction4);
		}
		break;

	case kTimer4:
		_app->timerStop(kTimer4);
		_app->playMovie("S00A05-9");
		_app->exitToMenu(kMenuAction8);
		break;

	case kTimer5:
		_app->timerStop(kTimer5);
		_app->playMovie("S00A05-1");
		_app->exitToMenu(kMenuAction8);
		break;

	case kTimer6:
		_app->timerStop(kTimer6);
		_app->playMovie("S00A01-3");
		_app->exitToMenu(kMenuAction9);
		break;

	case kTimer7:
		_app->timerStop(kTimer7);
		switch (_app->varGetByte(1097)) {
		default:
			break;

		case 1:
			_app->rotationSet3DSoundOff(10011002, 1191);
			break;

		case 2:
			_app->rotationSet3DSoundOff(1001902, 1191);
			break;

		case 3:
			_app->rotationSet3DSoundOff(1001702, 1191);
			break;

		case 4:
			_app->rotationSet3DSoundOff(1001602, 1191);
			break;

		case 5:
			_app->rotationSet3DSoundOff(1001502, 1191);
			break;

		case 6:
			_app->rotationSet3DSoundOff(1001402, 1191);
			break;

		case 7:
			_app->rotationSet3DSoundOff(10011402, 1191);
			break;

		case 8:
			_app->rotationSet3DSoundOff(1001202, 1191);
			break;

		case 9:
			_app->rotationSet3DSoundOff(1001102, 1191);
			_app->onCall(419);
			break;
		}
		break;

	case kTimer8:
		_app->timerStop(kTimer8);
		_app->rotationSet3DSoundOn(1001102, 1191);
		_app->timerStart(kTimer7, 3000);
		_app->varSetByte(1097, 9);
		break;

	case kTimer9:
		_app->timerStop(kTimer9);
		if (_app->varGetByte(90416) == 1 && _app->getCurrentRotationId() == 10011002) {
			_app->varSetByte(90416, 2);
			_app->timerStart(kTimer6, 2000);
			_app->soundPlay(1190);
		}
		break;

	case kTimer10:
		_app->timerStop(kTimer10);
		_app->objectPresentationUnpauseAnimation(kObject1001, 0);
		_app->objectPresentationUnpauseAnimation(kObject1001, 1);
		_app->objectPresentationUnpauseAnimation(kObject1001, 3);
		_app->objectPresentationUnpauseAnimation(kObject1006, 0);
		_app->objectPresentationUnpauseAnimation(kObject10071, 0);
		_app->objectPresentationUnpauseAnimation(kObject10071, 1);
		_app->objectPresentationUnpauseAnimation(kObject1009, 0);
		_app->objectPresentationUnpauseAnimation(kObject1009, 1);
		_app->objectPresentationUnpauseAnimation(kObject1011, 0);
		_app->objectPresentationUnpauseAnimation(kObject1011, 1);
		_app->objectPresentationUnpauseAnimation(kObject1013, 0);
		_app->objectPresentationUnpauseAnimation(kObject1013, 1);
		_app->objectPresentationUnpauseAnimation(kObject1016, 0);
		_app->objectPresentationUnpauseAnimation(kObject1017, 0);
		_app->objectPresentationUnpauseAnimation(kObject1017, 1);
		_app->objectPresentationUnpauseAnimation(kObject1019, 0);
		_app->objectPresentationUnpauseAnimation(kObject1019, 1);
		_app->objectPresentationUnpauseAnimation(kObject1020, 0);
		_app->objectPresentationUnpauseAnimation(kObject1020, 1);
		_app->objectPresentationUnpauseAnimation(kObject1021, 0);
		_app->objectPresentationUnpauseAnimation(kObject1021, 2);
		_app->objectPresentationUnpauseAnimation(kObject1023, 0);
		_app->objectPresentationUnpauseAnimation(kObject1023, 1);
		_app->objectPresentationUnpauseAnimation(kObject1039, 0);
		_app->objectPresentationUnpauseAnimation(kObject1039, 1);
		_app->objectPresentationUnpauseAnimation(kObject1040, 0);
		_app->objectPresentationUnpauseAnimation(kObject1035, 0);
		_app->objectPresentationUnpauseAnimation(kObject1041, 0);
		_app->objectPresentationUnpauseAnimation(kObject1045, 0);
		_app->objectPresentationUnpauseAnimation(kObject1028, 0);
		_app->objectPresentationUnpauseAnimation(kObject1028, 1);
		_app->objectPresentationUnpauseAnimation(kObject1031, 0);
		_app->objectPresentationUnpauseAnimation(kObject1031, 1);
		_app->objectPresentationUnpauseAnimation(kObject1033, 0);
		_app->objectPresentationUnpauseAnimation(kObject1033, 1);
		_app->objectPresentationUnpauseAnimation(kObject1046, 0);
		_app->objectPresentationUnpauseAnimation(kObject1046, 1);
		_app->objectPresentationUnpauseAnimation(kObject1050, 0);
		_app->objectPresentationUnpauseAnimation(kObject1051, 0);
		_app->objectPresentationUnpauseAnimation(kObject1053, 0);
		_app->objectPresentationUnpauseAnimation(kObject1054, 0);
		_app->objectPresentationUnpauseAnimation(kObject1055, 0);
		_app->objectPresentationUnpauseAnimation(kObject1049, 0);
		_app->objectPresentationUnpauseAnimation(kObject1057, 0);
		_app->objectPresentationUnpauseAnimation(kObject1057, 1);
		_app->objectPresentationUnpauseAnimation(kObject1059, 0);
		_app->objectPresentationUnpauseAnimation(kObject1059, 1);
		_app->objectPresentationUnpauseAnimation(kObject10591, 0);
		_app->objectPresentationUnpauseAnimation(kObject10591, 1);
		_app->objectPresentationUnpauseAnimation(kObject1062, 0);
		_app->objectPresentationUnpauseAnimation(kObject1062, 1);
		_app->objectPresentationUnpauseAnimation(kObject1061, 2);
		_app->objectPresentationUnpauseAnimation(kObject1064, 0);
		_app->objectPresentationUnpauseAnimation(kObject1066, 0);
		_app->objectPresentationUnpauseAnimation(kObject1067, 0);
		_app->objectPresentationUnpauseAnimation(kObject1068, 0);
		_app->objectPresentationUnpauseAnimation(kObject1068, 1);
		_app->objectPresentationUnpauseAnimation(kObject1077, 0);
		_app->objectPresentationUnpauseAnimation(kObject1077, 1);
		_app->objectPresentationUnpauseAnimation(kObject1079, 0);
		_app->objectPresentationUnpauseAnimation(kObject1081, 0);
		_app->objectPresentationUnpauseAnimation(kObject1081, 2);
		_app->objectPresentationUnpauseAnimation(kObject1084, 0);
		_app->objectPresentationUnpauseAnimation(kObject1085, 0);
		_app->objectPresentationUnpauseAnimation(kObject1093, 0);
		break;

	case kTimer11:
		_app->timerStop(kTimer11);
		_app->objectPresentationUnpauseAnimation(kObject1001, 2);
		_app->objectPresentationUnpauseAnimation(kObject1001, 4);
		_app->objectPresentationUnpauseAnimation(kObject10071, 2);
		_app->objectPresentationUnpauseAnimation(kObject1010, 0);
		_app->objectPresentationUnpauseAnimation(kObject1011, 2);
		_app->objectPresentationUnpauseAnimation(kObject1013, 2);
		_app->objectPresentationUnpauseAnimation(kObject1016, 1);
		_app->objectPresentationUnpauseAnimation(kObject1019, 2);
		_app->objectPresentationUnpauseAnimation(kObject1053, 1);
		_app->objectPresentationUnpauseAnimation(kObject1054, 1);
		_app->objectPresentationUnpauseAnimation(kObject1057, 2);
		_app->objectPresentationUnpauseAnimation(kObject1059, 2);
		_app->objectPresentationUnpauseAnimation(kObject1061, 0);
		_app->objectPresentationUnpauseAnimation(kObject1061, 1);
		_app->objectPresentationUnpauseAnimation(kObject1061, 3);
		_app->objectPresentationUnpauseAnimation(kObject1064, 1);
		_app->objectPresentationUnpauseAnimation(kObject1066, 1);
		_app->objectPresentationUnpauseAnimation(kObject1068, 2);
		_app->objectPresentationUnpauseAnimation(kObject1077, 2);
		_app->objectPresentationUnpauseAnimation(kObject1078, 0);
		_app->objectPresentationUnpauseAnimation(kObject1093, 1);
		break;
	}
}

void Zone1Pompeii::onAnimationNextFrame(Id animationId, const Common::String &/*name*/, uint32 frame, uint32 frameCount) {
	switch (animationId) {
	default:
		break;

	case 1001:
		if (frame == frameCount) {
			_app->objectPresentationPauseAnimation(kObject1001, 0);
			_app->objectPresentationPauseAnimation(kObject1001, 1);
			_app->objectPresentationPauseAnimation(kObject1001, 3);
			_app->objectPresentationPauseAnimation(kObject1006, 0);
			_app->objectPresentationPauseAnimation(kObject10071, 0);
			_app->objectPresentationPauseAnimation(kObject10071, 1);
			_app->objectPresentationPauseAnimation(kObject1009, 0);
			_app->objectPresentationPauseAnimation(kObject1009, 1);
			_app->objectPresentationPauseAnimation(kObject1011, 0);
			_app->objectPresentationPauseAnimation(kObject1011, 1);
			_app->objectPresentationPauseAnimation(kObject1013, 0);
			_app->objectPresentationPauseAnimation(kObject1013, 1);
			_app->objectPresentationPauseAnimation(kObject1016, 0);
			_app->objectPresentationPauseAnimation(kObject1017, 0);
			_app->objectPresentationPauseAnimation(kObject1017, 1);
			_app->objectPresentationPauseAnimation(kObject1019, 0);
			_app->objectPresentationPauseAnimation(kObject1019, 1);
			_app->objectPresentationPauseAnimation(kObject1020, 0);
			_app->objectPresentationPauseAnimation(kObject1020, 1);
			_app->objectPresentationPauseAnimation(kObject1021, 0);
			_app->objectPresentationPauseAnimation(kObject1021, 2);
			_app->objectPresentationPauseAnimation(kObject1023, 0);
			_app->objectPresentationPauseAnimation(kObject1023, 1);
			_app->objectPresentationPauseAnimation(kObject1039, 0);
			_app->objectPresentationPauseAnimation(kObject1039, 1);
			_app->objectPresentationPauseAnimation(kObject1040, 0);
			_app->objectPresentationPauseAnimation(kObject1035, 0);
			_app->objectPresentationPauseAnimation(kObject1041, 0);
			_app->objectPresentationPauseAnimation(kObject1045, 0);
			_app->objectPresentationPauseAnimation(kObject1028, 0);
			_app->objectPresentationPauseAnimation(kObject1028, 1);
			_app->objectPresentationPauseAnimation(kObject1031, 0);
			_app->objectPresentationPauseAnimation(kObject1031, 1);
			_app->objectPresentationPauseAnimation(kObject1033, 0);
			_app->objectPresentationPauseAnimation(kObject1033, 1);
			_app->objectPresentationPauseAnimation(kObject1046, 0);
			_app->objectPresentationPauseAnimation(kObject1046, 1);
			_app->objectPresentationPauseAnimation(kObject1050, 0);
			_app->objectPresentationPauseAnimation(kObject1051, 0);
			_app->objectPresentationPauseAnimation(kObject1053, 0);
			_app->objectPresentationPauseAnimation(kObject1054, 0);
			_app->objectPresentationPauseAnimation(kObject1055, 0);
			_app->objectPresentationPauseAnimation(kObject1049, 0);
			_app->objectPresentationPauseAnimation(kObject1057, 0);
			_app->objectPresentationPauseAnimation(kObject1057, 1);
			_app->objectPresentationPauseAnimation(kObject1059, 0);
			_app->objectPresentationPauseAnimation(kObject1059, 1);
			_app->objectPresentationPauseAnimation(kObject10591, 0);
			_app->objectPresentationPauseAnimation(kObject10591, 1);
			_app->objectPresentationPauseAnimation(kObject1062, 0);
			_app->objectPresentationPauseAnimation(kObject1062, 1);
			_app->objectPresentationPauseAnimation(kObject1061, 2);
			_app->objectPresentationPauseAnimation(kObject1064, 0);
			_app->objectPresentationPauseAnimation(kObject1066, 0);
			_app->objectPresentationPauseAnimation(kObject1067, 0);
			_app->objectPresentationPauseAnimation(kObject1068, 0);
			_app->objectPresentationPauseAnimation(kObject1068, 1);
			_app->objectPresentationPauseAnimation(kObject1077, 0);
			_app->objectPresentationPauseAnimation(kObject1077, 1);
			_app->objectPresentationPauseAnimation(kObject1079, 0);
			_app->objectPresentationPauseAnimation(kObject1081, 0);
			_app->objectPresentationPauseAnimation(kObject1081, 2);
			_app->objectPresentationPauseAnimation(kObject1084, 0);
			_app->objectPresentationPauseAnimation(kObject1085, 0);
			_app->objectPresentationPauseAnimation(kObject1093, 0);

			_app->timerStart(kTimer10, 1000 * (rnd(5) + 2));
		}
		break;

	case 1002:
		if (frame == frameCount) {
			_app->objectPresentationPauseAnimation(kObject1001, 2);
			_app->objectPresentationPauseAnimation(kObject1001, 4);
			_app->objectPresentationPauseAnimation(kObject10071, 2);
			_app->objectPresentationPauseAnimation(kObject1010, 0);
			_app->objectPresentationPauseAnimation(kObject1011, 2);
			_app->objectPresentationPauseAnimation(kObject1013, 2);
			_app->objectPresentationPauseAnimation(kObject1016, 1);
			_app->objectPresentationPauseAnimation(kObject1019, 2);
			_app->objectPresentationPauseAnimation(kObject1053, 1);
			_app->objectPresentationPauseAnimation(kObject1054, 1);
			_app->objectPresentationPauseAnimation(kObject1057, 2);
			_app->objectPresentationPauseAnimation(kObject1059, 2);
			_app->objectPresentationPauseAnimation(kObject1061, 0);
			_app->objectPresentationPauseAnimation(kObject1061, 1);
			_app->objectPresentationPauseAnimation(kObject1061, 3);
			_app->objectPresentationPauseAnimation(kObject1064, 1);
			_app->objectPresentationPauseAnimation(kObject1066, 1);
			_app->objectPresentationPauseAnimation(kObject1068, 2);
			_app->objectPresentationPauseAnimation(kObject1077, 2);
			_app->objectPresentationPauseAnimation(kObject1078, 0);
			_app->objectPresentationPauseAnimation(kObject1093, 1);
			_app->timerStart(kTimer11, 1000 * (rnd(5) + 2));
		}
		break;
	}
}

void Zone1Pompeii::onSound(Id id, SoundType /*type*/, uint32 /*a3*/, bool /*process*/) {
	switch (id) {
	default:
		break;

	case 1025:
		_app->objectSetAccessibilityOn(kObject1001, 0, 0);
		_app->objectSetAccessibilityOn(kObject1005, 0, 0);
		_app->soundPlay(1168 + rnd(2));
		break;

	case 1026:
		_app->objectSetAccessibilityOn(kObject1006, 0, 1);
		_app->objectSetAccessibilityOn(kObjectStick, 0, 0);
		_app->objectSetAccessibilityOn(kObjectSalad, 0, 0);
		_app->rotationSetActive(10012);
		_app->timerStart(kTimer0, 10000);
		break;

	case 1029:
		_app->playMovie("S00A0-10");
		_app->exitToMenu(kMenuAction1);
		break;

	case 1030:
		_app->playMovie("S00A01-1");
		_app->objectPresentationHide(kObject1001);
		_app->objectPresentationHide(kObject1005);
		_app->objectPresentationHide(kObject1006);
		_app->objectPresentationHide(kObjectRope);
		_app->objectPresentationHide(kObjectStick);
		_app->rotationSetActive(10012);
		onVisual(104);
		break;

	case 1031:
		_app->objectSetAccessibilityOn(kObject10071, 1, 1);
		// Fback to next case

	case 1032:
	case 1033:
	case 1034:
	case 1035:
		_app->rotationSetAlp(10022, 152.0f);
		_app->rotationSetBet(10022, 15.0f);
		_app->rotationSetActive(10022);

		if (_app->varGetByte(1054) == 1) {
			_app->objectSetAccessibilityOff(kObject10071, 1, 1);
			_app->playMovie("S00A02-3");
			_app->objectSetAccessibilityOff(kObject10071, 0, 0);
			_app->objectPresentationHide(kObject10071);
			_app->objectSetAccessibilityOn(kObject1008, 0, 0);
			_app->objectPresentationShow(kObject1010);
			_app->objectPresentationShow(kObject1011);
			_app->objectSetAccessibilityOn(kObject1011, 0, 0);
		}
		break;

	case 1036:
		_app->objectSetAccessibilityOn(kObject1009, 1, 1);
		// Fback to next case

	case 1037:
		_app->rotationSetAlp(10022, 175.0f);
		_app->rotationSetBet(10022, 10.0f);
		_app->rotationSetActive(10022);

		if (_app->varGetByte(1055) == 1) {
			_app->objectSetAccessibilityOff(kObject1009, 1, 1);
			_app->rotationSetActive(10022);
			_app->soundPlay(900004);
			_app->objectPresentationHide(kObject1009);
			_app->objectSetAccessibilityOn(kObject1008, 0, 0);
			onVisual(105);
		}
		break;

	case 1038:
		_app->rotationSetActive(10022);
		_app->soundPlay(900004);
		_app->objectPresentationHide(kObject1009);
		_app->objectSetAccessibilityOff(kObject1008, 0, 0);
		break;

	case 1039:
		_app->objectSetAccessibilityOn(kObject1011, 1, 1);
		// Fback to next case

	case 1040:
	case 1041:
	case 1042:
	case 1043:
		_app->rotationSetAlp(10021, 104.0f);
		_app->rotationSetBet(10021, 15.0f);
		_app->rotationSetActive(10021);

		if (_app->varGetByte(1056) == 1) {
			_app->objectSetAccessibilityOff(kObject1011, 1, 1);
			_app->bagAdd(kObjectContract);
			_app->takeMoney(100);
			_app->playMovie("S00A02-4");
			_app->objectPresentationHide(kObject1011);
			onVisual(106);
		}
		break;

	case 1046:
		_app->rotationSetAlp(10023, 80.0f);
		_app->rotationSetBet(10023, 10.0f);
		_app->rotationSetActive(10023);
		_app->objectSetAccessibilityOn(kObject1012, 1, 1);
		break;

	case 1047:
		_app->rotationSetAlp(10023, 80.0f);
		_app->rotationSetBet(10023, 10.0f);
		_app->rotationSetActive(10023);
		_app->objectSetAccessibilityOn(kObject1012, 2, 2);
		break;

	case 1048:
		_app->rotationSetAlp(10023, 80.0f);
		_app->rotationSetBet(10023, 10.0f);
		_app->rotationSetActive(10023);
		_app->objectSetAccessibilityOff(kObject1012, 2, 2);
		_app->takeMoney(300);
		_app->bagAdd(kObjectKnife);
		onVisual(108);
		break;

	case 1065:
		_app->objectSetAccessibilityOn(kObject1013, 1, 1);
		// Fback to next case

	case 1066:
	case 1067:
		_app->rotationSetAlp(10023, 80.0f);
		_app->rotationSetBet(10023, 10.0f);
		_app->rotationSetActive(10023);

		if (_app->varGetByte(1057) == 1) {
			_app->objectSetAccessibilityOff(kObject1013, 1, 1);
			onVisual(109);
		}
		break;

	case 1153:
	case 1154:
		if (_app->varGetByte(1041) == 1 && _app->varGetByte(90403) < 2)
			_app->soundPlay(1168 + rnd(2));
		break;

	case 2001:
		_app->rotationSetActive(10011);
		_app->objectPresentationHide(kObject1016);
		onVisual(202);
		break;

	case 2002:
		_app->objectSetAccessibilityOn(kObject1017, 0, 0);
		_app->rotationSetAlp(10031, 125.0f);
		_app->rotationSetBet(10031, 15.0f);
		_app->rotationSetActive(10031);
		// Fback to next case

	case 2003:
	case 2004:
		if (_app->varGetByte(1058) == 1) {
			_app->objectSetAccessibilityOff(kObject1017, 0, 0);
			onVisual(203);
		}
		break;

	case 2005:
	case 2006:
	case 2007:
	case 2008:
	case 2012:
	case 2014:
	case 2015:
		if (id == 2012) {
			_app->bagAdd(kObjectGlassTube);
			_app->bagAdd(kObjectGlassTube2);

			if (_app->varGetByte(90006) == 1)
				_app->soundPlay(2013);
		}
		// Fback to next case

	case 2016:
		_app->rotationSetAlp(10032, 157.0f);
		_app->rotationSetBet(10032, 15.0f);
		_app->rotationSetActive(10032);

		if (_app->varGetByte(1059) == 1) {
			_app->objectSetAccessibilityOff(kObject1019, 1, 1);
			_app->bagAdd(kObjectItemList);
			_app->soundPlay(2017);
		}
		break;

	case 2017:
		_app->playMovie("S00A03-3");
		_app->objectPresentationHide(kObject1019, 0);
		_app->objectPresentationHide(kObject1019, 1);
		_app->objectPresentationHide(kObject1019, 2);
		onVisual(205);
		break;

	case 2018:
		_app->rotationSetActive(10018);
		break;

	case 2019:
		_app->objectSetAccessibilityOn(kObject1021, 1, 1);
		// Fback to next case

	case 2020:
	case 2021:
	case 2022:
		_app->rotationSetAlp(100110, 300.0f);
		_app->rotationSetBet(100110, 18.0f);
		_app->rotationSetActive(100110);

		if (_app->varGetByte(1060) == 1) {
			_app->objectSetAccessibilityOff(kObject1021, 1, 1);
			_app->objectPresentationHide(kObject1021, 0);
			_app->objectPresentationHide(kObject1021, 2);
			_app->objectSetAccessibilityOn(kObject10043);
		}
		break;

	case 2028:
		_app->onCall(2089);
		onVisual(208);
		_app->rotationSetActive(10024);
		break;

	case 2032:
	case 2040:
		_app->rotationSetActive(10013);
		break;

	case 2033:
		_app->objectSetAccessibilityOn(kObject1039, 1, 1);
		// Fback to next case

	case 2034:
	case 2035:
		_app->rotationSetAlp(10013, 297.0f);
		_app->rotationSetBet(10013, 0.0f);
		_app->rotationSetActive(10013);

		if (_app->varGetByte(1062) == 1) {
			_app->objectSetAccessibilityOff(kObject1039, 1, 1);
			_app->objectSetAccessibilityOn(kObject1035, 0, 0);
			_app->varSetByte(1024, 1);
			_app->objectSetAccessibilityOn(kObjectBasket, 0, 0);
			_app->objectSetAccessibilityOn(kObjectBasket2, 0, 0);
			_app->objectSetAccessibilityOn(kObject10441, 0, 0);
		}
		break;

	case 2036:
	case 2037:
	case 2038:
	case 2039:
		_app->rotationSetAlp(10013, 297.0f);
		_app->rotationSetBet(10013, 0.0f);
		_app->rotationSetActive(10013);

		if (_app->varGetByte(1065) == 1) {
			_app->objectSetAccessibilityOff(kObject1039, 2, 2);
			_app->objectSetAccessibilityOn(kObject1035, 1, 1);
		}
		break;

	case 2041:
	case 2042:
	case 2043:
	case 2044:
		_app->rotationSetAlp(10013, 73.0f);
		_app->rotationSetBet(10013, 10.0f);
		_app->rotationSetActive(10013);

		if (_app->varGetByte(1063) == 1) {
			_app->objectSetAccessibilityOff(kObject1035, 1, 1);
			_app->objectSetAccessibilityOn(kObject1041, 0, 0);
		}
		break;

	case 2045:
	case 2046:
	case 2047:
	case 2048:
		_app->rotationSetAlp(10013, 130.0f);
		_app->rotationSetBet(10013, 5.0f);
		_app->rotationSetActive(10013);

		if (_app->varGetByte(1064) == 1) {
			_app->objectSetAccessibilityOff(kObject1041, 0, 0);
			_app->bagAdd(kObjectCommercialAdd);
			_app->soundPlay(1153 + rnd(2));
		}
		break;

	case 2049:
		_app->objectPresentationHide(kObject1045);
		_app->puzzleSetActive(kPuzzle1027);
		_app->soundPlay(2050);
		break;

	case 2050:
		_app->objectSetAccessibilityOn(kObject1039, 3, 3);
		_app->soundPlay(1168 + rnd(2));
		_app->rotationSetAlp(10013, 297.0f);
		_app->rotationSetBet(10013, 0.0f);
		_app->rotationSetActive(10013);
		break;

	case 2051:
		_app->bagAdd(kObjectFlask);
		// Fback to next case

	case 2052:
	case 2053:
		_app->rotationSetAlp(10013, 297.0f);
		_app->rotationSetBet(10013, 0.0f);
		_app->rotationSetActive(10013);

		if (_app->varGetByte(10651) == 1)
			_app->objectSetAccessibilityOff(kObject1039, 3, 3);
		break;

	case 2063:
		_app->objectSetAccessibilityOn(kObject1028, 1, 1);
		// Fback to next case

	case 2064:
	case 2065:
		_app->rotationSetAlp(10022, 175.0f);
		_app->rotationSetBet(10022, 10.0f);
		_app->rotationSetActive(10022);

		if (_app->varGetByte(1066) == 1) {
			_app->objectSetAccessibilityOff(kObject1028, 1, 1);
			_app->objectPresentationHide(kObject1028);
		}
		break;

	case 2066:
		_app->rotationSetActive(10015);
		_app->rotationSetMovabilityOff(10015);
		_app->soundPlay(2067);
		break;

	case 2067:
		if (_app->varGetByte(90006) == 2)
			_app->giveMoney(120);

		_app->playMovie("S00A0-20");
		_app->objectPresentationHide(kObject1031);
		_app->rotationSetMovabilityOn(10015);
		_app->bagRemove(kObjectItemListCompleted);
		_app->bagRemove(kObjectStones);
		_app->bagRemove(kObjectPureWine);
		_app->bagRemove(kObjectLamp2);
		_app->bagRemove(kObjectIncenseStick);
		_app->bagRemove(kObjectRose);
		_app->bagRemove(kObjectMuleToothPowder);
		_app->bagRemove(kObjectDropOfBlood);
		_app->bagRemove(kObjectApolloTear);
		_app->bagRemove(kObjectIbisFeather);
		onVisual(215);
		break;

	case 2076:
		_app->rotationSetActive(10018);
		break;

	case 3002:
		_app->rotationSetActive(100114);
		break;

	case 3003:
		_app->soundPlay(3004);
		break;

	case 3005:
		_app->rotationSetActive(10013);
		_app->objectSetAccessibilityOn(kObject1049, 0, 0);
		_app->bagAdd(kObjectRoses);
		break;

	case 3006:
		_app->rotationSetActive(10013);
		_app->objectSetAccessibilityOn(kObject1053, 0, 0);
		_app->objectSetAccessibilityOn(kObject1050, 0, 0);
		_app->objectSetAccessibilityOn(kObject1051, 0, 0);
		break;

	case 3008:
		_app->objectSetAccessibilityOn(kObject1050, 1, 1);
		// Fback to next case

	case 3009:
	case 3010:
		_app->rotationSetAlp(10013, 73.0f);
		_app->rotationSetBet(10013, 10.0f);
		_app->rotationSetActive(10013);

		if (_app->varGetByte(1067) == 1) {
			_app->objectSetAccessibilityOff(kObject1050, 1, 1);

			if (_app->varGetByte(1030) == 1
			 && _app->varGetByte(1031) == 1
			 && _app->varGetByte(1032) == 1
			 && _app->varGetByte(1033) == 1
			 && _app->varGetByte(1034) == 0) {
				_app->varSetByte(1034, 1);
				_app->puzzleSetActive(kPuzzle1040);
				_app->soundPlay(3012);
			}
		}
		break;

	case 3007:
	case 3011:
		_app->rotationSetActive(10013);

		if (_app->varGetByte(1030) == 1
		 && _app->varGetByte(1031) == 1
		 && _app->varGetByte(1067) == 1
		 && _app->varGetByte(1033) == 1
		 && _app->varGetByte(1034) == 0) {
			_app->varSetByte(1034, 1);
			_app->puzzleSetActive(kPuzzle1040);
			_app->soundPlay(3012);
		}
		break;

	case 3012:
		_app->playMovie("S00A01-4");
		_app->objectPresentationShow(kObject1055);
		_app->objectPresentationShow(kObject1054, 0);
		_app->objectPresentationShow(kObject1054, 1);
		_app->puzzleSetActive(kPuzzle1044);
		_app->soundPlay(3013);
		break;

	case 3013:
		_app->soundPlay(1168 + rnd(2));
		_app->soundPlay(3014);
		break;

	case 3014:
		_app->playMovie("S00A0-16");
		_app->fadeOut(15, Color(0, 0, 0), 0);
		_app->rotationSetMovabilityOn(10013);
		onVisual(303);
		_app->onCall(3039);
		break;

	case 3023:
	case 3024:
	case 3025:
		_app->objectSetAccessibilityOn(kObject1057, 1, 1);

		if (id == 3024)
			_app->bagAdd(kObjectPhiltre1);

		if (id == 3025)
			_app->bagAdd(kObjectPhiltre2);
		// Fback to next case

	case 3026:
		_app->rotationSetAlp(10043, 130.0f);
		_app->rotationSetBet(10043, 10.0f);
		_app->rotationSetActive(10043);

		if (_app->varGetByte(1068) == 1) {
			_app->objectSetAccessibilityOff(kObject1057, 1, 1);
			onVisual(307);
		}
		break;

	case 3028:
		_app->objectSetAccessibilityOn(kObject1059, 1, 1);
		// Fback to next case

	case 3029:
	case 3031:
		_app->rotationSetAlp(10046, 25.0f);
		_app->rotationSetBet(10046, 15.0f);
		_app->rotationSetActive(10046);

		if (_app->varGetByte(1069) == 1) {
			_app->objectSetAccessibilityOff(kObject1059, 1, 1);
			_app->objectSetAccessibilityOn(kObject1059, 2, 2);
		}
		break;

	case 3032:
	case 3033:
	case 3034:
	case 3035:
		_app->rotationSetAlp(10046, 25.0f);
		_app->rotationSetBet(10046, 15.0f);
		_app->rotationSetActive(10046);

		if (_app->varGetByte(1070) == 1) {
			_app->objectSetAccessibilityOff(kObject1059, 2, 2);
			onVisual(308);
		}
		break;

	case 3036:
		_app->objectSetAccessibilityOn(kObject10591, 1, 1);
		// Fback to next case

	case 3037:
	case 3040:
		_app->rotationSetAlp(100413, 30.0f);
		_app->rotationSetBet(100413, 15.0f);
		_app->rotationSetActive(100413);

		if (_app->varGetByte(1071) == 1) {
			_app->objectSetAccessibilityOff(kObject10591, 1, 1);

			if (id == 3037)
				_app->objectSetAccessibilityOn(kObject1059, 0, 0);
		}
		break;

	case 3038:
		_app->objectSetAccessibilityOff(kObject1059, 0, 0);
		// Fback to next case

	case 3030:
		_app->exitToMenu(kMenuAction6);
		break;

	case 3041:
		_app->objectSetAccessibilityOn(kObject1061, 1, 1);
		// Fback to next case

	case 3042:
	case 3043:
	case 3044:
		_app->rotationSetAlp(10043, 10.0f);
		_app->rotationSetBet(10043, 15.0f);
		_app->rotationSetActive(10043);

		if (_app->varGetByte(1072) == 1) {
			_app->objectSetAccessibilityOff(kObject1061, 1, 1);
			_app->playMovie("S00A04-2");
			_app->objectPresentationHide(kObject1061, 0);
			_app->objectPresentationHide(kObject1061, 2);
			_app->objectPresentationHide(kObject1061, 3);
			_app->objectPresentationShow(kObject1061, 1);
			_app->objectSetAccessibilityOn(kObject1062, 0, 0);
		}
		break;

	case 3045:
		_app->objectSetAccessibilityOn(kObject1062, 1, 1);
		// Fback to next case

	case 3046:
	case 3047:
		_app->rotationSetAlp(10043, 350.0f);
		_app->rotationSetBet(10043, 15.0f);
		_app->rotationSetActive(10043);

		if (_app->varGetByte(1073) == 1) {
			_app->objectSetAccessibilityOff(kObject1062, 1, 1);
			_app->onCall(3091);
		}
		break;

	case 4008:
		_app->objectSetAccessibilityOn(kObject1064, 0, 0);
		break;

	case 4009:
		_app->objectSetAccessibilityOn(kObject1064, 1, 1);
		_app->rotationSetAlp(10012, 100.0f);
		_app->rotationSetBet(10012, 15.0f);
		_app->rotationSetActive(10012);
		break;

	case 4011:
		_app->objectSetAccessibilityOff(kObject1064, 1, 1);
		_app->rotationSetAlp(10012, 100.0f);
		_app->rotationSetBet(10012, 15.0f);
		_app->rotationSetActive(10012);
		_app->rotationSetMovabilityOff(10012);
		_app->soundPlay(4012);
		break;

	case 4012:
		_app->soundPlay(4014);
		break;

	case 4013:
		_app->objectSetAccessibilityOff(kObject1064, 1, 1);
		_app->rotationSetAlp(10012, 100.0f);
		_app->rotationSetBet(10012, 15.0f);
		_app->rotationSetActive(10012);
		_app->rotationSetMovabilityOff(10012);
		_app->soundPlay(4014);
		break;

	case 4014:
		_app->rotationSetMovabilityOn(10012);
		_app->giveMoney(150);
		break;

	case 4015:
		_app->objectSetAccessibilityOn(kObject1066, 1, 1);
		_app->soundPlay(1153 + rnd(2));
		// Fback to next case

	case 4016:
	case 4017:
	case 4018:
	case 4019:
	case 4020:
	case 4021:
		_app->rotationSetAlp(10013, 297.0f);
		_app->rotationSetBet(10013, 0.0f);
		_app->rotationSetActive(10013);

		if (_app->varGetByte(1075) == 1)
			_app->objectSetAccessibilityOff(kObject1066, 1, 1);
		break;

	case 4031:
		_app->rotationSetAlp(10053, 80.0f);
		_app->rotationSetBet(10053, 20.0f);
		_app->rotationSetActive(10053);
		break;

	case 4032:
		_app->objectSetAccessibilityOn(kObject1068, 1, 1);
		// Fback to next case

	case 4033:
	case 4034:
	case 4035:
	case 4036:
	case 4037:
		_app->bagAdd(kObjectPapyrus);
		// Fback to next case

	case 4038:
	case 4039:
	case 4040:
	case 4041:
		_app->rotationSetAlp(10053, 80.0f);
		_app->rotationSetBet(10053, 20.0f);
		_app->rotationSetActive(10053);

		if (_app->varGetByte(1076) == 1)
			_app->objectSetAccessibilityOff(kObject1068, 1, 1);
		break;

	case 4042:
		_app->objectSetAccessibilityOn(kObject1077, 1, 1);
		// Fback to next case

	case 4043:
	case 4044:
	case 4045:
	case 4046:
	case 4047:
		_app->rotationSetAlp(10052, 82.0f);
		_app->rotationSetBet(10052, 10.0f);
		_app->rotationSetActive(10052);

		if (_app->varGetByte(1077) == 1) {
			_app->objectSetAccessibilityOff(kObject1077, 1, 1);
			_app->playMovie("S00A05-7");
			_app->objectPresentationHide(kObject1077, 0);
			_app->objectPresentationHide(kObject1077, 1);
			_app->objectPresentationHide(kObject1077, 2);
			_app->onCall(4069);
		}
		break;

	case 4108:
		_app->objectSetAccessibilityOn(kObject1081, 1, 1);
		// Fback to next case

	case 4109:
	case 4110:
	case 4111:
		_app->rotationSetAlp(1001202, 50.0f);
		_app->rotationSetBet(1001202, 5.0f);
		_app->rotationSetActive(1001202);

		if (_app->varGetByte(1078) == 1) {
			_app->objectSetAccessibilityOff(kObject1081, 1, 1);
			_app->objectSetAccessibilityOn(kObject1081, 2, 2);
		}
		break;

	case 4113:
		if (_app->varGetByte(1078) == 1)
			onVisual(419);
		break;

	case 4115:
		_app->rotationSetMovabilityOn(1005402);
		_app->fadeOut(15, Color(0, 0, 0), 0);
		_app->onCall(421);
		break;

	case 4116:
		_app->exitToMenu(kMenuAction11);
		break;

	case 4118:
		_app->objectSetAccessibilityOn(kObject1093, 1, 1);
		_app->rotationSetAlp(10011103, 90.0f);
		_app->rotationSetActive(10011103);
		break;

	case 4119:
		_app->objectSetAccessibilityOn(kObject1093, 2, 2);
		_app->rotationSetAlp(10011103, 90.0f);
		_app->rotationSetActive(10011103);
		break;

	case 4120:
		_app->soundPlay(4121);
		break;

	case 4123:
		_app->soundPlay(4124);
		break;

	case 4122:
	case 4124:
		_app->playMovie("S00A0-14");
		// Fback to next case

	case 4121:
		onVisual(421);
		_app->fadeOut(15, Color(0, 0, 0), 0);
		_app->exitToMenu(kMenuAction99);
		break;

	case 20071:
	case 20081:
	case 20091:
	case 20092:
	case 20121:
	case 20141:
	case 20151:
		if (id == 20121) {
			_app->bagAdd(kObjectGlassTube);
			_app->bagAdd(kObjectGlassTube2);

			if (_app->varGetByte(90006) == 1)
				_app->soundPlay(2013);
		}
		// Fback to next case

	case 20061:
	case 20161:
		_app->rotationSetAlp(10032, 157.0f);
		_app->rotationSetBet(10032, 15.0f);
		_app->rotationSetActive(10032);

		if (_app->varGetByte(1059) == 1) {
			_app->objectSetAccessibilityOff(kObject1019, 1, 1);
			_app->bagAdd(kObjectItemList);
			_app->soundPlay(2017);
		}
		break;
	}
}

void Zone1Pompeii::onUpdateBag(const Common::Point &/*point*/) {
	if (_hideBox) {
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;
	}
}

void Zone1Pompeii::onBagClickedObject(ObjectId objectId) {
	if (objectId == kObjectNightingale) {
		_app->cursorDelete();
		_app->soundPlay(4106);

		if (_app->varGetByte(90416) == 1
		 && _app->getCurrentRotationId() == 10011002) {
			 _app->timerStopAll();
			 _app->timerStart(kTimer9, 2000);
		}
	}
}

void Zone1Pompeii::onUpdateBefore(Id movabilityFrom, Id movabilityTo, uint32 /*movabilityIndex*/, Id /*target*/, const Common::Point &/*point*/) {
	Common::Point mouse = g_system->getEventManager()->getMousePos() - Common::Point(20, 16);

	switch (movabilityFrom) {
	default:
		break;

	case 1009:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 1036, mouse);
			_hideBox = true;
		}
		break;

	case 1011:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 1039, mouse);
			_hideBox = true;
		}
		break;

	case 1012:
		if (movabilityTo == 2 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 1047, mouse);
			_hideBox = true;
		}
		break;

	case 1013:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 1065, mouse);
			_hideBox = true;
		}
		break;

	case 1017:
		if (movabilityTo == 0 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 2002, mouse);
			_hideBox = true;
		}
		break;

	case 1019:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {

			if (_app->varGetByte(90006) == 1)
				_app->visualBoxSetParameters(6, kPuzzleMenu, 20052, mouse);

			if (_app->varGetByte(90006) == 2)
				_app->visualBoxSetParameters(6, kPuzzleMenu, 20051, mouse);

			_hideBox = true;
		}
		break;

	case 1021:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 2019, mouse);
			_hideBox = true;
		}
		break;

	case 1028:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 2063, mouse);
			_hideBox = true;
		}
		break;

	case 1035:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 20401, mouse);
			_hideBox = true;
		}
		break;

	case 1039:
		if (!_hideBox && !_app->bagHasClickedObject()) {

			switch (movabilityTo) {
			default:
				break;

			case 1:
				_app->visualBoxSetParameters(6, kPuzzleMenu, 2033, mouse);
				_hideBox = true;
				break;

			case 2:
				_app->visualBoxSetParameters(6, kPuzzleMenu, 20331, mouse);
				_hideBox = true;
				break;

			case 3:
				_app->visualBoxSetParameters(6, kPuzzleMenu, 2050, mouse);
				_hideBox = true;
				break;
			}
		}
		break;

	case 1041:
		if (movabilityTo == 0 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 20402, mouse);
			_hideBox = true;
		}
		break;

	case 1050:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 3008, mouse);
			_hideBox = true;
		}
		break;

	case 1057:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			if (_app->varGetByte(1035) == 1) {
				_app->visualBoxSetParameters(6, kPuzzleMenu, 3023, mouse);
				_hideBox = true;
			}
		}
		break;

	case 1059:
		if (!_hideBox && !_app->bagHasClickedObject()) {

			if (movabilityTo == 1) {
				_app->visualBoxSetParameters(6, kPuzzleMenu, 3028, mouse);
				_hideBox = true;
			}

			if (movabilityTo == 2) {
				_app->visualBoxSetParameters(6, kPuzzleMenu, 30310, mouse);
				_hideBox = true;
			}
		}
		break;

	case 1061:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 3041, mouse);
			_hideBox = true;
		}
		break;

	case 1062:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 2033, mouse);
			_hideBox = true;
		}
		break;

	case 1064:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 4009, mouse);
			_hideBox = true;
		}
		break;

	case 1066:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 4015, mouse);
			_hideBox = true;
		}
		break;

	case 1068:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 4032, mouse);
			_hideBox = true;
		}
		break;

	case 1077:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 4042, mouse);
			_hideBox = true;
		}
		break;

	case 1081:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 4108, mouse);
			_hideBox = true;
		}
		break;

	case 10071:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 1031, mouse);
			_hideBox = true;
		}
		break;

	case 10591:
		if (movabilityTo == 1 && !_hideBox && !_app->bagHasClickedObject()) {
			_app->visualBoxSetParameters(6, kPuzzleMenu, 3036, mouse);
			_hideBox = true;
		}
		break;
	}
}

void Zone1Pompeii::onUpdateAfter(Id /*movabilityFrom*/, Id /*movabilityTo*/, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType /*movabilityType*/, const Common::Point &/*point*/) {
	if (_hideBox) {
		_app->visualBoxHide(6, kPuzzleMenu);
		_hideBox = false;
	}
}

void Zone1Pompeii::onBeforeRide(Id movabilityFrom, Id movabilityTo, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType movabilityType) {
	if (movabilityType != kMovabilityRotationToRotation)
		return;

	switch (movabilityFrom) {
	default:
		break;

	case 10032:
		if (movabilityTo == 100515)
			_app->onCall(30);
		break;

	case 10012:
		if (movabilityTo == 10013 && _app->varGetByte(90001) == 3 && !_app->varGetByte(90302)) {
			_app->timerStop(kTimer3);
			_app->varSetByte(1029, 1);
			_app->playMovie("S00A01-5");
			_app->objectPresentationShow(kObject1048, 0);
			_app->rotationSetActive(10012);
			onVisual(302);
		}
		break;

	case 100515:
		if (movabilityTo == 10032)
			_app->onCall(20);
		break;

	case 1001102:
		if (movabilityTo == 10011302 && _app->varGetByte(90416) >= 2 && _app->varGetByte(90419) == 1)
			_app->exitToMenu(kMenuAction12);
		break;

	case 10011002:
		if (_app->varGetByte(90416) == 2) {
			_app->varSetByte(90416, 3);
			_app->timerStop(kTimer6);
			_app->rotationSet3DSoundOn(10011002, 1191);
			_app->timerStart(kTimer7, 3000);
			_app->varSetByte(1097, 1);

			for (uint32 i = 1; i <= 6; i++)
				_app->objectPresentationShow(kObject1080, i);
		}
		break;
	}
}

void Zone1Pompeii::onAfterRide(Id movabilityFrom, Id movabilityTo, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType movabilityType) {
	switch (movabilityTo) {
	default:
		break;

	case 1001202:
		if (movabilityFrom == 1001102 && _app->varGetByte(1097) == 8) {
			_app->rotationSetMovabilityOff(1001102, 0, 0);
			_app->rotationSetMovabilityOff(1001102, 1, 1);
			_app->timerStop(kTimer7);
			_app->rotationSet3DSoundOff(1001202, 1191);
			_app->soundSetVolume(900001, 95);
			_app->soundPlay(900001);
			_app->varSetByte(1097, 9);
			_app->timerStart(kTimer8, 4000);
		}
		break;

	case 1001402:
		if (movabilityFrom == 10011402 && _app->varGetByte(1097) == 6) {
			_app->timerStop(kTimer7);
			_app->rotationSet3DSoundOff(1001402, 1191);
			_app->objectPresentationHide(kObject1080, 2);
			_app->rotationSet3DSoundOn(10011402, 1191);
			_app->timerStart(kTimer7, 3000);
			_app->varSetByte(1097, 7);
		}
		break;

	case 1001502:
		if (movabilityFrom == 1001402 && _app->varGetByte(1097) == 5) {
			_app->timerStop(kTimer7);
			_app->rotationSet3DSoundOff(1001502, 1191);
			_app->objectPresentationHide(kObject1080, 3);
			_app->rotationSet3DSoundOn(1001402, 1191);
			_app->timerStart(kTimer7, 3000);
			_app->varSetByte(1097, 6);
		}
		break;

	case 1001602:
		if (movabilityFrom == 1001502 && _app->varGetByte(1097) == 4) {
			_app->timerStop(kTimer7);
			_app->rotationSet3DSoundOff(1001602, 1191);
			_app->objectPresentationHide(kObject1080, 4);
			_app->rotationSet3DSoundOn(1001502, 1191);
			_app->timerStart(kTimer7, 3000);
			_app->varSetByte(1097, 5);
		}
		break;

	case 1001702:
		if (movabilityFrom == 1001602 && _app->varGetByte(1097) == 3) {
			_app->timerStop(kTimer7);
			_app->rotationSet3DSoundOff(1001702, 1191);
			_app->objectPresentationHide(kObject1080, 5);
			_app->rotationSet3DSoundOn(1001602, 1191);
			_app->timerStart(kTimer7, 3000);
			_app->varSetByte(1097, 4);
		}
		break;

	case 1001902:
		if (movabilityFrom == 1001702 && _app->varGetByte(1097) == 2) {
			_app->timerStop(kTimer7);
			_app->rotationSet3DSoundOff(1001902, 1191);
			_app->objectPresentationHide(kObject1080, 6);
			_app->rotationSet3DSoundOn(1001702, 1191);
			_app->timerStart(kTimer7, 3000);
			_app->varSetByte(1097, 3);
		}
		break;

	case 10011002:
		if (movabilityFrom == 1001902 && _app->varGetByte(1097) == 1) {
			_app->timerStop(kTimer7);
			_app->rotationSet3DSoundOff(10011002, 1191);
			_app->rotationSet3DSoundOn(1001902, 1191);
			_app->timerStart(kTimer7, 3000);
			_app->varSetByte(1097, 2);
		}

		if (_app->varGetByte(90416) == 3) {
			_app->varSetByte(90416, 4);
			_app->playMovie("S00A0-15");
			_app->rotationSetActive(10011002);
			_app->objectPresentationShow(kObjectStone2, 0);
			_app->objectSetAccessibilityOn(kObjectStone2);
		}
		break;

	case 10011402:
		if (movabilityFrom == 1001202 && _app->varGetByte(1097) == 7) {
			_app->timerStop(kTimer7);
			_app->rotationSet3DSoundOff(10011402, 1191);
			_app->objectPresentationHide(kObject1080, 1);
			_app->rotationSet3DSoundOn(1001202, 1191);
			_app->timerStart(kTimer7, 3000);
			_app->varSetByte(1097, 8);
		}
		break;
	}

	if (_app->varGetByte(90104) == 2) {
		_app->varSetByte(90104, 3);
		_app->onCall(1049);
	}

	if (_app->varGetByte(90109) == 2) {
		_app->varSetByte(90109, 3);
		_app->onCall(1099);
	}

	if (_app->varGetByte(90203) == 2) {
		_app->varSetByte(90203, 3);
		_app->onCall(2039);
	}

	if (_app->bagHas(kObjectItemListCompleted) && !_app->varGetByte(1099)) {
		_app->varSetByte(1099, 1);
		_app->onCall(215);
	}

	if (_app->varGetByte(90301) == 2) {
		_app->varSetByte(90301, 3);
		_app->onCall(3019);
	}

	if (_app->varGetByte(90307) == 2) {
		_app->varSetByte(90307, 3);
		_app->onCall(3079);
	}

	if (_app->varGetByte(90402) == 2) {
		_app->varSetByte(90402, 3);
		_app->onCall(4029);
	}

	if (_app->varGetByte(90405) == 2) {
		_app->varSetByte(90405, 3);
		_app->onCall(4039);
		_app->onCall(4059);
	}

	if (_app->varGetByte(90410) >= 2 && _app->varGetByte(90411) >= 2 && !_app->varGetByte(1098)) {
		_app->varSetByte(1098, 1);
		_app->onCall(413);
	}

	onVisual(204);
	onVisual(206);
	onVisual(208);
	onVisual(211);
	onVisual(214);
	onVisual(402);
	onVisual(403);
	onVisual(405);
	onVisual(406);
	onVisual(409);
	onVisual(420);

	if (movabilityType != kMovabilityRotationToRotation)
		return;

	switch (movabilityFrom) {
	default:
		break;

	case 10011:
		if (movabilityTo == 10012 && !_app->varGetByte(90104) && _app->varGetByte(90103) >= 2) {
			_app->objectSetAccessibilityOff(kObject1001, 0, 0);
			_app->objectSetAccessibilityOff(kObject1005, 0, 0);
		}
		break;

	case 10012:
		if (movabilityTo == 10011 && !_app->varGetByte(90104) && _app->varGetByte(90103) >= 2) {
			_app->soundPlay(1025);
		} else if (movabilityTo == 100114 && !_app->varGetByte(1029) && _app->varGetByte(90001) == 3 && !_app->varGetByte(90302)) {
			_app->timerStart(kTimer3, 5000);
			_app->soundPlay(3003);
		}

		if (_app->varGetByte(90001) == 4 && !_app->varGetByte(90402) && !_app->varGetByte(1040))
			_app->soundPlay(4008);
		break;

	case 10023:
		if (_app->varGetByte(90001) == 1 && _app->varGetByte(90109) < 2 && _app->varGetByte(90107) >= 2)
			_app->soundPlay(1064);
		break;

	case 10031:
		if (movabilityTo == 100113 && !_app->varGetByte(1008) && _app->varGetByte(90001) == 2 && !_app->varGetByte(90203))
			_app->onCall(203);
		break;

	case 10013:
		if (movabilityTo == 10012 && _app->varGetByte(90001) == 3 && !_app->varGetByte(90303) && _app->varGetByte(1029) == 1) {
			_app->rotationSetMovabilityOff(10013);
			_app->varSetByte(90303, 1);
			_app->puzzleSetActive(kPuzzle1027);
			_app->soundPlay(3005);
		}
		break;

	case 10043:
		if (_app->varGetByte(90001) == 3 && !_app->varGetByte(90307) && _app->varGetByte(90305) >= 2)
			_app->soundPlay(3022);
		break;

	case 100515:
		if (_app->varGetByte(90001) == 4 && _app->varGetByte(90406) && _app->varGetByte(90408) >= 2 && !_app->varGetByte(90409)) {
			_app->varSetByte(90409, 1);
			_app->onCall(409);
		}
		break;

	case 1001202:
		if (movabilityTo == 1001102 && _app->varGetByte(90001) == 4 && _app->varGetByte(90416) >= 2 && _app->varGetByte(90419) == 1) {
			_app->varSetByte(90419, 2);
			_app->playMovie("S00A0-12");
			_app->puzzleSetActive(kPuzzle1060);
			_app->soundPlay(4108);
			_app->varSetByte(1050, 1);
		}
		break;

	case 1005402:
		if (_app->varGetByte(90001) == 4 && _app->varGetByte(90419) >= 2 && !_app->varGetByte(90420)) {
			_app->varSetByte(90420, 1);
			_app->soundPlay(4114);
		}
		break;

	case 10011103:
		if (movabilityTo == 10011203) {
			if (_app->varGetByte(90001) == 4 && !_app->varGetByte(1052) && !_app->varGetByte(1052))
				_app->objectSetAccessibilityOn(kObject1093, 0, 0);

			if (_app->varGetByte(90001) == 4 && _app->varGetByte(1052) == 1 && !_app->varGetByte(1053))
				_app->objectSetAccessibilityOn(kObject1093, 0, 0);
		}
		break;
	}
}

void Zone1Pompeii::onVisual(int x) {
	switch (x) {
	default:
		break;

	case 0:
		_app->objectPresentationPauseAnimation(kObject1001, 0);
		_app->objectPresentationPauseAnimation(kObject1001, 1);
		_app->objectPresentationPauseAnimation(kObject1001, 3);
		_app->objectPresentationPauseAnimation(kObject1006, 0);
		_app->objectPresentationPauseAnimation(kObject10071, 0);
		_app->objectPresentationPauseAnimation(kObject10071, 1);
		_app->objectPresentationPauseAnimation(kObject1009, 0);
		_app->objectPresentationPauseAnimation(kObject1009, 1);
		_app->objectPresentationPauseAnimation(kObject1011, 0);
		_app->objectPresentationPauseAnimation(kObject1011, 1);
		_app->objectPresentationPauseAnimation(kObject1013, 0);
		_app->objectPresentationPauseAnimation(kObject1013, 1);
		_app->objectPresentationPauseAnimation(kObject1016, 0);
		_app->objectPresentationPauseAnimation(kObject1017, 0);
		_app->objectPresentationPauseAnimation(kObject1017, 1);
		_app->objectPresentationPauseAnimation(kObject1019, 0);
		_app->objectPresentationPauseAnimation(kObject1019, 1);
		_app->objectPresentationPauseAnimation(kObject1020, 0);
		_app->objectPresentationPauseAnimation(kObject1020, 1);
		_app->objectPresentationPauseAnimation(kObject1021, 0);
		_app->objectPresentationPauseAnimation(kObject1021, 2);
		_app->objectPresentationPauseAnimation(kObject1023, 0);
		_app->objectPresentationPauseAnimation(kObject1023, 1);
		_app->objectPresentationPauseAnimation(kObject1039, 0);
		_app->objectPresentationPauseAnimation(kObject1039, 1);
		_app->objectPresentationPauseAnimation(kObject1040, 0);
		_app->objectPresentationPauseAnimation(kObject1035, 0);
		_app->objectPresentationPauseAnimation(kObject1041, 0);
		_app->objectPresentationPauseAnimation(kObject1045, 0);
		_app->objectPresentationPauseAnimation(kObject1028, 0);
		_app->objectPresentationPauseAnimation(kObject1028, 1);
		_app->objectPresentationPauseAnimation(kObject1031, 0);
		_app->objectPresentationPauseAnimation(kObject1031, 1);
		_app->objectPresentationPauseAnimation(kObject1033, 0);
		_app->objectPresentationPauseAnimation(kObject1033, 1);
		_app->objectPresentationPauseAnimation(kObject1046, 0);
		_app->objectPresentationPauseAnimation(kObject1046, 1);
		_app->objectPresentationPauseAnimation(kObject1050, 0);
		_app->objectPresentationPauseAnimation(kObject1051, 0);
		_app->objectPresentationPauseAnimation(kObject1053, 0);
		_app->objectPresentationPauseAnimation(kObject1054, 0);
		_app->objectPresentationPauseAnimation(kObject1055, 0);
		_app->objectPresentationPauseAnimation(kObject1049, 0);
		_app->objectPresentationPauseAnimation(kObject1057, 0);
		_app->objectPresentationPauseAnimation(kObject1057, 1);
		_app->objectPresentationPauseAnimation(kObject1059, 0);
		_app->objectPresentationPauseAnimation(kObject1059, 1);
		_app->objectPresentationPauseAnimation(kObject10591, 0);
		_app->objectPresentationPauseAnimation(kObject10591, 1);
		_app->objectPresentationPauseAnimation(kObject1062, 0);
		_app->objectPresentationPauseAnimation(kObject1062, 1);
		_app->objectPresentationPauseAnimation(kObject1061, 2);
		_app->objectPresentationPauseAnimation(kObject1064, 0);
		_app->objectPresentationPauseAnimation(kObject1066, 0);
		_app->objectPresentationPauseAnimation(kObject1067, 0);
		_app->objectPresentationPauseAnimation(kObject1068, 0);
		_app->objectPresentationPauseAnimation(kObject1068, 1);
		_app->objectPresentationPauseAnimation(kObject1077, 0);
		_app->objectPresentationPauseAnimation(kObject1077, 1);
		_app->objectPresentationPauseAnimation(kObject1079, 0);
		_app->objectPresentationPauseAnimation(kObject1081, 0);
		_app->objectPresentationPauseAnimation(kObject1081, 2);
		_app->objectPresentationPauseAnimation(kObject1084, 0);
		_app->objectPresentationPauseAnimation(kObject1085, 0);
		_app->objectPresentationPauseAnimation(kObject1093, 0);
		_app->objectPresentationPauseAnimation(kObject1001, 2);
		_app->objectPresentationPauseAnimation(kObject1001, 4);
		_app->objectPresentationPauseAnimation(kObject10071, 2);
		_app->objectPresentationPauseAnimation(kObject1010, 0);
		_app->objectPresentationPauseAnimation(kObject1011, 2);
		_app->objectPresentationPauseAnimation(kObject1013, 2);
		_app->objectPresentationPauseAnimation(kObject1016, 1);
		_app->objectPresentationPauseAnimation(kObject1019, 2);
		_app->objectPresentationPauseAnimation(kObject1053, 1);
		_app->objectPresentationPauseAnimation(kObject1054, 1);
		_app->objectPresentationPauseAnimation(kObject1057, 2);
		_app->objectPresentationPauseAnimation(kObject1059, 2);
		_app->objectPresentationPauseAnimation(kObject1061, 0);
		_app->objectPresentationPauseAnimation(kObject1061, 1);
		_app->objectPresentationPauseAnimation(kObject1061, 3);
		_app->objectPresentationPauseAnimation(kObject1064, 1);
		_app->objectPresentationPauseAnimation(kObject1066, 1);
		_app->objectPresentationPauseAnimation(kObject1068, 2);
		_app->objectPresentationPauseAnimation(kObject1077, 2);
		_app->objectPresentationPauseAnimation(kObject1078, 0);
		_app->objectPresentationPauseAnimation(kObject1093, 1);
		break;

	case 9:
		_app->objectPresentationUnpauseAnimation(kObject1001, 0);
		_app->objectPresentationUnpauseAnimation(kObject1001, 1);
		_app->objectPresentationUnpauseAnimation(kObject1001, 3);
		_app->objectPresentationUnpauseAnimation(kObject1006, 0);
		_app->objectPresentationUnpauseAnimation(kObject10071, 0);
		_app->objectPresentationUnpauseAnimation(kObject10071, 1);
		_app->objectPresentationUnpauseAnimation(kObject1009, 0);
		_app->objectPresentationUnpauseAnimation(kObject1009, 1);
		_app->objectPresentationUnpauseAnimation(kObject1011, 0);
		_app->objectPresentationUnpauseAnimation(kObject1011, 1);
		_app->objectPresentationUnpauseAnimation(kObject1013, 0);
		_app->objectPresentationUnpauseAnimation(kObject1013, 1);
		_app->objectPresentationUnpauseAnimation(kObject1016, 0);
		_app->objectPresentationUnpauseAnimation(kObject1017, 0);
		_app->objectPresentationUnpauseAnimation(kObject1017, 1);
		_app->objectPresentationUnpauseAnimation(kObject1019, 0);
		_app->objectPresentationUnpauseAnimation(kObject1019, 1);
		_app->objectPresentationUnpauseAnimation(kObject1020, 0);
		_app->objectPresentationUnpauseAnimation(kObject1020, 1);
		_app->objectPresentationUnpauseAnimation(kObject1021, 0);
		_app->objectPresentationUnpauseAnimation(kObject1021, 2);
		_app->objectPresentationUnpauseAnimation(kObject1023, 0);
		_app->objectPresentationUnpauseAnimation(kObject1023, 1);
		_app->objectPresentationUnpauseAnimation(kObject1039, 0);
		_app->objectPresentationUnpauseAnimation(kObject1039, 1);
		_app->objectPresentationUnpauseAnimation(kObject1040, 0);
		_app->objectPresentationUnpauseAnimation(kObject1035, 0);
		_app->objectPresentationUnpauseAnimation(kObject1041, 0);
		_app->objectPresentationUnpauseAnimation(kObject1045, 0);
		_app->objectPresentationUnpauseAnimation(kObject1028, 0);
		_app->objectPresentationUnpauseAnimation(kObject1028, 1);
		_app->objectPresentationUnpauseAnimation(kObject1031, 0);
		_app->objectPresentationUnpauseAnimation(kObject1031, 1);
		_app->objectPresentationUnpauseAnimation(kObject1033, 0);
		_app->objectPresentationUnpauseAnimation(kObject1033, 1);
		_app->objectPresentationUnpauseAnimation(kObject1046, 0);
		_app->objectPresentationUnpauseAnimation(kObject1046, 1);
		_app->objectPresentationUnpauseAnimation(kObject1050, 0);
		_app->objectPresentationUnpauseAnimation(kObject1051, 0);
		_app->objectPresentationUnpauseAnimation(kObject1053, 0);
		_app->objectPresentationUnpauseAnimation(kObject1054, 0);
		_app->objectPresentationUnpauseAnimation(kObject1055, 0);
		_app->objectPresentationUnpauseAnimation(kObject1049, 0);
		_app->objectPresentationUnpauseAnimation(kObject1057, 0);
		_app->objectPresentationUnpauseAnimation(kObject1057, 1);
		_app->objectPresentationUnpauseAnimation(kObject1059, 0);
		_app->objectPresentationUnpauseAnimation(kObject1059, 1);
		_app->objectPresentationUnpauseAnimation(kObject10591, 0);
		_app->objectPresentationUnpauseAnimation(kObject10591, 1);
		_app->objectPresentationUnpauseAnimation(kObject1062, 0);
		_app->objectPresentationUnpauseAnimation(kObject1062, 1);
		_app->objectPresentationUnpauseAnimation(kObject1061, 2);
		_app->objectPresentationUnpauseAnimation(kObject1064, 0);
		_app->objectPresentationUnpauseAnimation(kObject1066, 0);
		_app->objectPresentationUnpauseAnimation(kObject1067, 0);
		_app->objectPresentationUnpauseAnimation(kObject1068, 0);
		_app->objectPresentationUnpauseAnimation(kObject1068, 1);
		_app->objectPresentationUnpauseAnimation(kObject1077, 0);
		_app->objectPresentationUnpauseAnimation(kObject1077, 1);
		_app->objectPresentationUnpauseAnimation(kObject1079, 0);
		_app->objectPresentationUnpauseAnimation(kObject1081, 0);
		_app->objectPresentationUnpauseAnimation(kObject1081, 2);
		_app->objectPresentationUnpauseAnimation(kObject1084, 0);
		_app->objectPresentationUnpauseAnimation(kObject1085, 0);
		_app->objectPresentationUnpauseAnimation(kObject1093, 0);
		_app->objectPresentationUnpauseAnimation(kObject1001, 2);
		_app->objectPresentationUnpauseAnimation(kObject1001, 4);
		_app->objectPresentationUnpauseAnimation(kObject10071, 2);
		_app->objectPresentationUnpauseAnimation(kObject1010, 0);
		_app->objectPresentationUnpauseAnimation(kObject1011, 2);
		_app->objectPresentationUnpauseAnimation(kObject1013, 2);
		_app->objectPresentationUnpauseAnimation(kObject1016, 1);
		_app->objectPresentationUnpauseAnimation(kObject1019, 2);
		_app->objectPresentationUnpauseAnimation(kObject1053, 1);
		_app->objectPresentationUnpauseAnimation(kObject1054, 1);
		_app->objectPresentationUnpauseAnimation(kObject1057, 2);
		_app->objectPresentationUnpauseAnimation(kObject1059, 2);
		_app->objectPresentationUnpauseAnimation(kObject1061, 0);
		_app->objectPresentationUnpauseAnimation(kObject1061, 1);
		_app->objectPresentationUnpauseAnimation(kObject1061, 3);
		_app->objectPresentationUnpauseAnimation(kObject1064, 1);
		_app->objectPresentationUnpauseAnimation(kObject1066, 1);
		_app->objectPresentationUnpauseAnimation(kObject1068, 2);
		_app->objectPresentationUnpauseAnimation(kObject1077, 2);
		_app->objectPresentationUnpauseAnimation(kObject1078, 0);
		_app->objectPresentationUnpauseAnimation(kObject1093, 1);
		break;

	case 104:
		if (_app->varGetByte(90104) < 2) {
			_app->bagAdd(kObjectDates);
			_app->varSetByte(90104, 2);
			_app->onCall(105);
			_app->onCall(107);
		}
		break;

	case 105:
		if (_app->varGetByte(90105) < 2 && _app->varGetByte(1055) == 1) {
			_app->varSetByte(90105, 2);
			_app->onCall(106);
		}
		break;

	case 106:
		if (_app->varGetByte(90106) < 2 && _app->varGetByte(1056) == 1) {
			_app->varSetByte(90106, 2);
			_app->onCall(108);
		}
		break;

	case 108:
		if (_app->varGetByte(90108) < 2) {
			_app->varSetByte(90108, 2);
			_app->onCall(1089);
		}
		break;

	case 109:
		if (_app->varGetByte(90109) < 2 && _app->varGetByte(1057) == 1) {
			_app->varSetByte(90109, 2);
			_app->onCall(110);
		}
		break;

	case 202:
		if (_app->varGetByte(90202) < 2)
			_app->varSetByte(90202, 2);
		break;

	case 203:
		if (_app->varGetByte(90203) < 2 && _app->varGetByte(1058) == 1) {
			_app->varSetByte(90203, 2);
			_app->onCall(204);
		}
		break;

	case 204:
		if (_app->varGetByte(90204) < 2 && _app->varGetByte(1009) == 1) {
			_app->varSetByte(90204, 2);
			_app->onCall(205);
		}
		break;

	case 205:
		if (_app->varGetByte(90205) < 2 && _app->varGetByte(1059) == 1) {
			_app->varSetByte(90205, 2);
			_app->onCall(206);
			_app->onCall(207);
			_app->onCall(208);
			_app->onCall(209);
			_app->onCall(210);
			_app->onCall(211);
			_app->onCall(212);
			_app->onCall(213);
			_app->onCall(214);
		}
		break;

	case 206:
		if (_app->varGetByte(90206) < 2 && _app->varGetByte(1060) == 1)
			_app->varSetByte(90206, 2);
		break;

	case 208:
		if (_app->varGetByte(90208) < 2 && _app->varGetByte(1013) == 1) {
			_app->varSetByte(90208, 2);
			_app->objectSetAccessibilityOn(kObject10047);
		}
		break;

	case 210:
		if (_app->varGetByte(90210) < 2 && _app->bagHas(kObjectPerfumedOil))
			_app->varSetByte(90210, 2);
		break;

	case 211:
		if (_app->varGetByte(90211) < 2 && _app->varGetByte(1015) == 1)
			_app->varSetByte(90211, 2);
		break;

	case 214:
		if (_app->varGetByte(90214) < 2 && _app->varGetByte(1066) == 1)
			_app->varSetByte(90214, 2);
		break;

	case 215:
		if (_app->varGetByte(90215) < 2 && _app->varGetByte(1018) == 1)
			_app->objectSetAccessibilityOn(kObject10046);
		break;

	case 302:
		if (_app->varGetByte(90302) < 2 && _app->varGetByte(1029) == 1) {
			_app->varSetByte(90302, 2);
			_app->onCall(303);
		}
		break;

	case 303:
		if (_app->varGetByte(90303) < 2) {
			_app->varSetByte(90303, 2);
			_app->onCall(30);
			onVisual(0);
			_app->timerStopAll();
			_app->onCall(305);
			_app->onCall(10008);
		}
		break;

	case 307:
		if (_app->varGetByte(90307) < 2 && _app->varGetByte(1035) == 1) {
			_app->varSetByte(90307, 2);
			_app->onCall(308);
		}
		break;

	case 308:
		if (_app->varGetByte(90308) < 2 && _app->varGetByte(1070) == 1) {
			_app->varSetByte(90308, 2);
			_app->onCall(309);
		}
		break;

	case 402:
		if (_app->varGetByte(90402) < 2 && _app->varGetByte(1074) == 1)
			_app->varSetByte(90402, 2);
		break;

	case 403:
		if (_app->varGetByte(90403) < 2
		 && _app->varGetByte(1075) == 1) {

			_app->varSetByte(90403, 2);

			if (_app->varGetByte(90404) >= 2) {
				_app->onCall(405);
				_app->onCall(412);
			}
		}
		break;

	case 405:
		if (_app->varGetByte(90405) < 2
		 && _app->varGetByte(1076) == 1
		 && _app->varGetByte(1047) == 1
		 && _app->varGetByte(1046) == 1) {
			_app->varSetByte(90405, 2);
			_app->onCall(406);
			_app->onCall(408);
			_app->onCall(411);
		}
		break;

	case 406:
		if (_app->varGetByte(90406) < 2 && _app->varGetByte(1077) == 1) {
			_app->varSetByte(90406, 2);
			_app->onCall(407);
		}
		break;

	case 409:
		if (_app->varGetByte(90409) < 2 && _app->varGetByte(1049) == 1)
			_app->varSetByte(90409, 2);
		break;

	case 419:
		if (_app->varGetByte(90419) <= 2 && _app->varGetByte(1078) == 1) {
			_app->varSetByte(90419, 3);
			_app->onCall(420);
		}
		break;

	case 420:
		if (_app->varGetByte(90420) < 2 && _app->varGetByte(1051) == 1)
			_app->varSetByte(90420, 2);
		break;

	case 421:
		if (_app->varGetByte(90421) < 2)
			_app->varSetByte(90421, 2);
		break;

	case 1031:
		_app->varSetByte(1054, 1);
		break;

	case 1032:
	case 1033:
	case 1034:
	case 1035:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1004);
		break;

	case 1036:
		_app->varSetByte(1055, 1);
		break;

	case 1037:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1007);
		break;

	case 1039:
		_app->varSetByte(1056, 1);
		break;

	case 1040:
	case 1041:
	case 1042:
	case 1043:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1010);
		break;

	case 1048:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1011);
		break;

	case 1065:
		_app->varSetByte(1057, 1);
		break;

	case 1066:
	case 1067:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1013);
		break;

	case 2003:
		_app->varSetByte(1058, 1);
		_app->varSetByte(90006, 2);
		_app->visualBoxHide(6, kPuzzleMenu);
		break;

	case 2004:
		_app->varSetByte(1058, 1);
		_app->varSetByte(90006, 1);
		_app->visualBoxHide(6, kPuzzleMenu);
		break;

	case 2006:
	case 2007:
	case 2008:
	case 2012:
	case 2014:
	case 2015:
	case 2016:
	case 20061:
	case 20071:
	case 20081:
	case 20091:
	case 20092:
	case 20121:
	case 20141:
	case 20151:
	case 20161:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1017);
		break;

	case 2019:
		_app->varSetByte(1060, 1);
		break;

	case 2022:
		_app->varSetByte(1060, 1);
		// Fback to next case

	case 2020:
	case 2021:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1023);
		break;

	case 2033:
		_app->varSetByte(1062, 1);
		break;

	case 2034:
	case 2035:
	case 2036:
	case 2037:
	case 2038:
	case 2039:
	case 2051:
	case 2052:
	case 2053:
	case 4016:
	case 4017:
	case 4018:
	case 4019:
	case 4020:
	case 4021:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1027);
		break;

	case 2044:
		_app->varSetByte(1063, 1);
		// Fback to next case

	case 2041:
	case 2042:
	case 2043:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1030);
		break;

	case 2063:
		_app->varSetByte(1066, 1);
		break;

	case 2047:
		_app->varSetByte(1064, 1);
		// Fback to next case

	case 2045:
	case 2046:
	case 2048:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1033);
		break;

	case 2050:
		_app->varSetByte(10651, 1);
		break;

	case 2064:
	case 2065:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1007);
		break;

	case 3008:
		_app->varSetByte(1067, 1);
		break;

	case 3009:
	case 3010:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1030);
		break;

	case 3023:
		_app->varSetByte(1068, 1);
		break;

	case 3024:
	case 3025:
	case 3026:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1045);
		break;

	case 3030:
	case 3031:
		_app->varSetByte(1069, 1);
		// Fback to next case

	case 3029:
	case 3032:
	case 3033:
	case 3034:
	case 3035:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1046);
		break;

	case 3037:
	case 3040:
		_app->varSetByte(1071, 1);
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1048);
		break;

	case 3043:
		_app->varSetByte(1072, 1);
		// Fback to next case

	case 3042:
	case 3044:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1049);
		break;

	case 3045:
		_app->varSetByte(1073, 1);
		break;

	case 3046:
		_app->varSetByte(1073, 1);
		// Fback to next case

	case 3047:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1051);
		break;

	case 3041:
		_app->varSetByte(1072, 1);
		break;

	case 4011:
	case 4013:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1002);
		_app->varSetByte(1074, 1);
		break;

	case 4015:
		_app->varSetByte(1075, 1);
		break;

	case 4032:
		_app->varSetByte(1076, 1);
		break;

	case 4033:
	case 4034:
	case 4035:
	case 4036:
	case 4037:
	case 4038:
	case 4039:
	case 4040:
	case 4041:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1053);
		break;

	case 4042:
		_app->varSetByte(1077, 1);
		break;

	case 4047:
		_app->varSetByte(1077, 1);
		// Fback to next case

	case 4043:
	case 4044:
	case 4045:
	case 4046:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1056);
		break;

	case 4108:
		_app->varSetByte(1078, 1);
		break;

	case 4109:
	case 4110:
	case 4111:
		_app->visualBoxHide(6, kPuzzleMenu);
		_app->puzzleSetActive(kPuzzle1060);
		break;

	case 20051:
	case 20052:
		_app->varSetByte(1059, 1);
		break;

	case 20331:
		_app->varSetByte(1065, 1);
		break;

	case 20401:
		_app->varSetByte(1063, 1);
		break;

	case 20402:
		_app->varSetByte(1064, 1);
		break;

	case 30310:
		_app->varSetByte(1070, 1);
		break;
	}
}

} // End of namespace Ring
