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

#ifndef NUVIE_ACTORS_MD_ACTOR_H
#define NUVIE_ACTORS_MD_ACTOR_H

#include "ultima/nuvie/actors/wou_actor.h"

namespace Ultima {
namespace Nuvie {

class MDActor: public WOUActor {
public:

	MDActor(Map *m, ObjManager *om, GameClock *c);
	~MDActor() override;

	bool init(uint8 unused = 0) override;
	bool will_not_talk() const override;
	uint8 get_maxhp() const override {
		return (((level * 24 + strength * 2) < 255) ? (level * 24 + strength * 2) : 255);
	}
	uint8 get_hp_text_color() const override;
	uint8 get_str_text_color() const override;
	uint8 get_dex_text_color() const override;
	bool is_immobile() const override;

	bool check_move(uint16 new_x, uint16 new_y, uint8 new_z, ActorMoveFlags flags = 0) override;
	uint16 get_downward_facing_tile_num() const override;
	void set_direction(NuvieDir d) override;
	bool is_passable() const override;

};

} // End of namespace Nuvie
} // End of namespace Ultima

#endif
