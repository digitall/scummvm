/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "video/flic_decoder.h"
#include "common/endian.h"
#include "common/rect.h"
#include "common/stream.h"
#include "common/system.h"
#include "common/textconsole.h"
#include "graphics/surface.h"

namespace Video {

FlicDecoder::FlicDecoder() {
}

FlicDecoder::~FlicDecoder() {
	close();
}

bool FlicDecoder::loadStream(Common::SeekableReadStream *stream) {
	close();

	/* uint32 frameSize = */ stream->readUint32LE();
	uint16 frameType = stream->readUint16LE();

	// Check FLC magic number
	if (frameType != 0xAF12) {
		warning("FlicDecoder::loadStream(): attempted to load non-FLC data (type = 0x%04X)", frameType);
		return false;
	}

	uint16 frameCount = stream->readUint16LE();
	uint16 width = stream->readUint16LE();
	uint16 height = stream->readUint16LE();
	uint16 colorDepth = stream->readUint16LE();
	if (colorDepth != 8) {
		warning("FlicDecoder::loadStream(): attempted to load an FLC with a palette of color depth %d. Only 8-bit color palettes are supported", colorDepth);
		return false;
	}

	addTrack(new FlicVideoTrack(stream, frameCount, width, height));
	return true;
}

const Common::List<Common::Rect> *FlicDecoder::getDirtyRects() const {
	const Track *track = getTrack(0);

	if (track)
		return ((const FlicVideoTrack *)track)->getDirtyRects();

	return 0;
}

void FlicDecoder::clearDirtyRects() {
	Track *track = getTrack(0);

	if (track)
		((FlicVideoTrack *)track)->clearDirtyRects();
}

void FlicDecoder::copyDirtyRectsToBuffer(uint8 *dst, uint pitch) {
	Track *track = getTrack(0);

	if (track)
		((FlicVideoTrack *)track)->copyDirtyRectsToBuffer(dst, pitch);
}

FlicDecoder::FlicVideoTrack::FlicVideoTrack(Common::SeekableReadStream *stream, uint16 frameCount, uint16 width, uint16 height, bool skipHeader) : _palette(256) {
	_fileStream = stream;
	_frameCount = frameCount;

	_surface = new Graphics::Surface();
	_surface->create(width, height, Graphics::PixelFormat::createFormatCLUT8());
	_dirtyPalette = false;

	_curFrame = -1;
	_nextFrameStartTime = 0;
	_atRingFrame = false;

	if (!skipHeader)
		readHeader();
}

FlicDecoder::FlicVideoTrack::~FlicVideoTrack() {
	delete _fileStream;

	_surface->free();
	delete _surface;
}

void FlicDecoder::FlicVideoTrack::readHeader() {
	_fileStream->readUint16LE();	// flags
	// Note: The normal delay is a 32-bit integer (dword), whereas the overridden delay is a 16-bit integer (word)
	// the frame delay is the FLIC "speed", in milliseconds.
	_frameDelay = _startFrameDelay = _fileStream->readUint32LE();

	_fileStream->seek(80);
	_offsetFrame1 = _fileStream->readUint32LE();
	_offsetFrame2 = _fileStream->readUint32LE();

	if (_offsetFrame1 == 0)
		_offsetFrame1 = 0x80; //length of FLIC header

	// Seek to the first frame
	_fileStream->seek(_offsetFrame1);
}

bool FlicDecoder::FlicVideoTrack::endOfTrack() const {
	return getCurFrame() >= getFrameCount() - 1;
}

bool FlicDecoder::FlicVideoTrack::rewind() {
	if (endOfTrack() && _fileStream->pos() < _fileStream->size() && _frameCount != 1)
		_atRingFrame = true;
	else
		_fileStream->seek(_offsetFrame1);

	_curFrame = -1;
	_nextFrameStartTime = 0;
	_frameDelay = _startFrameDelay;
	return true;
}

uint16 FlicDecoder::FlicVideoTrack::getWidth() const {
	return _surface->w;
}

uint16 FlicDecoder::FlicVideoTrack::getHeight() const {
	return _surface->h;
}

Graphics::PixelFormat FlicDecoder::FlicVideoTrack::getPixelFormat() const {
	return _surface->format;
}

#define FLI_SETPAL            4
#define FLI_SS2               7
#define FLI_BLACK             13
#define FLI_BRUN              15
#define FLI_COPY              16
#define PSTAMP                18
#define FRAME_TYPE            0xF1FA
#define FLC_FILE_HEADER       0xAF12
#define FLC_FILE_HEADER_SIZE  0x80

const Graphics::Surface *FlicDecoder::FlicVideoTrack::decodeNextFrame() {
	// Read chunk
	/*uint32 frameSize = */ _fileStream->readUint32LE();
	uint16 frameType = _fileStream->readUint16LE();

	switch (frameType) {
	case FRAME_TYPE:
		handleFrame();
		break;
	case FLC_FILE_HEADER:
		// Skip 0x80 bytes of file header subtracting 6 bytes of header
		_fileStream->skip(FLC_FILE_HEADER_SIZE - 6);
		break;
	default:
		error("FlicDecoder::decodeFrame(): unknown main chunk type (type = 0x%02X)", frameType);
		break;
	 }

	_curFrame++;
	_nextFrameStartTime += _frameDelay;

	if (_atRingFrame) {
		// If we decoded the ring frame, seek to the second frame
		_atRingFrame = false;
		_fileStream->seek(_offsetFrame2);
	}

	return _surface;
}

void FlicDecoder::FlicVideoTrack::handleFrame() {
	uint16 chunkCount = _fileStream->readUint16LE();
	// Note: The overridden delay is a 16-bit integer (word), whereas the normal delay is a 32-bit integer (dword)
	// the frame delay is the FLIC "speed", in milliseconds.
	uint16 newFrameDelay = _fileStream->readUint16LE();	// "speed", in milliseconds
	if (newFrameDelay > 0)
		_frameDelay = newFrameDelay;

	_fileStream->readUint16LE();	// reserved, always 0
	uint16 newWidth = _fileStream->readUint16LE();
	uint16 newHeight = _fileStream->readUint16LE();

	if ((newWidth != 0) || (newHeight != 0)) {
		if (newWidth == 0)
			newWidth = _surface->w;
		if (newHeight == 0)
			newHeight = _surface->h;

		_surface->free();
		delete _surface;
		_surface = new Graphics::Surface();
		_surface->create(newWidth, newHeight, Graphics::PixelFormat::createFormatCLUT8());
	}

	// Read subchunks
	for (uint32 i = 0; i < chunkCount; ++i) {
		uint32 frameSize = _fileStream->readUint32LE();
		uint16 frameType = _fileStream->readUint16LE();
		uint8 *data = new uint8[frameSize - 6];
		_fileStream->read(data, frameSize - 6);

		switch (frameType) {
		case FLI_SETPAL:
			unpackPalette(data);
			_dirtyPalette = true;
			break;
		case FLI_SS2:
			decodeDeltaFLC(data);
			break;
		case FLI_BLACK:
			_surface->fillRect(Common::Rect(0, 0, getWidth(), getHeight()), 0);
			_dirtyRects.clear();
			_dirtyRects.push_back(Common::Rect(0, 0, getWidth(), getHeight()));
			break;
		case FLI_BRUN:
			decodeByteRun(data);
			break;
		case FLI_COPY:
			copyFrame(data);
			break;
		case PSTAMP:
			/* PSTAMP - skip for now */
			break;
		default:
			error("FlicDecoder::decodeNextFrame(): unknown subchunk type (type = 0x%02X)", frameType);
			break;
		}

		delete[] data;
	}
}

void FlicDecoder::FlicVideoTrack::copyDirtyRectsToBuffer(uint8 *dst, uint pitch) {
	for (const auto &dirtyRect : _dirtyRects) {
		for (int y = dirtyRect.top; y < dirtyRect.bottom; ++y) {
			const int x = dirtyRect.left;
			memcpy(dst + y * pitch + x, (byte *)_surface->getBasePtr(x, y), dirtyRect.right - x);
		}
	}

	clearDirtyRects();
}

void FlicDecoder::FlicVideoTrack::copyFrame(uint8 *data) {
	memcpy((byte *)_surface->getPixels(), data, getWidth() * getHeight());

	// Redraw
	_dirtyRects.clear();
	_dirtyRects.push_back(Common::Rect(0, 0, getWidth(), getHeight()));
}

void FlicDecoder::FlicVideoTrack::decodeByteRun(uint8 *data) {
	byte *ptr = (byte *)_surface->getPixels();
	for (int i = 0; i < getHeight(); ++i) {
		data++;
		for (int j = 0; j < getWidth();) {
			int count = (int8)*data++;
			if (count > 0) {
				memset(ptr, *data++, count);
			} else {
				count = -count;
				memcpy(ptr, data, count);
				data += count;
			}
			ptr += count;
			j += count;
		}
	}

	// Redraw
	_dirtyRects.clear();
	_dirtyRects.push_back(Common::Rect(0, 0, getWidth(), getHeight()));
}

#define OP_PACKETCOUNT   0
#define OP_UNDEFINED     1
#define OP_LASTPIXEL     2
#define OP_LINESKIPCOUNT 3

void FlicDecoder::FlicVideoTrack::decodeDeltaFLC(uint8 *data) {
	uint16 linesInChunk = READ_LE_UINT16(data); data += 2;
	uint16 currentLine = 0;
	uint16 packetCount = 0;

	while (linesInChunk--) {
		uint16 opcode;

		// First process all the opcodes.
		do {
			opcode = READ_LE_UINT16(data); data += 2;

			switch ((opcode >> 14) & 3) {
			case OP_PACKETCOUNT:
				packetCount = opcode;
				break;
			case OP_UNDEFINED:
			default:
				break;
			case OP_LASTPIXEL:
				*((byte *)_surface->getBasePtr(getWidth() - 1, currentLine)) = (opcode & 0xFF);
				_dirtyRects.push_back(Common::Rect(getWidth() - 1, currentLine, getWidth(), currentLine + 1));
				break;
			case OP_LINESKIPCOUNT:
				currentLine += -(int16)opcode;
				break;
			}
		} while (((opcode >> 14) & 3) != OP_PACKETCOUNT);

		uint16 column = 0;

		// Now interpret the RLE data
		while (packetCount--) {
			column += *data++;
			int rleCount = (int8)*data++;
			if (rleCount > 0) {
				memcpy((byte *)_surface->getBasePtr(column, currentLine), data, rleCount * 2);
				data += rleCount * 2;
				_dirtyRects.push_back(Common::Rect(column, currentLine, column + rleCount * 2, currentLine + 1));
			} else if (rleCount < 0) {
				rleCount = -rleCount;
				uint16 dataWord = READ_UINT16(data); data += 2;
				for (int i = 0; i < rleCount; ++i) {
					WRITE_UINT16((byte *)_surface->getBasePtr(column + i * 2, currentLine), dataWord);
				}
				_dirtyRects.push_back(Common::Rect(column, currentLine, column + rleCount * 2, currentLine + 1));
			}
			column += rleCount * 2;
		}

		currentLine++;
	}
}

void FlicDecoder::FlicVideoTrack::unpackPalette(uint8 *data) {
	uint16 numPackets = READ_LE_UINT16(data); data += 2;

	if (0 == READ_LE_UINT16(data)) { //special case
		data += 2;
		for (int i = 0; i < 256; ++i) {
			byte r = data[i * 3];
			byte g = data[i * 3 + 1];
			byte b = data[i * 3 + 2];
			_palette.set(i, r, g, b);
		}
	} else {
		uint8 palPos = 0;

		while (numPackets--) {
			palPos += *data++;
			uint8 change = *data++;

			for (int i = 0; i < change; ++i) {
				byte r = data[i * 3];
				byte g = data[i * 3 + 1];
				byte b = data[i * 3 + 2];
				_palette.set(palPos + i, r, g, b);
			}

			palPos += change;
			data += (change * 3);
		}
	}
}

} // End of namespace Video
