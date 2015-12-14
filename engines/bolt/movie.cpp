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

#include "bolt/movie.h"

#include "audio/audiostream.h"
#include "audio/decoders/raw.h"
#include "audio/mixer.h"
#include "common/debug.h"
#include "common/system.h"
#include "common/util.h"
#include "graphics/palette.h"
#include "graphics/surface.h"

#include "bolt/bolt.h"
#include "bolt/graphics.h"
#include "bolt/pf_file.h"

namespace Bolt {

Movie::Movie()
	: _graphics(nullptr),
	_mixer(nullptr),
	_audioStream(nullptr),
	_audioStarted(false),
	_triggerCallback(nullptr),
	_triggerCallbackParam(nullptr)
{ }

Movie::~Movie() {
	stopAudio();
}

void Movie::load(Graphics *graphics, Audio::Mixer *mixer, PfFile &pfFile, uint32 name, uint32 curTime) {
	debug(3, "loading movie %c%c%c%c ...",
		(name >> 24) & 0xff, (name >> 16) & 0xff, (name >> 8) & 0xff, name & 0xff);

	_graphics = graphics;
	_mixer = mixer;

	_file = pfFile.seekMovieAndGetFile(name);

	stop();

	_parserActive = true;
	_timelineActive = true;

	// FIXME: Use a timer that pauses when game is inactive
	_curFrameTimeMs = curTime;
	_curFrameNum = 0;

	// Load timeline (it should be the first packet)
	loadTimeline(fetchTimelineBuffer());

	if (!_audioStream) {
		_audioStream = Audio::makeQueuingAudioStream(22050, false);
		// Audio will play when first frame is presented.
	}
	fillAudioQueue();

	advanceTimeline();
}

void Movie::stop() {
	stopAudio();

	_parserActive = false;
	_timelineActive = false;

	_timelineBufAssembler.buf.reset();
	_audioBufAssembler.buf.reset();
	_videoBufAssembler.buf.reset();
	_auxVideoBufAssembler.buf.reset();

	_timeline.reset();
	for (int i = 0; i < 5; ++i) {
		_videoQueues[i].clear();
	}
	_queue4Buf.reset();
	_queue4Bg.reset();
	_queue4CameraY = 0;
	_queue4ScrollType = -1;

	_numColorCycles = 0;
}

void Movie::stopAudio() {
	if (_audioStream) {
		if (_audioStarted) {
			_mixer->stopHandle(_audioHandle); // Mixer deletes audio stream
		}
		else {
			delete _audioStream;
		}

		_audioStream = nullptr;
	}

	_audioStarted = false;
}

bool Movie::isAudioRunning() const {
	return _audioStarted && _audioStream &&
		_mixer->isSoundHandleActive(_audioHandle);
}

bool Movie::isRunning() const {
	return _graphics && (_timelineActive || isAudioRunning());
}

bool Movie::process(uint32 curTime) {

	fillAudioQueue();

	if (_timelineActive) {
		uint32 timeDelta = curTime - _curFrameTimeMs;
		if (timeDelta >= _framePeriod) {
			_curFrameTimeMs += _framePeriod;
			++_curFrameNum;
			advanceTimeline();
		}
	}

	return isRunning();
}

void Movie::setTriggerCallback(TriggerCallback callback, void *param) {
	_triggerCallback = callback;
	_triggerCallbackParam = param;
}

void Movie::fillAudioQueue() {
	static const int kNumSoundPacketsToQueue = 2;

	while (_parserActive && _audioStream &&
		_audioStream->numQueuedStreams() < kNumSoundPacketsToQueue) {

		readNextPacket();
	}
}

void Movie::ensureAudioStarted() {
	if (_audioStream && !_audioStarted) {
		// PF audio is premixed; speech and music volumes cannot be controlled
		// independently.
		_mixer->playStream(Audio::Mixer::kPlainSoundType, &_audioHandle,
			_audioStream);
		_audioStarted = true;
	}
}

struct Queue4ImageHeader {
	static const int kSize = 0x14;
	Queue4ImageHeader(const byte *src) {
		queueNum = READ_BE_UINT16(&src[0]);
		width = READ_BE_UINT16(&src[2]);
		height = READ_BE_UINT16(&src[4]);
		numFrames = READ_BE_UINT16(&src[6]);
		// FIXME: unknown fields
		controlDataOffset = READ_BE_UINT32(&src[0xC]);
	}

