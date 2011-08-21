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
	int16 _scriptNumber;
	int16 _overlayNumber;
private:
	int16 _ccr;
	int16 _scriptOffset;
	uint8 *_data;
	int16 _dataSize;
	int16 _sysKey;
	int16 _freeze;
	scriptTypeEnum _type;
	int16 _var16;
	int16 _var18;
	int16 _var1A;

	uint8 *scriptDataPtrTable[7];
public:
	ScriptInstance();
	ScriptInstance(int16 overlayNumber, int16 scriptNumber, int16 var1A, int16 var16, int16 var18, scriptTypeEnum scriptType, int dataSize, int useArg3Neg);
	void remove();

	void setFreeze(int16 oldFreeze, int16 newFreeze);
	void setSysKey(int16 syskey) {_sysKey = syskey;}
	uint8 *getData() {return _data;}
	int32 execute();
	void sync(Common::Serializer &s);
private:
	int8 getByte();
	short int getShort();

	int16 operateFunction(int opCode);
	bool isValidOperation(int opCode);

	//the functions
	int16 Op_FadeIn();
	int16 Op_FadeOut();
	int16 Op_LoadBackground();
	int16 Op_LoadAbs();
	int16 Op_AddCell();
	int16 Op_AddProc();
	int16 Op_InitializeState();
	int16 Op_RemoveCell();
	int16 Op_FreeCell();
	int16 Op_RemoveProc();
	int16 Op_RemoveFrame();
	int16 Op_LoadOverlay();
	int16 Op_SetColor();
	int16 Op_PlayFX();
	int16 Op_FreeOverlay();
	int16 Op_FindOverlay();
	int16 Op_AddMessage();
	int16 Op_RemoveMessage();
	int16 Op_UserWait();
	int16 Op_FreezeCell();
	int16 Op_LoadCt();
	int16 Op_AddAnimation();
	int16 Op_RemoveAnimation();
	int16 Op_SetZoom();
	int16 Op_SetObjectAtNode();
	int16 Op_SetNodeState();
	int16 Op_SetNodeColor();
	int16 Op_TrackAnim();
	int16 Op_GetNodeX();
	int16 Op_GetNodeY();
	int16 Op_EndAnim();
	int16 Op_GetZoom();
	int16 Op_GetStep();
	int16 Op_SetStringColors();
	int16 Op_XClick();
	int16 Op_YClick();
	int16 Op_GetPixel();
	int16 Op_UserOn();
	int16 Op_FreeCT();
	int16 Op_FindObject();
	int16 Op_FindProc();
	int16 Op_WriteObject();
	int16 Op_ReadObject();
	int16 Op_RemoveOverlay();
	int16 Op_AddBackgroundIncrust();
	int16 Op_RemoveBackgroundIncrust();
	int16 Op_UnmergeBackgroundIncrust();
	int16 Op_freeBackgroundInscrustList();
	int16 Op_DialogOn();
	int16 Op_DialogOff();
	int16 Op_UserDelay();
	int16 Op_ThemeReset();
	int16 Op_Narrator();
	int16 Op_RemoveBackground();
	int16 Op_SetActiveBackground();
	int16 Op_CTOn();
	int16 Op_CTOff();
	int16 Op_Random();
	int16 Op_LoadSong();
	int16 Op_FadeSong();
	int16 Op_PlaySong();
	int16 Op_FreeSong();
	int16 Op_FrameExist();
	int16 Op_SetVolume();
	int16 Op_SongExist();
	int16 Op_TrackPos();
	int16 Op_StopSong();
	int16 Op_RestoreSong();
	int16 Op_SongSize();
	int16 Op_SetPattern();
	int16 Op_SongLoop();
	int16 Op_SongPlayed();
	int16 Op_LinkObjects();
	int16 Op_UserClick();
	int16 Op_XMenuItem();
	int16 Op_YMenuItem();
	int16 Op_Menu();
	int16 Op_AutoControl();
	int16 Op_MouseMove();
	int16 Op_MouseEnd();
	int16 Op_MsgExist();
	int16 Op_SetFont();
	int16 Op_Display();
	int16 Op_GetMouseX();
	int16 Op_GetMouseY();
	int16 Op_GetMouseButton();
	int16 Op_FindSet();
	int16 Op_regenerateBackgroundIncrust();
	int16 Op_BgName();
	int16 Op_LoopFX();
	int16 Op_StopFX();
	int16 Op_FreqFX();
	int16 Op_FreezeAni();
	int16 Op_FindMsg();
	int16 Op_FreezeParent();
	int16 Op_UnfreezeParent();
	int16 Op_Exec();
	int16 Op_AutoCell();
	int16 Op_Sizeof();
	int16 Op_Preload();
	int16 Op_FreePreload();
	int16 Op_VBL();
	int16 Op_LoadFrame();
	int16 Op_FreezeOverlay();
	int16 Op_Strcpy();
	int16 Op_Strcat();
	int16 Op_Itoa();
	int16 Op_comment();
	int16 Op_ComputeLine();
	int16 Op_FindSymbol();
	int16 Op_SetXDial();
	int16 Op_GetlowMemory();
	int16 Op_AniDir();
	int16 Op_Protect();
	int16 Op_ClearScreen();
	int16 Op_Inventory();
	int16 Op_UserMenu();
	int16 Op_GetRingWord();
	int16 Op_Sec();
	int16 Op_ProtectionFlag();
	int16 Op_KillMenu();
	//end of the functions
	
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

	int32 executeScript(int16 Op_Code);
};

class ScriptList: private Common::List<ScriptInstance> {
private:
	static ScriptInstance *pCurrentScript;
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
	static ScriptInstance *getCurrentScript() {return pCurrentScript;}

};

} // End of namespace Cruise

#endif
