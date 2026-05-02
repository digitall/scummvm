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

#include "mads/madsv2/core/conv.h"
#include "mads/madsv2/core/game.h"
#include "mads/madsv2/core/imath.h"
#include "mads/madsv2/core/kernel.h"
#include "mads/madsv2/core/sound.h"
#include "mads/madsv2/core/text.h"
#include "mads/madsv2/dragonsphere/global.h"
#include "mads/madsv2/dragonsphere/rooms/section9.h"
#include "mads/madsv2/dragonsphere/rooms/room909.h"

namespace MADS {
namespace MADSV2 {
namespace Dragonsphere {
namespace Rooms {

//static Scratch scratch;

void room_909_init() {

}

void room_909_daemon() {

}

void room_909_pre_parser() {
}

void room_909_parser() {
}

void room_909_synchronize(Common::Serializer &s) {
	
}

void room_909_preload() {
	room_init_code_pointer = room_909_init;
	room_pre_parser_code_pointer = room_909_pre_parser;
	room_parser_code_pointer = room_909_parser;
	room_daemon_code_pointer = room_909_daemon;

	section_9_walker();
	section_9_interface();
}

} // namespace Rooms
} // namespace Dragonsphere
} // namespace MADSV2
} // namespace MADS
