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

	ScriptInstance();
};

class ScriptList: public Common::List<ScriptInstance> {
	
};
void setupFuncArray();
int8 getByteFromScript();

int removeFinishedScripts(Common::List<ScriptInstance> *ptrHandle);
void removeAllScripts(Common::List<ScriptInstance> *ptrHandle);
int removeScript(int overlay, int idx, Common::List<ScriptInstance> *headPtr);
void resetPtr2(Common::List<ScriptInstance> * ptr);
uint8 *attacheNewScriptToTail(Common::List<ScriptInstance> *scriptHandlePtr, int16 overlayNumber, int16 param, int16 arg0, int16 arg1, int16 arg2, scriptTypeEnum scriptType);
void manageScripts(Common::List<ScriptInstance> * scriptHandle);

} // End of namespace Cruise

#endif
