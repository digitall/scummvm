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

#include "ring/game/pompeii/pompeii_application.h"

#include "ring/base/art.h"
#include "ring/base/bag.h"
#include "ring/base/cursor.h"
#include "ring/base/preferences.h"
#include "ring/base/puzzle.h"
#include "ring/base/rotation.h"
#include "ring/base/saveload.h"
#include "ring/base/timer.h"

#include "ring/game/pompeii/pompeii_shared.h"
#include "ring/game/pompeii/pompeii_zonesystem.h"
#include "ring/game/pompeii/pompeii_zone1.h"
#include "ring/game/pompeii/pompeii_zone2.h"
#include "ring/game/pompeii/pompeii_zone3.h"
#include "ring/game/pompeii/pompeii_zone4.h"
#include "ring/game/pompeii/pompeii_zone5.h"
#include "ring/game/pompeii/pompeii_zone6.h"
#include "ring/game/pompeii/pompeii_zone7.h"
#include "ring/game/pompeii/pompeii_zone8.h"
#include "ring/game/pompeii/pompeii_zone9.h"
#include "ring/game/pompeii/pompeii_zone10.h"
#include "ring/game/pompeii/pompeii_zone11.h"
#include "ring/game/pompeii/pompeii_zone12.h"

#include "ring/graphics/dragControl.h"
#include "ring/graphics/image.h"
#include "ring/graphics/screen.h"

#include "ring/sound/soundhandler.h"

#include "ring/debug.h"
#include "ring/helpers.h"
#include "ring/ring.h"

using namespace PompeiiGame;

