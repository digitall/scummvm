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

#ifndef M4_BURGER_ROOMS_SECTION6_ROOM609_H
#define M4_BURGER_ROOMS_SECTION6_ROOM609_H

#include "m4/burger/rooms/section6/section6_room.h"

namespace M4 {
namespace Burger {
namespace Rooms {

class Room609 : public Section6Room {
private:
	static const seriesStreamBreak SERIES1[];
	static int32 _state1;
public:
	Room609();
	~Room609() override {}

	void preload() override;
	void init() override;
	void daemon() override;
};

} // namespace Rooms
} // namespace Burger
} // namespace M4

#endif
