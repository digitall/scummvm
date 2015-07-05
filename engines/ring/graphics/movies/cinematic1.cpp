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

#include "ring/graphics/movies/cinematic1.h"

#include "ring/base/art.h"

#include "ring/ring.h"
#include "ring/helpers.h"

namespace Ring {

#define CINEMATIC_FILEBUFFER_SIZE      1130048
#define CINEMATIC_BUFFER_SIZE          1024
#define CINEMATIC_BACKBUFFER_SIZE      1200000
#define CINEMATIC_TCONTROLBUFFER_SIZE  0x4000
#define CINEMATIC_CACHEBUFFER_SIZE     512
#define CINEMATIC_MAX_BUFFER_SIZE      577536

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

	_backBuffer = static_cast<byte *>(calloc(CINEMATIC_BACKBUFFER_SIZE, 1));
	if (!_backBuffer)
		error("[Cinematic1::init] Error creating back buffer!");

	_tControlBuffer = static_cast<TControl *>(calloc(CINEMATIC_TCONTROLBUFFER_SIZE, sizeof(TControl)));
	if (!_tControlBuffer)
		error("[Cinematic1::init] Error creating control buffer!");

	_cacheBuffer = static_cast<byte *>(calloc(CINEMATIC_CACHEBUFFER_SIZE, 1));
	if (!_cacheBuffer)
		error("[Cinematic1::init] Error creating cache buffer!");

	_compressedData = static_cast<byte *>(calloc(CINEMATIC_FILEBUFFER_SIZE, 1));
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
	_stream->read(_compressedData, dataSize + _tControlHeader.size);

	// Process
	_compressedDataEnd   = &_compressedData[dataSize];
	_field_46            = _tControlHeader.field_8;
	_compressedBuffer    = &_compressedData[2 * (_tControlHeader.field_8 * _tControlHeader.field_A) + dataSize];
	_compressedBufferEnd = &_compressedData[1 * (_tControlHeader.size    - _tControlHeader.field_4) + dataSize];

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
	memset(_tControlBuffer, 0, CINEMATIC_TCONTROLBUFFER_SIZE);

	// Read frame header
	FrameHeader header;
	header.read(_stream);

	debugC(kRingDebugMovie, "    Reading Frame header (size: %d, field_4: %d, field_8: %d, field_A: %d, width: %d, height: %d)",
	       header.size, header.field_4, header.field_8, header.field_A, header.width, header.height);
	
	// Read stream
	uint32 dataSize = 2 * _tControlHeader.field_C + 2;
	_stream->read(&_compressedData[dataSize], header.size);

	// Process
	_compressedDataEnd   = &_compressedData[dataSize];
	_field_46            = header.field_8;
	_compressedBuffer    = &_compressedData[2 * (header.field_8 * header.field_A) + dataSize];
	_compressedBufferEnd = &_compressedData[header.size + dataSize - header.field_4];

	// Update control buffer
	if (!_tControlBuffer[1920].count) {
		byte *compressedData = _compressedData;
		byte *backBuffer     = _backBuffer;

		if (compressedData < _compressedDataEnd) {
			int index = 1920;
			do {
				uint32 offset = 8 * *reinterpret_cast<int16 *>(compressedData);

				_tControlBuffer[index].pBuffer = reinterpret_cast<int *>(backBuffer);
				_tControlBuffer[index].count   = offset;

				compressedData += 2;
				backBuffer += offset;
				++index;
			} while (compressedData < _compressedDataEnd);
		}
	}

	// Decompress data
	uint32 decompressedSize = decompress(_compressedBuffer, buffer, _compressedBufferEnd - _compressedBuffer);
	if (decompressedSize >= CINEMATIC_MAX_BUFFER_SIZE) {
		warning("[Cinematic1::sControl] Buffer overrun");
		return false;
	}

	debugC(kRingDebugMovie, "    Decompressed %d bytes of data", decompressedSize);

	return true;
}

