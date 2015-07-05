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

#ifndef RING_IMAGELOADER_BMA_H
#define RING_IMAGELOADER_BMA_H

#include "ring/graphics/codecs/imageloader.h"

#include "ring/shared.h"

namespace Ring {

class CompressedStream;
class ImageSurface;

class ImageLoaderBMA : public ImageLoader {
public:
	ImageLoaderBMA();
	~ImageLoaderBMA();

	bool load(ImageSurface *image, ArchiveType type, ZoneId zone, LoadFrom loadFrom, DrawType drawType) override;

private:
	struct Header {
		uint16 coreWidth;
		uint16 coreHeight;
		uint32 seqWidth;
		uint32 seqHeight;
		uint32 field_C;
		uint16 field_10;
	};

	Header _header;
	uint32 _coreSize;
	uint32 _seqSize;
	uint32 _blockSize;

	CompressedStream *_stream;

	bool init(ArchiveType type, ZoneId zone, LoadFrom loadFrom);
	void deinit();
	bool readHeader();
	bool readImage(ImageSurface *image);
	void copySurface(Graphics::Surface *out, const Graphics::Surface &in);
};

} // End of namespace Ring

#endif // RING_IMAGELOADER_BMA_H
