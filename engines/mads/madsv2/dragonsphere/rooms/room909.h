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

#ifndef MADS_DRAGONSPHERE_ROOMS_909_H
#define MADS_DRAGONSPHERE_ROOMS_909_H

#include "common/serializer.h"
#include "mads/madsv2/core/general.h"

namespace MADS {
namespace MADSV2 {
namespace Dragonsphere {
namespace Rooms {

#define local (&scratch)
#define ss    local->sprite
#define seq   local->sequence
#define aa    local->animation

/**
 * Room local variables (field names encode game.scratch byte offsets).
 *
 * Layout verified against disassembly of room_909_init / room_909_anim:
 *   x00 = game.scratch+0x00  series handle: kernel_name('x', 0)
 *   x02 = game.scratch+0x02  series handle: kernel_name('x', 1)
 *   x04 = game.scratch+0x04  series handle: kernel_name('x', 2)
 *   x1e = game.scratch+0x1E  seq handle: forward sequence for series x0
 *   x20 = game.scratch+0x20  seq handle: forward sequence for series x1
 *   x22 = game.scratch+0x22  seq handle: forward sequence for series x2
 *   x3c = game.scratch+0x3C  animation handle: kernel_name('w', 1)
 *   x44 = game.scratch+0x44  last observed animation frame (change detector)
 */
struct Scratch {
	int16 x00;         // sprite series: kernel_name('x', 0)
	int16 x02;         // sprite series: kernel_name('x', 1)
	int16 x04;         // sprite series: kernel_name('x', 2)
	int16 _pad06[12];  // offsets 0x06..0x1C (unused in this room)
	int16 x1e;         // seq: forward play of series x0, depth 12, range [-1..-2]
	int16 x20;         // seq: forward play of series x1, depth 12, range [-1..-2]
	int16 x22;         // seq: forward play of series x2, depth 12, range [-1..-2]
	int16 _pad24[12];  // offsets 0x24..0x3B (unused in this room)
	int16 x3c;         // animation handle: kernel_name('w', 1)
	int16 _pad3e[3];   // offsets 0x3E..0x43 (unused in this room)
	int16 x44;         // last observed animation frame (for frame-change detection)
};


/* ========================= Triggers ======================== */

#define TRIGGER_SHOW_SCORE  61  // fired by kernel_timing_trigger after frame 105; daemon shows score screen


/* ======================== Anim frames ====================== */

#define FRAME_HEARTBEAT_SOUND    52   // 0x34: play speech line 1 (or sound fallback)
#define FRAME_MUSIC_SOUND        82   // 0x52: play music sound (60) if music enabled
#define FRAME_TIMING_TRIGGER    105   // 0x69: arm 2-second delay for TRIGGER_SHOW_SCORE
#define FRAME_LOOP_RESET        106   // 0x6A: reset animation to FRAME_TIMING_TRIGGER


extern void room_909_preload();
extern void room_909_init();
extern void room_909_daemon();
extern void room_909_pre_parser();
extern void room_909_parser();
extern void room_909_synchronize(Common::Serializer &s);

} // namespace Rooms
} // namespace Dragonsphere
} // namespace MADSV2
} // namespace MADS

#endif
