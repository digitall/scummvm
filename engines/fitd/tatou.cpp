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

#include "fitd/tatou.h"
#include "audio/decoders/raw.h"
#include "audio/decoders/voc.h"
#include "audio/mixer.h"
#include "common/memstream.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/game_time.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/life.h"

namespace Fitd {

static void computePalette(byte *inPalette, byte *outPalette, int coef) {
	for (int i = 0; i < 256; i++) {
		*outPalette++ = (*inPalette++ * coef) >> 8;
		*outPalette++ = (*inPalette++ * coef) >> 8;
		*outPalette++ = (*inPalette++ * coef) >> 8;
	}
}

void fadeInPhys(int step, int start) {
	byte localPalette[0x300];

	freezeTime();

	if (g_engine->_engine->fadeState == 2) {
		// only used for the end sequence
		for (int i = 256; i >= 0; i -= 16) {
			process_events();
			paletteFill(localPalette, 0xFF, 0, 0);
			fadeLevelDestPal(g_engine->_engine->currentGamePalette, localPalette, i);
			gfx_setPalette(localPalette);
			gfx_refreshFrontTextureBuffer();
			drawBackground();
		}
	} else {
		for (int i = 0; i < 256; i += step) {
			process_events();
			computePalette(g_engine->_engine->currentGamePalette, localPalette, i);
			gfx_setPalette(localPalette);
			gfx_refreshFrontTextureBuffer();
			drawBackground();
		}
	}

	g_engine->_engine->fadeState = 1;

	unfreezeTime();
}

void fadeOutPhys(int var1, int var2) {
	byte localPalette[0x300];

	freezeTime();

	for (int i = 256; i >= 0; i -= var1) {
		process_events();
		computePalette(g_engine->_engine->currentGamePalette, localPalette, i);
		gfx_setPalette(localPalette);
		gfx_refreshFrontTextureBuffer();
		drawBackground();
	}

	unfreezeTime();
}

void playRepeatedSound(int num) {
	if (g_engine->_engine->lastSample == num)
		return;

	const int16 *priorities = (int16 *)g_engine->_engine->ptrPrioritySample;
	if (g_engine->_engine->lastPriority < priorities[num]) {
		g_engine->_engine->lastSample = num;
		g_engine->_engine->lastPriority = priorities[num];
		g_engine->_mixer->stopID(g_engine->_engine->lastSample);

		const byte *samplePtr = hqrGet(g_engine->_engine->listSamp, num);
		Audio::SoundHandle handle;
		Common::MemoryReadStream *memStream = new Common::MemoryReadStream(samplePtr, 30834);
		Audio::SeekableAudioStream *voc = Audio::makeVOCStream(memStream, Audio::FLAG_UNSIGNED, DisposeAfterUse::YES);
		Audio::makeLoopingAudioStream(voc, Audio::Timestamp(), voc->getLength(), 0);
		g_engine->_mixer->playStream(Audio::Mixer::kSFXSoundType, &handle, voc);
	}
}

void playSound(int num) {
	if (num == -1)
		return;

	const int16 *priorities = (int16 *)g_engine->_engine->ptrPrioritySample;
	if (g_engine->_engine->lastPriority < priorities[num]) {
		g_engine->_engine->lastSample = num;
		g_engine->_engine->lastPriority = priorities[num];
		g_engine->_mixer->stopID(g_engine->_engine->lastSample);
		const byte *samplePtr = hqrGet(g_engine->_engine->listSamp, num);
		Audio::SoundHandle handle;
		Common::MemoryReadStream *memStream = new Common::MemoryReadStream(samplePtr, 30834);
		Audio::SeekableAudioStream *voc = Audio::makeVOCStream(memStream, Audio::FLAG_UNSIGNED, DisposeAfterUse::YES);
		g_engine->_mixer->playStream(Audio::Mixer::kSFXSoundType, &handle, voc, num);
	}
}

void makeBlackPalette() {
	const byte pal[256 * 3] = {};
	gfx_setPalette(pal);
}

void paletteFill(void *palette, byte r, byte g, byte b) {
	byte *paletteLocal = static_cast<byte *>(palette);
	int offset = 0;

	r <<= 1;
	g <<= 1;
	b <<= 1;

	for (int i = 0; i < 256; i++) {
		paletteLocal[offset] = r;
		paletteLocal[offset + 1] = g;
		paletteLocal[offset + 2] = b;
		offset += 3;
	}
}

void copyPalette(byte *source, byte *dest) {
	for (int i = 0; i < 768; i++) {
		dest[i] = source[i];
	}
}

void fastCopyScreen(void *source, void *dest) {
	memcpy(dest, source, 64000);
}

void startChrono(uint *chrono) {
	*chrono = g_engine->_engine->timer;
}

int evalChrono(uint *chrono) {
	return g_engine->_engine->timer - *chrono;
}

} // namespace Fitd
