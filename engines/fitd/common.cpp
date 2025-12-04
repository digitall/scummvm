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

#include "audio/decoders/raw.h"
#include "audio/decoders/voc.h"
#include "audio/mixer.h"
#include "common/config-manager.h"
#include "common/debug.h"
#include "common/file.h"
#include "common/memstream.h"
#include "common/system.h"
#include "graphics/managed_surface.h"

#include "fitd/aitd1.h"
#include "fitd/aitd2.h"
#include "fitd/aitd3.h"
#include "fitd/aitd_box.h"
#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/debugtools.h"
#include "fitd/engine.h"
#include "fitd/file_access.h"
#include "fitd/fitd.h"
#include "fitd/floor.h"
#include "fitd/font.h"
#include "fitd/game_time.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/jack.h"
#include "fitd/life.h"
#include "fitd/lines.h"
#include "fitd/main_loop.h"
#include "fitd/music.h"
#include "fitd/object.h"
#include "fitd/pak.h"
#include "fitd/sequence.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"
#include "fitd/zv.h"
#include "graphics/screen.h"

namespace Fitd {

static const char *languageShortNameTable[LANGUAGE_NAME_SIZE] = {
	"fr",
	"it",
	"en",
	"es",
	"de",
};

static const char *languageNameTable[LANGUAGE_NAME_SIZE] = {
	"FRANCAIS.PAK",
	"ITALIANO.PAK",
	"ENGLISH.PAK",
	"ESPAGNOL.PAK",
	"DEUTSCH.PAK",
};

const char *listBodySelect[] = {
	"LISTBODY.PAK",
	"LISTBOD2.PAK",
};

const char *listAnimSelect[] = {
	"LISTANIM.PAK",
	"LISTANI2.PAK",
};

const byte defaultPalette[0x30] = {
	0x00,
	0x00,
	0x00,
	0x3F,
	0x3F,
	0x3F,
	0x0C,
	0x0C,
	0x0E,
	0x30,
	0x2F,
	0x3F,
	0x23,
	0x2C,
	0x23,
	0x2A,
	0x1D,
	0x2A,
	0x2A,
	0x21,
	0x18,
	0x3F,
	0x05,
	0x2A,
	0x12,
	0x14,
	0x18,
	0x31,
	0x15,
	0x17,
	0x15,
	0x25,
	0x15,
	0x15,
	0x2F,
	0x3F,
	0x3F,
	0x22,
	0x15,
	0x2B,
	0x15,
	0x3F,
	0x3F,
	0x3F,
	0x21,
	0x3F,
	0x3F,
	0x3F};

const byte defaultPaletteAITD3[0x30] = {
	0x00,
	0x00,
	0x00,
	0xFC,
	0xFC,
	0xFC,
	0x30,
	0x30,
	0x38,
	0xC0,
	0xBC,
	0xFC,
	0x78,
	0x58,
	0x3C,
	0x00,
	0x00,
	0x00,
	0xF0,
	0x70,
	0x10,
	0xFC,
	0xFC,
	0xFC,
	0x48,
	0x50,
	0x60,
	0xC4,
	0x54,
	0x5C,
	0x54,
	0x94,
	0x54,
	0x54,
	0xBC,
	0xFC,
	0xFC,
	0x88,
	0x54,
	0xAC,
	0x54,
	0xFC,
	0xFC,
	0xFC,
	0xFC,
	0xFC,
	0xFC,
	0xF8};

void executeFoundLife(int objIdx) {
	int lifeOffset = 0;

	if (objIdx == -1)
		return;

	const int foundLife = g_engine->_engine->worldObjets[objIdx].foundLife;

	if (g_engine->_engine->worldObjets[objIdx].foundLife == -1)
		return;

	Object *currentActorPtr = g_engine->_engine->currentProcessedActorPtr;
	const int currentActorIdx = g_engine->_engine->currentProcessedActorIdx;
	const int currentActorLifeIdx = g_engine->_engine->currentLifeActorIdx;
	Object *currentActorLifePtr = g_engine->_engine->currentLifeActorPtr;
	const int currentActorLifeNum = g_engine->_engine->currentLifeNum;

	if (g_engine->_engine->currentLifeNum != -1) {
		lifeOffset = (g_engine->_engine->currentLifePtr - hqrGet(g_engine->_engine->listLife, currentActorLifeNum).data) / 2;
	}

	int var_2 = 0;

	int actorIdx = g_engine->_engine->worldObjets[objIdx].objIndex;

	if (actorIdx == -1) {
		const Object *currentActorEntryPtr = &g_engine->_engine->objectTable[NUM_MAX_OBJECT - 1];
		int currentActorEntry = NUM_MAX_OBJECT - 1;

		while (currentActorEntry >= 0) {
			if (currentActorEntryPtr->indexInWorld == -1)
				break;

			currentActorEntryPtr--;
			currentActorEntry--;
		}

		if (currentActorEntry == -1) // no space, we will have to overwrite the last actor !
		{
			currentActorEntry = NUM_MAX_OBJECT - 1;
			currentActorEntryPtr = &g_engine->_engine->objectTable[NUM_MAX_OBJECT - 1];
		}

		actorIdx = currentActorEntry;
		var_2 = 1;

		g_engine->_engine->currentProcessedActorPtr = &g_engine->_engine->objectTable[actorIdx];
		g_engine->_engine->currentLifeActorPtr = &g_engine->_engine->objectTable[actorIdx];
		g_engine->_engine->currentProcessedActorIdx = actorIdx;
		g_engine->_engine->currentLifeActorIdx = actorIdx;

		g_engine->_engine->currentProcessedActorPtr->indexInWorld = objIdx;
		g_engine->_engine->currentProcessedActorPtr->life = -1;
		g_engine->_engine->currentProcessedActorPtr->bodyNum = -1;
		g_engine->_engine->currentProcessedActorPtr->_flags = 0;
		g_engine->_engine->currentProcessedActorPtr->trackMode = -1;
		g_engine->_engine->currentProcessedActorPtr->room = -1;
		g_engine->_engine->currentProcessedActorPtr->lifeMode = -1;
		g_engine->_engine->currentProcessedActorPtr->ANIM = -1;
	}

	processLife(foundLife, true);

	if (var_2) {
		g_engine->_engine->currentProcessedActorPtr->indexInWorld = -1;
	}

	g_engine->_engine->currentProcessedActorPtr = currentActorPtr;
	g_engine->_engine->currentProcessedActorIdx = currentActorIdx;
	g_engine->_engine->currentLifeActorIdx = currentActorLifeIdx;
	g_engine->_engine->currentLifeActorPtr = currentActorLifePtr;

	if (currentActorLifeNum != -1) {
		g_engine->_engine->currentLifeNum = currentActorLifeNum;
		g_engine->_engine->currentLifePtr = hqrGet(g_engine->_engine->listLife, g_engine->_engine->currentLifeNum).data + lifeOffset * 2;
	}
}

static void drawGradient(int x1, int x2) {
	int right = x1 + (x2 - x1) / 2;
	int left = x1 + 1;
	fillBox(left, 0, right, 199, 19);
	left = right;
	right += (x2 - right) / 2;
	fillBox(left, 0, right, 199, 20);
	left = right;
	right += (x2 - right) / 2;
	fillBox(left, 0, right, 199, 21);
	left = right;
	right += (x2 - right) / 2;
	fillBox(left, 0, right, 199, 22);
	left = right;
	right += (x2 - right) / 2;
	fillBox(left, 0, right, 199, 23);
	fillBox(right, 0, x2, 199, 24);
}

static void turnPageForward() {
	setClip(0, 0, 319, 199);
	gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
	byte *saveLogicalScreen = g_engine->_engine->logicalScreen;
	g_engine->_engine->logicalScreen = &g_engine->_engine->frontBuffer[0];
	g_engine->_engine->polyBackBuffer = &g_engine->_engine->frontBuffer[0];
	int i = 20;
	int left = 260;
	int right = 319;
	while (right > -1) {
		fastCopyScreen(saveLogicalScreen, g_engine->_engine->frontBuffer);
		right = 280 - (i / 2);
		line(left, 0, left, 199, 16);
		drawGradient(left + 1, right);
		if (right < -2) {
			copyBoxLogPhys(0, 0, right + 21, 199);
		} else {
			copyBoxLogPhys(left + 1, 0, right + 21, 199);
		}
		i += 10;
		left -= 10;
	}
	g_engine->_engine->logicalScreen = saveLogicalScreen;
}

static void turnPageBackward() {
	setClip(0, 0, 319, 199);

	byte *saveLogicalScreen = g_engine->_engine->logicalScreen;
	g_engine->_engine->logicalScreen = &g_engine->_engine->frontBuffer[0];
	g_engine->_engine->polyBackBuffer = &g_engine->_engine->frontBuffer[0];
	int si = -540;
	int di = 820;
	do {
		if (si >= 20) {
			copyBlock(saveLogicalScreen, g_engine->_engine->frontBuffer, 0, 0, si - 19, 199);
			copyBoxLogPhys(0, 0, 280, 199);
		}

		line(si, 0, si, 199, 16);
		drawGradient(si + 1, 280 - (di / 2));
		di -= 10;
		si += 10;
	} while (si < 260);
	copyBoxLogPhys(si - 20, 0, 319, 199);

	g_engine->_engine->logicalScreen = saveLogicalScreen;

	gfx_copyBlockPhys(saveLogicalScreen, 0, 0, 320, 200);
	drawBackground();
}

int lire(int index, int startx, int top, int endx, int bottom, int demoMode, int color, int perso) {
	bool lastPageReached = false;
	char tabString[] = "    ";
	int firstpage = 1;
	int page = 0;
	int quit = 0;
	int previousPage = -1;
	int var_1C3;
	char *ptrpage[100];
	int currentTextIdx;
	Audio::SoundHandle handle;

	extSetFont(g_engine->_engine->ptrFont, color);

	const int maxStringWidth = endx - startx + 4;

	const int textIndexMalloc = hqMalloc(g_engine->_engine->hqMemory, pakGetPakSize(g_engine->_engine->languageNameString, index) + 300);
	byte *textPtr = hqPtrMalloc(g_engine->_engine->hqMemory, textIndexMalloc);
	if (!textPtr)
		error("No memory left");

	if (!pakLoad(g_engine->_engine->languageNameString, index, textPtr)) {
		error("Failed to load pak %s", g_engine->_engine->languageNameString);
	}

	memset(ptrpage, 0, sizeof(char *) * 100);
	ptrpage[0] = (char *)textPtr;

	//  LastSample = -1;
	//  LastPriority = -1;

	while (!::Engine::shouldQuit() && !quit) {
		fastCopyScreen(g_engine->_engine->aux, g_engine->_engine->logicalScreen);
		process_events();
		setClip(startx, top, endx, bottom);

		char *ptrt = ptrpage[page];

		int currentTextY = top;
		lastPageReached = false;

		while (currentTextY <= bottom - 16) {
			int line_type = 1;
			int var_1BA = 0;
			int currentStringWidth;
			int currentTextX;

			RegularTextEntry *currentText = g_engine->_engine->textTable;

			int numWordInLine = 0;

			int interWordSpace = 0;

			while (!::Engine::shouldQuit()) {
				while (*ptrt == '#') {
					// char* var_1BE = var_1C2;
					ptrt++;

					switch (*ptrt++) {
					case 'P': // page change
					{
						if (currentTextY > top) // Hu ?
							goto pageChange;
						break;
					}
					case 'T': // tab
					{
						currentText->textPtr = tabString;
						currentText->width = extGetSizeFont(currentText->textPtr) + 3;
						var_1BA += currentText->width;
						numWordInLine++;
						currentText++;
						break;
					}
					case 'C': // center
					{
						line_type &= 0xFFFE;
						line_type |= 8;
						break;
					}
					case 'G': // print number
					{
						currentTextIdx = 0;

						while (*ptrt >= '0' && *ptrt <= '9') {
							currentTextIdx = currentTextIdx * 10 + *ptrt - 48;
							ptrt++;
						}

						if (pakLoad("ITD_RESS.PAK", AITD1_TEXT_GRAPH, g_engine->_engine->aux2)) {
							assert(0); // when is this used?
									   /*  var_C = printTextSub3(currentTextIdx,g_engine->_engine->aux2);
									   var_A = printTextSub4(currentTextIdx,g_engine->_engine->aux2);

									   if(currentTextY + var_A > bottom)
									   {
									   var_1C2 = var_1BE;

									   goto pageChange;
									   }
									   else
									   {
									   printTextSub5((((right-left)/2)+left)-var_C, currentTextY, currentTextIdx, g_engine->_engine->aux2);
									   currentTextY = var_A;
									   }*/
						}

						break;
					}
					}
				}

				currentText->textPtr = ptrt;

				do {
					var_1C3 = *(byte *)ptrt++;
				} while (var_1C3 > ' '); // go to the end of the string

				*(ptrt - 1) = 0; // add end of string marker to cut the word

				currentStringWidth = extGetSizeFont(currentText->textPtr) + 3;

				if (currentStringWidth > maxStringWidth) {
					quit = 1;
					break;
				}

				if (var_1BA + currentStringWidth > maxStringWidth) {
					ptrt = currentText->textPtr;
					break;
				}

				currentText->width = currentStringWidth;
				var_1BA += currentStringWidth;

				numWordInLine++;
				currentText++;

				// eval the character that caused the 'end of word' state
				if (var_1C3 == 26) {
					line_type &= 0xFFFE;
					line_type |= 4;
					lastPageReached = true;
					break;
				}

				if ((var_1C3 == 13 || var_1C3 == 0) && *ptrt < ' ') {
					++ptrt;
					if (*ptrt == 0xD) {
						ptrt += 2;
						line_type &= 0xFFFE;
						line_type |= 2;
						break;
					}
					if (*ptrt == '#') {
						line_type &= 0xFFFE;
						break;
					}
				}
			}

			if (line_type & 1) // stretch words on line
			{
				interWordSpace = (maxStringWidth - var_1BA) / (numWordInLine - 1);
			}

			currentText = g_engine->_engine->textTable;

			if (line_type & 8) // center
			{
				currentTextX = startx + (maxStringWidth - var_1BA) / 2;
			} else {
				currentTextX = startx;
			}

			for (int i = 0; i < numWordInLine; i++) {
				renderText(currentTextX, currentTextY, currentText->textPtr);
				currentTextX += currentText->width + interWordSpace; // add inter word space
				currentText++;
			}
			currentTextIdx = 0;

			if (line_type & 2) // font size
			{
				currentTextY += 8;
			}

			currentTextY += 16;

			if (lastPageReached)
				break;
		}

	pageChange:
		if (lastPageReached) {
			*(ptrt - 1) = 0x1A; // rewrite End Of Text
		} else {
			ptrpage[page + 1] = ptrt;
		}

		if (demoMode == 0) {
			if (page > 0) {
				affSpfI(startx - 19, 185, 12, g_engine->_engine->ptrCadre);
			}

			if (!lastPageReached) {
				affSpfI(endx + 4, 185, 11, g_engine->_engine->ptrCadre);
			}
		}

		if (demoMode == 2) {
			if (page > 0) {
				affSpfI(startx - 3, 191, 13, g_engine->_engine->ptrCadre);
			}

			if (!lastPageReached) {
				affSpfI(endx - 10, 191, 14, g_engine->_engine->ptrCadre);
			}
		}

		if (firstpage) {
			if (demoMode != 1) {
				gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
				fadeInPhys(16, 0);
			} else {
				if (g_engine->_engine->turnPageFlag) {
					turnPageForward();
				} else {
					gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
					drawBackground();
				}
			}

			firstpage = 0;
		} else {
			if (g_engine->_engine->turnPageFlag) {
				if (previousPage < page) {
					turnPageForward();
				} else {
					turnPageBackward();
				}
			} else {
				gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
				drawBackground();
			}
		}

		if (perso != -1) {
			Audio::QueuingAudioStream *queuing_audio_stream = nullptr;
			int part = 0;
			while (true) {
				g_engine->_mixer->stopHandle(handle);
				Common::String fileName(Common::String::format("%02d%02d%02d.VOC", perso, page, part++));
				Common::File *f = new Common::File();
				f->open(fileName.c_str());
				if (!f->isOpen())
					break;
				Audio::SeekableAudioStream *voc = Audio::makeVOCStream(f, Audio::FLAG_UNSIGNED, DisposeAfterUse::YES);
				if (!queuing_audio_stream) {
					queuing_audio_stream = Audio::makeQueuingAudioStream(voc->getRate(), voc->isStereo());
				}
				queuing_audio_stream->queueAudioStream(voc);
			}
			g_engine->_mixer->playStream(Audio::Mixer::kSpeechSoundType, &handle, queuing_audio_stream);
		}

		if (demoMode != 1) // mode != 1: normal behavior (user can flip pages)
		{
			do {
				process_events();
			} while (g_engine->_engine->key || g_engine->_engine->joyD || g_engine->_engine->click);

			while (!::Engine::shouldQuit()) {
				process_events();
				g_engine->_engine->localKey = g_engine->_engine->key;
				g_engine->_engine->localJoyD = g_engine->_engine->joyD;
				g_engine->_engine->localClick = g_engine->_engine->click;

				if (g_engine->_engine->localKey == 1 || g_engine->_engine->localClick) {
					quit = 1;
					break;
				}

				if (demoMode == 2 && g_engine->_engine->localKey == 0x1C) {
					quit = 1;
					break;
				}

				// flip to next page
				if (g_engine->_engine->joyD & 0xA || g_engine->_engine->localKey == 0x1C) {
					if (!lastPageReached) {
						previousPage = page;
						page++;

						if (demoMode == 2) {
							playSound(g_engine->_engine->cVars[getCVarsIdx(SAMPLE_PAGE)]);
							g_engine->_engine->lastSample = -1;
							g_engine->_engine->lastPriority = -1;
						}
						break;
					}
					if (g_engine->_engine->localKey == 0x1C) {
						quit = 1;
						break;
					}
				}

				// flip to previous page
				if (g_engine->_engine->joyD & 5) {
					if (page > 0) {
						previousPage = page;
						page--;
						if (demoMode == 2) {
							playSound(g_engine->_engine->cVars[getCVarsIdx(SAMPLE_PAGE)]);
							g_engine->_engine->lastSample = -1;
							g_engine->_engine->lastPriority = -1;
						}
						break;
					}
				}
			}
		} else // Demo mode: pages automatically flips
		{
			uint var_6;
			startChrono(&var_6);

			do {
				process_events();
				if (evalChrono(&var_6) > 300) {
					break;
				}
			} while (!g_engine->_engine->key && !g_engine->_engine->click);

			if (g_engine->_engine->key || g_engine->_engine->click) {
				quit = 1;
			}

			if (!lastPageReached) {
				page++;
				playSound(g_engine->_engine->cVars[getCVarsIdx(SAMPLE_PAGE)]);
				g_engine->_engine->lastSample = -1;
			} else {
				quit = 1;
				demoMode = 0;
			}
		}
	}

	g_engine->_mixer->stopHandle(handle);
	// HQ_Free_Malloc(HQ_Memory, textIndexMalloc);

	return demoMode;
}

void initEngine() {
	Common::File f;
	if (!f.open("OBJETS.ITD")) {
		error("Fitd::initEngine: can't open OBJETS.ITD");
	}

	g_engine->_engine->maxObjects = f.readSint16LE();

	if (g_engine->getGameId() == GID_AITD1 || g_engine->getGameId() == GID_JACK) {
		g_engine->_engine->worldObjets.resize(300);
	} else {
		g_engine->_engine->worldObjets.resize(g_engine->_engine->maxObjects);
	}

	for (int i = 0; i < g_engine->_engine->maxObjects; i++) {
		g_engine->_engine->worldObjets[i].objIndex = f.readUint16LE();
		g_engine->_engine->worldObjets[i].body = f.readUint16LE();
		g_engine->_engine->worldObjets[i].flags = f.readUint16LE();
		g_engine->_engine->worldObjets[i].typeZV = f.readUint16LE();
		g_engine->_engine->worldObjets[i].foundBody = f.readUint16LE();
		g_engine->_engine->worldObjets[i].foundName = f.readUint16LE();
		g_engine->_engine->worldObjets[i].flags2 = f.readUint16LE();
		g_engine->_engine->worldObjets[i].foundLife = f.readUint16LE();
		g_engine->_engine->worldObjets[i].x = f.readUint16LE();
		g_engine->_engine->worldObjets[i].y = f.readUint16LE();
		g_engine->_engine->worldObjets[i].z = f.readUint16LE();
		g_engine->_engine->worldObjets[i].alpha = f.readUint16LE();
		g_engine->_engine->worldObjets[i].beta = f.readUint16LE();
		g_engine->_engine->worldObjets[i].gamma = f.readUint16LE();
		g_engine->_engine->worldObjets[i].stage = f.readUint16LE();
		g_engine->_engine->worldObjets[i].room = f.readUint16LE();
		g_engine->_engine->worldObjets[i].lifeMode = f.readUint16LE();
		g_engine->_engine->worldObjets[i].life = f.readUint16LE();
		g_engine->_engine->worldObjets[i].floorLife = f.readUint16LE();
		g_engine->_engine->worldObjets[i].anim = f.readUint16LE();
		g_engine->_engine->worldObjets[i].frame = f.readUint16LE();
		g_engine->_engine->worldObjets[i].animType = f.readUint16LE();
		g_engine->_engine->worldObjets[i].animInfo = f.readUint16LE();
		g_engine->_engine->worldObjets[i].trackMode = f.readUint16LE();
		g_engine->_engine->worldObjets[i].trackNumber = f.readUint16LE();
		g_engine->_engine->worldObjets[i].positionInTrack = f.readUint16LE();

		if (g_engine->getGameId() >= GID_JACK) {
			g_engine->_engine->worldObjets[i].mark = f.readUint16LE();
		}
		g_engine->_engine->worldObjets[i].flags |= 0x20;
	}
	f.close();

	g_engine->_engine->varSize = loadFromItd("VARS.ITD", g_engine->_engine->vars, sizeof(g_engine->_engine->vars));

	int16 choosePersoBackup = 0;
	if (g_engine->getGameId() == GID_AITD1) {
		choosePersoBackup = g_engine->_engine->cVars[getCVarsIdx(CHOOSE_PERSO)]; // backup hero selection
	}

	if (!f.open("DEFINES.ITD")) {
		error("Fitd::initEngine: can't open DEFINES.ITD");
	}

	///////////////////////////////////////////////
	f.read(&g_engine->_engine->cVars[0], g_engine->_engine->cVarsSize * 2);
	f.close();

	for (int i = 0; i < g_engine->_engine->cVarsSize; i++) {
		g_engine->_engine->cVars[i] = (g_engine->_engine->cVars[i] & 0xFF) << 8 | (g_engine->_engine->cVars[i] & 0xFF00) >> 8;
	}
	//////////////////////////////////////////////

	if (g_engine->getGameId() == GID_AITD1) {
		g_engine->_engine->cVars[getCVarsIdx(CHOOSE_PERSO)] = choosePersoBackup;
	}

	hqrFree(g_engine->_engine->listLife);
	hqrFree(g_engine->_engine->listTrack);
	hqrFree(g_engine->_engine->listBody);
	hqrFree(g_engine->_engine->listAnim);

	g_engine->_engine->listLife = hqrInitRessource("LISTLIFE.PAK", 65000, 100);
	g_engine->_engine->listTrack = hqrInitRessource("LISTTRAK.PAK", 20000, 100);

	if (g_engine->getGameId() == GID_AITD1) {
		g_engine->_engine->listBody = hqrInitRessource(listBodySelect[g_engine->_engine->cVars[getCVarsIdx(CHOOSE_PERSO)]], 37000, 50); // was calculated from free mem size
		g_engine->_engine->listAnim = hqrInitRessource(listAnimSelect[g_engine->_engine->cVars[getCVarsIdx(CHOOSE_PERSO)]], 30000, 80); // was calculated from free mem size
	} else {
		g_engine->_engine->listBody = hqrInitRessource("LISTBODY.PAK", 37000, 50); // was calculated from free mem size
		g_engine->_engine->listAnim = hqrInitRessource("LISTANIM.PAK", 30000, 80); // was calculated from free mem size

		hqrFree(g_engine->_engine->listMatrix);
		g_engine->_engine->listMatrix = hqrInitRessource("LISTMAT.PAK", 64000, 5);
	}
	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		g_engine->_engine->objectTable[i].indexInWorld = -1;
	}

