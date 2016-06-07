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

#include "ring/graphics/aquator/image_header_entry.h"

#include "ring/base/stream.h"

#include "ring/graphics/pixel.h"

#include "ring/ring.h"

#include "graphics/surface.h"

namespace Ring {

#define IMAGEHEADER_BUFFER_SIZE 129600

ImageHeaderEntry::Header::Header() {
	field_0  = 0;
	field_4  = 0;
	field_8  = 0;
	field_C  = 0;
	field_10 = 0;
	field_14 = 0;
	field_18 = 0;
	field_1C = 0;
	field_20 = 0;
	field_24 = 0;
	field_28 = 0;
	field_2C = 0;
	field_30 = 0;
}

void ImageHeaderEntry::Header::update(const Header &header) {
	field_0  = header.field_0;
	field_4  = header.field_4;
	field_8  = header.field_8;
	field_C  = header.field_C;
	field_10 = header.field_10;
	field_14 = header.field_14;
	field_18 = header.field_18;
	field_1C = header.field_1C;
	field_20 = header.field_20;
	field_24 = header.field_24;
	field_28 = header.field_28;
	field_2C = header.field_2C;
	field_30 = header.field_30;
}

void ImageHeaderEntry::Header::load(Common::SeekableReadStream *stream) {
	field_0  = stream->readUint32LE();
	field_4  = stream->readUint32LE();
	field_8  = stream->readUint32LE();
	field_C  = stream->readUint32LE();
	field_10 = convertIEEEFloat(stream->readUint32LE());
	field_14 = convertIEEEFloat(stream->readUint32LE());
	field_18 = convertIEEEFloat(stream->readUint32LE());
	field_1C = stream->readUint32LE();
	field_20 = stream->readUint32LE();
	field_24 = stream->readUint32LE();
	field_28 = stream->readUint32LE();
	field_2C = stream->readUint32LE();
	field_30 = stream->readUint32LE();
}

void ImageHeaderEntry::InternalData::init(float a1, float a2, float a3, float a4, float a5, float a6) {
	field_0 = a3;
	field_4 = a4;
	field_8 = a5;
	field_C = a6;
	field_10 = a1 - 1;
	field_14 = a2 - 1;
	field_18 = 0;
	field_1C = 0;
}

ImageHeaderEntry::ImageHeaderEntry() {
	_buffer      = nullptr;
	_bufferData  = nullptr;
	_hasAdditionnalData = false;
}

ImageHeaderEntry::~ImageHeaderEntry() {
	free(_buffer);
	_buffer = nullptr;

	_bufferData = nullptr;
}

void ImageHeaderEntry::reset() {
	free(_buffer);
	_buffer = nullptr;

	_bufferData = nullptr;
}

void ImageHeaderEntry::init(Common::SeekableReadStream *stream, bool hasAdditionnalData) {
	reset();

	// Read entry header
	_header.load(stream);
	initData();

	// Allocate buffer and setup pointers
	_buffer = allocBuffer(hasAdditionnalData);
	if (hasAdditionnalData)
		_bufferData = (byte *)_buffer + IMAGEHEADER_BUFFER_SIZE;

	// Read buffers
	stream->read(_bufferData, _header.field_2C / 4);

	if (hasAdditionnalData)
		stream->read(_buffer, IMAGEHEADER_BUFFER_SIZE);

	_hasAdditionnalData = hasAdditionnalData;
}

void ImageHeaderEntry::init(ImageHeaderEntry *entry) {
	reset();

	// Copy header information
	_header.update(entry->getHeader());
	initData();

	// ALlocate new buffer
	_buffer = allocBuffer(true);
	_bufferData = (byte *)_buffer + IMAGEHEADER_BUFFER_SIZE;
	_hasAdditionnalData = true;
}

void ImageHeaderEntry::initData() {
	if (!_header.field_20)
		_header.field_20 = _header.field_0;

	if (!_header.field_28)
		_header.field_28 = _header.field_4;

	_header.field_30 = _header.field_8  * (_header.field_20 - _header.field_1C);
	_header.field_2C = (uint32)(_header.field_30 * (_header.field_28 - _header.field_24));
}

void ImageHeaderEntry::update(ImageHeaderEntry *entry, bool updateCaller) {
	error("[ImageHeaderEntry::update] Not implemented");
}

void ImageHeaderEntry::process() {
	error("[ImageHeaderEntry::update] Not implemented");
}

void ImageHeaderEntry::prepareBuffer() {
	int32 *buffer = (int32 *)_bufferData;
	if (!buffer)
		error("[ImageHeaderEntry::prepareBuffer] Buffer not initialized properly");

	int count = buffer[8203];
	if (count < 0)
		return;

	// Prepare buffer chunks
	int32 *bufferPtr = buffer + 4098;
	do {
		for (int i = 0; i < buffer[8202]; i++)
			Pixel::set(&bufferPtr[i], bufferPtr[i], buffer[8231], buffer[8232]);

		bufferPtr += 64;
		--count;
	} while (count > 0);
}

void ImageHeaderEntry::drawBuffer(Graphics::Surface *surface) {
	int32 *buffer = (int32 *)_bufferData;
	if (!buffer)
		error("[ImageHeaderEntry::prepareBuffer] Buffer not initialized properly");

	int count = buffer[8203];
	if (count < 0)
		return;

	// Setup buffers
	byte  *surfacePtr = (byte *)surface->getPixels();
	int32 *bufferPtr  = buffer;

	// Draw buffer to surface
	do {

		for (int i = 0; i < buffer[8202]; i++) {
			copyToSurface((int16 *)(buffer + 1),
			              16,
			              16,
			              bufferPtr[i + 2],
			              bufferPtr[i + 4098],
			              bufferPtr[i + 3],
			              bufferPtr[i + 4099],
			              bufferPtr[i + 66],
			              bufferPtr[i + 4162],
			              bufferPtr[i + 67],
			              bufferPtr[i + 41636],
			              surfacePtr,
			              surface->pitch - 32);

			surfacePtr += 32;
		}

		surfacePtr += surface->pitch * 16; // TODO handle any type of bpp
		bufferPtr  += 64;
		--count;
	} while (count > 0);
	error("[ImageHeaderEntry::drawBuffer] Not implemented");
}

void ImageHeaderEntry::copyToSurface(int16 *data, int width, int height, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, byte *surface, int pitch) {
	error("[ImageHeaderEntry::copyToSurface] Not implemented");
}

void ImageHeaderEntry::computeCoordinates(Common::Point *point) {
	int32 *buffer = (int32 *)_bufferData;
	if (!buffer)
		error("[ImageHeaderEntry::updateBuffer] Buffer not initialized properly");

	// Compute coordinates values
	Common::Point pointDiv = *point;
	pointDiv.x /= 16;
	pointDiv.y /= 16;

	Common::Point pointMod = *point;
	pointMod.x %= 16;
	pointMod.y %= 16;

	// Adjust coordinates
	if (point->x < 0)
		point->x = 0;

	if (point->x >= (int16)buffer[8200])
		point->x = buffer[8200] - 1;

	if (point->y < 0)
		point->y = 0;

	if (point->y >= (int16)buffer[8201])
		point->y = buffer[8201] - 1;

	int offset = ((uint16)pointDiv.y << 6) + pointDiv.x;
	float dist = 2048.0f * 0.5f;

	float a1 = (float)buffer[offset + 2]  * 0.000015258789f;
	float a2 = (float)buffer[offset + 3]  * 0.000015258789f;
	float a3 = (float)buffer[offset + 66] * 0.000015258789f;
	float a4 = (float)buffer[offset + 67] * 0.000015258789f;

	error("[ImageHeaderEntry::updateBuffer] Missing code");

	if (true /* testing an unknown var related to x / y */) {
		if (a3 - a1 < dist) {
			if (a4 - a2 >= dist) {
				a2 += 2048.0f;
			} else if (a3 - a4 >= dist) {
				a4 += 2048.0f;
			}
		} else {
			a1 += 2048.0f;
			a2 += 2048.0f;
			a4 += 2048.0f;
		}
	} else {
		if (a3 - a4 < dist) {
			a2 += 2048.0f;

			if (true /* testing an unknown var related to x / y */) {
				a4 += 2048.0f;
				a3 += 2048.0f;
			}
		} else {
			a4 += 2048.0f;
			a2 += 2048.0f;
		}
	}

	float a5 = (float)buffer[offset + 4098] * 0.000015258789f;
	float a6 = (float)buffer[offset + 4099] * 0.000015258789f;
	float a7 = (float)buffer[offset + 4162]  * 0.000015258789f;
	float a8 = (float)buffer[offset + 4163]  * 0.000015258789f;

	float a9  = (float)(16 - pointMod.x);
	float a10 = (float)(16 - pointMod.y);

	// Update coordinates
	point->x = ((a9 * a3 + pointMod.x * a4) * pointMod.y + (a9 * a1 + pointMod.x * a2) * a10       ) * 0.0625f * 0.0625f;
	point->y = ((a9 * a5 + pointMod.x * a6) * a10        + (a9 * a7 + pointMod.x * a8) * pointMod.y) * 0.0625f * 0.0625f;

	point->x %= 2048;
	point->y %= buffer[8229];
}

void ImageHeaderEntry::adjustCoordinates(Common::Point *point) {
	uint32 *buffer = (uint32 *)_bufferData;
	if (!buffer)
		error("[ImageHeaderEntry::updateCoordinates] Buffer not initialized properly");

	point->x = (int16)((point->x * (float)buffer[8227] * 10.0f) / 2048.0f);
	point->y = (int16)((point->y - (float)buffer[8230] * 0.5f) * (float)buffer[8225] / (float)buffer[8230] + (float)buffer[8226] + 10.0f);
}

void *ImageHeaderEntry::allocBuffer(bool hasAdditionnalData) const {
	uint32 size = _header.field_2C / 4 + (hasAdditionnalData ? IMAGEHEADER_BUFFER_SIZE : 0);

	void *buffer = malloc(size + 1024);
	if (!buffer)
		error("[ImageHeaderEntry::allocBuffer] Cannot allocate buffer of size %d", size + 1024);

	return buffer;
}

void ImageHeaderEntry::updateData(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8) {
	if (!_bufferData)
		error("[ImageHeaderEntry::updateData] BufferData not initialized properly");

	int *data = (int *)_bufferData;

	data[8204] = a1;
	data[8205] = a2;
	data[8206] = data[8195];
	data[8207] = a3;
	data[8208] = a4;
	data[8209] = data[8195];
	data[8210] = a5;
	data[8211] = a6;
	data[8212] = data[8195];
	data[8213] = a7;
	data[8214] = a8;
	data[8215] = data[8195];
}

} // End of namespace Ring
