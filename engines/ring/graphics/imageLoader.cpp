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

#include "ring/graphics/imageLoader.h"

#include "ring/base/stream.h"

#include "ring/graphics/image.h"
#include "ring/graphics/movie.h"

#include "ring/helpers.h"

#include "common/archive.h"

#include "image/bmp.h"
#include "image/tga.h"

namespace Ring {

#pragma region BMP

bool ImageLoaderBMP::load(ImageSurface *image, ArchiveType, ZoneId, LoadFrom, DrawType) {
	if (!image)
		error("[ImageLoaderBMP::load] Invalid image pointer!");

	// Get image stream
	Common::SeekableReadStream *stream = SearchMan.createReadStreamForMember(image->getName());
	if (!stream) {
		warning("[ImageLoaderBMP::load] Cannot load image (%s)", image->getName().c_str());
		return false;
	}

	// Get image surface
	Image::BitmapDecoder *bmp = new Image::BitmapDecoder();
	bool loaded = bmp->loadStream(*stream);
	if (!loaded) {
		warning("[ImageLoaderBMP::load] Cannot decode image (%s)", image->getName().c_str());
		delete bmp;
		delete stream;
		return false;
	}

	Graphics::Surface *surface = new Graphics::Surface();
	surface->copyFrom(*bmp->getSurface());

	image->setSurface(surface);

	delete stream;
	delete bmp;

	return true;
}

#pragma endregion

#pragma region BMA

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
	copySurface(surfaceInvert, *surface);

	image->setSurface(surfaceInvert);

	return true;
}

void ImageLoaderBMA::copySurface(Graphics::Surface *out, const Graphics::Surface &in) {

	// Images should be decoded with origin at the top
	out->create(in.w, in.h, in.format);

	switch (in.format.bytesPerPixel) {
	default:
		error("[ImageLoader::copySurface] Unsupported pixel depth (%s)", _filename.c_str());

	// FIXME Handle endianess
	case 2:
		for (int i = 0; i < out->h; i++) {
			uint16 *dst = (uint16 *) out->getBasePtr(0, out->h - i - 1);
			uint16 *orig = (uint16 *) in.getBasePtr(0, i);

			for (int j = 0; j < out->w; j++)
				*dst++ = *orig++;
		}
		break;

	case 4:
		for (int i = 0; i < out->h; i++) {
			uint32 *dst = (uint32 *) out->getBasePtr(0, out->h - i - 1);
			uint32 *orig = (uint32 *) in.getBasePtr(0, i);

			for (int j = 0; j < out->w; j++)
				*dst++ = *orig++;
		}
		break;
	}
}

#pragma endregion

#pragma region TGC

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

#pragma endregion

#pragma region TGA

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

#pragma region CNM

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
		image->create(bitdepth, 2, _width, _height);

	// Compute stride
	_widthAndPadding = _width + 3;
	_stride = _widthAndPadding * 3;

	if (!_cinematic->sControl((byte *)image->getSurface()->getPixels()))
		error("[ImageLoaderCIN::readImage] Cannot read image");

	return true;
}

#pragma endregion

#pragma region CI2

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

#pragma endregion

} // End of namespace Ring