	uint16 queueNum;
	uint16 width;
	uint16 height;
	uint16 numFrames;
	uint32 controlDataOffset;
};

struct TimelineHeader {
	static const int kSize = 0xC;
	TimelineHeader(const byte *src) {
		numCommands = READ_BE_UINT16(&src[8]);
		framePeriod = READ_BE_UINT16(&src[0xA]);
	}

	uint16 numCommands;
	uint16 framePeriod;
};

void Movie::loadTimeline(ScopedBuffer::Movable buf) {
	_timeline.reset(buf);

	TimelineHeader header(&_timeline[0]);
	_numTimelineCmds = header.numCommands;
	_framePeriod = header.framePeriod;

	_curTimelineCmd = 0;
	_timelineCursor = TimelineHeader::kSize;
	_lastTimelineCmdFrameNum = _curFrameNum;
	loadTimelineCmd();
}

struct TimelineCommand {
	static const int kSize = 5;
	TimelineCommand(const byte *src) {
		delay = READ_BE_UINT16(&src[0]);
		opcode = READ_BE_UINT16(&src[2]);
		reps = src[4];
	}

	uint16 delay;
	uint16 opcode;
	byte reps;
};

void Movie::loadTimelineCmd() {
	assert(_timeline);

	TimelineCommand cmd(&_timeline[_timelineCursor]);
	_timelineReps = cmd.reps;
}

namespace TimelineOpcodes {
	enum {
		kDrawQueue0 = 1, // param size: 0
		kDrawQueue1 = 2, // param size: 0
		kStartColorCycles = 0xC, // param size: 8
		kStopColorCycles = 0xD, // param size: 0
		kLoadQueue4 = 0xF, // param size: 0
		kDrawQueue4 = 0x10, // param size: 0
		kFade = 0x13, // param size: 4
		kSetName = 0x7FFF, // param size: 4
		kTriggerEvent1 = 0x8001, // param size: 0
		kTriggerEvent2 = 0x8002, // param size: 0
	};
}

int Movie::getTimelineCmdParamSize(uint16 opcode) {
	switch (opcode) {
	case TimelineOpcodes::kDrawQueue0: return 0;
	case TimelineOpcodes::kDrawQueue1: return 0;
	case TimelineOpcodes::kStartColorCycles: return 8;
	case TimelineOpcodes::kStopColorCycles: return 0;
	case TimelineOpcodes::kLoadQueue4: return 0;
	case TimelineOpcodes::kDrawQueue4: return 0;
	case TimelineOpcodes::kFade: return 4;
	case TimelineOpcodes::kSetName: return 4;
	case TimelineOpcodes::kTriggerEvent1: return 0;
	case TimelineOpcodes::kTriggerEvent2: return 0;
	default:
		error("Unhandled timeline opcode 0x%X (unknown size)", opcode);
		return 0;
	}
}

void Movie::runTimelineCmd() {
	TimelineCommand cmd(&_timeline[_timelineCursor]);

	switch (cmd.opcode) {
	case TimelineOpcodes::kDrawQueue0: // render queue 0 (param size: 0)
		_graphics->getBackPlane().clear(); // FIXME: Is it correct to clear back?
		drawQueue0or1(_graphics->getForePlane(), ScopedBuffer(fetchVideoBuffer(0)), 0, 0);
		break;
	case TimelineOpcodes::kDrawQueue1: // render queue 1 (param size: 0)
		_graphics->getForePlane().clear(); // FIXME: Is it correct to clear fore?
		drawQueue0or1(_graphics->getBackPlane(), ScopedBuffer(fetchVideoBuffer(1)), 0, 0);
		break;
	case TimelineOpcodes::kStartColorCycles: // start palette cycling (param size: 8)
	{
		const byte *params = &_timeline[_timelineCursor + TimelineCommand::kSize];
		uint16 start = READ_BE_UINT16(&params[0]);
		uint16 plane = READ_BE_UINT16(&params[2]); // ?? Always 1 in Merlin
		uint16 num = READ_BE_UINT16(&params[4]);
		int16 delay = READ_BE_INT16(&params[6]); // ?? Maybe delay? Negative for backwards?
		debug(3, "start color cycles (%d, %d, %d, %d)",
			(int)start, (int)plane, (int)num, (int)delay
			);
		if (plane != 1) {
			warning("Color cycling plane not 1 in movie");
		}
		if (_numColorCycles < 0 || _numColorCycles >= kMaxColorCycles) {
			warning("tried to start too many color cycles");
		}
		// TODO: use correct delay
		_graphics->setColorCycle(_numColorCycles, start, num, delay);
		++_numColorCycles;
		break;
	}
	case TimelineOpcodes::kStopColorCycles: // stop palette cycling (param size: 0)
		debug(3, "stop color cycles");
		_graphics->resetColorCycles();
		_numColorCycles = 0;
		break;
	case TimelineOpcodes::kLoadQueue4: // load queue 4 (param size: 0)
		loadQueue4(fetchVideoBuffer(4));
		break;
	case TimelineOpcodes::kDrawQueue4: // render queue 4 (param size: 0)
		if (!_queue4Buf) {
			loadQueue4(fetchVideoBuffer(4));
		}
		if (_queue4Buf) {
			Queue4ImageHeader header(&_queue4Buf[0]);
			int queue4FrameNum = _curFrameNum - _queue4StartFrameNum;
			queue4FrameNum = MIN<int>(queue4FrameNum, header.numFrames - 1);

			runQueue4Control();

			if (_queue4Bg) {
				updateScroll();
			}

			// Do NOT redraw background here; it breaks win movies.

			drawQueue4(_graphics->getForePlane(), _queue4Buf, queue4FrameNum);
		}
		break;
	case TimelineOpcodes::kFade: // fade (param size: 4)
	{
		const byte *params = &_timeline[_timelineCursor + TimelineCommand::kSize];
		uint16 param1 = READ_BE_UINT16(&params[0]); // Duration in milliseconds (or frames?)
		int16 param2 = READ_BE_INT16(&params[2]); // 1: fade in; -1: fade out
		debug(3, "fade (%d, %d)", (int)param1, (int)param2);
		// TODO: implement
		break;
	}
	case TimelineOpcodes::kSetName: // set name (param size: 4, movie name)
		// Ignored
		break;
	case TimelineOpcodes::kTriggerEvent1: // trigger event (param size: 0, used in INTR)
	case TimelineOpcodes::kTriggerEvent2: // trigger event (param size: 0, enters hub card in win movies)
		if (_triggerCallback) {
			_triggerCallback(_triggerCallbackParam, cmd.opcode);
		}
		break;
	default:
		error("Unimplemented timeline command 0x%X", (int)cmd.opcode);
		break;
	}
}

void Movie::advanceTimeline() {
	assert(_timelineActive);
	assert(_timeline);

	// Note that there may be a sequence of timeline commands with 0 delay.
	// We want to process them all before returning.
	bool done = false;
	while (!done) {

		TimelineCommand cmd(&_timeline[_timelineCursor]);

		if (_timelineReps <= 0) {
			// Advance to next timeline command
			_timelineCursor += TimelineCommand::kSize + getTimelineCmdParamSize(cmd.opcode);
			++_curTimelineCmd;
			if (_curTimelineCmd >= _numTimelineCmds) {
				// TODO: Guarantee start of audio coincides with first frame of
				// movie.
				_graphics->markDirty();
				ensureAudioStarted();
				_timelineActive = false;
				done = true;
			}
			else {
				loadTimelineCmd();
			}
		}
		else if ((_curFrameNum - _lastTimelineCmdFrameNum) < cmd.delay) {
			// Delay occurs BEFORE command.
			_graphics->markDirty();
			ensureAudioStarted();
			done = true;
		}
		else {
			runTimelineCmd();
			_lastTimelineCmdFrameNum = _curFrameNum;
			--_timelineReps;
		}
	}
}

void Movie::loadQueue4(ScopedBuffer::Movable buf) {

	_queue4Buf.reset(buf);

	Queue4ImageHeader header(&_queue4Buf[0]);

	// Do NOT reset queue4 background or camera here.
	// It remains after queue4 is finished.
	_queue4StartFrameNum = _curFrameNum;
	_lastQueue4ControlFrameNum = _curFrameNum;
	_queue4ControlCursor = header.controlDataOffset;

	// Do NOT reset scrolling vars here.
}

struct Queue4Command {
	static const int kSize = 4;
	Queue4Command(const byte *src) {
		delay = src[0];
		opcode = src[1];
		// FIXME: unknown fields.
		// src[2] and src[3] seem to be ignored in original program!
	}

