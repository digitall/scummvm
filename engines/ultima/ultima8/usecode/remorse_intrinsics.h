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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef ULTIMA8_USECODE_REMORSEINTRINSICS_H
#define ULTIMA8_USECODE_REMORSEINTRINSICS_H

#include "ultima/ultima8/usecode/intrinsics.h"
#include "ultima/ultima8/world/item.h"

namespace Ultima {
namespace Ultima8 {

// Crusader: No Remorse Intrinsics
// Unknown function signatures were generate by the usecode disassembly
// and looking at handling of SP and retval after function.
// Most up-to-date version of unknown functions is in convert_usecode_crusader.h
Intrinsic RemorseIntrinsics[] = {
	// 0x000
	World::I_getAlertActive,
	Item::I_getFrame, // int Intrinsic001(Item *)
	Item::I_setFrame, //
	Item::I_getMapArray, // See TRIGGER::ordinal21 - stored in a variable 'mapNum'
	Item::I_getStatus,
	Item::I_orStatus,
	Item::I_equip, // void Intrinsic006(6 bytes)
	Item::I_isPartlyOnScreen, //
	Actor::I_isNPC, // byte Intrinsic008(Item *)
	Item::I_getZ, // byte Intrinsic009(Item *)
	Item::I_destroy, // void Intrinsic00A(Item *)
	Actor::I_getUnkByte, // get something about npcdata - struct byte 0x63 (99)
	Ultima8Engine::I_setAvatarInStasis, // void Intrinsic00C(2 bytes)
	Item::I_getDirToItem, // byte Intrinsic00D(6 bytes)
	Actor::I_turnToward,
	MovieGump::I_playMovieCutsceneAlt,
	// 0x010
	Item::I_getQLo, // Based on having same coff as 02B
	Actor::I_getMap, // int Intrinsic011(4 bytes)
	MusicProcess::I_playMusic, // void Intrinsic012(2 bytes)
	Item::I_getX, //int Intrinsic013(4 bytes)
	Item::I_getY, //int Intrinsic014(4 bytes)
	AudioProcess::I_playSFXCru,
	Item::I_getShape,
	Item::I_explode, // void Intrinsic017(8 bytes)
	UCMachine::I_rndRange, // int16 Intrinsic018(4 bytes)
	Item::I_legalCreateAtCoords, // byte Intrinsic019(14 bytes)
	Item::I_andStatus, // void Intrinsic01A(6 bytes)
	World::I_getControlledNPCNum,  // int16 Intrinsic01B(void)
	Actor::I_getDir, // byte Intrinsic01C(4 bytes)
	Actor::I_getLastAnimSet, // int Intrinsic01D(4 bytes)
	Item::I_fireWeapon, // int Intrinsic01E(16 bytes)
	Item::I_create,
	// 0x020
	Item::I_popToCoords, // void Intrinsic020(10 bytes)
	Actor::I_setDead, // void Intrinsic021(4 bytes)
	Item::I_push,
	Item::I_getEtherealTop, // int Intrinsic023(void)
	Item::I_setShape, // Probably, see PEPSIEW::gotHit
	Item::I_touch,
	Item::I_getQHi, // int16 Intrinsic026(Item *)
	Item::I_getClosestDirectionInRange, // int Intrinsic027(14 bytes)
	Item::I_hurl, // int Intrinsic028(12 bytes)
	World::I_gameDifficulty,
	AudioProcess::I_playAmbientSFXCru,
	Item::I_getQLo, // int16 Intrinsic02B(4 bytes)
	Item::I_inFastArea, // byte Intrinsic02C(4 bytes) // based on disassembly - checks for flag 0x2000
	Item::I_setQHi,
	Item::I_legalMoveToPoint, // byte Intrinsic02E(12 bytes)
	CurrentMap::I_canExistAtPoint, // byte Intrinsic02F(10 bytes)
	// 0x030
	Item::I_pop,
	Item::I_andStatus,
	Item::I_receiveHit, // void Intrinsic032(12 bytes)
	Actor::I_isBusy, // int Intrinsic033(4 bytes)
	Item::I_getDirFromTo16,
	Actor::I_isKneeling,
	Actor::I_doAnim, // void Intrinsic036(12 bytes)
	MainActor::I_addItemCru, // int Intrinsic037(4 bytes)
	AudioProcess::I_stopSFXCru, // takes Item *, sndno (from disasm)
	Actor::I_isDead, // int Intrinsic039(4 bytes)
	AudioProcess::I_isSFXPlayingForObject,
	Item::I_setQLo,
	Item::I_getFamily,
	Container::I_destroyContents,
	Item::I_fall,
	Egg::I_getEggId, // void Intrinsic03F(4 bytes)
	// 0x040
	CameraProcess::I_moveTo, // void Intrinsic040(8 bytes)
	CameraProcess::I_setCenterOn, // void Intrinsic041(2 bytes)
	Item::I_getRangeIfVisible, // int Intrinsic042(6 bytes)
	AudioProcess::I_playSFXCru, // TODO: Work out how this is different from Int015 - to a first approximation they are quite similar.
	Item::I_isOn,
	Item::I_getQHi,  // based on same coff set as 026
	Item::I_isOn,
	Item::I_getQHi,  // based on same coff set as 026
	Item::I_isOn,
	Item::I_getQHi,  // based on same coff set as 026
	Item::I_isOn,
	Item::I_getQHi,  // based on same coff set as 026
	Item::I_isOn,
	Item::I_getQHi,  // based on same coff set as 026
	Item::I_isOn,
	Item::I_getQHi,  // based on same coff set as 026
	// 0x050
	Actor::I_getCurrentActivityNo, // void Intrinsic050(4 bytes)
	Actor::I_clrInCombat, // void Intrinsic051(4 bytes)
	Actor::I_setDefaultActivity0, // void Intrinsic052(6 bytes)
	Actor::I_setDefaultActivity1, // void Intrinsic053(6 bytes)
	Actor::I_setDefaultActivity2, // void Intrinsic054(6 bytes)
	Actor::I_setActivity, // void Intrinsic055(6 bytes)
	World::I_setControlledNPCNum, // void Intrinsic056(2 bytes)
	Item::I_getSurfaceWeight, // void Intrinsic057(4 bytes)
	Item::I_isCentreOn, // int Intrinsic058(6 bytes)
	Item::I_setFrame, // based on same coff as 002
	Actor::I_getLastAnimSet, // void Intrinsic05A(4 bytes)
	Item::I_legalCreateAtPoint, // probably. see PEPSIEW::use
	Item::I_getPoint, // void Intrinsic05C(8 bytes)
	CruStatusGump::I_hideStatusGump, // void Intrinsic05D(void)
	MovieGump::I_playMovieOverlay, // int16 Intrinsic05E(uint32, char *, int16 a, int16 b)
	CruStatusGump::I_showStatusGump, // void Intrinsic05F(void)
	// 0x060
	Actor::I_setDead, // void Intrinsic060(4 bytes)
	Actor::I_createActor, // void Intrinsic061(8 bytes)
	0, // void Intrinsic062(void)
	Actor::I_teleport, // void Intrinsic063(12 bytes)
	Item::I_getFootpadData, // void Intrinsic064(16 bytes)
	Item::I_isInNpc,
	Item::I_getQLo, // based on same coff set as 02B
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_setNpcNum, // void Item::I_setSomething068(Item *, int16 something) , see VALUEBOX:ordinal20
	Item::I_andStatus, // void Intrinsic069(6 bytes)
	Item::I_move, // void Intrinsic06A(10 bytes)
	UCMachine::I_true, // Note: This is actualy "is compiled with VIOLENCE=1" (was set to 0 in germany). For now just always say yes.
	Kernel::I_resetRef, // void Intrinsic06C(4 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_andStatus, // void Intrinsic06E(6 bytes)
	Item::I_isCompletelyOn,
	// 0x070
	Ultima8Engine::I_getUnkCrusaderFlag, // int Intrinsic070(void)
	Ultima8Engine::I_setUnkCrusaderFlag, // void Intrinsic071(void)
	Ultima8Engine::I_setCruStasis,
	Actor::I_setDead,
	Ultima8Engine::I_clrUnkCrusaderFlag, // void Intrinsic074(void)
	Ultima8Engine::I_clrCruStasis,
	AudioProcess::I_stopSFXCru, // takes Item *, from disasm
	PaletteFaderProcess::I_fadeToBlack, // void Intrinsic077(void)
	MainActor::I_clrKeycards, // void Intrinsic078(void)
	MainActor::I_teleportToEgg, // different than U8's? void Intrinsic079(6 bytes)
	PaletteFaderProcess::I_fadeToGamePal, // void Intrinsic07A(void)
	Actor::I_clrImmortal, // based on disasm
	Actor::I_getHp, // void Intrinsic07C(4 bytes)
	Actor::I_setActivity, // void Intrinsic07D(6 bytes)
	Item::I_getQuality,
	Item::I_setQuality,
	// 0x080
	Item::I_use, // void Intrinsic080(4 bytes)
	MainActor::I_getMaxEnergy, // void Intrinsic081(4 bytes)
	Actor::I_getMana, // Actually energy, but map to mana as we don't need that in Crusader.
	Actor::I_setMana, //
	Item::I_getQLo, // based on same coff set as 02B
	Actor::I_setImmortal, // void Intrinsic085(4 bytes)
	CameraProcess::I_getCameraX, // void Intrinsic086(void)
	CameraProcess::I_getCameraY, // void Intrinsic087(void)
	Item::I_setMapArray,
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_shoot, // void Intrinsic08A(12 bytes)
	Item::I_enterFastArea, // void Intrinsic08B(4 bytes)
	Item::I_setBroken, // void Intrinsic08C(4 bytes)
	Item::I_hurl, // void Intrinsic08D(12 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	PaletteFaderProcess::I_jumpToAllBlack, // TODO: should also resume cycle process.
	// 0x090
	MusicProcess::I_stopMusic, // void Intrinsic090(void)
	0, // pause cycler - not needed as our cycler does not run when paused
	MovieGump::I_playMovieCutsceneAlt, // TODO: not exactly the same, Alt includes a fade.
	0, // resume cycler - not needed as our cycler does not run when paused
	Game::I_playCredits, // TODO: Implement this
	Ultima8Engine::I_moveKeyDownRecently,
	MainActor::I_teleportToEgg, // void Intrinsic096(4 bytes)
	PaletteFaderProcess::I_jumpToGreyScale,
	World::I_resetVargasShield, // void Intrinsic098(void)
	Item::I_andStatus, // void Intrinsic099(6 bytes)
	PaletteFaderProcess::I_jumpToNormalPalette, // TODO: should also stop cycle process?
	PaletteFaderProcess::I_fadeToGamePal, // fade to game pal with number of steps
	PaletteFaderProcess::I_fadeToGamePal, // fade from black with nsteps and another unknown param
	PaletteFaderProcess::I_fadeToBlack, // fade to black with number of steps
	PaletteFaderProcess::I_fadeToBlack, // fade to black with nsteps and another unknown param
	PaletteFaderProcess::I_fadeToGivenColor,
	// 0x0A0
	Actor::I_setDead,
	Item::I_getQLo, // based on same coff set as 02B
	Item::I_getUnkEggType,
	Egg::I_setEggXRange, // void Intrinsic0A3(6 bytes)
	Item::I_overlaps,
	Item::I_isOn,
	UCMachine::I_true, // I_getAnimationsEnabled -> default to true (animations enabled)
	Egg::I_getEggXRange, // void Intrinsic0A7(4 bytes)
	Actor::I_setDead,
	MovieGump::I_playMovieCutsceneAlt, // TODO: not exactly the same, Alt includes a fade.
	AudioProcess::I_playSFX, // void Intrinsic0AA(2 bytes)
	Actor::I_isFalling, // int Actor::I_getFlag0x59Field1  Intrinsic0AB(4 bytes)
	Item::I_getFamilyOfType, // void Intrinsic0AC(2 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_getQLo, // based on same coff set as 02B
	Item::I_getQHi,  // based on same coff set as 026
	// 0x0B0
	Item::I_unequip, // void Intrinsic0B0(6 bytes)
	Item::I_avatarStoleSomething, // void Intrinsic0B1(6 bytes)
	Item::I_andStatus, // void Intrinsic0B2(6 bytes)
	Ultima8Engine::I_getCurrentTimerTick, // int32 Intrinsic0B3(void), probably, see FREE::ordinal32
	World::I_setAlertActive, // void Intrinsic0B4(void)
	Item::I_equip, // void Intrinsic0B5(6 bytes)
	World::I_clrAlertActive, // void Intrinsic0B6(void)
	Ultima8Engine::I_getAvatarInStasis, // void Intrinsic0B7(void)
	MainActor::I_addItemCru, // int Intrinsic0B8(4 bytes)
	Actor::I_getLastAnimSet, // void Intrinsic0B9(4 bytes)
	Item::I_setQuality,
	CurrentMap::I_canExistAt, // int Intrinsic0BB(8 bytes)
	Item::I_isOn,
	Item::I_hurl, // void Intrinsic0BD(12 bytes)
	Item::I_getQHi,  // based on same coff set as 026
	Item::I_andStatus, // void Intrinsic0BF(6 bytes)
	// 0x0C0
	Item::I_hurl, // void Intrinsic0C0(12 bytes)
	Item::I_andStatus, // void Intrinsic0C1(6 bytes)
	Item::I_hurl, // void Intrinsic0C2(12 bytes)
	Item::I_andStatus, // void Intrinsic0C3(6 bytes)
	KeypadGump::I_showKeypad, // void Intrinsic0C4(2 bytes)
	Item::I_isOn,
	SpriteProcess::I_createSprite, // void Intrinsic0C6(14 bytes)
	Item::I_getDirFromItem, // int Intrinsic0C7(6 bytes)
	Item::I_hurl, // void Intrinsic0C8(12 bytes)
	Item::I_getQHi,  // based on same coff set as 026
	Actor::I_addHp, // int Intrinsic0CA(6 bytes)
	MainActor::I_switchMap, // 0CB
	Actor::I_isInCombat, // int Intrinsic0CC(4 bytes)
	Actor::I_setActivity, // void Intrinsic0CD(6 bytes)
	UCMachine::I_true, // whether the string "GAME COMPILE=1" has the 1.  Might be interesting to see how this changes the game.. for now just set to true.
	Item::I_setQAndCombine, // void Intrinsic0CF(6 bytes)
	// 0x0D0
	Item::I_use, // void Intrinsic0D0(4 bytes)
	AudioProcess::I_stopAllSFX,
	MovieGump::I_playMovieCutscene, // void I_playFlic(int *item,char *flicname,word sizex,word sizey) // play flic
	0, // void Intrinsic0D3(void)
	AudioProcess::I_playSFX, // void Intrinsic0D4(2 bytes)
	Item::I_use, // void Intrinsic0D5(4 bytes)
	CameraProcess::I_getCameraZ, // int Intrinsic0D6(void)
	Actor::I_getLastAnimSet, // void Intrinsic0D7(4 bytes)
	Actor::I_setDead,
	Item::I_getQLo, // based on same coff set as 02B
	PaletteFaderProcess::I_jumpToAllWhite,
	Actor::I_setActivity, // void Intrinsic0DB(6 bytes)
	Item::I_isOn,
	Actor::I_getLastActivityNo, // void Intrinsic0DD(4 bytes)
	Actor::I_setCombatTactic, // void Intrinsic0DE(6 bytes)
	Actor::I_getEquip, // void Intrinsic0DF(6 bytes)
	// 0x0E0
	Actor::I_setEquip, // void Intrinsic0E0(8 bytes)
	Actor::I_getDefaultActivity0, // void Intrinsic0E1(4 bytes)
	Actor::I_getDefaultActivity1, // void Intrinsic0E2(4 bytes)
	Actor::I_getDefaultActivity2, // void Intrinsic0E3(4 bytes)
	Actor::I_getLastAnimSet, // void Intrinsic0E4(4 bytes)
	Actor::I_setTarget,
	Actor::I_setUnkByte, // void Intrinsic0E6(6 bytes)
	Actor::I_setDead,
	Item::I_cast, // void Intrinsic0E8(6 bytes)
	Item::I_andStatus, // void Intrinsic0E9(6 bytes)
	Item::I_getQLo, // based on same coff set as 02B
	MainActor::I_getNumberOfCredits, // void Intrinsic0EB(void)
	Item::I_popToEnd,
	Item::I_popToContainer,
	BatteryChargerProcess::I_create,
	Kernel::I_getNumProcesses, // void Intrinsic0EF(4 bytes)
	// 0x0F0
	Item::I_getQHi,  // based on same coff set as 026
	Item::I_isOn,
	Actor::I_setActivity, // void Intrinsic0F2(6 bytes)
	Item::I_getQHi,  // based on same coff set as 026
	Item::I_getQ, // void Intrinsic0F4(4 bytes)
	Item::I_setQ, // void Intrinsic0F5(6 bytes)
	CruHealerProcess::I_create,
	Item::I_hurl, // void Intrinsic0F7(12 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_hurl, // void Intrinsic0F9(12 bytes)
	Item::I_isOn,
	Item::I_getQHi,  // based on same coff set as 026
	Item::I_andStatus, // void Intrinsic0FC(6 bytes)
	MainActor::I_hasKeycard, // int Intrinsic0FD(2 bytes)
	ComputerGump::I_readComputer, // void Intrinsic0FE(4 bytes)
	UCMachine::I_numToStr, // same as 113 based on same coff set 0FF, 113, 126
	// 0x100
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_andStatus, // void Intrinsic101(6 bytes)
	Item::I_getNpcNum, // Based on variable name in TRIGGER::ordinal21
	Item::I_isCrusTypeNPC,
	Item::I_andStatus, // void Intrinsic104(6 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_andStatus, // void Intrinsic106(6 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_andStatus, // void Intrinsic108(6 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_andStatus, // void Intrinsic10A(6 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_andStatus, // void Intrinsic10C(6 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_andStatus, // void Intrinsic10E(6 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	// 0x110
	Item::I_andStatus, // void Intrinsic110(6 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Actor::I_getDir, // byte Intrinsic112(4 bytes)
	UCMachine::I_numToStr, // see VMAIL::func0A for example usage
	Item::I_andStatus, // void Intrinsic114(6 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_fireDistance, // byte Intrinsic116(14 bytes)
	Item::I_andStatus, // void Intrinsic117(6 bytes)
	Item::I_hurl, // int16 Intrinsic118(12 bytes)
	Item::I_setBroken, // void Intrinsic119(4 bytes)
	Item::I_andStatus, // void Intrinsic11A(6 bytes)
	Item::I_getTypeFlag, // byte Intrinsic11B(6 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_hurl, // int16 Intrinsic11D(12 bytes)
	Item::I_getCY, // int16 Intrinsic11E(4 bytes)
	Item::I_getCZ, // byte Intrinsic11F(4 bytes)
	// 0x120
	Item::I_getCX, // void Intrinsic120(4 bytes)
	Actor::I_getDir, // int Intrinsic121(4 bytes)
	Actor::I_isDead, // int Intrinsic122(4 bytes)
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Actor::I_getLastAnimSet, // void Intrinsic124(4 bytes)
	Item::I_setQuality,
	UCMachine::I_numToStr, // same as 113 based on same coff set 0FF, 113, 126
	Item::I_getDirToCoords, // int Intrinsic127(8 bytes)
	Item::I_andStatus, // void Intrinsic128(6 bytes) // maybe Item::andStatus?? see ITEM::ordinal22
	Item::I_getNpcNum, // based on same coff as 102 (-> variable name in TRIGGER::ordinal21)
	Item::I_setBroken, // void Intrinsic12A(4 bytes)
	Item::I_getCY, // void Intrinsic12B(4 bytes)
	Item::I_isOn,
	Item::I_getFootpadData, // void Intrinsic12D(16 bytes)
	Actor::I_isDead, // int Intrinsic12E(4 bytes)
	Actor::I_createActorCru, // void Intrinsic12F(6 bytes)
	// 0x130
	Actor::I_clrImmortal, // void Intrinsic130(4 bytes)
	Actor::I_setActivity, // void Intrinsic131(6 bytes)
	Item::I_andStatus, // void Intrinsic132(6 bytes)
	Item::I_getQHi,  // based on same coff set as 026
	WeaselGump::I_showWeaselGump, // void Intrinsic134(2 bytes)
	Actor::I_setDead,
	0, // void UNUSEDInt136()
	0  // void UNUSEDInt137()
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Intrinsics for the Remorse demo.
Intrinsic RemorseDemoIntrinsics[] = {
	World::I_getAlertActive,
	Item::I_getFrame,
	Item::I_setFrame,
	Item::I_getMapArray,
	Item::I_getStatus,
	Item::I_orStatus,
	Item::I_equip,
	Item::I_isPartlyOnScreen,
	Actor::I_isNPC,
	Item::I_getZ,
	Item::I_destroy,
	Actor::I_getUnkByte,
	Ultima8Engine::I_setAvatarInStasis,
	Item::I_getDirToItem,
	Actor::I_turnToward,
	MovieGump::I_playMovieCutsceneAlt,
	// 0x10
	Item::I_getQLo,
	Actor::I_getMap,
	MusicProcess::I_playMusic,
	Item::I_getX,
	Item::I_getY,
	AudioProcess::I_playSFXCru,
	Item::I_getShape,
	Item::I_explode,
	UCMachine::I_rndRange,
	Item::I_legalCreateAtCoords,
	Item::I_andStatus,
	World::I_getControlledNPCNum,
	Actor::I_getDir,
	Actor::I_getLastAnimSet,
	Item::I_fireWeapon,
	Item::I_create,
	// 0x20
	Item::I_popToCoords,
	Actor::I_setDead,
	Item::I_push,
	Item::I_getEtherealTop,
	Item::I_setShape,
	Item::I_touch,
	Item::I_getQHi,
	Item::I_getClosestDirectionInRange,
	Item::I_hurl,
	World::I_gameDifficulty,
	AudioProcess::I_playAmbientSFXCru,
	Item::I_isCompletelyOn,
	UCMachine::I_true,
	Container::I_destroyContents,
	Item::I_getQLo,
	Item::I_inFastArea,
	// 0x30
	Item::I_setQHi,
	Item::I_legalMoveToPoint,
	CurrentMap::I_canExistAtPoint,
	Item::I_pop,
	Item::I_andStatus,
	Item::I_receiveHit,
	Actor::I_isBusy,
	Item::I_getDirFromTo16,
	Actor::I_isKneeling,
	Actor::I_doAnim,
	MainActor::I_addItemCru,
	AudioProcess::I_stopSFXCru,
	Actor::I_isDead,
	AudioProcess::I_isSFXPlayingForObject,
	Item::I_setQLo,
	Item::I_getFamily,
	// 0x40
	Item::I_fall,
	Egg::I_getEggId,
	CameraProcess::I_moveTo,
	CameraProcess::I_setCenterOn,
	Item::I_getRangeIfVisible,
	AudioProcess::I_playSFXCru,
	Item::I_isOn,
	Item::I_getQHi,
	Item::I_isOn,
	Item::I_getQHi,
	Item::I_isOn,
	Item::I_getQHi,
	Item::I_isOn,
	Item::I_getQHi,
	Item::I_isOn,
	Item::I_getQHi,
	// 0x50
	Item::I_isOn,
	Item::I_getQHi,
	Actor::I_getCurrentActivityNo,
	Actor::I_clrInCombat,
	Actor::I_setDefaultActivity0,
	Actor::I_setDefaultActivity1,
	Actor::I_setDefaultActivity2,
	Actor::I_setActivity,
	World::I_setControlledNPCNum,
	Item::I_getSurfaceWeight,
	Item::I_isCentreOn,
	Item::I_setFrame,
	Actor::I_getLastAnimSet,
	Item::I_legalCreateAtPoint,
	Item::I_getPoint,
	CruStatusGump::I_hideStatusGump,
	// 0x60
	MovieGump::I_playMovieOverlay,
	CruStatusGump::I_showStatusGump,
	Actor::I_setDead,
	Actor::I_createActor,
	0,
	Actor::I_teleport,
	Item::I_getFootpadData,
	Item::I_isInNpc,
	Item::I_getQLo,
	Item::I_getNpcNum,
	Item::I_setNpcNum,
	Item::I_andStatus,
	Item::I_move,
	UCMachine::I_true,
	Kernel::I_resetRef,
	Item::I_getNpcNum,
	// 0x70
	Item::I_andStatus,
	Ultima8Engine::I_getUnkCrusaderFlag,
	Ultima8Engine::I_setUnkCrusaderFlag,
	Ultima8Engine::I_setCruStasis,
	Actor::I_setDead,
	Ultima8Engine::I_clrUnkCrusaderFlag,
	Ultima8Engine::I_clrCruStasis,
	AudioProcess::I_stopSFXCru,
	PaletteFaderProcess::I_fadeToBlack,
	MainActor::I_clrKeycards,
	MainActor::I_teleportToEgg,
	PaletteFaderProcess::I_fadeToGamePal,
	Actor::I_setImmortal,
	Actor::I_getHp,
	Actor::I_setActivity,
	Item::I_getQuality,
	// 0x80
	Item::I_setQuality,
	Item::I_use,
	MainActor::I_getMaxEnergy,
	Actor::I_getMana,
	Actor::I_setMana,
	Item::I_getQLo,
	Actor::I_setImmortal,
	CameraProcess::I_getCameraX,
	CameraProcess::I_getCameraY,
	Item::I_setMapArray,
	Item::I_getNpcNum,
	Item::I_shoot,
	Item::I_enterFastArea,
	Item::I_setBroken,
	Item::I_hurl,
	Item::I_getNpcNum,
	// 0x90
	PaletteFaderProcess::I_jumpToAllBlack,
	MusicProcess::I_stopMusic,
	0,
	MovieGump::I_playMovieCutsceneAlt,
	0,
	Game::I_playCredits,
	Ultima8Engine::I_moveKeyDownRecently,
	MainActor::I_teleportToEgg,
	PaletteFaderProcess::I_jumpToGreyScale,
	World::I_resetVargasShield,
	Item::I_andStatus,
	PaletteFaderProcess::I_jumpToNormalPalette,
	PaletteFaderProcess::I_fadeToGamePal,
	PaletteFaderProcess::I_fadeToGamePal,
	PaletteFaderProcess::I_fadeToBlack,
	PaletteFaderProcess::I_fadeToBlack,
	// 0xA0
	PaletteFaderProcess::I_fadeToGivenColor,
	Actor::I_setDead,
	Item::I_getQLo,
	Item::I_getUnkEggType,
	Egg::I_setEggXRange,
	Item::I_overlaps,
	Item::I_isOn,
	UCMachine::I_true,
	Egg::I_getEggXRange,
	Actor::I_setDead,
	MovieGump::I_playMovieCutsceneAlt,
	AudioProcess::I_playSFX,
	Actor::I_isFalling,
	Item::I_getFamilyOfType,
	Item::I_getNpcNum,
	Item::I_getQLo,
	// 0xB0
	Item::I_getQHi,
	Item::I_unequip,
	Item::I_avatarStoleSomething,
	Item::I_andStatus,
	Ultima8Engine::I_getCurrentTimerTick,
	World::I_setAlertActive,
	Item::I_equip,
	World::I_clrAlertActive,
	Ultima8Engine::I_getAvatarInStasis,
	MainActor::I_addItemCru,
	Actor::I_getLastAnimSet,
	Item::I_setQuality,
	CurrentMap::I_canExistAt,
	Item::I_isOn,
	Item::I_hurl,
	Item::I_getQHi,
	// 0xC0
	Item::I_andStatus,
	Item::I_hurl,
	Item::I_andStatus,
	Item::I_hurl,
	Item::I_andStatus,
	KeypadGump::I_showKeypad,
	Item::I_isOn,
	SpriteProcess::I_createSprite,
	Item::I_getDirFromItem,
	Item::I_hurl,
	Item::I_getQHi,
	Actor::I_addHp,
	Game::I_playDemoScreen,
	Actor::I_isInCombat,
	Actor::I_setActivity,
	Item::I_setQAndCombine,
	// 0xD0
	Item::I_use,
	AudioProcess::I_stopAllSFX,
	MovieGump::I_playMovieCutscene,
	0,
	AudioProcess::I_playSFX,
	Item::I_use,
	CameraProcess::I_getCameraZ,
	Actor::I_getLastAnimSet,
	Actor::I_setDead,
	Item::I_getQLo,
	PaletteFaderProcess::I_jumpToAllWhite,
	Actor::I_setActivity,
	Item::I_isOn,
	Actor::I_getLastActivityNo,
	Actor::I_setCombatTactic,
	Actor::I_getEquip,
	// 0xE0
	Actor::I_setEquip,
	Actor::I_getDefaultActivity0,
	Actor::I_getDefaultActivity1,
	Actor::I_getDefaultActivity2,
	Actor::I_getLastAnimSet,
	Actor::I_setTarget,
	Actor::I_setUnkByte,
	Actor::I_setDead,
	Item::I_cast,
	Item::I_andStatus,
	Item::I_getQLo,
	MainActor::I_getNumberOfCredits,
	Item::I_popToEnd,
	Item::I_popToContainer,
	BatteryChargerProcess::I_create,
	Kernel::I_getNumProcesses,
	// 0xF0
	Item::I_getQHi,
	Item::I_isOn,
	Actor::I_setActivity,
	Item::I_getQHi,
	Item::I_getQ,
	Item::I_setQ,
	CruHealerProcess::I_create,
	Item::I_hurl,
	Item::I_getNpcNum,
	Item::I_hurl,
	Item::I_isOn,
	Item::I_getQHi,
	Item::I_andStatus,
	MainActor::I_hasKeycard,
	ComputerGump::I_readComputer,
	UCMachine::I_numToStr,
	// 0x100
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_isCrusTypeNPC,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Actor::I_getDir,
	UCMachine::I_numToStr,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_fireDistance,
	Item::I_andStatus,
	Item::I_hurl,
	Item::I_setBroken,
	Item::I_andStatus,
	Item::I_getTypeFlag,
	Item::I_getNpcNum,
	Item::I_hurl,
	Item::I_getCY,
	Item::I_getCZ,
	Item::I_getCX,
	Actor::I_getDir,
	UCMachine::I_numToStr,
	Item::I_getDirToCoords,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_isOn,
	Item::I_getFootpadData,
	Actor::I_isDead,
	Actor::I_createActorCru,
	Actor::I_setImmortal,
	Item::I_andStatus,
	Item::I_getQHi,
	WeaselGump::I_showWeaselGump,
	Actor::I_setDead,
	Item::I_getQLo,
	Item::I_getNpcNum,
	Actor::I_getDir,
	Actor::I_getLastAnimSet,
	Item::I_setQuality,
	0 // Unused
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Intrinsics for the Remorse Spanish version.
Intrinsic RemorseEsIntrinsics[] = {
	ComputerGump::I_readComputer,
	Item::I_getMapArray,
	Item::I_getFrame,
	Item::I_setFrame,
	UCMachine::I_rndRange,
	UCMachine::I_numToStr,
	CruStatusGump::I_hideStatusGump,
	Actor::I_getMap,
	MovieGump::I_playMovieOverlay,
	CruStatusGump::I_showStatusGump,
	Item::I_getDirToItem,
	Actor::I_turnToward,
	MovieGump::I_playMovieCutsceneAlt,
	AudioProcess::I_playSFXCru,
	World::I_getAlertActive,
	Item::I_getStatus,
	// 0x010
	Item::I_orStatus,
	Item::I_equip,
	Item::I_isPartlyOnScreen,
	Actor::I_isNPC,
	Item::I_getZ,
	Item::I_destroy,
	Actor::I_getUnkByte,
	Ultima8Engine::I_setAvatarInStasis,
	Item::I_getQLo,
	MusicProcess::I_playMusic,
	Item::I_getX,
	Item::I_getY,
	Item::I_getShape,
	Item::I_explode,
	Item::I_legalCreateAtCoords,
	Item::I_andStatus,
	// 0x020
	World::I_getControlledNPCNum,
	Actor::I_getDir,
	Actor::I_getLastAnimSet,
	Item::I_fireWeapon,
	Item::I_create,
	Item::I_popToCoords,
	Actor::I_setDead,
	Item::I_push,
	Item::I_getEtherealTop,
	Item::I_setShape,
	Item::I_touch,
	Item::I_getQHi,
	Item::I_getClosestDirectionInRange,
	Item::I_hurl,
	World::I_gameDifficulty,
	AudioProcess::I_playAmbientSFXCru,
	// 0x030
	Item::I_isCompletelyOn,
	UCMachine::I_true,
	Container::I_destroyContents,
	Item::I_getQLo,
	Item::I_inFastArea,
	Item::I_setQHi,
	Item::I_legalMoveToPoint,
	CurrentMap::I_canExistAtPoint,
	Item::I_pop,
	Item::I_andStatus,
	Item::I_receiveHit,
	Actor::I_isBusy,
	Item::I_getDirFromTo16,
	Actor::I_isKneeling,
	Actor::I_doAnim,
	MainActor::I_addItemCru,
	// 0x040
	AudioProcess::I_stopSFXCru,
	Actor::I_isDead,
	AudioProcess::I_isSFXPlayingForObject,
	Item::I_setQLo,
	Item::I_getFamily,
	Item::I_fall,
	Egg::I_getEggId,
	CameraProcess::I_moveTo,
	CameraProcess::I_setCenterOn,
	Item::I_getRangeIfVisible,
	AudioProcess::I_playSFXCru,
	Item::I_isOn,
	Item::I_getQHi,
	Item::I_isOn,
	Item::I_getQHi,
	Item::I_isOn,
	// 0x050
	Item::I_getQHi,
	Item::I_isOn,
	Item::I_getQHi,
	Item::I_isOn,
	Item::I_getQHi,
	Item::I_isOn,
	Item::I_getQHi,
	Kernel::I_resetRef,
	Actor::I_getCurrentActivityNo,
	Actor::I_clrInCombat,
	Actor::I_setDefaultActivity0,
	Actor::I_setDefaultActivity1,
	Actor::I_setDefaultActivity2,
	Actor::I_setActivity,
	World::I_setControlledNPCNum,
	Item::I_getSurfaceWeight,
	Item::I_isCentreOn,
	Item::I_setFrame,
	Actor::I_getLastAnimSet,
	Item::I_legalCreateAtPoint,
	Item::I_getPoint,
	Actor::I_setDead,
	Actor::I_createActor,
	0,
	Actor::I_teleport,
	Item::I_getFootpadData,
	Item::I_isInNpc,
	Item::I_getQLo,
	Item::I_getNpcNum,
	Item::I_setNpcNum,
	Item::I_andStatus,
	Item::I_move,
	UCMachine::I_true,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Ultima8Engine::I_getUnkCrusaderFlag,
	Ultima8Engine::I_setUnkCrusaderFlag,
	Ultima8Engine::I_setCruStasis,
	Actor::I_setDead,
	Ultima8Engine::I_clrUnkCrusaderFlag,
	Ultima8Engine::I_clrCruStasis,
	AudioProcess::I_stopSFXCru,
	PaletteFaderProcess::I_fadeToBlack,
	MainActor::I_clrKeycards,
	MainActor::I_teleportToEgg,
	PaletteFaderProcess::I_fadeToGamePal,
	Actor::I_setImmortal,
	Actor::I_getHp,
	Actor::I_setActivity,
	Item::I_getQuality,
	Item::I_setQuality,
	Item::I_use,
	MainActor::I_getMaxEnergy,
	Actor::I_getMana,
	Actor::I_setMana,
	Item::I_getQLo,
	Actor::I_setImmortal,
	CameraProcess::I_getCameraX,
	CameraProcess::I_getCameraY,
	Item::I_setMapArray,
	Item::I_getNpcNum,
	Item::I_shoot,
	Item::I_enterFastArea,
	Item::I_setBroken,
	Item::I_hurl,
	Item::I_getNpcNum,
	PaletteFaderProcess::I_jumpToAllBlack,
	MusicProcess::I_stopMusic,
	0,
	MovieGump::I_playMovieCutsceneAlt,
	0,
	Game::I_playCredits,
	Ultima8Engine::I_moveKeyDownRecently,
	MainActor::I_teleportToEgg,
	PaletteFaderProcess::I_jumpToGreyScale,
	World::I_resetVargasShield,
	Item::I_andStatus,
	PaletteFaderProcess::I_jumpToNormalPalette,
	PaletteFaderProcess::I_fadeToGamePal,
	PaletteFaderProcess::I_fadeToGamePal,
	PaletteFaderProcess::I_fadeToBlack,
	PaletteFaderProcess::I_fadeToBlack,
	PaletteFaderProcess::I_fadeToGivenColor,
	Actor::I_setDead,
	Item::I_getQLo,
	Item::I_getUnkEggType,
	Egg::I_setEggXRange,
	Item::I_overlaps,
	Item::I_isOn,
	UCMachine::I_true,
	KeypadGump::I_showKeypad,
	MovieGump::I_playMovieCutsceneAlt,
	Egg::I_getEggXRange,
	Actor::I_setDead,
	AudioProcess::I_playSFX,
	Actor::I_isFalling,
	Item::I_getFamilyOfType,
	Item::I_getNpcNum,
	Item::I_getQLo,
	Item::I_getQHi,
	Item::I_unequip,
	Item::I_avatarStoleSomething,
	Item::I_andStatus,
	Ultima8Engine::I_getCurrentTimerTick,
	World::I_setAlertActive,
	Item::I_equip,
	World::I_clrAlertActive,
	Ultima8Engine::I_getAvatarInStasis,
	MainActor::I_addItemCru,
	Actor::I_getLastAnimSet,
	Item::I_setQuality,
	CurrentMap::I_canExistAt,
	Item::I_isOn,
	Item::I_hurl,
	Item::I_getQHi,
	Item::I_andStatus,
	Item::I_hurl,
	Item::I_andStatus,
	Item::I_hurl,
	Item::I_andStatus,
	Item::I_getDirToCoords,
	Item::I_popToContainer,
	Item::I_getNpcNum,
	Item::I_setBroken,
	Item::I_getCY,
	Item::I_getCX,
	SpriteProcess::I_createSprite,
	Item::I_isOn,
	Item::I_getFootpadData,
	Actor::I_isDead,
	Actor::I_createActorCru,
	Actor::I_setActivity,
	Item::I_andStatus,
	Item::I_setQuality,
	Actor::I_getLastAnimSet,
	Item::I_getDirFromItem,
	Item::I_hurl,
	Item::I_getQHi,
	Actor::I_addHp,
	MainActor::I_switchMap, // TODO: Intrinsic0DB 1028:049c,
	Actor::I_setActivity,
	Actor::I_isInCombat,
	Item::I_setQAndCombine,
	Item::I_use,
	AudioProcess::I_stopAllSFX,
	MovieGump::I_playMovieCutscene,
	0,
	AudioProcess::I_playSFX,
	Item::I_use,
	CameraProcess::I_getCameraZ,
	Actor::I_getLastAnimSet,
	Actor::I_setDead,
	Item::I_getQLo,
	PaletteFaderProcess::I_jumpToAllWhite,
	Item::I_equip,
	Actor::I_setActivity,
	Item::I_isOn,
	Actor::I_getLastActivityNo,
	Actor::I_setCombatTactic,
	Actor::I_getEquip,
	Actor::I_setEquip,
	Actor::I_getDefaultActivity0,
	Actor::I_getDefaultActivity1,
	Actor::I_getDefaultActivity2,
	Actor::I_getLastAnimSet,
	Item::I_getQHi,
	Item::I_getNpcNum,
	Item::I_cast,
	MainActor::I_getNumberOfCredits,
	Actor::I_setTarget,
	Actor::I_setUnkByte,
	Item::I_hurl,
	Actor::I_setDead,
	Item::I_andStatus,
	Item::I_getQLo,
	Item::I_popToEnd,
	BatteryChargerProcess::I_create,
	Kernel::I_getNumProcesses,
	Item::I_isOn,
	Actor::I_setActivity,
	Item::I_getQHi,
	Item::I_getQ,
	Item::I_setQ,
	CruHealerProcess::I_create,
	Item::I_hurl,
	Item::I_getNpcNum,
	Item::I_hurl,
	Item::I_isOn,
	Item::I_andStatus,
	Item::I_getQHi,
	MainActor::I_hasKeycard,
	Actor::I_setDead,
	Actor::I_setImmortal,
	Item::I_getQLo,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_isCrusTypeNPC,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_andStatus,
	Item::I_getNpcNum,
	Item::I_fireDistance,
	Item::I_andStatus,
	Item::I_hurl,
	Item::I_setBroken,
	Item::I_andStatus,
	WeaselGump::I_showWeaselGump,
	Item::I_getTypeFlag,
	Item::I_getNpcNum,
	Item::I_hurl,
	Item::I_getCY,
	Item::I_getCZ,
	0 //UNUSEDInt136,
};

} // End of namespace Ultima8
} // End of namespace Ultima

#endif