	if (g_engine->getGameId() == GID_AITD1) {
		g_engine->_engine->currentWorldTarget = g_engine->_engine->cVars[getCVarsIdx(WORLD_NUM_PERSO)];
	}
}

static void clearMessageList() {
	for (int i = 0; i < 5; i++) {
		g_engine->_engine->messageTable[i].string = nullptr;
	}
}

void initVars() {
	g_engine->_engine->flagGameOver = 0;

	g_engine->_engine->numObjInInventoryTable = 0;
	g_engine->_engine->inHandTable = -1;

	g_engine->_engine->action = 0;

	g_engine->_engine->listPhysBox = g_engine->_engine->listBox1;
	g_engine->_engine->listLogBox = g_engine->_engine->listBox2;

	g_engine->_engine->nbPhysBoxs = 0;
	g_engine->_engine->nbLogBoxs = 0;

	g_engine->_engine->lastSample = -1;
	g_engine->_engine->nextSample = -1;
	g_engine->_engine->lastPriority = -1;
	g_engine->_engine->currentMusic = -1;
	g_engine->_engine->nextMusic = -1;

	g_engine->_engine->lightOff = 0;
	g_engine->_engine->newFlagLight = 0;

	g_engine->_engine->currentCameraTargetActor = -1;
	g_engine->_engine->currentWorldTarget = -1;

	g_engine->_engine->statusScreenAllowed = 1;

	clearMessageList();

	g_engine->_canSaveGame = true;
}

