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

#include "ring/sound/sound_entry.h"

#include "ring/base/application.h"

#include "ring/sound/sound_loader.h"
#include "ring/sound/sound_manager.h"

#include "ring/helpers.h"
#include "ring/ring.h"

#include "audio/decoders/wave.h"

#include "common/file.h"

namespace Ring {

#pragma region SoundEntry

SoundEntry::SoundEntry(Id soundId, SoundType type, Common::String name, LoadFrom loadFrom, SoundFormat format) : BaseObject(soundId) {
	_type       = type;
	_name       = name;
	_isPlaying  = false;
	_loadFrom   = loadFrom;
	_volume     = 100;
	_multiplier = 100;
	_pan        = 0;
	_loop       = false;
	_format     = format;
	_field_125  = 1;
}

SoundEntry::~SoundEntry() {
}

bool SoundEntry::isPlaying() const {
	return getSound()->getMixer()->isSoundHandleActive(_handle);
}

void SoundEntry::setVolume(int32 volume) {
	if (volume >= 0)
		_volume = (volume > 100) ? 100 : volume;
	else
		_volume = 0;

	setVolumeAndPan();
}

void SoundEntry::setMultiplier(int32 multiplier) {
	if (multiplier >= 0)
		_multiplier = (multiplier > 100) ? 100 : multiplier;
	else
		_multiplier = 0;

	setVolumeAndPan();
}

void SoundEntry::setPan(int32 pan) {
	if (pan >= -100)
		_pan = (pan > 100) ? 100 : pan;
	else
		_pan = -100;

	setVolumeAndPan();
}

bool SoundEntry::checkPlaying() {
	if (!_isPlaying)
		return false;

	bool playing = isPlaying();
	if (!playing) {
		_isPlaying = false;
		stop();
	}

	return (playing == false);
}

void SoundEntry::setVolumeAndPan() const {
	getSound()->getMixer()->setChannelVolume(_handle, (byte)(_volume * _multiplier * 0.01f));
	getSound()->getMixer()->setChannelBalance(_handle, (byte)_pan);
}

SoundFormat SoundEntry::getFormat(Common::String filename) {
	filename.toLowercase();

	if (filename.hasSuffix("wav"))
		return kSoundFormatWAV;

	if (filename.hasSuffix("wac"))
		return kSoundFormatWAC;

	if (filename.hasSuffix("was"))
		return kSoundFormatWAS;

	return kSoundFormatInvalid;
}

#pragma endregion

#pragma region SoundEntryStream

SoundEntryStream::SoundEntryStream(Id soundId, SoundType type, Common::String name, LoadFrom loadFrom, SoundFormat format, int32 soundChunk) : SoundEntry(soundId, type, name, loadFrom, format) {
	_audioStream = nullptr;
	_chunkSize = 0;
	_size = 0;
	_bufferOffset = 0;
	_field_14A = 0;
	_loop = 0;
	_field_152 = 0;
	_isBufferPlaying = false;
	_soundChunk = soundChunk;

	_loader = nullptr;
}

SoundEntryStream::~SoundEntryStream() {
	stopAndReleaseSoundBuffer();
}

void SoundEntryStream::play(bool loop) {
	// Compute sound path
	Common::String path = getApp()->getSoundPath(_name, _type);

	debugC(kRingDebugSound, "Playing sound: %s", path.c_str());

	initSoundBuffer(path, _soundChunk, loop, _format);

	// Rewind and play sound
	stop();

	if (_audioStream) {
		_audioStream->rewind();
		getSound()->getMixer()->playStream(Audio::Mixer::kPlainSoundType, &_handle, makeLoopingAudioStream(_audioStream, loop ? 0 : 1));

		setVolumeAndPan();
		_isPlaying = true;
	}
}

void SoundEntryStream::stop() {
	getSound()->getMixer()->stopHandle(_handle);

	_isPlaying = false;
}

void SoundEntryStream::stopAndClear() {
	stop();
	stopAndReleaseSoundBuffer();
}

void SoundEntryStream::initSoundBuffer(const Common::String &path, int32 soundChunk, bool loop, SoundFormat format) {
	stopAndReleaseSoundBuffer();

	_loop = loop;

	// Handle compressed sounds
	if (format != kSoundFormatWAV) {
		_loader = new SoundLoader(format);

		if (loadData(format, path, soundChunk))
			SAFE_DELETE(_loader);

		return;
	}

	// Handle uncompressed wav file
	SAFE_DELETE(_audioStream);

	Common::SeekableReadStream *stream = SearchMan.createReadStreamForMember(path);
	if (!stream)
		error("[SoundEntryStream::initSoundBuffer] Cannot open stream to file (%s)", path.c_str());

	_audioStream = Audio::makeWAVStream(stream, DisposeAfterUse::YES);
}

void SoundEntryStream::stopAndReleaseSoundBuffer() {
	if (!_audioStream)
		return;

	stop();

	// Close loader
	if (_loader)
		_loader->close();

	SAFE_DELETE(_loader);

	// Delete audiostream
	SAFE_DELETE(_audioStream);
}

bool SoundEntryStream::loadData(SoundFormat format, const Common::String &path, int32 soundChunk) {
	if (!_loader)
		error("[SoundEntryStream::loadData] Loader not initialized properly");

	if (_loader->load(path, this))
		return true;

	if (_loader->getType() != 1) { // PCM data
		_loader->close();
		return true;
	}

	if (_loader->getChunk()) {
		_loader->close();
		return true;
	}

	// Adjust entry size
	if (soundChunk <= 0)
		_size = -soundChunk;
	else
		_size = soundChunk * _loader->getSamplesPerSec() * _loader->getBlockAlign();

	_size >>= 2;

	if (_size % _loader->getBlockAlign())
		_size += _loader->getBlockAlign() - _size % _loader->getBlockAlign();

	_chunkSize = _size * 4;
	_field_152 = 0;
	_bufferOffset = 0;

	// Load data chunk
	loadDataChunk();

	_field_14A = 0;

	return false;
}

void SoundEntryStream::loadDataChunk() {
	error("[SoundEntryStream::loadDataChunk] Not implemented");
}

#pragma endregion

#pragma region SoundEntryData

SoundEntryData::SoundEntryData(Id soundId, SoundType type, Common::String name, LoadFrom loadFrom, SoundFormat format) : SoundEntry(soundId, type, name, loadFrom, format) {
	_audioStream = nullptr;
	_isPreloaded = 0;
}

SoundEntryData::~SoundEntryData() {
	stopAndReleaseSoundBuffer();
}

void SoundEntryData::play(bool loop) {
	if (!_audioStream)
		preload();

	// Rewind and play sound
	_audioStream->rewind();
	getSound()->getMixer()->playStream(Audio::Mixer::kPlainSoundType, &_handle, makeLoopingAudioStream(_audioStream, loop ? 0 : 1));

	setVolumeAndPan();
	_isPlaying = !loop;
}

void SoundEntryData::stop() {
	getSound()->getMixer()->stopHandle(_handle);

	if (!_isPreloaded)
		stopAndReleaseSoundBuffer();

	_isPlaying = false;
}

void SoundEntryData::stopAndClear() {
	stop();
	stopAndReleaseSoundBuffer();
}

void SoundEntryData::preload() {
	_isPreloaded = true;

	// Compute sound path
	Common::String path = getApp()->getSoundPath(_name, _type);
	initSoundBuffer(path);
}

void SoundEntryData::unload() {
	if (_isPreloaded)
		stopAndReleaseSoundBuffer();

	_isPreloaded = false;
}

void SoundEntryData::initSoundBuffer(const Common::String &path) {
	if (_audioStream)
		delete _audioStream;

	// Open stream to file
	Common::SeekableReadStream *stream = SearchMan.createReadStreamForMember(path);
	if (!stream)
		error("[SoundEntryData::initSoundBuffer] Cannot open stream to file (%s)", path.c_str());

	_audioStream = Audio::makeWAVStream(stream, DisposeAfterUse::YES);
}

void SoundEntryData::stopAndReleaseSoundBuffer() {
	if (!_audioStream)
		return;

	stop();
	SAFE_DELETE(_audioStream);
}

#pragma endregion

} // End of namespace Ring