namespace Ring {

#pragma region Static data

static const struct {
	int32 val;
	int16 multiplierX;
	int16 multiplierY;
} _puzzle6Coordinates[] = {
	{10155,    336, 273},
	{10011,    327, 296},
	{1001102,  327, 296},
	{1001103,  327, 296},
	{10012,    305, 283},
	{1001202,  305, 283},
	{10013,    295, 292},
	{10014,    345, 250},
	{1001402,  345, 250},
	{10015,    380, 223},
	{1001502,  380, 223},
	{10016,    417, 193},
	{1001602,  417, 193},
	{10017,    451, 166},
	{1001702,  451, 166},
	{10018,    461, 158},
	{1001802,  461, 158},
	{10019,    484, 197},
	{1001902,  484, 197},
	{100110,   502, 183},
	{10011002, 502, 183},
	{100111,   384, 344},
	{10011102, 384, 344},
	{10011103, 384, 344},
	{100112,   355, 319},
	{10011202, 355, 319},
	{10011203, 355, 319},
	{100113,   329, 302},
	{10011302, 329, 302},
	{10011303, 329, 302},
	{100114,   329, 265},
	{10011402, 329, 265},
	{10021,    205, 334},
	{10022,    199, 354},
	{10023,    201, 328},
	{10024,    172, 325},
	{10025,    176, 304},
	{10026,    158, 274},
	{10027,    222, 327},
	{10028,    209, 349},
	{10029,    194, 355},
	{100210,   186, 349},
	{100211,   179, 333},
	{100212,   166, 312},
	{100213,   191, 340},
	{100214,   184, 325},
	{100215,   178, 314},
	{100216,   172, 307},
	{100217,   172, 296},
	{100218,   203, 334},
	{100219,   198, 335},
	{100220,   190, 316},
	{100221,   186, 307},
	{10031,    325, 306},
	{100312,   325, 306},
	{10032,    302, 324},
	{100322,   302, 324},
	{10041,    538, 164},
	{10042,    554, 191},
	{10043,    538, 164},
	{10044,    554, 191},
	{10045,    554, 191},
	{10046,    569, 204},
	{10047,    569, 191},
	{10048,    554, 191},
	{100410,   538, 164},
	{100411,   538, 164},
	{100412,   538, 164},
	{100413,   585, 196},
	{10051,    309, 358},
	{1005102,  309, 358},
	{10052,    309, 358},
	{1005202,  309, 358},
	{10053,    309, 358},
	{1005302,  309, 358},
	{10054,    309, 358},
	{1005402,  309, 358},
	{10055,    309, 358},
	{1005502,  309, 358},
	{10056,    309, 358},
	{1005602,  309, 358},
	{10057,    309, 358},
	{1005702,  309, 358},
	{10058,    309, 358},
	{1005802,  309, 358},
	{10059,    309, 358},
	{1005902,  309, 358},
	{100510,   309, 358},
	{10051002, 309, 358},
	{100511,   309, 358},
	{10051102, 309, 358},
	{100512,   309, 358},
	{10051202, 309, 358},
	{100513,   309, 358},
	{10051302, 309, 358},
	{100514,   309, 358},
	{10051402, 309, 358},
	{100515,   309, 358},
	{10051502, 309, 358},
	{100516,   309, 358},
	{10051602, 309, 358},
	{100517,   309, 358},
	{10051702, 309, 358},
	{100518,   309, 358},
	{10051802, 309, 358},
	{100519,   309, 358},
	{10051902, 309, 358},
	{100520,   309, 358},
	{10052002, 309, 358},
	{100521,   309, 358},
	{10052102, 309, 358},
	{100522,   309, 358},
	{10052202, 309, 358},
	{10111,    336, 273},
	{10112,    336, 273},
	{10113,    336, 273},
	{10114,    336, 273},
	{10121,    336, 273},
	{10122,    336, 273},
	{10123,    336, 273},
	{10124,    336, 273},
	{10125,    336, 273},
	{10126,    336, 273},
	{10127,    336, 273},
	{10128,    336, 273},
	{10131,    336, 273},
	{10141,    336, 273},
	{10142,    336, 273},
	{10151,    336, 273},
	{10152,    336, 273},
	{10153,    336, 273},
	{10154,    336, 273},
	{10161,    336, 273},
	{10162,    336, 273},
	{10163,    336, 273},
	{10164,    336, 273},
	{10166,    336, 273},
	{10171,    336, 273},
	{10172,    336, 273},
	{10173,    336, 273},
	{10181,    336, 273},
	{10191,    336, 273},
	{10101,    336, 273},
	{20111,    336, 273},
	{20112,    336, 273},
	{20113,    336, 273},
	{20114,    336, 273},
	{20121,    336, 273},
	{20122,    336, 273},
	{20123,    336, 273},
	{20124,    336, 273},
	{20125,    336, 273},
	{20126,    336, 273},
	{20127,    336, 273},
	{20128,    336, 273},
	{20161,    336, 273},
	{20162,    336, 273},
	{20163,    336, 273},
	{20164,    336, 273},
	{20166,    336, 273},
	{20171,    336, 273},
	{20172,    336, 273},
	{20173,    336, 273},
	{10211,    464, 162},
	{102112,   464, 162},
	{10212,    464, 162},
	{10213,    464, 162},
	{10214,    464, 162},
	{10215,    464, 162},
	{10221,    469, 167},
	{10231,    473, 167},
	{10232,    473, 167},
	{10233,    473, 167},
	{10241,    479, 176},
	{10242,    479, 176},
	{10243,    487, 182},
	{10301,    212, 321},
	{10302,    212, 321},
	{10303,    212, 321},
	{10304,    212, 321},
	{10305,    212, 321},
	{10411,    150, 279},
	{10412,    150, 279},
	{10413,    150, 279},
	{10414,    150, 279},
	{10431,    150, 279},
	{10441,    150, 279},
	{10442,    150, 279},
	{10451,    150, 279},
	{10452,    150, 279},
	{10511,    386, 224},
	{10521,    386, 224},
	{10531,    386, 224},
	{10611,    168, 336},
	{10612,    168, 336},
	{10613,    168, 336},
	{10614,    168, 336},
	{10615,    168, 336},
	{10616,    168, 336},
	{10617,    168, 336},
	{10618,    168, 336},
	{10619,    168, 336},
	{106110,   168, 336},
	{10621,    168, 336},
	{10622,    168, 336},
	{10623,    168, 336},
	{10624,    168, 336},
	{10711,    321, 318},
	{10712,    321, 318},
	{10713,    321, 318},
	{10714,    321, 318},
	{10715,    321, 318},
	{10721,    321, 318},
	{10811,    526, 195},
	{10812,    526, 195},
	{10813,    526, 195},
	{10814,    526, 195},
	{10815,    526, 195},
	{10816,    526, 195},
	{10911,    546, 161},
	{10912,    560, 191},
	{10913,    553, 179},
	{10921,    573, 165},
	{10922,    584, 178},
	{10923,    584, 178},
	{10924,    573, 165},
	{10925,    573, 165},
	{10931,    591, 184},
	{10932,    589, 189},
	{11011,    421, 196},
	{11021,    421, 196},
	{11022,    421, 196},
	{11023,    421, 196},
	{11024,    421, 196},
	{11025,    421, 196},
	{11111,    327, 335},
	{11112,    327, 335},
	{11113,    327, 335},
	{11114,    327, 335},
	{11121,    327, 335},
	{11122,    327, 335},
	{11123,    327, 335},
	{11124,    327, 335}
};

#pragma endregion

ApplicationPompeii::ApplicationPompeii(RingEngine *engine) : Application(engine) {
	_systemZone = kZone100;

	// Event handlers
	_zoneSystem = new ZoneSystemPompeii(this);
	_zone1      = new Zone1Pompeii(this);
	_zone2      = new Zone2Pompeii(this);
	_zone3      = new Zone3Pompeii(this);
	_zone4      = new Zone4Pompeii(this);
	_zone5      = new Zone5Pompeii(this);
	_zone6      = new Zone6Pompeii(this);
	_zone7      = new Zone7Pompeii(this);
	_zone8      = new Zone8Pompeii(this);
	_zone9      = new Zone9Pompeii(this);
	_zone10     = new Zone10Pompeii(this);
	_zone11     = new Zone11Pompeii(this);
	_zone12     = new Zone12Pompeii(this);

	// Data
	_savedZone = kZoneNone;
}

ApplicationPompeii::~ApplicationPompeii() {
	SAFE_DELETE(_zoneSystem);
	SAFE_DELETE(_zone1);
	SAFE_DELETE(_zone2);
	SAFE_DELETE(_zone3);
	SAFE_DELETE(_zone4);
	SAFE_DELETE(_zone5);
	SAFE_DELETE(_zone6);
	SAFE_DELETE(_zone7);
	SAFE_DELETE(_zone8);
	SAFE_DELETE(_zone9);
	SAFE_DELETE(_zone10);
	SAFE_DELETE(_zone11);
	SAFE_DELETE(_zone12);
}

#pragma region Game setup

void ApplicationPompeii::initFont() {
	// Original asks for size 12, but the font only contains size 8
	fontAdd(kFontDefault, "pompei.fon", "Arxel1", 8, true, false, false, false, getCurrentLanguage());
}

void ApplicationPompeii::setup() {
	// Add the list of episodes
	addEpisode(kZone2, "One",   2);
	addEpisode(kZone3, "Two",   1);
	addEpisode(kZone4, "Three", 2);
	addEpisode(kZone5, "Four",  1);
	addEpisode(kZone6, "Five",  1);

	// Add the list of zones
	addZone(kZone1,  "S00", "S00", kArchiveArt, kLoadFromCd);
	addZone(kZone2,  "S01", "S01", kArchiveArt, kLoadFromCd);
	addZone(kZone3,  "S02", "S02", kArchiveArt, kLoadFromCd);
	addZone(kZone4,  "S03", "S03", kArchiveArt, kLoadFromCd);
	addZone(kZone5,  "S04", "S04", kArchiveArt, kLoadFromCd);
	addZone(kZone6,  "S05", "S05", kArchiveArt, kLoadFromCd);
	addZone(kZone7,  "S06", "S06", kArchiveArt, kLoadFromCd);
	addZone(kZone8,  "S07", "S07", kArchiveArt, kLoadFromCd);
	addZone(kZone9,  "S08", "S08", kArchiveArt, kLoadFromCd);
	addZone(kZone10, "S09", "S09", kArchiveArt, kLoadFromCd);
	addZone(kZone11, "S10", "S10", kArchiveArt, kLoadFromCd);
	addZone(kZone12, "S11", "S11", kArchiveArt, kLoadFromCd);

	// Setup system zone
	setupZone(kZone1, kSetupTypeNone);
	setSpace(kZone100);

	// Setup cursors
	ArchiveType archiveType = (_configuration.artCURSOR ? kArchiveArt : kArchiveFile);

	cursorAdd(kCursorDefault,               "", kCursorTypeNormal, 1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorBusy,          "cur_busy", kCursorTypeImage,  1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorIdle,          "cur_idle", kCursorTypeImage,  1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorMove,           "cur_muv", kCursorTypeImage,  1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorBack,          "cur_back", kCursorTypeImage,  1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorMenuIdle,      "cur_me-1", kCursorTypeImage,  1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorMenuActive,    "cur_me-2", kCursorTypeImage,  1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorZoom,          "cur_zoom", kCursorTypeImage,  1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorTake,          "cur_take", kCursorTypeImage,  1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorAction,        "cur_ac-1", kCursorTypeImage,  1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorActionObject,  "cur_ac-2", kCursorTypeImage,  1, kLoadFromCursor, archiveType);
	cursorAdd(kCursorDragDrop,         "adrag", kCursorTypeImage,  1, kLoadFromCursor, archiveType);

	cursorSetOffset(kCursorBusy,         Common::Point(8, 8));
	cursorSetOffset(kCursorIdle,         Common::Point(17, 1));
	cursorSetOffset(kCursorMove,         Common::Point(17, 1));
	cursorSetOffset(kCursorBack,         Common::Point(17, 1));
	cursorSetOffset(kCursorMenuIdle,     Common::Point(8, 8));
	cursorSetOffset(kCursorMenuActive,   Common::Point(8, 8));
	cursorSetOffset(kCursorZoom,         Common::Point(17, 1));
	cursorSetOffset(kCursorTake,         Common::Point(17, 1));
	cursorSetOffset(kCursorAction,       Common::Point(17, 1));
	cursorSetOffset(kCursorActionObject, Common::Point(17, 1));
	cursorSetOffset(kCursorDragDrop,     Common::Point(8, 8));

	// Setup subtitles
	subtitleSetColor(Color(255, 255, 255));
	subtitleSetBackgroundColor(Color(50, 50, 50));
}

void ApplicationPompeii::initBag() {
	_bag->setOrigin(Common::Point(0, 0));
	_bag->sub_417D40(15, 26, 40, 61);
	_bag->setBackgroundOffset(Common::Point(0, 0));
	_bag->sub_417DD0(10);
	_bag->sub_417D80(0, 0, 30, 448);
	_bag->sub_417DA0(610, 0, 30, 448);
	_bag->sub_4192A0(6, 12);
	_bag->sub_4192C0(622, 12);
	_bag->sub_417DE0(291, 4);
	_bag->sub_419280(500);
	_bag->loadBackground("bagbgr.tga", "", "", "", "", "", "", "bag_h.bmp", _archiveType);
	_bag->initHotspots();
}

#pragma endregion

#pragma region Startup

void ApplicationPompeii::showStartupScreen() {
	// Original setups zones a second time for no good reason
	setCurrentZone(kZone100);

	playMovie("dclogo");
	playMovie("Rmn");
	playMovie("Logo");
	playMovie("Sur");
	playMovie("Arxel");

	varSetByte(90001, 5);
	showDay();

	playMovie("INTRO_-2");
	varSetByte(90001, 0);
	showDay();

	playMovie("INTRO_-1");
	varSetByte(90001, 1);

	playMovie("INTRO_-3");
	varSetByte(90904, 0);

	displayFade("black.bmp", "menu.bmp", 5, 0, kLoadFromDisk, getConfiguration().artSY ? kArchiveArt : kArchiveFile);
	varSetByte(90904, 16);

	setupUser(1);

	_preferenceHandler->load();
}

void ApplicationPompeii::startMenu(bool savegame) {
	visualBoxHide(6, kPuzzleMenu);

	if (_currentGameZone)
		return;

	if (savegame) {
		cursorSelect(kCursorBusy);
		_vm->setFlag(false);
		_bag->reset();

		if (!_saveManager->loadSave(0, kLoadSaveWrite))
			error("[ApplicationRing::startMenu] Cannot save game in slot 0");

		// Save a copy of the screen surface for savegame
		SAFE_DELETE(_thumbnail);

		_thumbnail = new ImageSurface();
		_thumbnail->create(24, 2, 640, 480);

		// Save a copy of the screen to our image
		_screenManager->copySurface(_thumbnail);
	}

	_currentGameZone = getCurrentZone();

	soundStopAll(4);
	timerStopAll();
	setupZone(kZone1, kSetupTypeNone);
	setSpace(kZone100);
	puzzleSetActive(kPuzzleGeneralMenu);
	puzzleSetMod(kPuzzleMenu, 1, 0);

	for (uint32 i = 1; i < 8; i++) {
		objectSetAccessibilityOff(i);
		objectPresentationHideAndRemove(i);
	}

	Bag *bag = getBag();
	if (bag && bag->isInitialized())
		bag->reset();

	cursorDelete();

	if (savegame)
		objectSetAccessibilityOn(kObject99002);
	else
		objectSetAccessibilityOn(kObject99002);

	// Load user preferences
	setUser("user1");
	_preferenceHandler->load();
}

void ApplicationPompeii::showMenu(ZoneId /*zone*/, MenuAction menuAction) {
	int8 byte90005, byte90006, byte90007, byte90903;
	int16 amount;

	switch (menuAction) {
	default:
		setSpace(kZone100);

		if (menuAction >= 13) {
			fadeOut(15, Color(0, 0, 0), 0);
		} else {
			varSetByte(90904, 0);
			Common::String image = Common::String::format("end_%02d.bmp", menuAction);
			showImage(image, Common::Point(0, 0), 6000, kLoadFromDisk, kArchiveInvalid);
			displayFade(image, "black.bmp", 5, 0, kLoadFromDisk, _configuration.artSY ? kArchiveArt : kArchiveFile);
			varSetByte(90904, 16);
		}

		if (getSaveManager()->hasSavegame(1))
			getSaveManager()->deleteSavegame(1);

		exitZone();
		initZones();
		setSpace(kZone100);
		startMenu(false);
		break;

	case kMenuAction50:
		exitZone();
		initZones();
		setupZone(kZone2, kSetupTypeNone);
		varSetByte(98009, 1);
		break;

	case kMenuAction51:
		exitZone();
		initZones();
		setupUser(2);
		if (!getSaveManager()->hasSavegame(2)) {
			setupZone(kZone2, kSetupType1);
		} else if (!getSaveManager()->loadSave("SaveGame", kLoadSaveRead)) {
			// Cannot load game
			exitZone();
			initZones();
			messageFormat("CanNotLoadGame", "SaveGame");
			messageShowWarning(0);
		}
		break;

	case kMenuAction52:
		if (getSaveManager()->hasSavegame(1)) {
			exitZone();
			initZones();
			setupUser(1);
			if (getSaveManager()->loadSave("SaveGame", kLoadSaveRead)) {
				visualBoxHide(6, kPuzzleMenu);
			} else {
				// Cannot load game
				exitZone();
				initZones();
				messageFormat("CanNotLoadGame", "SaveGame");
				messageShowWarning(0);
			}
		}
		break;

	case kMenuAction99:
		objectSetAccessibilityOff(kObject16, 0, 0);
		objectSetAccessibilityOn(kObject16, 2, 2);
		varSetByte(90001, 4);
		setupZone(kZone3, kSetupTypeNone);
		break;

	case kMenuAction200:
		byte90005 = varGetByte(90005);
		byte90006 = varGetByte(90006);
		byte90007 = varGetByte(90007);
		amount    = varGetWord(99500);
		byte90903 = varGetByte(90903);
		exitZone();
		initZones();
		varSetByte(90005, byte90005);
		varSetByte(90006, byte90006);
		varSetByte(90007, byte90007);
		varSetByte(90903, byte90903);
		bagAdd(kObjectAmulet);
		bagAdd(kObjectRopes);
		takeMoney(amount);
		bagAdd(kObjectDates);
		bagAdd(kObjectKnife);
		bagAdd(kObjectHollyWater);
		setupZone(kZone2, kSetupType200);
		break;

	case kMenuAction300:
		byte90005 = varGetByte(90005);
		byte90006 = varGetByte(90006);
		byte90007 = varGetByte(90007);
		amount    = varGetWord(99500);
		byte90903 = varGetByte(90903);
		exitZone();
		initZones();
		varSetByte(90005, byte90005);
		varSetByte(90006, byte90006);
		varSetByte(90007, byte90007);
		varSetByte(90903, byte90903);
		bagAdd(kObjectAmulet);
		bagAdd(kObjectRopes);
		takeMoney(amount);
		bagAdd(kObjectKnife);
		bagAdd(kObjectDoveFeather);
		bagAdd(kObjectTali2);
		setupZone(kZone2, kSetupType300);
		break;

	case kMenuAction400:
		byte90005 = varGetByte(90005);
		byte90006 = varGetByte(90006);
		byte90007 = varGetByte(90007);
		amount    = varGetWord(99500);
		byte90903 = varGetByte(90903);
		exitZone();
		initZones();
		varSetByte(90903, byte90903);
		varSetByte(90005, byte90005);
		varSetByte(90006, byte90006);
		varSetByte(90007, byte90007);
		bagAdd(kObjectAmulet);
		bagAdd(kObjectRopes);
		takeMoney(amount);
		bagAdd(kObjectKnife);
		bagAdd(kObjectDoveFeather);
		bagAdd(kObjectPhiltre2);
		bagAdd(kObjectNightingale);
		setupZone(kZone2, kSetupType400);
		break;
	}
}

void ApplicationPompeii::showCredits() {
	soundStopAll(1024);
	soundPlay(90801, kSoundLoop);
	setCurrentZone(kZone100);

	// Scroll credits
	for (uint i = 0; i < 14; i++) {
		if (scrollImage(Common::String::format("cre%02d", i + 1), 0, kLoadFromDisk, _configuration.artSY ? kArchiveArt : kArchiveFile))
			break;
	}

	soundStop(90801, 1024);
}

void ApplicationPompeii::startGame() {
	error("[ApplicationPompeii::startGame] Not implemented");
}

#pragma endregion

#pragma region Drawing

void ApplicationPompeii::draw() {
	// Update our screen
	_screenManager->updateScreen();

	// Update engine state
	//error("[ApplicationPompeii::draw] Engine state update not implemented!");
}

#pragma endregion

#pragma region Messages

void ApplicationPompeii::messageInsertCd(ZoneId zone) {
	setupZone(kZone1, kSetupTypeNone);
	setSpace(kZone100);

	for (uint32 i = 0; i <= 7; i++) {
		objectSetAccessibilityOff(i);
		objectPresentationHideAndRemove(i);
	}

	objectPresentationSetTextToPuzzle(kObject16, 0, 0, _messageType);
	objectPresentationSetTextCoordinatesToPuzzle(kObject16, 0, 0, Common::Point(225, 193));
	objectPresentationSetTextToPuzzle(kObject16, 0, 1, _message);
	objectPresentationSetTextCoordinatesToPuzzle(kObject16, 0, 1, Common::Point(225, 213));
	objectPresentationShow(kObject16, 0);
	objectSetAccessibilityOff(kObject16);

	if (zone == kZone3)
		objectSetAccessibilityOn(kObject16, 1, 2);
	else
		objectSetAccessibilityOn(kObject16, 0, 1);

	puzzleSetActive(kPuzzleInsertCd);
}

#pragma endregion

#pragma region Zone initialization

void ApplicationPompeii::initZones() {
	debugC(kRingDebugLogic, "Init zone data");

	_loadFrom = kLoadFromDisk;

	drawZoneName(kZone100);
	_archiveType = getZoneArchiveType(kZone100);
	_zoneSystem->onInit();

	_loadFrom = kLoadFromCd;

	drawZoneName(kZone1);
	_archiveType = getZoneArchiveType(kZone2);
	_zone1->onInit();

	drawZoneName(kZone2);
	_archiveType = getZoneArchiveType(kZone2);
	_zone2->onInit();

	drawZoneName(kZone2);
	_archiveType = getZoneArchiveType(kZone3);
	_zone3->onInit();

	drawZoneName(kZone4);
	_archiveType = getZoneArchiveType(kZone4);
	_zone4->onInit();

	drawZoneName(kZone5);
	_archiveType = getZoneArchiveType(kZone5);
	_zone5->onInit();

	drawZoneName(kZone6);
	_archiveType = getZoneArchiveType(kZone6);
	_zone6->onInit();

	drawZoneName(kZone7);
	_archiveType = getZoneArchiveType(kZone7);
	_zone7->onInit();

	drawZoneName(kZone8);
	_archiveType = getZoneArchiveType(kZone8);
	_zone8->onInit();

	drawZoneName(kZone9);
	_archiveType = getZoneArchiveType(kZone9);
	_zone9->onInit();

	drawZoneName(kZone10);
	_archiveType = getZoneArchiveType(kZone10);
	_zone10->onInit();

	_loadFrom = kLoadFromDisk;

	drawZoneName(kZone11);
	_archiveType = getZoneArchiveType(kZone11);
	_zone11->onInit();

	drawZoneName(kZone12);
	_archiveType = getZoneArchiveType(kZone12);
	_zone12->onInit();

	_archiveType = getZoneArchiveType(getCurrentZone());

	setState(kStateNone);

	// Clear screen
	_screenManager->clear();
	g_system->updateScreen();
}

#pragma endregion

#pragma region Event handling

void ApplicationPompeii::onMouseLeftButtonUp(const Common::Event &evt, bool isControlPressed) {
	warning("[ApplicationPompeii::onMouseLeftButtonUp] Not implemented (evt: %d, CTRL pressed: %d)", evt.type, isControlPressed);
}

void ApplicationPompeii::onMouseLeftButtonDown(const Common::Event &evt) {
	warning("[ApplicationPompeii::onMouseLeftButtonDown] Not implemented (evt: %d)", evt.type);
}

void ApplicationPompeii::onMouseRightButtonUp(const Common::Event &evt) {
	debugC(kRingDebugLogic, "onMouseRightButtonUp: Coords (%d, %d)", evt.mouse.x, evt.mouse.y);

	if (getDragControl()->getField20() || getCurrentGameZone())
		return;

	Puzzle *puzzleMenu = getApp()->getPuzzle(kPuzzleMenu);
	if (puzzleMenu && puzzleMenu->getField24() == 2)
		return;

	Bag *bag = getApp()->getBag();
	if (bag->isInitialized()) {
		bag->reset();
		setFreOffCurrentRotation();
	} else {
		getApp()->cursorDelete();
		bag->initialize();

		if (getApp()->getCurrentRotation())
			bag->setRotationFre(getApp()->getCurrentRotation()->getFre());

		getApp()->setupCurrentRotation();
	}
}

void ApplicationPompeii::onKeyDown(Common::Event &evt) {
	onKeyDown(evt.kbd.keycode, evt.kbd.ascii);
}

void ApplicationPompeii::onKeyDown(const Common::KeyCode &keycode, uint ascii) {
	switch (getCurrentPuzzleId()) {
	default:
		if (keycode == Common::KEYCODE_SPACE
		 && !soundIsPlayingType(kSoundTypeDialog)) {
			objectPresentationHide(kObjectItemList);
			objectSetAccessibilityOff(kObjectItemList);
			visualBoxHide(6, kPuzzleMenu);

			if (!varGetByte(90508)) {
				puzzleSetMod(kPuzzleMenu, 2, 6);
				objectPresentationShow(kObject6, 0);
				objectSetAccessibilityOn(kObject6);
				if (hasCurrentRotation())
					varSetDword(99027, (int16)getCurrentRotationId());

				objectPresentationShow(kObject6, 5);
				setCoordinatesOnPuzzle6();
				objectSetAccessibilityOn(kObject6, 1, 4);
				objectSetAccessibilityOff(kObject6, 6, 18);
				varSetByte(90508, 1);
			}
		}
		break;

	case kPuzzleEncyclopedia:
	case kPuzzle50001:
	case kPuzzlePreferences:
	case kPuzzleStatus:
	case kPuzzle90005:
	case kPuzzleInsertCd:
		break;

	case kPuzzleLoad:
		if (keycode == Common::KEYCODE_DELETE) {
			if (visualListGetObjectIdClicked(4, kPuzzleLoad)) {
				messageGet("DoYouWantToDeleteSavedGame");
				messageShowQuestion(8);
			}
		}
		break;

	case kPuzzleSave:
		if (keycode == Common::KEYCODE_DELETE) {
			if (visualListGetObjectIdClicked(1, kPuzzleSave)) {
				messageGet("DoYouWantToDeleteSavedGame");
				messageShowQuestion(4);
			}

			break;
		}

		visualListResetObjectClicked(1, kPuzzleSave);
		objectSetAccessibilityOn(kObject99025);
		objectPresentationHide(kObject99025, 1);

		switch (keycode) {
		default:
			if (objectPresentationGetTextWidth(kObjectSaveName, 1, 0) >= 235)
				return;

			*getSaveManager()->getName() += (char)ascii;
			break;

		case Common::KEYCODE_ESCAPE:
			getSaveManager()->getName()->clear();
			break;

		case Common::KEYCODE_BACKSPACE:
			if (getSaveManager()->getName()->size() != 1)
				getSaveManager()->getName()->deleteLastChar();
			break;

		case Common::KEYCODE_RETURN:
			// Do nothing
			return;
		}

		objectPresentationSetTextToPuzzle(kObjectSaveName, 1, 0, *getSaveManager()->getName());
		objectPresentationSetTextCoordinatesToPuzzle(kObjectSaveName, 1, 0, Common::Point(344, 181));
		objectPresentationSetAnimationCoordinatesOnPuzzle(kObjectSaveName, 0, Common::Point((int16)objectPresentationGetTextWidth(kObjectSaveName, 1, 0) + 30, 435));
		break;
	}
}

void ApplicationPompeii::onButtonUp(ObjectId id, Id target, Id puzzleRotationId, uint32 a4, const Common::Point &point) {
	debugC(kRingDebugLogic, "onButtonUp (ObjectId: %d, target: %d, coords: (%d, %d))", id.id(), target, point.x, point.y);

	if (puzzleRotationId == 1 && a4 == 1) {
		_zoneSystem->onButtonUp(id, target, puzzleRotationId, a4, point);
		return;
	}

	switch (getCurrentZone()) {
	default:
		break;

	case kZone100:
		_zoneSystem->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;

	case kZone1:
		_zone1->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;

	case kZone2:
		_zone2->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;

	case kZone3:
		_zone3->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;

	case kZone4:
		_zone4->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;

	case kZone5:
		_zone5->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;

	case kZone6:
		_zone6->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;

	case kZone7:
		_zone7->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;

	case kZone8:
		_zone8->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;

	case kZone9:
		_zone9->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;

	case kZone10:
		_zone10->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;

	case kZone11:
		_zone11->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;

	case kZone12:
		_zone12->onButtonUp(id, target, puzzleRotationId, a4, point);
		break;
	}

	if (!checkBag()) {
		bagRemove(kObjectItemList);
		bagAdd(kObjectItemListCompleted);
	}
}

void ApplicationPompeii::onTimer(TimerId timerId) {
	debugC(kRingDebugLogic, "[ApplicationPompeii::onTimer] Timer id: %d", timerId);

	switch (getCurrentZone()) {
	default:
		break;

	case kZone1:
		_zone1->onTimer(timerId);
		break;

	case kZone2:
		_zone2->onTimer(timerId);
		break;

	case kZone3:
		_zone3->onTimer(timerId);
		break;

	case kZone4:
		_zone4->onTimer(timerId);
		break;

	case kZone5:
		_zone5->onTimer(timerId);
		break;

	case kZone6:
		_zone6->onTimer(timerId);
		break;

	case kZone8:
		_zone8->onTimer(timerId);
		break;

	case kZone9:
		_zone9->onTimer(timerId);
		break;

	case kZone10:
		_zone10->onTimer(timerId);
		break;

	case kZone11:
		_zone11->onTimer(timerId);
		break;

	case kZone12:
		_zone12->onTimer(timerId);
		break;
	}

	if (getState() != kStateShowMenu)
		getTimerHandler()->incrementFiredCount(timerId);
}

void ApplicationPompeii::onSound(Id id, SoundType type, uint32 a3) {
	debugC(kRingDebugLogic, "onSound (id: %d, type: %d)", id, type);

	bool process = (a3 & 0x1000) != 0;
	a3 &= 239;

	switch (getCurrentZone()) {
	default:
		break;

	case kZone100:
		_zoneSystem->onSound(id, type, a3, process);
		break;

	case kZone1:
		_zone1->onSound(id, type, a3, process);
		break;

	case kZone2:
		_zone2->onSound(id, type, a3, process);
		break;

	case kZone3:
		_zone3->onSound(id, type, a3, process);
		break;

	case kZone4:
		_zone4->onSound(id, type, a3, process);
		break;

	case kZone5:
		_zone5->onSound(id, type, a3, process);
		break;

	case kZone6:
		_zone6->onSound(id, type, a3, process);
		break;

	case kZone7:
		_zone7->onSound(id, type, a3, process);
		break;

	case kZone8:
		_zone8->onSound(id, type, a3, process);
		break;

	case kZone10:
		_zone10->onSound(id, type, a3, process);
		break;

	case kZone11:
		_zone11->onSound(id, type, a3, process);
		break;

	case kZone12:
		_zone12->onSound(id, type, a3, process);
		break;
	}
}

void ApplicationPompeii::onSetup(ZoneId zone, SetupType type) {
	debugC(kRingDebugLogic, "onSetup (zone: %s, type: %d)", getZoneFolder(zone).c_str(), type);

	switch (zone) {
	default:
		break;

	case kZone2:
		_zone2->onSetup(type);
		break;

	case kZone3:
		_zone3->onSetup(type);
		break;

	case kZone4:
		_zone4->onSetup(type);
		break;
	}
}

void ApplicationPompeii::onBag(ObjectId id, Id target, Id puzzleRotationId, uint32 a4, DragControl *dragControl, byte type) {
	if (getCurrentZone() == kZone100)
		_zoneSystem->onBag(id, target, puzzleRotationId, a4, dragControl, type);
}

void ApplicationPompeii::onUpdateBefore(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, const Common::Point &point) {
	if (movabilityIndex == 1 && target == 1) {
		_zoneSystem->onUpdateBefore(movabilityFrom, movabilityTo, movabilityIndex, target, point);
		return;
	}

	if (hasCurrentRotation())
		varSetWord(99028, (int16)getCurrentRotationId());

	switch (getCurrentZone()) {
	default:
		break;

	case kZone100:
		_zoneSystem->onUpdateBefore(movabilityFrom, movabilityTo, movabilityIndex, target, point);
		break;

	case kZone1:
		_zone1->onUpdateBefore(movabilityFrom, movabilityTo, movabilityIndex, target, point);
		break;

	case kZone2:
		_zone2->onUpdateBefore(movabilityFrom, movabilityTo, movabilityIndex, target, point);
		break;

	case kZone3:
		_zone3->onUpdateBefore(movabilityFrom, movabilityTo, movabilityIndex, target, point);
		break;

	case kZone4:
		_zone4->onUpdateBefore(movabilityFrom, movabilityTo, movabilityIndex, target, point);
		break;

	case kZone6:
		_zone6->onUpdateBefore(movabilityFrom, movabilityTo, movabilityIndex, target, point);
		break;

	case kZone7:
		_zone7->onUpdateBefore(movabilityFrom, movabilityTo, movabilityIndex, target, point);
		break;

	case kZone8:
		_zone8->onUpdateBefore(movabilityFrom, movabilityTo, movabilityIndex, target, point);
		break;

	case kZone9:
		_zone9->onUpdateBefore(movabilityFrom, movabilityTo, movabilityIndex, target, point);
		break;

	case kZone11:
		_zone11->onUpdateBefore(movabilityFrom, movabilityTo, movabilityIndex, target, point);
		break;

	case kZone12:
		_zone12->onUpdateBefore(movabilityFrom, movabilityTo, movabilityIndex, target, point);
		break;
	}
}

void ApplicationPompeii::onUpdateAfter(Id movabilityFrom, Id /*movabilityTo*/, uint32 /*movabilityIndex*/, Id /*target*/, MovabilityType /*movabilityType*/, const Common::Point &/*point*/) {
	if (movabilityFrom == 1)
		return;

	visualBoxHide(6, kPuzzleMenu);
}

void ApplicationPompeii::onUpdateBag(const Common::Point &point) {
	if (getCurrentZone() == kZone100)
		_cursorHandler->select(kCursorMenuIdle);

	if (_bag->isInitialized()) {
		_cursorHandler->select(kCursorMenuIdle);
		objectPresentationHide(kObjectItemList);
		objectSetAccessibilityOff(kObjectItemList);
	}

	switch (getCurrentZone()) {
	default:
		break;

	case kZone100:
		_zoneSystem->onUpdateBag(point);
		break;

	case kZone1:
		_zone1->onUpdateBag(point);
		break;

	case kZone2:
		_zone2->onUpdateBag(point);
		break;

	case kZone3:
		_zone3->onUpdateBag(point);
		break;

	case kZone4:
		_zone4->onUpdateBag(point);
		break;

	case kZone6:
		_zone6->onUpdateBag(point);
		break;

	case kZone7:
		_zone7->onUpdateBag(point);
		break;

	case kZone8:
		_zone8->onUpdateBag(point);
		break;

	case kZone11:
		_zone11->onUpdateBag(point);
		break;

	case kZone12:
		_zone12->onUpdateBag(point);
		break;
	}
}

void ApplicationPompeii::onBagClickedObject(ObjectId id) {
	if (hasCurrentRotation())
		varSetDword(99028, getCurrentRotationId());

	switch (getCurrentZone()) {
	default:
		break;

	case kZone1:
		_zone1->onBagClickedObject(id);
		break;

	case kZone2:
	case kZone3:
		_zone3->onBagClickedObject(id);
		break;
	}

	// All zones also use the system event
	_zoneSystem->onBagClickedObject(id);
}

void ApplicationPompeii::onBeforeRide(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, MovabilityType movabilityType) {
	if (movabilityFrom == 1)
		return;

	if (hasCurrentRotation())
		varSetDword(99027, getCurrentRotationId());

	switch (getCurrentZone()) {
	default:
		break;

	case kZone1:
		_zone1->onBeforeRide(movabilityFrom, movabilityTo, movabilityIndex, target, movabilityType);
		break;

	case kZone2:
		_zone2->onBeforeRide(movabilityFrom, movabilityTo, movabilityIndex, target, movabilityType);
		break;

	case kZone10:
		_zone10->onBeforeRide(movabilityFrom, movabilityTo, movabilityIndex, target, movabilityType);
		break;
	}
}

void ApplicationPompeii::onAfterRide(Id movabilityFrom, Id movabilityTo, uint32 movabilityIndex, Id target, MovabilityType movabilityType) {
	if (movabilityFrom == 1)
		return;

	if (!varGetByte(90001))
		return;

	if (!checkBag()) {
		bagRemove(kObjectItemList);
		bagAdd(kObjectItemListCompleted);
	}

	switch (getCurrentZone()) {
	default:
		break;

	case kZone1:
		_zone1->onAfterRide(movabilityFrom, movabilityTo, movabilityIndex, target, movabilityType);
		break;

	case kZone2:
		_zone2->onAfterRide(movabilityFrom, movabilityTo, movabilityIndex, target, movabilityType);
		break;

	case kZone6:
		_zone6->onAfterRide(movabilityFrom, movabilityTo, movabilityIndex, target, movabilityType);
		break;

	case kZone11:
		_zone11->onAfterRide(movabilityFrom, movabilityTo, movabilityIndex, target, movabilityType);
		break;
	}
}

void ApplicationPompeii::onAnimationNextFrame(Id animationId, const Common::String &name, uint32 frame, uint32 frameCount) {
	switch (getCurrentZone()) {
	default:
		break;

	case kZone1:
		_zone1->onAnimationNextFrame(animationId, name, frame, frameCount);
		break;

	case kZone2:
		_zone2->onAnimationNextFrame(animationId, name, frame, frameCount);
		break;

	case kZone3:
		_zone3->onAnimationNextFrame(animationId, name, frame, frameCount);
		break;

	case kZone4:
		_zone4->onAnimationNextFrame(animationId, name, frame, frameCount);
		break;

	case kZone5:
		_zone5->onAnimationNextFrame(animationId, name, frame, frameCount);
		break;

	case kZone6:
		_zone6->onAnimationNextFrame(animationId, name, frame, frameCount);
		break;

	case kZone7:
		_zone7->onAnimationNextFrame(animationId, name, frame, frameCount);
		break;

	case kZone8:
		_zone8->onAnimationNextFrame(animationId, name, frame, frameCount);
		break;

	case kZone9:
		_zone9->onAnimationNextFrame(animationId, name, frame, frameCount);
		break;

	case kZone10:
		_zone10->onAnimationNextFrame(animationId, name, frame, frameCount);
		break;

	case kZone11:
		_zone11->onAnimationNextFrame(animationId, name, frame, frameCount);
		break;

	case kZone12:
		_zone12->onAnimationNextFrame(animationId, name, frame, frameCount);
		break;
	}
}

void ApplicationPompeii::onVisualList(Id id, uint32 type, const Common::Point &point) {
	switch (id) {
	default:
		break;

	case 1:
		if (type == 3) {
			Common::String userFolder = getObject(visualListGetObjectIdClicked(1, kPuzzleLoad))->getName();
			objectPresentationSetTextToPuzzle(99601, 1, 0, userFolder);
			objectPresentationSetAnimationCoordinatesOnPuzzle(99601, 0, Common::Point((int16)objectPresentationGetTextWidth(99601, 1, 0) + 30, 435));
			objectSetAccessibilityOn(kObject99025);
			objectPresentationHide(kObject99025, 1);
			varSetByte(98003, 1);
		}
		break;

	case 4:
		if (type == 3) {
			Common::String userFolder = getObject(visualListGetObjectIdClicked(1, kPuzzlePreferences))->getName();
			objectPresentationSetTextToPuzzle(99601, 1, 1, userFolder);
			objectPresentationShow(kObjectSaveName, 1);
			objectSetAccessibilityOn(kObject99023);
			objectPresentationHide(kObject99023, 1);
		}
		break;

	case 6:
		if (type == 6 || type == 7) {
			int x = point.x;
			if (type == 7 && point.y > 0)
				x += 100000;

			switch (getCurrentZone()) {
			default:
				break;

			case kZone1:
				_zone1->onVisual(x);
				break;

			case kZone2:
				_zone2->onVisual(x);
				break;

			case kZone3:
				_zone3->onVisual(x);
				break;

			case kZone4:
				_zone4->onVisual(x);
				break;

			case kZone6:
				_zone6->onVisual(x);
				break;

			case kZone7:
				_zone7->onVisual(x);
				break;

			case kZone8:
				_zone8->onVisual(x);
				break;

			case kZone11:
				_zone11->onVisual(x);
				break;

			case kZone12:
				_zone12->onVisual(x);
				break;
			}
		}
		break;
	}
}

#pragma endregion

#pragma region Helpers

uint32 ApplicationPompeii::checkBag() {
	uint32 count = 9;

	objectPresentationHide(kObjectItemList);

	for (uint32 i = 0; i < 9; i++) {
		ObjectId object = (ObjectId)(kObjectStones + i);
		if (bagHas(object)) {
			objectPresentationShow(kObjectItemList, 0);
			objectSetAccessibilityOn(kObjectItemList);
			bagRemove(object);

			for (uint32 j = 0; j < 9; j++) {
				if (varGetByte(91200 + j) > 0)
					objectPresentationShow(kObjectItemList, j);
			}

			uint32 startTick = g_system->getMillis();
			while (g_system->getMillis() - startTick < 1000)
				handleEvents();

			varSetByte(object, 1);
			objectPresentationShow(kObjectItemList, i + 1);
			soundPlay(990015);
		}

		if (varGetByte(object) > 0)
			--count;
	}

	return count;
}

void ApplicationPompeii::setCoordinatesOnPuzzle6() {
	for (int i = 0; i < ARRAYSIZE(_puzzle6Coordinates); i++) {
		if (varGetDword(99027) == _puzzle6Coordinates[i].val)
			objectPresentationSetAnimationCoordinatesOnPuzzle(kObject6,
															  5,
															  Common::Point((int16)(_puzzle6Coordinates[i].multiplierX * varGetByte(90509) - varGetWord(99501)),
																			(int16)(_puzzle6Coordinates[i].multiplierY * varGetByte(90509) - varGetWord(99502))));
	}
}

void ApplicationPompeii::showDay() {
	switch (varGetByte(90001)) {
	default:
		_message = "";
		break;

	case 0:
		messageGet("Day0");
		break;

	case 1:
		messageGet("Day1");
		break;

	case 2:
		messageGet("Day2");
		break;

	case 3:
		messageGet("Day3");
		break;

	case 4:
		messageGet("Day4");
		break;

	case 5:
		messageGet("Day5");
		break;
	}

	objectPresentationSetTextToPuzzle(kObject1, 1, 0, _message);
}

void ApplicationPompeii::setupUser(Id userId) {
	// Original setups per-user preferences

	setUser(Common::String::format("user%d", userId));
}

bool ApplicationPompeii::giveMoney(int16 amount) {
	if (varGetWord(99500) - amount < 0) {
		// Not enough money
		messageFormat("NotEnoughMoney", varGetWord(99500), amount);
		messageShowWarning(0);

		return false;
	}

	varSetWord(99500, varGetWord(99500) - amount);
	if (!varGetWord(99500))
		bagRemove(kObjectSesterces);

	return true;
}

void ApplicationPompeii::takeMoney(int16 amount) {
	varSetWord(99500, varGetWord(99500) + amount);
	if (!bagHas(kObjectSesterces))
		bagAdd(kObjectSesterces);
}

void ApplicationPompeii::showEncyclopedia(uint32 index) {
	_savedZone = getCurrentZone();
	setCurrentZone(kZone100);

	if (hasCurrentPuzzle()) {
		varSetByte(90905, 1);
		varSetDword(90026, getCurrentPuzzleId());
	} else {
		varSetByte(90905, 0);
		varSetDword(90026, getCurrentRotationId());
	}

	puzzleSetActive(kPuzzleEncyclopedia);
	visualEncyclopediaShowFile(5, kPuzzleEncyclopedia, Common::String::format("E%03d.out", index));
}

void ApplicationPompeii::restore() {
	if (varGetByte(90905) == 1)
		puzzleSetActive(varGetDword(90026));
	else
		rotationSetActive(varGetDword(90026));

	setSpace(_savedZone);
}

void ApplicationPompeii::loadSaveList(uint32 userId, Id visualId, PuzzleId puzzleId) {
	error("[ApplicationPompeii::loadSaveList] Not implemented");
}


void ApplicationPompeii::onCall(Id callType) {
	switch (callType) {
	default:
		warning("[ApplicationPompeii::onCall] Unknown call type: %d", callType);
		break;

	case 106:
	case 302:
	case 304:
	case 306:
	case 401:
	case 417:
	case 418:
	case 4161:
		// Nothing to do
		break;

	case 20:
		switch (varGetByte(90001)) {
		default:
			break;

		case 1:
			soundPlay(5001, kSoundLoop);
			break;

		case 2:
			soundPlay(5002, kSoundLoop);
			break;

		case 3:
			soundPlay(5003, kSoundLoop);
			break;

		case 4:
			soundPlay(5004, kSoundLoop);
			break;
		}
		break;

	case 30:
		soundStopType(kSoundTypeAmbientMusic, 1024);
		break;

	case 102:
		varSetByte(90903, varGetByte(90903) + 1);
		playMovie("S01A01-1");
		rotationSetMovabilityOff(10111, 0, 0);
		rotationSetMovabilityOff(10111, 1, 1);
		objectPresentationShow(kObject10007);
		objectSetAccessibilityOn(kObject10007);
		objectPresentationShow(kObject10008);
		objectSetAccessibilityOn(kObject10008);
		rotationSetAlp(10111, 180.0f);
		rotationSetBet(10111, 10.0f);
		break;

	case 103:
		playMovie("S01A01-2");
		objectPresentationShow(kObject10009);
		objectSetAccessibilityOn(kObject10009);
		varSetByte(10008, 1);
		break;

	case 104: {
		varSetByte(90903, varGetByte(90903) + 1);
		soundPlay(900001 + rnd(3));
		Id soundId = 1153 + rnd(2);
		soundPlay(soundId);
		soundSetVolume(soundId, 88);
		objectPresentationShow(kObject1001, 0);
		objectPresentationShow(kObject1001, 1);
		objectPresentationShow(kObject1001, 2);
		objectPresentationShow(kObject1005, 0);
		objectPresentationShow(kObject1006);
		}
		break;

	case 105:
		varSetByte(90903, varGetByte(90903) + 1);
		rotationSetMovabilityOn(10012, 0, 0);
		rotationSetMovabilityOn(10012, 1, 1);
		objectPresentationShow(kObject10071);
		objectSetAccessibilityOn(kObject10071, 0, 0);
		break;

	case 107:
		objectPresentationShow(kObject30301, 1);
		objectPresentationShow(kObject30301, 5);
		objectPresentationShow(kObject30301, 6);
		objectPresentationShow(kObject30301, 2);
		objectPresentationShow(kObject30301, 7);
		objectSetAccessibilityOn(kObject30311, 0, 2);
		objectSetAccessibilityOn(kObject30313, 0, 0);
		break;

	case 108:
		objectPresentationShow(kObject1012, 0);
		objectSetAccessibilityOn(kObject1012, 0, 0);
		break;

	case 109:
		varSetByte(90903, varGetByte(90903) + 1);
		objectPresentationHide(kObject30301, 1);
		objectPresentationHide(kObject30301, 5);
		objectPresentationHide(kObject30301, 6);
		objectPresentationHide(kObject30301, 2);
		objectPresentationHide(kObject30301, 7);
		objectPresentationShow(kObject30301, 0);
		objectPresentationShow(kObject30301, 3);
		objectPresentationShow(kObject30301, 8);
		objectPresentationShow(kObject30301, 4);
		objectPresentationShow(kObject30301, 9);
		objectSetAccessibilityOff(kObject30311);
		objectSetAccessibilityOff(kObject30313);
		objectSetAccessibilityOff(kObject30312);
		break;

	case 110:
		varSetByte(90903, varGetByte(90903) + 1);
		rotationSetMovabilityOn(10012, 2, 2);
		objectSetAccessibilityOn(kObject10046);
		objectPresentationHide(kObject50501);
		objectPresentationShow(kObject50501, 0);
		objectPresentationShow(kObject50501, 10);
		objectPresentationShow(kObject50501, 11);
		objectPresentationShow(kObject50501, 3);
		objectPresentationHide(kObject50502);
		objectSetAccessibilityOn(kObject50511, 0, 0);
		objectPresentationShow(kObject50502, 11);
		objectPresentationShow(kObject50502, 2);
		objectPresentationShow(kObject50502, 14);
		objectPresentationShow(kObject50502, 15);
		objectPresentationShow(kObject50502, 0);
		objectPresentationShow(kObject50502, 4);
		objectPresentationShow(kObject50502, 13);
		rotationSetMovabilityOff(10511, 0, 0);
		objectSetAccessibilityOn(kObject50521, 0, 0);
		break;

	case 112:
		objectSetAccessibilityOn(kObject10045);
		objectPresentationShow(kObject10405, 0);
		objectPresentationShow(kObject10405, 3);
		objectSetAccessibilityOn(kObject40412);
		objectPresentationShow(kObject10405, 1);
		objectPresentationShow(kObject10405, 4);
		objectSetAccessibilityOn(kObject40411);
		rotationSetMovabilityOff(10411);
		rotationSetMovabilityOff(10441, 1, 2);
		objectSetAccessibilityOn(kObject10045);
		break;

	case 113:
		varSetByte(90903, varGetByte(90903) + 1);
		onCall(30);
		setSpace(kZone3);
		rotationSetActive(102112);
		objectPresentationUnpauseAnimation(kObject20001, 0);
		objectPresentationUnpauseAnimation(kObject20001, 1);
		objectPresentationUnpauseAnimation(kObject20002, 0);
		objectPresentationUnpauseAnimation(kObject20002, 1);
		objectPresentationUnpauseAnimation(kObject20006, 0);
		objectPresentationUnpauseAnimation(kObject20007, 0);
		objectPresentationUnpauseAnimation(kObject20007, 1);
		objectPresentationUnpauseAnimation(kObject20001, 2);
		objectPresentationUnpauseAnimation(kObject20002, 2);
		objectPresentationUnpauseAnimation(kObject20007, 2);
		objectPresentationUnpauseAnimation(kObject10202, 1);
		soundPlay(20042, kSoundLoop);
		_zone3->onVisual(1);
		break;

	case 200:
		timerStopAll();
		varSetByte(90001, 2);
		showDay();
		rotationSetMovabilityOn(10011, 1, 1);
		rotationSetMovabilityOff(100113, 1, 1);
		rotationSetMovabilityOff(10015, 1, 1);
		rotationSetMovabilityOff(100110, 1, 1);
		rotationSetMovabilityOff(10032, 1, 1);
		rotationSetMovabilityOff(10242, 0, 0);
		objectSetAccessibilityOff(kObject10101);
		objectSetAccessibilityOff(kObject10045);
		objectSetAccessibilityOff(kObject10047);
		objectSetAccessibilityOff(kObject10048);
		objectSetAccessibilityOff(kObject10049);
		objectSetAccessibilityOff(kObject10050);
		objectSetAccessibilityOff(kObject10051);
		objectSetAccessibilityOff(kObject10052);
		onCall(30);
		setSpace(kZone2);
		rotationSetAlp(10122, 88.0f);
		rotationSetBet(10122, 0.0f);
		rotationSetRan(10122, 87.0f);
		rotationSetActive(10122);
		objectPresentationUnpauseAnimation(kObject10007, 0);
		objectPresentationUnpauseAnimation(kObject10007, 1);
		objectPresentationUnpauseAnimation(kObject10008, 0);
		objectPresentationUnpauseAnimation(kObject10008, 1);
		objectPresentationUnpauseAnimation(kObject10009, 1);
		objectPresentationUnpauseAnimation(kObject10010, 0);
		objectPresentationUnpauseAnimation(kObject10011, 0);
		objectPresentationUnpauseAnimation(kObject10011, 1);
		objectPresentationUnpauseAnimation(kObject10018, 0);
		objectPresentationUnpauseAnimation(kObject10018, 1);
		objectPresentationUnpauseAnimation(kObject10020, 0);
		objectPresentationUnpauseAnimation(kObject10020, 1);
		objectPresentationUnpauseAnimation(kObject10007, 2);
		objectPresentationUnpauseAnimation(kObject10009, 0);
		objectPresentationUnpauseAnimation(kObject10009, 2);
		objectPresentationUnpauseAnimation(kObject10011, 2);
		objectPresentationUnpauseAnimation(kObject10018, 2);
		objectPresentationShow(kObject10010);
		objectSetAccessibilityOn(kObject10010, 0, 0);
		rotationSetMovabilityOn(10012);
		break;

	case 202:
		if (varGetByte(90202) < 2) {
			playMovie("S00A01-8");
			objectPresentationShow(kObject1016, 0);
			objectPresentationShow(kObject1016, 1);
			puzzleSetActive(kPuzzle1015);
			soundPlay(2001);
		}
		break;

	case 203:
		varSetByte(90903, varGetByte(90903) + 1);
		playMovie("S00A03-1");
		puzzleSetActive(kPuzzle1016);
		soundPlay(2002);
		objectPresentationShow(kObject1017, 0);
		objectPresentationShow(kObject1017, 1);
		varSetByte(1008, 1);
		break;

	case 204:
		objectPresentationShow(kObjectBracelet, 0);
		objectSetAccessibilityOn(kObjectBracelet, 0, 0);
		break;

	case 205:
		varSetByte(90903, varGetByte(90903) + 1);
		playMovie("S00A03-2");
		objectPresentationShow(kObject1019, 0);
		objectPresentationShow(kObject1019, 1);
		objectPresentationShow(kObject1019, 2);
		objectSetAccessibilityOn(kObject1019, 1, 1);
		puzzleSetActive(kPuzzle1017);
		soundPlay(2005);
		break;

	case 206:
		objectSetAccessibilityOff(kObject10042);
		objectSetAccessibilityOff(kObject10043);
		rotationSetMovabilityOn(10015, 1, 1);
		rotationSetMovabilityOff(10017, 2, 2);
		objectPresentationShow(kObject1020, 0);
		objectPresentationShow(kObject1020, 1);
		objectSetAccessibilityOn(kObject1020, 0, 0);
		objectPresentationShow(kObject1021, 0);
		objectPresentationShow(kObject1021, 2);
		objectSetAccessibilityOn(kObject1021, 0, 0);
		objectPresentationShow(kObjectStones, 0);
		objectSetAccessibilityOn(kObjectStones, 0, 0);
		break;

	case 207:
		varSetByte(90903, varGetByte(90903) + 1);
		objectPresentationHide(kObject50501);
		objectPresentationHide(kObject50502);
		objectPresentationShow(kObject50501, 0);
		objectPresentationShow(kObject50501, 10);
		objectPresentationShow(kObject50501, 11);
		objectPresentationShow(kObject50501, 9);
		objectPresentationShow(kObject50501, 6);
		objectPresentationShow(kObject50501, 5);
		objectPresentationShow(kObject50501, 4);
		objectSetAccessibilityOn(kObject50511, 2, 2);
		objectPresentationShow(kObject10011);
		objectSetAccessibilityOn(kObject10011, 0, 0);
		objectPresentationShow(kObjectWine, 0);
		objectPresentationShow(kObjectLamp, 0);
		break;

	case 208:
		objectPresentationShow(kObject1023, 0);
		objectPresentationShow(kObject1023, 1);
		objectSetAccessibilityOn(kObject1023, 0, 0);
		break;

	case 209:
		varSetByte(90903, varGetByte(90903) + 1);
		objectSetAccessibilityOn(kObject60611, 0, 0);
		objectPresentationShow(kObject10601, 0);
		break;

	case 210:
		objectSetAccessibilityOn(kObject1039, 0, 0);
		break;

	case 211:
		varSetByte(90903, varGetByte(90903) + 1);
		objectSetAccessibilityOn(kObjectRose, 0, 0);
		objectSetAccessibilityOn(kObjectRose, 1, 1);
		objectSetAccessibilityOn(kObjectRose, 2, 2);
		break;

	case 212:
		rotationSetMovabilityOff(10711);
		objectPresentationShow(kObject10701, 0);
		objectPresentationShow(kObject10701, 7);
		objectPresentationShow(kObject10701, 8);
		objectPresentationShow(kObject10701, 1);
		objectPresentationShow(kObject10701, 3);
		objectSetAccessibilityOn(kObject70701, 0, 0);
		objectSetAccessibilityOn(kObject70702, 0, 0);
		objectSetAccessibilityOn(kObjectIbisFeather, 0, 0);
		objectSetAccessibilityOn(kObject10048);
		break;

	case 213:
		varSetByte(90903, varGetByte(90903) + 1);
		objectSetAccessibilityOn(kObject10053);
		objectPresentationShow(kObject11001, 0);
		objectPresentationShow(kObject11001, 11);
		objectPresentationShow(kObject11001, 1);
		objectPresentationShow(kObject11001, 6);
		objectPresentationShow(kObject11001, 8);
		objectSetAccessibilityOn(kObject11002, 0, 0);
		objectSetAccessibilityOn(kObjectMuleTooth);
		objectSetAccessibilityOn(kObject11004);
		break;

	case 214:
		objectSetAccessibilityOn(kObject1029, 0, 0);
		break;

	case 215:
		objectPresentationShow(kObject1031, 0);
		objectPresentationShow(kObject1031, 1);
		objectSetAccessibilityOn(kObject1031, 0, 0);
		objectSetAccessibilityOff(kObject10046);
		objectPresentationHide(kObject50501);
		objectPresentationHide(kObject50502);
		objectPresentationShow(kObject50501, 0);
		objectPresentationShow(kObject50501, 10);
		objectPresentationShow(kObject50501, 11);
		objectPresentationShow(kObject50501, 9);
		objectPresentationShow(kObject50502, 9);
		objectSetAccessibilityOn(kObject50511, 4, 4);
		objectSetAccessibilityOn(kObject50525, 0, 0);
		break;

	case 216:
		varSetByte(90903, varGetByte(90903) + 1);
		onCall(102001);
		rotationSetAlp(10018, 90.0f);
		objectSetAccessibilityOn(kObject10045);
		objectSetAccessibilityOff(kObject10043);
		objectPresentationHide(kObject1020, 0);
		objectPresentationHide(kObject1020, 1);
		objectPresentationShow(kObject1033, 0);
		objectPresentationShow(kObject1033, 1);
		objectSetAccessibilityOn(kObject1033, 0, 0);
		objectPresentationShow(kObject20001);
		objectSetAccessibilityOn(kObject20001, 0, 0);
		objectPresentationShow(kObject1021, 1);
		break;

	case 300:
		timerStopAll();
		varSetByte(90001, 3);
		showDay();
		rotationSetMovabilityOff(100113, 1, 1);
		rotationSetMovabilityOff(100110, 1, 1);
		rotationSetMovabilityOff(10032, 1, 1);
		objectSetAccessibilityOff(kObject10042);
		objectSetAccessibilityOff(kObject10043);
		objectSetAccessibilityOff(kObject10045);
		objectSetAccessibilityOff(kObject10048);
		objectSetAccessibilityOff(kObject10101);
		onCall(30);
		setSpace(kZone2);
		rotationSetAlp(10163, 270.0f);
		rotationSetActive(10163);
		objectPresentationShow(kObject10017, 0);
		objectPresentationShow(kObject1021, 1);
		objectPresentationShow(kObject1046, 0);
		objectPresentationShow(kObject1046, 1);
		objectPresentationUnpauseAnimation(kObject10007, 0);
		objectPresentationUnpauseAnimation(kObject10007, 1);
		objectPresentationUnpauseAnimation(kObject10008, 0);
		objectPresentationUnpauseAnimation(kObject10008, 1);
		objectPresentationUnpauseAnimation(kObject10009, 1);
		objectPresentationUnpauseAnimation(kObject10010, 0);
		objectPresentationUnpauseAnimation(kObject10011, 0);
		objectPresentationUnpauseAnimation(kObject10011, 1);
		objectPresentationUnpauseAnimation(kObject10018, 0);
		objectPresentationUnpauseAnimation(kObject10018, 1);
		objectPresentationUnpauseAnimation(kObject10020, 0);
		objectPresentationUnpauseAnimation(kObject10020, 1);
		objectPresentationUnpauseAnimation(kObject10007, 2);
		objectPresentationUnpauseAnimation(kObject10009, 0);
		objectPresentationUnpauseAnimation(kObject10009, 2);
		objectPresentationUnpauseAnimation(kObject10011, 2);
		objectPresentationUnpauseAnimation(kObject10018, 2);
		break;

	case 301:
		objectSetAccessibilityOff(kObject10102);
		objectSetAccessibilityOn(kObject10101);
		if (varGetByte(90003) == 1 && varGetByte(90004) != 0) {
			playMovie("S00A01-6");
			varSetByte(90004, 1);
			puzzleSetActive(kPuzzle1064);
			soundPlay(3002);
			varSetByte(90301, 2);
			onCall(302);
		}
		break;

	case 303:
		varSetByte(90903, varGetByte(90903) + 1);
		objectPresentationHide(kObject1039, 0);
		objectPresentationHide(kObject1039, 1);
		objectPresentationHide(kObject1040, 0);
		objectPresentationHide(kObject1035, 0);
		objectPresentationHide(kObject1041, 0);
		objectPresentationHide(kObjectCommercialAdd, 0);
		objectPresentationHide(kObjectBasket, 1);
		objectPresentationHide(kObjectBasket2, 1);
		objectPresentationShow(kObject1052, 0);
		objectPresentationShow(kObject1049, 0);
		objectPresentationShow(kObject1050, 0);
		objectPresentationShow(kObject1051, 0);
		objectPresentationShow(kObject1053, 0);
		objectPresentationShow(kObject1053, 1);
		break;

	case 305:
		varSetByte(90903, varGetByte(90903) + 1);
		rotationSetMovabilityOn(100110);
		objectPresentationShow(kObject10801, 0);
		objectPresentationShow(kObject10801, 3);
		objectPresentationShow(kObject10801, 1);
		objectSetAccessibilityOn(kObjectJavelin, 0, 0);
		break;

	case 307:
		varSetByte(90903, varGetByte(90903) + 1);
		objectPresentationShow(kObject1057, 0);
		objectPresentationShow(kObject1057, 1);
		objectPresentationShow(kObject1057, 2);
		objectSetAccessibilityOn(kObject1057, 0, 0);
		break;

	case 308:
		varSetByte(90903, varGetByte(90903) + 1);
		objectPresentationShow(kObject1059, 0);
		objectPresentationShow(kObject1059, 1);
		objectPresentationShow(kObject1059, 2);
		objectSetAccessibilityOn(kObject1059, 0, 0);
		objectPresentationShow(kObject10591, 0);
		objectPresentationShow(kObject10591, 1);
		rotationSetMovabilityOff(10046, 3, 3);
		break;

	case 309:
		objectSetAccessibilityOn(kObject10050);
		objectSetAccessibilityOn(kObject10051);
		objectSetAccessibilityOn(kObject10052);
		rotationSetMovabilityOn(10046, 3, 3);
		objectPresentationShow(kObject1061, 0);
		objectPresentationShow(kObject1061, 2);
		objectPresentationShow(kObject1061, 3);
		objectSetAccessibilityOn(kObject1061, 0, 0);
		objectPresentationShow(kObject1062, 0);
		objectPresentationShow(kObject1062, 1);
		objectSetAccessibilityOn(kObject10591, 0, 0);
		break;

	case 310:
		varSetByte(90903, varGetByte(90903) + 1);
		objectPresentationHide(kObject50501);
		objectPresentationHide(kObject50502);
		objectPresentationShow(kObject50501, 0);
		objectPresentationShow(kObject50501, 10);
		objectPresentationShow(kObject50501, 11);
		objectPresentationShow(kObject50501, 9);
		objectSetAccessibilityOn(kObject50511, 5, 5);
		objectPresentationShow(kObject50502, 4);
		objectPresentationShow(kObject50502, 10);
		objectPresentationShow(kObject50502, 16);
		objectSetAccessibilityOn(kObject50526, 0, 0);
		break;

	case 311:
		varSetByte(90903, varGetByte(90903) + 1);
		objectSetAccessibilityOn(kObject10042);
		objectSetAccessibilityOn(kObject10045);
		rotationSetMovabilityOff(10215);
		objectPresentationShow(kObject20006);
		objectSetAccessibilityOn(kObject20006, 0, 0);
		break;

	case 400:
		timerStopAll();
		varSetByte(90001, 4);
		showDay();
		rotationSetMovabilityOff(100113, 1, 1);
		rotationSetMovabilityOff(10032, 1, 1);
		objectSetAccessibilityOff(kObject10042);
		objectSetAccessibilityOff(kObject10043);
		objectSetAccessibilityOff(kObject10101);
		objectSetAccessibilityOff(kObject10102);
		objectSetAccessibilityOff(kObject10045);
		objectSetAccessibilityOff(kObject10054);
		onCall(30);
		setSpace(kZone2);
		rotationSetActive(10122);
		objectPresentationShow(kObject1021, 1);
		objectPresentationShow(kObject10018, 0);
		objectPresentationShow(kObject10018, 1);
		objectPresentationShow(kObject10018, 2);
		objectSetAccessibilityOn(kObject10018, 0, 0);
		objectPresentationShow(kObjectRedFish, 0);
		objectPresentationUnpauseAnimation(kObject10007, 0);
		objectPresentationUnpauseAnimation(kObject10007, 1);
		objectPresentationUnpauseAnimation(kObject10008, 0);
		objectPresentationUnpauseAnimation(kObject10008, 1);
		objectPresentationUnpauseAnimation(kObject10009, 1);
		objectPresentationUnpauseAnimation(kObject10010, 0);
		objectPresentationUnpauseAnimation(kObject10011, 0);
		objectPresentationUnpauseAnimation(kObject10011, 1);
		objectPresentationUnpauseAnimation(kObject10018, 0);
		objectPresentationUnpauseAnimation(kObject10018, 1);
		objectPresentationUnpauseAnimation(kObject10020, 0);
		objectPresentationUnpauseAnimation(kObject10020, 1);
		objectPresentationUnpauseAnimation(kObject10007, 2);
		objectPresentationUnpauseAnimation(kObject10009, 0);
		objectPresentationUnpauseAnimation(kObject10009, 2);
		objectPresentationUnpauseAnimation(kObject10011, 2);
		objectPresentationUnpauseAnimation(kObject10018, 2);
		break;

	case 402:
		varSetByte(90903, varGetByte(90903) + 1);
		objectPresentationShow(kObject1064, 0);
		objectPresentationShow(kObject1064, 1);
		break;

	case 403:
		objectPresentationHide(kObject1053, 0);
		objectPresentationHide(kObject1053, 1);
		objectPresentationHide(kObject1055, 0);
		objectPresentationHide(kObject1050, 0);
		objectPresentationHide(kObject1051, 0);
		objectPresentationShow(kObject1066, 0);
		objectPresentationShow(kObject1066, 1);
		objectSetAccessibilityOn(kObject1066, 0, 0);
		objectPresentationShow(kObject1067, 0);
		break;

	case 404:
		varSetByte(90903, varGetByte(90903) + 1);
		objectPresentationHide(kObject50501);
		objectPresentationHide(kObject50502);
		objectPresentationShow(kObject50501, 0);
		objectPresentationShow(kObject50501, 10);
		objectPresentationShow(kObject50501, 11);
		objectPresentationShow(kObject50501, 9);
		objectPresentationShow(kObject50502, 10);
		objectPresentationShow(kObject50502, 16);
		objectPresentationShow(kObject50502, 4);
		objectSetAccessibilityOn(kObject50511, 6, 6);
		objectSetAccessibilityOn(kObject50526, 1, 1);
		break;

	case 405:
		rotationSetMovabilityOn(10032, 1, 1);
		objectPresentationShow(kObject1068, 0);
		objectPresentationShow(kObject1068, 1);
		objectPresentationShow(kObject1068, 2);
		objectSetAccessibilityOn(kObject1068, 0, 0);
		break;

	case 406:
		varSetByte(90903, varGetByte(90903) + 1);
		objectPresentationShow(kObject1077, 0);
		objectPresentationShow(kObject1077, 1);
		objectPresentationShow(kObject1077, 2);
		objectSetAccessibilityOn(kObject1077, 0, 0);
		break;

	case 407:
		objectPresentationShow(kObject11101, 1);
		objectPresentationShow(kObject11101, 2);
		objectSetAccessibilityOn(kObjectMaskAndCoat);
		objectSetAccessibilityOn(kObjectCapsa);
		objectPresentationShow(kObject11101, 4);
		objectPresentationShow(kObject11101, 6);
		objectSetAccessibilityOff(kObject111001);
		objectSetAccessibilityOn(kObject111001, 0, 0);
		break;

	case 408:
		rotationSetMovabilityOff(11011);
		objectPresentationShow(kObject11001, 0);
		objectPresentationShow(kObject11001, 11);
		objectPresentationShow(kObject11001, 8);
		objectPresentationShow(kObject11001, 2);
		objectPresentationShow(kObject11001, 9);
		objectPresentationShow(kObject11001, 7);
		objectSetAccessibilityOn(kObject11002, 1, 1);
		objectSetAccessibilityOn(kObjectFabricAndFlour);
		break;

	case 409:
		objectPresentationShow(kObject1079, 0);
		objectPresentationShow(kObject1078, 0);
		break;

	case 410:
		objectPresentationShow(kObject11101, 3);
		objectPresentationShow(kObject11101, 4);
		objectPresentationShow(kObject11101, 7);
		objectSetAccessibilityOff(kObject111001);
		objectSetAccessibilityOn(kObject111001, 2, 2);
		break;

	case 411:
		rotationSetMovabilityOff(10711);
		objectPresentationShow(kObject10701, 0);
		objectPresentationShow(kObject10701, 7);
		objectPresentationShow(kObject10701, 8);
		objectPresentationShow(kObject10701, 1);
		objectSetAccessibilityOn(kObject70701, 3, 3);
		objectSetAccessibilityOn(kObject70702, 1, 1);
		objectSetAccessibilityOn(kObject70703);
		break;

	case 412:
		varSetByte(90903, varGetByte(90903) + 1);
		objectPresentationHide(kObject50501);
		objectPresentationHide(kObject50502);
		objectPresentationShow(kObject50501, 0);
		objectPresentationShow(kObject50501, 10);
		objectPresentationShow(kObject50501, 11);
		objectPresentationShow(kObject50501, 9);
		objectPresentationShow(kObject50502, 4);
		objectPresentationShow(kObject50502, 10);
		objectPresentationShow(kObject50502, 16);
		objectSetAccessibilityOn(kObject50511, 8, 8);
		objectSetAccessibilityOn(kObject50526, 3, 3);
		break;

	case 413:
		objectPresentationShow(kObject10018, 0);
		objectPresentationShow(kObject10018, 1);
		objectPresentationShow(kObject10018, 2);
		objectSetAccessibilityOn(kObject10018, 2, 2);
		varSetByte(90411, 2);
		break;

	case 414:
		varSetByte(90903, varGetByte(90903) + 1);
		fadeOut(15, Color(0, 0, 0), 0);
		rotationSetAlp(10141, 280.0f);
		rotationSetBet(10141, 10.0f);
		rotationSetActive(10141);
		objectPresentationShow(kObject10020);
		objectSetAccessibilityOn(kObject10020, 0, 0);
		break;

	case 415:
		objectPresentationShow(kObject10018, 0);
		objectPresentationShow(kObject10018, 1);
		objectPresentationShow(kObject10018, 2);
		objectSetAccessibilityOn(kObject10018, 4, 4);
		break;

	case 416:
		varSetByte(90903, varGetByte(90903) + 1);
		objectSetAccessibilityOff(kObject10042);
		objectSetAccessibilityOff(kObject10044);
		objectSetAccessibilityOff(kObject10045);
		objectSetAccessibilityOff(kObject10046);
		objectSetAccessibilityOff(kObject10047);
		objectSetAccessibilityOff(kObject10048);
		objectSetAccessibilityOff(kObject10049);
		objectSetAccessibilityOff(kObject10050);
		objectSetAccessibilityOff(kObject10051);
		objectSetAccessibilityOff(kObject10052);
		objectSetAccessibilityOff(kObject10053);
		objectSetAccessibilityOff(kObject10054);
		rotationSetMovabilityOff(100322, 1, 1);
		soundStopType(kSoundTypeAmbientMusic, 1024);
		onCall(102002999);
		varSetByte(90416, 1);
		break;

	case 419:
		rotationSetMovabilityOn(1001102, 0, 0);
		rotationSetMovabilityOn(1001102, 1, 1);
		varSetByte(90419, 1);
		objectPresentationShow(kObject1080, 0);
		objectSetAccessibilityOn(kObject1080, 0, 0);
		objectPresentationShow(kObject1081, 0);
		objectPresentationShow(kObject1081, 2);
		break;

	case 420:
		rotationSetMovabilityOn(100322, 1, 1);
		objectPresentationShow(kObject1084, 0);
		objectSetAccessibilityOn(kObject1084, 0, 0);
		break;

	case 421:
		varSetByte(90903, varGetByte(90903) + 1);
		onCall(10100999);
		rotationSetMovabilityOff(10011203, 0, 0);
		objectPresentationShow(kObject1093, 0);
		objectPresentationShow(kObject1093, 1);
		objectSetAccessibilityOn(kObject1089);
		soundPlay(1168 + rnd(2));
		break;

	case 666:
		setSpace(kZone6);
		puzzleSetActive(kPuzzle505213);
		objectPresentationHide(kObjectTali);
		varSetByte(50002, 0);
		varSetByte(50003, 0);
		objectSetAccessibilityOff(kObjectTali, 0, 0);
		objectSetAccessibilityOn(kObjectCubes, 2, 2);
		varSetByte(50616, 0);
		break;

	case 667:
		setSpace(kZone6);
		puzzleSetActive(kPuzzle505213);
		objectPresentationHide(kObjectTali);

		for (uint32 i = 0; i < 4; i++) {
			uint32 presentationIndex = 0;
			while (presentationIndex == 0)
				presentationIndex = rnd(10);

			objectPresentationSetImageCoordinatesOnPuzzle(kObjectTali, presentationIndex + 36, Common::Point((int16)(20 + rnd(20)), (int16)(400 + rnd(20))));
			objectPresentationShow(kObjectTali, presentationIndex + 36);

			presentationIndex = 0;
			while (presentationIndex == 0)
				presentationIndex = rnd(10);

			objectPresentationSetImageCoordinatesOnPuzzle(kObjectTali, presentationIndex + 36, Common::Point((int16)(20 + rnd(20)), (int16)(40 + rnd(20))));
			objectPresentationShow(kObjectTali, presentationIndex + 36);
		}

		varSetByte(50002, 0);
		varSetByte(50003, 0);
		objectSetAccessibilityOn(kObjectCubes, 1, 1);
		objectSetAccessibilityOff(kObjectTali, 0, 0);
		varSetByte(50616, 1);
		break;

	case 777:
		switch (varGetDword(99028)) {
		default:
			showEncyclopedia(1);
			error("[ApplicationPompeii::onCall] Missing code");
			//break;

		case 10011:
			showEncyclopedia(101);
			break;

		case 10013:
		case 10016:
			showEncyclopedia(201);
			break;

		case 10018:
			showEncyclopedia(203);
			break;

		case 10021:
			showEncyclopedia(206);
			break;

		case 10022:
			showEncyclopedia(207);
			break;

		case 10023:
			showEncyclopedia(208);
			break;

		case 10024:
			showEncyclopedia(209);
			break;

		case 10025:
			showEncyclopedia(210);
			break;

		case 10026:
			showEncyclopedia(211);
			break;

		case 10041:
		case 10042:
			showEncyclopedia(213);
			break;

		case 10043:
		case 10044:
		case 10045:
		case 10046:
			showEncyclopedia(96);
			break;

		case 10031:
			showEncyclopedia(212);
			break;

		case 10012:
		case 10014:
		case 10017:
			showEncyclopedia(90);
			break;

		case 10015:
			showEncyclopedia(202);
			break;

		case 10019:
		case 10011002:
			showEncyclopedia(204);
			break;

		case 10032:
		case 10051:
		case 10053:
		case 10054:
		case 10055:
			showEncyclopedia(120);
			break;

		case 10052:
		case 1005202:
			showEncyclopedia(214);
			break;

		case 10101:
		case 10111:
		case 10121:
		case 10122:
		case 10123:
		case 10131:
		case 10141:
		case 10151:
		case 10161:
		case 10162:
		case 10163:
		case 10171:
		case 10181:
		case 10191:
			showEncyclopedia(101);
			break;

		case 10211:
		case 10221:
		case 10231:
		case 10232:
			showEncyclopedia(93);
			break;

		case 10241:
			showEncyclopedia(65);
			break;

		case 10242:
			showEncyclopedia(65);
			break;

		case 10301:
		case 10302:
			showEncyclopedia(72);
			break;

		case 10411:
		case 10431:
		case 10441:
		case 10451:
			showEncyclopedia(215);
			break;

		case 10521:
		case 10511:
			showEncyclopedia(44);
			break;

		case 10611:
		case 10612:
		case 10621:
			showEncyclopedia(108);
			break;

		case 10711:
		case 10712:
		case 10713:
		case 10721:
			showEncyclopedia(103);
			break;

		case 10811:
		case 10812:
		case 10813:
			showEncyclopedia(97);
			break;

		case 10911:
		case 10912:
		case 10921:
		case 10922:
		case 10931:
			showEncyclopedia(96);
			break;

		case 11011:
		case 11021:
			showEncyclopedia(18);
			break;

		case 100111:
		case 10011101:
		case 10011102:
			showEncyclopedia(69);
			break;
		}
		break;

	case 888:
		switch (getCurrentRotationId()) {
		default:
			soundPlay(990011 + rnd(4));
			break;

		case 10051:
		case 10053:
		case 10054:
		case 10055:
		case 10056:
		case 10057:
		case 10059:
		case 10621:
		case 10622:
		case 10811:
		case 10812:
		case 10813:
		case 10814:
		case 10815:
		case 10816:
		case 100511:
		case 100514:
		case 100515:
		case 1005102:
		case 1005302:
		case 1005402:
		case 1005502:
		case 1005602:
		case 1005702:
		case 1005902:
		case 10051102:
		case 10051402:
		case 10051502:
			soundPlay(990001 + rnd(4));
			break;
		}
		break;

	case 999:
		setSpace(kZone6);
		varSetByte(50003, 0);
		varSetByte(50002, 0);
		puzzleSetActive(kPuzzle505213);
		objectPresentationShow(kObjectTali, 20 + rnd(4));
		objectPresentationShow(kObjectTali, 24 + rnd(4));
		objectPresentationShow(kObjectTali, 28 + rnd(4));
		objectPresentationShow(kObjectTali, 32 + rnd(4));
		objectSetAccessibilityOff(kObjectCubes, 0, 0);
		objectSetAccessibilityOn(kObjectTali, 0, 0);
		break;

	case 1039:
		objectPresentationHide(kObject10007);
		objectPresentationHide(kObject10008);
		objectPresentationHide(kObject10009);
		break;

	case 1059:
		objectSetAccessibilityOff(kObject1008);
		objectPresentationHide(kObject1009);
		objectSetAccessibilityOff(kObject1009);
		break;

	case 1089:
		objectPresentationHide(kObject1012, 0);
		objectSetAccessibilityOff(kObject1012);
		break;

	case 1091:
		objectPresentationShow(kObject1013);
		objectSetAccessibilityOn(kObject1013, 0, 0);
		break;

	case 1099:
		objectPresentationHide(kObject1013);
		break;

	case 1129:
		objectPresentationHide(kObject10405);
		objectPresentationShow(kObject10405, 2);
		objectSetAccessibilityOff(kObject10045);
		break;

	case 2039:
		objectPresentationHide(kObject1017, 0);
		objectPresentationHide(kObject1017, 1);
		break;

	case 2061:
		rotationSetMovabilityOn(10017, 2, 2);
		break;

	case 2079:
		objectPresentationHide(kObject50501);
		objectPresentationShow(kObject50501, 9);
		objectPresentationHide(kObject50502);
		objectSetAccessibilityOff(kObject50511);
		objectSetAccessibilityOff(kObjectIncenseStick);
		objectSetAccessibilityOff(kObjectLamp2);
		objectSetAccessibilityOff(kObjectPureWine);
		objectPresentationHide(kObject10011);
		objectSetAccessibilityOff(kObject10011, 0, 0);
		objectPresentationHide(kObjectWine, 0);
		objectPresentationHide(kObjectLamp, 0);
		objectSetAccessibilityOff(kObjectLamp, 0, 0);
		objectSetAccessibilityOff(kObject10012, 0, 0);
		objectSetAccessibilityOff(kObjectWine, 0, 0);
		break;

	case 2089:
		objectPresentationHide(kObject1023, 0);
		objectPresentationHide(kObject1023, 1);
		break;

	case 2099:
		objectPresentationHide(kObject10010);
		break;

	case 2129:
		rotationSetMovabilityOff(10712);
		break;

	case 2161:
		rotationSetMovabilityOn(10211, 0, 0);
		rotationSetMovabilityOn(10242, 0, 0);
		objectSetAccessibilityOn(kObject10042);
		objectSetAccessibilityOn(kObject10201);
		objectSetAccessibilityOff(kObject10202);
		break;

	case 2169:
		objectPresentationHide(kObject1033, 0);
		objectPresentationHide(kObject1033, 1);
		objectPresentationHide(kObject20002);
		objectSetAccessibilityOff(kObject20002, 0, 0);
		objectSetAccessibilityOff(kObject10045);
		objectSetAccessibilityOff(kObjectRose, 1, 1);
		objectSetAccessibilityOff(kObjectRose, 2, 2);
		break;

	case 2340:
		setCurrentZone(kZone100);
		displayFade("black2.bmp", "JourNuit_N.bmp", 5, 0, kLoadFromDisk, _configuration.artSY ? kArchiveArt : kArchiveFile);
		displayFade("JourNuit_N.bmp", "JourNuit_N.bmp", 15, 0, kLoadFromDisk, _configuration.artSY ? kArchiveArt : kArchiveFile);
		fadeOut(5, Color(0, 0, 0), 0);
		displayFade("black2.bmp", "black2.bmp", 30, 0, kLoadFromDisk, _configuration.artSY ? kArchiveArt : kArchiveFile);
		displayFade("black2.bmp", "JourNuit_J.bmp", 5, 0, kLoadFromDisk, _configuration.artSY ? kArchiveArt : kArchiveFile);
		displayFade("JourNuit_J.bmp", "JourNuit_J.bmp", 10, 0, kLoadFromDisk, _configuration.artSY ? kArchiveArt : kArchiveFile);
		break;

	case 3019:
		objectPresentationHide(kObject10017, 0);
		objectPresentationHide(kObject1046, 0);
		objectPresentationHide(kObject1046, 1);
		break;

	case 3039:
		objectSetAccessibilityOn(kObject10049);
		objectPresentationHide(kObject1049, 0);
		objectPresentationHide(kObject1052, 0);
		objectPresentationHide(kObject1054, 0);
		objectPresentationHide(kObject1054, 1);
		break;

	case 3079:
		objectPresentationHide(kObject1057, 0);
		objectPresentationHide(kObject1057, 1);
		objectPresentationHide(kObject1057, 2);
		break;

	case 3091:
		varSetByte(90903, varGetByte(90903) + 1);
		objectPresentationShow(kObject10901, 0);
		rotationSetMovabilityOff(10922, 1, 1);
		objectPresentationShow(kObject10901, 0);
		objectPresentationShow(kObject10901, 5);
		objectSetAccessibilityOn(kObjectWood);
		objectSetAccessibilityOn(kObject99902);
		objectSetAccessibilityOn(kObject99903);
		objectSetAccessibilityOn(kObject99904);
		break;

	case 3099:
		objectPresentationHide(kObject1059, 0);
		objectPresentationHide(kObject1059, 1);
		objectPresentationHide(kObject1059, 2);
		objectSetAccessibilityOff(kObject1059);
		objectPresentationHide(kObject1062, 0);
		objectPresentationHide(kObject1062, 1);
		objectPresentationHide(kObject10591, 0);
		objectPresentationHide(kObject10591, 1);
		objectPresentationHide(kObject1061, 1);
		objectPresentationHide(kObject10901, 0);
		objectPresentationHide(kObject10901, 4);
		break;

	case 3111:
		objectPresentationShow(kObject20007);
		objectSetAccessibilityOn(kObject20007, 0, 0);
		break;

	case 3119:
		objectPresentationHide(kObject20007);
		objectPresentationHide(kObject20006);
		break;

	case 4019:
	case 4139:
		objectPresentationHide(kObject10018, 0);
		objectPresentationHide(kObject10018, 1);
		objectPresentationHide(kObject10018, 2);
		break;

	case 4029:
		objectPresentationHide(kObject1064, 0);
		objectPresentationHide(kObject1064, 1);
		break;

	case 4039:
		objectPresentationHide(kObject1066, 0);
		objectPresentationHide(kObject1066, 1);
		objectPresentationHide(kObject1067, 0);
		break;

	case 4059:
		objectPresentationHide(kObject1068, 0);
		objectPresentationHide(kObject1068, 1);
		objectPresentationHide(kObject1068, 2);
		break;

	case 4081:
		objectSetAccessibilityOn(kObject11006, 1, 1);
		break;

	case 4099:
		objectPresentationHide(kObject1079, 0);
		objectPresentationHide(kObject1078, 0);
		break;

	case 4109:
		objectSetAccessibilityOn(kObject10048);
		break;

	case 4149:
		objectPresentationHide(kObject10020);
		break;

	case 4159:
		objectPresentationHide(kObject10018, 0);
		objectPresentationHide(kObject10018, 1);
		objectPresentationHide(kObject10018, 2);
		objectPresentationHide(kObjectRedFish, 0);
		break;

	case 4199:
		objectPresentationHide(kObject1081, 1);
		break;

	case 10001:
		setSpace(kZone2);
		rotationSetAlp(10111, 0.0f);
		rotationSetBet(10111, 0.0f);
		rotationSetRan(10111, 87.0f);
		rotationSetActive(10111);
		objectPresentationUnpauseAnimation(kObject10007, 0);
		objectPresentationUnpauseAnimation(kObject10007, 1);
		objectPresentationUnpauseAnimation(kObject10008, 0);
		objectPresentationUnpauseAnimation(kObject10008, 1);
		objectPresentationUnpauseAnimation(kObject10009, 1);
		objectPresentationUnpauseAnimation(kObject10010, 0);
		objectPresentationUnpauseAnimation(kObject10011, 0);
		objectPresentationUnpauseAnimation(kObject10011, 1);
		objectPresentationUnpauseAnimation(kObject10018, 0);
		objectPresentationUnpauseAnimation(kObject10018, 1);
		objectPresentationUnpauseAnimation(kObject10020, 0);
		objectPresentationUnpauseAnimation(kObject10020, 1);
		objectPresentationUnpauseAnimation(kObject10007, 2);
		objectPresentationUnpauseAnimation(kObject10009, 0);
		objectPresentationUnpauseAnimation(kObject10009, 2);
		objectPresentationUnpauseAnimation(kObject10011, 2);
		objectPresentationUnpauseAnimation(kObject10018, 2);
		soundPlay(900001 + rnd(5));
		onCall(888);
		break;

	case 10003:
		setSpace(kZone4);
		rotationSetAlp(10301, 0.0f);
		rotationSetBet(10301, 0.0f);
		rotationSetRan(10301, 87.0f);
		rotationSetActive(10301);
		objectPresentationUnpauseAnimation(kObject30301, 5);
		objectPresentationUnpauseAnimation(kObject30301, 7);
		objectPresentationUnpauseAnimation(kObject30301, 8);
		objectPresentationUnpauseAnimation(kObject30301, 9);
		objectPresentationUnpauseAnimation(kObject30301, 1);
		objectPresentationUnpauseAnimation(kObject30301, 2);
		objectPresentationUnpauseAnimation(kObject30301, 3);
		objectPresentationUnpauseAnimation(kObject30301, 4);
		objectPresentationUnpauseAnimation(kObject30301, 6);
		soundPlay(900001);
		onCall(888);
		break;

	case 10004:
		setSpace(kZone5);
		rotationSetAlp(10411, 0.0f);
		rotationSetBet(10411, 0.0f);
		rotationSetRan(10411, 87.0f);
		rotationSetActive(10411);
		objectPresentationUnpauseAnimation(kObject10405, 4);
		objectPresentationUnpauseAnimation(kObject10405, 3);
		soundPlay(900001 + rnd(5));
		onCall(888);
		break;

	case 10005:
		setSpace(kZone6);
		rotationSetAlp(10511, 0.0f);
		rotationSetBet(10511, 0.0f);
		rotationSetRan(10511, 87.0f);
		rotationSetActive(10511);
		objectPresentationUnpauseAnimation(kObject50501, 11);
		objectPresentationUnpauseAnimation(kObject50502, 15);
		objectPresentationUnpauseAnimation(kObject50502, 16);
		objectPresentationUnpauseAnimation(kObject50501, 0);
		objectPresentationUnpauseAnimation(kObject50501, 10);
		objectPresentationUnpauseAnimation(kObject50502, 0);
		objectPresentationUnpauseAnimation(kObject50502, 2);
		objectPresentationUnpauseAnimation(kObject50502, 9);
		objectPresentationUnpauseAnimation(kObject50502, 10);
		objectPresentationUnpauseAnimation(kObject50502, 11);
		objectPresentationUnpauseAnimation(kObject50502, 14);
		soundPlay(900001 + rnd(5));
		onCall(888);
		break;

	case 10006:
		setSpace(kZone7);
		rotationSetAlp(10611, 265.0f);
		rotationSetBet(10611, 0.0f);
		rotationSetRan(10611, 87.0f);
		rotationSetActive(10611);
		objectPresentationUnpauseAnimation(kObject10601, 0);
		soundPlay(900001 + rnd(5));
		onCall(888);
		break;

	case 10007:
		setSpace(kZone8);
		rotationSetAlp(10711, 270.0f);
		rotationSetBet(10711, 0.0f);
		rotationSetRan(10711, 87.0f);
		rotationSetActive(10711);
		objectPresentationUnpauseAnimation(kObject10701, 0);
		objectPresentationUnpauseAnimation(kObject10701, 7);
		objectPresentationUnpauseAnimation(kObject10701, 8);
		soundPlay(900001 + rnd(5));
		onCall(888);
		break;

	case 10008:
		setSpace(kZone9);
		rotationSetAlp(10811, 180.0f);
		rotationSetBet(10811, 0.0f);
		rotationSetRan(10811, 87.0f);
		rotationSetActive(10811);
		if (varGetByte(90001) <= 0) {
			soundPlay(900001 + rnd(5));
		} else if (!varGetByte(90304)) {
			varSetByte(90304, 1);
			soundPlay(3016);
		} else {
			soundPlay(900001 + rnd(5));
		}

		objectPresentationUnpauseAnimation(kObject10801, 0);
		objectPresentationUnpauseAnimation(kObject10801, 3);
		onCall(888);
		break;

	case 10010:
		setSpace(kZone11);
		rotationSetAlp(11011, 320.0f);
		rotationSetBet(11011, 0.0f);
		rotationSetRan(11011, 87.0f);
		rotationSetActive(11011);
		objectPresentationUnpauseAnimation(kObject11001, 8);
		objectPresentationUnpauseAnimation(kObject11001, 9);
		objectPresentationUnpauseAnimation(kObject11001, 10);
		objectPresentationUnpauseAnimation(kObject11001, 11);
		soundPlay(900001 + rnd(5));
		onCall(888);
		break;

	case 10011:
		setSpace(kZone12);
		rotationSetAlp(11122, 90.0f);
		rotationSetBet(11122, 0.0f);
		rotationSetRan(11122, 87.0f);
		rotationSetActive(11122);
		objectPresentationUnpauseAnimation(kObject11101, 6);
		objectPresentationUnpauseAnimation(kObject11101, 7);
		objectPresentationUnpauseAnimation(kObject111001, 0);
		soundPlay(900001 + rnd(5));
		onCall(888);
		break;

	case 10100:
		setSpace(kZone1);
		rotationSetAlp(10011, 180.0f);
		rotationSetBet(10011, 0.0f);
		rotationSetRan(10011, 87.0f);
		rotationSetActive(10011);
		_zone1->onVisual(9);
		soundPlay(900001 + rnd(5));
		onCall(888);
		onCall(20);
		break;

	case 10300:
		setSpace(kZone1);
		rotationSetAlp(10023, 270.0f);
		rotationSetBet(10023, 0.0f);
		rotationSetRan(10023, 87.0f);
		rotationSetActive(10023);
		_zone1->onVisual(9);
		soundPlay(900001 + rnd(5));
		onCall(888);
		onCall(20);

		if (varGetByte(90001) == 1
		 && varGetByte(90109) < 2
		 && varGetByte(90107) >= 2)
			soundPlay(1064);
		break;

	case 10400:
		setSpace(kZone1);
		rotationSetAlp(10026, 90.0f);
		rotationSetBet(10026, 0.0f);
		rotationSetRan(10026, 87.0f);
		rotationSetActive(10026);
		_zone1->onVisual(9);
		soundPlay(900001 + rnd(5));
		onCall(888);
		onCall(20);
		break;

	case 10500:
		setSpace(kZone1);
		rotationSetAlp(10016, 270.0f);
		rotationSetBet(10016, 0.0f);
		rotationSetRan(10016, 87.0f);
		rotationSetActive(10016);
		_zone1->onVisual(9);
		soundPlay(900001 + rnd(5));
		onCall(888);
		onCall(20);
		break;

	case 10600:
		setSpace(kZone1);
		rotationSetAlp(10024, 86.0f);
		rotationSetBet(10024, 0.0f);
		rotationSetRan(10024, 87.0f);
		rotationSetActive(10024);
		_zone1->onVisual(9);
		soundPlay(900001 + rnd(5));
		onCall(888);
		onCall(20);
		break;

	case 10700:
		setSpace(kZone1);
		rotationSetAlp(10031, 270.0f);
		rotationSetBet(10031, 0.0f);
		rotationSetRan(10031, 87.0f);
		rotationSetActive(10031);
		_zone1->onVisual(9);
		soundPlay(900001 + rnd(5));
		onCall(888);
		onCall(20);
		break;

	case 10800:
		setSpace(kZone1);
		rotationSetAlp(10042, 0.0f);
		rotationSetBet(10042, 0.0f);
		rotationSetRan(10042, 87.0f);
		rotationSetActive(10042);
		_zone1->onVisual(9);
		soundPlay(900001 + rnd(5));
		onCall(888);
		onCall(20);
		break;

	case 11000:
		setSpace(kZone1);
		rotationSetAlp(10015, 270.0f);
		rotationSetBet(10015, 0.0f);
		rotationSetRan(10015, 87.0f);
		rotationSetActive(10015);
		_zone1->onVisual(9);
		soundPlay(900001 + rnd(5));
		onCall(888);
		onCall(20);
		break;

	case 11100:
		setSpace(kZone1);
		rotationSetAlp(10052, 180.0f);
		rotationSetBet(10052, 0.0f);
		rotationSetRan(10052, 87.0f);
		rotationSetActive(10052);
		_zone1->onVisual(9);
		soundPlay(900001 + rnd(5));
		onCall(888);
		onCall(20);
		break;

	case 100021:
		setSpace(kZone3);
		rotationSetAlp(10215, 0.0f);
		rotationSetBet(10215, 0.0f);
		rotationSetRan(10215, 87.0f);
		rotationSetActive(10215);
		objectPresentationUnpauseAnimation(kObject20001, 0);
		objectPresentationUnpauseAnimation(kObject20001, 1);
		objectPresentationUnpauseAnimation(kObject20002, 0);
		objectPresentationUnpauseAnimation(kObject20002, 1);
		objectPresentationUnpauseAnimation(kObject20006, 0);
		objectPresentationUnpauseAnimation(kObject20007, 0);
		objectPresentationUnpauseAnimation(kObject20007, 1);
		objectPresentationUnpauseAnimation(kObject20001, 2);
		objectPresentationUnpauseAnimation(kObject20002, 2);
		objectPresentationUnpauseAnimation(kObject20007, 2);
		objectPresentationUnpauseAnimation(kObject10202, 1);
		soundPlay(900005);
		break;

	case 100022:
		setSpace(kZone3);
		rotationSetAlp(10242, 180.0f);
		rotationSetBet(10242, 0.0f);
		rotationSetRan(10242, 87.0f);
		rotationSetActive(10242);
		objectPresentationUnpauseAnimation(kObject20001, 0);
		objectPresentationUnpauseAnimation(kObject20001, 1);
		objectPresentationUnpauseAnimation(kObject20002, 0);
		objectPresentationUnpauseAnimation(kObject20002, 1);
		objectPresentationUnpauseAnimation(kObject20006, 0);
		objectPresentationUnpauseAnimation(kObject20007, 0);
		objectPresentationUnpauseAnimation(kObject20007, 1);
		objectPresentationUnpauseAnimation(kObject20001, 2);
		objectPresentationUnpauseAnimation(kObject20002, 2);
		objectPresentationUnpauseAnimation(kObject20007, 2);
		objectPresentationUnpauseAnimation(kObject10202, 1);
		onCall(888);
		break;

	case 100091:
		setSpace(kZone10);
		rotationSetAlp(10911, 110.0f);
		rotationSetBet(10911, 0.0f);
		rotationSetRan(10911, 87.0f);
		rotationSetActive(10911);
		objectPresentationUnpauseAnimation(kObject10901, 0);
		objectPresentationUnpauseAnimation(kObject10901, 1);
		objectPresentationUnpauseAnimation(kObject10901, 4);
		onCall(888);
		break;

	case 100092:
		setSpace(kZone10);
		rotationSetAlp(10911, 320.0f);
		rotationSetBet(10911, 0.0f);
		rotationSetRan(10911, 87.0f);
		rotationSetActive(10911);
		objectPresentationUnpauseAnimation(kObject10901, 0);
		objectPresentationUnpauseAnimation(kObject10901, 1);
		objectPresentationUnpauseAnimation(kObject10901, 4);
		onCall(888);
		break;

	case 100093:
		setSpace(kZone10);
		rotationSetAlp(10912, 270.0f);
		rotationSetBet(10912, 0.0f);
		rotationSetRan(10912, 87.0f);
		rotationSetActive(10912);
		objectPresentationUnpauseAnimation(kObject10901, 0);
		objectPresentationUnpauseAnimation(kObject10901, 1);
		objectPresentationUnpauseAnimation(kObject10901, 4);
		onCall(888);
		break;

	case 102001:
		setSpace(kZone1);
		rotationSetAlp(10018, 230.0f);
		rotationSetBet(10018, 0.0f);
		rotationSetRan(10018, 87.0f);
		rotationSetActive(10018);
		_zone1->onVisual(9);
		soundPlay(900001 + rnd(5));
		onCall(888);
		onCall(20);
		break;

	case 102002:
		setSpace(kZone1);
		rotationSetAlp(100110, 90.0f);
		rotationSetBet(100110, 0.0f);
		rotationSetRan(100110, 87.0f);
		rotationSetActive(100110);
		_zone1->onVisual(9);
		onCall(888);
		onCall(20);
		break;

	case 102003:
		setSpace(kZone1);
		rotationSetAlp(100114, 270.0f);
		rotationSetBet(100114, 0.0f);
		rotationSetRan(100114, 87.0f);
		rotationSetActive(100114);
		_zone1->onVisual(9);
		onCall(888);
		onCall(20);
		break;

	case 109001:
		setSpace(kZone1);
		rotationSetAlp(10043, 330.0f);
		rotationSetBet(10043, 0.0f);
		rotationSetRan(10043, 87.0f);
		rotationSetActive(10043);
		_zone1->onVisual(9);
		onCall(888);
		onCall(20);
		break;

	case 109002:
		setSpace(kZone1);
		rotationSetAlp(10042, 180.0f);
		rotationSetBet(10042, 0.0f);
		rotationSetRan(10042, 87.0f);
		rotationSetActive(10042);
		_zone1->onVisual(9);
		onCall(888);
		onCall(20);
		break;

	case 109003:
		setSpace(kZone1);
		rotationSetAlp(10044, 90.0f);
		rotationSetBet(10044, 0.0f);
		rotationSetRan(10044, 87.0f);
		rotationSetActive(10044);
		_zone1->onVisual(9);
		onCall(888);
		onCall(20);
		break;

	case 109005:
		setSpace(kZone1);
		rotationSetAlp(10045, 90.0f);
		rotationSetBet(10045, 0.0f);
		rotationSetRan(10045, 87.0f);
		rotationSetActive(10045);
		_zone1->onVisual(9);
		onCall(888);
		onCall(20);

		if (varGetByte(90309) == 2) {
			varSetByte(90309, 3);
			setSpace(kZone10);
			soundPlay(3053);

			uint32 startTicks = g_system->getMillis();
			while ((g_system->getMillis() - startTicks) < 2000)
				handleEvents();

			exitToMenu(kMenuAction7);
		}
		break;

	case 1030012:
		setSpace(kZone1);
		rotationSetAlp(100412, 120.0f);
		rotationSetBet(100412, 0.0f);
		rotationSetRan(100412, 87.0f);
		rotationSetActive(100412);
		_zone1->onVisual(9);
		onCall(888);
		onCall(20);

		if (varGetByte(90309) == 2) {
			varSetByte(90309, 3);
			setSpace(kZone10);
			soundPlay(3053);

			uint32 startTicks = g_system->getMillis();
			while ((g_system->getMillis() - startTicks) < 2000)
				handleEvents();

			exitToMenu(kMenuAction7);
		}
		break;

	case 10100999:
		setSpace(kZone1);
		rotationSetAlp(1001103, 300.0f);
		rotationSetBet(1001103, 0.0f);
		rotationSetRan(1001103, 87.0f);
		rotationSetActive(1001103);
		_zone1->onVisual(9);
		onCall(888);
		onCall(20);
		break;

	case 102002999:
		setSpace(kZone1);
		timerStopAll();
		rotationSetAlp(10011002, 90.0f);
		rotationSetBet(10011002, 0.0f);
		rotationSetRan(10011002, 87.0f);
		rotationSetActive(10011002);
		_zone1->onVisual(9);
		onCall(888);
		break;
	}
}

#pragma endregion

} // End of namespace Ring
