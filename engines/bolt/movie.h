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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef BOLT_MOVIE_H
#define BOLT_MOVIE_H

#include "audio/mixer.h"

#include "bolt/util.h"

class OSystem;

namespace Audio {
class QueuingAudioStream;
}

namespace Common {
class File;
};

namespace Bolt {

class BoltEngine;
class Graphics;
class PfFile;
class Plane;

class Movie {
public:
	Movie();
	~Movie();

	// TODO: Find a better way of communicating time to Movie and other classes
	void start(Graphics *graphics, Audio::Mixer *mixer, PfFile &pfFile, uint32 name, uint32 curTime);
	void stop();

	bool isRunning() const;
	// Returns true if movie is running, false if movie finished.
	bool drive(uint32 curTime);

	typedef void (*TriggerCallback)(void *param, uint16 triggerType);
	void setTriggerCallback(TriggerCallback callback, void *param);

private:
	void stopAudio();
	bool isAudioRunning() const;

	Graphics *_graphics;
	Audio::Mixer *_mixer;
	Common::File *_file;

	typedef ScopedArray<byte> ScopedBuffer;
	typedef ScopedArrayQueue<byte> ScopedBufferQueue;

	bool _parserActive; // Set to false when final packet is found
	bool _timelineActive; // Set to false when timeline is finished

	// PACKET STREAMING

	void readNextPacket();

	ScopedBuffer::Movable fetchBuffer(ScopedBufferQueue &queue);

	void enqueueVideoBuffer(ScopedBuffer::Movable buf);

	struct BufferAssembler {
		ScopedBuffer buf;
		uint32 totalSize;
		uint32 cursor;
	};

	// Read from file into buffer assembler. Returns true if buffer is
	// complete. A new buffer is created when assembler.buf is clear. Please
	// reset assembler.buf when buffer is complete!
	struct PacketHeader;
	bool readIntoBuffer(BufferAssembler &assembler, const PacketHeader &header);

	BufferAssembler _timelineBufAssembler;
	BufferAssembler _audioBufAssembler;
	BufferAssembler _videoBufAssembler;
	BufferAssembler _auxVideoBufAssembler;

	ScopedBufferQueue _timelineQueue;
	static const int kNumVideoQueues = 5;
	ScopedBufferQueue _videoQueues[kNumVideoQueues];

	// AUDIO

	void loadAudio();
	void driveAudio();
	void playAudio();

	void fillAudioQueue();

	Audio::QueuingAudioStream *_audioStream;
	Audio::SoundHandle _audioHandle;
	bool _audioStarted;

	// TIMELINE

	void startTimeline(ScopedBuffer::Movable buf, uint32 curTime);
	void driveTimeline(uint32 curTime);

	void stepTimeline();
	int getTimelineCmdParamSize(uint16 opcode);
	void loadTimelineCommand(); // From _timelineCursor
	void runTimelineCommand(); // From _timelineCursor

	ScopedBuffer _timeline;
	uint32 _curFrameTime; // Time of current frame in milliseconds
	int _curFrameNum; // Number of currently-displayed frame
	int _curTimelineCmdNum;
	uint16 _framePeriod; // In milliseconds
	uint16 _numTimelineCmds;
	int _lastTimelineCmdFrame;
	int _timelineCursor;
	byte _timelineReps;

	TriggerCallback _triggerCallback;
	void *_triggerCallbackParam;

	static const int kMaxColorCycles = 4;
	int _numColorCycles;

	// QUEUE 4 CEL SEQUENCES

	void loadCels(ScopedBuffer::Movable buf);
	void stepCels();

	void stepCelCommands();
	void drawCelBackground();
	void drawCel(const ScopedBuffer &src, uint16 frameNum);

	ScopedBuffer _cels;
	ScopedBuffer _celsBackground;
	int _celsFrame;
	int _celControlCursor;
	int _celCurCameraX;
	int _celCurCameraY;
	int _celNextCameraX;
	int _celNextCameraY;

	// SCROLLING

	void startScroll(int duration, int speed, int type);
	void advanceScroll();

	int _celScrollDuration; // in steps
	int _celScrollSpeed;
	int _celScrollType; // -1 means no scrolling
	int _celScrollProgress;
	int _celScrollStartCameraX;
	int _celScrollStartCameraY;

	// FADING

	void startFade(uint16 duration, int16 direction);
	void driveFade(uint32 curTime);

	uint32 _fadeStartTime;
	uint16 _fadeDuration;
	int16 _fadeDirection;

	// DRAWING

	void applyQueue0or1Palette(int plane, const ScopedBuffer &src);
	void drawQueue0or1(int plane, const ScopedBuffer &src, int x, int y);
};

} // End of namespace Bolt

#endif