uint32 Cinematic1::decompress(byte *data, byte *output, uint32 dataSize) {
	if (!_cacheBuffer || !_compressedDataEnd || !_compressedBufferEnd || !_tControlBuffer)
		error("[Cinematic1::decompress] Buffers not initialized properly");

	bool check = false;

	// Get start and end of buffer
	byte *start = data;
	byte *end   = &data[dataSize];

	// Store buffers position
	const int *cacheBufferStart = reinterpret_cast<int *>(_cacheBuffer);

	int *buffer = reinterpret_cast<int *>(output);
	int *cacheBuffer = reinterpret_cast<int *>(_cacheBuffer);

	while (end > start) {

		// Get id value
		byte val = *start++;

		if (check) {

			// Get state
			byte state = val & 0xF;

			if (state < 8) {

				int controlIndex = *start++ + (state << 8);
				*cacheBuffer = controlIndex;

				if (controlIndex > _field_46) {
					if (controlIndex >= 1920) {
						updateBufferControl(controlIndex, &buffer);
					} else {
						uint32 skip = controlIndex - (_field_46 + 1);
						byte *offset;
						for (offset = _compressedBufferEnd; ; offset += *offset + 1) {
							if (skip-- < 1)
								break;
						}

						// Setup control buffer
						uint32 total = static_cast<int>(*offset) >> 1;
						_tControlBuffer[controlIndex].pBuffer = buffer;
						_tControlBuffer[controlIndex].count   = 8 * total;

						uint16 *index2 = reinterpret_cast<uint16 *>(offset + 1);
						while (true) {
							if (total-- < 1)
								break;

							updateBuffer(*index2, &buffer);

							index2++;
						}

					}
				} else {
					if (_tControlBuffer[controlIndex].count) {
						updateBufferControl(controlIndex, &buffer);
					} else {
						updateBuffer(controlIndex, &buffer);

						// Setup control buffer
						_tControlBuffer[controlIndex].pBuffer = reinterpret_cast<int *>(&_compressedDataEnd[8 * controlIndex]);
						_tControlBuffer[controlIndex].count   = 8;
					}
				}

				// Advance cache (and loop if necessary)
				++cacheBuffer;

				if (cacheBuffer - cacheBufferStart >= 128)
					cacheBuffer = const_cast<int *>(cacheBufferStart);

				check = false;

			} else {
				updateBufferControl(cacheBufferStart[(*start >> 4) + 16 * state - 128], &buffer);
			}

		} else {
			if (val < 128) {

				int controlIndex = 16 * val + (*start >> 4);
				*cacheBuffer = controlIndex;

				if (controlIndex > _field_46) {
					if (controlIndex >= 1920) {
						updateBufferControl(controlIndex, &buffer);
					} else {

						uint32 skip = controlIndex - (_field_46 + 1);
						byte *offset;
						for (offset = _compressedBufferEnd; ; offset += *offset + 1) {
							if (skip-- < 1)
								break;
						}

						// Setup control buffer
						uint32 total = *offset >> 1;
						_tControlBuffer[controlIndex].pBuffer = buffer;
						_tControlBuffer[controlIndex].count   = 8 * total;

						uint16 *index2 = reinterpret_cast<uint16 *>(offset + 1);
						while (true) {
							if (total-- < 1)
								break;

							updateBuffer(*index2, &buffer);

							++index2;
						}
					}
				} else {
					if (_tControlBuffer[controlIndex].count) {
						updateBufferControl(controlIndex, &buffer);
					} else {
						updateBuffer(controlIndex, &buffer);

						// Setup control buffer
						_tControlBuffer[controlIndex].pBuffer = reinterpret_cast<int *>(&_compressedDataEnd[8 * controlIndex]);
						_tControlBuffer[controlIndex].count = 8;
					}
				}

				// Advance cache (and loop if necessary)
				++cacheBuffer;

				if (cacheBuffer - cacheBufferStart >= 128)
					cacheBuffer = const_cast<int *>(cacheBufferStart);

				check = true;

			} else {
				updateBufferControl(cacheBufferStart[val - 128], &buffer);
			}
		}
	}

	return reinterpret_cast<byte *>(buffer) - output;
}

void Cinematic1::updateBuffer(const int index, int **buffer) {
	int *control = reinterpret_cast<int *>(&_compressedDataEnd[8 * index]);
	uint32 count = 2;

	do {
		**buffer = *control;
		++control;
		++*buffer;
		--count;
	} while (count);
}

void Cinematic1::updateBufferControl(const int index, int **buffer) {
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

} // End of namespace Ring
