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

#include "fitd/aitd1.h"
#include "fitd/aitd2.h"
#include "fitd/fitd.h"
#include "fitd/jack.h"
#include "fitd/aitd_box.h"
#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/costable.h"
#include "fitd/file_access.h"
#include "fitd/floor.h"
#include "fitd/font.h"
#include "fitd/game_time.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/inventory.h"
#include "fitd/life.h"
#include "fitd/main_loop.h"
#include "fitd/object.h"
#include "fitd/pak.h"
#include "fitd/sequence.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"
#include "fitd/zv.h"
#include "common/debug.h"
#include "common/file.h"

namespace Fitd {

const unsigned char defaultPalette[0x30] = {
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

// const unsigned char defaultPaletteAITD3[0x30] =
// 	{
// 		0x00,
// 		0x00,
// 		0x00,
// 		0xFC,
// 		0xFC,
// 		0xFC,
// 		0x30,
// 		0x30,
// 		0x38,
// 		0xC0,
// 		0xBC,
// 		0xFC,
// 		0x78,
// 		0x58,
// 		0x3C,
// 		0x00,
// 		0x00,
// 		0x00,
// 		0xF0,
// 		0x70,
// 		0x10,
// 		0xFC,
// 		0xFC,
// 		0xFC,
// 		0x48,
// 		0x50,
// 		0x60,
// 		0xC4,
// 		0x54,
// 		0x5C,
// 		0x54,
// 		0x94,
// 		0x54,
// 		0x54,
// 		0xBC,
// 		0xFC,
// 		0xFC,
// 		0x88,
// 		0x54,
// 		0xAC,
// 		0x54,
// 		0xFC,
// 		0xFC,
// 		0xFC,
// 		0xFC,
// 		0xFC,
// 		0xFC,
// 		0xF8};

void executeFoundLife(int objIdx) {
	int var_2;
	int actorIdx;
	int lifeOffset;
	int currentActorIdx;
	int currentActorLifeIdx;
	int currentActorLifeNum;
	int foundLife;
	tObject *currentActorPtr;
	tObject *currentActorLifePtr;

	if (objIdx == -1)
		return;

	foundLife = ListWorldObjets[objIdx].foundLife;

	if (ListWorldObjets[objIdx].foundLife == -1)
		return;

	currentActorPtr = currentProcessedActorPtr;
	currentActorIdx = currentProcessedActorIdx;
	currentActorLifeIdx = currentLifeActorIdx;
	currentActorLifePtr = currentLifeActorPtr;
	currentActorLifeNum = currentLifeNum;

	if (currentLifeNum != -1) {
		lifeOffset = (currentLifePtr - HQR_Get(listLife, currentActorLifeNum)) / 2;
	}

	var_2 = 0;

	actorIdx = ListWorldObjets[objIdx].objIndex;

	if (actorIdx == -1) {
		tObject *currentActorEntryPtr = &objectTable[NUM_MAX_OBJECT - 1];
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
			currentActorEntryPtr = &objectTable[NUM_MAX_OBJECT - 1];
		}

		actorIdx = currentActorEntry;
		var_2 = 1;

		currentProcessedActorPtr = &objectTable[actorIdx];
		currentLifeActorPtr = &objectTable[actorIdx];
		currentProcessedActorIdx = actorIdx;
		currentLifeActorIdx = actorIdx;

		currentProcessedActorPtr->indexInWorld = objIdx;
		currentProcessedActorPtr->life = -1;
		currentProcessedActorPtr->bodyNum = -1;
		currentProcessedActorPtr->_flags = 0;
		currentProcessedActorPtr->trackMode = -1;
		currentProcessedActorPtr->room = -1;
		currentProcessedActorPtr->lifeMode = -1;
		currentProcessedActorPtr->ANIM = -1;
	}

	processLife(foundLife, true);

	if (var_2) {
		currentProcessedActorPtr->indexInWorld = -1;
	}

	currentProcessedActorPtr = currentActorPtr;
	currentProcessedActorIdx = currentActorIdx;
	currentLifeActorIdx = currentActorLifeIdx;
	currentLifeActorPtr = currentActorLifePtr;

	if (currentActorLifeNum != -1) {
		currentLifeNum = currentActorLifeNum;
		currentLifePtr = HQR_Get(listLife, currentLifeNum) + lifeOffset * 2;
	}
}

void initCopyBox(char *var0, char *var1) {
	screenSm1 = var0;
	screenSm2 = var0;

	screenSm3 = var1;
	screenSm4 = var1;
	screenSm5 = var1;
}

void freeAll(void) {
	/*  HQR_Free(hqrUnk);

	HQR_Free(listSamp);

	HQR_Free(listMus);

	free(languageData);

	free(tabTextes);

	free(priority);

	free(aitdBoxGfx);

	free(fontData);

	free(bufferAnim);

	if(aux != aux3)
	{
	free(aux);
	}

	free(aux2);*/

	// TODO: implement all the code that restore the interrupts & all
}

static void turnPageForward() {
}

static void turnPageBackward() {
}

void readBook(int index, int type) {
	freezeTime();

	switch (g_engine->getGameId()) {
	case GID_AITD1:
		aitd1_readBook(index, type);
		break;
	case GID_JACK:
		JACK_ReadBook(index, type);
		break;
	case GID_AITD2:
		AITD2_ReadBook(index, type);
		break;
	default:
		assert(0);
	}

	unfreezeTime();
}

int lire(int index, int startx, int top, int endx, int bottom, int demoMode, int color, int shadow) {
	bool lastPageReached = false;
	char tabString[] = "    ";
	int firstpage = 1;
	int page = 0;
	int quit = 0;
	int previousPage = -1;
	int var_1C3;
	char *ptrpage[100];
	int currentTextIdx;
	int maxStringWidth;
	char *textPtr;

	extSetFont(PtrFont, color);

	maxStringWidth = endx - startx + 4;

	int textIndexMalloc = HQ_Malloc(HQ_Memory, getPakSize(languageNameString, index) + 300);
	textPtr = (char *)HQ_PtrMalloc(HQ_Memory, textIndexMalloc);

	if (!loadPak(languageNameString, index, (char *)textPtr)) {
		error("Failed to load pak %s", languageNameString);
	}

	memset(ptrpage, 0, sizeof(char *) * 100);
	ptrpage[0] = textPtr;

	//  LastSample = -1;
	//  LastPriority = -1;

	while (!g_engine->shouldQuit() && !quit) {
		char *ptrt;
		int currentTextY;
		fastCopyScreen(aux, logicalScreen);
		process_events();
		setClip(startx, top, endx, bottom);

		ptrt = ptrpage[page];

		currentTextY = top;
		lastPageReached = false;

		while (currentTextY <= bottom - 16) {
			int line_type = 1;
			int var_1BA = 0;
			int currentStringWidth;
			int currentTextX;

			regularTextEntryStruct *currentText = textTable;

			int numWordInLine = 0;

			int interWordSpace = 0;

			while (true) {
				while (*ptrt == '#') {
					// char* var_1BE = var_1C2;
					ptrt++;

					switch (*(ptrt++)) {
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
							currentTextIdx = (currentTextIdx * 10 + *ptrt - 48);
							ptrt++;
						}

						if (loadPak("ITD_RESS.PAK", 9, aux2)) {
							assert(0); // when is this used?
									   /*  var_C = printTextSub3(currentTextIdx,aux2);
									   var_A = printTextSub4(currentTextIdx,aux2);

									   if(currentTextY + var_A > bottom)
									   {
									   var_1C2 = var_1BE;

									   goto pageChange;
									   }
									   else
									   {
									   printTextSub5((((right-left)/2)+left)-var_C, currentTextY, currentTextIdx, aux2);
									   currentTextY = var_A;
									   }*/
						}

						break;
					}
					}
				}

				currentText->textPtr = ptrt;

				do {
					var_1C3 = *((unsigned char *)ptrt++);
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

				if (((var_1C3 == 13) || (var_1C3 == 0)) && (*ptrt < ' ')) {
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

			currentText = textTable;

			if (line_type & 8) // center
			{
				currentTextX = startx + ((maxStringWidth - var_1BA) / 2);
			} else {
				currentTextX = startx;
			}

			for (int i = 0; i < numWordInLine; i++) {
				renderText(currentTextX, currentTextY, logicalScreen, currentText->textPtr);
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
				affSpfI(startx - 19, 185, 12, PtrCadre);
			}

			if (!lastPageReached) {
				affSpfI(endx + 4, 185, 11, PtrCadre);
			}
		}

		if (demoMode == 2) {
			if (page > 0) {
				affSpfI(startx - 3, 191, 13, PtrCadre);
			}

			if (!lastPageReached) {
				affSpfI(endx - 10, 191, 14, PtrCadre);
			}
		}

		if (firstpage) {
			if (demoMode != 1) {
				gfx_copyBlockPhys((unsigned char *)logicalScreen, 0, 0, 320, 200);
				fadeInPhys(16, 0);
			} else {
				if (turnPageFlag) {
					turnPageForward();
				} else {
					gfx_copyBlockPhys((unsigned char *)logicalScreen, 0, 0, 320, 200);
				}
			}

			firstpage = 0;
		} else {
			if (turnPageFlag) {
				if (previousPage < page) {
					turnPageForward();
				} else {
					turnPageBackward();
				}
			} else {
				gfx_copyBlockPhys((unsigned char *)logicalScreen, 0, 0, 320, 200);
			}
		}

		osystem_drawBackground();

		if (demoMode != 1) // mode != 1: normal behavior (user can flip pages)
		{
			do {
				process_events();
			} while (key || JoyD || Click);

			while (1) {
				process_events();
				localKey = key;
				localJoyD = JoyD;
				localClick = Click;

				if ((localKey == 1) || localClick) {
					quit = 1;
					break;
				}

				if ((demoMode == 2) && (localKey == 0x1C)) {
					quit = 1;
					break;
				}

				// flip to next page
				if (JoyD & 0xA || localKey == 0x1C) {
					if (!lastPageReached) {
						previousPage = page;
						page++;

						if (demoMode == 2) {
							playSound(CVars[getCVarsIdx(SAMPLE_PAGE)]);
							LastSample = -1;
							LastPriority = -1;
						}
						break;
					} else {
						if (localKey == 0x1C) {
							quit = 1;
							break;
						}
					}
				}

				// flip to previous page
				if (JoyD & 5) {
					if (page > 0) {
						previousPage = page;
						page--;
						if (demoMode == 2) {
							playSound(CVars[getCVarsIdx(SAMPLE_PAGE)]);
							LastSample = -1;
							LastPriority = -1;
						}
						break;
					}
				}
			}
		} else // Demo mode: pages automatically flips
		{
			unsigned int var_6;
			startChrono(&var_6);

			do {
				process_events();
				if (evalChrono(&var_6) > 300) {
					break;
				}
			} while (!key && !Click);

			if (key || Click) {
				quit = 1;
			}

			if (!lastPageReached) {
				page++;
				playSound(CVars[getCVarsIdx(SAMPLE_PAGE)]);
				LastSample = -1;
			} else {
				quit = 1;
				demoMode = 0;
			}
		}
	}

	// HQ_Free_Malloc(HQ_Memory, textIndexMalloc);

	return (demoMode);
}

void initEngine(void) {
	uint8 *pObjectData;
	uint8 *pObjectDataBackup;
	unsigned long int objectDataSize;
	Common::File f;
	int choosePersoBackup;

	f.open("OBJETS.ITD");
	objectDataSize = f.size();

	pObjectDataBackup = pObjectData = (uint8 *)malloc(objectDataSize);
	assert(pObjectData);
	f.read(pObjectData, objectDataSize);
	f.close();

	maxObjects = READ_LE_U16(pObjectData);
	pObjectData += 2;

	if (g_engine->getGameId() == GID_AITD1) {
		ListWorldObjets.resize(300);
	} else {
		ListWorldObjets.resize(maxObjects);
	}

	for (int i = 0; i < maxObjects; i++) {
		ListWorldObjets[i].objIndex = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].body = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].flags = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].typeZV = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].foundBody = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].foundName = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].flags2 = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].foundLife = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].x = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].y = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].z = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].alpha = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].beta = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].gamma = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].stage = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].room = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].lifeMode = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].life = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].floorLife = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].anim = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].frame = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].animType = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].animInfo = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].trackMode = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].trackNumber = READ_LE_U16(pObjectData);
		pObjectData += 2;

		ListWorldObjets[i].positionInTrack = READ_LE_U16(pObjectData);
		pObjectData += 2;

		if (g_engine->getGameId() >= GID_JACK) {
			ListWorldObjets[i].mark = READ_LE_U16(pObjectData);
			pObjectData += 2;
		}
		ListWorldObjets[i].flags |= 0x20;
	}

	free(pObjectDataBackup);

	vars = (int16 *)loadFromItd("VARS.ITD");

	varSize = fileSize;

	if (g_engine->getGameId() == GID_AITD1) {
		choosePersoBackup = CVars[getCVarsIdx(CHOOSE_PERSO)]; // backup hero selection
	}

	f.open("DEFINES.ITD");

	///////////////////////////////////////////////
	{
		f.read(&CVars[0], CVars.size() * 2);
		f.close();

		for (int i = 0; i < CVars.size(); i++) {
			CVars[i] = ((CVars[i] & 0xFF) << 8) | ((CVars[i] & 0xFF00) >> 8);
		}
	}
	//////////////////////////////////////////////

	if (g_engine->getGameId() == GID_AITD1) {
		CVars[getCVarsIdx(CHOOSE_PERSO)] = choosePersoBackup;
	}

	listLife = HQR_InitRessource("LISTLIFE.PAK", 65000, 100);
	listTrack = HQR_InitRessource("LISTTRAK.PAK", 20000, 100);

	// TODO: missing dos memory check here

	if (g_engine->getGameId() == GID_AITD1) {
		listBody = HQR_InitRessource(listBodySelect[CVars[getCVarsIdx(CHOOSE_PERSO)]], 37000, 50); // was calculated from free mem size
		listAnim = HQR_InitRessource(listAnimSelect[CVars[getCVarsIdx(CHOOSE_PERSO)]], 30000, 80); // was calculated from free mem size
	} else {
		listBody = HQR_InitRessource("LISTBODY.PAK", 37000, 50); // was calculated from free mem size
		listAnim = HQR_InitRessource("LISTANIM.PAK", 30000, 80); // was calculated from free mem size

		listMatrix = HQR_InitRessource("LISTMAT.PAK", 64000, 5);
	}
	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		objectTable[i].indexInWorld = -1;
	}

	if (g_engine->getGameId() == GID_AITD1) {
		currentWorldTarget = CVars[getCVarsIdx(WORLD_NUM_PERSO)];
	}
}