static void loadCamera(int cameraIdx) {

	Common::String name = Common::String::format("CAMERA%02d.PAK", g_engine->_engine->currentFloor);

	if (g_engine->getGameId() == GID_AITD1) {
		int useSpecial = -1;
		if (g_engine->_engine->cVars[getCVarsIdx(KILLED_SORCERER)] == 1) {
			switch (g_engine->_engine->currentFloor) {
			case 6: {
				if (cameraIdx == 0) {
					useSpecial = AITD1_CAM06000;
				}
				if (cameraIdx == 5) {
					useSpecial = AITD1_CAM06005;
				}
				if (cameraIdx == 8) {
					useSpecial = AITD1_CAM06008;
				}
				break;
			}
			case 7: {
				if (cameraIdx == 0) {
					useSpecial = AITD1_CAM07000;
				}
				if (cameraIdx == 1) {
					useSpecial = AITD1_CAM07001;
				}
				break;
			}
			}
		}

		if (useSpecial != -1) {
			name = "ITD_RESS.PAK";
			cameraIdx = useSpecial;
		}
	}

	if (!pakLoad(name.c_str(), cameraIdx, g_engine->_engine->aux)) {
		error("Cannot load pak %s", name.c_str());
	}

	if (g_engine->getGameId() == GID_AITD3) {
		// memmove(g_engine->_engine->aux, g_engine->_engine->aux + 4, 64000 + 0x300);
	}

	if (g_engine->getGameId() >= GID_JACK) {
		copyPalette(g_engine->_engine->aux + 64000, g_engine->_engine->currentGamePalette);

		if (g_engine->getGameId() == GID_AITD3) {
			for (int i = 0; i < 16; i++) {
				g_engine->_engine->currentGamePalette[i * 3 + 0] = defaultPaletteAITD3[i * 3 + 0];
				g_engine->_engine->currentGamePalette[i * 3 + 1] = defaultPaletteAITD3[i * 3 + 1];
				g_engine->_engine->currentGamePalette[i * 3 + 2] = defaultPaletteAITD3[i * 3 + 2];
			}
		} else {
			memcpy(g_engine->_engine->currentGamePalette, defaultPalette, 0x30);
		}
		convertPaletteIfRequired(g_engine->_engine->currentGamePalette);

		gfx_setPalette(g_engine->_engine->currentGamePalette);
	}
}

struct maskStruct {
	uint16 x1;
	uint16 y1;
	uint16 x2;
	uint16 y2;
	uint16 deltaX;
	uint16 deltaY;

	uint8 mask[320 * 200];
};

static maskStruct maskBuffers[10][10];

static void loadMask(int cameraIdx) {
	if (g_engine->getGameId() == GID_TIMEGATE)
		return;

	const Common::String name = Common::String::format("MASK%02d.PAK", g_engine->_engine->currentFloor);

	ScopedPtr pMask(pakLoad(name.c_str(), cameraIdx));

	for (int i = 0; i < g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera]->numViewedRooms; i++) {
		const CameraViewedRoom *pRoomView = &g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera]->viewedRoomTable[i];
		const byte *pViewedRoomMask = pMask.get() + READ_LE_U32(pMask.get() + i * 4);

		for (int j = 0; j < pRoomView->numMask; j++) {
			const byte *pMaskData = pViewedRoomMask + READ_LE_U32(pViewedRoomMask + j * 4);

			maskStruct *pDestMask = &maskBuffers[i][j];

			memset(pDestMask->mask, 0, 320 * 200);

			pDestMask->x1 = READ_LE_U16(pMaskData);
			pMaskData += 2;
			pDestMask->y1 = READ_LE_U16(pMaskData);
			pMaskData += 2;
			pDestMask->x2 = READ_LE_U16(pMaskData);
			pMaskData += 2;
			pDestMask->y2 = READ_LE_U16(pMaskData);
			pMaskData += 2;
			pDestMask->deltaX = READ_LE_U16(pMaskData);
			pMaskData += 2;
			pDestMask->deltaY = READ_LE_U16(pMaskData);
			pMaskData += 2;

			assert(pDestMask->deltaX == pDestMask->x2 - pDestMask->x1 + 1);
			assert(pDestMask->deltaY == pDestMask->y2 - pDestMask->y1 + 1);

			for (int k = 0; k < pDestMask->deltaY; k++) {
				const uint16 uNumEntryForLine = READ_LE_U16(pMaskData);
				pMaskData += 2;

				int offset = pDestMask->x1 + pDestMask->y1 * 320 + k * 320;

				for (int l = 0; l < uNumEntryForLine; l++) {
					const byte uNumSkip = *pMaskData++;
					const byte uNumCopy = *pMaskData++;

					offset += uNumSkip;

					for (int m = 0; m < uNumCopy; m++) {
						pDestMask->mask[offset] = 0xFF;
						offset++;
					}
				}
			}

			createMask(pDestMask->mask, i, j, g_engine->_engine->aux, pDestMask->x1, pDestMask->y1, pDestMask->x2, pDestMask->y2);
		}
	}
}

static void createAITD1Mask() {
	for (int viewedRoomIdx = 0; viewedRoomIdx < g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera]->numViewedRooms; viewedRoomIdx++) {
		const CameraViewedRoom *pcameraViewedRoomData = &g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera]->viewedRoomTable[viewedRoomIdx];

		byte *dataStart = g_engine->_engine->roomPtrCamera[g_engine->_engine->currentCamera] + pcameraViewedRoomData->offsetToMask;
		byte *data = dataStart;
		data += 2;

		const int numMask = *(int16 *)dataStart;

		Graphics::ManagedSurface s;
		s.create(320, 200, Graphics::PixelFormat::createFormatCLUT8());
		const byte colorMask = 0xFF;
		byte pal[256 * 3] = {};
		pal[255 * 3 + 0] = 0xFF;
		pal[255 * 3 + 1] = 0xFF;
		pal[255 * 3 + 2] = 0xFF;

		for (int maskIdx = 0; maskIdx < numMask; maskIdx++) {
			maskStruct *pDestMask = &maskBuffers[viewedRoomIdx][maskIdx];
			byte *src = dataStart + *(uint16 *)(data + 2);
			const int numMaskPoly = *(int16 *)src;
			src += 2;

			int minX = 319;
			int maxX = 0;
			int minY = 199;
			int maxY = 0;

			s.clear();
			for (int maskPolyIdx = 0; maskPolyIdx < numMaskPoly; maskPolyIdx++) {
				const int numPoints = *(int16 *)src;
				src += 2;

				switch (numPoints) {
				case 1: {
					const int16 x = *(int16 *)(src + 0);
					const int16 y = *(int16 *)(src + 2);
					s.setPixel(x, y, colorMask);
					break;
				}
				case 2: {
					const int16 x1 = *(int16 *)(src + 0);
					const int16 y1 = *(int16 *)(src + 2);
					const int16 x2 = *(int16 *)(src + 4);
					const int16 y2 = *(int16 *)(src + 6);
					s.drawLine(x1, y1, x2, y2, colorMask);
					break;
				}
				default: {
					Common::Array<int> x;
					Common::Array<int> y;
					x.resize(numPoints);
					y.resize(numPoints);
					for (int verticeId = 0; verticeId < numPoints; verticeId++) {
						const int16 verticeX = *(int16 *)(src + verticeId * 4 + 0);
						const int16 verticeY = *(int16 *)(src + verticeId * 4 + 2);
						x[verticeId] = verticeX;
						y[verticeId] = verticeY;

						minX = MIN(minX, static_cast<int>(verticeX));
						minY = MIN(minY, static_cast<int>(verticeY));
						maxX = MAX(maxX, static_cast<int>(verticeX));
						maxY = MAX(maxY, static_cast<int>(verticeY));
					}

					// draw polygon
					s.drawPolygonScan(x.data(), y.data(), numPoints, Common::Rect(0, 0, 319, 199), colorMask);
					for (int i = 0; i < numPoints - 1; ++i) {
						s.drawLine(x[i], y[i], x[i + 1], y[i + 1], colorMask);
					}

					// draw polygon contour
					s.drawLine(x[numPoints - 1], y[numPoints - 1], x[0], y[0], colorMask);
					break;
				}
				}

				src += numPoints * 4;
			}

			memcpy(pDestMask->mask, s.getBasePtr(0, 0), 320 * 200);
			createMask(pDestMask->mask, viewedRoomIdx, maskIdx, g_engine->_engine->aux, minX, minY, maxX, maxY);

			const int numOverlay = *(int16 *)data;
			data += 2;
			data += (numOverlay * 4 + 1) * 2;
		}
	}

	g_engine->_engine->polyBackBuffer = nullptr;
}

static int isInViewList(int value) {
	for (uint i = 0; i < ARRAYSIZE(g_engine->_engine->currentCameraVisibilityList); i++) {
		int var = g_engine->_engine->currentCameraVisibilityList[i];
		if (var == -1)
			break;
		if (var == value)
			return 1;
	}
	return 0;
}

// setup visibility list
static void setupCameraSub1() {

	byte *dataTabPos = g_engine->_engine->currentCameraVisibilityList;

	*dataTabPos = UINT8_MAX;

	// visibility list: add linked rooms
	for (uint32 i = 0; i < g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].numSceZone; i++) {
		if (g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].sceZoneTable[i].type == 0) {
			const int var_10 = g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].sceZoneTable[i].parameter;
			if (!isInViewList(var_10)) {
				*dataTabPos++ = var_10;
				*dataTabPos = UINT8_MAX;
			}
		}
	}

	// visibility list: add room seen by the current camera
	for (int j = 0; j < g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera]->numViewedRooms; j++) {
		if (!isInViewList(g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera]->viewedRoomTable[j].viewedRoomIdx)) {
			*dataTabPos++ = static_cast<char>(g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera]->viewedRoomTable[j].viewedRoomIdx);
			*dataTabPos = UINT8_MAX;
		}
	}
}

static void deleteObjet(int index) // remove actor
{
	Object *actorPtr = &g_engine->_engine->objectTable[index];

	if (actorPtr->indexInWorld == -2) // flow
	{
		actorPtr->indexInWorld = -1;

		if (actorPtr->ANIM == 4) {
			g_engine->_engine->cVars[getCVarsIdx(FOG_FLAG)] = 0;
		}

		// HQ_Free_Malloc(HQ_Memory,actorPtr->FRAME);
	} else {
		if (actorPtr->indexInWorld >= 0) {
			WorldObject *objectPtr = &g_engine->_engine->worldObjets[actorPtr->indexInWorld];

			objectPtr->objIndex = -1;
			actorPtr->indexInWorld = -1;

			objectPtr->body = actorPtr->bodyNum;
			objectPtr->anim = actorPtr->ANIM;
			objectPtr->frame = actorPtr->FRAME;
			objectPtr->animType = actorPtr->animType;
			objectPtr->animInfo = actorPtr->animInfo;
			objectPtr->flags = actorPtr->_flags & ~AF_BOXIFY;
			objectPtr->flags |= (AF_SPECIAL * actorPtr->dynFlags); // ugly hack, need rewrite
			objectPtr->life = actorPtr->life;
			objectPtr->lifeMode = actorPtr->lifeMode;
			objectPtr->trackMode = actorPtr->trackMode;

			if (objectPtr->trackMode) {
				objectPtr->trackNumber = actorPtr->trackNumber;
				objectPtr->positionInTrack = actorPtr->positionInTrack;
				if (g_engine->getGameId() != GID_AITD1) {
					objectPtr->mark = actorPtr->MARK;
				}
			}

			objectPtr->x = actorPtr->roomX + actorPtr->stepX;
			objectPtr->y = actorPtr->roomY + actorPtr->stepY;
			objectPtr->z = actorPtr->roomZ + actorPtr->stepZ;

			objectPtr->alpha = actorPtr->alpha;
			objectPtr->beta = actorPtr->beta;
			objectPtr->gamma = actorPtr->gamma;

			objectPtr->stage = actorPtr->stage;
			objectPtr->room = actorPtr->room;

			g_engine->_engine->actorTurnedToObj = 1;
		}
	}
}

void refreshAux2() {
	fastCopyScreen(g_engine->_engine->aux, g_engine->_engine->aux2);

	// TODO: implementer la suite
}

void setMoveMode(int trackMode, int trackNumber) {
	g_engine->_engine->currentProcessedActorPtr->trackMode = trackMode;

	switch (trackMode) {
	case 2: {
		g_engine->_engine->currentProcessedActorPtr->trackNumber = trackNumber;
		g_engine->_engine->currentProcessedActorPtr->MARK = -1;
		break;
	}
	case 3: {
		g_engine->_engine->currentProcessedActorPtr->trackNumber = trackNumber;
		g_engine->_engine->currentProcessedActorPtr->positionInTrack = 0;
		g_engine->_engine->currentProcessedActorPtr->MARK = -1;
		break;
	}
	}
}

int16 cameraVisibilityVar = 0;

int IsInCamera(int roomNumber) {
	const int numZone = g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera]->numViewedRooms;

	for (int i = 0; i < numZone; i++) {
		if (g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera]->viewedRoomTable[i].viewedRoomIdx == roomNumber) {
			cameraVisibilityVar = i;
			return 1;
		}
	}

	cameraVisibilityVar = -1;

	return 0;
}

int IsInCamRectTestAITD2(int X, int Z) // TODO: not 100% exact
{
	// if(changeCameraSub1(X,X,Z,Z,&cameraDataTable[g_engine->_engine->currentCamera]->cameraZoneDefTable[cameraVisibilityVar]))
	return 1;
	return 0;
}

int updateActorAitd2Only(int actorIdx) {
	Object *currentActor = &g_engine->_engine->objectTable[actorIdx];

	if (g_engine->getGameId() == GID_AITD1) {
		return 0;
	}

	if (currentActor->bodyNum != -1) {
		if (IsInCamera(currentActor->room)) {
			if (IsInCamRectTestAITD2(currentActor->roomX + currentActor->stepX, currentActor->roomZ + currentActor->stepZ)) {
				currentActor->lifeMode |= 4;
				return 1;
			}
		}
	}

	return 0;
}

