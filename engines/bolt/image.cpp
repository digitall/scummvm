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

#include "bolt/image.h"

#include "bolt/graphics.h"

namespace Bolt {

struct BltImageHeader {

	static const int SIZE = 0x18;
	BltImageHeader(const byte *src) {
		compression = src[0];
		// FIXME: unknown fields
		offset.x = READ_BE_INT16(&src[6]);
		offset.y = READ_BE_INT16(&src[8]);
		width = READ_BE_UINT16(&src[0xA]);
		height = READ_BE_UINT16(&src[0xC]);
	}

	byte compression;
	Common::Point offset;
	uint16 width;
	uint16 height;
};

void BltImage::load(BltFile &bltFile, BltLongId id) {
	_res.reset(bltFile.loadResource(id, kBltImage));
}

void BltImage::draw(::Graphics::Surface &surface, bool transparency) const {
	assert(_res);

	BltImageHeader header(&_res[0]);
	const byte *src = &_res[BltImageHeader::SIZE];
	int srcLen = _res.size() - BltImageHeader::SIZE;

	if (header.compression) {
		decodeRL7(surface, 0, 0, header.width, header.height, src, srcLen, transparency);
	}
	else {
		decodeCLUT7(surface, 0, 0, header.width, header.height, src, srcLen, transparency);
	}
}

void BltImage::drawAt(::Graphics::Surface &surface, int x, int y,
	bool transparency) const {
	assert(_res);

	BltImageHeader header(&_res[0]);
	int topLeftX = x + header.offset.x;
	int topLeftY = y + header.offset.y;

	const byte *src = &_res[BltImageHeader::SIZE];
	int srcLen = _res.size() - BltImageHeader::SIZE;

	if (header.compression) {
		decodeRL7(surface, topLeftX, topLeftY, header.width, header.height, src, srcLen,
			transparency);
	}
	else {
		decodeCLUT7(surface, topLeftX, topLeftY, header.width, header.height, src, srcLen,
			transparency);
	}
}

byte BltImage::query(int x, int y) const {
	BltImageHeader header(&_res[0]);
	const byte *src = &_res[BltImageHeader::SIZE];
	int srcLen = _res.size() - BltImageHeader::SIZE;
	return header.compression ?
		queryRL7(x, y, src, srcLen, header.width, header.height) :
		queryCLUT7(x, y, src, srcLen, header.width, header.height);
}

uint16 BltImage::getWidth() const {
	BltImageHeader header(&_res[0]);
	return header.width;
}

uint16 BltImage::getHeight() const {
	BltImageHeader header(&_res[0]);
	return header.height;
}

Common::Point BltImage::getOffset() const {
	BltImageHeader header(&_res[0]);
	return header.offset;
}

} // End of namespace Bolt
