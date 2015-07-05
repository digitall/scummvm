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

#ifndef RING_IMAGELOADER_H
#define RING_IMAGELOADER_H

#include "ring/shared.h"

namespace Ring {

class Cinematic;
class ImageSurface;

class ImageLoader {
public:
	virtual ~ImageLoader() {};

	virtual bool load(ImageSurface *image, ArchiveType archiveType, ZoneId zone, LoadFrom loadFrom, DrawType drawType) = 0;

protected:
	Common::String _filename;
};

#define CINEMATIC_FORMAT 0x53

class ImageLoaderMovie : public ImageLoader {
public:
	virtual ~ImageLoaderMovie() {};

	virtual bool init(Common::String path, ArchiveType archiveType, ZoneId zone, LoadFrom loadFrom) = 0;
	virtual bool readImage(ImageSurface *image, byte bitdepth, DrawType drawType) = 0;

	virtual Cinematic *getCinematic() = 0;
	virtual uint32 getChunkCount() = 0;
	virtual uint32 getFrameRate() = 0;
	virtual byte getChannels(uint32 index) = 0;
	virtual byte getBitsPerSample(uint32 index) = 0;
	virtual byte getSamplesPerSec(uint32 index) = 0;
};

} // End of namespace Ring

#endif // RING_IMAGELOADER_H