void updateAllActorAndObjectsAITD2() {
	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		Object *pObject = &g_engine->_engine->objectTable[i];

		if (pObject->indexInWorld == -1) {
			continue;
		}

		pObject->lifeMode &= ~4;

		if (pObject->stage == g_engine->_engine->currentFloor) {
			switch (pObject->lifeMode) {
			case 0: // OFF
				break;
			case 1: // STAGE
				continue;
			case 2: // ROOM
				if (pObject->room == g_engine->_engine->currentRoom) {
					continue;
				}
				break;
			case 3: // CAMERA
				if (isInViewList(pObject->room)) {
					continue;
				}
				break;
			default:
				// assert(0);
				break;
			}

			if (updateActorAitd2Only(i)) {
				pObject->lifeMode |= 4;
				continue;
			}
		}
		deleteObjet(i);
	}

	for (int i = 0; i < g_engine->_engine->maxObjects; i++) {
		WorldObject *currentObject = &g_engine->_engine->worldObjets[i];

		if (currentObject->objIndex != -1) {
			if (g_engine->_engine->currentWorldTarget == i) {
				g_engine->_engine->currentCameraTargetActor = currentObject->objIndex;
			}
		} else {
			if (currentObject->stage == g_engine->_engine->currentFloor) {
				if (currentObject->life != -1) {
					if (currentObject->lifeMode != -1) {
						int di;

						switch (currentObject->lifeMode & 3) {
						case 0: {
							di = 0;
							break;
						}
						case 1: {
							di = 1;
							break;
						}
						case 2: {
							if (currentObject->room != g_engine->_engine->currentRoom) {
								di = 0;
							} else {
								di = 1;
							}
							break;
						}
						case 3: {
							if (!isInViewList(currentObject->room)) {
								di = 0;
							} else {
								di = 1;
							}
							break;
						}
						default: {
							di = 0;
							break;
						}
						}

						if (!di) {
							if (currentObject->body != -1) {
								if (IsInCamera(currentObject->room)) {
									if (IsInCamRectTestAITD2(currentObject->x, currentObject->z)) {
										currentObject->lifeMode |= 4;
									} else {
										continue;
									}
								} else {
									continue;
								}
							} else {
								continue;
							}
						}

						// int var_C = currentObject->flags & 0xFFDF;
						// int var_E = currentObject->field_2;
						// int var_A = currentObject->anim;
					addObject:
						const int actorIdx = copyObjectToActor(currentObject->body, currentObject->typeZV, currentObject->foundName,
															   currentObject->flags & 0xFFDF,
															   currentObject->x, currentObject->y, currentObject->z,
															   currentObject->stage, currentObject->room,
															   currentObject->alpha, currentObject->beta, currentObject->gamma,
															   currentObject->anim,
															   currentObject->frame, currentObject->animType, currentObject->animInfo);

						currentObject->objIndex = actorIdx;

						if (actorIdx != -1) {
							g_engine->_engine->currentProcessedActorPtr = &g_engine->_engine->objectTable[actorIdx];
							g_engine->_engine->currentProcessedActorIdx = actorIdx;

							if (g_engine->_engine->currentWorldTarget == i) {
								g_engine->_engine->currentCameraTargetActor = g_engine->_engine->currentProcessedActorIdx;
							}

							g_engine->_engine->currentProcessedActorPtr->dynFlags = (currentObject->flags & 0x20) / 0x20; // recheck
							g_engine->_engine->currentProcessedActorPtr->life = currentObject->life;
							g_engine->_engine->currentProcessedActorPtr->lifeMode = currentObject->lifeMode;

							g_engine->_engine->currentProcessedActorPtr->indexInWorld = i;

							setMoveMode(currentObject->trackMode, currentObject->trackNumber);

							g_engine->_engine->currentProcessedActorPtr->positionInTrack = currentObject->positionInTrack;

							if (g_engine->getGameId() != GID_AITD1) {
								g_engine->_engine->currentProcessedActorPtr->MARK = currentObject->mark;
							}

							g_engine->_engine->actorTurnedToObj = 1;
						}
					}
				} else {
					if (isInViewList(currentObject->room))
						goto addObject;
				}
			}
		}
	}

	//  objModifFlag1 = 0;

	// TODO: object update
}

void updateAllActorAndObjects() {
	int i;
	const Object *currentActor = g_engine->_engine->objectTable;

	if (g_engine->getGameId() > GID_JACK) {
		updateAllActorAndObjectsAITD2();
		return;
	}
	for (i = 0; i < NUM_MAX_OBJECT; i++) {
		if (currentActor->indexInWorld != -1) {
			if (currentActor->stage == g_engine->_engine->currentFloor) {
				if (currentActor->life != -1) {
					switch (currentActor->lifeMode) {
					case 0: {
						break;
					}
					case 1: {
						if (currentActor->room != g_engine->_engine->currentRoom) {
							deleteObjet(i);
						}
						break;
					}
					case 2: {
						if (!isInViewList(currentActor->room)) {
							deleteObjet(i);
						}
						break;
					}
					default: {
						deleteObjet(i);
						break;
					}
					}
				} else {
					if (!isInViewList(currentActor->room)) {
						deleteObjet(i);
					}
				}
			} else {
				deleteObjet(i);
			}
		}

		currentActor++;
	}

	WorldObject *currentObject = &g_engine->_engine->worldObjets[0];

	for (i = 0; i < g_engine->_engine->maxObjects; i++) {
		if (currentObject->objIndex != -1) {
			if (g_engine->_engine->currentWorldTarget == i) {
				g_engine->_engine->currentCameraTargetActor = currentObject->objIndex;
			}
		} else {
			if (currentObject->stage == g_engine->_engine->currentFloor) {
				if (currentObject->life != -1) {
					if (currentObject->lifeMode != -1) {

						switch (currentObject->lifeMode) {
						case 1: {
							if (currentObject->room != g_engine->_engine->currentRoom) {
								currentObject++;
								continue;
							}
							break;
						}
						case 2: {
							if (!isInViewList(currentObject->room)) {
								currentObject++;
								continue;
							}
							break;
						}
						}

						// int var_C = currentObject->flags & 0xFFDF;
						// int var_E = currentObject->field_2;
						// int var_A = currentObject->anim;

					addObject:
						const int actorIdx = copyObjectToActor(currentObject->body, currentObject->typeZV, currentObject->foundName,
															   currentObject->flags & 0xFFDF,
															   currentObject->x, currentObject->y, currentObject->z,
															   currentObject->stage, currentObject->room,
															   currentObject->alpha, currentObject->beta, currentObject->gamma,
															   currentObject->anim,
															   currentObject->frame, currentObject->animType, currentObject->animInfo);

						currentObject->objIndex = actorIdx;

						if (actorIdx != -1) {
							g_engine->_engine->currentProcessedActorPtr = &g_engine->_engine->objectTable[actorIdx];
							g_engine->_engine->currentProcessedActorIdx = actorIdx;

							if (g_engine->_engine->currentWorldTarget == i) {
								g_engine->_engine->currentCameraTargetActor = g_engine->_engine->currentProcessedActorIdx;
							}

							g_engine->_engine->currentProcessedActorPtr->dynFlags = (currentObject->flags & 0x20) / 0x20; // recheck
							g_engine->_engine->currentProcessedActorPtr->life = currentObject->life;
							g_engine->_engine->currentProcessedActorPtr->lifeMode = currentObject->lifeMode;

							g_engine->_engine->currentProcessedActorPtr->indexInWorld = i;

							setMoveMode(currentObject->trackMode, currentObject->trackNumber);

							g_engine->_engine->currentProcessedActorPtr->positionInTrack = currentObject->positionInTrack;

							g_engine->_engine->actorTurnedToObj = 1;
						}
					}
				} else {
					if (isInViewList(currentObject->room))
						goto addObject;
				}
			}
		}

		currentObject++;
	}

	//  FlagGenereActiveList = 0;

	// TODO: object update
}

static int checkActorInRoom(int room) {

	for (int i = 0; i < g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera]->numViewedRooms; i++) {
		if (g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera]->viewedRoomTable[i].viewedRoomIdx == room) {
			return 1;
		}
	}

	return 0;
}

void createActorList() {

	g_engine->_engine->numActorInList = 0;

	Object *actorPtr = g_engine->_engine->objectTable;

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		if (actorPtr->indexInWorld != -1 && actorPtr->bodyNum != -1) {
			if (checkActorInRoom(actorPtr->room)) {
				g_engine->_engine->sortedActorTable[g_engine->_engine->numActorInList] = i;
				if (!(actorPtr->_flags & (AF_SPECIAL & AF_ANIMATED))) {
					actorPtr->_flags |= AF_BOXIFY;
					g_engine->_engine->flagRefreshAux2 = 1;
				}
				g_engine->_engine->numActorInList++;
			}
		}

		actorPtr++;
	}
}

void setupCamera() {

	freezeTime();

	g_engine->_engine->currentCamera = g_engine->_engine->startGameVar1;

	assert(static_cast<uint32>(g_engine->_engine->startGameVar1) < g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].numCameraInRoom);

	loadCamera(g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].cameraIdxTable[g_engine->_engine->startGameVar1]);
	if (g_engine->getGameId() >= GID_JACK) {
		loadMask(g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].cameraIdxTable[g_engine->_engine->startGameVar1]);
	} else {
		createAITD1Mask();
	}
	g_engine->_engine->cameraBackgroundChanged = true;

	const CameraData *pCamera = g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera];

	setAngleCamera(pCamera->alpha, pCamera->beta, pCamera->gamma);

	const int x = (pCamera->x - g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldX) * 10;
	const int y = (g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldY - pCamera->y) * 10;
	const int z = (g_engine->_engine->roomDataTable[g_engine->_engine->currentRoom].worldZ - pCamera->z) * 10;

	setPosCamera(x, y, z); // setup camera position

	setupCameraProjection(160, 100, pCamera->focal1, pCamera->focal2, pCamera->focal3); // setup focale

	setupCameraSub1();
	updateAllActorAndObjects();
	createActorList();
	//  setupCameraSub3();
	refreshAux2();
	/*  setupCameraSub5();
	 */
	if (g_engine->_engine->flagInitView == 2) {
		g_engine->_engine->flagRedraw = 2;
	} else {
		if (g_engine->_engine->flagRedraw != 2) {
			g_engine->_engine->flagRedraw = 1;
		}
	}

	g_engine->_engine->flagInitView = 0;
	unfreezeTime();
}

int16 computeDistanceToPoint(int x1, int z1, int x2, int z2) {
	// int axBackup = x1;
	x1 -= x2;
	if (static_cast<int16>(x1) < 0) {
		x1 = -static_cast<int16>(x1);
	}

	z1 -= z2;
	if (static_cast<int16>(z1) < 0) {
		z1 = -static_cast<int16>(z1);
	}

	if (x1 + z1 > 0xFFFF) {
		return 0x7D00;
	} else {
		return x1 + z1;
	}
}

void initRealValue(int16 beta, int16 newBeta, int16 param, InterpolatedValue *rotatePtr) {
	rotatePtr->oldValue = beta;
	rotatePtr->newValue = newBeta;
	rotatePtr->param = param;
	rotatePtr->timeOfRotate = g_engine->_engine->timer;
}

int16 updateActorRotation(InterpolatedValue *rotatePtr) {

	if (!rotatePtr->param)
		return rotatePtr->newValue;

	const int timeDif = g_engine->_engine->timer - rotatePtr->timeOfRotate;

	if (timeDif > rotatePtr->param) {
		rotatePtr->param = 0;
		return rotatePtr->newValue;
	}

	const int angleDif = (rotatePtr->newValue & 0x3FF) - (rotatePtr->oldValue & 0x3FF);

	if (angleDif <= 0x200) {
		if (angleDif >= -0x200) {
			const int angle = (rotatePtr->newValue & 0x3FF) - (rotatePtr->oldValue & 0x3FF);
			return (rotatePtr->oldValue & 0x3FF) + angle * timeDif / rotatePtr->param;
		} else {
			const int16 angle = (rotatePtr->newValue & 0x3FF) + 0x400 - (rotatePtr->oldValue & 0x3FF);
			return (rotatePtr->oldValue & 0x3FF) + angle * timeDif / rotatePtr->param;
		}
	} else {
		const int angle = (rotatePtr->newValue & 0x3FF) - ((rotatePtr->oldValue & 0x3FF) + 0x400);
		return angle * timeDif / rotatePtr->param + (rotatePtr->oldValue & 0x3FF);
	}
}

void removeFromBGIncrust(int actorIdx) {
	Object *actorPtr = &g_engine->_engine->objectTable[actorIdx];

	actorPtr->_flags &= ~AF_BOXIFY;

	g_engine->_engine->flagRefreshAux2 = 1;

	g_engine->_engine->BBox3D1 = actorPtr->screenXMin;

	if (g_engine->_engine->BBox3D1 > -1) {
		g_engine->_engine->BBox3D2 = actorPtr->screenYMin;
		g_engine->_engine->BBox3D3 = actorPtr->screenXMax;
		g_engine->_engine->BBox3D4 = actorPtr->screenYMax;

		// deleteSubSub();
	}
}

int findObjectInInventory(int objIdx) {
	for (int i = 0; i < g_engine->_engine->numObjInInventoryTable; i++) {
		if (g_engine->_engine->inventoryTable[i] == objIdx) {
			return i;
		}
	}

	return -1;
}

void deleteInventoryObjet(int objIdx) {
	const int inventoryIdx = findObjectInInventory(objIdx);
	if (inventoryIdx == -1)
		return;

	memmove(&g_engine->_engine->inventoryTable[inventoryIdx], &g_engine->_engine->inventoryTable[inventoryIdx + 1], (30 - inventoryIdx - 1) * 2);
	g_engine->_engine->numObjInInventoryTable--;
	g_engine->_engine->worldObjets[objIdx].flags2 &= 0x7FFF;
}