	byte delay;
	byte opcode;
};

namespace Queue4Opcodes {
	enum {
		kLoadBack = 1,
		kLoadForePalette = 2,
		kScroll = 3,
		kStop = 0xFF,
	};
}

void Movie::runQueue4Control() {
	assert(_queue4Buf);

	// FIXME: Queue 4 control commands are different between Merlin and Crete.
	// Only Merlin commands are implemented.

	bool done = false;
	while (!done) {

		Queue4Command cmd(&_queue4Buf[_queue4ControlCursor]);
		int paramsOffset = _queue4ControlCursor + Queue4Command::kSize;

		// Delay occurs BEFORE command
		if ((_curFrameNum - _lastQueue4ControlFrameNum) >= cmd.delay) {

			switch (cmd.opcode) {

			case Queue4Opcodes::kLoadBack: // load background from queue 1
			{
				_queue4Bg.reset(fetchVideoBuffer(1));

				_queue4CameraX = (int16)READ_BE_UINT16(&_queue4Buf[paramsOffset]);
				_queue4CameraY = (int16)READ_BE_UINT16(&_queue4Buf[paramsOffset + 2]);

				// FIXME: should camera params be initialized here? original
				// program doesn't seem to do anything with scroll variables
				// other than camera position

				// FIXME: is it correct to draw background here?
				drawBackground();

				_queue4ControlCursor += Queue4Command::kSize + 4;
				break;
			}
			case Queue4Opcodes::kLoadForePalette: // modify foreground (???) palette
			{
				byte numColors = _queue4Buf[paramsOffset];
				// FIXME: first color or plane number?
				byte firstColor = _queue4Buf[paramsOffset + 1];
				debug(3, "Queue4 cmd: load fore palette num %d first %d", (int)numColors, (int)firstColor);
				_graphics->getForePlane().setPalette(&_queue4Buf[paramsOffset + 2],
					firstColor, numColors);

				_queue4ControlCursor += Queue4Command::kSize + 2 + numColors * 3;
				break;
			}
			case Queue4Opcodes::kScroll: // start scroll
				_queue4ScrollStartFrameNum = _curFrameNum;
				_queue4ScrollOriginalX = _queue4CameraX;
				_queue4ScrollOriginalY = _queue4CameraY;
				_queue4ScrollProgress = 0; // ???

				// FIXME: scrolling is broken. Unknown whether these names are
				// correct.
				_queue4ScrollTime = (int16)READ_BE_UINT16(&_queue4Buf[paramsOffset]);
				_queue4ScrollMult = _queue4Buf[paramsOffset + 2];
				_queue4ScrollType = _queue4Buf[paramsOffset + 3];

				debug(3, "scroll params: type %d, time %d, mult %d",
					_queue4ScrollType, _queue4ScrollTime, _queue4ScrollMult);

				_queue4ControlCursor += Queue4Command::kSize + 4;
				break;
			case Queue4Opcodes::kStop: // stop? (found at end)
				done = true;
				break;
			default:
				error("Unknown queue4 control command 0x%X", (int)cmd.opcode);
				break;
			}

			_lastQueue4ControlFrameNum = _curFrameNum;
		}
		else {
			done = true;
		}
	}
}

void Movie::drawBackground() {
	drawQueue0or1(_graphics->getBackPlane(), _queue4Bg,
		0, -_queue4CameraY);
}

void Movie::updateScroll() {
	// FIXME: scrolling is almost completely broken. Reverse-engineer
	// this more carefully.

	if (_queue4ScrollType != -1)
	{
		int frames = 1;

		int esi = 0;
		int ecx = 0;
		int edi = _queue4ScrollProgress;
		int ebx = _queue4ScrollTime;
		int edx = edi;
		edi += frames;
		if (edi > ebx) {
			//edi = ebx;
		}

		if (edi != edx) {
			switch (_queue4ScrollType) {
			case -1: // Disabled
				break;
				// FIXME: Left and right may be unused, but Up is used in FNLE.
				// Please test.
			case 0:
				esi = _queue4ScrollMult * edi;
				break;
			case 1:
				esi = -_queue4ScrollMult * edi;
				break;
			case 2:
				ecx = -_queue4ScrollMult * edi;
				break;
			case 3: // Down
				ecx = _queue4ScrollMult * edi;
				break;
			default:
				warning("unhandled queue4 scroll type %d", _queue4ScrollType);
				break;
			}

			_queue4ScrollProgress = edi;
			_queue4CameraX = _queue4ScrollOriginalX + esi;
			_queue4CameraY = _queue4ScrollOriginalY + ecx;

			drawBackground();
		}
	}
}

enum PfPacketType {
	kPfTimeline = 0, // Timeline info (appears at beginning of movie)
	kPfAudio = 1, // Raw mono unsigned 8-bit PCM at 22050 Hz
	kPfVideo = 2, // Video
	kPfAuxVideo = 3, // Auxiliary video (used for large scrolling backgrounds)
	// NOTE: type 4 is related to sound. Original program flushes sound queue
	// when it encounters a type 4 packet.
	// NOTE: type 0xFE occurs near the end. It might signal the end of sound
	// packets.
	kPfFinal = 0xFF, // End of packets
};

struct Movie::PacketHeader {

