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

#ifndef FITD_DETECTION_H
#define FITD_DETECTION_H

#include "engines/advancedDetector.h"

namespace Fitd {

enum FitdDebugChannels {
	kDebugConsole	= 1,
};

enum FitdGameId {
	GID_NONE		= 0,
	GID_AITD1		= 1,
	GID_JACK		= 2,
	GID_AITD2		= 3,
	GID_AITD3		= 4,
	GID_TIMEGATE	= 5,
};

struct FitdGameDescription {
	AD_GAME_DESCRIPTION_HELPERS(desc);

	ADGameDescription desc;
	FitdGameId gameId;
};

extern const PlainGameDescriptor fitdGames[];

} // End of namespace Fitd

#endif // FITD_DETECTION_H
