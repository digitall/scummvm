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

#include "fitd/common.h"
#include "fitd/file_access.h"
#include "fitd/font.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/inventory.h"
#include "fitd/pak.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"
#include "common/file.h"

namespace Fitd {

static void turnPageForward() {
}

static void turnPageBackward() {
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

	while (!quit) {
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

static void initEngine(void) {
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

	ListWorldObjets.resize(maxObjects);

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

		ListWorldObjets[i].flags |= 0x20;
	}

	free(pObjectDataBackup);

	vars = (int16 *)loadFromItd("VARS.ITD");

	varSize = fileSize;

	choosePersoBackup = CVars[getCVarsIdx(CHOOSE_PERSO)]; // backup hero selection

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

	CVars[getCVarsIdx(CHOOSE_PERSO)] = choosePersoBackup;

	listLife = HQR_InitRessource("LISTLIFE", 65000, 100);
	listTrack = HQR_InitRessource("LISTTRAK", 20000, 100);

	// TODO: missing dos memory check here

	listBody = HQR_InitRessource(listBodySelect[CVars[getCVarsIdx(CHOOSE_PERSO)]], 37000, 50); // was calculated from free mem size
	listAnim = HQR_InitRessource(listAnimSelect[CVars[getCVarsIdx(CHOOSE_PERSO)]], 30000, 80); // was calculated from free mem size

	for (int i = 0; i < NUM_MAX_OBJECT; i++) {
		objectTable[i].indexInWorld = -1;
	}

	currentWorldTarget = CVars[getCVarsIdx(WORLD_NUM_PERSO)];
}

static void initVarsSub1(void) {
	for (int i = 0; i < 5; i++) {
		messageTable[i].string = NULL;
	}
}

static void initVars() {
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

void startGame(int startupFloor, int startupRoom, int allowSystemMenu) {
	initEngine();

	initVars();
	assert(false);

	// loadFloor(startupFloor);

	// currentCamera = -1;

	// loadRoom(startupRoom);

	// startGameVar1 = 0;
	// flagInitView = 2;

	// setupCamera();

	// mainLoop(allowSystemMenu, 1);

	// /*freeScene();

	// fadeOut(8,0);*/
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

void copyZv(ZVStruct *source, ZVStruct *dest) {
	memcpy(dest, source, sizeof(ZVStruct));
}

void getZvRelativePosition(ZVStruct *zvPtr, int startRoom, int destRoom) {
	assert(0);
	// unsigned int Xdif = 10 * (roomDataTable[destRoom].worldX - roomDataTable[startRoom].worldX);
	// unsigned int Ydif = 10 * (roomDataTable[destRoom].worldY - roomDataTable[startRoom].worldY);
	// unsigned int Zdif = 10 * (roomDataTable[destRoom].worldZ - roomDataTable[startRoom].worldZ);

	// zvPtr->ZVX1 -= Xdif;
	// zvPtr->ZVX2 -= Xdif;
	// zvPtr->ZVY1 += Ydif;
	// zvPtr->ZVY2 += Ydif;
	// zvPtr->ZVZ1 += Zdif;
	// zvPtr->ZVZ2 += Zdif;
}

void initCopyBox(char *var0, char *var1) {
	screenSm1 = var0;
	screenSm2 = var0;

	screenSm3 = var1;
	screenSm4 = var1;
	screenSm5 = var1;
}

void menuWaitVSync() {
}

void executeFoundLife(int objIdx) {
	assert(0);
	// int var_2;
	// int actorIdx;
	// int lifeOffset;
	// int currentActorIdx;
	// int currentActorLifeIdx;
	// int currentActorLifeNum;
	// int foundLife;
	// tObject *currentActorPtr;
	// tObject *currentActorLifePtr;

	// if (objIdx == -1)
	// 	return;

	// foundLife = ListWorldObjets[objIdx].foundLife;

	// if (ListWorldObjets[objIdx].foundLife == -1)
	// 	return;

	// currentActorPtr = currentProcessedActorPtr;
	// currentActorIdx = currentProcessedActorIdx;
	// currentActorLifeIdx = currentLifeActorIdx;
	// currentActorLifePtr = currentLifeActorPtr;
	// currentActorLifeNum = currentLifeNum;

	// if (currentLifeNum != -1) {
	// 	lifeOffset = (currentLifePtr - HQR_Get(listLife, currentActorLifeNum)) / 2;
	// }

	// var_2 = 0;

	// actorIdx = ListWorldObjets[objIdx].objIndex;

	// if (actorIdx == -1) {
	// 	tObject *currentActorEntryPtr = &objectTable[NUM_MAX_OBJECT - 1];
	// 	int currentActorEntry = NUM_MAX_OBJECT - 1;

	// 	while (currentActorEntry >= 0) {
	// 		if (currentActorEntryPtr->indexInWorld == -1)
	// 			break;

	// 		currentActorEntryPtr--;
	// 		currentActorEntry--;
	// 	}

	// 	if (currentActorEntry == -1) // no space, we will have to overwrite the last actor !
	// 	{
	// 		currentActorEntry = NUM_MAX_OBJECT - 1;
	// 		currentActorEntryPtr = &objectTable[NUM_MAX_OBJECT - 1];
	// 	}

	// 	actorIdx = currentActorEntry;
	// 	var_2 = 1;

	// 	currentProcessedActorPtr = &objectTable[actorIdx];
	// 	currentLifeActorPtr = &objectTable[actorIdx];
	// 	currentProcessedActorIdx = actorIdx;
	// 	currentLifeActorIdx = actorIdx;

	// 	currentProcessedActorPtr->indexInWorld = objIdx;
	// 	currentProcessedActorPtr->life = -1;
	// 	currentProcessedActorPtr->bodyNum = -1;
	// 	currentProcessedActorPtr->_flags = 0;
	// 	currentProcessedActorPtr->trackMode = -1;
	// 	currentProcessedActorPtr->room = -1;
	// 	currentProcessedActorPtr->lifeMode = -1;
	// 	currentProcessedActorPtr->ANIM = -1;
	// }

	// processLife(foundLife, true);

	// if (var_2) {
	// 	currentProcessedActorPtr->indexInWorld = -1;
	// }

	// currentProcessedActorPtr = currentActorPtr;
	// currentProcessedActorIdx = currentActorIdx;
	// currentLifeActorIdx = currentActorLifeIdx;
	// currentLifeActorPtr = currentActorLifePtr;

	// if (currentActorLifeNum != -1) {
	// 	currentLifeNum = currentActorLifeNum;
	// 	currentLifePtr = HQR_Get(listLife, currentLifeNum) + lifeOffset * 2;
	// }
}

} // namespace Fitd
