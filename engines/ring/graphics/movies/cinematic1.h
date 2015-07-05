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

#ifndef RING_CINEMATIC1_H
#define RING_CINEMATIC1_H

#include "ring/graphics/movies/cinematic.h"

#include "ring/shared.h"

#include "common/stream.h"

namespace Ring {

class Cinematic1 : public Cinematic {
public:
	struct TControl {
		int    *pBuffer;
		uint32  count;
	};

	Cinematic1();
	~Cinematic1();

	bool init(Common::String name);
	void deinit();

	void skipFrame() override;
	bool tControl() override;
	bool sControl(byte* buffer);

	// Accessors
	void setState(bool state) override { _state = state; }

private:
	struct FrameHeader {
		uint32 size;
		uint32 field_4;
		uint16 field_8;
		uint16 field_A;
		uint32 width;
		uint32 height;

		FrameHeader() {
			size     = 0;
			field_4  = 0;
			field_8  = 0;
			field_A  = 0;
			width  = 0;
			height = 0;
		}

		void read(Common::SeekableReadStream *stream) {
			size     = stream->readUint32LE();
			field_4  = stream->readUint32LE();
			field_8  = stream->readUint16LE();
			field_A  = stream->readUint16LE();
			width  = stream->readUint32LE();
			height = stream->readUint32LE();
		}
	};

	struct TControlHeader {
		uint32 size;
		uint32 field_4;
		uint16 field_8;
		uint16 field_A;
		byte   field_C;

		TControlHeader() {
			size = 0;
			field_4 = 0;
			field_8 = 0;
			field_A = 0;
			field_C = 0;
		}

		void load(Common::SeekableReadStream *stream) {
			size = stream->readUint32LE();
			field_4 = stream->readUint32LE();
			field_8 = stream->readUint16LE();
			field_A = stream->readUint16LE();
			field_C = stream->readByte();
		}
	};

	byte   *_buffer;
	byte   *_buffer2;
	bool    _state;
	byte   *_backBuffer;
	TControlHeader _tControlHeader;
	TControl      *_tControlBuffer;
	byte   *_cacheBuffer;
	byte   *_compressedData;
	byte   *_compressedDataEnd;
	byte   *_compressedBuffer;
	byte   *_compressedBufferEnd;
	int32   _field_46;
	bool    _isStreaming;

	uint32 decompress(byte *data, byte *buffer, uint32 size);
	void updateBuffer(const int index, int **buffer);
	void updateBufferControl(const int index, int **buffer);
};

} // End of namespace Ring

#endif // RING_CINEMATIC1_H
