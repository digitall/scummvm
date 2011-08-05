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

#ifndef CRUISE_SCRIPT_H
#define CRUISE_SCRIPT_H

namespace Cruise {

enum scriptTypeEnum {
	scriptType_MinusPROC = -20,
	scriptType_Minus30 = -30,
	scriptType_PROC = 20,
	scriptType_REL = 30
};

class ScriptInstance {
public:
	int16 _ccr;
	int16 _scriptOffset;
	uint8 *_data;
	int16 _dataSize;
	int16 _scriptNumber;
	int16 _overlayNumber;
	int16 _sysKey;
	int16 _freeze;
	scriptTypeEnum _type;
	int16 _var16;
	int16 _var18;
	int16 _var1A;

	uint8 *scriptDataPtrTable[7];

	ScriptInstance();
	int32 execute();

private:

	int32 opcodeType0();
	int32 opcodeType1();
	int32 opcodeType2();
	int32 opcodeType3();
	int32 opcodeType4();
	int32 opcodeType5();
	int32 opcodeType6();
	int32 opcodeType7();
	int32 opcodeType8();
	int32 opcodeType9();
	int32 opcodeType10();
	int32 opcodeType11();

	int32 executeScript(int16 opCode);
};

class ScriptList: private Common::List<ScriptInstance> {
public:
	Common::List<ScriptInstance>::iterator begin();
	uint size();

	uint8 *add(int16 overlayNumber, int16 param, int16 arg0, int16 arg1, int16 arg2, scriptTypeEnum scriptType);
	void add(ScriptInstance scriptToAdd);
	
	int remove(int overlay, int idx);
	int removeFinished();
	void removeAll();
	void resetPtr2();
	void manage();
	void changeParam(int param1, int param2, int newValue, int param3);
	void scriptFunc2(int scriptNumber, int param, int param2);

};
int8 getByteFromScript();


} // End of namespace Cruise

#endif
