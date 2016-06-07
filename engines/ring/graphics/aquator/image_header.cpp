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

#include "ring/graphics/aquator/image_header.h"

#include "ring/graphics/aquator/image_header_entry.h"

#include "ring/helpers.h"

namespace Ring {

ImageHeader::ImageHeader() {
	_field_4 = 0;
	_current = new ImageHeaderEntry();
	_field_4C = -1;
}

ImageHeader::~ImageHeader() {
	reset();
	delete _current;
}

void ImageHeader::reset() {
	CLEAR_ARRAY(ImageHeaderEntry, _entries);
	_current->reset();
}

void ImageHeader::init(Common::SeekableReadStream *stream) {
	reset();

	uint32 count = stream->readUint32LE();
	_field_4  = stream->readUint32LE();
	_field_4C = stream->readUint32LE();

	// Create entries
	for (uint32 i = 0; i < count; i++) {
		ImageHeaderEntry *entry = new ImageHeaderEntry();
		entry->init(stream, false);
		_entries.push_back(entry);
	}

	if (count == 1)
		_field_4C = -1;
}

void ImageHeader::update(ImageHeaderEntry* entry) {
	if (!_current)
		error("[ImageHeader::update] entry not initialized properly");

	_current->init(_entries[0]);
	entry->update(_current);
}

ImageHeaderEntry *ImageHeader::get(uint32 index) {
	if (index >= _entries.size())
		error("[ImageHeader::get] Invalid index (was:%d, max:%d)", index, _entries.size() - 1);

	return _entries[index];
}

} // End of namespace Ring
