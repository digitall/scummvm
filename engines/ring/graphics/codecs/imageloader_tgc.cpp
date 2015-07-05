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

#include "ring/graphics/codecs/imageloader_tgc.h"

#include "ring/base/stream.h"

#include "ring/graphics/image.h"

#include "ring/helpers.h"

#include "image/tga.h"

namespace Ring {

ImageLoaderTGC::ImageLoaderTGC() {
	_stream = nullptr;
}

ImageLoaderTGC::~ImageLoaderTGC() {
	deinit();
}

bool ImageLoaderTGC::load(ImageSurface *image, ArchiveType type, ZoneId zone, LoadFrom loadFrom, DrawType) {
	if (!image)
		error("[ImageLoaderTGC::load] Invalid image pointer!");

	_stream = nullptr;
	_filename = image->getName();

	Common::SeekableReadStream *decompressedData = init(type, zone, loadFrom);
	if (!decompressedData){
		warning("[ImageLoaderTGC::load] Error opening image file (%s)", _filename.c_str());
		deinit();
		return false;
	}

	// Get image surface
	Image::TGADecoder *tga = new Image::TGADecoder();
	bool loaded = tga->loadStream(*decompressedData);
	if (!loaded) {
		warning("[ImageLoaderBMP::load] Cannot decode image (%s)", image->getName().c_str());
		delete tga;
		deinit();
		return false;
	}

	Graphics::Surface *surface = new Graphics::Surface();
	surface->copyFrom(*tga->getSurface());

	image->setSurface(surface);

	delete tga;
	deinit();

	return true;
}

Common::SeekableReadStream *ImageLoaderTGC::init(ArchiveType type, ZoneId zone, LoadFrom loadFrom) {
	_stream = new CompressedStream();

	// Initialize stream
	switch (type) {
	default:
		warning("[ImageLoaderTGC::init] Invalid archive type (%d)!", type);
		return nullptr;

	case kArchiveFile:
		if (!_stream->init(_filename, 1, 0))
			return nullptr;
		break;

	case kArchiveArt:
		if (!_stream->initArt(_filename, zone, loadFrom))
			return nullptr;
		break;
	}

	// TGC are compressed as chunks of up to 64kb
	Common::SeekableReadStream *stream = _stream->getCompressedStream();
	uint32 chunks = stream->readUint32LE();
	uint32 size = stream->readUint32LE();

	return _stream->decompressChuncks(chunks, size);
}

void ImageLoaderTGC::deinit() {
	SAFE_DELETE(_stream);
}

} // End of namespace Ring
