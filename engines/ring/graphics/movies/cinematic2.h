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

#ifndef RING_CINEMATIC2_H
#define RING_CINEMATIC2_H

#include "ring/graphics/movies/cinematic.h"

#include "ring/shared.h"

#include "common/stream.h"

namespace Ring {

class Cinematic2 : public Cinematic {
public:
	Cinematic2();
	~Cinematic2();

	bool init(Common::String filename, ArchiveType type, ZoneId zone, LoadFrom loadFrom);

	bool allocBuffer(size_t bufferSize);

	void skipFrame() override;
	bool tControl() override;
	bool sControl(byte* buffer, uint32 bitdepth);

	void setState(bool state) override { _state = state; }

private:
	struct TControlHeader {
		uint32 size;
		uint16 decompressedSize;
		uint16 bufferSize;

		TControlHeader() {
			size             = 0;
			decompressedSize = 0;
			bufferSize       = 0;
		}

		bool read(Common::SeekableReadStream *stream) {
			size             = stream->readUint32LE();
			decompressedSize = stream->readUint16LE();
			bufferSize       = stream->readUint16LE();

			return (!stream->eos() && !stream->err());
		}
	};

	struct FrameHeader {
		uint32 size;
		uint32 offset;
		uint16 decompressedSize;
		uint16 bufferSize;
		uint32 width;
		uint32 height;
		uint32 field_14;
		uint32 field_18;
		uint32 field_1C;
		uint32 field_20;
		uint32 field_24;
		uint32 field_28;
		uint16 field_2C;
		byte   field_2E;

		FrameHeader() {
			size             = 0;
			offset           = 0;
			decompressedSize = 0;
			bufferSize       = 0;
			width          = 0;
			height         = 0;
			field_14         = 0;
			field_18         = 0;
			field_1C         = 0;
			field_20         = 0;
			field_24         = 0;
			field_28         = 0;
			field_2C         = 0;
			field_2E         = 0;
		}

		bool read(Common::SeekableReadStream *stream) {
			size             = stream->readUint32LE();
			offset           = stream->readUint32LE();
			decompressedSize = stream->readUint16LE();
			bufferSize       = stream->readUint16LE();
			width          = stream->readUint32LE();
			height         = stream->readUint32LE();
			field_14         = stream->readUint32LE();
			field_18         = stream->readUint32LE();
			field_1C         = stream->readUint32LE();
			field_20         = stream->readUint32LE();
			field_24         = stream->readUint32LE();
			field_28         = stream->readUint32LE();
			field_2C         = stream->readUint16LE();
			field_2E         = stream->readByte();

			return (!stream->eos() && !stream->err());
		}
	};

	byte            _buffer1[65536];
	void           *_seqBuffer;
	bool            _state;
	byte           *_frameBuffer;
	byte           *_tControlBuffer;
	FrameHeader     _frameHeader;
	TControlHeader  _tControlHeader;
	// Original stores a flag to know if the data is streamed or not (movie: true - images: false)

	void decompressTControl(byte *buffer, uint32 bufferSize, uint16 decompressedSize);
	void decompressSeq(byte *buffer);
};

} // End of namespace Ring

#endif // RING_CINEMATIC2_H
