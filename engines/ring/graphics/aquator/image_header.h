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

#ifndef RING_IMAGE_HEADER_H
#define RING_IMAGE_HEADER_H

#include "ring/shared.h"

namespace Ring {

class ImageHeaderEntry;

class ImageHeader {
public:
	ImageHeader();
	~ImageHeader();

	void reset();

	void init(Common::SeekableReadStream *stream);
	void update(ImageHeaderEntry* entry);

	bool hasEntries() { return _entries.size() != 0; }
	uint32 getField4() { return _field_4; }
	ImageHeaderEntry *get(uint32 index);
	ImageHeaderEntry *getCurrent() { return _current; }

private:
	uint32 _field_4;
	Common::Array<ImageHeaderEntry *> _entries;
	ImageHeaderEntry *_current;
	int32 _field_4C;
};

} // End of namespace Ring

#endif // RING_IMAGE_HEADER_H
