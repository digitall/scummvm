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

typedef int16(*opcodeFunction)();

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

	int8 getByte();
	short int getShort();
	int32 execute();

private:
	static opcodeFunction opcodeTablePtr[];

	//the functions
	static int16 Op_FadeIn();
	static int16 Op_FadeOut();
	static int16 Op_LoadBackground();
	static int16 Op_LoadAbs();
	static int16 Op_AddCell();
	static int16 Op_AddProc();
	static int16 Op_InitializeState();
	static int16 Op_RemoveCell();
	static int16 Op_FreeCell();
	static int16 Op_RemoveProc();
	static int16 Op_RemoveFrame();
	static int16 Op_LoadOverlay();
	static int16 Op_SetColor();
	static int16 Op_PlayFX();
	static int16 Op_FreeOverlay();
	static int16 Op_FindOverlay();
	static int16 Op_AddMessage();
	static int16 Op_RemoveMessage();
	static int16 Op_UserWait();
	static int16 Op_FreezeCell();
	static int16 Op_LoadCt();
	static int16 Op_AddAnimation();
	static int16 Op_RemoveAnimation();
	static int16 Op_SetZoom();
	static int16 Op_SetObjectAtNode();
	static int16 Op_SetNodeState();
	static int16 Op_SetNodeColor();
	static int16 Op_TrackAnim();
	static int16 Op_GetNodeX();
	static int16 Op_GetNodeY();
	static int16 Op_EndAnim();
	static int16 Op_GetZoom();
	static int16 Op_GetStep();
	static int16 Op_SetStringColors();
	static int16 Op_XClick();
	static int16 Op_YClick();
	static int16 Op_GetPixel();
	static int16 Op_UserOn();
	static int16 Op_FreeCT();
	static int16 Op_FindObject();
	static int16 Op_FindProc();
	static int16 Op_WriteObject();
	static int16 Op_ReadObject();
	static int16 Op_RemoveOverlay();
	static int16 Op_AddBackgroundIncrust();
	static int16 Op_RemoveBackgroundIncrust();
	static int16 Op_UnmergeBackgroundIncrust();
	static int16 Op_freeBackgroundInscrustList();
	static int16 Op_DialogOn();
	static int16 Op_DialogOff();
	static int16 Op_UserDelay();
	static int16 Op_ThemeReset();
	static int16 Op_Narrator();
	static int16 Op_RemoveBackground();
	static int16 Op_SetActiveBackground();
	static int16 Op_CTOn();
	static int16 Op_CTOff();
	static int16 Op_Random();
	static int16 Op_LoadSong();
	static int16 Op_FadeSong();
	static int16 Op_PlaySong();
	static int16 Op_FreeSong();
	static int16 Op_FrameExist();
	static int16 Op_SetVolume();
	static int16 Op_SongExist();
	static int16 Op_TrackPos();
	static int16 Op_StopSong();
	static int16 Op_RestoreSong();
	static int16 Op_SongSize();
	static int16 Op_SetPattern();
	static int16 Op_SongLoop();
	static int16 Op_SongPlayed();
	static int16 Op_LinkObjects();
	static int16 Op_UserClick();
	static int16 Op_XMenuItem();
	static int16 Op_YMenuItem();
	static int16 Op_Menu();
	static int16 Op_AutoControl();
	static int16 Op_MouseMove();
	static int16 Op_MouseEnd();
	static int16 Op_MsgExist();
	static int16 Op_SetFont();
	static int16 Op_Display();
	static int16 Op_GetMouseX();
	static int16 Op_GetMouseY();
	static int16 Op_GetMouseButton();
	static int16 Op_FindSet();
	static int16 Op_regenerateBackgroundIncrust();
	static int16 Op_BgName();
	static int16 Op_LoopFX();
	static int16 Op_StopFX();
	static int16 Op_FreqFX();
	static int16 Op_FreezeAni();
	static int16 Op_FindMsg();
	static int16 Op_FreezeParent();
	static int16 Op_UnfreezeParent();
	static int16 Op_Exec();
	static int16 Op_AutoCell();
	static int16 Op_Sizeof();
	static int16 Op_Preload();
	static int16 Op_FreePreload();
	static int16 Op_VBL();
	static int16 Op_LoadFrame();
	static int16 Op_FreezeOverlay();
	static int16 Op_Strcpy();
	static int16 Op_Strcat();
	static int16 Op_Itoa();
	static int16 Op_comment();
	static int16 Op_ComputeLine();
	static int16 Op_FindSymbol();
	static int16 Op_SetXDial();
	static int16 Op_GetlowMemory();
	static int16 Op_AniDir();
	static int16 Op_Protect();
	static int16 Op_ClearScreen();
	static int16 Op_Inventory();
	static int16 Op_UserMenu();
	static int16 Op_GetRingWord();
	static int16 Op_Sec();
	static int16 Op_ProtectionFlag();
	static int16 Op_KillMenu();
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

} // End of namespace Cruise

#endif
