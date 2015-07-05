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

#include "ring/graphics/codecs/imageloader_tga.h"

#include "ring/base/stream.h"

#include "ring/graphics/image.h"

#include "common/archive.h"

#include "image/tga.h"

namespace Ring {

bool ImageLoaderTGA::load(ImageSurface *image, ArchiveType, ZoneId, LoadFrom, DrawType) {
	if (!image)
		error("[ImageLoaderTGA::load] Invalid image pointer!");

	_filename = image->getName();

	// Open a stream
	Common::SeekableReadStream *stream = SearchMan.createReadStreamForMember(_filename);
	if (!stream) {
		warning("[ImageLoaderTGA::load] Error opening image file (%s)", _filename.c_str());
		delete stream;
		return false;
	}

	// Get image surface
	Image::TGADecoder *tga = new Image::TGADecoder();
	bool loaded = tga->loadStream(*stream);
	if (!loaded) {
		warning("[ImageLoaderBMP::load] Cannot decode image (%s)", image->getName().c_str());
		delete stream;
		return false;
	}

	Graphics::Surface *surface = new Graphics::Surface();
	surface->copyFrom(*tga->getSurface());

	image->setSurface(surface);

	delete stream;

	return true;
}

#pragma endregion

} // End of namespace Ring
