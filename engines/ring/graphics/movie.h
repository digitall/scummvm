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

#ifndef RING_MOVIE_H
#define RING_MOVIE_H

#include "ring/shared.h"

#include "audio/audiostream.h"
#include "audio/mixer.h"

#include "common/mutex.h"
#include "common/rect.h"
#include "common/stream.h"
#include "common/substream.h"

namespace Ring {

class ImageLoaderMovie;
class Movie;
class ScreenManager;

class Cinematic : public Common::SeekableReadStream {
public:
	virtual ~Cinematic() {}

	virtual void skipFrame() = 0;
	virtual bool tControl() = 0;
	virtual void setState(bool state) = 0;

	// ReadStream
	bool eos() const override;
	uint32 read(void *dataPtr, uint32 dataSize) override;

	// SeekableReadStream
	int32 pos() const override;
	int32 size() const override;
	bool seek(int32 offset, int whence = SEEK_SET) override;

protected:
	Common::SeekableReadStream *_stream;    ///< The movie file stream
};

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
	void updateBuffer(int index, int *compressedDataEnd, int **buffer);
	void updateBufferControl(int index, int **buffer);
};

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

class CinematicSound {
public:
	CinematicSound();
	~CinematicSound();

	void init(uint32 channel, uint32 bitsPerSample, uint32 samplesPerSec);
	void deinit();

	void play();
	void setVolume(int32 volume);
	void queueBuffer(Common::SeekableReadStream *stream);

private:
	Audio::SoundHandle                            _handle;

	int16                                         _channels;
	int32                                         _samplesPerSec;
	int32                                         _avgBytesPerSec;
	int16                                         _blockAlign;
	int16                                         _bitsPerSample;
	Audio::QueuingAudioStream                    *_audioStream;
	bool                                          _isPlaying;
	float                                         _volume;

};

class Movie {
public:
	Movie(ScreenManager *screen);
	~Movie();

	bool init(const Common::String &path, Common::String filename, const Common::String &languageFolder, uint32 channel);
	void deinit();

	void play(const Common::Point &point);

	uint32 playNextFrame(const Common::Point &point, DrawType drawType);
	uint32 getNumberOfFrames();

	void setSynchroOff();

	void setFramerate(float rate) { _framerate = rate; }

private:
	enum ChunkType {
		kChunkA = 65,
		kChunkB = 66,
		kChunkS = 83,
		kChunkT = 84,
		kChunkU = 85,
		kChunkZ = 90
	};

	ImageLoaderMovie *_image;
	ScreenManager    *_screen;
	CinematicSound   *_sound;
	bool              _isSoundInitialized;
	bool              _field_5B;
	float             _framerate;
	bool              _hasDialog;
	uint32            _channel;
	bool              _isCI2;

	// Sound
	bool readSound();
	bool skipSound();
};

} // End of namespace Ring

#endif // RING_MOVIE_H
