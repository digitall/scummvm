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
 *
 * This file is dual-licensed.
 * In addition to the GPLv3 license mentioned above, this code is also
 * licensed under LGPL 2.1. See LICENSES/COPYING.LGPL file for the
 * full text of the license.
 *
 */

#include "common/stream.h"
#include "common/substream.h"

#include "gob/rxyfile.h"

namespace Gob {

RXYFile::RXYFile(Common::SeekableReadStream &rxy) : _width(0), _height(0) {
	Common::SeekableReadStreamEndianWrapper sub(&rxy, false, DisposeAfterUse::NO);

	load(sub);
}

RXYFile::RXYFile(Common::SeekableReadStreamEndian &rxy) : _width(0), _height(0) {
	load(rxy);
}

RXYFile::RXYFile(uint16 width, uint16 height) : _realCount(1), _width(width), _height(height) {
	_coords.resize(1);

	_coords[0].left   = 0;
	_coords[0].top    = 0;
	_coords[0].right  = _width  - 1;
	_coords[0].bottom = _height - 1;
}

RXYFile::~RXYFile() {
}

uint RXYFile::size() const {
	return _coords.size();
}

uint16 RXYFile::getWidth() const {
	return _width;
}

uint16 RXYFile::getHeight() const {
	return _height;
}

uint16 RXYFile::getRealCount() const {
	return _realCount;
}

const RXYFile::Coordinates &RXYFile::operator[](uint i) const {
	assert(i < _coords.size());

	return _coords[i];
}

void RXYFile::load(Common::SeekableReadStreamEndian &rxy) {
	if (rxy.size() < 2)
		return;

	rxy.seek(0);

	_realCount = rxy.readUint16();

	uint16 count = (rxy.size() - 2) / 8;

	_coords.resize(count);
	for (auto &coord : _coords) {
		coord.left   = rxy.readUint16();
		coord.right  = rxy.readUint16();
		coord.top    = rxy.readUint16();
		coord.bottom = rxy.readUint16();

		if (coord.left != 0xFFFF) {
			_width  = MAX<uint16>(_width , coord.right  + 1);
			_height = MAX<uint16>(_height, coord.bottom + 1);
		}
	}
}

uint16 RXYFile::add(uint16 left, uint16 top, uint16 right, uint16 bottom) {
	_coords.resize(_coords.size() + 1);

	_coords.back().left   = left;
	_coords.back().top    = top;
	_coords.back().right  = right;
	_coords.back().bottom = bottom;

	return _coords.size() - 1;
}

} // End of namespace Gob