static int isBgOverlayRequired(int X1, int X2, int Z1, int Z2, byte *data, int param) {
	for (int i = 0; i < param; i++) {
		////////////////////////////////////// DEBUG
		//  drawOverlayZone(data, 80);
		/////////////////////////////////////

		const int zoneX1 = *(int16 *)data;
		const int zoneZ1 = *(int16 *)(data + 2);
		const int zoneX2 = *(int16 *)(data + 4);
		const int zoneZ2 = *(int16 *)(data + 6);

		if (X1 >= zoneX1 && Z1 >= zoneZ1 && X2 <= zoneX2 && Z2 <= zoneZ2) {
			return 1;
		}

		data += 0x8;
	}

	return 0;
}

static void drawBgOverlay(Object *actorPtr) {

	actorPtr->screenXMin = g_engine->_engine->BBox3D1;
	actorPtr->screenYMin = g_engine->_engine->BBox3D2;
	actorPtr->screenXMax = g_engine->_engine->BBox3D3;
	actorPtr->screenYMax = g_engine->_engine->BBox3D4;

	// if(actorPtr->trackMode != 1)
	//	return;

	setClip(g_engine->_engine->BBox3D1, g_engine->_engine->BBox3D2, g_engine->_engine->BBox3D3, g_engine->_engine->BBox3D4);

	const CameraData *pCamera = g_engine->_engine->cameraDataTable[g_engine->_engine->currentCamera];

	// look for the correct room data of that camera
	const CameraViewedRoom *pcameraViewedRoomData = nullptr;
	int relativeCameraIndex = -1;
	for (int i = 0; i < pCamera->numViewedRooms; i++) {
		if (pCamera->viewedRoomTable[i].viewedRoomIdx == actorPtr->room) {
			pcameraViewedRoomData = &pCamera->viewedRoomTable[i];
			relativeCameraIndex = i;
			break;
		}
	}
	if (pcameraViewedRoomData == nullptr)
		return;

	if (g_engine->getGameId() == GID_AITD1) {
		byte *data2 = g_engine->_engine->roomPtrCamera[g_engine->_engine->currentCamera] + pcameraViewedRoomData->offsetToMask;
		byte *data = data2;
		data += 2;

		const int numOverlayZone = *(int16 *)data2;

		if (g_engine->_engine->lightOff && g_engine->_engine->lightX != 20000) {
			clearClip();
			drawMask(0, 666);
		} else {
			for (int i = 0; i < numOverlayZone; i++) {
				if (isBgOverlayRequired(actorPtr->zv.ZVX1 / 10, actorPtr->zv.ZVX2 / 10,
										actorPtr->zv.ZVZ1 / 10, actorPtr->zv.ZVZ2 / 10,
										data + 4,
										*(int16 *)data)) {
					setClip();
					drawMask(relativeCameraIndex, i);
					clearClip();
				}

				const int numOverlay = *(int16 *)data;
				data += 2;
				data += (numOverlay * 4 + 1) * 2;
			}
		}

	} else {
		for (int i = 0; i < pcameraViewedRoomData->numMask; i++) {
			const CameraMask *pMaskZones = &pcameraViewedRoomData->masks[i];

			for (int j = 0; j < pMaskZones->numTestRect; j++) {
				const RectTest *pRect = &pMaskZones->rectTests[j];

				const int actorX1 = actorPtr->zv.ZVX1 / 10;
				const int actorX2 = actorPtr->zv.ZVX2 / 10;
				const int actorZ1 = actorPtr->zv.ZVZ1 / 10;
				const int actorZ2 = actorPtr->zv.ZVZ2 / 10;

				if (actorX1 >= pRect->zoneX1 && actorZ1 >= pRect->zoneZ1 && actorX2 <= pRect->zoneX2 && actorZ2 <= pRect->zoneZ2) {
					setClip();
					drawMask(relativeCameraIndex, i);
					clearClip();
					break;
				}
			}
		}
	}
	setClip(0, 0, 319, 199);
}

static void calcXYZNuage(int16 x, int y, int16 z, int16 alpha, int16 beta, int16 gamma, byte *modelPtr) {
	rotateNuage2(x, y, z, alpha, beta, gamma, *(int16 *)modelPtr, (int16 *)(modelPtr + 2));
}

static int16 regleTrois(int i1, int i2, int i3, int i4) {
	return i1 + (((i2 - i1) * i4) / i3);
}

static int sub_104B7(int si, int ax, int dx, int bx, int cx) {
	const int siSaved = si;
	SWAP(si, ax);
	SWAP(ax, dx);
	if (ax) {
		ax *= bx;
	}
	if (cx != 0) {
		SWAP(ax, cx);
		ax *= si;
		ax += cx;
	}
	SWAP(ax, si);
	ax *= bx;
	dx += si;
	si = siSaved;
	return ax;
}

static void drawSpecialObject(int actorIdx) {
	Object *actorPtr = &g_engine->_engine->objectTable[actorIdx];

	byte *flowPtr = hqPtrMalloc(g_engine->_engine->hqMemory, actorPtr->FRAME);
	if (!flowPtr)
		return;

	switch (actorPtr->ANIM) {
	case 0: { // evaporate
		const int16 color = *(int16 *)flowPtr;
		flowPtr += 2;
		actorPtr->beta += 8;
		int16 *flowPointList = (int16 *)flowPtr;
		calcXYZNuage(actorPtr->worldX, actorPtr->worldY, actorPtr->worldZ, actorPtr->alpha, actorPtr->beta, actorPtr->gamma, flowPtr);
		// skip point list
		int16 *flowAnimList = (int16 *)(flowPtr + 2 + 6 * (*flowPointList));

		const int16 *pPointList = g_engine->_engine->renderPointList;
		bool freeData = true;

		const int16 numPoints = flowPointList[0];
		flowPointList++;

		for (int i = 0; i < numPoints; ++i) {
			const int16 size = flowAnimList[0];
			if (size > 0) {
				freeData = false;
				const int16 z = pPointList[2];
				if (z > 300) {
					const float transformedSize = size * static_cast<float>(g_engine->_engine->cameraFovX) / static_cast<float>(z + g_engine->_engine->cameraPerspective);
					drawSphere(pPointList[0], pPointList[1], z, color, 3, transformedSize);
				}
				flowAnimList[0] -= 5;                // size -= 5
				flowPointList[1] -= flowAnimList[1]; // y -= dy
			}
			flowAnimList += 2;
			flowPointList += 3;
			pPointList += 3;
		}

		if (freeData) {
			// HQR_FreeMalloc(HQ_Memory, actorPtr->FRAME)
			actorPtr->indexInWorld = -1;
			g_engine->_engine->actorTurnedToObj = 1;
		}
		break;
	}
	case 1: // blood
	case 2: // debris
	{
		const int16 color = *(int16 *)flowPtr;
		flowPtr += 2;
		int16 *flowPointList = (int16 *)flowPtr;
		calcXYZNuage(actorPtr->worldX, 0, actorPtr->worldZ, actorPtr->alpha, actorPtr->beta, actorPtr->gamma, flowPtr);
		// skip point list
		int16 *flowAnimList = (int16 *)(flowPtr + 2 + 6 * (*flowPointList));

		const int16 *pPointList = g_engine->_engine->renderPointList;
		bool freeData = true;
		const int16 numPoints = flowPointList[0];
		flowPointList++;

		for (int i = 0; i < numPoints; ++i) {
			const int16 y = flowPointList[1];
			if (y >= 0) {
				freeData = false;
				if (pPointList[2] > 100) {
					drawPoint(pPointList[0], pPointList[1], pPointList[2], color);
				}
				flowAnimList[1] += 6;
				walkStep(10, 0, flowAnimList[0]);
				flowPointList[0] += g_engine->_engine->animMoveX;
				flowPointList[1] += flowAnimList[1];
				flowPointList[2] += g_engine->_engine->animMoveZ;
			}
			flowAnimList += 2;
			flowPointList += 3;
			pPointList += 3;
		}

		if (freeData) {
			// HQR_FreeMalloc(HQ_Memory, actorPtr->FRAME);
			actorPtr->indexInWorld = -1;
			g_engine->_engine->actorTurnedToObj = 1;
		}
		break;
	}
	case 3: {
		// muzzle flash
		flowPtr = hqrGet(g_engine->_engine->listBody, g_engine->_engine->cVars[getCVarsIdx(BODY_FLAMME)]).data;
		affObjet(actorPtr->worldX, actorPtr->worldY, actorPtr->worldZ, 0, actorPtr->beta, 0, flowPtr);
		actorPtr->indexInWorld = -1;
		break;
	}
	case 4: {
		// cigar smoke
		const uint32 *chronoPtr = (uint32 *)flowPtr;
		uint tmpChrono = *chronoPtr;
		const uint chrono = evalChrono(&tmpChrono);
		const int16 chrono1 = static_cast<int16>((chrono & 0xFFFF0000) >> 16);
		const int16 chrono2 = static_cast<int16>(chrono & 0x0000FFFF);
		flowPtr += 4;
		byte *flowPtrSaved = flowPtr;
		flowPtr += 120; // skip 20 * x,y,z
		byte *flowPtrSaved2 = flowPtr;
		*(int16 *)flowPtr = 20; // number of points
		flowPtr += 2;
		for (int j = 0; j < 20; ++j) {
			const int16 x = *(int16 *)flowPtrSaved;
			const int16 z = *(int16 *)(flowPtrSaved + 4);
			*(int16 *)flowPtr = regleTrois(0, x, 600, chrono2);
			*(int16 *)(flowPtr + 2) = -200;
			*(int16 *)(flowPtr + 4) = regleTrois(0, z, 600, chrono2);
			flowPtr += 6;
			flowPtrSaved += 6;
		}
		flowPtr = flowPtrSaved2;
		actorPtr->beta += 4;
		calcXYZNuage(actorPtr->worldX, actorPtr->worldY, actorPtr->worldZ, actorPtr->alpha, actorPtr->beta, actorPtr->gamma, flowPtr);

		if (g_engine->_engine->cVars[getCVarsIdx(FOG_FLAG)] == 2) {
			g_engine->_engine->cVars[getCVarsIdx(FOG_FLAG)] = 3;
		}

		if (chrono1 >= 0) {
			if (chrono1 != 0 || chrono2 >= 480) {
				if (g_engine->_engine->cVars[getCVarsIdx(FOG_FLAG)] == 1) {
					g_engine->_engine->cVars[getCVarsIdx(FOG_FLAG)] = 2;
				}
			}
		}
		int16 size;
		if (chrono1 > 0 || (chrono1 == 0 && chrono2 > 540)) {
			size = sub_104B7(20, 5, 0, 660 - chrono2, 660 - chrono1);
		} else {
			size = chrono2;
		}

		const int16 *pPointList = g_engine->_engine->renderPointList;
		for (int i = 0; i < 20; ++i) {
			const int x = pPointList[0];
			const int y = pPointList[1];
			const int z = pPointList[2];
			if (z > 10) {
				const float transformedSize = static_cast<float>(size) * static_cast<float>(g_engine->_engine->cameraFovX) / static_cast<float>(z + g_engine->_engine->cameraPerspective);
				drawSphere(x, y, z, 176, 2, transformedSize);
			}
			pPointList += 3;
		}

		if (chrono1 >= 0) {
			if (chrono1 != 0 || chrono2 >= 660) {
				g_engine->_engine->cVars[getCVarsIdx(FOG_FLAG)] = 0;
				// TODO: HQR_Free
				actorPtr->indexInWorld = -1;
				g_engine->_engine->actorTurnedToObj = 1;
			}
		}
		break;
	}
	default:
		warning("drawSpecialObject ANIM=%d not implemented", actorPtr->ANIM);
		break;
	}

	// TODO: finish
}

static void getHotPoint(int hotPointIdx, byte *bodyPtr, Point3d *hotPoint) {

	const int16 flag = *(int16 *)bodyPtr;
	bodyPtr += 2;

	if (flag & 2) {
		bodyPtr += 12;

		int16 offset = *(int16 *)bodyPtr;
		bodyPtr += 2;
		bodyPtr += offset;

		offset = *(int16 *)bodyPtr; // num points
		bodyPtr += 2;
		bodyPtr += offset * 6; // skip point buffer

		offset = *(int16 *)bodyPtr; // num bones
		bodyPtr += 2;
		bodyPtr += offset * 2; // skip bone buffer

		assert(hotPointIdx < offset);

		if (hotPointIdx < offset) {

			if (flag & INFO_OPTIMISE) {
				bodyPtr += hotPointIdx * 0x18;
			} else {
				bodyPtr += hotPointIdx * 16;
			}

			const int pointIdx = *(int16 *)(bodyPtr + 4); // first point

			// ASSERT(pointIdx > 0 && pointIdx < 1200);

			const int16 *source = (int16 *)((byte *)g_engine->_engine->pointBuffer + pointIdx);

			hotPoint->x = source[0];
			hotPoint->y = source[1];
			hotPoint->z = source[2];
		} else {
			hotPoint->x = 0;
			hotPoint->y = 0;
			hotPoint->z = 0;
		}
	} else {
		hotPoint->x = 0;
		hotPoint->y = 0;
		hotPoint->z = 0;
	}
}

static int drawTextOverlay();