	PacketHeader(Common::File &file) {
		totalSize = file.readUint32BE();
		partialSize = file.readUint32BE();
		type = file.readByte();
		unk = file.readByte();
	}

	uint32 totalSize;
	uint32 partialSize;
	uint8 type;
	uint8 unk;
};

void Movie::readNextPacket() {
	assert(_parserActive);

	// Read packet header
	PacketHeader header(*_file);
	// header.unk is always 0 in Merlin; original program seems to use it for
	// something related to multi-streaming.
	if (header.unk != 0) {
		warning("Unknown pf packet header unk %u", header.unk);
	}

	switch (header.type) {

	case kPfTimeline:
		if (readIntoBuffer(_timelineBufAssembler, header)) {
			_timelineQueue.push(_timelineBufAssembler.buf.release());
			_timelineBufAssembler.buf.reset();
		}
		break;

	case kPfAudio:
		if (readIntoBuffer(_audioBufAssembler, header)) {

			if (_audioStream) {
				// FIXME: Make this more efficient by reading directly into a
				// malloc'ed buffer.
				byte *sound = (byte*)malloc(_audioBufAssembler.totalSize);
				memcpy(sound, &_audioBufAssembler.buf[0], _audioBufAssembler.totalSize);

				_audioStream->queueBuffer(sound, _audioBufAssembler.totalSize,
					DisposeAfterUse::YES, Audio::FLAG_UNSIGNED);
				// sound will be freed by audio system
			}

			_audioBufAssembler.buf.reset();
		}
		break;

	case kPfVideo:
		if (readIntoBuffer(_videoBufAssembler, header)) {
			enqueueVideoBuffer(_videoBufAssembler.buf.release());
			_videoBufAssembler.buf.reset();
		}
		break;

	case kPfAuxVideo:
		if (readIntoBuffer(_auxVideoBufAssembler, header)) {
			// FIXME: Is any special handling required for auxiliary video? I
			// believe auxiliary video is nothing more than a second video
			// stream, allowing large video buffers (like background images)
			// to be loaded alongside regular video.
			enqueueVideoBuffer(_auxVideoBufAssembler.buf.release());
			_auxVideoBufAssembler.buf.reset();
		}
		break;

	case kPfFinal:
		// Final packet found, but don't stop the movie because the timeline may
		// still be running.
		if (_audioStream) {
			_audioStream->finish();
			_audioStream = nullptr; // Audio stream will be freed by the mixer.
		}
		_parserActive = false;
		break;

	default:
		warning("Unknown PF packet type %u skipped", header.type);
		_file->seek(header.partialSize, SEEK_CUR);
		break;
	}
}

// Returns true if buffer is complete. Buffers may be split across multiple
// packets.
bool Movie::readIntoBuffer(BufferAssembler &assembler, const PacketHeader &header) {

	if (!assembler.buf) {
		// Begin buffer
		assembler.totalSize = header.totalSize;
		assembler.buf.alloc(header.totalSize);
		assembler.cursor = 0;
	}
	else if (header.totalSize != assembler.totalSize) {
		warning("Bad PF packet: total size field mismatch");
	}

	uint32 partialSize = header.partialSize;
	if ((assembler.cursor + partialSize) > assembler.totalSize) {
		warning("Bad PF packet: buffer overflow");
		partialSize = assembler.totalSize - assembler.cursor;
	}

	_file->read(&assembler.buf[assembler.cursor], partialSize);
	assembler.cursor += partialSize;

	return assembler.cursor >= assembler.totalSize;
}

Movie::ScopedBuffer::Movable Movie::fetchTimelineBuffer() {

	while (_parserActive && _timelineQueue.empty()) {
		readNextPacket();
	}

	if (_timelineQueue.empty()) {
		error("Failed to fetch PF timeline");
		return Movie::ScopedBuffer::Movable();
	}

	return _timelineQueue.pop();
}

Movie::ScopedBuffer::Movable Movie::fetchVideoBuffer(uint16 queueNum) {

	while (_parserActive && _videoQueues[queueNum].empty()) {
		readNextPacket();
	}

	if (_videoQueues[queueNum].empty()) {
		error("Failed to fetch PF video");
		return Movie::ScopedBuffer::Movable();
	}

	return _videoQueues[queueNum].pop();
}

struct Queue01ImageHeader {

