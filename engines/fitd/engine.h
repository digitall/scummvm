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

#ifndef FITD_ENGINE_H
#define FITD_ENGINE_H

#include "common/array.h"
#include "common/hashmap.h"
#include "common/scummsys.h"
#include "fitd/room.h"

namespace Common {
template<>
struct Hash<void *> {
	uint operator()(void *s) const {
		uint64 u = (uint64)s;
		return ((u >> 32) & 0xFFFFFFFF) ^ (u & 0xFFFFFFFF);
	}
};
} // namespace Common

namespace Fitd {
class Engine {
public:
	Common::HashMap<void *, byte *> bodyBufferMap;
	Common::Array<CameraData> currentFloorCameraData;
	Common::Array<Body *> bodies;
	Common::Array<Animation *> animations;
	Common::Array<RoomData> roomDataTable;
	Common::Array<WorldObject> worldObjets;
};
} // namespace Fitd

#endif // FITD_ENGINE_H
