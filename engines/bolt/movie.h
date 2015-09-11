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
class PfFile;
class Plane;

class Movie {
public:
	Movie();
	~Movie();

	void load(BoltEngine *engine, PfFile &pfFile, uint32 name);
	void stop();

	bool isRunning() const;
	// Returns true if movie is running, false if movie is finished.
	bool process();

	typedef void (*TriggerCallback)(void *param, uint16 triggerType);
	void setTriggerCallback(TriggerCallback callback, void *param);

private:
	void stopAudio();
	bool isAudioRunning() const;

	BoltEngine *_engine;
	Common::File *_file;

	typedef ScopedArray<byte> ScopedBuffer;
	typedef ScopedArrayQueue<byte> ScopedBufferQueue;

	void fillAudioQueue();
	void ensureAudioStarted();

	bool _parserActive; // Set to false when final packet is found
	bool _timelineActive; // Set to false when timeline is finished

	Audio::QueuingAudioStream *_audioStream;
	bool _audioStarted;
	Audio::SoundHandle _audioHandle;

	// PACKET STREAMING

	void readNextPacket();

	ScopedBuffer::Movable fetchTimelineBuffer();
	ScopedBuffer::Movable fetchVideoBuffer(uint16 queueNum);

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
	ScopedBufferQueue _videoQueues[5];

	// TIMELINE

	void loadTimeline(ScopedBuffer::Movable buf);
	void loadTimelineCmd(); // load from _timelineCursor
	void advanceTimeline();
	int getTimelineCmdParamSize(uint16 opcode);
	void runTimelineCmd(); // at _timelineCursor

	uint32 _curFrameTimeMs; // Time of currently-displayed frame in milliseconds
	int _curFrameNum; // Number of currently-displayed frame

	ScopedBuffer _timeline;
	uint16 _numTimelineCmds;
	uint16 _framePeriod; // In milliseconds
	int _lastTimelineCmdFrameNum;
	uint16 _curTimelineCmd;
	int _timelineCursor;
	byte _timelineReps;

	TriggerCallback _triggerCallback;
	void *_triggerCallbackParam;

	// QUEUE 4 VIDEO SEQUENCES

	void loadQueue4(ScopedBuffer::Movable buf);
	void runQueue4Control();
	void drawBackground();
	void updateScroll();

	ScopedBuffer _queue4Buf;
	ScopedBuffer _queue4Bg; // Background image
	int _queue4StartFrameNum;
	int _lastQueue4ControlFrameNum;
	int _queue4ControlCursor; // Offset within queue 4 buffer of control data

	int _queue4CameraX;
	int _queue4CameraY;

	int _queue4ScrollStartFrameNum;
	int _queue4ScrollOriginalX;
	int _queue4ScrollOriginalY;
	int _queue4ScrollProgress;
	int _queue4ScrollTime;
	int _queue4ScrollMult;
	int _queue4ScrollType;

	// DRAWING

	void drawQueue0or1(Plane &plane, const ScopedBuffer &src, int x, int y);
	void drawQueue4(Plane &plane, const ScopedBuffer &src, uint16 frameNum);
};

} // End of namespace Bolt

#endif
