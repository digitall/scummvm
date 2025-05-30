/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NANCY_DETECTION_H
#define NANCY_DETECTION_H

#include "engines/advancedDetector.h"

namespace Nancy {

enum GameType {
	kGameTypeNone 	 = 0,
	kGameTypeVampire = 1,
	kGameTypeNancy1  = 2,
	kGameTypeNancy2  = 3,
	kGameTypeNancy3  = 4,
	kGameTypeNancy4  = 5,
	kGameTypeNancy5  = 6,
	kGameTypeNancy6  = 7,
	kGameTypeNancy7  = 8,
	kGameTypeNancy8  = 9,
	kGameTypeNancy9  = 10,
	kGameTypeNancy10 = 11,
	kGameTypeNancy11 = 12,
};

enum NancyGameFlags {
	GF_COMPRESSED 		= 1 << 0
};

struct NancyGameDescription {
	AD_GAME_DESCRIPTION_HELPERS(desc);

	ADGameDescription desc;
	GameType gameType;
};

enum NancyDebugChannels {
	kDebugEngine = 1,
	kDebugActionRecord,
	kDebugScene,
	kDebugSound,
	kDebugVideo,
	kDebugHypertext,
};

// Settings found in the original engine
#define GAMEOPTION_PLAYER_SPEECH		GUIO_GAMEOPTIONS1
#define GAMEOPTION_CHARACTER_SPEECH		GUIO_GAMEOPTIONS2
#define GAMEOPTION_AUTO_MOVE			GUIO_GAMEOPTIONS3

// Patch settings, general
#define GAMEOPTION_FIX_SOFTLOCKS		GUIO_GAMEOPTIONS4
#define GAMEOPTION_FIX_ANNOYANCES		GUIO_GAMEOPTIONS5

// Patch settings, specific to each game
#define GAMEOPTION_NANCY2_TIMER			GUIO_GAMEOPTIONS6

#define GAMEOPTION_ORIGINAL_SAVELOAD	GUIO_GAMEOPTIONS7

} // End of namespace Nancy

#endif // NANCY_DETECTION_H
