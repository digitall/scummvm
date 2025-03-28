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
#include "graphics/framelimiter.h"
#include "fitd/detection.h"
#include "fitd/console.h"
#include "fitd/gfx.h"
#include "fitd/tatou.h"
#include "common/scummsys.h"
#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/events.h"
#include "common/system.h"
#include "engines/util.h"
#include "graphics/paletteman.h"
#include "common/file.h"
#include "unpack.h"
#include "audio/mixer.h"
#include "audio/decoders/raw.h"
#include "audio/decoders/voc.h"
#include "common/memstream.h"

namespace Fitd {

FitdEngine *g_engine;

FitdEngine::FitdEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst),
																		   _gameDescription(gameDesc), _randomSource("Fitd") {
	g_engine = this;
}

FitdEngine::~FitdEngine() {
}

uint32 FitdEngine::getFeatures() const {
	return _gameDescription->flags;
}

Common::String FitdEngine::getGameId() const {
	return _gameDescription->gameId;
}

typedef struct pakInfoStruct // warning: allignement unsafe
{
	int32 discSize;
	int32 uncompressedSize;
	char compressionFlag;
	char info5;
	int16 offset;
} pakInfoStruct;

static void readPakInfo(pakInfoStruct *pPakInfo, Common::File &f) {
	pPakInfo->discSize = f.readSint32LE();
	pPakInfo->uncompressedSize = f.readSint32LE();
	pPakInfo->compressionFlag = f.readByte();
	pPakInfo->info5 = f.readByte();
	pPakInfo->offset = f.readSint16LE();
}

static char *openData(const char *fileName, int index) {
	Common::File f;
	f.open(fileName);
	f.readUint32LE();
	uint32 fileOffset = f.readUint32LE();
	uint32 numFiles = (fileOffset / 4) - 2;
	assert(index < numFiles);
	uint32 idOffset = (index + 1) * 4;
	f.seek(idOffset, SEEK_SET);
	fileOffset = f.readUint32LE();
	f.seek(fileOffset, SEEK_SET);
	uint32 additionalDescriptorSize = f.readUint32LE();
	if (additionalDescriptorSize) {
		f.seek(additionalDescriptorSize - 4, SEEK_CUR);
	}
	pakInfoStruct pakInfo;
	readPakInfo(&pakInfo, f);
	if (pakInfo.offset) {
		Common::String name = f.readString();
		debug("Loading %s\n", name.c_str());
	}

	char *ptr = 0;
	switch (pakInfo.compressionFlag) {
	case 0: {
		ptr = (char *)malloc(pakInfo.discSize);
		f.read(ptr, pakInfo.discSize);
		break;
	}
	case 1: {
		char *compressedDataPtr = (char *)malloc(pakInfo.discSize);
		f.read(compressedDataPtr, pakInfo.discSize);
		ptr = (char *)malloc(pakInfo.uncompressedSize);

		PAK_explode((unsigned char *)compressedDataPtr, (unsigned char *)ptr, pakInfo.discSize, pakInfo.uncompressedSize, pakInfo.info5);

		free(compressedDataPtr);
		break;
	}
	case 4: {
		char *compressedDataPtr = (char *)malloc(pakInfo.discSize);
		f.read(compressedDataPtr, pakInfo.discSize);
		ptr = (char *)malloc(pakInfo.uncompressedSize);

		PAK_deflate((unsigned char *)compressedDataPtr, (unsigned char *)ptr, pakInfo.discSize, pakInfo.uncompressedSize);

		free(compressedDataPtr);
		break;
	}
	default:
		assert(false);
		break;
	}
	return ptr;
}

void waitMs(uint timeMs) {
	uint time = 0;
	// Simple event handling loop
	Common::Event e;
	Graphics::FrameLimiter limiter(g_system, 25);
	while (!g_engine->shouldQuit() && time < timeMs) {
		while (g_system->getEventManager()->pollEvent(e)) {
		}

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		limiter.delayBeforeSwap();

		time += limiter.getLastFrameDuration();
		limiter.startFrame();
	}
}

static void playSound(int num) {
	byte *samplePtr = (byte *)openData("LISTSAMP.PAK", num);
	Audio::SoundHandle handle;
	Common::MemoryReadStream *memStream = new Common::MemoryReadStream(samplePtr, 30834);
	Audio::SeekableAudioStream *voc = Audio::makeVOCStream(memStream, Audio::FLAG_UNSIGNED, DisposeAfterUse::YES);
	g_engine->_mixer->playStream(Audio::Mixer::kSFXSoundType, &handle, voc);
}

static void clearScreenTatou(void) {
	int i;

	for (i = 0; i < 45120; i++) {
		frontBuffer[i] = 0;
	}
}

Common::Error FitdEngine::run() {
	initGraphics3d(320 * 4, 200 * 4);
	gfx_init();

	// Set the engine's debugger console
	setDebugger(new Console());

	// If a savegame was selected from the launcher, load it
	int saveSlot = ConfMan.getInt("save_slot");
	if (saveSlot != -1)
		(void)loadGameState(saveSlot);

	char *tatou3d = openData("ITD_RESS.PAK", 0);
	char *ptr = openData("ITD_RESS.PAK", 2);
	char *palPtr = openData("ITD_RESS.PAK", 1);

	setupCameraProjection(160, 100, 128, 500, 490);
	gfx_setPalette((const byte *)palPtr);
	memcpy(frontBuffer, ptr + 770, 64000);
	gfx_copyBlockPhys(frontBuffer, 0, 0, 320, 200);

	fadeInPhys(8, 0);
	waitMs(3000);

	playSound(6);

	clearScreenTatou();
	gfx_copyBlockPhys(frontBuffer, 0, 0, 320, 200);
	gfx_refreshFrontTextureBuffer();
	gfx_draw();
	g_system->updateScreen();

	int unk1 = 8;
	int rotation = 256;
	int time = 8920;
	setCameraTarget(0, 0, 0, unk1, rotation, 0, time);
	affObjet(0, 0, 0, 0, 0, 0, tatou3d);
	g_system->updateScreen();

	Graphics::FrameLimiter limiter(g_system, 25);
	Common::Event e;
	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(e)) {
		}
		time+=25;

		// if (time > 16000)
		// 	break;

		rotation -= 8;

		clearScreenTatou();
		gfx_copyBlockPhys(frontBuffer, 0, 0, 320, 200);
		gfx_refreshFrontTextureBuffer();

		setCameraTarget(0, 0, 0, unk1, rotation, 0, time);

		gfx_draw();
		affObjet(0, 0, 0, 0, 0, 0, tatou3d);
		g_system->updateScreen();

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		limiter.delayBeforeSwap();
		limiter.startFrame();
	}

	// Simple event handling loop
	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(e)) {
		}

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		limiter.delayBeforeSwap();
		limiter.startFrame();
	}

	return Common::kNoError;
}

Common::Error FitdEngine::syncGame(Common::Serializer &s) {
	// The Serializer has methods isLoading() and isSaving()
	// if you need to specific steps; for example setting
	// an array size after reading it's length, whereas
	// for saving it would write the existing array's length
	int dummy = 0;
	s.syncAsUint32LE(dummy);

	return Common::kNoError;
}

} // End of namespace Fitd
