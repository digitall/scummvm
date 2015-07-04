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

#ifndef RING_VISUAL_H
#define RING_VISUAL_H

#include "ring/shared.h"

#include "common/keyboard.h"
#include "common/rect.h"
#include "common/serializer.h"

namespace Ring {

class Visual : public BaseObject, public Common::Serializable {
public:
	Visual(Id id) : BaseObject(id), _type(0), _visible(false) {}
	virtual ~Visual() {}

	virtual void draw() = 0;
	virtual uint32 handleLeftButtonUp(const Common::Point &point) = 0;
	virtual uint32 handleUpdate(const Common::Point &point) = 0;
	virtual uint32 handleLeftButtonDown(Common::Point point) { return 0; }
	virtual bool handleKey(Common::KeyCode key) { return false; }
	virtual void alloc() = 0;
	virtual void dealloc() = 0;

	// Serializable
	void saveLoadWithSerializer(Common::Serializer &s) override {}

	// Accessors
	void setType(uint32 type) { _type = type; }
	void setVisible(bool state) { _visible = state; }

protected:
	uint32 _type;
	bool _visible;
};

} // End of namespace Ring

#endif // RING_VISUAL_H
