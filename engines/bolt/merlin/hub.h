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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef BOLT_MERLIN_HUB_H
#define BOLT_MERLIN_HUB_H

#include "bolt/menu_card.h"

namespace Bolt {

class MerlinEngine;
struct HubEntry;
	
class HubCard : public MenuCard {
public:
	void init(Graphics *graphics, BltFile &boltlib, BltId resId);
	void enter();
protected:
	Signal processButtonClick(int num);

private:
	Graphics *_graphics;
	ScopedArray<BltImage> _itemImages;
};

} // End of namespace Bolt

#endif
