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
 *
 */

#include "bolt/image.h"

#include "bolt/graphics.h"

namespace Bolt {

BltImagePtr BltImage::load(BltFile *bltFile, BltLongId id) {
	BltImagePtr self(new BltImage());
	self->init(bltFile, id);
	return self;
}

BltImage::BltImage()
{ }

struct BltImageHeader {

	static const int SIZE = 0x18;
	BltImageHeader(const byte *src) {
		compression = src[0];
		// FIXME: unknown fields
		offsetX = (int16)READ_BE_UINT16(&src[6]);
		offsetY = (int16)READ_BE_UINT16(&src[8]);
		width = READ_BE_UINT16(&src[0xA]);
		height = READ_BE_UINT16(&src[0xC]);
	}

	byte compression;
	int16 offsetX;
	int16 offsetY;
	uint16 width;
	uint16 height;
};

void BltImage::init(BltFile *bltFile, BltLongId id) {
	_res = bltFile->loadLongId(id);

	BltImageHeader header(&_res->getData()[0]);
	_compression = header.compression;
	_width = header.width;
	_height = header.height;
	_offset.x = header.offsetX;
	_offset.y = header.offsetY;
}

void BltImage::drawToBack(Graphics *graphics, int x, int y, bool transparency) const {
	int topLeftX = x + _offset.x;
	int topLeftY = y + _offset.y;

	const Common::Array<byte> &data = _res->getData();

	if (_compression) {
		graphics->decodeRL7ToBack(topLeftX, topLeftY, _width, _height,
			&data[BltImageHeader::SIZE], data.size() - BltImageHeader::SIZE,
			transparency);
	}
	else {
		graphics->decodeCLUT7ToBack(topLeftX, topLeftY, _width, _height,
			&data[BltImageHeader::SIZE], data.size() - BltImageHeader::SIZE,
			transparency);
	}
}

const byte* BltImage::getImageData() const {
	return &_res->getData()[BltImageHeader::SIZE];
}

byte BltImage::query(const Common::Point &pt) const {
	if (pt.x < 0 || pt.y < 0 || pt.x >= _width || pt.y >= _height) {
		// Point outside image
		return 0;
	}

	const byte *data = getImageData();
	if (_compression) {
		// RL7
		byte result = 0;

		int srcY = 0;
		int in_cursor = 0;
		// Skip to line
		while (srcY < pt.y && srcY < _height) {
			byte in_byte = data[in_cursor];
			++in_cursor;
			if ((in_byte & 0x80) != 0) {
				byte lengthByte = data[in_cursor];
				++in_cursor;
				if (lengthByte == 0) {
					// length 0 means end-of-line
					++srcY;
				}
			}
		}

		int srcX = 0;
		while (srcX < _width) {
			byte in_byte = data[in_cursor];
			++in_cursor;
			result = in_byte & 0x7F;
			if ((in_byte & 0x80) != 0) {
				byte lengthByte = data[in_cursor];
				++in_cursor;
				if (lengthByte == 0) {
					break;
				}
				else {
					srcX += lengthByte;
					if (srcX > pt.x) {
						break;
					}
				}
			}
			else {
				if (srcX >= pt.x) {
					break;
				}
				++srcX;
			}
		}

		return result;
	}
	else {
		// CLUT7
		// TODO: test? may be unused.
		return data[pt.y * _width + pt.x];
	}
}

} // End of namespace Bolt
