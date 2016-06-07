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

#ifndef RING_AQUATOR_IMAGE_HEADER_H
#define RING_AQUATOR_IMAGE_HEADER_H

#include "ring/shared.h"

namespace Ring {

class ImageHeader;

class AquatorImageHeader {
public:
	AquatorImageHeader();
	~AquatorImageHeader();

	void setField0(bool state) { _field_0 = state; }
	void setField4(uint32 val) { _field_4 = val; }
	void setField8(uint32 val) { _field_8 = val; }
	void setChannel(uint32 val);

	bool getField0() { return _field_0; }
	uint32 getField4() { return _field_4; }
	uint32 getField8() { return _field_8; }
	ImageHeader *getHeader() { return _header; }
	uint32 getChannel() { return _channel; }

private:
	bool _field_0;
	uint32 _field_4;
	uint32 _field_8;
	ImageHeader *_header;
	uint32 _channel;
};

} // End of namespace Ring

#endif // RING_AQUATOR_IMAGE_HEADER_H
