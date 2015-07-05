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

#ifndef RING_IMAGELOADER_CI2_H
#define RING_IMAGELOADER_CI2_H

#include "ring/graphics/codecs/imageloader.h"

#include "ring/shared.h"

namespace Ring {

class Cinematic;
class Cinematic2;
class ImageSurface;

class ImageLoaderCI2 : public ImageLoaderMovie {
public:
	struct Header {
		char   signature[8];
		uint32 chunkCount;
		uint32 frameRate;
		byte field_D;
		uint32 width;
		uint32 height;
		byte   field_19;
		byte   field_1A;
		byte   soundChannelCount;
		uint32 controlTableSize;
		uint32 field_20;
		uint32 field_24;
		uint32 field_28;
		uint32 field_2C;
		uint32 field_30;
		uint32 field_34;
		uint32 field_38;
		uint32 field_3C;
		uint32 field_40;
		uint32 field_44;
		uint32 field_48;
		uint32 field_4C;
		uint32 field_50;
		uint32 field_54;
		uint32 field_58;
		uint32 field_5C;
		uint32 field_60;
		uint32 field_64;
		uint32 field_68;
		uint32 field_6C;
		uint32 field_70;
		uint32 field_74;
		uint32 field_78;
		uint32 field_7C;
		uint32 field_80;
		uint32 field_84;
		uint32 field_88;
		uint32 field_8C;
		uint32 field_90;
		uint32 field_94;
		uint32 field_98;
		uint32 field_9C;
		uint32 field_A0;
		uint32 field_A4;
		uint32 field_A8;
		uint32 field_AC;
		uint32 field_B0;
		uint32 field_B4;
		uint32 field_B8;
		uint32 field_BC;
	};

	struct SoundConfiguration {
		byte   channels;
		byte   bitsPerSample;
		uint32 samplesPerSec;
		uint16 field_6;
		uint32 field_8;
		uint32 field_C;

		SoundConfiguration() {
			channels = 0;
			bitsPerSample = 0;
			samplesPerSec = 0;
			field_6 = 0;
			field_8 = 0;
			field_C = 0;
		}

		bool read(Common::SeekableReadStream *stream);
	};

	ImageLoaderCI2();
	~ImageLoaderCI2();

	// ImageLoader
	bool load(ImageSurface *image, ArchiveType archiveType, ZoneId zone, LoadFrom loadFrom, DrawType drawType) override;

	// ImageLoaderMovie
	bool init(Common::String path, ArchiveType archiveType, ZoneId zone, LoadFrom loadFrom) override;
	bool readImage(ImageSurface *image, byte bitdepth, DrawType drawType) override;

	Cinematic *getCinematic()           override { return reinterpret_cast<Cinematic *>(_cinematic); }
	uint32 getChunkCount()              override { return _header.chunkCount; }
	uint32 getFrameRate()               override { return _header.frameRate; }
	byte getChannels(uint32 index)      override { return _soundConfigs[index].channels; }
	byte getBitsPerSample(uint32 index) override { return _soundConfigs[index].bitsPerSample; }
	byte getSamplesPerSec(uint32 index) override { return _soundConfigs[index].samplesPerSec; }

private:
	Cinematic2         *_cinematic;
	Header              _header;
	SoundConfiguration  _soundConfigs[4];
	void               *_controlTable;
	uint32              _widthAndPadding;
	uint32              _stride;
	uint32              _width;
	uint32              _height;

	void deinit();
	bool readHeader();
};

} // End of namespace Ring

#endif // RING_IMAGELOADER_CI2_H