static void initVarsSub1(void) {
	for (int i = 0; i < 5; i++) {
		messageTable[i].string = NULL;
	}
}

void initVars() {
	giveUp = 0;

	currentInventory = 0;

	for (int i = 0; i < NUM_MAX_INVENTORY; i++) {
		numObjInInventoryTable[i] = 0;
		inHandTable[i] = -1;
	}

	action = 0;

	genVar1 = genVar2;
	genVar3 = genVar4;

	genVar5 = 0;
	genVar6 = 0;

	LastSample = -1;
	nextSample = -1;
	LastPriority = -1;
	currentMusic = -1;
	nextMusic = -1;

	lightOff = 0;
	lightVar2 = 0;

	currentCameraTargetActor = -1;
	currentWorldTarget = -1;

	statusScreenAllowed = 1;

	initVarsSub1();
}

static void loadCamera(int cameraIdx) {
	int useSpecial = -1;

	Common::String name = Common::String::format("CAMERA%02d.PAK", g_currentFloor);
	// strcat(name,".PAK");

	if (g_engine->getGameId() == GID_AITD1) {
		if (CVars[getCVarsIdx(KILLED_SORCERER)] == 1) {
			switch (g_currentFloor) {
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

	if (!loadPak(name.c_str(), cameraIdx, aux)) {
		error("Cannot load pak %s", name.c_str());
	}

	if (g_engine->getGameId() == GID_AITD3) {
		memmove(aux, aux + 4, 64000 + 0x300);
	}

	if (g_engine->getGameId() >= GID_JACK) {
		copyPalette((unsigned char *)aux + 64000, currentGamePalette);

		if (g_engine->getGameId() == GID_AITD3) {
			// memcpy(palette,defaultPaletteAITD3,0x30);
		} else {
			memcpy(currentGamePalette, defaultPalette, 0x30);
			convertPaletteIfRequired((unsigned char *)currentGamePalette);
		}

		gfx_setPalette(currentGamePalette);
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

maskStruct g_maskBuffers[10][10];

static void loadMask(int cameraIdx) {
	if (g_engine->getGameId() == GID_TIMEGATE)
		return;

	Common::String name = Common::String::format("MASK%02d.PAK", g_currentFloor);

	if (g_MaskPtr) {
		free(g_MaskPtr);
	}

	g_MaskPtr = (unsigned char *)loadPak(name.c_str(), cameraIdx);

	for (int i = 0; i < cameraDataTable[currentCamera]->numViewedRooms; i++) {
		cameraViewedRoomStruct *pRoomView = &cameraDataTable[currentCamera]->viewedRoomTable[i];
		unsigned char *pViewedRoomMask = g_MaskPtr + READ_LE_U32(g_MaskPtr + i * 4);

		for (int j = 0; j < pRoomView->numMask; j++) {
			unsigned char *pMaskData = pViewedRoomMask + READ_LE_U32(pViewedRoomMask + j * 4);

			maskStruct *pDestMask = &g_maskBuffers[i][j];

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
				uint16 uNumEntryForLine = READ_LE_U16(pMaskData);
				pMaskData += 2;

				// unsigned char *pSourceBuffer = (unsigned char *)aux;

				int offset = pDestMask->x1 + pDestMask->y1 * 320 + k * 320;

				for (int l = 0; l < uNumEntryForLine; l++) {
					unsigned char uNumSkip = *(pMaskData++);
					unsigned char uNumCopy = *(pMaskData++);

					offset += uNumSkip;

					for (int m = 0; m < uNumCopy; m++) {
						pDestMask->mask[offset] = 0xFF;
						offset++;
					}
				}
			}

			osystem_createMask(pDestMask->mask, i, j, (unsigned char *)aux, pDestMask->x1, pDestMask->y1, pDestMask->x2, pDestMask->y2);
		}
	}
}

void fillpoly(int16 *datas, int n, unsigned char c);
extern unsigned char *polyBackBuffer;

static void createAITD1Mask() {
	for (int viewedRoomIdx = 0; viewedRoomIdx < cameraDataTable[currentCamera]->numViewedRooms; viewedRoomIdx++) {
		cameraViewedRoomStruct *pcameraViewedRoomData = &cameraDataTable[currentCamera]->viewedRoomTable[viewedRoomIdx];

		char *data2 = room_PtrCamera[currentCamera] + pcameraViewedRoomData->offsetToMask;
		char *data = data2;
		data += 2;

		int numMask = *(int16 *)(data2);

		for (int maskIdx = 0; maskIdx < numMask; maskIdx++) {
			maskStruct *pDestMask = &g_maskBuffers[viewedRoomIdx][maskIdx];
			memset(pDestMask->mask, 0, 320 * 200);
			polyBackBuffer = &pDestMask->mask[0];

			char *src = data2 + *(uint16 *)(data + 2);

			// int numMaskZone = *(int16 *)(data);

			int minX = 319;
			int maxX = 0;
			int minY = 199;
			int maxY = 0;

			/*if(isBgOverlayRequired( actorPtr->zv.ZVX1 / 10, actorPtr->zv.ZVX2 / 10,
			actorPtr->zv.ZVZ1 / 10, actorPtr->zv.ZVZ2 / 10,
			data+4,
			*(int16*)(data) ))*/
			{
				int numMaskPoly = *(int16 *)src;
				src += 2;

				for (int maskPolyIdx = 0; maskPolyIdx < numMaskPoly; maskPolyIdx++) {
					int numPoints = *(int16 *)(src);
					src += 2;

					memcpy(cameraBuffer, src, numPoints * 4);

					fillpoly((int16 *)src, numPoints, 0xFF);

					for (int verticeId = 0; verticeId < numPoints; verticeId++) {
						short verticeX = *(short *)(src + verticeId * 4 + 0);
						short verticeY = *(short *)(src + verticeId * 4 + 2);

						minX = MIN(minX, (int)verticeX);
						minY = MIN(minY, (int)verticeY);
						maxX = MAX(maxX, (int)verticeX);
						maxY = MAX(maxY, (int)verticeY);
					}

					src += numPoints * 4;
					// drawBgOverlaySub2(param);
				}

				//      blitOverlay(src);

				polyBackBuffer = nullptr;
			}

			osystem_createMask(pDestMask->mask, viewedRoomIdx, maskIdx, (unsigned char *)aux, minX - 1, minY - 1, maxX + 1, maxY + 1);

			int numOverlay = *(int16 *)(data);
			data += 2;
			data += ((numOverlay * 4) + 1) * 2;
		}

		/*		unsigned char* pViewedRoomMask = g_MaskPtr + READ_LE_U32(g_MaskPtr + i*4);

		for(int j=0; j<pRoomView->numMask; j++)
		{
		unsigned char* pMaskData = pViewedRoomMask + READ_LE_U32(pViewedRoomMask + j*4);

		maskStruct* pDestMask = &g_maskBuffers[i][j];

		memset(pDestMask->mask, 0, 320*200);

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

		for(int k=0; k<pDestMask->deltaY; k++)
		{
		u16 uNumEntryForLine = READ_LE_U16(pMaskData);
		pMaskData += 2;

		unsigned char* pDestBuffer = pDestMask->mask;
		unsigned char* pSourceBuffer = (unsigned char*)aux;

		int offset = pDestMask->x1 + pDestMask->y1 * 320 + k * 320;

		for(int l=0; l<uNumEntryForLine; l++)
		{
		unsigned char uNumSkip = *(pMaskData++);
		unsigned char uNumCopy = *(pMaskData++);

		offset += uNumSkip;

		for(int m=0; m<uNumCopy; m++)
		{
		pDestBuffer[offset] = 0xFF;
		offset++;
		}
		}
		}

		osystem_createMask(pDestMask->mask, i, j, (unsigned char*)aux, pDestMask->x1, pDestMask->y1, pDestMask->x2, pDestMask->y2);
		}*/
	}

	polyBackBuffer = NULL;
}

static int isInViewList(int value) {
	char *ptr = currentCameraVisibilityList;
	int var;

	while ((var = *(ptr++)) != -1) {
		if (value == var) {
			return (1);
		}
	}

	return (0);
}

// setup visibility list
static void setupCameraSub1() {
	uint32 i;
	int j;
	int var_10;

	char *dataTabPos = currentCameraVisibilityList;

	*dataTabPos = -1;

	// visibility list: add linked rooms
	for (i = 0; i < roomDataTable[currentRoom].numSceZone; i++) {
		if (roomDataTable[currentRoom].sceZoneTable[i].type == 0) {
			var_10 = roomDataTable[currentRoom].sceZoneTable[i].parameter;
			if (!isInViewList(var_10)) {
				*(dataTabPos++) = var_10;
				*(dataTabPos) = -1;
			}
		}
	}

	// visibility list: add room seen by the current camera
	for (j = 0; j < cameraDataTable[currentCamera]->numViewedRooms; j++) {
		if (!isInViewList(cameraDataTable[currentCamera]->viewedRoomTable[j].viewedRoomIdx)) {
			*(dataTabPos++) = (char)cameraDataTable[currentCamera]->viewedRoomTable[j].viewedRoomIdx;
			*(dataTabPos) = -1;
		}
	}
}

static void deleteObjet(int index) // remove actor
{
	tObject *actorPtr = &objectTable[index];

	if (actorPtr->indexInWorld == -2) // flow
	{
		actorPtr->indexInWorld = -1;

		if (actorPtr->ANIM == 4) {
			CVars[getCVarsIdx(FOG_FLAG)] = 0;
		}

		// HQ_Free_Malloc(HQ_Memory,actorPtr->FRAME);
	} else {
		if (actorPtr->indexInWorld >= 0) {
			tWorldObject *objectPtr = &ListWorldObjets[actorPtr->indexInWorld];

			objectPtr->objIndex = -1;
			actorPtr->indexInWorld = -1;

			objectPtr->body = actorPtr->bodyNum;
			objectPtr->anim = actorPtr->ANIM;
			objectPtr->frame = actorPtr->FRAME;
			objectPtr->animType = actorPtr->animType;
			objectPtr->animInfo = actorPtr->animInfo;
			objectPtr->flags = actorPtr->_flags & ~AF_BOXIFY;
			objectPtr->flags |= AF_SPECIAL * actorPtr->dynFlags; // ugly hack, need rewrite
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

			actorTurnedToObj = 1;
		}
	}
}

bool pointRotateEnable = true;

int pointRotateCosX;
int pointRotateSinX;
int pointRotateCosY;
int pointRotateSinY;
int pointRotateCosZ;
int pointRotateSinZ;

static void setupPointRotate(int alpha, int beta, int gamma) {
	pointRotateEnable = true;

	pointRotateCosX = cosTable[alpha & 0x3FF];
	pointRotateSinX = cosTable[((alpha & 0x3FF) + 0x100) & 0x3FF];

	pointRotateCosY = cosTable[beta & 0x3FF];
	pointRotateSinY = cosTable[((beta & 0x3FF) + 0x100) & 0x3FF];

	pointRotateCosZ = cosTable[gamma & 0x3FF];
	pointRotateSinZ = cosTable[((gamma & 0x3FF) + 0x100) & 0x3FF];
}

static void pointRotate(int x, int y, int z, int *destX, int *destY, int *destZ) {
	if (pointRotateEnable) {
		{
			int tempX = x;
			int tempY = y;
			x = ((((tempX * pointRotateSinZ) - (tempY * pointRotateCosZ))) >> 16) << 1;
			y = ((((tempX * pointRotateCosZ) + (tempY * pointRotateSinZ))) >> 16) << 1;
		}

		{
			int tempX = x;
			int tempZ = z;

			x = ((((tempX * pointRotateSinY) - (tempZ * pointRotateCosY))) >> 16) << 1;
			z = ((((tempX * pointRotateCosY) + (tempZ * pointRotateSinY))) >> 16) << 1;
		}

		{
			int tempY = y;
			int tempZ = z;
			y = ((((tempY * pointRotateSinX) - (tempZ * pointRotateCosX))) >> 16) << 1;
			z = ((((tempY * pointRotateCosX) + (tempZ * pointRotateSinX))) >> 16) << 1;
		}

		*destX = x;
		*destY = y;
		*destZ = z;
	}
}

static void zvRotSub(int X, int Y, int Z, int alpha, int beta, int gamma) {
	if (alpha || beta || gamma) {
		setupPointRotate(alpha, beta, gamma);
		pointRotate(X, Y, Z, &animMoveX, &animMoveY, &animMoveZ);
	} else {
		animMoveX = X;
		animMoveY = Y;
		animMoveZ = Z;
	}
}

void getZvRot(char *bodyPtr, ZVStruct *zvPtr, int alpha, int beta, int gamma) {
	int X1 = 32000;
	int Y1 = 32000;
	int Z1 = 32000;

	int X2 = -32000;
	int Y2 = -32000;
	int Z2 = -32000;

	int i;
	int tempX;
	int tempY;
	int tempZ;

	giveZVObjet(bodyPtr, zvPtr);

	for (i = 0; i < 8; i++) {
		switch (i) {
		case 0: {
			tempX = zvPtr->ZVX1;
			tempY = zvPtr->ZVY1;
			tempZ = zvPtr->ZVZ1;
			break;
		}
		case 1: {
			tempZ = zvPtr->ZVZ2;
			break;
		}
		case 2: {
			tempX = zvPtr->ZVX2;
			break;
		}
		case 3: {
			tempZ = zvPtr->ZVZ1;
			break;
		}
		case 4: {
			tempY = zvPtr->ZVY2;
			break;
		}
		case 5: {
			tempX = zvPtr->ZVX1;
			break;
		}
		case 6: {
			tempZ = zvPtr->ZVZ2;
			break;
		}
		case 7: {
			tempX = zvPtr->ZVX2;
			break;
		}
		}

		zvRotSub(tempX, tempY, tempZ, alpha, beta, gamma);

		if (animMoveX < X1)
			X1 = animMoveX;

		if (animMoveX > X2)
			X2 = animMoveX;

		if (animMoveY < Y1)
			Y1 = animMoveY;

		if (animMoveY > Y2)
			Y2 = animMoveY;

		if (animMoveZ < Z1)
			Z1 = animMoveZ;

		if (animMoveZ > Z2)
			Z2 = animMoveZ;
	}

	zvPtr->ZVX1 = X1;
	zvPtr->ZVX2 = X2;
	zvPtr->ZVY1 = Y1;
	zvPtr->ZVY2 = Y2;
	zvPtr->ZVZ1 = Z1;
	zvPtr->ZVZ2 = Z2;
}

void copyZv(ZVStruct *source, ZVStruct *dest) {
	memcpy(dest, source, sizeof(ZVStruct));
}

static void setupCameraSub4(void) {
	fastCopyScreen(aux, aux2);

	// TODO: implementer la suite
}

void setMoveMode(int trackMode, int trackNumber) {
	currentProcessedActorPtr->trackMode = trackMode;

	switch (trackMode) {
	case 2: {
		currentProcessedActorPtr->trackNumber = trackNumber;
		currentProcessedActorPtr->MARK = -1;
		break;
	}
	case 3: {
		currentProcessedActorPtr->trackNumber = trackNumber;
		currentProcessedActorPtr->positionInTrack = 0;
		currentProcessedActorPtr->MARK = -1;
		break;
	}
	}
}

int16 cameraVisibilityVar = 0;

int IsInCamera(int roomNumber) {
	int numZone = cameraDataTable[currentCamera]->numViewedRooms;

	for (int i = 0; i < numZone; i++) {
		if (cameraDataTable[currentCamera]->viewedRoomTable[i].viewedRoomIdx == roomNumber) {
			cameraVisibilityVar = i;
			return (1);
		}
	}

	cameraVisibilityVar = -1;

	return 0;
}

int IsInCamRectTestAITD2(int X, int Z) // TODO: not 100% exact
{
	// if(changeCameraSub1(X,X,Z,Z,&cameraDataTable[currentCamera]->cameraZoneDefTable[cameraVisibilityVar]))
	return 1;

	return 0;
}

int updateActorAitd2Only(int actorIdx) {
	tObject *currentActor = &objectTable[actorIdx];

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
		tObject *pObject = &objectTable[i];

		if (pObject->indexInWorld == -1) {
			continue;
		}

		pObject->lifeMode &= ~4;

		if (pObject->stage == g_currentFloor) {
			switch (pObject->lifeMode) {
			case 0: // OFF
				break;
			case 1: // STAGE
				continue;
			case 2: // ROOM
				if (pObject->room == currentRoom) {
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

	for (int i = 0; i < maxObjects; i++) {
		tWorldObject *currentObject = &ListWorldObjets[i];

		if (currentObject->objIndex != -1) {
			if (currentWorldTarget == i) {
				currentCameraTargetActor = currentObject->objIndex;
			}
		} else {
			if (currentObject->stage == g_currentFloor) {
				if (currentObject->life != -1) {
					if (currentObject->lifeMode != -1) {
						int actorIdx;
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
							if (currentObject->room != currentRoom) {
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
						actorIdx = copyObjectToActor(currentObject->body, currentObject->typeZV, currentObject->foundName,
													 currentObject->flags & 0xFFDF,
													 currentObject->x, currentObject->y, currentObject->z,
													 currentObject->stage, currentObject->room,
													 currentObject->alpha, currentObject->beta, currentObject->gamma,
													 currentObject->anim,
													 currentObject->frame, currentObject->animType, currentObject->animInfo);

						currentObject->objIndex = actorIdx;

						if (actorIdx != -1) {
							currentProcessedActorPtr = &objectTable[actorIdx];
							currentProcessedActorIdx = actorIdx;

							if (currentWorldTarget == i) {
								currentCameraTargetActor = currentProcessedActorIdx;
							}

							currentProcessedActorPtr->dynFlags = (currentObject->flags & 0x20) / 0x20; // recheck
							currentProcessedActorPtr->life = currentObject->life;
							currentProcessedActorPtr->lifeMode = currentObject->lifeMode;

							currentProcessedActorPtr->indexInWorld = i;

							setMoveMode(currentObject->trackMode, currentObject->trackNumber);

							currentProcessedActorPtr->positionInTrack = currentObject->positionInTrack;

							if (g_engine->getGameId() != GID_AITD1) {
								currentProcessedActorPtr->MARK = currentObject->mark;
							}

							actorTurnedToObj = 1;
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
	tObject *currentActor = objectTable;
	tWorldObject *currentObject;

	if (g_engine->getGameId() > GID_JACK) {
		updateAllActorAndObjectsAITD2();
		return;
	}
	for (i = 0; i < NUM_MAX_OBJECT; i++) {
		if (currentActor->indexInWorld != -1) {
			if (currentActor->stage == g_currentFloor) {
				if (currentActor->life != -1) {
					switch (currentActor->lifeMode) {
					case 0: {
						break;
					}
					case 1: {
						if (currentActor->room != currentRoom) {
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

	currentObject = &ListWorldObjets[0];

	for (i = 0; i < maxObjects; i++) {
		if (currentObject->objIndex != -1) {
			if (currentWorldTarget == i) {
				currentCameraTargetActor = currentObject->objIndex;
			}
		} else {
			if (currentObject->stage == g_currentFloor) {
				if (currentObject->life != -1) {
					if (currentObject->lifeMode != -1) {
						int actorIdx;

						switch (currentObject->lifeMode) {
						case 1: {
							if (currentObject->room != currentRoom) {
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
						actorIdx = copyObjectToActor(currentObject->body, currentObject->typeZV, currentObject->foundName,
													 currentObject->flags & 0xFFDF,
													 currentObject->x, currentObject->y, currentObject->z,
													 currentObject->stage, currentObject->room,
													 currentObject->alpha, currentObject->beta, currentObject->gamma,
													 currentObject->anim,
													 currentObject->frame, currentObject->animType, currentObject->animInfo);

						currentObject->objIndex = actorIdx;

						if (actorIdx != -1) {
							currentProcessedActorPtr = &objectTable[actorIdx];
							currentProcessedActorIdx = actorIdx;

							if (currentWorldTarget == i) {
								currentCameraTargetActor = currentProcessedActorIdx;
							}

							currentProcessedActorPtr->dynFlags = (currentObject->flags & 0x20) / 0x20; // recheck
							currentProcessedActorPtr->life = currentObject->life;
							currentProcessedActorPtr->lifeMode = currentObject->lifeMode;

							currentProcessedActorPtr->indexInWorld = i;

							setMoveMode(currentObject->trackMode, currentObject->trackNumber);

							currentProcessedActorPtr->positionInTrack = currentObject->positionInTrack;

							actorTurnedToObj = 1;
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
	int i;

	for (i = 0; i < cameraDataTable[currentCamera]->numViewedRooms; i++) {
		if (cameraDataTable[currentCamera]->viewedRoomTable[i].viewedRoomIdx == room) {
			return (1);
		}
	}

	return (0);
}

void createActorList() {
	int i;
	tObject *actorPtr;

	numActorInList = 0;

	actorPtr = objectTable;

	for (i = 0; i < NUM_MAX_OBJECT; i++) {
		if (actorPtr->indexInWorld != -1 && actorPtr->bodyNum != -1) {
			if (checkActorInRoom(actorPtr->room)) {
				sortedActorTable[numActorInList] = i;
				if (!(actorPtr->_flags & (AF_SPECIAL & AF_ANIMATED))) {
					actorPtr->_flags |= AF_BOXIFY;
					//  FlagRefreshAux2 = 1;
				}
				numActorInList++;
			}
		}

		actorPtr++;
	}
}

void setupCamera() {
	int x;
	int y;
	int z;
	cameraDataStruct *pCamera;

	freezeTime();

	currentCamera = startGameVar1;

	assert(startGameVar1 < roomDataTable[currentRoom].numCameraInRoom);

	loadCamera(roomDataTable[currentRoom].cameraIdxTable[startGameVar1]);
	if (g_engine->getGameId() >= GID_JACK) {
		loadMask(roomDataTable[currentRoom].cameraIdxTable[startGameVar1]);
	} else {
		createAITD1Mask();
	}
	cameraBackgroundChanged = true;

	pCamera = cameraDataTable[currentCamera];

	setAngleCamera(pCamera->alpha, pCamera->beta, pCamera->gamma);

	x = (pCamera->x - roomDataTable[currentRoom].worldX) * 10;
	y = (roomDataTable[currentRoom].worldY - pCamera->y) * 10;
	z = (roomDataTable[currentRoom].worldZ - pCamera->z) * 10;

	setPosCamera(x, y, z); // setup camera position

	setupCameraProjection(160, 100, pCamera->focal1, pCamera->focal2, pCamera->focal3); // setup focale

	setupCameraSub1();
	updateAllActorAndObjects();
	createActorList();
	//  setupCameraSub3();
	setupCameraSub4();
	/*  setupCameraSub5();
	 */
	if (flagInitView == 2) {
		flagRedraw = 2;
	} else {
		if (flagRedraw != 2) {
			flagRedraw = 1;
		}
	}

	flagInitView = 0;
	unfreezeTime();
}

int16 computeDistanceToPoint(int x1, int z1, int x2, int z2) {
	// int axBackup = x1;
	x1 -= x2;
	if ((int16)x1 < 0) {
		x1 = -(int16)x1;
	}

	z1 -= z2;
	if ((int16)z1 < 0) {
		z1 = -(int16)z1;
	}

	if ((x1 + z1) > 0xFFFF) {
		return (0x7D00);
	} else {
		return (x1 + z1);
	}
}

void InitRealValue(int16 beta, int16 newBeta, int16 param, interpolatedValue *rotatePtr) {
	rotatePtr->oldAngle = beta;
	rotatePtr->newAngle = newBeta;
	rotatePtr->param = param;
	rotatePtr->timeOfRotate = timer;
}

int16 updateActorRotation(interpolatedValue *rotatePtr) {
	int timeDif;
	int angleDif;

	if (!rotatePtr->param)
		return (rotatePtr->newAngle);

	timeDif = timer - rotatePtr->timeOfRotate;

	if (timeDif > rotatePtr->param) {
		rotatePtr->param = 0;
		return (rotatePtr->newAngle);
	}

	angleDif = (rotatePtr->newAngle & 0x3FF) - (rotatePtr->oldAngle & 0x3FF);

	if (angleDif <= 0x200) {
		if (angleDif >= -0x200) {
			int angle = (rotatePtr->newAngle & 0x3FF) - (rotatePtr->oldAngle & 0x3FF);
			return (rotatePtr->oldAngle & 0x3FF) + (angle * timeDif) / rotatePtr->param;
		} else {
			int16 angle = ((rotatePtr->newAngle & 0x3FF) + 0x400) - ((rotatePtr->oldAngle & 0x3FF));
			return (((rotatePtr->oldAngle & 0x3FF)) + ((angle * timeDif) / rotatePtr->param));
		}
	} else {
		int angle = (rotatePtr->newAngle & 0x3FF) - ((rotatePtr->oldAngle & 0x3FF) + 0x400);
		return ((angle * timeDif) / rotatePtr->param) + ((rotatePtr->oldAngle & 0x3FF));
	}
}

void removeFromBGIncrust(int actorIdx) {
	tObject *actorPtr = &objectTable[actorIdx];

	actorPtr->_flags &= ~AF_BOXIFY;

	//  FlagRefreshAux2 = 1;

	BBox3D1 = actorPtr->screenXMin;

	if (BBox3D1 > -1) {
		BBox3D2 = actorPtr->screenYMin;
		BBox3D3 = actorPtr->screenXMax;
		BBox3D4 = actorPtr->screenYMax;

		// deleteSubSub();
	}
}

int findObjectInInventory(int objIdx) {
	for (int i = 0; i < numObjInInventoryTable[currentInventory]; i++) {
		if (inventoryTable[currentInventory][i] == objIdx) {
			return (i);
		}
	}

	return (-1);
}

void DeleteInventoryObjet(int objIdx) {
	int inventoryIdx;

	inventoryIdx = findObjectInInventory(objIdx);

	if (inventoryIdx != -1) {
		memmove(&inventoryTable[currentInventory][inventoryIdx], &inventoryTable[currentInventory][inventoryIdx + 1], (30 - inventoryIdx - 1) * 2);

		numObjInInventoryTable[currentInventory]--;
	}

	ListWorldObjets[objIdx].flags2 &= 0x7FFF;
}

void deleteObject(int objIdx) {
	tWorldObject *objPtr;
	int actorIdx;
	tObject *actorPtr;

	objPtr = &ListWorldObjets[objIdx];
	actorIdx = objPtr->objIndex;

	if (actorIdx != -1) {
		actorPtr = &objectTable[actorIdx];

		actorPtr->room = -1;
		actorPtr->stage = -1;

		//    FlagGenereActiveList = 1;

		if (actorPtr->_flags & AF_BOXIFY) {
			removeFromBGIncrust(actorIdx);
		}
	}

	objPtr->room = -1;
	objPtr->stage = -1;

	DeleteInventoryObjet(objIdx);
}

static int isBgOverlayRequired(int X1, int X2, int Z1, int Z2, char *data, int param) {
	int i;
	for (i = 0; i < param; i++) {
		////////////////////////////////////// DEBUG
		//  drawOverlayZone(data, 80);
		/////////////////////////////////////

		int zoneX1 = *(int16 *)(data);
		int zoneZ1 = *(int16 *)(data + 2);
		int zoneX2 = *(int16 *)(data + 4);
		int zoneZ2 = *(int16 *)(data + 6);

		if (X1 >= zoneX1 && Z1 >= zoneZ1 && X2 <= zoneX2 && Z2 <= zoneZ2) {
			return (1);
		}

		data += 0x8;
	}

	return (0);
}

static void drawBgOverlay(tObject *actorPtr) {
	char *data;
	char *data2;

	int numOverlayZone;

	actorPtr->screenXMin = BBox3D1;
	actorPtr->screenYMin = BBox3D2;
	actorPtr->screenXMax = BBox3D3;
	actorPtr->screenYMax = BBox3D4;

	// if(actorPtr->trackMode != 1)
	//	return;

	setClip(BBox3D1, BBox3D2, BBox3D3, BBox3D4);

	cameraDataStruct *pCamera = cameraDataTable[currentCamera];

	// look for the correct room data of that camera
	cameraViewedRoomStruct *pcameraViewedRoomData = NULL;
	int relativeCameraIndex = -1;
	for (int i = 0; i < pCamera->numViewedRooms; i++) {
		if (pCamera->viewedRoomTable[i].viewedRoomIdx == actorPtr->room) {
			pcameraViewedRoomData = &pCamera->viewedRoomTable[i];
			relativeCameraIndex = i;
			break;
		}
	}
	if (pcameraViewedRoomData == NULL)
		return;

	if (g_engine->getGameId() == GID_AITD1) {
		data2 = room_PtrCamera[currentCamera] + pcameraViewedRoomData->offsetToMask;
		data = data2;
		data += 2;

		numOverlayZone = *(int16 *)(data2);

		for (int i = 0; i < numOverlayZone; i++) {
			int numOverlay;
			// char *src = data2 + *(uint16 *)(data + 2);

			if (isBgOverlayRequired(actorPtr->zv.ZVX1 / 10, actorPtr->zv.ZVX2 / 10,
									actorPtr->zv.ZVZ1 / 10, actorPtr->zv.ZVZ2 / 10,
									data + 4,
									*(int16 *)(data))) {
				osystem_setClip(clipLeft, clipTop, clipRight, clipBottom);
				osystem_drawMask(relativeCameraIndex, i);
				osystem_clearClip();

				/*
				int j;
				numOverlay = *(s16*)src;
				src += 2;

				for(j=0;j<numOverlay;j++)
				{
				int param = *(s16*)(src);
				src+=2;

				memcpy(cameraBuffer, src, param*4);

				src+=param*4;

				drawBgOverlaySub2(param);
				}
				*/

				//      blitOverlay(src);
			}

			numOverlay = *(int16 *)(data);
			data += 2;
			data += ((numOverlay * 4) + 1) * 2;
		}
	} else {
		for (int i = 0; i < pcameraViewedRoomData->numMask; i++) {
			cameraMaskStruct *pMaskZones = &pcameraViewedRoomData->masks[i];

			for (int j = 0; j < pMaskZones->numTestRect; j++) {
				rectTestStruct *pRect = &pMaskZones->rectTests[j];

				int actorX1 = actorPtr->zv.ZVX1 / 10;
				int actorX2 = actorPtr->zv.ZVX2 / 10;
				int actorZ1 = actorPtr->zv.ZVZ1 / 10;
				int actorZ2 = actorPtr->zv.ZVZ2 / 10;

				if (actorX1 >= pRect->zoneX1 && actorZ1 >= pRect->zoneZ1 && actorX2 <= pRect->zoneX2 && actorZ2 <= pRect->zoneZ2) {
					osystem_setClip(clipLeft, clipTop, clipRight, clipBottom);
					osystem_drawMask(relativeCameraIndex, i);
					osystem_clearClip();
					break;
				}
			}
		}
	}
	setClip(0, 0, 319, 199);
}

static void mainDrawSub2(int actorIdx) // draw flow
{
	// actorStruct* actorPtr = &actorTable[actorIdx];

	// char* data = printTextSub2(hqrUnk, actorPtr->FRAME);

	// TODO: finish
}

static void getHotPoint(int hotPointIdx, char *bodyPtr, point3dStruct *hotPoint) {
	int16 flag;

	flag = *(int16 *)bodyPtr;
	bodyPtr += 2;

	if (flag & 2) {
		int16 offset;
		bodyPtr += 12;

		offset = *(int16 *)bodyPtr;
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
			int pointIdx;
			int16 *source;

			if (flag & INFO_OPTIMISE) {
				bodyPtr += hotPointIdx * 0x18;
			} else {
				bodyPtr += hotPointIdx * 16;
			}

			pointIdx = *(int16 *)(bodyPtr + 4); // first point

			// ASSERT(pointIdx > 0 && pointIdx < 1200);

			source = (int16 *)(((char *)pointBuffer) + pointIdx);

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

static int drawTextOverlay(void);

void mainDraw(int flagFlip) {
	int i;
	// if(flagFlip == 2)
	{
		if (cameraBackgroundChanged) {
			gfx_copyBlockPhys((unsigned char *)aux, 0, 0, 320, 200);
			cameraBackgroundChanged = false;
		}
	}

	if (flagFlip == 0) {
		// restoreDirtyRects();
	} else {
		genVar5 = 0;
		fastCopyScreen(aux2, logicalScreen);
	}

	// osystem_drawBackground();

	setClip(0, 0, 319, 199);
	genVar6 = 0;

	// osystem_startModelRender();

	for (i = 0; i < numActorInList; i++) {
		int currentDrawActor = sortedActorTable[i];
		tObject *actorPtr;

		actorPtr = &objectTable[currentDrawActor];

		// this is commented out to draw actors incrusted in background
		// if(actorPtr->_flags & (AF_ANIMATED + AF_DRAWABLE + AF_SPECIAL))
		{
			actorPtr->_flags &= ~AF_DRAWABLE;

			if (actorPtr->_flags & AF_SPECIAL) {
				mainDrawSub2(currentDrawActor);
			} else {
				char *bodyPtr = HQR_Get(listBody, actorPtr->bodyNum);

				if (HQ_Load) {
					//          initAnimInBody(actorPtr->FRAME, HQR_Get(listAnim, actorPtr->ANIM), bodyPtr);
				}

				affObjet(actorPtr->worldX + actorPtr->stepX, actorPtr->worldY + actorPtr->stepY, actorPtr->worldZ + actorPtr->stepZ, actorPtr->alpha, actorPtr->beta, actorPtr->gamma, bodyPtr);

				if (actorPtr->animActionType != 0) {
					if (actorPtr->hotPointID != -1) {
						getHotPoint(actorPtr->hotPointID, bodyPtr, &actorPtr->hotPoint);
					}
				}
			}

			if (BBox3D1 < 0)
				BBox3D1 = 0;
			if (BBox3D3 > 319)
				BBox3D3 = 319;
			if (BBox3D2 < 0)
				BBox3D2 = 0;
			if (BBox3D4 > 199)
				BBox3D4 = 199;

			if (BBox3D1 <= 319 && BBox3D2 <= 199 && BBox3D3 >= 0 && BBox3D4 >= 0) // is the character on screen ?
			{
				if (g_engine->getGameId() == GID_AITD1) {
					if (actorPtr->indexInWorld == CVars[getCVarsIdx(LIGHT_OBJECT)]) {
						lightX = (BBox3D3 + BBox3D1) / 2;
						lightY = (BBox3D4 + BBox3D2) / 2;
					}
				}

#ifdef FITD_DEBUGGER
				if (backgroundMode == backgroundModeEnum_2D)
#endif
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

	osystem_stopModelRender();

	if (drawTextOverlay()) {
		gfx_copyBlockPhys((unsigned char *)logicalScreen, BBox3D1, BBox3D2, BBox3D3, BBox3D4);
	} else {
		// TODO: check if it's okay
		fastCopyScreen(aux, logicalScreen);
		gfx_copyBlockPhys((unsigned char *)logicalScreen, 0, 0, 320, 200);
	}

	if (!lightOff) {
		if (flagFlip) {
			if (flagFlip == 2 || lightVar2) {
				// makeBlackPalette();
				osystem_flip(NULL);
				fadeInPhys(0x10, 0);
				lightVar2 = 0;
			} else {
				// osystem_flip(NULL);
			}
		} else {
			// mainDrawSub1();
		}
	} else {
	}

	//    osystem_stopFrame();

	//	osystem_flip(NULL);

	flagRedraw = 0;
}

void walkStep(int angle1, int angle2, int angle3) {
	rotate(angle3, angle1, angle2, &animMoveZ, &animMoveX);
}

void addActorToBgInscrust(int actorIdx) {
	objectTable[actorIdx]._flags |= AF_BOXIFY + AF_DRAWABLE;
	objectTable[actorIdx]._flags &= ~AF_ANIMATED;

	// FlagRefreshAux2 = 1;
}

int checkZvCollision(ZVStruct *zvPtr1, ZVStruct *zvPtr2) {
	if (zvPtr1->ZVX1 >= zvPtr2->ZVX2)
		return 0;

	if (zvPtr2->ZVX1 >= zvPtr1->ZVX2)
		return 0;

	if (zvPtr1->ZVY1 >= zvPtr2->ZVY2)
		return 0;

	if (zvPtr2->ZVY1 >= zvPtr1->ZVY2)
		return 0;

	if (zvPtr1->ZVZ1 >= zvPtr2->ZVZ2)
		return 0;

	if (zvPtr2->ZVZ1 >= zvPtr1->ZVZ2)
		return 0;

	return 1;
}

void getZvRelativePosition(ZVStruct *zvPtr, int startRoom, int destRoom) {
	unsigned int Xdif = 10 * (roomDataTable[destRoom].worldX - roomDataTable[startRoom].worldX);
	unsigned int Ydif = 10 * (roomDataTable[destRoom].worldY - roomDataTable[startRoom].worldY);
	unsigned int Zdif = 10 * (roomDataTable[destRoom].worldZ - roomDataTable[startRoom].worldZ);

	zvPtr->ZVX1 -= Xdif;
	zvPtr->ZVX2 -= Xdif;
	zvPtr->ZVY1 += Ydif;
	zvPtr->ZVY2 += Ydif;
	zvPtr->ZVZ1 += Zdif;
	zvPtr->ZVZ2 += Zdif;
}

int checkObjectCollisions(int actorIdx, ZVStruct *zvPtr) {
	int currentCollisionSlot = 0;
	tObject *currentActor = objectTable;
	int actorRoom = objectTable[actorIdx].room;

	for (int i = 0; i < 3; i++) {
		currentProcessedActorPtr->COL[i] = -1;
	}

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		if (currentActor->indexInWorld != -1 && i != actorIdx) {
			ZVStruct *currentActorZv = &currentActor->zv;

			if (currentActor->room != actorRoom) {
				ZVStruct localZv;

				copyZv(zvPtr, &localZv);

				getZvRelativePosition(&localZv, actorRoom, currentActor->room);

				if (checkZvCollision(&localZv, currentActorZv)) {
					currentProcessedActorPtr->COL[currentCollisionSlot++] = i;

					if (currentCollisionSlot == 3)
						return (3);
				}
			} else {
				if (checkZvCollision(zvPtr, currentActorZv)) {
					currentProcessedActorPtr->COL[currentCollisionSlot++] = i;

					if (currentCollisionSlot == 3)
						return (3);
				}
			}
		}
		currentActor++;
	}

	return (currentCollisionSlot);
}

void cleanClip() {
	for (int x = clipLeft; x < clipRight; x++) {
		for (int y = clipTop; y < clipBottom; y++) {
			logicalScreen[y * 320 + x] = 0;
		}
	}
}

void drawFoundObect(int menuState, int objectName, int zoomFactor) {
	cleanClip();

	setCameraTarget(0, 0, 0, 60, statusVar1, 0, zoomFactor);

	affObjet(0, 0, 0, 0, 0, 0, HQR_Get(listBody, currentFoundBodyIdx));

	simpleMessage(160, WindowY1, 20, 1);
	simpleMessage(160, WindowY1 + 16, objectName, 1);
	simpleMessage(160, WindowY1 + 16, objectName, 1);

	switch (menuState) {
	case 0: {
		selectedMessage(130, WindowY2 - 16, 21, 1, 4);
		simpleMessage(190, WindowY2 - 16, 22, 4);
		break;
	}
	case 1: {
		simpleMessage(130, WindowY2 - 16, 21, 4);
		selectedMessage(190, WindowY2 - 16, 22, 1, 4);
		break;
	}
	case 2: {
		selectedMessage(160, WindowY2 - 16, 10, 1, 4);
		break;
	}
	}
}

void take(int objIdx) {
	tWorldObject *objPtr = &ListWorldObjets[objIdx];

	if (numObjInInventoryTable[currentInventory] == 0) {
		inventoryTable[currentInventory][0] = objIdx;
	} else {
		int i;

		for (i = numObjInInventoryTable[currentInventory]; i > 0; i--) {
			inventoryTable[currentInventory][i + 1] = inventoryTable[currentInventory][i];
		}

		inventoryTable[currentInventory][1] = objIdx;
	}

	numObjInInventoryTable[currentInventory]++;

	action = 0x800;

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
	tWorldObject *objPtr;
	int var_C = 0;
	int var_6 = 1;
	int i;
	int var_A = 15000;
	int var_8 = -200;

	if (objIdx < 0)
		return;

	if (param == 2) {
		debug("foundObject with param == 2\n");
	}

	objPtr = &ListWorldObjets[objIdx];

	if (param != 0 && (objPtr->flags2 & 0xC000)) {
		return;
	}

	if (objPtr->trackNumber) {
		if (timer - objPtr->trackNumber < 300) // prevent from reopening the window every frame
			return;
	}

	objPtr->trackNumber = 0;

	freezeTime();
	//  setupShaking(1000); // probably to remove the shaking when in foundObject screen

	int weight = 0;
	for (i = 0; i < numObjInInventoryTable[currentInventory]; i++) {
		weight += ListWorldObjets[inventoryTable[currentInventory][i]].positionInTrack;
	}

	if (objPtr->positionInTrack + weight > CVars[getCVarsIdx(MAX_WEIGHT_LOADABLE)] || numObjInInventoryTable[currentInventory] + 1 == 30) {
		var_6 = 3;
	}

	currentFoundBodyIdx = objPtr->foundBody;
	currentFoundBody = HQR_Get(listBody, currentFoundBodyIdx);

	setupCameraProjection(160, 100, 128, 300, 298);

	statusVar1 = 0;

	memset(frontBuffer, 0, 320 * 200);
	fastCopyScreen(frontBuffer, logicalScreen);

	affBigCadre(160, 100, 240, 120);

	drawFoundObect(var_6, objPtr->foundName, var_A);
	osystem_flip(NULL);

	input5 = 1;

	while (!var_C) {
		gfx_copyBlockPhys((unsigned char *)logicalScreen, 0, 0, 320, 200);

		process_events();
		osystem_drawBackground();

		localKey = key;
		localJoyD = JoyD;
		localClick = Click;

		if (!input5) {
			if (localKey == 1) {
				if (var_6 != 2) {
					var_6 = 0;
				}

				var_C = 1;
			}
			if (var_6 != 2) {
				if (localJoyD & 4) {
					var_6 = 0;
				}

				if (localJoyD & 8) {
					var_6 = 1;
				}
			}

			if (localKey == 28 || localClick != 0) {
				while (key) {
					process_events();
				}

				var_C = 1;
			}
		} else {
			if (!localKey && !localJoyD && !localClick)
				input5 = 0;
		}

		statusVar1 -= 8;

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
		objPtr->trackNumber = timer;
	}

	while (key && Click) {
		process_events();
	}

	localJoyD = 0;
	localKey = 0;
	localClick = 0;

	//  if(mainLoopVar1 != 0)
	{
		// setupShaking(-600);
	}

	flagInitView = 1;
}

static void hardColSuB1Sub1(int flag) {
	switch (flag) {
	case 1:
	case 2: {
		hardColStepZ = 0;
		break;
	}
	case 4:
	case 8: {
		hardColStepX = 0;
		break;
	}
	default: {
		break;
	}
	}
}

void handleCollision(ZVStruct *startZv, ZVStruct *zvPtr2, ZVStruct *zvPtr3) {
	int32 flag = 0;
	int32 var_8;
	int32 halfX;
	int32 halfZ;
	int32 var_A;
	int32 var_6;

	if (startZv->ZVX2 > zvPtr3->ZVX1) {
		if (zvPtr3->ZVX2 <= startZv->ZVX1) {
			flag = 8;
		}
	} else {
		flag = 4;
	}

	if (startZv->ZVZ2 > zvPtr3->ZVZ1) {
		if (startZv->ZVZ1 >= zvPtr3->ZVZ2) {
			flag |= 2;
		}
	} else {
		flag |= 1;
	}

	if (flag == 5 || flag == 9 || flag == 6 || flag == 10) {
		var_8 = 2;
	} else {
		if (!flag) {
			var_8 = 0;

			hardColStepZ = 0;
			hardColStepX = 0;

			return;
		} else {
			var_8 = 1;
		}
	}

	halfX = (zvPtr2->ZVX1 + zvPtr2->ZVX2) / 2;
	halfZ = (zvPtr2->ZVZ1 + zvPtr2->ZVZ2) / 2;

	if (zvPtr3->ZVX1 > halfX) {
		var_A = 4;
	} else {
		if (zvPtr3->ZVX2 < halfX) {
			var_A = 0;
		} else {
			var_A = 8;
		}
	}

	if (zvPtr3->ZVZ1 > halfZ) {
		var_A |= 1;
	} else {
		if (zvPtr3->ZVZ2 < halfZ) {
			var_A |= 0; // once again, not that much usefull
		} else {
			var_A |= 2;
		}
	}

	if (var_A == 5 || var_A == 9 || var_A == 6 || var_A == 10) {
		var_6 = 2;
	} else {
		if (!var_A) {
			var_6 = 0;
		} else {
			var_6 = 1;
		}
	}

	if (var_8 == 1) {
		hardColSuB1Sub1(flag);
		return;
	}

	if (var_6 == 1 && (var_A & flag)) {
		hardColSuB1Sub1(var_A);
		return;
	}

	if (var_A == flag || flag == 15) {
		int Xmod = abs(zvPtr2->ZVX1 - startZv->ZVX1); // recheck
		int Zmod = abs(zvPtr2->ZVZ1 - startZv->ZVZ1);

		if (Xmod > Zmod) {
			hardColStepZ = 0;
		} else {
			hardColStepX = 0;
		}
	} else {
		if (!var_6 || (var_6 == 1 && !(var_A & flag))) {
			hardColStepZ = 0;
			hardColStepX = 0;
		} else {
			hardColSuB1Sub1(flag & var_A);
		}
	}
}

int AsmCheckListCol(ZVStruct *zvPtr, roomDataStruct *pRoomData) {
	uint16 i;
	int hardColVar = 0;
	hardColStruct *pCurrentEntry = pRoomData->hardColTable;

	for (i = 0; i < pRoomData->numHardCol; i++) {
		if (((pCurrentEntry->zv.ZVX1) < (zvPtr->ZVX2)) && ((zvPtr->ZVX1) < (pCurrentEntry->zv.ZVX2))) {
			if (((pCurrentEntry->zv.ZVY1) < (zvPtr->ZVY2)) && ((zvPtr->ZVY1) < (pCurrentEntry->zv.ZVY2))) {
				if (((pCurrentEntry->zv.ZVZ1) < (zvPtr->ZVZ2)) && ((zvPtr->ZVZ1) < (pCurrentEntry->zv.ZVZ2))) {
					assert(hardColVar < 10);
					hardColTable[hardColVar++] = pCurrentEntry;
				}
			}
		}

		pCurrentEntry++;
	}

	return hardColVar;
}

void menuWaitVSync() {
}

static int testCrossProduct(int x1, int z1, int x2, int z2, int x3, int z3, int x4, int z4) {
	int returnFlag = 0;

	int xAB = x1 - x2;
	int yCD = z3 - z4;
	int xCD = x3 - x4;
	int yAB = z1 - z2;

	int xAC = x1 - x3;
	int yAC = z1 - z3;

	int DotProduct = (xAB * yCD) - (xCD * yAC);

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

	return (returnFlag);
}

static int isInPoly(int x1, int x2, int z1, int z2, cameraViewedRoomStruct *pCameraZoneDef) {
	int xMid = (x1 + x2) / 2;
	int zMid = (z1 + z2) / 2;

	int i;

	for (i = 0; i < pCameraZoneDef->numCoverZones; i++) {
		int j;
		int flag = 0;

		for (j = 0; j < pCameraZoneDef->coverZones[i].numPoints; j++) {
			int zoneX1;
			int zoneZ1;
			int zoneX2;
			int zoneZ2;

			zoneX1 = pCameraZoneDef->coverZones[i].pointTable[j].x;
			zoneZ1 = pCameraZoneDef->coverZones[i].pointTable[j].y;
			zoneX2 = pCameraZoneDef->coverZones[i].pointTable[j + 1].x;
			zoneZ2 = pCameraZoneDef->coverZones[i].pointTable[j + 1].y;

			if (testCrossProduct(xMid, zMid, xMid - 10000, zMid, zoneX1, zoneZ1, zoneX2, zoneZ2)) {
				flag |= 1;
			}

			if (testCrossProduct(xMid, zMid, xMid + 10000, zMid, zoneX1, zoneZ1, zoneX2, zoneZ2)) {
				flag |= 2;
			}
		}

		if (flag == 3) {
			return (1);
		}
	}

	return (0);
}

static int findBestCamera(void) {
	int foundAngle = 32000;
	int foundCamera = -1;

	tObject *actorPtr = &objectTable[currentCameraTargetActor];

	int x1 = actorPtr->zv.ZVX1 / 10;
	int x2 = actorPtr->zv.ZVX2 / 10;
	int z1 = actorPtr->zv.ZVZ1 / 10;
	int z2 = actorPtr->zv.ZVZ2 / 10;

	int i;

	for (i = 0; i < numCameraInRoom; i++) {
		assert(i < NUM_MAX_CAMERA_IN_ROOM);
		if (currentCameraZoneList[i])
			if (isInPoly(x1, x2, z1, z2, currentCameraZoneList[i])) // if in camera zone ?
			{
				// we try to select the best camera that looks behind the player
				int newAngle = actorPtr->beta + (((cameraDataTable[i]->beta) + 0x200) & 0x3FF);

				if (newAngle < 0) {
					newAngle = -newAngle;
				}

				if (newAngle < foundAngle) {
					foundAngle = newAngle;
					foundCamera = i;
				}
			}
	}

	return (foundCamera);
}

void checkIfCameraChangeIsRequired(void) {
	int localCurrentCam = currentCamera;
	int newCamera;

	if (currentCamera != -1) {
		tObject *actorPtr;
		int zvx1;
		int zvx2;
		int zvz1;
		int zvz2;

		actorPtr = &objectTable[currentCameraTargetActor];

		zvx1 = actorPtr->zv.ZVX1 / 10;
		zvx2 = actorPtr->zv.ZVX2 / 10;

		zvz1 = actorPtr->zv.ZVZ1 / 10;
		zvz2 = actorPtr->zv.ZVZ2 / 10;

		if (isInPoly(zvx1, zvx2, zvz1, zvz2, currentCameraZoneList[currentCamera])) // is still in current camera zone ?
		{
			return;
		}
	}

	newCamera = findBestCamera(); // find new camera

	if (newCamera != -1) {
		localCurrentCam = newCamera;
	}

	if (currentCamera != localCurrentCam) {
		startGameVar1 = localCurrentCam;
		flagInitView = 1;
	}
}

static bool isPointInZV(int x, int y, int z, ZVStruct *pZV) {
	if (pZV->ZVX1 <= x && pZV->ZVX2 >= x) {
		if (pZV->ZVY1 <= y && pZV->ZVY2 >= y) {
			if (pZV->ZVZ1 <= z && pZV->ZVZ2 >= z) {
				return (true);
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
		roomDataStruct *pRoomData = &roomDataTable[currentProcessedActorPtr->room];
		for (uint32 i = 0; i < pRoomData->numSceZone; i++) {
			sceZoneStruct *pCurrentZone = &pRoomData->sceZoneTable[i];

			if (isPointInZV(currentProcessedActorPtr->roomX + currentProcessedActorPtr->stepX,
							currentProcessedActorPtr->roomY + currentProcessedActorPtr->stepY,
							currentProcessedActorPtr->roomZ + currentProcessedActorPtr->stepZ,
							&pCurrentZone->zv)) {
				switch (pCurrentZone->type) {
				case 0: {
					int x;
					int y;
					int z;

					int oldRoom = currentProcessedActorPtr->room;

					currentProcessedActorPtr->room = (short)pCurrentZone->parameter;

					x = (roomDataTable[currentProcessedActorPtr->room].worldX - roomDataTable[oldRoom].worldX) * 10;
					y = (roomDataTable[currentProcessedActorPtr->room].worldY - roomDataTable[oldRoom].worldY) * 10;
					z = (roomDataTable[currentProcessedActorPtr->room].worldZ - roomDataTable[oldRoom].worldZ) * 10;

					currentProcessedActorPtr->roomX -= x;
					currentProcessedActorPtr->roomY += y;
					currentProcessedActorPtr->roomZ += z;

					currentProcessedActorPtr->zv.ZVX1 -= x;
					currentProcessedActorPtr->zv.ZVX2 -= x;

					currentProcessedActorPtr->zv.ZVY1 += y;
					currentProcessedActorPtr->zv.ZVY2 += y;

					currentProcessedActorPtr->zv.ZVZ1 += z;
					currentProcessedActorPtr->zv.ZVZ2 += z;

					onceMore = true;
					if (currentProcessedActorIdx == currentCameraTargetActor) {
						needChangeRoom = 1;
						newRoom = (short)pCurrentZone->parameter;
						if (g_engine->getGameId() > GID_AITD1)
							loadRoom(newRoom);

					} else {
						actorTurnedToObj = 1;
					}

					startChrono(&currentProcessedActorPtr->ROOM_CHRONO);

					break;
				}
				case 8: {
					assert(g_engine->getGameId() != GID_AITD1);
					if (g_engine->getGameId() != GID_AITD1) {
						currentProcessedActorPtr->hardMat = (short)pCurrentZone->parameter;
					}
					break;
				}
				case 9: // Scenar
				{
					if ((g_engine->getGameId() == GID_AITD1) || !flagFloorChange) {
						currentProcessedActorPtr->HARD_DEC = (short)pCurrentZone->parameter;
					}
					break;
				}
				case 10: // stage
				{
					int life;

					life = ListWorldObjets[currentProcessedActorPtr->indexInWorld].floorLife;

					if (life == -1)
						return;

					currentProcessedActorPtr->life = life;

					currentProcessedActorPtr->HARD_DEC = (short)pCurrentZone->parameter;
					flagFloorChange = true;
					break;
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
	int tempX;
	int tempZ;

	localZv.ZVX1 = X - param;
	localZv.ZVX2 = X + param;
	localZv.ZVY1 = Y - param;
	localZv.ZVY2 = Y + param;
	localZv.ZVZ1 = Z - param;
	localZv.ZVZ2 = Z + param;

	walkStep(param * 2, 0, beta);

	while (foundFlag == -2) {
		localZv.ZVX1 += animMoveX;
		localZv.ZVX2 += animMoveX;

		localZv.ZVZ1 += animMoveZ;
		localZv.ZVZ2 += animMoveZ;

		tempX = X;
		tempZ = Z;

		X += animMoveX;
		Z += animMoveZ;

		if (X > 20000 || X < -20000 || Z > 20000 || Z < -20000) {
			foundFlag = -1;
			break;
		}

		if (AsmCheckListCol(&localZv, &roomDataTable[room]) <= 0) {
			foundFlag = -1;
		} else {
			tObject *currentActorPtr = objectTable;

			int i;

			for (i = 0; i < NUM_MAX_OBJECT; i++) {
				if (currentActorPtr->indexInWorld != -1 && i != actorIdx && !(currentActorPtr->_flags & AF_SPECIAL)) {
					ZVStruct *zvPtr = &currentActorPtr->zv;

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

	animMoveX = tempX;
	animMoveY = Y;
	animMoveZ = tempZ;

	return (foundFlag);
}

void PutAtObjet(int objIdx, int objIdxToPutAt) {
	tWorldObject *objPtr = &ListWorldObjets[objIdx];
	tWorldObject *objPtrToPutAt = &ListWorldObjets[objIdxToPutAt];

	if (objPtrToPutAt->objIndex != -1) {
		tObject *actorToPutAtPtr = &objectTable[objPtrToPutAt->objIndex];

		DeleteInventoryObjet(objIdx);

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
			currentProcessedActorPtr->roomX = actorToPutAtPtr->roomX;
			currentProcessedActorPtr->roomY = actorToPutAtPtr->roomY;
			currentProcessedActorPtr->roomZ = actorToPutAtPtr->roomZ;
			currentProcessedActorPtr->room = actorToPutAtPtr->room;
			currentProcessedActorPtr->stage = actorToPutAtPtr->stage;
			currentProcessedActorPtr->alpha = actorToPutAtPtr->alpha;
			currentProcessedActorPtr->beta = actorToPutAtPtr->beta;
			currentProcessedActorPtr->gamma = actorToPutAtPtr->gamma;

			ListWorldObjets[currentProcessedActorPtr->indexInWorld].flags2 |= 0x4000;
			ListWorldObjets[currentProcessedActorPtr->indexInWorld].flags |= 0x80;

			//      FlagGenereActiveList = 1;
			//      FlagRefreshAux2 = 1;
		}

	} else {
		DeleteInventoryObjet(objIdx);

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
			currentProcessedActorPtr->roomX = objPtrToPutAt->x;
			currentProcessedActorPtr->roomY = objPtrToPutAt->y;
			currentProcessedActorPtr->roomZ = objPtrToPutAt->z;
			currentProcessedActorPtr->room = objPtrToPutAt->room;
			currentProcessedActorPtr->stage = objPtrToPutAt->stage;
			currentProcessedActorPtr->alpha = objPtrToPutAt->alpha;
			currentProcessedActorPtr->beta = objPtrToPutAt->beta;
			currentProcessedActorPtr->gamma = objPtrToPutAt->gamma;

			ListWorldObjets[currentProcessedActorPtr->indexInWorld].flags2 |= 0x4000;
			ListWorldObjets[currentProcessedActorPtr->indexInWorld].flags |= 0x80;

			//      FlagGenereActiveList = 1;
			//      FlagRefreshAux2 = 1;
		}
	}
}

void throwStoppedAt(int x, int z) {
	int x2;
	int y2;
	int z2;
	int foundPosition;
	int step;

	ZVStruct zvCopy;
	ZVStruct zvLocal;
	uint8 *bodyPtr;

	bodyPtr = (uint8 *)HQR_Get(listBody, currentProcessedActorPtr->bodyNum);

	giveZVObjet((char *)bodyPtr, &zvLocal);

	x2 = x;
	y2 = (currentProcessedActorPtr->roomY / 2000) * 2000;
	z2 = z;

	foundPosition = 0;
	step = 0;

	while (!foundPosition) {
		walkStep(0, -step, currentProcessedActorPtr->beta + 0x200);
		copyZv(&zvLocal, &zvCopy);

		x2 = x + animMoveX;
		z2 = z + animMoveZ;

		zvCopy.ZVX1 += x2;
		zvCopy.ZVX2 += x2;

		zvCopy.ZVY1 += y2;
		zvCopy.ZVY2 += y2;

		zvCopy.ZVZ1 += z2;
		zvCopy.ZVZ2 += z2;

		if (!AsmCheckListCol(&zvCopy, &roomDataTable[currentProcessedActorPtr->room])) {
			foundPosition = 1;
		}

		if (foundPosition) {
			if (y2 < -500) {
				zvCopy.ZVY1 += 100; // is the object reachable ? (100 is Carnby height. If hard col at Y + 100, carnby can't reach that spot)
				zvCopy.ZVY2 += 100;

				if (!AsmCheckListCol(&zvCopy, &roomDataTable[currentProcessedActorPtr->room])) {
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

	currentProcessedActorPtr->worldX = x2;
	currentProcessedActorPtr->roomX = x2;
	currentProcessedActorPtr->worldY = y2;
	currentProcessedActorPtr->roomY = y2;
	currentProcessedActorPtr->worldZ = z2;
	currentProcessedActorPtr->roomZ = z2;

	currentProcessedActorPtr->stepX = 0;
	currentProcessedActorPtr->stepZ = 0;

	currentProcessedActorPtr->animActionType = 0;
	currentProcessedActorPtr->speed = 0;
	currentProcessedActorPtr->gamma = 0;

	giveZVObjet((char *)bodyPtr, &currentProcessedActorPtr->zv);

	currentProcessedActorPtr->zv.ZVX1 += x2;
	currentProcessedActorPtr->zv.ZVX2 += x2;
	currentProcessedActorPtr->zv.ZVY1 += y2;
	currentProcessedActorPtr->zv.ZVY2 += y2;
	currentProcessedActorPtr->zv.ZVZ1 += z2;
	currentProcessedActorPtr->zv.ZVZ2 += z2;

	ListWorldObjets[currentProcessedActorPtr->indexInWorld].flags2 |= 0x4000;
	ListWorldObjets[currentProcessedActorPtr->indexInWorld].flags2 &= 0xEFFF;

	addActorToBgInscrust(currentProcessedActorIdx);
}

void startGame(int startupFloor, int startupRoom, int allowSystemMenu) {
	initEngine();

	initVars();

	loadFloor(startupFloor);

	currentCamera = -1;

	loadRoom(startupRoom);

	startGameVar1 = 0;
	flagInitView = 2;

	setupCamera();

	mainLoop(allowSystemMenu, 1);

	/*freeScene();

	fadeOut(8,0);*/
}

static int drawTextOverlay(void) {
	int hasText = 0;
	int y = 183;

	BBox3D4 = 199;
	BBox3D1 = 319;
	BBox3D3 = 0;

	messageStruct *currentMessage = messageTable;

	if (lightOff == 0) {
		for (int i = 0; i < 5; i++) {
			if (currentMessage->string) {
				int width = currentMessage->string->width;
				int X = 160 - width / 2;
				int Y = X + width;

				if (X < BBox3D1) {
					BBox3D1 = X;
				}

				if (Y > BBox3D3) {
					BBox3D3 = Y;
				}

				if ((currentMessage->time++) > 55) {
					currentMessage->string = NULL;
				} else {
					if (currentMessage->time < 26) {
						extSetFont(PtrFont, 16);
					} else {
						extSetFont(PtrFont, 16 + (currentMessage->time - 26) / 2);
					}

					renderText(X, y + 1, logicalScreen, currentMessage->string->textPtr);
				}

				y -= 16;
				hasText = 1;
			}

			currentMessage++;
		}
	}

	BBox3D2 = y;
	return hasText;
}

void makeMessage(int messageIdx) {
	textEntryStruct *messagePtr;

	messagePtr = getTextFromIdx(messageIdx);

	if (messagePtr) {
		int i;

		for (i = 0; i < 5; i++) {
			if (messageTable[i].string == messagePtr) {
				messageTable[i].time = 0;
				return;
			}
		}

		for (i = 0; i < 5; i++) {
			if (messageTable[i].string == NULL) {
				messageTable[i].string = messagePtr;
				messageTable[i].time = 0;
				return;
			}
		}
	}
}

void hit(int animNumber, int arg_2, int arg_4, int arg_6, int hitForce, int arg_A) {
	if (initAnim(animNumber, 0, arg_A)) {
		currentProcessedActorPtr->animActionANIM = animNumber;
		currentProcessedActorPtr->animActionFRAME = arg_2;
		currentProcessedActorPtr->animActionType = 1;
		currentProcessedActorPtr->animActionParam = arg_6;
		currentProcessedActorPtr->hotPointID = arg_4;
		currentProcessedActorPtr->hitForce = hitForce;
	}
}

} // namespace Fitd
