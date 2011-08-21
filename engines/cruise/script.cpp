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

ScriptInstance::ScriptInstance(int16 overlayNumber, int16 scriptNumber, int16 var1A, int16 var16, int16 var18, scriptTypeEnum scriptType, int dataSize, int useArg3Neg) {
	if(dataSize) {
			_dataSize = dataSize;
			_data = (uint8 *)mallocAndZero(_dataSize);
	} else {
			_dataSize = 0;
			_data = NULL;
	}
	_scriptOffset = 0;

	_scriptNumber = scriptNumber;
	_overlayNumber = overlayNumber;
	_freeze = 0;
	_type = scriptType;
	_var18 = var18;
	_var16 = var16;
	_var1A = var1A;
	_ccr = 0;
	if(_type == 20)
			_sysKey = useArg3Neg;
	else
			_sysKey = 1;
}

void ScriptInstance::remove() {
	if (_data)
		MemFree(_data);
}

int16 ScriptInstance::operateFunction(int opCode) {
	switch (opCode) {
		//case 0:NULL // 0x00
		case 1: return Op_FadeIn();
		case 2: return Op_FadeOut();
		case 3: return Op_LoadBackground();
		case 4: return Op_LoadAbs();
		case 5: return Op_AddCell();
		case 6: return Op_AddProc();
		case 7: return Op_InitializeState();
		case 8: return Op_RemoveCell();
		case 9: return Op_FreeCell();
		case 10: return Op_RemoveProc();
		case 11: return Op_RemoveFrame();
		case 12: return Op_LoadOverlay();
		case 13: return Op_SetColor();
		case 14: return Op_PlayFX();
		//case 15: NULL used to be debug

		case 16: return Op_FreeOverlay(); // 0x10
		case 17: return Op_FindOverlay();
		//case 18: NULL used to be exec debug
		case 19: return Op_AddMessage();
		case 20: return Op_RemoveMessage();
		case 21: return Op_UserWait();
		case 22: return Op_FreezeCell();
		case 23: return Op_LoadCt();
		case 24: return Op_AddAnimation();
		case 25: return Op_RemoveAnimation();
		case 26: return Op_SetZoom();
		case 27: return Op_SetObjectAtNode();
		case 28: return Op_SetNodeState();
		case 29: return Op_SetNodeColor();
		case 30: return Op_TrackAnim();
		case 31: return Op_GetNodeX();

		case 32: return Op_GetNodeY(); // 0x20
		case 33: return Op_EndAnim();
		case 34: return Op_GetZoom();
		case 35: return Op_GetStep();
		case 36: return Op_SetStringColors();
		case 37: return Op_XClick();
		case 38: return Op_YClick();
		case 39: return Op_GetPixel();
		case 40: return Op_UserOn();
		case 41: return Op_FreeCT();
		case 42: return Op_FindObject();
		case 43: return Op_FindProc();
		case 44: return Op_WriteObject();
		case 45: return Op_ReadObject();
		case 46: return Op_RemoveOverlay();
		case 47: return Op_AddBackgroundIncrust();

		case 48: return Op_RemoveBackgroundIncrust(); // 0x30
		case 49: return Op_UnmergeBackgroundIncrust();
		case 50: return Op_freeBackgroundInscrustList();
		case 51: return Op_DialogOn();
		case 52: return Op_DialogOff();
		case 53: return Op_UserDelay();
		case 54: return Op_ThemeReset();
		case 55: return Op_Narrator();
		case 56: return Op_RemoveBackground();
		case 57: return Op_SetActiveBackground();
		case 58: return Op_CTOn();
		case 59: return Op_CTOff();
		case 60: return Op_Random();
		case 61: return Op_LoadSong();
		case 62: return Op_FadeSong();
		case 63: return Op_PlaySong();

		case 64: return Op_FreeSong(); // 0x40
		case 65: return Op_FrameExist();
		case 66: return Op_SetVolume();
		case 67: return Op_SongExist();
		case 68: return Op_TrackPos();
		case 69: return Op_StopSong();
		case 70: return Op_RestoreSong();
		case 71: return Op_SongSize();
		case 72: return Op_SetPattern();
		case 73: return Op_SongLoop();
		case 74: return Op_SongPlayed();
		case 75: return Op_LinkObjects();
		case 76: return Op_UserClick();
		case 77: return Op_XMenuItem();
		case 78: return Op_YMenuItem();
		case 79: return Op_Menu();

		case 80: return Op_AutoControl(); // 0x50
		case 81: return Op_MouseMove();
		case 82: return Op_MouseEnd();
		case 83: return Op_MsgExist();
		case 84: return Op_SetFont();
		// case 85: NULL MergeMsg
		case 86: return Op_Display();
		case 87: return Op_GetMouseX();
		case 88: return Op_GetMouseY();
		case 89: return Op_GetMouseButton();
		case 90: return Op_FindSet();
		case 91: return Op_regenerateBackgroundIncrust();
		case 92: return Op_BgName();
		case 93: return Op_LoopFX();
		case 94: return Op_StopFX();
		case 95: return Op_FreqFX();

		case 96: return Op_FreezeAni(); // 0x60
		case 97: return Op_FindMsg();
		case 98: return Op_FreezeParent();
		case 99: return Op_UnfreezeParent();
		case 100: return Op_Exec();
		case 101: return Op_AutoCell();
		case 102: return Op_Sizeof();
		case 103: return Op_Preload();
		case 104: return Op_FreePreload();
		// case 105: NULL DeletePreload
		case 106: return Op_VBL();
		case 107: return Op_LoadFrame();
		case 108: return Op_FreezeOverlay();
		case 109: return Op_Strcpy();
		case 110: return Op_Strcat();
		case 111: return Op_Itoa();

		case 112: return Op_comment(); // 0x70
		case 113: return Op_ComputeLine();
		case 114: return Op_FindSymbol();
		case 115: return Op_SetXDial();
		case 116: return Op_GetlowMemory();
		case 117: return Op_AniDir();
		case 118: return Op_Protect();
		case 119: return Op_ClearScreen();
		case 120: return Op_Inventory();
		case 121: return Op_UserMenu();
		case 122: return Op_GetRingWord();
		case 123: return Op_Sec();
		case 124: return Op_ProtectionFlag();
		case 125: return Op_KillMenu();
		default: error("Unsupported opcode %d ", opCode);
	}
}

