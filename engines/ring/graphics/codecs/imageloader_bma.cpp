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

#include "ring/graphics/codecs/imageloader_bma.h"

#include "ring/base/stream.h"

#include "ring/graphics/image.h"

#include "ring/helpers.h"

namespace Ring {

ImageLoaderBMA::ImageLoaderBMA() {
	_stream = nullptr;
	memset(&_header, 0, sizeof(_header));
	_coreSize = 0;
	_seqSize = 0;
	_blockSize = 0;
}

ImageLoaderBMA::~ImageLoaderBMA() {
	deinit();
}

bool ImageLoaderBMA::load(ImageSurface *image, ArchiveType type, ZoneId zone, LoadFrom loadFrom, DrawType) {
	if (!image)
		error("[ImageLoaderBMA::load] Invalid image pointer!");

	_stream = nullptr;
	_filename = image->getName();

	if (!init(type, zone, loadFrom)){
		warning("[ImageLoaderBMA::load] Error opening image file (%s)", _filename.c_str());
		goto cleanup;
	}

	// Read header
	if (!readHeader()) {
		warning("[ImageLoaderBMA::load] Error reading header (%s)", _filename.c_str());
		goto cleanup;
	}

	// Read image data
	if (!readImage(image)) {
		warning("[ImageLoaderBMA::load] Error reading image (%s)", _filename.c_str());
		goto cleanup;
	}

	deinit();

	return true;

cleanup:
	deinit();
	return false;
}

bool ImageLoaderBMA::init(ArchiveType type, ZoneId zone, LoadFrom loadFrom) {
	_stream = new CompressedStream();

	// Initialize stream
	switch (type) {
	default:
		warning("[ImageLoaderBMA::init] Invalid archive type (%d)!", type);
		break;

	case kArchiveFile:
		return _stream->init(_filename, 1, 0);

	case kArchiveArt:
		return _stream->initArt(_filename, zone, loadFrom);
	}

	return false;
}

void ImageLoaderBMA::deinit() {
	SAFE_DELETE(_stream);
}

bool ImageLoaderBMA::readHeader() {
	if (!_stream)
		error("[ImageLoaderBMA::readHeader] Stream not initialized");

	Common::SeekableReadStream *data = _stream->getCompressedStream();
	if (!data) {
		warning("[ImageLoaderBMA::readHeader] Cannot get compressed stream (%s)", _filename.c_str());
		return false;
	}

	// Read Seq size
	_seqSize = data->readUint32LE();

	// Read header
	_header.coreWidth = data->readUint16LE();
	_header.coreHeight = data->readUint16LE();
	_header.seqWidth = data->readUint32LE();
	_header.seqHeight = data->readUint32LE();
	_header.field_C = data->readUint32LE();
	_header.field_10 = data->readUint16LE();

	// Read core size
	data->seek(_seqSize + 76, SEEK_SET);
	_coreSize = data->readUint32LE();

	// Compute block size
	_blockSize = 0;
	uint16 val = _header.coreWidth;
	if (val) {
		do {
			val >>= 1;
			++_blockSize;
		} while (val);
	}

	return true;
}

bool ImageLoaderBMA::readImage(ImageSurface *image) {
	if (!_stream)
		error("[ImageLoaderBMA::readImage] Stream not initialized");

	Common::MemoryReadStream *imageData = _stream->decompressIndexed(_blockSize,
	                                                                 _seqSize,
	                                                                 (2 * _header.seqWidth * _header.seqHeight) / _header.coreHeight,
	                                                                 _coreSize,
	                                                                 2 * _header.coreWidth * _header.coreHeight,
	                                                                 _header.seqWidth * _header.seqHeight * 2,
	                                                                 2 * _header.seqWidth * _header.seqHeight - 6,
	                                                                 _header.field_C,
	                                                                 _header.field_10);

	// Create surface to hold the data
	Graphics::PixelFormat format = g_system->getScreenFormat();
	format.bytesPerPixel = 2;

	Graphics::Surface *surface = new Graphics::Surface();
	surface->create((uint16)_header.seqWidth, (uint16)_header.seqHeight, format); // FIXME: Always 16bpp BMPs?

	// Read from compressed stream
	imageData->read(surface->getPixels(), _header.seqWidth * _header.seqHeight * 2);
	delete imageData;

	Graphics::Surface *surfaceInvert = new Graphics::Surface();
	invertSurface(surfaceInvert, *surface);

	image->setSurface(surfaceInvert);

	return true;
}

} // End of namespace Ring
