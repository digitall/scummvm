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

#define CINEMATIC_BUFFER_SIZE          1024
#define CINEMATIC_BACKBUFFER_SIZE      1200000
#define CINEMATIC_TCONTROLBUFFER_SIZE  8192
#define CINEMATIC_CACHEBUFFER_SIZE     512

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

} // End of namespace Ring
