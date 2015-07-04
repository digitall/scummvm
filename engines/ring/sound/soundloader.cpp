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

#include "ring/sound/soundloader.h"

#include "ring/helpers.h"

#include "audio/decoders/wave.h"
#include "audio/decoders/raw.h"

#include "common/archive.h"

namespace Ring {

#pragma region SoundResource

SoundResource::SoundResource() {
	_buffer  = nullptr;
	_currentPointer = nullptr;
	_size    = 0;
}

SoundResource::~SoundResource() {
	cleanup();
}

void SoundResource::cleanup() {
	SAFE_FREE(_buffer);
	_currentPointer = nullptr;
	_size = 0;
}

void SoundResource::add(void *data, uint32 dataSize) {
	if (!_buffer)
		error("[SoundResource::add] Buffer not initialized properly");

	if (data == nullptr)
		error("[SoundResource::add] Invalid data pointer");

	if (dataSize == 0)
		error("[SoundResource::add] Invalid data size (cannot be 0)");

	// Check if existing data is present
	if (_size) {

		// Create new buffer to hold the data
		uint32 totalSize = _size + dataSize;
		byte *buffer = (byte *)malloc(totalSize);
		if (!buffer)
			error("[SoundResource::add] Cannot reallocate memory for SoundResource buffer");

		memcpy(buffer, _buffer, _size);
		memcpy((byte*)(buffer + _size), data, dataSize);

		cleanup();

		_buffer = buffer;
		_currentPointer = buffer;
		_size = totalSize;

	} else {
		cleanup();

		byte *buffer = (byte *)malloc(dataSize);
		if (!buffer)
			error("[SoundResource::add] Cannot allocate memory for SoundResource buffer");

		memcpy(buffer, data, dataSize);

		_buffer = buffer;
		_currentPointer = buffer;
		_size = dataSize;
	}
}

void SoundResource::getBuffer(SoundBuffer *buffer, uint32 size) {
	if (!_currentPointer)
		error("[SoundResource::getBuffer] Current pointer not initialized properly");

	if (!buffer)
		error("[SoundResource::getBuffer] Invalid info parameter");

	if (_size) {
		if (size < _size || size == _size) {
			_size -= size;

			buffer->buffer = _currentPointer;
			buffer->size   = size;

			_currentPointer += size;
		} else {
			buffer->buffer = _currentPointer;
			buffer->size   = _size;

			_size = 0;
			_currentPointer = &_currentPointer[_size];
		}
	} else {
		cleanup();
		buffer->buffer = nullptr;
		buffer->size   = 0;
	}
}

void SoundResource::getBuffer(SoundBuffer *buffer) {
	return getBuffer(buffer, _size);
}

#pragma endregion

#pragma region CompressedSound

CompressedSound::CompressedSound() {
	_stream = nullptr;
	_buffer = nullptr;
	_field_C = 0;
	_field_10 = 0;
	_resource = nullptr;

	// WAV related
	_type = 0;
	_samplesPerSec = 0;
	_blockAlign = 0;
	_flags = 0;
	_chunkCount = 0;
	_dataSize = 0;

	// Stream related
	_offset = 0;
	_initialValue = 0;
}

bool CompressedSound::init(const Common::String &path) {
	_stream = SearchMan.createReadStreamForMember(path);
	if (!_stream) {
		warning("[CompressedSound::init] Error opening file (%s)", path.c_str());
		return false;
	}

	_resource = new SoundResource();

	if (!decompressHeader())
		return false;

	return true;
}

uint16 CompressedSound::getBitsPerSample() const {
	if (_flags & Audio::FLAG_UNSIGNED)
		return 8;

	if (_flags & Audio::FLAG_16BITS) {
		if (_flags & Audio::FLAG_LITTLE_ENDIAN)
			return 16;

		return 4;
	}

	error("[CompressedSound::getBitsPerSample] Invalid set of flags");
}

uint32 CompressedSound::decode(byte delta, uint32 start, uint32, int16 *buffer) {
	if (!_stream)
		error("[CompressedSound::decode] Stream not initialized properly");

	// Store buffer position
	int16 *bufferStart = buffer;

	start += 3;

	while (buffer < (bufferStart + 256)) {
		_stream->seek(start >> 3, SEEK_SET);

		uint32 var = _stream->readUint32BE();
		uint32 position = 31 - (start & 7);

		if (CHECK_BIT(var, position)) {
			_initialValue += _offset;
			WRITE_UINT16(buffer, (uint16)_initialValue);
			++buffer;
			++start;
		} else {
			uint16 decoded = (uint16)((var << (32 - position)) >> (32 - delta));

			if (decoded >= 512)
				decoded = 512 - decoded;

			_offset = (uint16)(decoded << 6);

			_initialValue += _offset;
			WRITE_UINT16(buffer, _initialValue);
			++buffer;

			start += 11;
		}
	}

	return (uint32)(buffer - bufferStart);
}

#pragma endregion

#pragma region CompressedSoundMono

CompressedSoundMono::CompressedSoundMono() : CompressedSound() {
	_compressedDataOffset = 0;
}

CompressedSoundMono::~CompressedSoundMono() {
	SAFE_DELETE(_stream);
	SAFE_FREE(_buffer);
	_field_C = 0;
	SAFE_DELETE(_resource);
}

bool CompressedSoundMono::decompressHeader() {
	if (!_stream)
		error("[CompressedSoundMono::decompressHeader] sound stream not initialized");

	_chunkCount = _stream->readUint32LE();
	_dataSize   = _stream->readUint32LE();

	int size;
	if (!Audio::loadWAVFromStream(*_stream, size, _samplesPerSec, _flags, &_type, &_blockAlign)) {
		warning("[CompressedSoundMono::decompressHeader] Cannot parse WAVE header");
		return false;
	}

	// Setup fields and buffer
	--_chunkCount;

	_compressedDataOffset = _stream->readUint16LE();
	_field_C  = 0;
	_field_10 = 44;

	_buffer = (byte *)malloc(1536);
	if (!_buffer)
		error("[CompressedSoundMono::decompressHeader] Cannot allocate sound decoding buffer");

	return true;
}

bool CompressedSoundMono::decompress(SoundBuffer *buffer) {
	error("[CompressedSoundMono::decompress] Not implemented");
}

bool CompressedSoundMono::getChunk() {
	if (!_stream)
		error("[CompressedSoundMono::getChunk] Stream not initialized properly");

	_stream->seek(52, SEEK_SET);
	_compressedDataOffset = _stream->readUint16LE();

	_field_C  = 0;
	_field_10 = 44;

	return true;
}

#pragma endregion

#pragma region CompressedSoundStereo

CompressedSoundStereo::CompressedSoundStereo() : CompressedSound() {
	_compressedDataOffset = 0;
	_field_30 = 0;
}

CompressedSoundStereo::~CompressedSoundStereo() {
	SAFE_DELETE(_stream);
	SAFE_FREE(_buffer);
	_field_C = 0;
	SAFE_DELETE(_resource);
}

bool CompressedSoundStereo::decompressHeader() {
	if (!_stream)
		error("[CompressedSoundStereo::decompressHeader] sound stream not initialized");

	_chunkCount = _stream->readUint32LE();
	_dataSize   = _stream->readUint32LE();

	int size;
	if (!Audio::loadWAVFromStream(*_stream, size, _samplesPerSec, _flags, &_type, &_blockAlign)) {
		warning("[CompressedSoundStereo::decompressHeader] Cannot parse WAVE header");
		return false;
	}

	// Setup fields and buffer
	--_chunkCount;

	_field_C  = 0;
	_field_10 = 44;
	_compressedDataOffset = _stream->readUint32LE();
	_field_30 = _stream->readUint32LE();

	// Compute buffer size
	uint32 bufferSize = (uint32)((getType() != 0 ? 2 : 1) * getSamplesPerSec() * getBitsPerSample());

	_buffer = (byte *)malloc(bufferSize + 1024);
	if (!_buffer)
		error("[CompressedSoundStereo::decompressHeader] Cannot allocate sound decoding buffer");

	return true;
}

bool CompressedSoundStereo::decompress(SoundBuffer *soundBuffer) {
	if (!_stream || !_resource)
		error("[CompressedSoundStereo::decompress] Stream not initialized");

	// Compute buffer size
	uint32 size = soundBuffer->size;
	if (size > _dataSize - _field_10)
		size = _dataSize - _field_10;

	if (!size)
		error("[CompressedSoundStereo::decompress] Invalid buffer size (cannot be 0)");

	// Allocate buffer
	byte *buffer = (byte *)malloc(size + 1024);
	if (!buffer)
		error("[CompressedSoundStereo::decompress] Cannot allocate buffer for sound decompression (size: %d)", size + 1024);

	while (true) {

		// Get buffer from the sound resource
		if (_resource->getSize()) {
			error("[CompressedSoundStereo::decompress] Not implemented");
		}

		// Check that the stream contains enough data
		int32 remainingSize = _stream->size() - _stream->pos();
		if (remainingSize < 0 || remainingSize < (int32)_compressedDataOffset + 8)
			error("[CompressedSoundStereo::decompress] Invalid remaining stream size (needed: %d, found: %d)", _compressedDataOffset + 8, remainingSize);

		if (_field_30 >= size)
			break;

		error("[CompressedSoundStereo::decompress] Not implemented");
	}

	error("[CompressedSoundStereo::decompress] Not implemented");
}

bool CompressedSoundStereo::getChunk() {
	if (!_stream)
		error("[CompressedSoundStereo::getChunk] Stream not initialized");

	_stream->seek(52, SEEK_SET);
	_compressedDataOffset = _stream->readUint32LE();
	_field_30 = _stream->readUint32LE();

	_field_C  = 0;
	_field_10 = 44;

	return true;
}

#pragma endregion

#pragma region SoundLoader

SoundLoader::SoundLoader(SoundFormat format) {
	_type = 0;
	_channels = 0;
	_samplesPerSec = 0;
	_avgBytesPerSec = 0;
	_blockAlign = 0;
	_bitsPerSample = 0;
	//_field_10 = 0;
	_compressedStream = nullptr;
	_field_16 = false;
	//_field_1A = 0;
	_format = format;
}

SoundLoader::~SoundLoader() {
	SAFE_DELETE(_compressedStream);
}

bool SoundLoader::load(const Common::String &path, SoundEntryStream *soundEntry) {
	_field_16 = false;

	switch (_format) {
	default:
		error("[SoundLoader::load] Invalid sound format (%d)", _format);

	case kSoundFormatWAC:
		_compressedStream = new CompressedSoundMono();
		break;

	case kSoundFormatWAS:
		_compressedStream = new CompressedSoundStereo();
		break;
	}

	if (!_compressedStream->init(path))
		return true;

	// Initialize loader data from wave header
	_type = _compressedStream->getType();
	//_channels = _compressedStream->getChannels();
	_samplesPerSec = _compressedStream->getSamplesPerSec();
	//_avgBytesPerSec = _compressedStream->getAvgBytesPerSec();
	_bitsPerSample = _compressedStream->getBitsPerSample();
	_blockAlign    = _compressedStream->getBlockAlign();
	//_field_10 = ???

	return false;
}

void SoundLoader::close() {
	SAFE_DELETE(_compressedStream);
}

bool SoundLoader::getChunk() {
	if (!_compressedStream)
		error("[SoundLoader::getChunk] Compressed stream not initialized properly");

	_compressedStream->getChunk();
	_field_16 = false;

	return false;
}

uint32 SoundLoader::getDataSize() {
	if (!_compressedStream)
		error("[SoundLoader::getChunk] Compressed stream not initialized properly");

	return _compressedStream->getDataSize();
}

bool SoundLoader::read(uint32 size, byte *buffer, uint32 *readSize) {
	if (!_compressedStream)
		error("[SoundLoader::read] Stream not initialized");

	if (_field_16) {
		*readSize = 0;
		return false;
	}

	SoundBuffer bufferDec;
	if (!_compressedStream->decompress(&bufferDec))
		return true;

	// Copy decompressed data
	memcpy(buffer, bufferDec.buffer, bufferDec.size);
	*readSize = bufferDec.size;

	if (bufferDec.field_4) {
		_field_16 = true;
		*readSize -= 88;
	}

	return false;
}

#pragma endregion

} // End of namespace Ring
