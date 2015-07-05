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

#include "ring/graphics/movies/cinematic2.h"

#include "ring/base/application.h"
#include "ring/base/art.h"

#include "ring/ring.h"
#include "ring/helpers.h"

namespace Ring {

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
	_frameBuffer = nullptr;

	free(_tControlBuffer);
	_tControlBuffer = nullptr;
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

} // End of namespace Ring
