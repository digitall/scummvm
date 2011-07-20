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
	_type = scriptType_Invalid;
	_var16 = 0;
	_var18 = 0;
	_var1A = 0;
}

int removeFinishedScripts(Common::List<ScriptInstance> *ptrHandle) {
	Common::List<ScriptInstance>::iterator iter =  ptrHandle->begin();

	while (iter != ptrHandle->end()) {
		if (iter->_scriptNumber == -1) {
			if (iter->_data)
				MemFree(iter->_data);
			iter = ptrHandle->erase(iter);
		} else {
			iter++;
		}
	}

	return (0);
}

void removeAllScripts(Common::List<ScriptInstance> *ptrHandle) {
	Common::List<ScriptInstance>::iterator iter =  ptrHandle->begin();
	while (iter != ptrHandle->end()) {
		if (iter->_data)
			MemFree(iter->_data);
		ptrHandle->erase(iter++);
	}
}

void resetPtr2(Common::List<ScriptInstance> *ptr) {
	ptr->begin()->_scriptNumber = -1;
}

int8 getByteFromScript() {
	int8 var = *(int8 *)(currentData3DataPtr + currentScriptPtr->_scriptOffset);
	++currentScriptPtr->_scriptOffset;

	return (var);
}

short int getShortFromScript() {
	short int var = (int16)READ_BE_UINT16(currentData3DataPtr + currentScriptPtr->_scriptOffset);
	currentScriptPtr->_scriptOffset += 2;

	return (var);
}

