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

#include "fitd/fitd.h"
#include "fitd/game_time.h"
#include "fitd/gfx.h"
#include "fitd/tatou.h"
#include "fitd/hqr.h"
#include "fitd/vars.h"
#include "audio/decoders/raw.h"
#include "audio/decoders/voc.h"
#include "audio/mixer.h"
#include "common/events.h"
#include "common/memstream.h"
#include "graphics/framelimiter.h"

namespace Fitd {

static void computePalette(byte *inPalette, byte *outPalette, int coef) {
	for (int i = 0; i < 256; i++) {
		*(outPalette++) = ((*(inPalette++)) * coef) >> 8;
		*(outPalette++) = ((*(inPalette++)) * coef) >> 8;
		*(outPalette++) = ((*(inPalette++)) * coef) >> 8;
	}
}

void fadeInPhys(int step, int start) {
	byte localPalette[0x300];

	freezeTime();

	if (fadeState == 2) // only used for the ending ?
	{
	} else {
		for (int i = 0; i < 256; i += step) {
			process_events();
			computePalette(currentGamePalette, localPalette, i);
			gfx_setPalette(localPalette);
			gfx_refreshFrontTextureBuffer();
			osystem_drawBackground();
		}
	}

	fadeState = 1;

	unfreezeTime();
}

void fadeOutPhys(int var1, int var2) {
	byte localPalette[0x300];

	freezeTime();

	for (int i = 256; i >= 0; i -= var1) {
		process_events();
		computePalette(currentGamePalette, localPalette, i);
		gfx_setPalette(localPalette);
		gfx_refreshFrontTextureBuffer();
		osystem_drawBackground();
	}

	unfreezeTime();
}

void playSound(int num) {
	if (num == -1)
		return;

	byte *samplePtr = (byte *)HQR_Get(listSamp, num);
	Audio::SoundHandle handle;
	Common::MemoryReadStream *memStream = new Common::MemoryReadStream(samplePtr, 30834);
	Audio::SeekableAudioStream *voc = Audio::makeVOCStream(memStream, Audio::FLAG_UNSIGNED, DisposeAfterUse::YES);
	g_engine->_mixer->playStream(Audio::Mixer::kSFXSoundType, &handle, voc);
}

void paletteFill(void *palette, byte r, byte g, byte b) {
	unsigned char *paletteLocal = (unsigned char *)palette;
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

void startChrono(unsigned int *chrono) {
	*chrono = timer;
}

int evalChrono(unsigned int *chrono) {
	return (timer - *chrono);
}

} // namespace Fitd