void mainDraw(int flagFlip) {
	// if(flagFlip == 2)
	{
		if (g_engine->_engine->cameraBackgroundChanged) {
			gfx_copyBlockPhys(g_engine->_engine->aux, 0, 0, 320, 200);
			g_engine->_engine->cameraBackgroundChanged = false;
		}
	}

	if (flagFlip == 0) {
		// restoreDirtyRects();
	} else {
		g_engine->_engine->nbPhysBoxs = 0;
		fastCopyScreen(g_engine->_engine->aux2, g_engine->_engine->logicalScreen);
	}

	// osystem_drawBackground();

	setClip(0, 0, 319, 199);
	g_engine->_engine->nbLogBoxs = 0;

	// osystem_startModelRender();

	memset(g_engine->_engine->frontBuffer, 0, 320 * 200);
	for (int i = 0; i < g_engine->_engine->numActorInList; i++) {
		const int currentDrawActor = g_engine->_engine->sortedActorTable[i];

		Object *actorPtr = &g_engine->_engine->objectTable[currentDrawActor];

		// this is commented out to draw actors incrusted in background
		// if(actorPtr->_flags & (AF_ANIMATED | AF_DRAWABLE | AF_SPECIAL))
		{
			actorPtr->_flags &= ~AF_DRAWABLE;

			if (actorPtr->_flags & AF_SPECIAL) {
				drawSpecialObject(currentDrawActor);
			} else {
				byte *bodyPtr = hqrGet(g_engine->_engine->listBody, actorPtr->bodyNum).data;

				if (g_engine->_engine->hqLoad) {
					// setAnimObjet(actorPtr->FRAME, HQR_Get(g_engine->_engine->listAnim, actorPtr->ANIM), bodyPtr);
				}

				affObjet(actorPtr->worldX + actorPtr->stepX, actorPtr->worldY + actorPtr->stepY, actorPtr->worldZ + actorPtr->stepZ, actorPtr->alpha, actorPtr->beta, actorPtr->gamma, bodyPtr);

				if (actorPtr->animActionType != 0) {
					if (actorPtr->hotPointID != -1) {
						getHotPoint(actorPtr->hotPointID, bodyPtr, &actorPtr->hotPoint);
					}
				}
			}

			if (g_engine->_engine->BBox3D1 < 0)
				g_engine->_engine->BBox3D1 = 0;
			if (g_engine->_engine->BBox3D3 > 319)
				g_engine->_engine->BBox3D3 = 319;
			if (g_engine->_engine->BBox3D2 < 0)
				g_engine->_engine->BBox3D2 = 0;
			if (g_engine->_engine->BBox3D4 > 199)
				g_engine->_engine->BBox3D4 = 199;

			if (g_engine->_engine->BBox3D1 <= 319 && g_engine->_engine->BBox3D2 <= 199 && g_engine->_engine->BBox3D3 >= 0 && g_engine->_engine->BBox3D4 >= 0) // is the character on screen ?
			{
				if (g_engine->getGameId() == GID_AITD1) {
					if (actorPtr->indexInWorld == g_engine->_engine->cVars[getCVarsIdx(LIGHT_OBJECT)]) {
						g_engine->_engine->lightX = (g_engine->_engine->BBox3D3 + g_engine->_engine->BBox3D1) / 2;
						g_engine->_engine->lightY = (g_engine->_engine->BBox3D4 + g_engine->_engine->BBox3D2) / 2;

						// TODO: fix light mask
						// this is not a translation of the original code
						// the original code is using a material 7 to copy only the pixels inside the circle (drawSphere)
						// and maskId 666 is just a trick here to identify this special mask
						// I'm way too lazy to change that right now
						if (g_engine->_engine->lightOff && g_engine->_engine->lightX != 20000) {
							drawSphere(g_engine->_engine->lightX, g_engine->_engine->lightY, 0, 0, 7, 30);
							createMask(g_engine->_engine->frontBuffer, 0, 666, nullptr, 0, 0, 320, 200);
							g_engine->_engine->ancLumiereX = g_engine->_engine->lightX;
							g_engine->_engine->ancLumiereY = g_engine->_engine->lightY;
						}
					} else if (g_engine->_engine->lightOff) {
						createMask(g_engine->_engine->frontBuffer, 0, 666, nullptr, 0, 0, 320, 200);
					}
				}

				{
					// if (g_engine->getGameId() == GID_AITD1)
					drawBgOverlay(actorPtr);
				}
				// addToRedrawBox();
			} else {
				actorPtr->screenYMax = -1;
				actorPtr->screenXMax = -1;
				actorPtr->screenYMin = -1;
				actorPtr->screenXMin = -1;
			}
		}
	}

	flushPendingPrimitives();

	if (drawTextOverlay()) {
		gfx_copyBlockPhys(g_engine->_engine->logicalScreen, g_engine->_engine->BBox3D1, g_engine->_engine->BBox3D2, g_engine->_engine->BBox3D3, g_engine->_engine->BBox3D4);
	} else {
		// TODO: check if it's okay
		fastCopyScreen(g_engine->_engine->aux, g_engine->_engine->logicalScreen);
		gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);
	}

	if (!g_engine->_engine->lightOff) {
		if (flagFlip) {
			if (flagFlip == 2 || g_engine->_engine->newFlagLight) {
				makeBlackPalette();
				flip();
				fadeInPhys(0x10, 0);
				g_engine->_engine->newFlagLight = 0;
			} else {
				// osystem_flip(NULL);
			}
		} else {
			// flipLogPhys();
		}
	} else {
	}

	// SWAP(ListLogBox, ListPhysBox);
	// SWAP(NbLogBoxs, NbPhysBoxs);

	if (g_engine->_screen)
		memcpy(g_engine->_engine->aux2, g_engine->_screen->getBasePtr(0, 0), 320 * 200);

	g_engine->_engine->flagRedraw = 0;
}

void walkStep(int angle1, int angle2, int angle3) {
	rotate(angle3, angle1, angle2, &g_engine->_engine->animMoveZ, &g_engine->_engine->animMoveX);
}

void addActorToBgInscrust(int actorIdx) {
	g_engine->_engine->objectTable[actorIdx]._flags |= AF_BOXIFY + AF_DRAWABLE;
	g_engine->_engine->objectTable[actorIdx]._flags &= ~AF_ANIMATED;

	// FlagRefreshAux2 = 1;
}

void cleanClip() {
	for (int x = g_engine->_engine->clipLeft; x < g_engine->_engine->clipRight; x++) {
		for (int y = g_engine->_engine->clipTop; y < g_engine->_engine->clipBottom; y++) {
			g_engine->_engine->logicalScreen[y * 320 + x] = 0;
		}
	}
}

void drawFoundObect(int menuState, int objectName, int zoomFactor) {
	cleanClip();

	setCameraTarget(0, 0, 0, 60, g_engine->_engine->statusVar1, 0, zoomFactor);

	affObjet(0, 0, 0, 0, 0, 0, hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentFoundBodyIdx).data);

	simpleMessage(160, g_engine->_engine->windowY1, 20, 1);
	simpleMessage(160, g_engine->_engine->windowY1 + 16, objectName, 1);
	simpleMessage(160, g_engine->_engine->windowY1 + 16, objectName, 1);

	switch (menuState) {
	case 0: {
		selectedMessage(130, g_engine->_engine->windowY2 - 16, 21, 1, 4);
		simpleMessage(190, g_engine->_engine->windowY2 - 16, 22, 4);
		break;
	}
	case 1: {
		simpleMessage(130, g_engine->_engine->windowY2 - 16, 21, 4);
		selectedMessage(190, g_engine->_engine->windowY2 - 16, 22, 1, 4);
		break;
	}
	case 2: {
		selectedMessage(160, g_engine->_engine->windowY2 - 16, 10, 1, 4);
		break;
	}
	}
}

void take(int objIdx) {
	WorldObject *objPtr = &g_engine->_engine->worldObjets[objIdx];

	if (g_engine->_engine->numObjInInventoryTable == 0) {
		g_engine->_engine->inventoryTable[0] = objIdx;
	} else {

		for (int i = g_engine->_engine->numObjInInventoryTable; i > 0; i--) {
			g_engine->_engine->inventoryTable[i + 1] = g_engine->_engine->inventoryTable[i];
		}

		g_engine->_engine->inventoryTable[1] = objIdx;
	}

	g_engine->_engine->numObjInInventoryTable++;

	g_engine->_engine->action = 0x800;

	executeFoundLife(objIdx);

	if (objPtr->objIndex != -1) {
		deleteObjet(objPtr->objIndex);
	}

	objPtr->flags2 &= 0xBFFF;
	objPtr->flags2 |= 0x8000;

	objPtr->room = -1;
	objPtr->stage = -1;
}

void foundObject(int objIdx, int param) {
	int var_C = 0;
	int var_6 = 1;
	int var_A = 15000;
	int var_8 = -200;

	if (objIdx < 0)
		return;

	if (param == 2) {
		debug("foundObject with param == 2\n");
	}

	WorldObject *objPtr = &g_engine->_engine->worldObjets[objIdx];

	if (param != 0 && objPtr->flags2 & 0xC000) {
		return;
	}

	if (objPtr->trackNumber) {
		if (g_engine->_engine->timer - objPtr->trackNumber < 300) // prevent from reopening the window every frame
			return;
	}

	objPtr->trackNumber = 0;

	freezeTime();
	setWaterHeight(10000);

	int weight = 0;
	for (int i = 0; i < g_engine->_engine->numObjInInventoryTable; i++) {
		weight += g_engine->_engine->worldObjets[g_engine->_engine->inventoryTable[i]].positionInTrack;
	}

	if (objPtr->positionInTrack + weight > g_engine->_engine->cVars[getCVarsIdx(MAX_WEIGHT_LOADABLE)] || g_engine->_engine->numObjInInventoryTable + 1 == 30) {
		var_6 = 3;
	}

	g_engine->_engine->currentFoundBodyIdx = objPtr->foundBody;
	g_engine->_engine->currentFoundBody = hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentFoundBodyIdx).data;

	setupCameraProjection(160, 100, 128, 300, 298);

	g_engine->_engine->statusVar1 = 0;

	memset(g_engine->_engine->frontBuffer, 0, 320 * 200);
	fastCopyScreen(g_engine->_engine->frontBuffer, g_engine->_engine->logicalScreen);

	if (g_engine->getGameId() == GID_AITD1 || g_engine->getGameId() == GID_JACK) {
		affBigCadre(160, 100, 240, 120);
	} else {
		affBigCadre2(160, 100, 240, 120);
	}

	drawFoundObect(var_6, objPtr->foundName, var_A);
	flip();

	input5 = 1;

	while (!var_C && !::Engine::shouldQuit()) {
		gfx_copyBlockPhys(g_engine->_engine->logicalScreen, 0, 0, 320, 200);

		process_events();
		drawBackground();

		g_engine->_engine->localKey = g_engine->_engine->key;
		g_engine->_engine->localJoyD = g_engine->_engine->joyD;
		g_engine->_engine->localClick = g_engine->_engine->click;

		if (!input5) {
			if (g_engine->_engine->localKey == 1) {
				if (var_6 != 2) {
					var_6 = 0;
				}

				var_C = 1;
			}
			if (var_6 != 2) {
				if (g_engine->_engine->localJoyD & 4) {
					var_6 = 0;
				}

				if (g_engine->_engine->localJoyD & 8) {
					var_6 = 1;
				}
			}

			if (g_engine->_engine->localKey == 28 || g_engine->_engine->localClick != 0) {
				while (g_engine->_engine->key) {
					process_events();
				}

				var_C = 1;
			}
		} else {
			if (!g_engine->_engine->localKey && !g_engine->_engine->localJoyD && !g_engine->_engine->localClick)
				input5 = 0;
		}

		g_engine->_engine->statusVar1 -= 8;

		var_A += var_8; // zoom / dezoom

		if (var_A > 8000) // zoom management
			var_8 = -var_8;

		if (var_A < 25000)
			var_8 = -var_8;

		drawFoundObect(var_6, objPtr->foundName, var_A);

		//    menuWaitVSync();
	}

	unfreezeTime();

	if (var_6 == 1) {
		take(objIdx);
	} else {
		objPtr->trackNumber = g_engine->_engine->timer;
	}

	while (g_engine->_engine->key && g_engine->_engine->click) {
		process_events();
	}

	g_engine->_engine->localJoyD = 0;
	g_engine->_engine->localKey = 0;
	g_engine->_engine->localClick = 0;

	if (g_engine->_engine->flagRotPal != 0) {
		setWaterHeight(-600);
	}

	g_engine->_engine->flagInitView = 1;
}

static int testCrossProduct(int x1, int z1, int x2, int z2, int x3, int z3, int x4, int z4) {
	int returnFlag = 0;

	const int xAB = x1 - x2;
	const int yCD = z3 - z4;
	const int xCD = x3 - x4;
	const int yAB = z1 - z2;

	const int xAC = x1 - x3;
	const int yAC = z1 - z3;

	int DotProduct = xAB * yCD - xCD * yAC;

	if (DotProduct) {
		int Dda = xAC * yCD - xCD * yAC;
		int Dmu = -xAB * yAC + xAC * yAB;

		if (DotProduct < 0) {
			DotProduct = -DotProduct;
			Dda = -Dda;
			Dmu = -Dmu;
		}

		if (Dda >= 0 && Dmu >= 0 && DotProduct >= Dda && DotProduct >= Dmu)
			returnFlag = 1;
	}

	return returnFlag;
}

