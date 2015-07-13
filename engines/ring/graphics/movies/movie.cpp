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

#include "ring/graphics/movies/movie.h"

#include "ring/base/application.h"
#include "ring/base/art.h"
#include "ring/base/dialog.h"
#include "ring/base/preferences.h"
#include "ring/base/puzzle.h"

#include "ring/graphics/codecs/imageloader_cin.h"
#include "ring/graphics/codecs/imageloader_ci2.h"
#include "ring/graphics/movies/cinematic.h"
#include "ring/graphics/movies/cinematic_sound.h"
#include "ring/graphics/image.h"
#include "ring/graphics/screen.h"

#include "ring/sound/soundhandler.h"

#include "ring/ring.h"
#include "ring/helpers.h"

#include "common/file.h"
#include "common/memstream.h"

namespace Ring {

Movie::Movie(ScreenManager *screen) : _screen(screen) {
	_image     = nullptr;
	_isSoundInitialized = false;
	_enableFrameSkipping  = false;
	_framerate = 0.0f;
	_hasDialog = false;
	_channel   = 0;
	_isCI2     = false;

	_sound = new CinematicSound();
}

Movie::~Movie() {
	SAFE_DELETE(_image);
	SAFE_DELETE(_sound);

	// Zero-out passed pointers
	_screen = nullptr;
}

bool Movie::init(const Common::String &path, Common::String filename, const Common::String &languageFolder, uint32 channel) {
	debugC(kRingDebugMovie, "Loading movie %s%s with channel %d", path.c_str(), filename.c_str(), channel);

	// Find the proper path to the movie file
	Common::String filePath = Common::String::format("%s%s%s", path.c_str(), filename.c_str(), ".cnm");
	if (Common::File::exists(filePath))
		goto load_movie;

	// Try with language subfolder
	filePath = Common::String::format("%s%s/%s%s", path.c_str(), languageFolder.c_str(), filename.c_str(), ".cnm");
	if (Common::File::exists(filePath))
		goto load_movie;

	// Try with .ci2 suffix
	filePath = Common::String::format("%s%s%s", path.c_str(), filename.c_str(), ".ci2");
	if (Common::File::exists(filePath))
		goto load_movie;

	// Try with .ci2 suffix and language subfolder
	filePath = Common::String::format("%s%s/%s%s", path.c_str(), languageFolder.c_str(), filename.c_str(), ".ci2");
	if (Common::File::exists(filePath))
		goto load_movie;

	// Cannot find movie
	warning("[Movie::init] Cannot find movie file: %s", filename.c_str());
	return false;

load_movie:
	// Initialize movie stream
	if (filePath.hasSuffix("cnm")) {
		_image = new ImageLoaderCIN();
	} else {
		_image = new ImageLoaderCI2();
		_isCI2 = true;
	}

	if (!_image->init(filePath, kArchiveFile, kZoneNone, kLoadFromCd))
		error("[Movie::init] Cannot read cinematic frame size");

	// Set channel and sound state
	_channel = channel;
	_isSoundInitialized = true;

	uint32 index = (channel < 1 || channel > 3) ? 0 : channel - 1;

	// Setup sound
	_sound->init(_image->getChannels(index) + 1, _image->getBitsPerSample(index), _image->getSamplesPerSec(index));
	_sound->setVolume(getApp()->getPreferenceHandler()->getVolume());

	// Setup framerate
	_enableFrameSkipping = true;
	_framerate = 1000.0f / (_image->getFrameRate() * 0.01f);

	// Setup sound handler
	SoundHandler *soundHandler = getApp()->getSoundHandler();
	if (soundHandler->getField0()) {
		soundHandler->turnOffItems1();

		if (!soundHandler->updateItems(_image->getChunkCount())) {
			soundHandler->turnOffSounds1(true);
			soundHandler->setField0(false);
		}
	}

	// Init dialog
	Common::String dialogPath = Common::String::format("DATA/%s/DIA/%s/%s", getApp()->getCurrentZoneFolder().c_str(), getApp()->getLanguageFolder().c_str(), filename.c_str());
	if (!Common::File::exists(dialogPath)) {
		_hasDialog = false;
		return true;
	}

	getApp()->getDialogHandler()->addDialog(new Dialog(500001, filename));
	_hasDialog = true;

	return true;
}

void Movie::deinit() {
	SAFE_DELETE(_image);

	_screen = nullptr;
	_enableFrameSkipping = true;
}

void Movie::play(const Common::Point &point) {
	debugC(kRingDebugMovie, "Playing movie at coordinates (%d, %d)", point.x, point.y);

	if (!_sound)
		error("[Movie::play] sound not initialized properly");

	if (!_image)
		error("[Movie::play] image not initialized properly");

	SoundHandler *soundHandler = getApp()->getSoundHandler();
	ScreenManager *screen = getApp()->getScreenManager();

	// Setup
	Cinematic *cinematic = _image->getCinematic();
	ImageSurface *image = new ImageSurface();
	bool setupSound = true;
	bool readFrame = false;
	uint32 chunkIndex = 0;
	uint32 waitChunk = 0;
	uint32 ticks = g_system->getMillis();

	// Setup header and state
	cinematic->setState(false);
	uint32 chunkCount = _image->getChunkCount();

	// Parse cinematic
	if (chunkCount) {
		while (!cinematic->eos() && !cinematic->err()) {

			// Interrupt playing on escape
			if (checkEscape()) {
				if (soundHandler->getField0()) {
					soundHandler->updateItems2(chunkCount);
					soundHandler->updateItems3(chunkCount);
				}
				break;
			}

			// Read chunk type
			ChunkType chunkType = (ChunkType)cinematic->readByte();
			if (cinematic->eos() || cinematic->err())
				error("[Movie::play] Cannot read chunk type");

			debugC(kRingDebugMovie, " Reading chunk %c", chunkType);

			switch (chunkType) {
			default:
				error("[Movie::play] Invalid chunk type %d (index: %d)", chunkType, chunkIndex);

			case kChunkA:
				switch (_channel) {
				default:
					if (!skipSound())
						error("[Movie::play] Chunk A: Cannot skip sound (index: %d)", chunkIndex);
					break;

				case 2:
					if (!readSound())
						error("[Movie::play] Chunk A: Cannot read sound (index: %d)", chunkIndex);

					if (setupSound) {
						if (_isSoundInitialized)
							_sound->play();

						setupSound = false;
					}
					break;
				}
				break;

			case kChunkB:
				switch (_channel) {
				default:
					if (!skipSound())
						error("[Movie::play] Chunk B: Cannot skip sound (index: %d)", chunkIndex);
					break;

				case 3:
					if (!readSound())
						error("[Movie::play] Chunk B: Cannot read sound (index: %d)", chunkIndex);

					if (setupSound) {
						if (_isSoundInitialized)
							_sound->play();

						setupSound = false;
					}
					break;
				}
				break;

			case kChunkS:
			case kChunkU:    // CI2 movies
				if (_enableFrameSkipping) {
					uint32 tickInterval = (g_system->getMillis() - ticks);

					if (((chunkIndex + 1) * _framerate) < tickInterval) {
						if (readFrame) {
							if (chunkType == kChunkS)
								cinematic->skipFrame();

							// Process sound
							if (soundHandler->getField0()) {
								soundHandler->updateItems2(chunkCount);
								soundHandler->updateItems3(chunkCount);
							}

							++chunkIndex;
							waitChunk = chunkIndex;
							break;
						}

						readFrame = true;
						ticks = g_system->getMillis();

					} else {
						if (!readFrame) {
							readFrame = true;
							ticks = g_system->getMillis();
						} else {
							// Wait for tick interval
							while ((waitChunk * _framerate) > (tickInterval + 50)) {
								// If escape was pressed, bail out of the movie playing
								if (checkEvents())
									goto cleanup;

								tickInterval = (g_system->getMillis() - ticks);
							}
						}
					}
				}

				if (!_image->readImage(image, kChunkS ? 17 : 32, kDrawTypeNormal))
					error("[Movie::play] Chunk S: Error reading image (index: %d)", chunkIndex);

				// Draw frame
				screen->draw(image, point, kDrawTypeNormal);

				if (_hasDialog)
					getApp()->getDialogHandler()->play();

				// For CI2 movies, we need to update the menu
				if (_isCI2) {
					Puzzle *menu = getApp()->getPuzzle(kPuzzleMenu);
					if (menu) {
						menu->alloc();
						menu->update();
					}
				}

				screen->updateScreen();
				g_system->updateScreen();

				// Process sound
				if (soundHandler->getField0()) {
					soundHandler->updateItems2(chunkCount);
					soundHandler->updateItems3(chunkCount);
				}

				++chunkIndex;
				waitChunk = chunkIndex;
				break;

			case kChunkT:
				if (!cinematic->tControl())
					error("[Movie::play] Chunk T: Error reading T control (index: %d)", chunkIndex);
				break;

			case kChunkZ:
				switch (_channel) {
				default:
					if (!skipSound())
						error("[Movie::play] Chunk Z: Cannot skip sound (index: %d)", chunkIndex);
					break;

				case 0:
				case 1:
					if (!readSound())
						error("[Movie::play] Chunk Z: Cannot read sound (index: %d)", chunkIndex);

					if (setupSound) {
						if (_isSoundInitialized)
							_sound->play();

						setupSound = false;
					}
					break;
				}
				break;
			}

			// Stop after processing all chunks
			if (chunkIndex >= chunkCount)
				break;
		}
	}

	// Cleanup
cleanup:
	delete image;

	if (_isSoundInitialized)
		_sound->deinit();

	if (_hasDialog)
		getApp()->getDialogHandler()->removeDialog(500001);
}

#pragma region Sound

bool Movie::readSound() {
	if (!_image)
		error("[Movie::readSound] Image not initialized properly");

	Cinematic *cinematic = _image->getCinematic();
	if (!cinematic)
		error("[Movie::readSound] Cinematic not initialized properly");

	// Read sound data size
	uint32 soundSize = cinematic->readUint32LE();
	if (cinematic->err() || cinematic->eos()) {
		warning("[Movie::readSound] Error reading from file");
		deinit();
		return false;
	}

	debugC(kRingDebugMovie, "    Reading sound data (size: %d)", soundSize);

	// Check if there is any sound data
	if (!soundSize)
		return true;

	if (soundSize > 10000000) {
		warning("[Movie::readSound] Invalid sound data size (was: %d, max: 10000000)", soundSize);
		return false;
	}

	// Check remaining file size
	if (((uint32)cinematic->pos() + soundSize) >= (uint32)cinematic->size()) {
		warning("[Movie::readSound] Invalid sound data size (would read after end of file: %d)", soundSize);
		deinit();
		return false;
	}

	if (!_isSoundInitialized)
		return true;

	// Read sound data
	byte *soundBuffer = (byte *)calloc(soundSize, 1);
	if (!soundBuffer)
		error("[Movie::readSound] Cannot allocate sound buffer");

	cinematic->read(soundBuffer, soundSize);

	_sound->queueBuffer(new Common::MemoryReadStream(soundBuffer, soundSize, DisposeAfterUse::YES));

	return true;
}

bool Movie::skipSound() {
	Common::SeekableReadStream *cinematic = _image->getCinematic();
	if (!cinematic)
		error("[Movie::skipSound] Cinematic not initialized properly");

	// Read sound data offset
	uint32 offset = cinematic->readUint32LE();
	if (cinematic->err() || cinematic->eos()) {
		warning("[Movie::skipSound] Error reading from file");
		deinit();
		return false;
	}

	debugC(kRingDebugMovie, "    Skipping sound data (size: %d)", offset);

	// Check if there is any sound data
	if (!offset)
		return true;

	// Skip sound data
	cinematic->seek(offset, SEEK_CUR);
	if (cinematic->err() || cinematic->eos()) {
		warning("[Movie::skipSound] Error reading from file");
		deinit();
		return false;
	}

	return true;
}

#pragma endregion

#pragma region Frames

uint32 Movie::playNextFrame(const Common::Point &point, DrawType drawType) {
	error("[Movie::playNextFrame] Not implemented!");
}

uint32 Movie::getNumberOfFrames() {
	error("[Movie::getNumberOfFrames] Not implemented!");
}

void Movie::setSynchroOff() {
	error("[Movie::setSynchroOff] Not implemented!");
}

#pragma endregion

} // End of namespace Ring