	const static int SIZE = 0xD;
	Queue01ImageHeader(const byte *src) {
		queueNum = READ_BE_UINT16(&src[0]);
		width = READ_BE_UINT16(&src[2]);
		height = READ_BE_UINT16(&src[4]);
		// FIXME: Unknown fields here
		compression = src[0xC];
	}

	uint16 queueNum;
	uint16 width;
	uint16 height;
	byte compression;
};

void Movie::drawQueue0or1(Plane &plane, const ScopedBuffer &src, int x, int y) {

	// Queue 0 buffers contain background frames. (FIXME: Really?)
	// Queue 1 buffers contain background frames for use with queue 4
	// sequences.

	Queue01ImageHeader header(&src[0]);
	assert(header.queueNum == 0 || header.queueNum == 1);

	plane.setPalette(&src[Queue01ImageHeader::SIZE], 0, 128);

	const byte *imageSrc = &src[Queue01ImageHeader::SIZE + 128 * 3];
	int imageSrcLen = src.size() - 128 * 3 - Queue01ImageHeader::SIZE;

	if (header.compression) {
		decodeRL7(plane.getSurface(), x, y, header.width, header.height,
			imageSrc, imageSrcLen, false);
	}
	else {
		decodeCLUT7(plane.getSurface(), x, y, header.width, header.height,
			imageSrc, imageSrcLen, false);
	}
}

void Movie::drawQueue4(Plane &plane, const ScopedBuffer &src, uint16 frameNum) {

	// Queue 4 buffers contain a sequence of foreground frames and control data
	// for the background.

	Queue4ImageHeader header(&src[0]);
	assert(header.queueNum == 4);
	assert(frameNum < header.numFrames);

	uint32 rl7Offset = READ_BE_UINT32(&src[Queue4ImageHeader::kSize + frameNum * 8]);
	uint32 rl7Size = READ_BE_UINT32(&src[Queue4ImageHeader::kSize + frameNum * 8 + 4]);

	decodeRL7(plane.getSurface(), 0, 0, header.width, header.height,
		&src[rl7Offset], rl7Size, false);
}

void Movie::enqueueVideoBuffer(ScopedBuffer::Movable buf) {
	ScopedBuffer myBuf(buf);
	uint16 queueNum = READ_BE_UINT16(&myBuf[0]);
	if (queueNum < 5) {
		_videoQueues[queueNum].push(myBuf.release());
	}
	else {
		warning("Unknown PF image stream queue num %u", queueNum);
	}
}

} // End of namespace Bolt
