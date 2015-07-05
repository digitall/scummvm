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

#ifndef RING_IMAGELOADER_CIN_H
#define RING_IMAGELOADER_CIN_H

#include "ring/graphics/codecs/imageloader.h"

#include "ring/shared.h"

namespace Ring {

class Cinematic;
class Cinematic1;
class ImageSurface;

class ImageLoaderCIN : public ImageLoaderMovie {
public:
	struct Header {
		char   signature[8];
		byte   channels;
		byte   bitsPerSample;
		byte   samplesPerSec;
		byte   field_B;
		uint16 field_C;
		uint32 chunkCount;
		uint32 frameRate;
		byte   field_16;
		uint32 width;
		uint32 height;
		byte   field_1F;
		uint32 field_20;
		uint32 field_24;
		uint32 field_28;
		uint32 field_2C;
		uint32 field_30;
		uint32 field_34;
		uint32 field_38;
		uint32 field_3C;
	};

	ImageLoaderCIN();
	~ImageLoaderCIN();

	// ImageLoader
	bool load(ImageSurface *image, ArchiveType archiveType, ZoneId zone, LoadFrom loadFrom, DrawType drawType) override;

	// ImageLoaderMovie
	bool init(Common::String filename, ArchiveType type = kArchiveFile, ZoneId zone = kZoneNone, LoadFrom loadFrom = kLoadFromCd) override;
	virtual bool readImage(ImageSurface *image, byte bitdepth = 16, DrawType drawType = kDrawTypeNormal) override;

	Cinematic *getCinematic()     override { return reinterpret_cast<Cinematic *>(_cinematic); }
	uint32 getChunkCount()        override { return _header.chunkCount; }
	uint32 getFrameRate()         override { return _header.frameRate; }
	byte getChannels(uint32)      override { return _header.channels; }
	byte getBitsPerSample(uint32) override { return _header.bitsPerSample; }
	byte getSamplesPerSec(uint32) override { return _header.samplesPerSec; }

private:
	Cinematic1 *_cinematic;
	Header _header;
	uint32 _stride;
	uint32 _widthAndPadding;
	uint32 _width;
	uint32 _height;

	void deinit();
	bool readHeader();
};

} // End of namespace Ring

#endif // RING_IMAGELOADER_CIN_H
