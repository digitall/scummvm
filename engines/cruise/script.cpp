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
#include "common/endian.h"
#include "common/textconsole.h"
#include "common/list.h"

namespace Cruise {

ScriptInstance::ScriptInstance() {
	_ccr = 0;
	_scriptOffset = 0;
	_data = 0;
	_dataSize = 0;
	_scriptNumber = 0;
	_overlayNumber = 0;
	_sysKey = 0;
	_freeze = 0;
	_var16 = 0;
	_var18 = 0;
	_var1A = 0;
}

opcodeFunction ScriptInstance::opcodeTablePtr[] = {
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
	NULL,   // used to be debug

	Op_FreeOverlay, // 0x10
	Op_FindOverlay,
	NULL,   // used to be exec debug
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



int8 ScriptInstance::getByte() {
	int8 var = *(int8 *)(currentData3DataPtr + _scriptOffset);
	++_scriptOffset;

	return (var);
}

short int ScriptInstance::getShort() {
	short int var = (int16)READ_BE_UINT16(currentData3DataPtr + _scriptOffset);
	_scriptOffset += 2;

	return (var);
}

// load opcode
int32 ScriptInstance::opcodeType0() {
	int index = 0;

	switch (currentScriptOpcodeType) {
	case 0: {
		stack.pushVar(getShort());
		return (0);
	}
	case 5:
		index = saveOpcodeVar;
	case 1: {
		uint8 *address = 0;
		int type = getByte();
		int ovl = getByte();
		short int offset = getShort();
		offset += index;

		int typ7 = type & 7;

		if (!typ7) {
			return (-10); // unresloved link
		}

		if (!ovl) {
			address = scriptDataPtrTable[typ7];
		} else  { // TODO:
			if (!overlayTable[ovl].alreadyLoaded) {
				return (-7);
			}

			if (!overlayTable[ovl].ovlData) {
				return (-4);
			}

			if (typ7 == 5) {
				address = overlayTable[ovl].ovlData->data4Ptr;
			} else {
				assert(0);
			}
		}

		address += offset;

		int size = (type >> 3) & 3;

		if (size == 1) {
			address += index;
			stack.pushVar((int16)READ_BE_UINT16(address));
			return 0;
		} else if (size == 2) {
			stack.pushVar(*address);
			return 0;
		} else {
			error("Unsupported code in opcodeType0 case 1");
		}
	}
	case 2: {
		int16 var_16;
		int di = getByte();
		int si = getByte();
		int var_2 = getShort();

		if (!si) {
			si = _overlayNumber;
		}

		if (getSingleObjectParam(si, var_2, di, &var_16)) {
			return -10;
		}

		stack.pushVar(var_16);
		return 0;
	}
	default:
		error("Unsupported type %d in opcodeType0", currentScriptOpcodeType);
	}
}

// save opcode
int32 ScriptInstance::opcodeType1() {
	int var = stack.popVar();
	int offset = 0;

	switch (currentScriptOpcodeType) {
	case 0: {
		return (0); // strange, but happens also in original interpreter
	}
	case 5: {
		offset = saveOpcodeVar;
	}
	case 1: {
		int var_A = 0;

		int byte1 = getByte();
		int byte2 = getByte();

		int short1 = getShort();

		int var_6 = byte1 & 7;

		int var_C = short1;

		uint8 *ptr = 0;
		int type2;

		if (!var_6)
			return (-10);

		var_C = short1;

		if (byte2) {
			if (!overlayTable[byte2].alreadyLoaded) {
				return (-7);
			}

			if (!overlayTable[byte2].ovlData) {
				return (-4);
			}

			if (var_6 == 5) {
				ptr = overlayTable[byte2].ovlData->data4Ptr + var_C;
			} else {
				ASSERT(0);
			}
		} else {
			ptr = scriptDataPtrTable[var_6] + var_C;
		}

		type2 = ((byte1 & 0x18) >> 3);

		switch (type2) {
		case 1: {
			WRITE_BE_UINT16(ptr + var_A + offset * 2, var);
			return 0;
		}
		case 2: {
			assert(ptr);
			*(ptr + var_A + offset) = var;
			return 0;
		}
		default:
			error("Unsupported code in opcodeType1 case 1");
		}

		break;
	}
	case 2: {
		int mode = getByte();
		int di = getByte();
		int var_4 = getShort();

		if (!di) {
			di = _overlayNumber;
		}

		if ((var == 0x85) && !strcmp((char *)currentCtpName, "S26.CTP") && !di && mode == 1) { // patch in bar
			var = 0x87;
		}

		setObjectPosition(di, var_4, mode, var);

		break;
	}
	case 4: {
		saveOpcodeVar = var;
		break;
	}
	default:
		error("Unsupported type %d in opcodeType1", currentScriptOpcodeType);
	}

	return (0);
}

int32 ScriptInstance::opcodeType2() {
	int index = 0;
	switch (currentScriptOpcodeType) {
	case 5:
		index = saveOpcodeVar;
	case 1: {
		uint8 *adresse = NULL;
		int type = getByte();
		int overlay = getByte();

		int offset = getShort();
		offset += index;

		int typ7 = type & 7;
		if (!typ7) {
			return (-10);
		}
		if (!overlay) {
			adresse = scriptDataPtrTable[typ7];
		} else {
			if (!overlayTable[overlay].alreadyLoaded) {
				return (-7);
			}
			if (!overlayTable[overlay].ovlData) {
				return (-4);
			}
			ASSERT(0);
		}

		adresse += offset;
		int size = (type >> 3) & 3;

		if (size == 1) {
			adresse += index;
			stack.pushPtr(adresse);
		} else if (size == 2) {
			stack.pushPtr(adresse);
		}

	}
	}

	return 0;
}

int32 ScriptInstance::opcodeType3() {   // math
	int pop1 = stack.popVar();
	int pop2 = stack.popVar();

	switch (currentScriptOpcodeType) {
	case 0: {
		stack.pushVar(pop1 + pop2);
		return (0);
	}
	case 1: {
		stack.pushVar(pop1 / pop2);
		return (0);
	}
	case 2: {
		stack.pushVar(pop1 - pop2);
		return (0);
	}
	case 3: {
		stack.pushVar(pop1 * pop2);
		return (0);
	}
	case 4: {
		stack.pushVar(pop1 % pop2);
		return (0);
	}
	case 7:
	case 5: {
		stack.pushVar(pop2 | pop1);
		return (0);
	}
	case 6: {
		stack.pushVar(pop2 & pop1);
		return (0);
	}
	}

	return 0;
}

int32 ScriptInstance::opcodeType4() {       // test
	int boolVar = 0;

	int var1 = stack.popVar();
	int var2 = stack.popVar();

	switch (currentScriptOpcodeType) {
	case 0: {
		if (var2 != var1)
			boolVar = 1;
		break;
	}
	case 1: {
		if (var2 == var1)
			boolVar = 1;
		break;
	}
	case 2: {
		if (var2 < var1)
			boolVar = 1;
		break;
	}
	case 3: {
		if (var2 <= var1)
			boolVar = 1;
		break;
	}
	case 4: {
		if (var2 > var1)
			boolVar = 1;
		break;
	}
	case 5: {
		if (var2 >= var1)
			boolVar = 1;
		break;
	}

	}

	stack.pushVar(boolVar);

	return (0);
}

int32 ScriptInstance::opcodeType5() {
	int offset = _scriptOffset;
	int short1 = getShort();
	int newSi = short1 + offset;
	int bitMask = _ccr;

	switch (currentScriptOpcodeType) {
	case 0: {
		if (!(bitMask & 1)) {
			_scriptOffset = newSi;
		}
		break;
	}
	case 1: {
		if (bitMask & 1) {
			_scriptOffset = newSi;
		}
		break;
	}
	case 2: {
		if (bitMask & 2) {
			_scriptOffset = newSi;
		}
		break;
	}
	case 3: {
		if (bitMask & 3) {
			_scriptOffset = newSi;
		}
		break;
	}
	case 4: {
		if (bitMask & 4) {
			_scriptOffset = newSi;
		}
		break;
	}
	case 5: {
		if (bitMask & 5) {
			_scriptOffset = newSi;
		}
		break;
	}
	case 6: {
		break;  // never
	}
	case 7: {
		_scriptOffset = newSi;    //always
		break;
	}
	}

	return (0);
}

int32 ScriptInstance::opcodeType6() {
	int si = 0;

	int pop = stack.popVar();

	if (!pop)
		si = 1;

	if (pop < 0) {
		si |= 4;
	}

	if (pop > 0) {
		si |= 2;
	}

	_ccr = si;

	return (0);
}

int32 ScriptInstance::opcodeType7() {
	int var1 = stack.popVar();
	int var2 = stack.popVar();

	stack.pushVar(var1);
	stack.pushVar(var2);

	return (0);
}

int32 ScriptInstance::opcodeType8() {
	int opcode = getByte();

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

int32 ScriptInstance::opcodeType9() {       // stop script
	//debug("Stop a script of overlay %s", overlayTable[_overlayNumber].overlayName);
	_scriptNumber = -1;
	return (1);
}

int32 ScriptInstance::opcodeType10() {  // break
	return (0);
}

int32 ScriptInstance::opcodeType11() {  // break
	return (1);
}

Common::List<ScriptInstance>::iterator ScriptList::begin() {

	return Common::List<ScriptInstance>::begin();
}

uint ScriptList::size() {

	return  Common::List<ScriptInstance>::size();
}

int ScriptList::remove(int overlay, int idx) {
	Common::List<ScriptInstance>::iterator iter = begin();

	while (iter != end()) {
		if (iter->_overlayNumber == overlay
		        && (iter->_scriptNumber == idx || idx == -1)) {
			iter->_scriptNumber = -1;
		}

		iter++;
	}

	return (0);
}

int ScriptList::removeFinished() {
	Common::List<ScriptInstance>::iterator iter =  begin();

	while (iter != end()) {
		if (iter->_scriptNumber == -1) {
			if (iter->_data)
				MemFree(iter->_data);
			iter = erase(iter);
		} else {
			iter++;
		}
	}

	return (0);
}

void ScriptList::removeAll() {
	Common::List<ScriptInstance>::iterator iter = begin();
	while (iter != end()) {
		if (iter->_data)
			MemFree((*iter)._data);
		iter = erase(iter);
	}
}

void ScriptList::resetPtr2() {
	if (begin() != end())
		begin()->_scriptNumber = -1;
}

uint8 *ScriptList::add(int16 overlayNumber, int16 param, int16 arg0, int16 arg1, int16 arg2, scriptTypeEnum scriptType) {
	int useArg3Neg = 0;
	ovlData3Struct *data3Ptr;
	ScriptInstance tempScript;
	int var_C;

	//debug("Starting script %d of overlay %s", param,overlayTable[overlayNumber].overlayName);

	if (scriptType < 0) {
		useArg3Neg = 1;
		scriptType = (scriptTypeEnum) - scriptType;
	}

	if (scriptType == 20) {
		data3Ptr = getOvlData3Entry(overlayNumber, param);
	} else {
		if (scriptType == 30) {
			data3Ptr = scriptFunc1Sub2(overlayNumber, param);
		} else {
			return (NULL);
		}
	}

	if (!data3Ptr) {
		return (NULL);
	}

	if (!data3Ptr->dataPtr) {
		return (NULL);
	}

	var_C = data3Ptr->sysKey;

	tempScript._data = NULL;

	if (var_C) {
		tempScript._data = (uint8 *) mallocAndZero(var_C);
	}

	tempScript._dataSize = var_C;
	tempScript._scriptOffset = 0;

	tempScript._scriptNumber = param;
	tempScript._overlayNumber = overlayNumber;

	if (scriptType == 20) { // Obj or not ?
		tempScript._sysKey = useArg3Neg;
	} else {
		tempScript._sysKey = 1;
	}

	tempScript._freeze = 0;
	tempScript._type = scriptType;
	tempScript._var18 = arg2;
	tempScript._var16 = arg1;
	tempScript._var1A = arg0;

	push_back(tempScript);
	return (tempScript._data);
}

void ScriptList::add(ScriptInstance scriptToAdd) {
	push_back(scriptToAdd);
}

int ScriptInstance::execute() {
	int numScript2;
	ovlData3Struct *ptr2;
	ovlDataStruct *ovlData;
	uint8 opcodeType;

	numScript2 = _scriptNumber;

	if (_type == 20) {
		ptr2 = getOvlData3Entry(_overlayNumber, numScript2);

		if (!ptr2) {
			return (-4);
		}
	} else {
		if (_type == 30) {
			ptr2 = scriptFunc1Sub2(_overlayNumber, numScript2);

			if (!ptr2) {
				return (-4);
			}
		} else {
			return (-6);
		}
	}

	if (!overlayTable[_overlayNumber].alreadyLoaded) {
		return (-7);
	}

	ovlData = overlayTable[_overlayNumber].ovlData;

	if (!ovlData)
		return (-4);

	currentData3DataPtr = ptr2->dataPtr;

	scriptDataPtrTable[1] = (uint8 *) _data;
	scriptDataPtrTable[2] = getDataFromData3(ptr2, 1);
	scriptDataPtrTable[5] = ovlData->data4Ptr;  // free strings
	scriptDataPtrTable[6] = ovlData->ptr8;

	currentScriptPtr = this;

	stack.reset();

	do {
#ifdef SKIP_INTRO
		if (_scriptOffset == 290 && _overlayNumber == 4 && _scriptNumber == 0) {
			_scriptOffset = 923;
		}
#endif
		opcodeType = getByte();

		debugC(5, kCruiseDebugScript, "Script %s/%d ip=%d opcode=%d",
		       overlayTable[_overlayNumber].overlayName, _scriptNumber,
		       _scriptOffset, (opcodeType & 0xFB) >> 3);
		currentScriptOpcodeType = opcodeType & 7;

	} while (!executeScript((opcodeType & 0xFB) >> 3));

	currentScriptPtr = NULL;

	return (0);
}

int32 ScriptInstance::executeScript(int16 opCode) {
	switch (opCode) {
	case 1:
		return  opcodeType0();
	case 2:
		return  opcodeType1();
	case 3:
		return  opcodeType2();
	case 4:
		return  opcodeType3();
	case 5:
		return  opcodeType4();
	case 6:
		return  opcodeType5();
	case 7:
		return  opcodeType6();
	case 8:
		return  opcodeType7();
	case 9:
		return  opcodeType8();
	case 10:
		return opcodeType9();
	case 11:
		return opcodeType10();
	case 12:
		return opcodeType11();
	default:
		error("Unsupported opcode type %d", opCode);
	}
}

void ScriptList::manage() {
	Common::List<ScriptInstance>::iterator iter = begin();
	while (iter != end()) {
		if (!overlayTable[iter->_overlayNumber].executeScripts) {
			if ((iter->_scriptNumber != -1) && (iter->_freeze == 0) && (iter->_sysKey != 0)) {
				iter->execute();
			}

			if (iter->_sysKey == 0) {
				iter->_sysKey = 1;
			}
		}

		iter++;

	}

}

void ScriptList::scriptFunc2(int scriptNumber, int param, int param2) {
	Common::List<ScriptInstance>::iterator iter = begin();
	if (iter != end()) {
		if (scriptNumber == iter->_overlayNumber
		        || scriptNumber != -1) {
			if (param2 == iter->_scriptNumber
			        || param2 != -1) {
				iter->_sysKey = param;
			}
		}
	}
}

void ScriptList::changeParam(int param1, int param2, int oldFreeze, int newValue) {
	Common::List<ScriptInstance>::iterator iter = begin();
	while (iter != end()) {
		if ((iter->_overlayNumber == param1) || (param1 == -1))
			if ((iter->_scriptNumber == param2) || (param2 == -1))
				if ((iter->_freeze == oldFreeze) || (oldFreeze == -1)) {
					iter->_freeze = newValue;
				}
		iter++;
	}
}

} // End of namespace Cruise
