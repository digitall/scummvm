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

#ifndef RING_AQUATOR_STREAM_H
#define RING_AQUATOR_STREAM_H

#include "ring/shared.h"

#include "ring/graphics/aquator/image_header_entry.h"

namespace Ring {

class AquatorImageHeader;

class AquatorStream {
public:
	AquatorStream(uint32 count, Common::String path);
	~AquatorStream();

	void alloc(bool isCompressed, const Graphics::PixelFormat &format, uint32 size);
	void dealloc();

	uint32 sub_410F50(uint32 index);
	void updateEntries(float timeOffset);
	void sub_411530(uint32 index, uint32 a2);
	void setChannel(uint32 index, uint32 channel);

	ImageHeaderEntry *getEntry() { return _entry; }
	uint32 getCount() { return _headers.size(); }
	uint32 getChannel(uint32 index);

	bool isInitialized() { return _entry->isInitialized(); }

private:
	Common::Array<AquatorImageHeader *> _headers;
	Common::String _path;
	ImageHeaderEntry *_entry;

	void initNode(Common::SeekableReadStream *stream, const Graphics::PixelFormat &format);
	void initChannel(Common::SeekableReadStream *stream, uint32 index);

	// Decompressed aquator
	void initStream(uint32 index);
};

} // End of namespace Ring

#endif // RING_AQUATOR_STREAM_H
