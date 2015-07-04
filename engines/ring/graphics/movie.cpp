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

#include "ring/graphics/movie.h"

#include "ring/base/application.h"
#include "ring/base/art.h"
#include "ring/base/dialog.h"
#include "ring/base/preferences.h"
#include "ring/base/puzzle.h"

#include "ring/graphics/image.h"
#include "ring/graphics/imageLoader.h"
#include "ring/graphics/screen.h"

#include "ring/sound/soundentry.h"
#include "ring/sound/soundhandler.h"
#include "ring/sound/soundmanager.h"

#include "ring/ring.h"
#include "ring/helpers.h"

#include "audio/decoders/adpcm.h"

#include "common/file.h"
#include "common/memstream.h"
#include "common/timer.h"

namespace Ring {

#define CINEMATIC_BUFFER_SIZE          1024
#define CINEMATIC_BACKBUFFER_SIZE      1200000
#define CINEMATIC_TCONTROLBUFFER_SIZE  8192
#define CINEMATIC_CACHEBUFFER_SIZE     512

#pragma region Cinematic

#pragma region ReadStream

bool Cinematic::eos() const {
	if (!_stream)
		error("[Cinematic::eos] Not initialized properly!");

	return _stream->eos();
}

uint32 Cinematic::read(void *dataPtr, uint32 dataSize) {
	if (!_stream)
		error("[Cinematic::read] Not initialized properly!");

	return _stream->read(dataPtr, dataSize);
}

#pragma endregion

#pragma region SeekableReadStream

int32 Cinematic::pos() const {
	if (!_stream)
		error("[Cinematic::pos] Not initialized properly!");

	return _stream->pos();
}

int32 Cinematic::size() const {
	if (!_stream)
		error("[Cinematic::size] Not initialized properly!");

	return _stream->size();
}

bool Cinematic::seek(int32 offset, int whence) {
	if (!_stream)
		error("[Cinematic::seek] Not initialized properly!");

	return _stream->seek(offset, whence);
}

#pragma endregion

#pragma endregion

#pragma region Cinematic1

Cinematic1::Cinematic1() {
	_stream              = nullptr;
	_buffer              = nullptr;
	_buffer2             = nullptr;
	_state               = false;
	_backBuffer          = nullptr;
	_tControlBuffer      = nullptr;
	_cacheBuffer         = nullptr;
	_compressedData      = nullptr;
	_compressedDataEnd   = nullptr;
	_compressedBuffer    = nullptr;
	_compressedBufferEnd = nullptr;
	_field_46            = 0;
	_isStreaming         = false;
}

Cinematic1::~Cinematic1() {
	deinit();
}

bool Cinematic1::init(Common::String filename) {
	_stream = SearchMan.createReadStreamForMember(filename);
	if (!_stream || _stream->err()) {
		warning("[Cinematic1::init] Error opening file (%s)", filename.c_str());
		return false;
	}

	// Create buffers
	_buffer = (byte *)calloc(CINEMATIC_BUFFER_SIZE, 1);
	if (!_buffer)
		error("[Cinematic1::init] Error creating main buffer!");

	_buffer2 = _buffer;
	_state = false;

	_backBuffer = (byte *)calloc(CINEMATIC_BACKBUFFER_SIZE, 1);
	if (!_backBuffer)
		error("[Cinematic1::init] Error creating back buffer!");

	_tControlBuffer = (TControl *)calloc(CINEMATIC_TCONTROLBUFFER_SIZE, sizeof(TControl));
	if (!_tControlBuffer)
		error("[Cinematic1::init] Error creating control buffer!");

	_cacheBuffer = (byte *)calloc(CINEMATIC_CACHEBUFFER_SIZE, 1);
	if (!_cacheBuffer)
		error("[Cinematic1::init] Error creating cache buffer!");

	_compressedData = nullptr;
	_compressedDataEnd = nullptr;
	_compressedBuffer = nullptr;
	_compressedBufferEnd = nullptr;
	_field_46 = 0;

	return true;
}

void Cinematic1::deinit() {
	SAFE_DELETE(_stream);

	// Free buffers
	free(_buffer);
	_buffer2 = nullptr;

	free(_backBuffer);
	free(_tControlBuffer);
	free(_cacheBuffer);
	free(_compressedData);
	_compressedDataEnd = nullptr;
	_compressedBuffer = nullptr;
	_compressedBufferEnd = nullptr;
	_field_46 = 0;
}

void Cinematic1::skipFrame() {
	if (!_tControlBuffer)
		error("[Cinematic1::skipFrame] Control buffer not initialized");

	// Reset tControl buffer
	memset(_tControlBuffer, 0, CINEMATIC_TCONTROLBUFFER_SIZE * sizeof(TControl));

	// Read frame header
	FrameHeader header;
	header.read(_stream);

	debugC(kRingDebugMovie, "    Reading Frame header (size: %d, field_4: %d, field_8: %d, field_A: %d, width: %d, height: %d)",
	       header.size, header.field_4, header.field_8, header.field_A, header.width, header.height);
	debugC(kRingDebugMovie, "    Skipping frame data (size: %d)", header.size);

	// Skip frame
	seek(header.size, SEEK_CUR);
}

bool Cinematic1::tControl() {
	if (!_tControlBuffer)
		error("[Cinematic1::tControl] Control buffer not initialized");

	if (!_stream)
		error("[Cinematic1::tControl] Stream not initialized");

	// Reset tControl buffer
	memset(_tControlBuffer, 0, CINEMATIC_TCONTROLBUFFER_SIZE * sizeof(TControl));

	// Read tControl header
	_tControlHeader.load(_stream);

	debugC(kRingDebugMovie, "    Reading T control header (size: %d, field_4: %d, field_8: %d, field_A: %d, field_C: %d)",
	       _tControlHeader.size, _tControlHeader.field_4, _tControlHeader.field_8, _tControlHeader.field_A, _tControlHeader.field_C);

	// Read data
	uint32 dataSize = 2 * _tControlHeader.field_C + 2;

	free(_compressedData);
	_compressedData = (byte *)malloc(dataSize + _tControlHeader.size);
	if (!_compressedData)
		error("[Cinematic1::tControl] Cannot allocate memory for control data");

	_stream->read(_compressedData, dataSize + _tControlHeader.size);

	// Process
	_compressedDataEnd   = _compressedData + dataSize;
	_field_46            = _tControlHeader.field_8;
	_compressedBuffer    = _compressedData + 2 * (_tControlHeader.field_8 * _tControlHeader.field_A) + dataSize;
	_compressedBufferEnd = _compressedData + 1 * (_tControlHeader.size    - _tControlHeader.field_4) + dataSize;

	// Decompress data
	uint32 decompressedSize = decompress(_compressedBuffer, _backBuffer, _compressedBufferEnd - _compressedBuffer);
	if (decompressedSize > CINEMATIC_BACKBUFFER_SIZE) {
		warning("[Cinematic1::tControl] Back buffer overrun");
		return false;
	}

	debugC(kRingDebugMovie, "    Decompressed %d bytes of data", decompressedSize);

	return true;
}

bool Cinematic1::sControl(byte* buffer) {
	if (!_tControlBuffer || !_backBuffer)
		error("[Cinematic1::readFrameHeader] Buffers not initialized properly");

	// Reset tControl buffer
	memset(_tControlBuffer, 0, CINEMATIC_TCONTROLBUFFER_SIZE * sizeof(TControl));

	// Read frame header
	FrameHeader header;
	header.read(_stream);

	debugC(kRingDebugMovie, "    Reading Frame header (size: %d, field_4: %d, field_8: %d, field_A: %d, width: %d, height: %d)",
	       header.size, header.field_4, header.field_8, header.field_A, header.width, header.height);

	// FIXME This is broken (crashes scummvm), so disable for now
	seek(header.size, SEEK_CUR);
	return true;
	
	// Read stream
	uint32 dataSize = 2 * _tControlHeader.field_C + 2;

	free(_compressedData);
	_compressedData = (byte *)malloc(dataSize + header.size);
	if (!_compressedData)
		error("[Cinematic1::sControl] Cannot allocate memory for control data");

	_stream->read(_compressedData, dataSize + header.size);

	// Process
	_compressedDataEnd   = _compressedData + dataSize;
	_field_46            = header.field_8;
	_compressedBuffer    = _compressedData + 2 * (header.field_8 * header.field_A) + dataSize;
	_compressedBufferEnd = _compressedData + 1 * (header.size    - header.field_4) + dataSize;

	// Update control buffer
	if (!_tControlBuffer[1920].count) {
		byte *compressedData = _compressedData;
		byte *backBuffer     = _backBuffer;

		if (compressedData < _compressedDataEnd) {
			int index = 1920;
			do {
				uint32 offset = *compressedData * 8;

				_tControlBuffer[index].pBuffer = (int *)backBuffer;
				_tControlBuffer[index].count   = offset;

				compressedData += 2;
				backBuffer += offset;
				++index;
			} while (compressedData < _compressedDataEnd);
		}
	}

	// Decompress data
	uint32 decompressedSize = decompress(_compressedBuffer, buffer, _compressedBufferEnd - _compressedBuffer);
	if (decompressedSize >= 577536) {
		warning("[Cinematic1::sControl] Buffer overrun");
		return false;
	}

	debugC(kRingDebugMovie, "    Decompressed %d bytes of data", decompressedSize);

	return true;
}

uint32 Cinematic1::decompress(byte *data, byte *output, uint32 dataSize) {
	if (!_cacheBuffer || !_compressedDataEnd || !_compressedBufferEnd || !_tControlBuffer)
		error("[Cinematic1::decompress] Buffers not initialized properly");

	// TODO: Reduce code duplication

#define UPDATE_BUFFER_CONTROL(index) updateBufferControl(index, &buffer);
#define UPDATE_BUFFER(index) updateBuffer(index, compressedDataEnd, &buffer);

	// Get start and end of buffer
	byte *start = data;
	byte *end   = &data[dataSize];

	// Store buffers position
	int *buffer            = (int *)output;
	int *bufferStart       = buffer;
	int *cacheBuffer       = (int *)_cacheBuffer;
	int *cacheBufferStart  = (int *)_cacheBuffer;
	int *compressedDataEnd = (int *)_compressedDataEnd;

	bool check = false;

	while (end > start) {

		// Get id value
		byte val = *start++;

		if (check) {

			// Get state
			byte state = val & 0xF;

			if (state < 8) {

				*cacheBuffer = *start++ + (state << 8);

				if (*cacheBuffer > _field_46) {
					if (*cacheBuffer >= 1920) {
						UPDATE_BUFFER_CONTROL(2 * *cacheBuffer);
					} else {

						uint32 total = *cacheBuffer - (_field_46 + 1);
						byte *offset;
						for (offset = _compressedBufferEnd; ; offset += *offset + 1) {
							if (total-- < 1)
								break;
						}
						total = *offset >> 1;

						// Setup control buffer
						_tControlBuffer[2 * *cacheBuffer].pBuffer = buffer;
						_tControlBuffer[2 * *cacheBuffer].count   = 8 * total;

						int16 *index2 = (int16 *)(offset + 1);
						while (true) {
							if (total-- < 1)
								break;

							UPDATE_BUFFER(2 * *index2);

							index2++;
						}

					}
				} else {
					if (_tControlBuffer[*cacheBuffer].count) {
						UPDATE_BUFFER_CONTROL(2 * *cacheBuffer);
					} else {
						UPDATE_BUFFER(2 * *cacheBuffer);

						// Setup control buffer
						_tControlBuffer[2 * *cacheBuffer].pBuffer = &compressedDataEnd[*cacheBuffer];
						_tControlBuffer[2 * *cacheBuffer].count   = 8;
					}
				}

				// Advance cache (and loop if necessary)
				++cacheBuffer;

				if ((cacheBuffer - cacheBufferStart) >= 128)
					cacheBuffer = cacheBufferStart;

				check = false;

			} else {
				UPDATE_BUFFER_CONTROL(2 * cacheBufferStart[(*start >> 4) + 16 * state - 128]);
			}

		} else {
			if (val < 128) {

				*cacheBuffer = 16 * val + (*start >> 4);

				if (*cacheBuffer > _field_46) {
					if (*cacheBuffer >= 1920) {
						UPDATE_BUFFER_CONTROL(2 * *cacheBuffer);
					} else {

						uint32 total = *cacheBuffer - _field_46 - 1;
						byte *offset;
						for (offset = _compressedBufferEnd; ; offset += *offset + 1) {
							if (total-- < 1)
								break;
						}
						total = *offset >> 1;

						// Setup control buffer
						_tControlBuffer[*cacheBuffer].pBuffer = buffer;
						_tControlBuffer[*cacheBuffer].count   = 8 * total;

						int16 *index2 = (int16 *)(offset + 1);
						while (true) {
							if (total-- < 1)
								break;

							UPDATE_BUFFER(2 * *index2);

							index2++;
						}
					}
				} else {
					if (_tControlBuffer[*cacheBuffer].count) {
						UPDATE_BUFFER_CONTROL(2 * *cacheBuffer);
					} else {
						UPDATE_BUFFER(2 * *cacheBuffer);

						// Setup control buffer
						_tControlBuffer[2 * *cacheBuffer].pBuffer = &compressedDataEnd[*cacheBuffer];
						_tControlBuffer[2 * *cacheBuffer].count = 8;
					}
				}

				// Advance cache (and loop if necessary)
				++cacheBuffer;

				if ((cacheBuffer - cacheBufferStart) >= 128)
					cacheBuffer = cacheBufferStart;

				check = true;

			} else {
				UPDATE_BUFFER_CONTROL(2 * cacheBufferStart[val - 128]);
			}
		}
	}

	return (uint32)(buffer - bufferStart);
}

void Cinematic1::updateBuffer(int index, int *compressedDataEnd, int **buffer) {
	int *control = &compressedDataEnd[index];
	uint32 count = 2;

	do {
		**buffer = *control;
		++control;
		++*buffer;
		--count;
	} while (count);
}

void Cinematic1::updateBufferControl(int index, int **buffer) {
	int    *pBuffer = _tControlBuffer[index].pBuffer;
	uint32  count   = _tControlBuffer[index].count >> 2;

	if (count == 0)
		return;

	if (pBuffer == nullptr)
		error("[Cinematic1::updateBufferControl] Invalid control buffer");

	do {
		**buffer = *pBuffer;
		++pBuffer;
		++*buffer;
		--count;
	} while (count);
}

#pragma endregion

#pragma region Cinematic2

Cinematic2::Cinematic2() {
	_stream = nullptr;
	_seqBuffer = nullptr;

	_state = false;

	_frameBuffer = nullptr;
	_tControlBuffer = nullptr;

	memset(_buffer1, 0, sizeof(_buffer1));
}

Cinematic2::~Cinematic2() {
	SAFE_DELETE(_stream);

	free(_seqBuffer);
	_seqBuffer = nullptr;

	free(_frameBuffer);
	_frameBuffer = 0;

	free(_tControlBuffer);
	_tControlBuffer = 0;
}

bool Cinematic2::init(Common::String filename, ArchiveType type, ZoneId zone, LoadFrom loadFrom) {
	// Initialize stream
	switch (type) {
	default:
		warning("[Cinematic2::init] Invalid archive type (%d)!", type);
		return false;

	case kArchiveFile:
		_stream = SearchMan.createReadStreamForMember(filename);
		break;

	case kArchiveArt:
		_stream = getApp()->getArtHandler()->get(filename, zone, loadFrom);
		break;
	}

	if (_stream == nullptr)
		return false;

	_state = false;
	_seqBuffer = nullptr;

	return true;
}

bool Cinematic2::allocBuffer(size_t bufferSize) {
	_seqBuffer = malloc(bufferSize);

	return _seqBuffer != nullptr;
}

void Cinematic2::skipFrame() {
	// Read frame header
	if (!_frameHeader.read(_stream))
		error("[Cinematic2::skipFrame] Cannot read frame header");

	debugC(kRingDebugMovie, "    Reading Frame header (size: %d, offset: %d, decompressedSize: %d, bufferSize: %d, width: %d, height: %d)",
	       _frameHeader.size, _frameHeader.offset, _frameHeader.decompressedSize, _frameHeader.bufferSize, _frameHeader.width, _frameHeader.height);
	debugC(kRingDebugMovie, "    Skipping frame data (size: %d)", _frameHeader.size);

	// Skip frame
	seek(_frameHeader.size, SEEK_CUR);
}

bool Cinematic2::tControl() {
	if (!_stream)
		error("[Cinematic2::tControl] Stream not initialized");

	// Read tControl header
	if (!_tControlHeader.read(_stream)) {
		error("[Cinematic2::tControl] Cannot read tControl header");
		return false;
	}

	// Reset buffer
	if (_tControlBuffer)
		free(_tControlBuffer);

	_tControlBuffer = (byte *)malloc(_tControlHeader.size);
	if (!_tControlBuffer) {
		warning("[Cinematic2::tControl] Cannot allocate tControl buffer");
		return false;
	}

	// Read buffer
	_stream->read(_tControlBuffer, _tControlHeader.size);
	if (_stream->eos() || _stream->err()) {
		warning("[Cinematic2::tControl] Cannot read tControl data");
		return false;
	}

	// Decompress
	decompressTControl(_tControlBuffer, 4 * _tControlHeader.bufferSize, _tControlHeader.decompressedSize);

	return true;
}

bool Cinematic2::sControl(byte* buffer, uint32 bitdepth) {
	if (!_stream)
		error("[Cinematic2::sControl] Stream not initialized");

	// Read frame header
	if (!_frameHeader.read(_stream)) {
		warning("[Cinematic2::sControl] Cannot read frame header");
		return false;
	}

	debugC(kRingDebugMovie, "    Reading Frame header (size: %d, offset: %d, decompressedSize: %d, bufferSize: %d, width: %d, height: %d)",
	       _frameHeader.size, _frameHeader.offset, _frameHeader.decompressedSize, _frameHeader.bufferSize, _frameHeader.width, _frameHeader.height);

	// FIXME This is broken (crashes scummvm), so disable for now
	seek(_frameHeader.size, SEEK_CUR);
	return true;

	// Reset buffer
	if (_frameBuffer)
		free(_frameBuffer);

	_frameBuffer = (byte *)malloc(_frameHeader.size);
	if (!_frameBuffer) {
		warning("[Cinematic2::sControl] Cannot allocate frame buffer");
		return false;
	}

	// Read buffer
	_stream->read(_frameBuffer, _frameHeader.size);
	if (_stream->eos() || _stream->err()) {
		warning("[Cinematic2::sControl] Cannot read frame data");
		return false;
	}

	if (_frameHeader.offset < _frameHeader.size)
		decompressTControl(_frameBuffer + _frameHeader.offset, 4 * _frameHeader.bufferSize, _frameHeader.decompressedSize);

	if (bitdepth == 32)
		decompressSeq(buffer);

	return true;
}

void Cinematic2::decompressTControl(byte *buffer, uint32 bufferSize, uint16 decompressedSize) {

	memcpy(_buffer1, buffer, bufferSize);

	if (bufferSize == 16) {
		int *pBuffer = (int *)&_buffer1[16];

		// Iterate over buffer
		for (uint32 i = 0; i < decompressedSize; i++) {
			for (uint32 j = 0; j < 9; j++) {


				//warning("[Cinematic2::decompressTControl] Not implemented");

				++pBuffer;
			}

			pBuffer += 12;
		}

	} else {
		warning("[Cinematic2::decompressTControl] Not implemented");
	}

	if (_state) {
		for (uint32 i = 0; i < (uint32)decompressedSize * 4; i += 4) {
			_buffer1[i]     = _buffer1[i]     * 2;
			_buffer1[i + 1] = _buffer1[i + 1] * 2;
			_buffer1[i + 2] = _buffer1[i + 2] * 2;
			_buffer1[i + 3] = _buffer1[i + 3] * 2;
		}
	}


}

void Cinematic2::decompressSeq(byte *buffer) {
	warning("[Cinematic2::decompressSeq] Not implemented");
}

#pragma endregion

#pragma region CinematicSound

CinematicSound::CinematicSound() {
	_channels       = 0;
	_samplesPerSec  = 0;
	_avgBytesPerSec = 0;
	_blockAlign     = 0;
	_bitsPerSample  = 0;
	_audioStream    = nullptr;
	_isPlaying      = false;
	_volume         = 1.0f;
}

CinematicSound::~CinematicSound() {
	deinit();
}

void CinematicSound::init(uint32 channels, uint32 bitsPerSample, uint32 samplesPerSec) {
	deinit();

	_channels       = channels;
	_bitsPerSample  = bitsPerSample;
	_blockAlign     = bitsPerSample * channels / 8;
	_samplesPerSec  = samplesPerSec;
	_avgBytesPerSec = samplesPerSec * _blockAlign;

	_isPlaying      = false;
	_volume         = 1.0f;

	debugC(kRingDebugMovie, "Setting up movie sound (channels: %d, bitsPerSample: %d, blockAlign: %d, samplesPerSec: %d, avgBytesPerSec: %d)",
	       _channels, _bitsPerSample, _blockAlign, _samplesPerSec, _avgBytesPerSec);

	// Create an audio stream where the decoded chunks will be appended
	_audioStream = Audio::makeQueuingAudioStream(22050 /*samplesPerSec * 1000*/, channels == 1 ? false : true);
}

void CinematicSound::deinit() {
	if (!_audioStream)
		return;

	if (_isPlaying) {
		_isPlaying = false;

		// Stop sound
		getSound()->getMixer()->stopHandle(_handle);
	}

	// Close the audio stream
	_audioStream = nullptr;
}

void CinematicSound::play() {
	if (_isPlaying)
		return;

	if (!_audioStream)
		error("[CinematicSound::play] Audiostream not initialized properly");

	// Get sound volume
	int32 vol = (int32)(_volume * -10000.0f);
	SoundEntry::convertVolumeFrom(vol);

	// Play sound
	getSound()->getMixer()->setChannelVolume(_handle, (byte)vol);
	getSound()->getMixer()->playStream(Audio::Mixer::kPlainSoundType, &_handle, _audioStream);

	_isPlaying = true;
}

void CinematicSound::setVolume(int32 volume) {
	_volume = volume * 0.01f;

	if (_volume < 0.0f)
		_volume = 0.0f;

	if (_volume > 1.0f)
		_volume = 1.0f;

	int32 vol = (int32)(_volume * -10000.0f);
	SoundEntry::convertVolumeFrom(vol);

	getSound()->getMixer()->setChannelVolume(_handle, (byte)vol);
}

void CinematicSound::queueBuffer(Common::SeekableReadStream *stream) {
	if (!_audioStream)
		error("[CinematicSound::play] Audiostream not initialized properly");

	Audio::RewindableAudioStream *adpcm = Audio::makeADPCMStream(stream, DisposeAfterUse::YES, stream->size(), Audio::kADPCMMS, 22050/*_samplesPerSec * 1000*/, _channels, _blockAlign);
	if (!adpcm) {
		warning("[CinematicSound::queueBuffer] Cannot decode sound stream");
		return;
	}

	// TODO Queue the stream
	//_audioStream->queueAudioStream(adpcm);
}

#pragma endregion

#pragma region Movie

Movie::Movie(ScreenManager *screen) : _screen(screen) {
	_image     = nullptr;
	_isSoundInitialized = false;
	_field_5B  = false;
	_framerate = 0.0f;
	_hasDialog = false;
	_channel   = 0;
	_isCI2     = false;

	_sound = new CinematicSound();
}

Movie::~Movie() {
	SAFE_DELETE(_image);
	SAFE_DELETE(_sound);

	// Zero-out passed pointers
	_screen = nullptr;
}

bool Movie::init(const Common::String &path, Common::String filename, const Common::String &languageFolder, uint32 channel) {
	debugC(kRingDebugMovie, "Loading movie %s%s with channel %d", path.c_str(), filename.c_str(), channel);

	// Find the proper path to the movie file
	Common::String filePath = Common::String::format("%s%s%s", path.c_str(), filename.c_str(), ".cnm");
	if (Common::File::exists(filePath))
		goto load_movie;

	// Try with language subfolder
	filePath = Common::String::format("%s%s/%s%s", path.c_str(), languageFolder.c_str(), filename.c_str(), ".cnm");
	if (Common::File::exists(filePath))
		goto load_movie;

	// Try with .ci2 suffix
	filePath = Common::String::format("%s%s%s", path.c_str(), filename.c_str(), ".ci2");
	if (Common::File::exists(filePath))
		goto load_movie;

	// Try with .ci2 suffix and language subfolder
	filePath = Common::String::format("%s%s/%s%s", path.c_str(), languageFolder.c_str(), filename.c_str(), ".ci2");
	if (Common::File::exists(filePath))
		goto load_movie;

	// Cannot find movie
	warning("[Movie::init] Cannot find movie file: %s", filename.c_str());
	return false;

load_movie:
	// Initialize movie stream
	if (filePath.hasSuffix("cnm")) {
		_image = new ImageLoaderCIN();
	} else {
		_image = new ImageLoaderCI2();
		_isCI2 = true;
	}

	if (!_image->init(filePath, kArchiveFile, kZoneNone, kLoadFromCd))
		error("[Movie::init] Cannot read cinematic frame size");

	// Set channel and sound state
	_channel = channel;
	_isSoundInitialized = true;

	uint32 index = (channel < 1 || channel > 3) ? 0 : channel - 1;

	// Setup sound
	_sound->init(_image->getChannels(index) + 1, _image->getBitsPerSample(index), _image->getSamplesPerSec(index));
	_sound->setVolume(getApp()->getPreferenceHandler()->getVolume());

	// Setup framerate
	_field_5B = true;
	_framerate = 1000.0f / (_image->getFrameRate() * 0.01f);

	// Setup sound handler
	SoundHandler *soundHandler = getApp()->getSoundHandler();
	if (soundHandler->getField0()) {
		soundHandler->turnOffItems1();

		if (!soundHandler->updateItems(_image->getChunkCount())) {
			soundHandler->turnOffSounds1(true);
			soundHandler->setField0(false);
		}
	}

	// Init dialog
	Common::String dialogPath = Common::String::format("DATA/%s/DIA/%s/%s", getApp()->getCurrentZoneFolder().c_str(), getApp()->getLanguageFolder().c_str(), filename.c_str());
	if (!Common::File::exists(dialogPath)) {
		_hasDialog = false;
		return true;
	}

	getApp()->getDialogHandler()->addDialog(new Dialog(500001, filename));
	_hasDialog = true;

	return true;
}

void Movie::deinit() {
	SAFE_DELETE(_image);

	_screen = nullptr;
	_field_5B = true;
}

void Movie::play(const Common::Point &point) {
	debugC(kRingDebugMovie, "Playing movie at coordinates (%d, %d)", point.x, point.y);

	if (!_sound)
		error("[Movie::play] sound not initialized properly");

	if (!_image)
		error("[Movie::play] image not initialized properly");

	SoundHandler *soundHandler = getApp()->getSoundHandler();
	ScreenManager *screen = getApp()->getScreenManager();

	// Setup
	Cinematic *cinematic = _image->getCinematic();
	ImageSurface *image = new ImageSurface();
	bool setupSound = true;
	bool readFrame = false;
	uint32 chunkIndex = 0;
	uint32 waitChunk = 0;
	uint32 ticks = g_system->getMillis();

	// Setup header and state
	cinematic->setState(false);
	uint32 chunkCount = _image->getChunkCount();

	// Parse cinematic
	if (chunkCount) {
		while (!cinematic->eos() && !cinematic->err()) {

			// Interrupt playing on escape
			if (checkEscape()) {
				if (soundHandler->getField0()) {
					soundHandler->updateItems2(chunkCount);
					soundHandler->updateItems3(chunkCount);
				}
				break;
			}

			// Read chunk type
			ChunkType chunkType = (ChunkType)cinematic->readByte();
			if (cinematic->eos() || cinematic->err())
				error("[Movie::play] Cannot read chunk type");

			debugC(kRingDebugMovie, " Reading chunk %c", chunkType);

			switch (chunkType) {
			default:
				error("[Movie::play] Invalid chunk type %d (index: %d)", chunkType, chunkIndex);

			case kChunkA:
				switch (_channel) {
				default:
					if (!skipSound())
						error("[Movie::play] Chunk A: Cannot skip sound (index: %d)", chunkIndex);
					break;

				case 2:
					if (!readSound())
						error("[Movie::play] Chunk A: Cannot read sound (index: %d)", chunkIndex);

					if (setupSound) {
						if (_isSoundInitialized)
							_sound->play();

						setupSound = false;
					}
					break;
				}
				break;

			case kChunkB:
				switch (_channel) {
				default:
					if (!skipSound())
						error("[Movie::play] Chunk B: Cannot skip sound (index: %d)", chunkIndex);
					break;

				case 3:
					if (!readSound())
						error("[Movie::play] Chunk B: Cannot read sound (index: %d)", chunkIndex);

					if (setupSound) {
						if (_isSoundInitialized)
							_sound->play();

						setupSound = false;
					}
					break;
				}
				break;

			case kChunkS:
			case kChunkU:    // CI2 movies
				if (_field_5B) {
					uint32 tickInterval = (g_system->getMillis() - ticks);

					if (((chunkIndex + 1) * _framerate) < tickInterval) {
						if (readFrame) {
							if (chunkType == kChunkS)
								cinematic->skipFrame();

							// Process sound
							if (soundHandler->getField0()) {
								soundHandler->updateItems2(chunkCount);
								soundHandler->updateItems3(chunkCount);
							}

							++chunkIndex;
							waitChunk = chunkIndex;
							break;
						}

						readFrame = true;
						ticks = g_system->getMillis();

					} else {
						if (!readFrame) {
							readFrame = true;
							ticks = g_system->getMillis();
						} else {
							// Wait for tick interval
							while ((waitChunk * _framerate) > (tickInterval + 50)) {
								// If escape was pressed, bail out of the movie playing
								if (checkEvents())
									goto cleanup;

								tickInterval = (g_system->getMillis() - ticks);
							}
						}
					}
				}

				if (!_image->readImage(image, 32, kDrawTypeNormal))
					error("[Movie::play] Chunk S: Error reading image (index: %d)", chunkIndex);

				// Draw frame
				screen->draw(image, point, kDrawTypeNormal);

				if (_hasDialog)
					getApp()->getDialogHandler()->play();

				// For CI2 movies, we need to update the menu
				if (_isCI2) {
					Puzzle *menu = getApp()->getPuzzle(kPuzzleMenu);
					if (menu) {
						menu->alloc();
						menu->update();
					}
				}

				screen->updateScreen();
				g_system->updateScreen();

				// Process sound
				if (soundHandler->getField0()) {
					soundHandler->updateItems2(chunkCount);
					soundHandler->updateItems3(chunkCount);
				}

				++chunkIndex;
				waitChunk = chunkIndex;
				break;

			case kChunkT:
				if (!cinematic->tControl())
					error("[Movie::play] Chunk T: Error reading T control (index: %d)", chunkIndex);
				break;

			case kChunkZ:
				switch (_channel) {
				default:
					if (!skipSound())
						error("[Movie::play] Chunk Z: Cannot skip sound (index: %d)", chunkIndex);
					break;

				case 0:
				case 1:
					if (!readSound())
						error("[Movie::play] Chunk Z: Cannot read sound (index: %d)", chunkIndex);

					if (setupSound) {
						if (_isSoundInitialized)
							_sound->play();

						setupSound = false;
					}
					break;
				}
				break;
			}

			// Stop after processing all chunks
			if (chunkIndex >= chunkCount)
				break;
		}
	}

	// Cleanup
cleanup:
	delete image;

	if (_isSoundInitialized)
		_sound->deinit();

	if (_hasDialog)
		getApp()->getDialogHandler()->removeDialog(500001);
}

#pragma region Sound

bool Movie::readSound() {
	if (!_image)
		error("[Movie::readSound] Image not initialized properly");

	Cinematic *cinematic = _image->getCinematic();
	if (!cinematic)
		error("[Movie::readSound] Cinematic not initialized properly");

	// Read sound data size
	uint32 soundSize = cinematic->readUint32LE();
	if (cinematic->err() || cinematic->eos()) {
		warning("[Movie::readSound] Error reading from file");
		deinit();
		return false;
	}

	debugC(kRingDebugMovie, "    Reading sound data (size: %d)", soundSize);

	// Check if there is any sound data
	if (!soundSize)
		return true;

	if (soundSize > 10000000) {
		warning("[Movie::readSound] Invalid sound data size (was: %d, max: 10000000)", soundSize);
		return false;
	}

	// Check remaining file size
	if (((uint32)cinematic->pos() + soundSize) >= (uint32)cinematic->size()) {
		warning("[Movie::readSound] Invalid sound data size (would read after end of file: %d)", soundSize);
		deinit();
		return false;
	}

	if (!_isSoundInitialized)
		return true;

	// Read sound data
	byte *soundBuffer = (byte *)calloc(soundSize, 1);
	if (!soundBuffer)
		error("[Movie::readSound] Cannot allocate sound buffer");

	cinematic->read(soundBuffer, soundSize);

	_sound->queueBuffer(new Common::MemoryReadStream(soundBuffer, soundSize, DisposeAfterUse::YES));

	return true;
}

bool Movie::skipSound() {
	Common::SeekableReadStream *cinematic = _image->getCinematic();
	if (!cinematic)
		error("[Movie::skipSound] Cinematic not initialized properly");

	// Read sound data offset
	uint32 offset = cinematic->readUint32LE();
	if (cinematic->err() || cinematic->eos()) {
		warning("[Movie::skipSound] Error reading from file");
		deinit();
		return false;
	}

	debugC(kRingDebugMovie, "    Skipping sound data (size: %d)", offset);

	// Check if there is any sound data
	if (!offset)
		return true;

	// Skip sound data
	cinematic->seek(offset, SEEK_CUR);
	if (cinematic->err() || cinematic->eos()) {
		warning("[Movie::skipSound] Error reading from file");
		deinit();
		return false;
	}

	return true;
}

#pragma endregion

#pragma region Frames

uint32 Movie::playNextFrame(const Common::Point &point, DrawType drawType) {
	error("[Movie::playNextFrame] Not implemented!");
}

uint32 Movie::getNumberOfFrames() {
	error("[Movie::getNumberOfFrames] Not implemented!");
}

void Movie::setSynchroOff() {
	error("[Movie::setSynchroOff] Not implemented!");
}

#pragma endregion

#pragma endregion


} // End of namespace Ring