bool ScriptInstance::isValidOperation(int opCode) {
	return ((opCode < 126) && // biggest vaid opCode
			(opCode != 0) &&	//
			(opCode != 15) &&	// non valid opCodes
			(opCode != 18) &&	//
			(opCode != 85) &&	//
			(opCode != 105));	//
}

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

	if (isValidOperation(opcode)) {
		stack.pushVar(operateFunction(opcode));
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

void ScriptInstance::sync(Common::Serializer &s) {
	uint32 dummyLong = 0;
	uint16 dummyWord = 0;

	s.syncAsUint16LE(dummyWord);
	s.syncAsSint16LE(_ccr);
	s.syncAsSint16LE(_scriptOffset);
	s.syncAsUint32LE(dummyLong);
	s.syncAsSint16LE(_dataSize);
	s.syncAsSint16LE(_scriptNumber);
	s.syncAsSint16LE(_overlayNumber);
	s.syncAsSint16LE(_sysKey);
	s.syncAsSint16LE(_freeze);
	s.syncAsSint16LE(_type);
	s.syncAsSint16LE(_var16);
	s.syncAsSint16LE(_var18);
	s.syncAsSint16LE(_var1A);

	s.syncAsSint16LE(_dataSize);

	if (_dataSize) {
		if (s.isLoading())
			_data = (byte *)mallocAndZero(_dataSize);
		s.syncBytes(_data, _dataSize);
	}
}

int ScriptInstance::execute() {
	if (!((_scriptNumber != -1) && (_freeze == 0) && (_sysKey != 0))) {
		if (_sysKey == 0) {
			_sysKey = 1;
		}
		return (0);
	}

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

	if (_sysKey == 0) {
		_sysKey = 1;
	}
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

void ScriptInstance::setFreeze(int16 oldFreeze, int16 newFreeze) {
	if ((_freeze == oldFreeze) || (oldFreeze == -1))
		_freeze = newFreeze;
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
			iter->remove();
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
		iter->remove();
		iter = erase(iter);
	}
}

void ScriptList::resetPtr2() {
	pCurrentScript = NULL;
	if (begin() != end())
		begin()->_scriptNumber = -1;
}

uint8 *ScriptList::add(int16 overlayNumber, int16 scriptNumber, int16 var1A, int16 var16, int16 var18, scriptTypeEnum scriptType) {
	int useArg3Neg = 0;
	ovlData3Struct *data3Ptr;
	int dataSize;

	//debug("Starting script %d of overlay %s", param,overlayTable[overlayNumber].overlayName);

	if (scriptType < 0) {
		useArg3Neg = 1;
		scriptType = (scriptTypeEnum) - scriptType;
	}

	if (scriptType == 20) {
		data3Ptr = getOvlData3Entry(overlayNumber, scriptNumber);
	} else {
		if (scriptType == 30) {
			data3Ptr = scriptFunc1Sub2(overlayNumber, scriptNumber);
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

	dataSize = data3Ptr->sysKey;

	ScriptInstance script(overlayNumber, scriptNumber, var1A, var16, var18, scriptType, dataSize, useArg3Neg);

	push_back(script);
	return (script.getData());
}

void ScriptList::add(ScriptInstance scriptToAdd) {
	push_back(scriptToAdd);
}

void ScriptList::manage() {
	Common::List<ScriptInstance>::iterator iter = begin();
	while (iter != end()) {
		if (!overlayTable[iter->_overlayNumber].executeScripts) {
			pCurrentScript = &(*iter);
			iter->execute();
			pCurrentScript = NULL;
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
				iter->setSysKey(param);
			}
		}
	}
}

void ScriptList::changeParam(int param1, int param2, int oldFreeze, int newValue) {
	Common::List<ScriptInstance>::iterator iter = begin();
	while (iter != end()) {
		if ((iter->_overlayNumber == param1) || (param1 == -1))
			if ((iter->_scriptNumber == param2) || (param2 == -1))
				iter->setFreeze(oldFreeze, newValue);
		iter++;
	}
}

} // End of namespace Cruise