static int isInPoly(int x1, int x2, int z1, int z2, CameraViewedRoom *pCameraZoneDef) {
	const int xMid = (x1 + x2) / 2;
	const int zMid = (z1 + z2) / 2;

	for (int i = 0; i < pCameraZoneDef->numCoverZones; i++) {
		int flag = 0;

		for (int j = 0; j < pCameraZoneDef->coverZones[i].numPoints; j++) {

			const int zoneX1 = pCameraZoneDef->coverZones[i].pointTable[j].x;
			const int zoneZ1 = pCameraZoneDef->coverZones[i].pointTable[j].y;
			const int zoneX2 = pCameraZoneDef->coverZones[i].pointTable[j + 1].x;
			const int zoneZ2 = pCameraZoneDef->coverZones[i].pointTable[j + 1].y;

			if (testCrossProduct(xMid, zMid, xMid - 10000, zMid, zoneX1, zoneZ1, zoneX2, zoneZ2)) {
				flag |= 1;
			}

			if (testCrossProduct(xMid, zMid, xMid + 10000, zMid, zoneX1, zoneZ1, zoneX2, zoneZ2)) {
				flag |= 2;
			}
		}

		if (flag == 3) {
			return 1;
		}
	}

	return 0;
}

static int findBestCamera() {
	int foundAngle = 32000;
	int foundCamera = -1;

	const Object *actorPtr = &g_engine->_engine->objectTable[g_engine->_engine->currentCameraTargetActor];

	const int x1 = actorPtr->zv.ZVX1 / 10;
	const int x2 = actorPtr->zv.ZVX2 / 10;
	const int z1 = actorPtr->zv.ZVZ1 / 10;
	const int z2 = actorPtr->zv.ZVZ2 / 10;

	for (int i = 0; i < g_engine->_engine->numCameraInRoom; i++) {
		assert(i < NUM_MAX_CAMERA_IN_ROOM);
		if (g_engine->_engine->currentCameraZoneList[i])
			if (isInPoly(x1, x2, z1, z2, g_engine->_engine->currentCameraZoneList[i])) // if in camera zone ?
			{
				// we try to select the best camera that looks behind the player
				int newAngle = actorPtr->beta + ((g_engine->_engine->cameraDataTable[i]->beta + 0x200) & 0x3FF);

				if (newAngle < 0) {
					newAngle = -newAngle;
				}

				if (newAngle < foundAngle) {
					foundAngle = newAngle;
					foundCamera = i;
				}
			}
	}

	return foundCamera;
}

void checkIfCameraChangeIsRequired() {
	int localCurrentCam = g_engine->_engine->currentCamera;

	if (g_engine->_engine->currentCamera != -1) {

		const Object *actorPtr = &g_engine->_engine->objectTable[g_engine->_engine->currentCameraTargetActor];

		const int zvx1 = actorPtr->zv.ZVX1 / 10;
		const int zvx2 = actorPtr->zv.ZVX2 / 10;

		const int zvz1 = actorPtr->zv.ZVZ1 / 10;
		const int zvz2 = actorPtr->zv.ZVZ2 / 10;

		if (isInPoly(zvx1, zvx2, zvz1, zvz2, g_engine->_engine->currentCameraZoneList[g_engine->_engine->currentCamera])) // is still in current camera zone ?
		{
			return;
		}
	}

	const int newCamera = findBestCamera(); // find new camera

	if (newCamera != -1) {
		localCurrentCam = newCamera;
	}

	if (g_engine->_engine->currentCamera != localCurrentCam) {
		g_engine->_engine->startGameVar1 = localCurrentCam;
		g_engine->_engine->flagInitView = 1;
	}
}

static bool isPointInZV(int x, int y, int z, ZVStruct *pZV) {
	if (pZV->ZVX1 <= x && pZV->ZVX2 >= x) {
		if (pZV->ZVY1 <= y && pZV->ZVY2 >= y) {
			if (pZV->ZVZ1 <= z && pZV->ZVZ2 >= z) {
				return true;
			}
		}
	}

	return false;
}

void processActor2() {
	bool onceMore = false;
	bool flagFloorChange = false;
	// int zoneIdx = 0;

	do {
		onceMore = false;
		RoomData *pRoomData = &g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room];
		for (uint32 i = 0; i < pRoomData->numSceZone; i++) {
			SceZone *pCurrentZone = &pRoomData->sceZoneTable[i];

			if (isPointInZV(g_engine->_engine->currentProcessedActorPtr->roomX + g_engine->_engine->currentProcessedActorPtr->stepX,
							g_engine->_engine->currentProcessedActorPtr->roomY + g_engine->_engine->currentProcessedActorPtr->stepY,
							g_engine->_engine->currentProcessedActorPtr->roomZ + g_engine->_engine->currentProcessedActorPtr->stepZ,
							&pCurrentZone->zv)) {
				switch (pCurrentZone->type) {
				case 0: {

					const int oldRoom = g_engine->_engine->currentProcessedActorPtr->room;

					g_engine->_engine->currentProcessedActorPtr->room = static_cast<int16>(pCurrentZone->parameter);

					const int x = (g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldX - g_engine->_engine->roomDataTable[oldRoom].worldX) * 10;
					const int y = (g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldY - g_engine->_engine->roomDataTable[oldRoom].worldY) * 10;
					const int z = (g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room].worldZ - g_engine->_engine->roomDataTable[oldRoom].worldZ) * 10;

					g_engine->_engine->currentProcessedActorPtr->roomX -= x;
					g_engine->_engine->currentProcessedActorPtr->roomY += y;
					g_engine->_engine->currentProcessedActorPtr->roomZ += z;

					g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 -= x;
					g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 -= x;

					g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += y;
					g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += y;

					g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 += z;
					g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 += z;

					onceMore = true;
					if (g_engine->_engine->currentProcessedActorIdx == g_engine->_engine->currentCameraTargetActor) {
						g_engine->_engine->needChangeRoom = 1;
						g_engine->_engine->newRoom = static_cast<int16>(pCurrentZone->parameter);
						if (g_engine->getGameId() > GID_AITD1)
							loadRoom(g_engine->_engine->newRoom);

					} else {
						g_engine->_engine->actorTurnedToObj = 1;
					}

					startChrono(&g_engine->_engine->currentProcessedActorPtr->ROOM_CHRONO);

					break;
				}
				case 8: {
					assert(g_engine->getGameId() != GID_AITD1);
					if (g_engine->getGameId() != GID_AITD1) {
						g_engine->_engine->currentProcessedActorPtr->hardMat = static_cast<int16>(pCurrentZone->parameter);
					}
					break;
				}
				case 9: // Scenar
				{
					if (g_engine->getGameId() == GID_AITD1 || !flagFloorChange) {
						g_engine->_engine->currentProcessedActorPtr->HARD_DEC = static_cast<int16>(pCurrentZone->parameter);
					}
					break;
				}
				case 10: // stage
				{

					const int life = g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].floorLife;

					if (life == -1)
						return;

					g_engine->_engine->currentProcessedActorPtr->life = life;

					g_engine->_engine->currentProcessedActorPtr->HARD_DEC = static_cast<int16>(pCurrentZone->parameter);
					flagFloorChange = true;
					return;
				}
				}

				if (g_engine->getGameId() == GID_AITD1) // AITD1 stops at the first zone
					return;
			}
			if (onceMore)
				break;
		}
	} while (onceMore);
}

int checkLineProjectionWithActors(int actorIdx, int X, int Y, int Z, int beta, int room, int param) {
	ZVStruct localZv;
	int foundFlag = -2;
	int tempX = X;
	int tempZ = Z;

	localZv.ZVX1 = X - param;
	localZv.ZVX2 = X + param;
	localZv.ZVY1 = Y - param;
	localZv.ZVY2 = Y + param;
	localZv.ZVZ1 = Z - param;
	localZv.ZVZ2 = Z + param;

	walkStep(param * 2, 0, beta);

	while (foundFlag == -2) {
		localZv.ZVX1 += g_engine->_engine->animMoveX;
		localZv.ZVX2 += g_engine->_engine->animMoveX;

		localZv.ZVZ1 += g_engine->_engine->animMoveZ;
		localZv.ZVZ2 += g_engine->_engine->animMoveZ;

		tempX = X;
		tempZ = Z;

		X += g_engine->_engine->animMoveX;
		Z += g_engine->_engine->animMoveZ;

		if (X > 20000 || X < -20000 || Z > 20000 || Z < -20000) {
			foundFlag = -1;
			break;
		}

		if (asmCheckListCol(&localZv, &g_engine->_engine->roomDataTable[room]) <= 0) {
			foundFlag = -1;
		} else {
			const Object *currentActorPtr = g_engine->_engine->objectTable;

			for (int i = 0; i < NUM_MAX_OBJECT; i++) {
				if (currentActorPtr->indexInWorld != -1 && i != actorIdx && !(currentActorPtr->_flags & AF_SPECIAL)) {
					const ZVStruct *zvPtr = &currentActorPtr->zv;

					if (room != currentActorPtr->room) {
						ZVStruct localZv2;

						copyZv(&localZv, &localZv2);
						getZvRelativePosition(&localZv2, room, currentActorPtr->room);

						if (!checkZvCollision(&localZv2, zvPtr)) {
							currentActorPtr++;
							continue;
						}
					} else {
						if (!checkZvCollision(&localZv, zvPtr)) {
							currentActorPtr++;
							continue;
						}
					}

					foundFlag = i;
					break;
				}

				currentActorPtr++;
			}
		}
	}

	g_engine->_engine->animMoveX = tempX;
	g_engine->_engine->animMoveY = Y;
	g_engine->_engine->animMoveZ = tempZ;

	return foundFlag;
}

void putAtObjet(int objIdx, int objIdxToPutAt) {
	WorldObject *objPtr = &g_engine->_engine->worldObjets[objIdx];
	const WorldObject *objPtrToPutAt = &g_engine->_engine->worldObjets[objIdxToPutAt];

	if (objPtrToPutAt->objIndex != -1) {
		const Object *actorToPutAtPtr = &g_engine->_engine->objectTable[objPtrToPutAt->objIndex];

		deleteInventoryObjet(objIdx);

		if (objPtr->objIndex == -1) {
			objPtr->x = actorToPutAtPtr->roomX;
			objPtr->y = actorToPutAtPtr->roomY;
			objPtr->z = actorToPutAtPtr->roomZ;
			objPtr->room = actorToPutAtPtr->room;
			objPtr->stage = actorToPutAtPtr->stage;
			objPtr->alpha = actorToPutAtPtr->alpha;
			objPtr->beta = actorToPutAtPtr->beta;
			objPtr->gamma = actorToPutAtPtr->gamma;

			objPtr->flags2 |= 0x4000;
			objPtr->flags |= 0x80;

			//      FlagGenereActiveList = 1;
			//      FlagRefreshAux2 = 1;
		} else {
			g_engine->_engine->currentProcessedActorPtr->roomX = actorToPutAtPtr->roomX;
			g_engine->_engine->currentProcessedActorPtr->roomY = actorToPutAtPtr->roomY;
			g_engine->_engine->currentProcessedActorPtr->roomZ = actorToPutAtPtr->roomZ;
			g_engine->_engine->currentProcessedActorPtr->room = actorToPutAtPtr->room;
			g_engine->_engine->currentProcessedActorPtr->stage = actorToPutAtPtr->stage;
			g_engine->_engine->currentProcessedActorPtr->alpha = actorToPutAtPtr->alpha;
			g_engine->_engine->currentProcessedActorPtr->beta = actorToPutAtPtr->beta;
			g_engine->_engine->currentProcessedActorPtr->gamma = actorToPutAtPtr->gamma;

			g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].flags2 |= 0x4000;
			g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].flags |= 0x80;

			//      FlagGenereActiveList = 1;
			//      FlagRefreshAux2 = 1;
		}

	} else {
		deleteInventoryObjet(objIdx);

		if (objPtr->objIndex == -1) {
			objPtr->x = objPtrToPutAt->x;
			objPtr->y = objPtrToPutAt->y;
			objPtr->z = objPtrToPutAt->z;
			objPtr->room = objPtrToPutAt->room;
			objPtr->stage = objPtrToPutAt->stage;
			objPtr->alpha = objPtrToPutAt->alpha;
			objPtr->beta = objPtrToPutAt->beta;
			objPtr->gamma = objPtrToPutAt->gamma;

			objPtr->flags2 |= 0x4000;
			objPtr->flags |= 0x80;

			//      FlagGenereActiveList = 1;
			//      FlagRefreshAux2 = 1;
		} else {
			g_engine->_engine->currentProcessedActorPtr->roomX = objPtrToPutAt->x;
			g_engine->_engine->currentProcessedActorPtr->roomY = objPtrToPutAt->y;
			g_engine->_engine->currentProcessedActorPtr->roomZ = objPtrToPutAt->z;
			g_engine->_engine->currentProcessedActorPtr->room = objPtrToPutAt->room;
			g_engine->_engine->currentProcessedActorPtr->stage = objPtrToPutAt->stage;
			g_engine->_engine->currentProcessedActorPtr->alpha = objPtrToPutAt->alpha;
			g_engine->_engine->currentProcessedActorPtr->beta = objPtrToPutAt->beta;
			g_engine->_engine->currentProcessedActorPtr->gamma = objPtrToPutAt->gamma;

			g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].flags2 |= 0x4000;
			g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].flags |= 0x80;

			//      FlagGenereActiveList = 1;
			//      FlagRefreshAux2 = 1;
		}
	}
}