// load opcode
int32 opcodeType0() {
	int index = 0;

	switch (currentScriptOpcodeType) {
	case 0: {
		stack.pushVar(getShortFromScript());
		return (0);
	}
	case 5:
		index = saveOpcodeVar;
	case 1: {
		uint8 *address = 0;
		int type = getByteFromScript();
		int ovl = getByteFromScript();
		short int offset = getShortFromScript();
		offset += index;

		int typ7 = type & 7;

		if (!typ7) {
			return (-10); // unresloved link
		}

		if (!ovl) {
			address = scriptDataPtrTable[typ7];
		} else	{ // TODO:
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
		int di = getByteFromScript();
		int si = getByteFromScript();
		int var_2 = getShortFromScript();

		if (!si) {
			si = currentScriptPtr->_overlayNumber;
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
int32 opcodeType1()	{
	int var = stack.popVar();
	int offset = 0;

	switch (currentScriptOpcodeType) {
	case 0: {
		return (0);	// strange, but happens also in original interpreter
	}
	case 5: {
		offset = saveOpcodeVar;
	}
	case 1: {
		int var_A = 0;

		int byte1 = getByteFromScript();
		int byte2 = getByteFromScript();

		int short1 = getShortFromScript();

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
			assert (ptr);
			*(ptr + var_A + offset) = var;
			return 0;
		}
		default:
			error("Unsupported code in opcodeType1 case 1");
		}

		break;
	}
	case 2: {
		int mode = getByteFromScript();
		int di = getByteFromScript();
		int var_4 = getShortFromScript();

		if (!di) {
			di = currentScriptPtr->_overlayNumber;
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

int32 opcodeType2() {
	int index = 0;
	switch (currentScriptOpcodeType) {
	case 5:
		index = saveOpcodeVar;
	case 1: {
		uint8* adresse = NULL;
		int type = getByteFromScript();
		int overlay = getByteFromScript();

		int offset = getShortFromScript();
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

int32 opcodeType10() {	// break
	return (0);
}

int32 opcodeType11() {	// break
	return (1);
}

int32 opcodeType4() {		// test
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

int32 opcodeType6() {
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

	currentScriptPtr->_ccr = si;

	return (0);
}

int32 opcodeType7() {
	int var1 = stack.popVar();
	int var2 = stack.popVar();

	stack.pushVar(var1);
	stack.pushVar(var2);

	return (0);
}

int32 opcodeType5() {
	int offset = currentScriptPtr->_scriptOffset;
	int short1 = getShortFromScript();
	int newSi = short1 + offset;
	int bitMask = currentScriptPtr->_ccr;

	switch (currentScriptOpcodeType) {
	case 0: {
		if (!(bitMask & 1)) {
			currentScriptPtr->_scriptOffset = newSi;
		}
		break;
	}
	case 1: {
		if (bitMask & 1) {
			currentScriptPtr->_scriptOffset = newSi;
		}
		break;
	}
	case 2: {
		if (bitMask & 2) {
			currentScriptPtr->_scriptOffset = newSi;
		}
		break;
	}
	case 3: {
		if (bitMask & 3) {
			currentScriptPtr->_scriptOffset = newSi;
		}
		break;
	}
	case 4: {
		if (bitMask & 4) {
			currentScriptPtr->_scriptOffset = newSi;
		}
		break;
	}
	case 5: {
		if (bitMask & 5) {
			currentScriptPtr->_scriptOffset = newSi;
		}
		break;
	}
	case 6: {
		break;	// never
	}
	case 7: {
		currentScriptPtr->_scriptOffset = newSi;	//always
		break;
	}
	}

	return (0);
}

int32 opcodeType3()	{	// math
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

int32 opcodeType9() {		// stop script
	//debug("Stop a script of overlay %s", overlayTable[currentScriptPtr->_overlayNumber].overlayName);
	currentScriptPtr->_scriptNumber = -1;
	return (1);
}

void setupFuncArray() {
	int i;

	for (i = 0; i < 64; i++) {
		opcodeTypeTable[i] = NULL;
	}

	opcodeTypeTable[1] = opcodeType0;
	opcodeTypeTable[2] = opcodeType1;
	opcodeTypeTable[3] = opcodeType2;
	opcodeTypeTable[4] = opcodeType3;
	opcodeTypeTable[5] = opcodeType4;
	opcodeTypeTable[6] = opcodeType5;
	opcodeTypeTable[7] = opcodeType6;
	opcodeTypeTable[8] = opcodeType7;
	opcodeTypeTable[9] = opcodeType8;
	opcodeTypeTable[10] = opcodeType9;
	opcodeTypeTable[11] = opcodeType10;
	opcodeTypeTable[12] = opcodeType11;
}

int ScriptList::remove(int overlay, int idx) {
	Common::List<ScriptInstance>::iterator iter = this->begin();

		 while (iter != this->end()) {
			if (iter->_overlayNumber == overlay
			        && (iter->_scriptNumber == idx || idx == -1)) {
				iter->_scriptNumber = -1;
			}

			iter++;
		}

	return (0);
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

	if (scriptType == 20) {	// Obj or not ?
		tempScript._sysKey = useArg3Neg;
	} else {
		tempScript._sysKey = 1;
	}

	tempScript._freeze = 0;
	tempScript._type = scriptType;
	tempScript._var18 = arg2;
	tempScript._var16 = arg1;
	tempScript._var1A = arg0;

	this->push_back(tempScript);
	return (tempScript._data);
}

int executeScripts(ScriptInstance *ptr) {
	int numScript2;
	ovlData3Struct *ptr2;
	ovlDataStruct *ovlData;
	uint8 opcodeType;

	numScript2 = ptr->_scriptNumber;

	if (ptr->_type == 20) {
		ptr2 = getOvlData3Entry(ptr->_overlayNumber, numScript2);

		if (!ptr2) {
			return (-4);
		}
	} else {
		if (ptr->_type == 30) {
			ptr2 = scriptFunc1Sub2(ptr->_overlayNumber, numScript2);

			if (!ptr2) {
				return (-4);
			}
		} else {
			return (-6);
		}
	}

	if (!overlayTable[ptr->_overlayNumber].alreadyLoaded) {
		return (-7);
	}

	ovlData = overlayTable[ptr->_overlayNumber].ovlData;

	if (!ovlData)
		return (-4);

	currentData3DataPtr = ptr2->dataPtr;

	scriptDataPtrTable[1] = (uint8 *) ptr->_data;
	scriptDataPtrTable[2] = getDataFromData3(ptr2, 1);
	scriptDataPtrTable[5] = ovlData->data4Ptr;	// free strings
	scriptDataPtrTable[6] = ovlData->ptr8;

	currentScriptPtr = ptr;

	stack.reset();

	do {
#ifdef SKIP_INTRO
		if (currentScriptPtr->_scriptOffset == 290
		        && currentScriptPtr->_overlayNumber == 4
		        && currentScriptPtr->_scriptNumber == 0) {
			currentScriptPtr->_scriptOffset = 923;
		}
#endif
		opcodeType = getByteFromScript();

		debugC(5, kCruiseDebugScript, "Script %s/%d ip=%d opcode=%d",
			overlayTable[currentScriptPtr->_overlayNumber].overlayName,
			currentScriptPtr->_scriptNumber,
			currentScriptPtr->_scriptOffset,
			(opcodeType & 0xFB) >> 3);

		currentScriptOpcodeType = opcodeType & 7;

		if (!opcodeTypeTable[(opcodeType & 0xFB) >> 3]) {
			error("Unsupported opcode type %d", (opcodeType & 0xFB) >> 3);
		}
	} while (!opcodeTypeTable[(opcodeType & 0xFB) >> 3]());

	currentScriptPtr = NULL;

	return (0);
}

void manageScripts(Common::List<ScriptInstance> *scriptHandle) {
	Common::List<ScriptInstance>::iterator iter = scriptHandle->begin();
	while (iter != scriptHandle->end()) {
		if (!overlayTable[iter->_overlayNumber].executeScripts) {
			if ( (iter->_scriptNumber != -1) && (iter->_freeze == 0) && (iter->_sysKey != 0)) {
				executeScripts(&(*iter));
			}

			if (iter->_sysKey == 0) {
				iter->_sysKey = 1;
			}
		}

		iter++;

	}

}

} // End of namespace Cruise
