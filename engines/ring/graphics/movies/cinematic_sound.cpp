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

#include "ring/graphics/movies/cinematic_sound.h"

#include "ring/base/application.h"

#include "ring/sound/soundentry.h"
#include "ring/sound/soundmanager.h"

#include "ring/ring.h"
#include "ring/helpers.h"

#include "audio/decoders/adpcm.h"

namespace Ring {

#pragma region CinematicSound

CinematicSound::CinematicSound() {
	_channels       = 0;
	_samplesPerSec  = 0;
	_avgBytesPerSec = 0;
	_blockAlign     = 0;
	_bitsPerSample  = 0;
	_audioStream    = nullptr;
	_isPlaying      = false;
	_volume         = 1.0f;
}

CinematicSound::~CinematicSound() {
	deinit();
}

void CinematicSound::init(uint32 channels, uint32 bitsPerSample, uint32 samplesPerSec) {
	deinit();

	_channels       = channels;
	_bitsPerSample  = bitsPerSample;
	_blockAlign     = bitsPerSample * channels / 8;
	_samplesPerSec  = samplesPerSec;
	_avgBytesPerSec = samplesPerSec * _blockAlign;

	_isPlaying      = false;
	_volume         = 1.0f;

	debugC(kRingDebugMovie, "Setting up movie sound (channels: %d, bitsPerSample: %d, blockAlign: %d, samplesPerSec: %d, avgBytesPerSec: %d)",
	       _channels, _bitsPerSample, _blockAlign, _samplesPerSec, _avgBytesPerSec);

	// Create an audio stream where the decoded chunks will be appended
	_audioStream = Audio::makeQueuingAudioStream(_samplesPerSec, channels == 1 ? false : true);
}

void CinematicSound::deinit() {
	if (!_audioStream)
		return;

	if (_isPlaying) {
		_isPlaying = false;

		// Stop sound
		getSound()->getMixer()->stopHandle(_handle);
	}

	// Close the audio stream
	_audioStream = nullptr;
}

void CinematicSound::play() {
	if (_isPlaying)
		return;

	if (!_audioStream)
		error("[CinematicSound::play] Audiostream not initialized properly");

	// Get sound volume
	int32 vol = _volume * -10000.0f;
	SoundEntry::convertVolumeFrom(vol);

	// Play sound
	getSound()->getMixer()->setChannelVolume(_handle, vol);
	getSound()->getMixer()->playStream(Audio::Mixer::kPlainSoundType, &_handle, _audioStream);

	_isPlaying = true;
}

void CinematicSound::setVolume(int32 volume) {
	_volume = volume * 0.01f;

	if (_volume < 0.0f)
		_volume = 0.0f;

	if (_volume > 1.0f)
		_volume = 1.0f;

	int32 vol = _volume * -10000.0f;
	SoundEntry::convertVolumeFrom(vol);

	getSound()->getMixer()->setChannelVolume(_handle, vol);
}

void CinematicSound::queueBuffer(Common::SeekableReadStream *stream) {
	if (!_audioStream)
		error("[CinematicSound::play] Audiostream not initialized properly");

	Audio::RewindableAudioStream *adpcm = Audio::makeADPCMStream(stream, DisposeAfterUse::YES, stream->size(), Audio::kADPCMMS, _samplesPerSec, _channels, _blockAlign);
	if (!adpcm) {
		warning("[CinematicSound::queueBuffer] Cannot decode sound stream");
		return;
	}

	// Queue the stream
	_audioStream->queueAudioStream(adpcm);
}

#pragma endregion

} // End of namespace Ring