void throwStoppedAt(int x, int z) {

	ZVStruct zvCopy;
	ZVStruct zvLocal;

	const uint8 *bodyPtr = hqrGet(g_engine->_engine->listBody, g_engine->_engine->currentProcessedActorPtr->bodyNum).data;

	giveZVObjet(bodyPtr, &zvLocal);

	int x2 = x;
	int y2 = g_engine->_engine->currentProcessedActorPtr->roomY / 2000 * 2000;
	int z2 = z;

	int foundPosition = 0;
	int step = 0;

	while (!foundPosition) {
		walkStep(0, -step, g_engine->_engine->currentProcessedActorPtr->beta + 0x200);
		copyZv(&zvLocal, &zvCopy);

		x2 = x + g_engine->_engine->animMoveX;
		z2 = z + g_engine->_engine->animMoveZ;

		zvCopy.ZVX1 += x2;
		zvCopy.ZVX2 += x2;

		zvCopy.ZVY1 += y2;
		zvCopy.ZVY2 += y2;

		zvCopy.ZVZ1 += z2;
		zvCopy.ZVZ2 += z2;

		if (!asmCheckListCol(&zvCopy, &g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room])) {
			foundPosition = 1;
		}

		if (foundPosition) {
			if (y2 < -500) {
				zvCopy.ZVY1 += 100; // is the object reachable ? (100 is Carnby height. If hard col at Y + 100, carnby can't reach that spot)
				zvCopy.ZVY2 += 100;

				if (!asmCheckListCol(&zvCopy, &g_engine->_engine->roomDataTable[g_engine->_engine->currentProcessedActorPtr->room])) {
					y2 += 2000;
					foundPosition = 0;
				} else {
					zvCopy.ZVY1 -= 100;
					zvCopy.ZVY2 -= 100;
				}
			}
		} else {
			step += 100;
		}
	}

	g_engine->_engine->currentProcessedActorPtr->worldX = x2;
	g_engine->_engine->currentProcessedActorPtr->roomX = x2;
	g_engine->_engine->currentProcessedActorPtr->worldY = y2;
	g_engine->_engine->currentProcessedActorPtr->roomY = y2;
	g_engine->_engine->currentProcessedActorPtr->worldZ = z2;
	g_engine->_engine->currentProcessedActorPtr->roomZ = z2;

	g_engine->_engine->currentProcessedActorPtr->stepX = 0;
	g_engine->_engine->currentProcessedActorPtr->stepZ = 0;

	g_engine->_engine->currentProcessedActorPtr->animActionType = 0;
	g_engine->_engine->currentProcessedActorPtr->speed = 0;
	g_engine->_engine->currentProcessedActorPtr->gamma = 0;

	giveZVObjet(bodyPtr, &g_engine->_engine->currentProcessedActorPtr->zv);

	g_engine->_engine->currentProcessedActorPtr->zv.ZVX1 += x2;
	g_engine->_engine->currentProcessedActorPtr->zv.ZVX2 += x2;
	g_engine->_engine->currentProcessedActorPtr->zv.ZVY1 += y2;
	g_engine->_engine->currentProcessedActorPtr->zv.ZVY2 += y2;
	g_engine->_engine->currentProcessedActorPtr->zv.ZVZ1 += z2;
	g_engine->_engine->currentProcessedActorPtr->zv.ZVZ2 += z2;

	g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].flags2 |= 0x4000;
	g_engine->_engine->worldObjets[g_engine->_engine->currentProcessedActorPtr->indexInWorld].flags2 &= 0xEFFF;

	addActorToBgInscrust(g_engine->_engine->currentProcessedActorIdx);
}

void startGame(int startupFloor, int startupRoom, int allowSystemMenu) {
	initEngine();

	initVars();

	loadFloor(startupFloor);

	g_engine->_engine->currentCamera = -1;

	loadRoom(startupRoom);

	g_engine->_engine->startGameVar1 = 0;
	g_engine->_engine->flagInitView = 2;

	setupCamera();

	mainLoop(allowSystemMenu, 1);

	/*freeScene();

	fadeOut(8,0);*/
}

static int drawTextOverlay() {
	int hasText = 0;
	int y = 183;

	g_engine->_engine->BBox3D4 = 199;
	g_engine->_engine->BBox3D1 = 319;
	g_engine->_engine->BBox3D3 = 0;

	Message *currentMessage = g_engine->_engine->messageTable;

	if (g_engine->_engine->lightOff == 0) {
		for (int i = 0; i < 5; i++) {
			if (currentMessage->string) {
				const int width = currentMessage->string->width;
				const int X = 160 - width / 2;
				const int Y = X + width;

				if (X < g_engine->_engine->BBox3D1) {
					g_engine->_engine->BBox3D1 = X;
				}

				if (Y > g_engine->_engine->BBox3D3) {
					g_engine->_engine->BBox3D3 = Y;
				}

				if (currentMessage->time++ > 55) {
					currentMessage->string = nullptr;
				} else {
					if (currentMessage->time < 26) {
						extSetFont(g_engine->_engine->ptrFont, 16);
					} else {
						extSetFont(g_engine->_engine->ptrFont, 16 + (currentMessage->time - 26) / 2);
					}

					renderText(X, y + 1, currentMessage->string->textPtr);
				}

				y -= 16;
				hasText = 1;
			}

			currentMessage++;
		}
	} else {
		for (int i = 0; i < 5; i++) {
			if (currentMessage->string) {
				const int width = currentMessage->string->width;
				const int X = 160 - width / 2;
				const int Y = X + width;

				if (X < g_engine->_engine->BBox3D1) {
					g_engine->_engine->BBox3D1 = X;
				}

				if (Y > g_engine->_engine->BBox3D3) {
					g_engine->_engine->BBox3D3 = Y;
				}

				if (currentMessage->time < 26) {
					extSetFont(g_engine->_engine->ptrFont, 15);
				} else {
					extSetFont(g_engine->_engine->ptrFont, 0);
				}

				renderText(X, y + 1, currentMessage->string->textPtr);

				y -= 16;
				hasText = 1;
			}

			currentMessage++;
		}
	}

	g_engine->_engine->BBox3D2 = y;
	return hasText;
}

static void setupScreen() {
	// screenBufferSize = 64800;

	// unkScreenVar2 = 3;

	// TODO: remain of screen init
}

static void loadPalette() {
	byte localPalette[768];

	if (g_engine->getGameId() == GID_AITD2) {
		pakLoad("ITD_RESS.PAK", 59, g_engine->_engine->aux);
	} else {
		pakLoad("ITD_RESS.PAK", AITD1_PALETTE_JEU, g_engine->_engine->aux);
	}
	copyPalette(g_engine->_engine->aux, g_engine->_engine->currentGamePalette);

	copyPalette(g_engine->_engine->currentGamePalette, localPalette);
	//  fadeInSub1(localPalette);

	// to finish
}

static void allocTextes() {
	int currentIndex;

	const Common::String lang(ConfMan.get("language"));
	for (int i = 0; i < LANGUAGE_NAME_SIZE; i++) {
		if (lang == languageShortNameTable[i] && Common::File::exists(languageNameTable[i])) {
			g_engine->_engine->languageNameString = languageNameTable[i];
			break;
		}
	}

	if (!g_engine->_engine->languageNameString) {
		error("Unable to detect language file..\n");
		assert(0);
	}

	g_engine->_engine->systemTextes.reset(checkLoadMallocPak(g_engine->_engine->languageNameString, 0));
	const int textLength = pakGetPakSize(g_engine->_engine->languageNameString, 0);

	for (currentIndex = 0; currentIndex < NUM_MAX_TEXT_ENTRY; currentIndex++) {
		g_engine->_engine->tabTextes[currentIndex].index = -1;
		g_engine->_engine->tabTextes[currentIndex].textPtr = nullptr;
		g_engine->_engine->tabTextes[currentIndex].width = 0;
	}

	byte *currentPosInTextes = g_engine->_engine->systemTextes.get();

	int textCounter = 0;

	while (currentPosInTextes < g_engine->_engine->systemTextes.get() + textLength) {
		currentIndex = *currentPosInTextes++;

		if (currentIndex == 26)
			break;

		if (currentIndex == '@') // start of string marker
		{
			int stringIndex = 0;

			while ((currentIndex = *currentPosInTextes++) >= '0' && currentIndex <= '9') // parse string number
			{
				stringIndex = stringIndex * 10 + currentIndex - 48;
			}

			if (currentIndex == ':') // start of string
			{
				byte *stringPtr = currentPosInTextes;

				do {
					currentPosInTextes++;
				} while (*(currentPosInTextes - 1) >= ' '); // detect the end of the string

				*(currentPosInTextes - 1) = 0; // add the end of string

				g_engine->_engine->tabTextes[textCounter].index = stringIndex;
				g_engine->_engine->tabTextes[textCounter].textPtr = (char *)stringPtr;
				g_engine->_engine->tabTextes[textCounter].width = extGetSizeFont((char *)stringPtr);

				textCounter++;
			}

			if (currentIndex == 26) {
				return;
			}
		}
	}
}

void runGame() {
	gfx_init();

#ifdef USE_IMGUI
	ImGuiCallbacks callbacks;
	callbacks.init = onImGuiInit;
	callbacks.render = onImGuiRender;
	callbacks.cleanup = onImGuiCleanup;
	g_system->setImGuiCallbacks(callbacks);
#endif

	switch (g_engine->getGameId()) {
	case GID_AITD1:
		g_engine->_engine->cVarsSize = 45;
		currentCVarTable = aitd1KnownCVars;
		break;
	case GID_JACK:
		g_engine->_engine->cVarsSize = 15;
		currentCVarTable = aitd2KnownCVars;
		break;
	case GID_AITD2:
	case GID_AITD3:
		g_engine->_engine->cVarsSize = 70;
		currentCVarTable = aitd2KnownCVars;
		break;
	default:
		break;
	}

	setupScreen();

	initMusicDriver();
	g_engine->_engine->musicConfigured = 1;
	g_engine->_engine->musicEnabled = g_engine->_mixer->isSoundTypeMuted(Audio::Mixer::kMusicSoundType) ? 0 : 1;
	g_engine->_engine->soundToggle = g_engine->_mixer->isSoundTypeMuted(Audio::Mixer::kSFXSoundType) ? 0 : 1;
	g_engine->_engine->detailToggle = 1;

	initCopyBox(g_engine->_engine->aux2, g_engine->_engine->logicalScreen);

	switch (g_engine->getGameId()) {
	case GID_AITD3: {
		g_engine->_engine->ptrFont = checkLoadMallocPak("ITD_RESS.PAK", 1);
		break;
	}
	case GID_JACK:
	case GID_AITD2: {
		g_engine->_engine->ptrFont = checkLoadMallocPak("ITD_RESS.PAK", 1);
		break;
	}
	case GID_AITD1: {
		g_engine->_engine->ptrFont = checkLoadMallocPak("ITD_RESS.PAK", AITD1_ITDFONT);
		break;
	}
	case GID_TIMEGATE:
		g_engine->_engine->ptrFont = checkLoadMallocPak("ITD_RESS.PAK", 2);
		break;
	default:
		assert(0);
	}

	extSetFont(g_engine->_engine->ptrFont, 14);

	if (g_engine->getGameId() == GID_AITD1) {
		setFontSpace(2, 0);
	} else {
		setFontSpace(2, 1);
	}

	switch (g_engine->getGameId()) {
	case GID_JACK:
	case GID_AITD2:
	case GID_AITD3: {
		g_engine->_engine->ptrCadre = checkLoadMallocPak("ITD_RESS.PAK", 0);
		break;
	}
	case GID_AITD1: {
		g_engine->_engine->ptrCadre = checkLoadMallocPak("ITD_RESS.PAK", AITD1_CADRE_SPF);
		break;
	}
	default:
		break;
	}

	loadFromItd("PRIORITY.ITD", g_engine->_engine->ptrPrioritySample, sizeof(g_engine->_engine->ptrPrioritySample));

	// read cvars definitions
	{
		Common::File f;
		if (!f.open("DEFINES.ITD")) {
			error("Fitd::runGame: can't open DEFINES.ITD");
		}
		for (int i = 0; i < g_engine->_engine->cVarsSize; i++) {
			g_engine->_engine->cVars[i] = f.readSint16BE();
		}
		f.close();
	}

	allocTextes();

	paletteFill(g_engine->_engine->currentGamePalette, 0, 0, 0);
	loadPalette();

	// If a savegame was selected from the launcher, load it
	const int saveSlot = ConfMan.getInt("save_slot");

	// for (size_t i = 0; i < 12; i++) {
	// 	HqData data = hqrGet(g_engine->_engine->listMus, i); // Preload music entries
	// 	Common::DumpFile f;
	// 	Common::String path(Common::String::format("mus%02d.dat", i));
	// 	debug("Writing music file: %s\n", path.c_str());
	// 	f.open(path.c_str());
	// 	f.write(data.data, data.size);
	// 	f.close();
	// }

	switch (g_engine->getGameId()) {
	case GID_AITD1:
		aitd1Start(saveSlot);
		break;
	case GID_JACK:
		jackStart(saveSlot);
		break;
	case GID_AITD2:
		aitd2Start(saveSlot);
		break;
	case GID_AITD3:
		aitd3Start(saveSlot);
		break;
	// case GID_TIMEGATE:
	// 	startGame(0, 5, 1);
	// 	break;
	default:
		error("Unknown game");
		break;
	}

#ifdef USE_IMGUI
	g_system->setImGuiCallbacks(ImGuiCallbacks());
#endif

	g_engine->_mixer->stopAll();

	destroyMusicDriver();

	gfx_deinit();
}

} // namespace Fitd
