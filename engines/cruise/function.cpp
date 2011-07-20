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
 *
 */

#include "cruise/cruise.h"
#include "cruise/cruise_main.h"
#include "cruise/cell.h"
#include "cruise/sound.h"
#include "cruise/staticres.h"

#include "common/textconsole.h"
#include "common/util.h"

namespace Cruise {

uint32 Period(uint32 hz) {
	return ((uint32)(100000000L / ((uint32)hz * 28L)));
}

//#define FUNCTION_DEBUG

int16 Op_LoadOverlay() {
	char *pOverlayName;
	char overlayName[38] = "";
	int overlayLoadResult;

	pOverlayName = (char *)stack.popPtr();

	if (strlen(pOverlayName) == 0)
		return 0;

	strcpy(overlayName, pOverlayName);
	strToUpper(overlayName);

	//gfxModuleData.field_84();
	//gfxModuleData.field_84();

	overlayLoadResult = loadOverlay(overlayName);

	updateAllScriptsImports();

	Common::strlcpy(nextOverlay, overlayName, sizeof(nextOverlay));

	return overlayLoadResult;
}

int16 Op_Strcpy() {
	char *ptr1 = (char *)stack.popPtr();
	char *ptr2 = (char *)stack.popPtr();

	while (*ptr1) {
		*ptr2 = *ptr1;

		ptr2++;
		ptr1++;
	}

	*ptr2 = 0;

	return (0);
}

int16 Op_Exec() {
	int scriptIdx;
	int ovlIdx;
	uint8 *ptr;
	uint8 *ptr2;
	int16 popTable[200];

	int numOfArgToPop = stack.popVar();

	int i = 0;

	for (i = 0; i < numOfArgToPop; i++) {
		popTable[numOfArgToPop - i - 1] = stack.popVar();
	}

	scriptIdx = stack.popVar();
	ovlIdx = stack.popVar();

	if (!ovlIdx) {
		ovlIdx = currentScriptPtr->_overlayNumber;
	}

	ptr = procHead.add(ovlIdx, scriptIdx, currentScriptPtr->_type, currentScriptPtr->_scriptNumber, currentScriptPtr->_overlayNumber, scriptType_MinusPROC);

	if (!ptr)
		return (0);

	if (numOfArgToPop <= 0) {
		return (0);
	}

	ptr2 = ptr;

	for (i = 0; i < numOfArgToPop; i++) {
		WRITE_BE_UINT16(ptr2, popTable[i]);
		ptr2 += 2;
	}

	return (0);
}

int16 Op_AddProc() {
	int pop1 = stack.popVar();
	int pop2;
	int overlay;
	int param[160];

	for (long int i = 0; i < pop1; i++) {
		param[i] = stack.popVar();
	}

	pop2 = stack.popVar();
	overlay = stack.popVar();

	if (!overlay)
		overlay = currentScriptPtr->_overlayNumber;

	if (!overlay)
		return (0);

	uint8* procBss = procHead.add(overlay, pop2, currentScriptPtr->_type, currentScriptPtr->_scriptNumber, currentScriptPtr->_overlayNumber, scriptType_PROC);

	if (procBss) {
		for (long int i = 0; i < pop1; i++) {
			int16* ptr = (int16 *)(procBss + i * 2);
			*ptr = param[i];
			bigEndianShortToNative(ptr);
		}
	}

	return (0);
}

int16 Op_Narrator() {
	int pop1 = stack.popVar();
	int pop2 = stack.popVar();

	if (!pop2)
		pop2 = currentScriptPtr->_overlayNumber;

	narratorOvl = pop2;
	narratorIdx = pop1;

	return (0);
}

int16 Op_GetMouseX() {
	int16 dummy;
	int16 mouseX;
	int16 mouseY;
	int16 mouseButton;

	currentMouse.getStatus(&dummy, &mouseX, &mouseButton, &mouseY);

	return (mouseX);
}

int16 Op_GetMouseY() {
	int16 dummy;
	int16 mouseX;
	int16 mouseY;
	int16 mouseButton;

	currentMouse.getStatus(&dummy, &mouseX, &mouseButton, &mouseY);

	return (mouseY);
}

int16 Op_Random() {
	int var = stack.popVar();

	if (var < 2) {
		return (0);
	}

	return (_vm->_rnd.getRandomNumber(var - 1));
}

int16 Op_PlayFX() {
	int volume = stack.popVar();
	int speed = stack.popVar();
	/*int channelNum = */stack.popVar();
	int sampleNum = stack.popVar();

	if ((sampleNum >= 0) && (sampleNum < NUM_FILE_ENTRIES) && (filesDatabase[sampleNum].subData.ptr)) {
		if (speed == -1)
			speed = filesDatabase[sampleNum].subData.transparency;

		_vm->sound().playSound(filesDatabase[sampleNum].subData.ptr,
			filesDatabase[sampleNum].width, volume);
	}

	return (0);
}

int16 Op_LoopFX() {
	int volume = stack.popVar();
	int speed = stack.popVar();
	/*int channelNum = */stack.popVar();
	int sampleNum = stack.popVar();

	if ((sampleNum >= 0) && (sampleNum < NUM_FILE_ENTRIES) && (filesDatabase[sampleNum].subData.ptr)) {
		if (speed == -1)
			speed = filesDatabase[sampleNum].subData.transparency;

		_vm->sound().playSound(filesDatabase[sampleNum].subData.ptr,
			filesDatabase[sampleNum].width, volume);
	}

	return (0);
}

int16 Op_StopFX() {
	int channelNum = stack.popVar();

	if (channelNum == -1) {
		_vm->sound().stopChannel(0);
		_vm->sound().stopChannel(1);
		_vm->sound().stopChannel(2);
		_vm->sound().stopChannel(3);
	} else {
		_vm->sound().stopChannel(channelNum);
	}

	return 0;
}

int16 Op_FreqFX() {
	int volume = stack.popVar();
	int freq2 = stack.popVar();
	int channelNum = stack.popVar();
	int sampleNum = stack.popVar();

	if ((sampleNum >= 0) && (sampleNum < NUM_FILE_ENTRIES) && (filesDatabase[sampleNum].subData.ptr)) {
		int freq = Period(freq2 * 1000);

		_vm->sound().startNote(channelNum, volume, freq);
	}

	return (0);
}

int16 Op_FreeCT() {
	freeCTP();
	return (0);
}

void freeObjectList(CellListNode *pListHead) {
	int var_2 = 0;
	CellListNode *pCurrent = pListHead->_next;

	while (pCurrent) {
		CellListNode *pNext = pCurrent->_next;

		if (pCurrent->_cell->_freeze == 0) {
			delete pCurrent->_cell;
			MemFree(pCurrent);
		}

		var_2 = 1;

		pCurrent = pNext;
	}

	if (var_2) {
		pListHead->resetPtr();
	}
}

int16 Op_FreeCell() {
	freeObjectList(&cellHead);
	return (0);
}

int16 Op_freeBackgroundInscrustList() {
	backgroundIncrustListHead.freeBackgroundIncrustList();
	return (0);
}


int16 Op_UnmergeBackgroundIncrust() {
	int obj = stack.popVar();
	int ovl = stack.popVar();

	if (!ovl) {
		ovl = currentScriptPtr->_overlayNumber;
	}

	backgroundIncrustListHead.unmergeBackgroundIncrustList(ovl, obj);

	return (0);
}

int16 Op_FreePreload() {
	// TODO: See if this is needed
	debug(1, "Op_FreePreload not implemented");
	return (0);
}

int16 Op_RemoveMessage() {
	int idx;
	int overlay;

	idx = stack.popVar();
	overlay = stack.popVar();

	if (!overlay) {
		overlay = currentScriptPtr->_overlayNumber;
	}

	cellHead.removeCell(overlay, idx, 5, masterScreen);

	return (0);
}

int16 Op_FindSet() {
	int16 i;
	char name[36] = "";
	char *ptr;

	ptr = (char *) stack.popPtr();

	if (!ptr) {
		return -1;
	}

	strcpy(name, ptr);
	strToUpper(name);

	for (i = 0; i < NUM_FILE_ENTRIES; i++) {
		if (!strcmp(name, filesDatabase[i].subData.name)) {
			return (i);
		}
	}

	return -1;
}

int16 Op_RemoveFrame() {
	int count = stack.popVar();
	int start = stack.popVar();

	resetFileEntryRange(start, count);

	return (0);
}

int16 Op_comment() {
	char *var;

	var = (char *)stack.popPtr();

	debug(1, "COMMENT: \"%s\"", var);

	return (0);
}

int16 Op_RemoveProc() {
	int idx;
	int overlay;

	idx = stack.popVar();
	overlay = stack.popVar();

	if (!overlay) {
		overlay = currentScriptPtr->_overlayNumber;
	}

	procHead.remove(overlay, idx);

	return (0);
}

int16 Op_FreeOverlay() {
	char localName[36] = "";
	char *namePtr;

	namePtr = (char *) stack.popPtr();

	strcpy(localName, namePtr);

	if (localName[0]) {
		strToUpper(localName);
		releaseOverlay((char *)localName);
	}

	return 0;
}

int16 Op_FindProc() {
	char name[36] = "";
	char *ptr;
	int param;

	ptr = (char *)stack.popPtr();

	strcpy(name, ptr);

	param = getProcParam(stack.popVar(), 20, name);

	return param;
}

int16 Op_GetRingWord() {
	// Original method had a ringed queue allowing this method to return words one at a time.
	// But it never seemed to be used; no entries were ever added to the list
	return 0;
}

int16 Op_KillMenu() {
	// Free menus, if active
	if (menuTable[0]) {
		delete menuTable[0];
		menuTable[0] = NULL;
		currentActiveMenu = -1;
	}

	if (menuTable[1]) {
		delete menuTable[1];
		menuTable[1] = NULL;
		currentActiveMenu = -1;
	}

	// Free the message list
//	if (linkedMsgList) freeMsgList(linkedMsgList);
	linkedMsgList = NULL;
	linkedRelation = NULL;

	return 0;
}

int16 Op_UserMenu() {
	int oldValue = playerMenuEnabled;
	playerMenuEnabled = stack.popVar();

	return oldValue;
}

int16 Op_UserOn() {
	int oldValue = userEnabled;
	int newValue = stack.popVar();

	if (newValue != -1) {
		userEnabled = newValue;
	}

	return oldValue;
}

int16 Op_Display() {
	int oldValue = displayOn;
	int newValue = stack.popVar();

	if (newValue != -1) {
		displayOn = newValue;
	}

	return oldValue;
}

int16 Op_FreezeParent() {
	if (currentScriptPtr->_var1A == 20) {
		changeScriptParamInList(currentScriptPtr->_var18, currentScriptPtr->_var16, &procHead, -1, 9997);
	} else if (currentScriptPtr->_var1A == 30) {
		changeScriptParamInList(currentScriptPtr->_var18, currentScriptPtr->_var16, &relHead, -1, 9997);
	}

	return 0;
}

int16 Op_LoadBackground() {
	int result = 0;
	char bgName[36] = "";
	char *ptr;
	int bgIdx;

	ptr = (char *) stack.popPtr();

	strcpy(bgName, ptr);

	bgIdx = stack.popVar();

	if (bgIdx >= 0 || bgIdx < NBSCREENS) {
		strToUpper(bgName);

		gfxModuleData_gfxWaitVSync();
		gfxModuleData_gfxWaitVSync();

		result = loadBackground(bgName, bgIdx);

		gfxModuleData_addDirtyRect(Common::Rect(0, 0, 320, 200));
	}

	currentMouse.changeCursor(CURSOR_NORMAL);

	return result;
}

int16 Op_FrameExist() {
	int param;

	param = stack.popVar();

	if (param < 0 || param > 255) {
		return 0;
	}

	if (filesDatabase[param].subData.ptr) {
		return 1;
	}

	return 0;
}

int16 Op_LoadFrame() {
	int param1;
	int param2;
	int param3;
	char name[36] = "";
	char *ptr;

	ptr = (char *) stack.popPtr();

	strcpy(name, ptr);

	param1 = stack.popVar();
	param2 = stack.popVar();
	param3 = stack.popVar();

	if (param3 >= 0 || param3 < NUM_FILE_ENTRIES) {
		strToUpper(name);

		gfxModuleData_gfxWaitVSync();
		gfxModuleData_gfxWaitVSync();

		lastAni[0] = 0;

		loadFileRange(name, param2, param3, param1);

		lastAni[0] = 0;
	}

	currentMouse.changeCursor(CURSOR_NORMAL);
	return 0;
}

int16 Op_LoadAbs() {
	int slot;
	char name[36] = "";
	char *ptr;
	int result = 0;

	ptr = (char *) stack.popPtr();
	slot = stack.popVar();

	if ((slot >= 0) && (slot < NUM_FILE_ENTRIES)) {
		strcpy(name, ptr);
		strToUpper(name);

		gfxModuleData_gfxWaitVSync();
		gfxModuleData_gfxWaitVSync();

		result = loadFullBundle(name, slot);
	}

	currentMouse.changeCursor(CURSOR_NORMAL);
	return result;
}

int16 Op_InitializeState() {
	int param1 = stack.popVar();
	int objIdx = stack.popVar();
	int ovlIdx = stack.popVar();

	if (!ovlIdx)
		ovlIdx = currentScriptPtr->_overlayNumber;

#ifdef FUNCTION_DEBUG
	debug(1, "Init %s state to %d", getObjectName(objIdx, overlayTable[ovlIdx].ovlData->arrayNameObj), param1);
#endif

	objInit(ovlIdx, objIdx, param1);

	return (0);
}

int16 Op_GetlowMemory() {
	return lowMemory;
}

int16 Op_AniDir() {
	int type = stack.popVar();
	int objIdx = stack.popVar();
	int ovlIdx = stack.popVar();

	if (!ovlIdx)
		ovlIdx = currentScriptPtr->_overlayNumber;

	ActorListNode *pActorNode = actorHead.findActor(ovlIdx, objIdx, type);
	if (pActorNode)
		return pActorNode->_actor->_startDirection;

	return -1;
}

int16 Op_FadeOut() {
	for (long int i = 0; i < 256; i += 32) {
		for (long int j = 0; j < 256; j++) {
			int offsetTable[3];
			offsetTable[0] = -32;
			offsetTable[1] = -32;
			offsetTable[2] = -32;
			calcRGB(&workpal[3*j], &workpal[3*j], offsetTable);
		}
		gfxModuleData_setPal256(workpal);
		gfxModuleData_flipScreen();
	}

	memset(globalScreen, 0, 320 * 200);
	flip();

	fadeFlag = 1;
	PCFadeFlag = 1;

	return 0;
}

int16 isOverlayLoaded(const char * name) {
	int16 i;

	for (i = 1; i < numOfLoadedOverlay; i++) {
		if (!strcmp(overlayTable[i].overlayName, name) && overlayTable[i].alreadyLoaded) {
			return i;
		}
	}

	return 0;
}

int16 Op_FindOverlay() {
	char name[36] = "";
	char *ptr;

	ptr = (char *) stack.popPtr();

	strcpy(name, ptr);
	strToUpper(name);

	return (isOverlayLoaded(name));
}

int16 Op_WriteObject() {
	int16 returnParam;

	int16 param1 = stack.popVar();
	int16 param2 = stack.popVar();
	int16 param3 = stack.popVar();
	int16 param4 = stack.popVar();

	getSingleObjectParam(param4, param3, param2, &returnParam);
	setObjectPosition(param4, param3, param2, param1);

	return returnParam;
}

int16 Op_ReadObject() {
	int16 returnParam;

	int member = stack.popVar();
	int obj = stack.popVar();
	int ovl = stack.popVar();

	getSingleObjectParam(ovl, obj, member, &returnParam);

	return returnParam;
}

int16 Op_FadeIn() {
	doFade = 1;
	return 0;
}

int16 Op_GetMouseButton() {
	int16 dummy;
	int16 mouseX;
	int16 mouseY;
	int16 mouseButton;

	currentMouse.getStatus(&dummy, &mouseX, &mouseButton, &mouseY);

	return mouseButton;
}

int16 Op_AddCell() {
	int16 objType = stack.popVar();
	int16 objIdx = stack.popVar();
	int16 overlayIdx = stack.popVar();

	if (!overlayIdx)
		overlayIdx = currentScriptPtr->_overlayNumber;

	cellHead.addCell(overlayIdx, objIdx, objType, masterScreen, currentScriptPtr->_overlayNumber, currentScriptPtr->_scriptNumber, currentScriptPtr->_type);

	return 0;
}

int16 Op_AddBackgroundIncrust() {

	int16 objType = stack.popVar();
	int16 objIdx = stack.popVar();
	int16 overlayIdx = stack.popVar();

	if (!overlayIdx)
		overlayIdx = currentScriptPtr->_overlayNumber;

	backgroundIncrustListHead.addBackgroundIncrust(overlayIdx, objIdx, currentScriptPtr->_scriptNumber, currentScriptPtr->_overlayNumber, masterScreen, objType);

	return 0;
}

int16 Op_RemoveCell() {
	int objType = stack.popVar();
	int objectIdx = stack.popVar();
	int ovlNumber = stack.popVar();

	if (!ovlNumber) {
		ovlNumber = currentScriptPtr->_overlayNumber;
	}

	cellHead.removeCell(ovlNumber, objectIdx, objType, masterScreen);

	return 0;
}

int16 fontFileIndex = -1;

int16 Op_SetFont() {
	fontFileIndex = stack.popVar();

	return 0;
}

int16 Op_UnfreezeParent() {
	if (currentScriptPtr->_var1A == 0x14) {
		changeScriptParamInList(currentScriptPtr->_var18, currentScriptPtr->_var16, &procHead, -1, 0);
	} else if (currentScriptPtr->_var1A == 0x1E) {
		changeScriptParamInList(currentScriptPtr->_var18, currentScriptPtr->_var16, &relHead, -1, 0);
	}

	return 0;
}

int16 Op_ProtectionFlag() {
	int16 temp = protectionCode;
	int16 newVar;

	newVar = stack.popVar();
	if (newVar != -1) {
		protectionCode = newVar;
	}

	return temp;
}

int16 Op_ClearScreen() {
	int bgIdx = stack.popVar();

	if ((bgIdx >= 0) && (bgIdx < NBSCREENS) && (backgrounds[bgIdx]._backgroundScreen)) {
		memset(backgrounds[bgIdx]._backgroundScreen, 0, 320 * 200);
		backgrounds[bgIdx]._isChanged = true;
		strcpy(backgrounds[0]._backgroundTable.name, "");
	}

	return 0;
}

int16 Op_AddMessage() {
	int16 color = stack.popVar();
	int16 var_2 = stack.popVar();
	int16 var_4 = stack.popVar();
	int16 var_6 = stack.popVar();
	int16 var_8 = stack.popVar();
	int16 overlayIdx = stack.popVar();

	if (!overlayIdx)
		overlayIdx = currentScriptPtr->_overlayNumber;

	if (color == -1) {
		color = findHighColor();
	} else {
		if (CVTLoaded) {
			color = cvtPalette[color];
		}
	}

	cellHead.createTextObject(overlayIdx, var_8, var_6, var_4, var_2, color, masterScreen, currentScriptPtr->_overlayNumber, currentScriptPtr->_scriptNumber);

	return 0;
}

int16 Op_Preload() {
	stack.popPtr();
	stack.popVar();

	return 0;
}

int16 Op_LoadCt() {
	return initCt((const char *)stack.popPtr());
}

int16 Op_EndAnim() {
	int param1 = stack.popVar();
	int param2 = stack.popVar();
	int overlay = stack.popVar();

	if (!overlay)
		overlay = currentScriptPtr->_overlayNumber;

	return actorHead.isAnimFinished(overlay, param2, param1);
}

int16 Op_Protect() {
	stack.popPtr();
	stack.popVar();

	return 0;
}

int16 Op_AutoCell() {
	CellListNode *pObject;

	int signal = stack.popVar();
	int loop = stack.popVar();
	int wait = stack.popVar();
	int animStep = stack.popVar();
	int end = stack.popVar();
	int start = stack.popVar();
	int type = stack.popVar();
	int change = stack.popVar();
	int obj = stack.popVar();
	int overlay = stack.popVar();

	if (!overlay)
		overlay = currentScriptPtr->_overlayNumber;

	pObject = cellHead.addCell(overlay, obj, 4, masterScreen, currentScriptPtr->_overlayNumber, currentScriptPtr->_scriptNumber, currentScriptPtr->_type);

	if (!pObject)
		return 0;

	pObject->_cell->_animSignal = signal;
	pObject->_cell->_animLoop = loop;
	pObject->_cell->_animWait = wait;
	pObject->_cell->_animStep = animStep;
	pObject->_cell->_animEnd = end;
	pObject->_cell->_animStart = start;
	pObject->_cell->_animType = type;
	pObject->_cell->_animChange = change;

	if (type) {
		if (currentScriptPtr->_type == scriptType_PROC) {
			changeScriptParamInList(currentScriptPtr->_overlayNumber, currentScriptPtr->_scriptNumber, &procHead, -1, 9996);
		} else if (currentScriptPtr->_type == scriptType_REL) {
			changeScriptParamInList(currentScriptPtr->_overlayNumber, currentScriptPtr->_scriptNumber, &relHead, -1, 9996);
		}
	}

	if (change == 5) {
		objInit(pObject->_cell->_overlay, pObject->_cell->_idx, start);
	} else {
		setObjectPosition(pObject->_cell->_overlay, pObject->_cell->_idx, pObject->_cell->_animChange, start);
	}

	if (wait < 0) {
		objectParamsQuery params;

		getMultipleObjectParam(overlay, obj, &params);
		pObject->_cell->_animCounter = params.state2 - 1;
	}

	return 0;
}

int16 Op_Sizeof() {
	objectParamsQuery params;
	int index = stack.popVar();
	int overlay = stack.popVar();

	if (!overlay)
		overlay = currentScriptPtr->_overlayNumber;

	getMultipleObjectParam(overlay, index, &params);

	return params.nbState - 1;
}

int16 Op_SetActiveBackground() {
	int currentPlane = masterScreen;
	int newPlane = stack.popVar();

	if (newPlane >= 0 && newPlane < NBSCREENS) {
		if (backgrounds[newPlane]._backgroundScreen) {
			masterScreen = newPlane;
			backgrounds[newPlane]._isChanged = true;
			switchPal = 1;
		}
	}

	return currentPlane;
}

int16 Op_RemoveBackground() {
	int backgroundIdx = stack.popVar();

	if (backgroundIdx > 0 && backgroundIdx < 8) {
		if (backgrounds[backgroundIdx]._backgroundScreen)
			MemFree(backgrounds[backgroundIdx]._backgroundScreen);

		if (masterScreen == backgroundIdx) {
			masterScreen = 0;
			backgrounds[0]._isChanged = true;
		}

		strcpy(backgrounds[backgroundIdx]._backgroundTable.name, "");
	} else {
		strcpy(backgrounds[0]._backgroundTable.name, "");
	}

	return (0);
}

int vblLimit;

int16 Op_VBL() {
	vblLimit = stack.popVar();
	return 0;
}

int op7BVar = 0;

int16 Op_Sec() {
	int di = stack.popVar();
	int si = 1 - op7BVar;
	int sign;

	if (di) {
		sign = di / (ABS(di));
	} else {
		sign = 0;
	}

	op7BVar = -sign;

	return si;
}

int16 Op_RemoveBackgroundIncrust() {
	int idx = stack.popVar();
	int overlay = stack.popVar();

	if (!overlay) {
		overlay = currentScriptPtr->_overlayNumber;
	}

	backgroundIncrustListHead.removeBackgroundIncrustNode(overlay, idx);

	return 0;
}

int16 Op_SetColor()	{
	int colorB = stack.popVar();
	int colorG = stack.popVar();
	int colorR = stack.popVar();
	int endIdx = stack.popVar();
	int startIdx = stack.popVar();

	int i;

#define convertRatio 36.571428571428571428571428571429

	for (i = startIdx; i <= endIdx; i++) {
		int offsetTable[3];

		offsetTable[0] = (int)(colorR * convertRatio);
		offsetTable[1] = (int)(colorG * convertRatio);
		offsetTable[2] = (int)(colorB * convertRatio);

		if (CVTLoaded) {
			int colorIdx = cvtPalette[i];
			calcRGB(&palScreen[masterScreen][3*colorIdx], &workpal[3*colorIdx], offsetTable);
		} else {
			calcRGB(&palScreen[masterScreen][3*i], &workpal[3*i], offsetTable);
		}
	}

	gfxModuleData_setPalEntries(workpal, 0, 32);

	return 0;
}

int16 Op_Inventory() {
	int si = var41;

	var41 = stack.popVar();

	return si;
}

int16 Op_RemoveOverlay() {
	int overlayIdx;

	overlayIdx = stack.popVar();

	if (strlen(overlayTable[overlayIdx].overlayName)) {
		releaseOverlay(overlayTable[overlayIdx].overlayName);
	}

	return 0;
}

int16 Op_ComputeLine() {
	int y2 = stack.popVar();
	int x2 = stack.popVar();
	int y1 = stack.popVar();
	int x1 = stack.popVar();

	point* pDest = (point *)stack.popPtr();

	int maxValue = cor_droite(x1, y1, x2, y2, pDest);

	flipGen(pDest, maxValue * 4);

	return maxValue;
}

int16 Op_FindMsg() {
	int si = stack.popVar();
	stack.popVar();

	return si;
}

int16 Op_SetZoom() {
	var46 = stack.popVar();
	var45 = stack.popVar();
	var42 = stack.popVar();
	var39 = stack.popVar();
	return 0;
}

int16 computeZoom(int param) {
	return (((param - var46) * (var39 - var42)) / (var45 - var46)) + var42;
}

int16 subOp23(int param1, int param2) {
	return (param1 * param2) >> 8;
}

int16 Op_GetStep() {
	int si = stack.popVar();
	int dx = stack.popVar();

	return subOp23(dx, si);
}

int16 Op_GetZoom() {
	return (computeZoom(stack.popVar()));
}

int flag_obstacle;		// numPolyBis

// add animation
int16 Op_AddAnimation() {
	int stepY = stack.popVar();
	int stepX = stack.popVar();
	int direction = stack.popVar();
	int start = stack.popVar();
	int type = stack.popVar();
	int obj = stack.popVar();
	int overlay = stack.popVar();

	if (!overlay) {
		overlay = currentScriptPtr->_overlayNumber;
	}

	if (direction >= 0 && direction <= 3) {
		ActorListNode *si;

		si = actorHead.addActor(overlay, obj, direction, type);

		if (si) {
			objectParamsQuery params;

			getMultipleObjectParam(overlay, obj, &params);

			si->_actor->_x = params.X;
			si->_actor->_y = params.Y;
			si->_actor->_xDest = -1;
			si->_actor->_yDest = -1;
			si->_actor->_endDirection = -1;
			si->_actor->_start = start;
			si->_actor->_stepX = stepX;
			si->_actor->_stepY = stepY;

			int newFrame = ABS(actor_end[direction][0]) - 1;

			int zoom = computeZoom(params.Y);

			if (actor_end[direction][0] < 0) {
				zoom = -zoom;
			}

			getPixel(params.X, params.Y);

			setObjectPosition(overlay, obj, 3, newFrame + start);
			setObjectPosition(overlay, obj, 4, zoom);
			setObjectPosition(overlay, obj, 5, numPoly);

			animationStart = false;
		}
	}

	return 0;
}

int16 Op_RemoveAnimation() {
	int objType = stack.popVar();
	int objIdx = stack.popVar();
	int ovlIdx = stack.popVar();

	if (!ovlIdx) {
		ovlIdx = currentScriptPtr->_overlayNumber;
	}

	return actorHead.removeActor(ovlIdx, objIdx, objType);
}

int16 Op_regenerateBackgroundIncrust() {
	backgroundIncrustListHead.regenerateBackgroundIncrustList();
	return 0;
}

int16 Op_SetStringColors() {
	// TODO: here ignore if low color mode

	subColor = (uint8) stack.popVar();
	itemColor = (uint8) stack.popVar();
	selectColor = (uint8) stack.popVar();
	titleColor = (uint8) stack.popVar();

	return 0;
}

int16 Op_XClick() {
	int x = stack.popVar();

	if (x != -1) {
		aniX = x;
		animationStart = true;
	}

	return aniX;
}

int16 Op_YClick() {
	int y = stack.popVar();

	if (y != -1) {
		aniY = y;
		animationStart = true;
	}

	return aniY;
}

int16 Op_GetPixel() {
	int x = stack.popVar();
	int y = stack.popVar();

	getPixel(x, y);
	return numPoly;
}

int16 Op_TrackAnim() {		// setup actor position
	ActorListNode *pActor;

	int var0 = stack.popVar();
	int actorY = stack.popVar();
	int actorX = stack.popVar();
	int var1 = stack.popVar();
	int var2 = stack.popVar();
	int overlay = stack.popVar();

	if (!overlay) {
		overlay = currentScriptPtr->_overlayNumber;
	}

	pActor = actorHead.findActor(overlay, var2, var1);

	if (!pActor) {
		return 1;
	}

	animationStart = false;

	pActor->_actor->_xDest = actorX;
	pActor->_actor->_yDest = actorY;
	pActor->_actor->_flag = 1;
	pActor->_actor->_endDirection = var0;

	return 0;
}

int16 Op_BgName() {
	char* bgName = (char *)stack.popPtr();
	int bgIdx = stack.popVar();

	if ((bgIdx >= 0) && (bgIdx < NBSCREENS) && bgName) {
		strcpy(bgName, backgrounds[bgIdx]._backgroundTable.name);

		if (strlen(bgName))
			return 1;

		return 0;
	}

	return 0;
}

int16 Op_LoadSong() {
	const char *ptr = (const char *)stack.popPtr();
	char buffer[33];

	strcpy(buffer, ptr);
	strToUpper(buffer);
	_vm->sound().loadMusic(buffer);

	currentMouse.changeCursor(CURSOR_NORMAL);
	return 0;
}

int16 Op_PlaySong() {
	if (_vm->sound().songLoaded() && !_vm->sound().songPlayed())
		_vm->sound().playMusic();

	return 0;
}

int16 Op_StopSong() {
	if (_vm->sound().isPlaying())
		_vm->sound().stopMusic();

	return 0;
}

int16 Op_RestoreSong() {
	// Used in the original to restore the contents of a song. Doesn't seem to be used,
	// since the backup buffer it uses is never set
	return 0;
}

int16 Op_SongSize() {
	int size, oldSize;

	if (_vm->sound().songLoaded()) {
		oldSize = _vm->sound().numOrders();

		size = stack.popVar();
		if ((size >= 1) && (size < 128))
			_vm->sound().setNumOrders(size);
	} else
		oldSize = 0;

	return oldSize;
}

int16 Op_SetPattern() {
	int value = stack.popVar();
	int offset = stack.popVar();

	if (_vm->sound().songLoaded()) {
		_vm->sound().setPattern(offset, value);
	}

	return 0;
}

int16 Op_FadeSong() {
	_vm->sound().fadeSong();

	return 0;
}

int16 Op_FreeSong() {
	_vm->sound().stopMusic();
	_vm->sound().removeMusic();
	return 0;
}

int16 Op_SongLoop() {
	bool oldLooping = _vm->sound().musicLooping();
	_vm->sound().musicLoop(stack.popVar() != 0);

	return oldLooping;
}

int16 Op_SongPlayed() {
	return _vm->sound().songPlayed();
}

void setVar49Value(int value) {
	flagCt = value;
}

int16 Op_CTOn() {
	setVar49Value(1);
	return 0;
}

int16 Op_CTOff() {
	setVar49Value(0);
	return 0;
}

int16 Op_FreezeOverlay() {
	//int var0;
	//int var1;
	int temp;

	int var0 = stack.popVar();
	int var1 = stack.popVar();

	if (!var1) {
		var1 = currentScriptPtr->_overlayNumber;
	}

	temp = overlayTable[var1].executeScripts;
	overlayTable[var1].executeScripts = var0;

	return temp;
}

int16 Op_FreezeCell() {
	int newFreezz = stack.popVar();
	int oldFreeze = stack.popVar();
	int backgroundPlante = stack.popVar();
	int objType = stack.popVar();
	int objIdx = stack.popVar();
	int overlayIdx = stack.popVar();

	if (!overlayIdx) {
		overlayIdx = currentScriptPtr->_overlayNumber;
	}

	cellHead.freezeCell(overlayIdx, objIdx, objType, backgroundPlante, oldFreeze, newFreezz);

	return 0;
}

void Op_60Sub(int overlayIdx, ActorListNode * pActorHead, int _var0, int _var1, int _var2, int _var3) {
	ActorListNode *pActor = pActorHead->findActor(overlayIdx, _var0, _var3);

	if (pActor) {
		if ((pActor->_actor->_freeze == _var2) || (_var2 == -1)) {
			pActor->_actor->_freeze = _var1;
		}
	}
}

int16 Op_FreezeAni() {
	/*
	 * int var0;
	 * int var1;
	 * int var2;
	 * int var3;
	 * int var4;
	 */

	int var0 = stack.popVar();
	int var1 = stack.popVar();
	int var2 = stack.popVar();
	int var3 = stack.popVar();
	int var4 = stack.popVar();

	if (!var4) {
		var4 = currentScriptPtr->_overlayNumber;
	}

	Op_60Sub(var4, &actorHead, var3, var0, var1, var2);

	return 0;
}

int16 Op_Itoa() {
	int nbp = stack.popVar();
	int param[160];
	char txt[40];
	char format[30];
	char nbf[20];

	for (int i = nbp - 1; i >= 0; i--)
		param[i] = stack.popVar();

	int val = stack.popVar();
	char* pDest = (char *)stack.popPtr();

	if (!nbp)
		sprintf(txt, "%d", val);
	else {
		strcpy(format, "%");
		sprintf(nbf, "%d", param[0]);
		strcat(format, nbf);
		strcat(format, "d");
		sprintf(txt, format, val);
	}

	for (int i = 0; txt[i]; i++)
		*(pDest++) = txt[i];
	*(pDest++) = '\0';

	return 0;
}

int16 Op_Strcat() {
	char *pSource = (char *)stack.popPtr();
	char *pDest = (char *)stack.popPtr();

	while (*pDest)
		pDest++;

	while (*pSource)
		*(pDest++) = *(pSource++);
	*(pDest++) = '\0';

	return 0;
}

int16 Op_FindSymbol() {
	int var0 = stack.popVar();
	char *ptr = (char *)stack.popPtr();
	int var1 = stack.popVar();

	if (!var1)
		var1 = currentScriptPtr->_overlayNumber;

	return getProcParam(var1, var0, ptr);
}

int16 Op_FindObject() {
	char var_26[36];
	char *ptr = (char *)stack.popPtr();
	int overlayIdx;

	var_26[0] = 0;

	if (ptr) {
		strcpy(var_26, ptr);
	}

	overlayIdx = stack.popVar();

	if (!overlayIdx)
		overlayIdx = currentScriptPtr->_overlayNumber;

	return getProcParam(overlayIdx, 40, var_26);
}

int16 Op_SetObjectAtNode() {
	int16 node = stack.popVar();
	int16 obj = stack.popVar();
	int16 ovl = stack.popVar();

	if (!ovl)
		ovl = currentScriptPtr->_overlayNumber;

	int nodeInfo[2];
	if (!(node < 0 || node >= routeCount)) {
		routes[node].getCoords(nodeInfo);
		setObjectPosition(ovl, obj, 0, nodeInfo[0]);
		setObjectPosition(ovl, obj, 1, nodeInfo[1]);
		setObjectPosition(ovl, obj, 2, nodeInfo[1]);
		setObjectPosition(ovl, obj, 4, computeZoom(nodeInfo[1]));
	}

	return 0;
}

int16 Op_GetNodeX() {
	int16 node = stack.popVar();
	int result;
	int nodeInfo[2];
	if (node < 0 || node >= routeCount) {
		result = -1;
	} else {
		result = routes[node].getCoords(nodeInfo);
	}

	ASSERT(result == 0);

	return nodeInfo[0];
}

int16 Op_GetNodeY() {
	int16 node = stack.popVar();
	int result;
	int nodeInfo[2];
	if (node < 0 || node >= routeCount) {
		result = -1;
	} else {
		result = routes[node].getCoords(nodeInfo);
	}
	ASSERT(result == 0);

	return nodeInfo[1];
}

int16 Op_SetVolume() {
	int oldVolume = _vm->sound().getVolume();
	int newVolume = stack.popVar();

	if (newVolume > 63) newVolume = 63;
	if (newVolume >= 0) {
		int volume = 63 - newVolume;
		_vm->sound().setVolume(volume);
	}

	return oldVolume >> 2;
}

int16 Op_SongExist() {
	const char *songName = (char *)stack.popPtr();

	if (songName) {
		char name[33];
		strcpy(name, songName);
		strToUpper(name);

		if (!strcmp(_vm->sound().musicName(), name))
			return 1;
	}

	return 0;
}

int16 Op_TrackPos() {
	// This function returns a variable that never seems to change from 0
	return 0;
}

int16 Op_SetNodeState() {
	int16 state = stack.popVar();
	int16 node = stack.popVar();

	if (node < 0 || node > WalkboxCount)
		return -1;


	return walkboxes[node].setState(state);
}

int16 Op_SetNodeColor() {
	int16 color = stack.popVar();
	int16 node = stack.popVar();

	if (node < 0 || node > WalkboxCount)
		return -1;

	return walkboxes[node].setColor(color);
}

int16 Op_SetXDial() {
	int16 old = xdial;
	xdial = stack.popVar();

	return old;
}

int16 Op_DialogOn() {
	dialogueObj = stack.popVar();
	dialogueOvl = stack.popVar();

	if (dialogueOvl == 0)
		dialogueOvl = currentScriptPtr->_overlayNumber;

	dialogueEnabled = true;

	return 0;
}

int16 Op_DialogOff() {
	dialogueEnabled = false;

	objectReset();

	if (menuTable[0]) {
		delete menuTable[0];
		menuTable[0] = NULL;
		currentMouse.changeCursor(CURSOR_NORMAL);
		currentActiveMenu = -1;
	}

	return 0;
}

int16 Op_LinkObjects() {
	int type = stack.popVar();
	int obj2 = stack.popVar();
	int ovl2 = stack.popVar();
	int obj = stack.popVar();
	int ovl = stack.popVar();

	if (!ovl)
		ovl = currentScriptPtr->_overlayNumber;
	if (!ovl2)
		ovl2 = currentScriptPtr->_overlayNumber;

	cellHead.linkCell(ovl, obj, type, ovl2, obj2);

	return 0;
}

int16 Op_UserClick() {
	sysKey = stack.popVar();
	sysY = stack.popVar();
	sysX = stack.popVar();

	return 0;
}

int16 Op_XMenuItem() {
	int index = stack.popVar();
	int count = 0;

	if (!menuTable[0] || (menuTable[0]->_numElements == 0))
		return 0;

	menuElementStruct *p = menuTable[0]->_ptrNextElement;

	while (p) {
		if (count == index)
			return p->x + 1;

		++count;
		p = p->next;
	}

	return 0;
}

int16 Op_YMenuItem() {
	int index = stack.popVar();
	int count = 0;

	if (!menuTable[0] || (menuTable[0]->_numElements == 0))
		return 0;

	menuElementStruct *p = menuTable[0]->_ptrNextElement;

	while (p) {
		if (count == index)
			return p->y + 1;

		++count;
		p = p->next;
	}

	return 0;
}


int16 Op_Menu() {
	return (int16)(menuTable[0] != NULL);
}

int16 Op_AutoControl() {
	int oldValue = automaticMode;
	int newValue = stack.popVar();

	if (newValue >= 0) {
		automaticMode = newValue;
		activeMouse = newValue;
	}

	return oldValue;
}

int16 Op_MouseMove() {
	int16 handle, button;
	Common::Point pt;

	currentMouse.getStatus(&handle, &pt.x, &button, &pt.y);

	// x/y parameters aren't used
	stack.popVar();
	stack.popVar();

	return 0;
}

int16 Op_MouseEnd() {
	if (automoveInc < automoveMax)
		return (int16)false;

	return (int16)true;
}

int16 Op_MsgExist() {
	return isMessage;
}

int16 Op_UserDelay() {
	int delay = stack.popVar();

	if (delay >= 0) {
		userDelay = delay;
	}

	return userDelay;
}

int16 Op_ThemeReset() {
	objectReset();

	return 0;
}

int16 Op_UserWait() {
	userWait = 1;
	if (currentScriptPtr->_type == scriptType_PROC) {
		changeScriptParamInList(currentScriptPtr->_overlayNumber, currentScriptPtr->_scriptNumber, &procHead, -1, 9999);
	} else if (currentScriptPtr->_type == scriptType_REL) {
		changeScriptParamInList(currentScriptPtr->_overlayNumber, currentScriptPtr->_scriptNumber, &relHead, -1, 9999);
	}

	return 0;
}

opcodeFunction opcodeTablePtr[] = {
	NULL, // 0x00
	Op_FadeIn,
	Op_FadeOut,
	Op_LoadBackground,
	Op_LoadAbs,
	Op_AddCell,
	Op_AddProc,
	Op_InitializeState,
	Op_RemoveCell,
	Op_FreeCell,
	Op_RemoveProc,
	Op_RemoveFrame,
	Op_LoadOverlay,
	Op_SetColor,
	Op_PlayFX,
	NULL,	// used to be debug

	Op_FreeOverlay, // 0x10
	Op_FindOverlay,
	NULL,	// used to be exec debug
	Op_AddMessage,
	Op_RemoveMessage,
	Op_UserWait,
	Op_FreezeCell,
	Op_LoadCt,
	Op_AddAnimation,
	Op_RemoveAnimation,
	Op_SetZoom,
	Op_SetObjectAtNode,
	Op_SetNodeState,
	Op_SetNodeColor,
	Op_TrackAnim,
	Op_GetNodeX,

	Op_GetNodeY, // 0x20
	Op_EndAnim,
	Op_GetZoom,
	Op_GetStep,
	Op_SetStringColors,
	Op_XClick,
	Op_YClick,
	Op_GetPixel,
	Op_UserOn,
	Op_FreeCT,
	Op_FindObject,
	Op_FindProc,
	Op_WriteObject,
	Op_ReadObject,
	Op_RemoveOverlay,
	Op_AddBackgroundIncrust,

	Op_RemoveBackgroundIncrust, // 0x30
	Op_UnmergeBackgroundIncrust,
	Op_freeBackgroundInscrustList,
	Op_DialogOn,
	Op_DialogOff,
	Op_UserDelay,
	Op_ThemeReset,
	Op_Narrator,
	Op_RemoveBackground,
	Op_SetActiveBackground,
	Op_CTOn,
	Op_CTOff,
	Op_Random,
	Op_LoadSong,
	Op_FadeSong,
	Op_PlaySong,

	Op_FreeSong, // 0x40
	Op_FrameExist,
	Op_SetVolume,
	Op_SongExist,
	Op_TrackPos,
	Op_StopSong,
	Op_RestoreSong,
	Op_SongSize,
	Op_SetPattern,
	Op_SongLoop,
	Op_SongPlayed,
	Op_LinkObjects,
	Op_UserClick,
	Op_XMenuItem,
	Op_YMenuItem,
	Op_Menu,

	Op_AutoControl, // 0x50
	Op_MouseMove,
	Op_MouseEnd,
	Op_MsgExist,
	Op_SetFont,
	NULL, // MergeMsg
	Op_Display,
	Op_GetMouseX,
	Op_GetMouseY,
	Op_GetMouseButton,
	Op_FindSet,
	Op_regenerateBackgroundIncrust,
	Op_BgName,
	Op_LoopFX,
	Op_StopFX,
	Op_FreqFX,

	Op_FreezeAni, // 0x60
	Op_FindMsg,
	Op_FreezeParent,
	Op_UnfreezeParent,
	Op_Exec,
	Op_AutoCell,
	Op_Sizeof,
	Op_Preload,
	Op_FreePreload,
	NULL, // DeletePreload
	Op_VBL,
	Op_LoadFrame,
	Op_FreezeOverlay,
	Op_Strcpy,
	Op_Strcat,
	Op_Itoa,

	Op_comment, // 0x70
	Op_ComputeLine,
	Op_FindSymbol,
	Op_SetXDial,
	Op_GetlowMemory,
	Op_AniDir,
	Op_Protect,
	Op_ClearScreen,
	Op_Inventory,
	Op_UserMenu,
	Op_GetRingWord,
	Op_Sec,
	Op_ProtectionFlag,
	Op_KillMenu,
};

int32 opcodeType8() {
	int opcode = getByteFromScript();

	if (!opcode)
		return (-21);

	if (opcode > 0x100)
		return (-21);

	if (opcode < ARRAYSIZE(opcodeTablePtr) && opcodeTablePtr[opcode]) {
		stack.pushVar(opcodeTablePtr[opcode]());
		return (0);
	} else {
		warning("Unsupported opcode %d in opcode type 8", opcode);
		stack.pushVar(0);
		// exit(1);
	}

	return 0;

}

} // End of namespace Cruise
