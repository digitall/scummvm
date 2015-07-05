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

#ifndef RING_CINEMATIC_SOUND_H
#define RING_CINEMATIC_SOUND_H

#include "ring/shared.h"

#include "audio/audiostream.h"
#include "audio/mixer.h"

#include "common/stream.h"

namespace Ring {

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

} // End of namespace Ring

#endif // RING_CINEMATIC_SOUND_H
