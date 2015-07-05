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

#include "ring/graphics/codecs/imageloader_ci2.h"

#include "ring/graphics/image.h"

#include "ring/graphics/movies/cinematic2.h"

#include "ring/helpers.h"

namespace Ring {

bool ImageLoaderCI2::SoundConfiguration::read(Common::SeekableReadStream *stream) {
	channels = stream->readByte();
	bitsPerSample = stream->readByte();
	samplesPerSec = stream->readUint32LE();
	field_6 = stream->readUint16LE();
	field_8 = stream->readUint32LE();
	field_C = stream->readUint32LE();

	return (!stream->err() && !stream->eos());
}

ImageLoaderCI2::ImageLoaderCI2() {
	_cinematic = nullptr;
	memset(&_header, 0, sizeof(_header));
	memset(_soundConfigs, 0, ARRAYSIZE(_soundConfigs));
	_controlTable = nullptr;
	_widthAndPadding = 0;
	_stride = 0;
	_width = 0;
	_height = 0;
}

ImageLoaderCI2::~ImageLoaderCI2() {
	deinit();
}

bool ImageLoaderCI2::load(ImageSurface *image, ArchiveType archiveType, ZoneId zone, LoadFrom loadFrom, DrawType drawType) {
	if (!image)
		error("[ImageLoaderCI2::load] Invalid image pointer");

	byte format = 0;
	_filename = image->getName();

	if (!init(_filename, archiveType, zone, loadFrom)) {
		warning("[ImageLoaderCI2::load] Error initializing image reader (%s)", _filename.c_str());
		goto cleanup;
	}

	// Check cinematic format
	format = _cinematic->readByte();
	if (_cinematic->eos() || _cinematic->err() || format != CINEMATIC_FORMAT) {
		warning("[ImageLoaderCI2::load] Wrong cinematic format for %s (was: %d, valid: %d)", _filename.c_str(), format, CINEMATIC_FORMAT);
		goto cleanup;
	}

	// Read image data
	if (!readImage(image, 32, drawType)) {
		warning("[ImageLoaderCI2::load] Error reading image (%s)", _filename.c_str());
		goto cleanup;
	}

	deinit();

	return true;

cleanup:
	deinit();
	return false;
}

bool ImageLoaderCI2::init(Common::String filename, ArchiveType archiveType, ZoneId zone, LoadFrom loadFrom) {
	_controlTable = nullptr;
	_cinematic = new Cinematic2();

	if (!_cinematic->init(filename, archiveType, zone, loadFrom)) {
		warning("[ImageLoaderCI2::init] Error initializing cinematic (%s)", _filename.c_str());
		return false;
	}

	if (!readHeader()) {
		warning("[ImageLoaderCI2::init] Error reading header (%s)", _filename.c_str());
		return false;
	}

	if (!_cinematic->allocBuffer(_header.width * _header.height + 128)) {
		warning("[ImageLoaderCIN::init] Error allocating cinematic buffer (%s)", _filename.c_str());
		return false;
	}

	if (_header.field_1A)
		_cinematic->setState(true);

	return true;
}

bool ImageLoaderCI2::readHeader() {
	if (!_cinematic)
		error("[ImageLoaderCI2::readHeader] Cinematic not initialized properly");

	memset(&_header, 0, sizeof(_header));

	// Read header (size: 0xC0)
	_cinematic->read(&_header.signature, sizeof(_header.signature));
	_header.chunkCount        = _cinematic->readUint32LE();
	_header.frameRate         = _cinematic->readUint32LE();
	_header.field_D           = _cinematic->readByte();
	_header.width             = _cinematic->readUint32LE();
	_header.height            = _cinematic->readUint32LE();
	_header.field_19          = _cinematic->readByte();
	_header.field_1A          = _cinematic->readByte();
	_header.soundChannelCount = _cinematic->readByte();
	_header.controlTableSize  = _cinematic->readUint32LE();
	_header.field_20          = _cinematic->readUint32LE();
	_header.field_24          = _cinematic->readUint32LE();
	_header.field_28          = _cinematic->readUint32LE();
	_header.field_2C          = _cinematic->readUint32LE();
	_header.field_30          = _cinematic->readUint32LE();
	_header.field_34          = _cinematic->readUint32LE();
	_header.field_38          = _cinematic->readUint32LE();
	_header.field_3C          = _cinematic->readUint32LE();
	_header.field_40          = _cinematic->readUint32LE();
	_header.field_44          = _cinematic->readUint32LE();
	_header.field_48          = _cinematic->readUint32LE();
	_header.field_4C          = _cinematic->readUint32LE();
	_header.field_50          = _cinematic->readUint32LE();
	_header.field_54          = _cinematic->readUint32LE();
	_header.field_58          = _cinematic->readUint32LE();
	_header.field_5C          = _cinematic->readUint32LE();
	_header.field_60          = _cinematic->readUint32LE();
	_header.field_64          = _cinematic->readUint32LE();
	_header.field_68          = _cinematic->readUint32LE();
	_header.field_6C          = _cinematic->readUint32LE();
	_header.field_70          = _cinematic->readUint32LE();
	_header.field_74          = _cinematic->readUint32LE();
	_header.field_78          = _cinematic->readUint32LE();
	_header.field_7C          = _cinematic->readUint32LE();
	_header.field_80          = _cinematic->readUint32LE();
	_header.field_84          = _cinematic->readUint32LE();
	_header.field_88          = _cinematic->readUint32LE();
	_header.field_8C          = _cinematic->readUint32LE();
	_header.field_90          = _cinematic->readUint32LE();
	_header.field_94          = _cinematic->readUint32LE();
	_header.field_98          = _cinematic->readUint32LE();
	_header.field_9C          = _cinematic->readUint32LE();
	_header.field_A0          = _cinematic->readUint32LE();
	_header.field_A4          = _cinematic->readUint32LE();
	_header.field_A8          = _cinematic->readUint32LE();
	_header.field_AC          = _cinematic->readUint32LE();
	_header.field_B0          = _cinematic->readUint32LE();
	_header.field_B4          = _cinematic->readUint32LE();
	_header.field_B8          = _cinematic->readUint32LE();
	_header.field_BC          = _cinematic->readUint32LE();

	// Update width and height
	_width = _header.width;
	_height = _header.height;

	// Check width and height
	if (_width > 4096 || _height == 0) {
		warning("[ImageLoaderCIN::init] Invalid size for image %s (width:%d, height:%d)", _filename.c_str(), _width, _height);
		return false;
	}

	// Check channel count
	if (_header.soundChannelCount > 4) {
		warning("[ImageLoaderCIN::init] Invalid sound channel count for image %s (count: %d)", _filename.c_str(), _header.soundChannelCount);
		return false;
	}

	// Read sound tables
	for (uint32 i = 0; i < _header.soundChannelCount; i++)
		_soundConfigs[i].read(_cinematic);

	// Allocate control table
	uint32 tableSize = 8 * _header.controlTableSize;
	_controlTable = malloc(tableSize + 64);
	if (!_controlTable) {
		warning("[ImageLoaderCIN::init] Cannot allocate control table for image %s", _filename.c_str());
		return false;
	}

	// Read control table
	uint32 readData = _cinematic->read(_controlTable, tableSize);
	if (readData != tableSize || _cinematic->err() || _cinematic->eos()) {
		warning("[ImageLoaderCIN::init] Cannot read control table for image %s", _filename.c_str());
		return false;
	}

	return true;
}

bool ImageLoaderCI2::readImage(ImageSurface *image, byte bitdepth, DrawType drawType) {
	if (!_cinematic)
		error("[ImageLoaderCI2::readImage] Cinematic not initialized properly");

	if (!image)
		error("[ImageLoaderCI2::readImage] Invalid image pointer");

	if (!image->isInitialized())
		image->create(bitdepth, 2, _width, _height);

	// Compute stride
	_widthAndPadding = _width + 3;
	_stride = _widthAndPadding * 3;

	if (drawType == kDrawTypeAlpha) {
		// Create buffer to hold the data
		byte *buffer = (byte *)malloc(_height * _stride + 64);
		if (!buffer) {
			warning("[ImageLoaderCI2::readImage] Cannot create buffer for image %s", _filename.c_str());
			return false;
		}

		if (!_cinematic->sControl(buffer, bitdepth)) {
			free(buffer);

			warning("[ImageLoaderCI2::readImage] Error when decompressing image %s", _filename.c_str());
			return false;
		}

		// TODO Copy to image
		warning("[ImageLoaderCI2::readImage] image copy for kDrawType3 not implemented");

		free(buffer);
	} else {
		if (!_cinematic->sControl((byte *)image->getSurface()->getPixels(), bitdepth)) {
			warning("[ImageLoaderCI2::readImage] Error when decompressing image %s", _filename.c_str());
			return false;
		}
	}

	return true;
}

void ImageLoaderCI2::deinit() {
	SAFE_DELETE(_cinematic);
	free(_controlTable);
	_controlTable = nullptr;
}

} // End of namespace Ring
