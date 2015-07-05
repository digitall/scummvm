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

#include "ring/graphics/codecs/imageloader_cin.h"

#include "ring/graphics/image.h"

#include "ring/graphics/movies/cinematic1.h"

#include "ring/helpers.h"

namespace Ring {

ImageLoaderCIN::ImageLoaderCIN() {
	_cinematic = nullptr;
	memset(&_header, 0, sizeof(_header));
	_stride = 0;
	_widthAndPadding = 0;
	_width = 0;
	_height = 0;
}


ImageLoaderCIN::~ImageLoaderCIN() {
	deinit();
}

bool ImageLoaderCIN::load(ImageSurface *image, ArchiveType, ZoneId, LoadFrom, DrawType) {
	if (!image)
		error("[ImageLoaderCNM::load] Invalid image pointer!");

	byte format = 0;
	_filename = image->getName();

	if (!init(_filename)){
		warning("[ImageLoaderCIN::load] Error initializing image reader (%s)", _filename.c_str());
		goto cleanup;
	}

	// Check cinematic format
	format = _cinematic->readByte();
	if (_cinematic->eos() || _cinematic->err() || format != CINEMATIC_FORMAT) {
		warning("[ImageLoaderCIN::load] Wrong cinematic format for %s (was: %d, valid: %d)", _filename.c_str(), format, CINEMATIC_FORMAT);
		goto cleanup;
	}

	// Read image data
	if (!readImage(image)) {
		warning("[ImageLoaderCIN::load] Error reading image (%s)", _filename.c_str());
		goto cleanup;
	}

	deinit();

	return true;

cleanup:
	deinit();
	return false;
}

bool ImageLoaderCIN::init(Common::String filename, ArchiveType, ZoneId, LoadFrom) {
	_cinematic = new Cinematic1();

	if (!_cinematic->init(filename)) {
		warning("[ImageLoaderCIN::init] Error initializing cinematic (%s)", _filename.c_str());
		return false;
	}

	if (!readHeader()) {
		warning("[ImageLoaderCIN::init] Error reading header (%s)", _filename.c_str());
		return false;
	}

	return true;
}

void ImageLoaderCIN::deinit() {
	SAFE_DELETE(_cinematic);
}

bool ImageLoaderCIN::readHeader() {
	if (!_cinematic)
		error("[ImageLoaderCIN::readHeader] Cinematic not initialized properly");

	memset(&_header, 0, sizeof(_header));

	// Read header (size: 0x40)
	_cinematic->read(&_header.signature, sizeof(_header.signature));
	_header.channels    = _cinematic->readByte();
	_header.bitsPerSample    = _cinematic->readByte();
	_header.samplesPerSec    = _cinematic->readByte();
	_header.field_B    = _cinematic->readByte();
	_header.field_C    = _cinematic->readUint16LE();
	_header.chunkCount = _cinematic->readUint32LE();
	_header.frameRate   = _cinematic->readUint32LE();
	_header.field_16   = _cinematic->readByte();
	_header.width      = _cinematic->readUint32LE();
	_header.height     = _cinematic->readUint32LE();
	_header.field_1F   = _cinematic->readByte();
	_header.field_20   = _cinematic->readUint32LE();
	_header.field_24   = _cinematic->readUint32LE();
	_header.field_28   = _cinematic->readUint32LE();
	_header.field_2C   = _cinematic->readUint32LE();
	_header.field_30   = _cinematic->readUint32LE();
	_header.field_34   = _cinematic->readUint32LE();
	_header.field_38   = _cinematic->readUint32LE();
	_header.field_3C   = _cinematic->readUint32LE();

	// Update width and height
	_width = _header.width;
	_height = _header.height;

	return true;
}

bool ImageLoaderCIN::readImage(ImageSurface *image, byte bitdepth, DrawType) {
	if (!_cinematic)
		error("[ImageLoaderCIN::readImage] Cinematic not initialized properly");

	if (!image)
		error("[ImageLoaderCIN::readImage] Invalid image pointer!");

	if (!image->isInitialized())
		image->create(bitdepth == 17 ? 16 : bitdepth, 2, _width, _height);

	// Compute stride
	_widthAndPadding = _width + 3;
	_stride = _widthAndPadding * 3;

	// Create buffer
	byte *buffer = static_cast<byte *>(image->getSurface()->getPixels());
	if (bitdepth == 17) {
		// When decoding a video, data is stored past the normal surface size
		buffer = static_cast<byte *>(malloc(_width * _height * 16 + 4096));
	}

	if (!_cinematic->sControl(buffer))
		error("[ImageLoaderCIN::readImage] Cannot read image");

	if (bitdepth == 17) {
		image->getSurface()->copyRectToSurface(buffer, image->getSurface()->pitch, 0, 0, _width, _height);
	}

	Graphics::Surface *surfaceInvert = new Graphics::Surface();
	invertSurface(surfaceInvert, *image->getSurface());

	image->setSurface(surfaceInvert);

	return true;

	return true;
}

#pragma endregion

} // End of namespace Ring
