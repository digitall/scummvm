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

#include "scumm/actor.h"
#include "scumm/charset.h"
#include "scumm/object.h"
#include "scumm/resource.h"
#include "scumm/scumm_v2.h"
#include "scumm/sound.h"
#include "scumm/util.h"
#include "scumm/verbs.h"

namespace Scumm {

// Helper functions for Maniac Mansion workarounds
#define MM_SCRIPT(script)  ((script) + (_game.version == 0 ? 0 : 5))
#define MM_VALUE(v0,v1)    (_game.version == 0 ? (v0) : (v1))

#define OPCODE(i, x)	_opcodes[i]._OPCODE(ScummEngine_v2, x)

void ScummEngine_v2::setupOpcodes() {
	/* 00 */
	OPCODE(0x00, o5_stopObjectCode);
	OPCODE(0x01, o2_putActor);
	OPCODE(0x02, o5_startMusic);
	OPCODE(0x03, o5_getActorRoom);
	/* 04 */
	OPCODE(0x04, o2_isGreaterEqual);
	OPCODE(0x05, o2_drawObject);
	OPCODE(0x06, o2_getActorElevation);
	OPCODE(0x07, o2_setStateIntrinsicOn);
	/* 08 */
	OPCODE(0x08, o5_isNotEqual);
	OPCODE(0x09, o5_faceActor);
	OPCODE(0x0a, o2_assignVarWordIndirect);
	OPCODE(0x0b, o2_setObjPreposition);
	/* 0C */
	OPCODE(0x0c, o2_resourceRoutines);
	OPCODE(0x0d, o5_walkActorToActor);
	OPCODE(0x0e, o2_putActorAtObject);
	OPCODE(0x0f, o2_ifStateIntrinsicOn);
	/* 10 */
	OPCODE(0x10, o5_getObjectOwner);
	OPCODE(0x11, o5_animateActor);
	OPCODE(0x12, o2_panCameraTo);
	OPCODE(0x13, o2_actorOps);
	/* 14 */
	OPCODE(0x14, o5_print);
	OPCODE(0x15, o2_actorFromPos);
	OPCODE(0x16, o5_getRandomNr);
	OPCODE(0x17, o2_setStateTouchable);
	/* 18 */
	OPCODE(0x18, o5_jumpRelative);
	OPCODE(0x19, o2_doSentence);
	OPCODE(0x1a, o5_move);
	OPCODE(0x1b, o2_setBitVar);
	/* 1C */
	OPCODE(0x1c, o5_startSound);
	OPCODE(0x1d, o2_ifClassOfIs);
	OPCODE(0x1e, o2_walkActorTo);
	OPCODE(0x1f, o2_ifStateTouchable);
	/* 20 */
	OPCODE(0x20, o5_stopMusic);
	OPCODE(0x21, o2_putActor);
	OPCODE(0x22, o4_saveLoadGame);
	OPCODE(0x23, o2_getActorY);
	/* 24 */
	OPCODE(0x24, o2_loadRoomWithEgo);
	OPCODE(0x25, o2_drawObject);
	OPCODE(0x26, o5_setVarRange);
	OPCODE(0x27, o2_setStateLocked);
	/* 28 */
	OPCODE(0x28, o5_equalZero);
	OPCODE(0x29, o2_setOwnerOf);
	OPCODE(0x2a, o2_addIndirect);
	OPCODE(0x2b, o5_delayVariable);
	/* 2C */
	OPCODE(0x2c, o2_assignVarByte);
	OPCODE(0x2d, o2_putActorInRoom);
	OPCODE(0x2e, o2_delay);
	OPCODE(0x2f, o2_ifStateLocked);
	/* 30 */
	OPCODE(0x30, o3_setBoxFlags);
	OPCODE(0x31, o2_getBitVar);
	OPCODE(0x32, o2_setCameraAt);
	OPCODE(0x33, o2_roomOps);
	/* 34 */
	OPCODE(0x34, o5_getDist);
	OPCODE(0x35, o2_findObject);
	OPCODE(0x36, o2_walkActorToObject);
	OPCODE(0x37, o2_setStatePickupable);
	/* 38 */
	OPCODE(0x38, o2_isLessEqual);
	OPCODE(0x39, o2_doSentence);
	OPCODE(0x3a, o2_subtract);
	OPCODE(0x3b, o2_waitForActor);
	/* 3C */
	OPCODE(0x3c, o5_stopSound);
	OPCODE(0x3d, o2_setActorElevation);
	OPCODE(0x3e, o2_walkActorTo);
	OPCODE(0x3f, o2_ifStatePickupable);
	/* 40 */
	OPCODE(0x40, o2_cutscene);
	OPCODE(0x41, o2_putActor);
	OPCODE(0x42, o2_startScript);
	OPCODE(0x43, o2_getActorX);
	/* 44 */
	OPCODE(0x44, o2_isLess);
	OPCODE(0x45, o2_drawObject);
	OPCODE(0x46, o5_increment);
	OPCODE(0x47, o2_setStateIntrinsicOff);
	/* 48 */
	OPCODE(0x48, o5_isEqual);
	OPCODE(0x49, o5_faceActor);
	OPCODE(0x4a, o2_chainScript);
	OPCODE(0x4b, o2_setObjPreposition);
	/* 4C */
	OPCODE(0x4c, o2_waitForSentence);
	OPCODE(0x4d, o5_walkActorToActor);
	OPCODE(0x4e, o2_putActorAtObject);
	OPCODE(0x4f, o2_ifStateIntrinsicOff);
	/* 50 */
	OPCODE(0x50, o2_pickupObject);
	OPCODE(0x51, o5_animateActor);
	OPCODE(0x52, o5_actorFollowCamera);
	OPCODE(0x53, o2_actorOps);
	/* 54 */
	OPCODE(0x54, o5_setObjectName);
	OPCODE(0x55, o2_actorFromPos);
	OPCODE(0x56, o5_getActorMoving);
	OPCODE(0x57, o2_setStateUntouchable);
	/* 58 */
	OPCODE(0x58, o2_beginOverride);
	OPCODE(0x59, o2_doSentence);
	OPCODE(0x5a, o2_add);
	OPCODE(0x5b, o2_setBitVar);
	/* 5C */
	OPCODE(0x5c, o2_dummy);
	OPCODE(0x5d, o2_ifClassOfIs);
	OPCODE(0x5e, o2_walkActorTo);
	OPCODE(0x5f, o2_ifStateUntouchable);
	/* 60 */
	OPCODE(0x60, o2_cursorCommand);
	OPCODE(0x61, o2_putActor);
	OPCODE(0x62, o2_stopScript);
	OPCODE(0x63, o5_getActorFacing);
	/* 64 */
	OPCODE(0x64, o2_loadRoomWithEgo);
	OPCODE(0x65, o2_drawObject);
	OPCODE(0x66, o5_getClosestObjActor);
	OPCODE(0x67, o2_setStateUnlocked);
	/* 68 */
	OPCODE(0x68, o5_isScriptRunning);
	OPCODE(0x69, o2_setOwnerOf);
	OPCODE(0x6a, o2_subIndirect);
	OPCODE(0x6b, o2_dummy);
	/* 6C */
	OPCODE(0x6c, o2_getObjPreposition);
	OPCODE(0x6d, o2_putActorInRoom);
	OPCODE(0x6e, o2_dummy);
	OPCODE(0x6f, o2_ifStateUnlocked);
	/* 70 */
	OPCODE(0x70, o2_lights);
	OPCODE(0x71, o5_getActorCostume);
	OPCODE(0x72, o5_loadRoom);
	OPCODE(0x73, o2_roomOps);
	/* 74 */
	OPCODE(0x74, o5_getDist);
	OPCODE(0x75, o2_findObject);
	OPCODE(0x76, o2_walkActorToObject);
	OPCODE(0x77, o2_setStateUnpickupable);
	/* 78 */
	OPCODE(0x78, o2_isGreater);
	OPCODE(0x79, o2_doSentence);
	OPCODE(0x7a, o2_verbOps);
	OPCODE(0x7b, o2_getActorWalkBox);
	/* 7C */
	OPCODE(0x7c, o5_isSoundRunning);
	OPCODE(0x7d, o2_setActorElevation);
	OPCODE(0x7e, o2_walkActorTo);
	OPCODE(0x7f, o2_ifStateUnpickupable);
	/* 80 */
	OPCODE(0x80, o5_breakHere);
	OPCODE(0x81, o2_putActor);
	OPCODE(0x82, o5_startMusic);
	OPCODE(0x83, o5_getActorRoom);
	/* 84 */
	OPCODE(0x84, o2_isGreaterEqual);
	OPCODE(0x85, o2_drawObject);
	OPCODE(0x86, o2_getActorElevation);
	OPCODE(0x87, o2_setStateIntrinsicOn);
	/* 88 */
	OPCODE(0x88, o5_isNotEqual);
	OPCODE(0x89, o5_faceActor);
	OPCODE(0x8a, o2_assignVarWordIndirect);
	OPCODE(0x8b, o2_setObjPreposition);
	/* 8C */
	OPCODE(0x8c, o2_resourceRoutines);
	OPCODE(0x8d, o5_walkActorToActor);
	OPCODE(0x8e, o2_putActorAtObject);
	OPCODE(0x8f, o2_ifStateIntrinsicOn);
	/* 90 */
	OPCODE(0x90, o5_getObjectOwner);
	OPCODE(0x91, o5_animateActor);
	OPCODE(0x92, o2_panCameraTo);
	OPCODE(0x93, o2_actorOps);
	/* 94 */
	OPCODE(0x94, o5_print);
	OPCODE(0x95, o2_actorFromPos);
	OPCODE(0x96, o5_getRandomNr);
	OPCODE(0x97, o2_setStateTouchable);
	/* 98 */
	OPCODE(0x98, o2_restart);
	OPCODE(0x99, o2_doSentence);
	OPCODE(0x9a, o5_move);
	OPCODE(0x9b, o2_setBitVar);
	/* 9C */
	OPCODE(0x9c, o5_startSound);
	OPCODE(0x9d, o2_ifClassOfIs);
	OPCODE(0x9e, o2_walkActorTo);
	OPCODE(0x9f, o2_ifStateTouchable);
	/* A0 */
	OPCODE(0xa0, o5_stopObjectCode);
	OPCODE(0xa1, o2_putActor);
	OPCODE(0xa2, o4_saveLoadGame);
	OPCODE(0xa3, o2_getActorY);
	/* A4 */
	OPCODE(0xa4, o2_loadRoomWithEgo);
	OPCODE(0xa5, o2_drawObject);
	OPCODE(0xa6, o5_setVarRange);
	OPCODE(0xa7, o2_setStateLocked);
	/* A8 */
	OPCODE(0xa8, o5_notEqualZero);
	OPCODE(0xa9, o2_setOwnerOf);
	OPCODE(0xaa, o2_addIndirect);
	OPCODE(0xab, o2_switchCostumeSet);
	/* AC */
	OPCODE(0xac, o2_drawSentence);
	OPCODE(0xad, o2_putActorInRoom);
	OPCODE(0xae, o2_waitForMessage);
	OPCODE(0xaf, o2_ifStateLocked);
	/* B0 */
	OPCODE(0xb0, o3_setBoxFlags);
	OPCODE(0xb1, o2_getBitVar);
	OPCODE(0xb2, o2_setCameraAt);
	OPCODE(0xb3, o2_roomOps);
	/* B4 */
	OPCODE(0xb4, o5_getDist);
	OPCODE(0xb5, o2_findObject);
	OPCODE(0xb6, o2_walkActorToObject);
	OPCODE(0xb7, o2_setStatePickupable);
	/* B8 */
	OPCODE(0xb8, o2_isLessEqual);
	OPCODE(0xb9, o2_doSentence);
	OPCODE(0xba, o2_subtract);
	OPCODE(0xbb, o2_waitForActor);
	/* BC */
	OPCODE(0xbc, o5_stopSound);
	OPCODE(0xbd, o2_setActorElevation);
	OPCODE(0xbe, o2_walkActorTo);
	OPCODE(0xbf, o2_ifStatePickupable);
	/* C0 */
	OPCODE(0xc0, o2_endCutscene);
	OPCODE(0xc1, o2_putActor);
	OPCODE(0xc2, o2_startScript);
	OPCODE(0xc3, o2_getActorX);
	/* C4 */
	OPCODE(0xc4, o2_isLess);
	OPCODE(0xc5, o2_drawObject);
	OPCODE(0xc6, o5_decrement);
	OPCODE(0xc7, o2_setStateIntrinsicOff);
	/* C8 */
	OPCODE(0xc8, o5_isEqual);
	OPCODE(0xc9, o5_faceActor);
	OPCODE(0xca, o2_chainScript);
	OPCODE(0xcb, o2_setObjPreposition);
	/* CC */
	OPCODE(0xcc, o5_pseudoRoom);
	OPCODE(0xcd, o5_walkActorToActor);
	OPCODE(0xce, o2_putActorAtObject);
	OPCODE(0xcf, o2_ifStateIntrinsicOff);
	/* D0 */
	OPCODE(0xd0, o2_pickupObject);
	OPCODE(0xd1, o5_animateActor);
	OPCODE(0xd2, o5_actorFollowCamera);
	OPCODE(0xd3, o2_actorOps);
	/* D4 */
	OPCODE(0xd4, o5_setObjectName);
	OPCODE(0xd5, o2_actorFromPos);
	OPCODE(0xd6, o5_getActorMoving);
	OPCODE(0xd7, o2_setStateUntouchable);
	/* D8 */
	OPCODE(0xd8, o5_printEgo);
	OPCODE(0xd9, o2_doSentence);
	OPCODE(0xda, o2_add);
	OPCODE(0xdb, o2_setBitVar);
	/* DC */
	OPCODE(0xdc, o2_dummy);
	OPCODE(0xdd, o2_ifClassOfIs);
	OPCODE(0xde, o2_walkActorTo);
	OPCODE(0xdf, o2_ifStateUntouchable);
	/* E0 */
	OPCODE(0xe0, o2_cursorCommand);
	OPCODE(0xe1, o2_putActor);
	OPCODE(0xe2, o2_stopScript);
	OPCODE(0xe3, o5_getActorFacing);
	/* E4 */
	OPCODE(0xe4, o2_loadRoomWithEgo);
	OPCODE(0xe5, o2_drawObject);
	OPCODE(0xe6, o5_getClosestObjActor);
	OPCODE(0xe7, o2_setStateUnlocked);
	/* E8 */
	OPCODE(0xe8, o5_isScriptRunning);
	OPCODE(0xe9, o2_setOwnerOf);
	OPCODE(0xea, o2_subIndirect);
	OPCODE(0xeb, o2_dummy);
	/* EC */
	OPCODE(0xec, o2_getObjPreposition);
	OPCODE(0xed, o2_putActorInRoom);
	OPCODE(0xee, o2_dummy);
	OPCODE(0xef, o2_ifStateUnlocked);
	/* F0 */
	OPCODE(0xf0, o2_lights);
	OPCODE(0xf1, o5_getActorCostume);
	OPCODE(0xf2, o5_loadRoom);
	OPCODE(0xf3, o2_roomOps);
	/* F4 */
	OPCODE(0xf4, o5_getDist);
	OPCODE(0xf5, o2_findObject);
	OPCODE(0xf6, o2_walkActorToObject);
	OPCODE(0xf7, o2_setStateUnpickupable);
	/* F8 */
	OPCODE(0xf8, o2_isGreater);
	OPCODE(0xf9, o2_doSentence);
	OPCODE(0xfa, o2_verbOps);
	OPCODE(0xfb, o2_getActorWalkBox);
	/* FC */
	OPCODE(0xfc, o5_isSoundRunning);
	OPCODE(0xfd, o2_setActorElevation);
	OPCODE(0xfe, o2_walkActorTo);
	OPCODE(0xff, o2_ifStateUnpickupable);
}

#define SENTENCE_SCRIPT 2

int ScummEngine_v2::getVar() {
	return readVar(fetchScriptByte());
}

void ScummEngine_v2::decodeParseString() {
	byte buffer[512];
	byte *ptr = buffer;
	byte c;
	bool insertSpace = false;

	while ((c = fetchScriptByte())) {

		insertSpace = (c & 0x80) != 0;
		c &= 0x7f;

		if (c < 8) {
			// Special codes as seen in displayDialog etc. My guess is that they
			// have a similar function as the corresponding embedded stuff in modern
			// games. Hence for now we convert them to the modern format.
			// This might allow us to reuse the existing code.
			*ptr++ = 0xFF;
			*ptr++ = c;
			if (c > 3) {
				*ptr++ = fetchScriptByte();
				*ptr++ = 0;
			}
		} else
			*ptr++ = c;

		if (insertSpace)
			*ptr++ = ' ';

	}
	*ptr = 0;

	// WORKAROUND bug #13473: in the French version of Maniac Mansion, the cutscene
	// where Purple Tentacle is bullying Sandy hangs once Dr Fred is done talking,
	// because his reaction line in shorter in this translation (which is unusual for
	// French which tends to be more verbose) and the `unless (VAR_CHARCOUNT > 90)`
	// loop in script #155 hasn't been ajusted for this shorter length.
	//
	// So we add some extra spaces at the end of the string if it's too short; this
	// unblocks the cutscene and also lets Sandy react as intended.
	//
	// (Using `kEnhGameBreakingBugFixes`, because some users could be really confused
	// by the game hanging and they may not know about the Esc key.)
	if (_game.id == GID_MANIAC && _game.platform != Common::kPlatformNES && _language == Common::FR_FRA && currentScriptSlotIs(155) && _roomResource == 31 && _actorToPrintStrFor == 9 && enhancementEnabled(kEnhGameBreakingBugFixes)) {
		while (ptr - buffer < 100) {
			*ptr++ = ' ';
		}
		*ptr = 0;
	}

	// WORKAROUND: There is a typo in Syd's biography ("tring" instead of
	// "trying") in the English DOS version of Maniac Mansion (v1). As far
	// as I know, this is the only version with the typo.
	if (_game.id == GID_MANIAC && _game.version == 1
		&& _game.platform == Common::kPlatformDOS
		&& !(_game.features & GF_DEMO) && _language == Common::EN_ANY
		&& currentScriptSlotIs(260) && enhancementEnabled(kEnhTextLocFixes)
		&& strncmp((char *)buffer + 26, " tring ", 7) == 0) {
		for (byte *p = ptr; p >= buffer + 29; p--)
			*(p + 1) = *p;

		buffer[29] = 'y';
	}

	int pixelXOffset = (_game.platform == Common::kPlatformC64) ? 1 : 0;
	int textSlot = 0;
	_string[textSlot].xpos = 0 + pixelXOffset;
	_string[textSlot].ypos = 0;
	_string[textSlot].right = _screenWidth - 1 + pixelXOffset;
	_string[textSlot].center = false;
	_string[textSlot].overhead = false;

	if (_game.id == GID_MANIAC && _actorToPrintStrFor == 0xFF) {
		if (_game.version == 0) {
			_string[textSlot].color = 14;
		} else if (_game.features & GF_DEMO) {
			_string[textSlot].color = (_game.version == 2) ? 15 : 1;
		}
	}

	actorTalk(buffer);
}

int ScummEngine_v2::readVar(uint var) {
	if (_game.version >= 1 && var >= 14 && var <= 16)
		var = _scummVars[var];

	assertRange(0, var, _numVariables - 1, "variable (reading)");
	debugC(DEBUG_VARS, "readvar(%d) = %d", var, _scummVars[var]);
	return _scummVars[var];
}

void ScummEngine_v2::writeVar(uint var, int value) {
	assertRange(0, var, _numVariables - 1, "variable (writing)");
	debugC(DEBUG_VARS, "writeVar(%d) = %d", var, value);

	if (VAR_CUTSCENEEXIT_KEY != 0xFF && var == VAR_CUTSCENEEXIT_KEY) {
		// Remap the cutscene exit key in earlier games
		if (value == 4 || value == 13 || value == 64)
			value = 27;
	}

	// WORKAROUND: According to the Maniac Mansion manual, you should be
	// able to execute your command by clicking on the sentence line. But
	// this does not work until later games. The main difference between
	// the verb scripts (script 4) in Maniac Mansion and Zak McKracken is
	// that Zak will set variable 34 when you click on the sentence line
	// (as indicated by VAR_CLICK_AREA), and Maniac Mansion will not.
	//
	// When VAR_CLICK_AREA is 5, there is only one place where variable 34
	// is initialized to 0, so that seems like a good place to inject our
	// own check.

	if (_game.id == GID_MANIAC && (_game.version == 1 || _game.version == 2)
			&& _game.platform != Common::kPlatformNES
			&& currentScriptSlotIs(4)
			&& VAR(VAR_CLICK_AREA) == kSentenceClickArea
			&& var == 34 && value == 0 && enhancementEnabled(kEnhRestoredContent)) {
		value = 1;
	}

	_scummVars[var] = value;
}

void ScummEngine_v2::getResultPosIndirect() {
	_resultVarNumber = _scummVars[fetchScriptByte()];
}

void ScummEngine_v2::getResultPos() {
	_resultVarNumber = fetchScriptByte();
}

int ScummEngine_v2::getActiveObject() {
	return getVarOrDirectWord(PARAM_1);
}

void ScummEngine_v2::setStateCommon(byte type) {
	int obj = getActiveObject();
	putState(obj, getState(obj) | type);
}

void ScummEngine_v2::clearStateCommon(byte type) {
	int obj = getActiveObject();
	putState(obj, getState(obj) & ~type);
}

void ScummEngine_v2::ifStateZeroCommon(byte type) {
	int obj = getActiveObject();

	jumpRelative((getState(obj) & type) != 0);
}

void ScummEngine_v2::ifStateNotZeroCommon(byte type) {
	int obj = getActiveObject();

	jumpRelative((getState(obj) & type) == 0);
}

void ScummEngine_v2::o2_setStateIntrinsicOn() {
	int obj = getActiveObject();
	putState(obj, getState(obj) | kObjectStateIntrinsic);
	markObjectRectAsDirty(obj);
	clearDrawObjectQueue();
}

void ScummEngine_v2::o2_setStateIntrinsicOff() {
	int obj = getActiveObject();
	putState(obj, getState(obj) & ~kObjectStateIntrinsic);
	markObjectRectAsDirty(obj);
	clearDrawObjectQueue();
}

void ScummEngine_v2::o2_setStateLocked() {
	setStateCommon(kObjectStateLocked);
}

void ScummEngine_v2::o2_setStateUnlocked() {
	clearStateCommon(kObjectStateLocked);
}

void ScummEngine_v2::o2_setStateUntouchable() {
	setStateCommon(kObjectStateUntouchable);
}

void ScummEngine_v2::o2_setStateTouchable() {
	clearStateCommon(kObjectStateUntouchable);
}

void ScummEngine_v2::o2_setStatePickupable() {
	setStateCommon(kObjectStatePickupable);
}

void ScummEngine_v2::o2_setStateUnpickupable() {
	clearStateCommon(kObjectStatePickupable);
}

void ScummEngine_v2::o2_assignVarWordIndirect() {
	getResultPosIndirect();
	setResult(getVarOrDirectWord(PARAM_1));
}

void ScummEngine_v2::o2_assignVarByte() {
	getResultPos();
	setResult(fetchScriptByte());
}

void ScummEngine_v2::o2_setObjPreposition() {
	int obj = getVarOrDirectWord(PARAM_1);
	int unk = fetchScriptByte();

	if (_game.platform == Common::kPlatformNES)
		return;

	if (whereIsObject(obj) != WIO_NOT_FOUND) {
		// FIXME: this might not work properly the moment we save and restore the game.
		byte *ptr = getOBCDFromObject(obj) + 12;
		*ptr &= 0x1F;
		*ptr |= unk << 5;
	}
}

void ScummEngine_v2::o2_getObjPreposition() {
	getResultPos();
	int obj = getVarOrDirectWord(PARAM_1);

	if (whereIsObject(obj) != WIO_NOT_FOUND) {
		byte *ptr = getOBCDFromObject(obj) + 12;
		setResult(*ptr >> 5);
	} else {
		setResult(0xFF);
	}
}

void ScummEngine_v2::o2_setBitVar() {
	int var = fetchScriptWord();
	byte a = getVarOrDirectByte(PARAM_1);

	int bit_var = var + a;
	int bit_offset = bit_var & 0x0f;
	bit_var >>= 4;

	if (getVarOrDirectByte(PARAM_2))
		_scummVars[bit_var] |= (1 << bit_offset);
	else
		_scummVars[bit_var] &= ~(1 << bit_offset);

}

void ScummEngine_v2::o2_getBitVar() {
	getResultPos();
	int var = fetchScriptWord();
	byte a = getVarOrDirectByte(PARAM_1);

	int bit_var = var + a;
	int bit_offset = bit_var & 0x0f;
	bit_var >>= 4;

	setResult((_scummVars[bit_var] & (1 << bit_offset)) ? 1 : 0);
}

void ScummEngine_v2::o2_ifStateIntrinsicOff() {
	ifStateZeroCommon(kObjectStateIntrinsic);
}

void ScummEngine_v2::o2_ifStateIntrinsicOn() {
	ifStateNotZeroCommon(kObjectStateIntrinsic);
}

void ScummEngine_v2::o2_ifStateUnlocked() {
	ifStateZeroCommon(kObjectStateLocked);
}

void ScummEngine_v2::o2_ifStateLocked() {
	ifStateNotZeroCommon(kObjectStateLocked);
}

void ScummEngine_v2::o2_ifStateTouchable() {
	ifStateZeroCommon(kObjectStateUntouchable);
}

void ScummEngine_v2::o2_ifStateUntouchable() {
	ifStateNotZeroCommon(kObjectStateUntouchable);
}

void ScummEngine_v2::o2_ifStateUnpickupable() {
	ifStateZeroCommon(kObjectStatePickupable);
}

void ScummEngine_v2::o2_ifStatePickupable() {
	ifStateNotZeroCommon(kObjectStatePickupable);
}

void ScummEngine_v2::o2_addIndirect() {
	int a;
	getResultPosIndirect();
	a = getVarOrDirectWord(PARAM_1);
	_scummVars[_resultVarNumber] += a;
}

void ScummEngine_v2::o2_subIndirect() {
	int a;
	getResultPosIndirect();
	a = getVarOrDirectWord(PARAM_1);
	_scummVars[_resultVarNumber] -= a;
}

void ScummEngine_v2::o2_add() {
	int a;
	getResultPos();
	a = getVarOrDirectWord(PARAM_1);
	_scummVars[_resultVarNumber] += a;
}

void ScummEngine_v2::o2_subtract() {
	int a;
	getResultPos();
	a = getVarOrDirectWord(PARAM_1);
	_scummVars[_resultVarNumber] -= a;
}

void ScummEngine_v2::o2_waitForActor() {
	Actor *a = derefActor(getVarOrDirectByte(PARAM_1), "o2_waitForActor");
	if (a->_moving) {
		_scriptPointer -= 2;
		o5_breakHere();
	}
}

void ScummEngine_v2::o2_waitForMessage() {
	if (VAR(VAR_HAVE_MSG)) {
		_scriptPointer--;
		o5_breakHere();
	}
}

void ScummEngine_v2::o2_waitForSentence() {
	if (!_sentenceNum && !isScriptInUse(SENTENCE_SCRIPT))
		return;

	_scriptPointer--;
	o5_breakHere();
}

void ScummEngine_v2::o2_actorOps() {
	int act = getVarOrDirectByte(PARAM_1);
	int arg = getVarOrDirectByte(PARAM_2);
	Actor *a;
	int i;

	_opcode = fetchScriptByte();
	if (act == 0 && _opcode == 5) {
		// This case happens in the Zak/MM bootscripts, to set the default talk color (9).
		_string[0].color = arg;
		return;
	}

	a = derefActor(act, "actorOps");

	switch (_opcode) {
	case 1:		// SO_SOUND
		a->_sound[0] = arg;
		break;
	case 2:		// SO_PALETTE
		if (_game.version == 1)
			i = act;
		else
			i = fetchScriptByte();

		a->setPalette(i, arg);
		break;
	case 3:		// SO_ACTOR_NAME
		loadPtrToResource(rtActorName, a->_number, nullptr);
		break;
	case 4:		// SO_COSTUME
		a->setActorCostume(arg);
		break;
	case 5:		// SO_TALK_COLOR
		if (_game.id == GID_MANIAC && _game.version == 2 && (_game.features & GF_DEMO) && arg == 1)
			a->_talkColor = 15;
		else
			a->_talkColor = arg;
		break;
	default:
		error("o2_actorOps: opcode %d not yet supported", _opcode);
	}
}

void ScummEngine_v2::o2_restart() {
	restart();
}

void ScummEngine_v2::o2_drawObject() {
	int obj, idx, i;
	ObjectData *od;
	uint16 x, y, w, h;
	int xpos, ypos;

	obj = getVarOrDirectWord(PARAM_1);
	xpos = getVarOrDirectByte(PARAM_2);
	ypos = getVarOrDirectByte(PARAM_3);

	idx = getObjectIndex(obj);
	if (idx == -1)
		return;

	od = &_objs[idx];
	if (xpos != 0xFF) {
		od->walk_x += (xpos * 8) - od->x_pos;
		od->x_pos = xpos * 8;
		od->walk_y += (ypos * 8) - od->y_pos;
		od->y_pos = ypos * 8;
	}
	addObjectToDrawQue(idx);

	x = od->x_pos;
	y = od->y_pos;
	w = od->width;
	h = od->height;

	i = _numLocalObjects;
	while (i--) {
		if (_objs[i].obj_nr && _objs[i].x_pos == x && _objs[i].y_pos == y && _objs[i].width == w && _objs[i].height == h)
			putState(_objs[i].obj_nr, getState(_objs[i].obj_nr) & ~kObjectStateIntrinsic);
	}

	putState(obj, getState(od->obj_nr) | kObjectStateIntrinsic);
}

void ScummEngine_v2::o2_resourceRoutines() {
	const ResType resTypes[] = {
		rtInvalid,
		rtInvalid,
		rtCostume,
		rtRoom,
		rtInvalid,
		rtScript,
		rtSound
	};
	int resid = getVarOrDirectByte(PARAM_1);
	int opcode = fetchScriptByte();

	ResType type = rtInvalid;
	if (0 <= (opcode >> 4) && (opcode >> 4) < (int)ARRAYSIZE(resTypes))
		type = resTypes[opcode >> 4];

	if ((opcode & 0x0f) == 0 || type == rtInvalid)
		return;

	// HACK V2 Maniac Mansion tries to load an invalid sound resource in demo script.
	if (_game.id == GID_MANIAC && _game.version == 2 && currentScriptSlotIs(9) && type == rtSound && resid == 1)
		return;

	if ((opcode & 0x0f) == 1) {
		ensureResourceLoaded(type, resid);
	} else {
		if (opcode & 1)
			_res->lock(type, resid);
		else
			_res->unlock(type, resid);
	}
}

void ScummEngine_v2::o2_verbOps() {
	int verb = fetchScriptByte();
	int slot, state;

	switch (verb) {
	case 0:		// SO_DELETE_VERBS
		slot = getVarOrDirectByte(PARAM_1) + 1;
		assert(0 < slot && slot < _numVerbs);
		killVerb(slot);
		break;

	case 0xFF:	// Verb On/Off
		verb = fetchScriptByte();
		state = fetchScriptByte();
		slot = getVerbSlot(verb, 0);
		_verbs[slot].curmode = state;
		break;

	default: {	// New Verb
		int x = fetchScriptByte() * 8;
		int y = fetchScriptByte() * 8;
		slot = getVarOrDirectByte(PARAM_1) + 1;
		int prep = fetchScriptByte(); // Only used in V1?
		// V1 Maniac verbs are relative to the 'verb area' - under the sentence
		if (_game.platform == Common::kPlatformNES)
			x += 8;
		else if ((_game.id == GID_MANIAC) && (_game.version == 1))
			y += 8;

		VerbSlot *vs;
		assert(0 < slot && slot < _numVerbs);

		vs = &_verbs[slot];
		vs->verbid = verb;
		if (_game.platform == Common::kPlatformNES) {
			vs->color = 1;
			vs->hicolor = 1;
			vs->dimcolor = 1;
		} else if (_game.platform == Common::kPlatformC64) {
			vs->color = 5;
			vs->hicolor = 7;
			vs->dimcolor = 11;
		} else {
			vs->color = (_game.id == GID_MANIAC && (_game.features & GF_DEMO)) ? 13 : 2;
			vs->hicolor = _hiLiteColorVerbArrow;
			vs->dimcolor = 8;
		}
		vs->type = kTextVerbType;
		vs->charset_nr = _string[0]._default.charset;
		vs->curmode = 1;
		vs->saveid = 0;
		vs->key = 0;
		vs->center = 0;
		vs->imgindex = 0;
		vs->prep = prep;

		vs->curRect.left = vs->origLeft = x;

		// WORKAROUND for original bug (#14198): The italian version of Maniac Mansion
		// erroneously set one of the verbs' ("Unlock") y coordinate to 1600 instead of
		// 168 via scripts. We apply the fix and mark it as an enhancement.
		if (_game.id == GID_MANIAC && _game.version == 2 && _language == Common::IT_ITA &&
			slot == 15 && y == 1600 && enhancementEnabled(kEnhTextLocFixes)) {
			vs->curRect.top = 168;
		} else {
			vs->curRect.top = y;
		}

		// FIXME: these keyboard map depends on the language of the game.
		// E.g. a german keyboard has 'z' and 'y' swapped, while a french
		// keyboard starts with "azerty", etc.
		if (_game.platform == Common::kPlatformNES) {
			static const char keyboard[] = {
					'q','w','e','r',
					'a','s','d','f',
					'z','x','c','v'
				};
			if (1 <= slot && slot <= ARRAYSIZE(keyboard))
				vs->key = keyboard[slot - 1];
		} else {
			static const char keyboard[] = {
					'q','w','e','r','t',
					'a','s','d','f','g',
					'z','x','c','v','b'
				};
			if (1 <= slot && slot <= ARRAYSIZE(keyboard))
				vs->key = keyboard[slot - 1];
		}

		// It follows the verb name
		loadPtrToResource(rtVerb, slot, nullptr);
		}
		break;
	}

	// Force redraw of the modified verb slot
	drawVerb(slot, 0);
	verbMouseOver(0);
}

void ScummEngine_v2::o2_doSentence() {
	int a;
	SentenceTab *st;

	a = getVarOrDirectByte(PARAM_1);
	if (a == 0xFC) {
		_sentenceNum = 0;
		stopScript(SENTENCE_SCRIPT);
		return;
	}
	if (a == 0xFB) {
		resetSentence();
		return;
	}

	assert(_sentenceNum < NUM_SENTENCE);
	st = &_sentence[_sentenceNum++];

	st->verb = a;
	st->objectA = getVarOrDirectWord(PARAM_2);
	st->objectB = getVarOrDirectWord(PARAM_3);
	st->preposition = (st->objectB != 0);
	st->freezeCount = 0;

	// Execute or print the sentence
	_opcode = fetchScriptByte();
	switch (_opcode) {
	case 0:
		// Do nothing (besides setting up the sentence above)
		break;
	case 1:
		// Execute the sentence
		_sentenceNum--;

		if (st->verb == 254) {
			ScummEngine::stopObjectScript(st->objectA);
		} else {
			bool isBackgroundScript;
			bool isSpecialVerb;
			if (st->verb != 253 && st->verb != 250) {
				VAR(VAR_ACTIVE_VERB) = st->verb;
				VAR(VAR_ACTIVE_OBJECT1) = st->objectA;
				VAR(VAR_ACTIVE_OBJECT2) = st->objectB;

				isBackgroundScript = false;
				isSpecialVerb = false;
			} else {
				isBackgroundScript = (st->verb == 250);
				isSpecialVerb = true;
				st->verb = 253;
			}

			// Check if an object script for this object is already running. If
			// so, reuse its script slot. Note that we abuse two script flags:
			// freezeResistant and recursive. We use them to track two
			// script flags used in V1/V2 games. The main reason we do it this
			// ugly evil way is to avoid having to introduce yet another save
			// game revision.
			int slot = -1;
			ScriptSlot *ss;
			int i;

			ss = vm.slot;
			for (i = 0; i < NUM_SCRIPT_SLOT; i++, ss++) {
				if (st->objectA == ss->number &&
					ss->freezeResistant == isBackgroundScript &&
					ss->recursive == isSpecialVerb &&
					(ss->where == WIO_ROOM || ss->where == WIO_INVENTORY || ss->where == WIO_FLOBJECT)) {
					slot = i;
					break;
				}
			}

			runObjectScript(st->objectA, st->verb, isBackgroundScript, isSpecialVerb, nullptr, slot);
		}
		break;
	case 2:
		// Print the sentence
		_sentenceNum--;

		VAR(VAR_SENTENCE_VERB) = st->verb;
		VAR(VAR_SENTENCE_OBJECT1) = st->objectA;
		VAR(VAR_SENTENCE_OBJECT2) = st->objectB;

		o2_drawSentence();
		break;
	default:
		error("o2_doSentence: unknown subopcode %d", _opcode);
	}
}

void ScummEngine_v2::drawPreposition(int index) {
		// The prepositions, like the fonts, were hard code in the engine. Thus
		// we have to do that, too, and provde localized versions for all the
		// languages MM/Zak are available in.
		const char *prepositions[][5] = {
			{ " ", " in", " with", " on", " to" },   // English
			{ " ", " mit", " mit", " mit", " zu" },  // German
			{ " ", " dans", " avec", " sur", " <" }, // French
			{ " ", " in", " con", " su", " a" },     // Italian
			{ " ", " en", " con", " en", " a" },     // Spanish
			{ " ", " \x7f", " \x7f", " na", " \x7f" },// Russian
			{ " ", " B", " SN", " SM", " M" },       // Hebrew
			};
		int lang;
		switch (_language) {
		case Common::DE_DEU:
			lang = 1;
			break;
		case Common::FR_FRA:
			lang = 2;
			break;
		case Common::IT_ITA:
			lang = 3;
			break;
		case Common::ES_ESP:
			lang = 4;
			break;
		case Common::RU_RUS:
			lang = 5;
			break;
		case Common::HE_ISR:
			lang = 6;
			break;
		default:
			lang = 0;	// Default to english
		}

		if (_game.platform == Common::kPlatformNES) {
			_sentenceBuf += (const char *)(getResourceAddress(rtCostume, 78) + VAR(VAR_SENTENCE_PREPOSITION) * 8 + 2);
		} else
			_sentenceBuf += prepositions[lang][index];
}

void ScummEngine_v2::o2_drawSentence() {
	drawSentence();
}

void ScummEngine_v2::o2_ifClassOfIs() {
	int obj = getVarOrDirectWord(PARAM_1);
	int clsop = getVarOrDirectByte(PARAM_2);


	byte *obcd = getOBCDFromObject(obj);

	if (obcd == nullptr) {
		o5_jumpRelative();
		return;
	}

	byte cls = *(obcd + 6);
	jumpRelative((cls & clsop) == clsop);
}

void ScummEngine_v2::o2_walkActorTo() {
	int x, y;
	Actor *a;

	int act = getVarOrDirectByte(PARAM_1);

	// WORKAROUND bug #2110: crash when trying to fly back to San Francisco.
	// walkActorTo() is called with an invalid actor number by script 115,
	// after the room is loaded. The original DOS interpreter probably let
	// this slip by (TODO: confirm this? and choose an Enhancement class).
	if (_game.id == GID_ZAK && _game.version == 1 && currentScriptSlotIs(115) && act == 249) {
		act = VAR(VAR_EGO);
	}

	a = derefActor(act, "o2_walkActorTo");

	x = getVarOrDirectByte(PARAM_2);
	y = getVarOrDirectByte(PARAM_3);

	a->startWalkActor(x, y, -1);
}

void ScummEngine_v2::o2_putActor() {
	int act = getVarOrDirectByte(PARAM_1);
	int x, y;
	Actor *a;

	a = derefActor(act, "o2_putActor");
	x = getVarOrDirectByte(PARAM_2);
	y = getVarOrDirectByte(PARAM_3);

	a->putActor(x, y);
}

void ScummEngine_v2::o2_startScript() {
	int script = getVarOrDirectByte(PARAM_1);

	if (!_copyProtection) {
		// The enhanced version of Zak McKracken included in the
		// SelectWare Classic Collection bundle used CD check instead
		// of the usual key code check at airports.
		if ((_game.id == GID_ZAK) && (script == 15) && (_roomResource == 45))
			return;
	}

	// WORKAROUND bug #2524: In Maniac Mansion, when the door bell
	// rings, then this normally causes Ted Edison to leave his room.
	// This is controlled by script 87. On the other hand, when the
	// player enters Ted's room while Ted is in it, then Ted captures
	// the player and puts his active ego into the cellar prison.
	//
	// Unfortunately, the two events can collide: If the cutscene is
	// playing in which Ted captures the player (controlled by script
	// 88) and simultaneously the door bell rings (due to package
	// delivery...) then this leads to an assertion (in ScummVM, due to
	// its stricter validity checking), or to unexpected / strange
	// behavior (in the original engine). The script writers apparently
	// anticipated the possibility of the door bell ringing: Before
	// script 91 starts script 88, it explicitly stops script 87.
	// Unfortunately, this is not quite enough, as script 87 can be
	// started while script 88 is already running -- specifically, by
	// the package delivery sequence.
	//
	// Now, one can easily suppress this particular assertion, but then
	// one still gets odd behavior: Ted is in the process of
	// incarcerating the player, when the door bell rings; Ted promptly
	// leaves to get the package, leaving the player alone (!), but then
	// moments later we cut to the cellar, where Ted just put the
	// player. That seems weird and irrational (the Edisons may be mad,
	// but they are not stupid when it comes to putting people into
	// their dungeon ;)
	//
	// To avoid this, we use a somewhat more elaborate workaround: If
	// script 88 or 89 are running (which control the capture resp.
	// imprisonment of the player), then any attempt to start script 87
	// (which makes Ted go answer the door bell) is simply ignored. This
	// way, the door bell still chimes, but Ted ignores it.
	if (_game.id == GID_MANIAC) {
		if (script == MM_SCRIPT(82)) {
			if (isScriptRunning(MM_SCRIPT(83)) || isScriptRunning(MM_SCRIPT(84)))
				return;
		}
	}

	// WORKAROUND bug #4556: Purple Tentacle can appear in the lab, after being
	// chased out and end up stuck in the room. This bug is triggered if the player
	// enters the lab within 45 minutes of first entering the mansion and has chased Purple Tentacle
	// out. Eventually the cutscene with Purple Tentacle chasing Sandy in the lab
	// will play. This script leaves Purple Tentacle in the room causing him to become
	// a permanent resident.
	// Our fix is simply to prevent the Cutscene playing, if the lab has already been stormed
	if (_game.id == GID_MANIAC) {
		if (script == MM_SCRIPT(150)) {
			if (VAR(MM_VALUE(104, 120)) == 1)
				return;
		}
	}

	runScript(script, 0, 0, nullptr);
}

void ScummEngine_v2::stopScriptCommon(int script) {
	// WORKAROUND bug #4112: If you enter the lab while Dr. Fred has the power turned off
	// to repair the Zom-B-Matic, the script will be stopped and the power will never turn
	// back on. This fix forces the power on, when the player enters the lab,
	// if the script which turned it off is running
	if (_game.id == GID_MANIAC && _roomResource == 4 && isScriptRunning(MM_SCRIPT(138))) {
		if (currentScriptSlotIs(MM_VALUE(130, 163))) {
			if (script == MM_SCRIPT(138)) {
				int obj = MM_VALUE(124, 157);
				putState(obj, getState(obj) & ~kObjectStateIntrinsic);
			}
		}
	}

	// FIXME: Nasty hack for bug #1529
	// Don't let the exit script for room 26 stop the script (116), when
	// switching to the dungeon (script 89)
	if (_game.id == GID_MANIAC && _roomResource == 26 && currentScriptSlotIs(kScriptNumEXCD)) {
		if (script == MM_SCRIPT(111) && isScriptRunning(MM_SCRIPT(84)))
			return;
	}

	if (script == 0)
		script = vm.slot[_currentScript].number;

	if (_currentScript != 0 && vm.slot[_currentScript].number == script)
		stopObjectCode();
	else
		stopScript(script);
}

void ScummEngine_v2::o2_stopScript() {
	stopScriptCommon(getVarOrDirectByte(PARAM_1));
}

void ScummEngine_v2::o2_panCameraTo() {
	panCameraTo(getVarOrDirectByte(PARAM_1) * V12_X_MULTIPLIER, 0);
}

void ScummEngine_v2::walkActorToObject(int actor, int obj) {
	int x, y, dir;
	getObjectXYPos(obj, x, y, dir);

	Actor *a = derefActor(actor, "walkActorToObject");
	AdjustBoxResult r = a->adjustXYToBeInBox(x, y);
	x = r.x;
	y = r.y;

	a->startWalkActor(x, y, dir);
}

void ScummEngine_v2::o2_walkActorToObject() {
	int actor = getVarOrDirectByte(PARAM_1);
	int obj = getVarOrDirectWord(PARAM_2);
	if (whereIsObject(obj) != WIO_NOT_FOUND) {
		walkActorToObject(actor, obj);
	}
}

void ScummEngine_v2::o2_putActorAtObject() {
	int obj, x, y;
	Actor *a;

	a = derefActor(getVarOrDirectByte(PARAM_1), "o2_putActorAtObject");
	obj = getVarOrDirectWord(PARAM_2);
	if (whereIsObject(obj) != WIO_NOT_FOUND) {
		getObjectXYPos(obj, x, y);
		AdjustBoxResult r = a->adjustXYToBeInBox(x, y);
		x = r.x;
		y = r.y;
	} else {
		x = 30;
		y = 60;
	}

	a->putActor(x, y);
}

void ScummEngine_v2::o2_putActorInRoom() {
	Actor *a;
	int act = getVarOrDirectByte(PARAM_1);
	int room = getVarOrDirectByte(PARAM_2);

	a = derefActor(act, "o2_putActorInRoom");

	a->_room = room;
	if (!room) {
		if (_game.id == GID_MANIAC && _game.version <= 1 && _game.platform != Common::kPlatformNES)
			a->setFacing(180);

		a->putActor(0, 0, 0);
	}

	// WORKAROUND bug #2285: Caponians dont disguise after using blue crystal
	// This is for a game scripting oversight.
	// After first using the blue crystal, a cutscene of the two Caponians plays (script-96),
	// locking object 344 (which prevents the cutscene playing again) and setting Var[245] to 0x18.
	// script-5 uses this variable to set the Caponian costume
	// On first appearance after using the blue crystal, the Caponians now will have the disguise on
	//
	// If you visit the spacecraft and ring the doorbell, Var[245] will be set to 0x1C (Disguise off)
	// Using the blue crystal again, will result in the Caponian appearing without his disguise
	// as Var[245] is never set back to 0x18. This WORKAROUND fixes the problem by ensuring
	// Var[245] is set to have the Disguise on in most situations
	//
	// We don't touch the variable in the following situations
	//  If the Caponian is being put into the space ship room, or the current room is the
	//  space ship and the Caponian is being put into the backroom of the telephone company (you didn't show your fan club card)
	//
	// TODO: choose an Enhancement class for this
	if (_game.id == GID_ZAK && _game.version <= 2 && act == 7) {
		// Is script-96 cutscene done
		if ((getState(344) & kObjectStateLocked)) {
			// Not 'putting' in the space ship
			if (room != 10) {
				// not putting in telephone back room, and not in space ship
				if (room != 16 && _currentRoom != 10) {
					// Set caponian costume to 'disguise on'
					writeVar(245, 0x18);
				}
			}
		}
	}
}

void ScummEngine_v2::o2_getActorElevation() {
	getResultPos();
	int act = getVarOrDirectByte(PARAM_1);
	Actor *a = derefActor(act, "o2_getActorElevation");
	setResult(a->getElevation());
}

void ScummEngine_v2::o2_setActorElevation() {
	int act = getVarOrDirectByte(PARAM_1);
	int elevation = (int8)getVarOrDirectByte(PARAM_2);

	Actor *a = derefActor(act, "o2_setActorElevation");
	a->setElevation(elevation);
}

void ScummEngine_v2::o2_actorFromPos() {
	int x, y;
	getResultPos();
	x = getVarOrDirectByte(PARAM_1) * V12_X_MULTIPLIER;
	y = getVarOrDirectByte(PARAM_2) * V12_Y_MULTIPLIER;
	setResult(getActorFromPos(x, y));
}

void ScummEngine_v2::o2_findObject() {
	int obj;
	getResultPos();
	int x = getVarOrDirectByte(PARAM_1) * V12_X_MULTIPLIER;
	int y = getVarOrDirectByte(PARAM_2) * V12_Y_MULTIPLIER;
	obj = findObject(x, y);
	if (obj == 0 && (_game.platform == Common::kPlatformNES) && (_userState & USERSTATE_IFACE_INVENTORY)) {
		if (_mouseOverBoxV2 >= 0 && _mouseOverBoxV2 < 4) {
			// Simulate inverse order
			int invCount = getInventoryCount(VAR(VAR_EGO));
			obj = findInventory(VAR(VAR_EGO), invCount - _inventoryOffset - _mouseOverBoxV2);
		}
	}
	setResult(obj);
}

void ScummEngine_v2::o2_getActorX() {
	int a;
	getResultPos();

	a = getVarOrDirectByte(PARAM_1);
	setResult(getObjX(actorToObj(a)));
}

void ScummEngine_v2::o2_getActorY() {
	int a;
	getResultPos();

	a = getVarOrDirectByte(PARAM_1);
	setResult(getObjY(actorToObj(a)));
}

void ScummEngine_v2::o2_isGreater() {
	uint16 a = getVar();
	uint16 b = getVarOrDirectWord(PARAM_1);
	jumpRelative(b > a);
}

void ScummEngine_v2::o2_isGreaterEqual() {
	uint16 a = getVar();
	uint16 b = getVarOrDirectWord(PARAM_1);
	jumpRelative(b >= a);
}

void ScummEngine_v2::o2_isLess() {
	uint16 a = getVar();
	uint16 b = getVarOrDirectWord(PARAM_1);

	jumpRelative(b < a);
}

void ScummEngine_v2::o2_isLessEqual() {
	uint16 a = getVar();
	uint16 b = getVarOrDirectWord(PARAM_1);
	jumpRelative(b <= a);
}

void ScummEngine_v2::o2_lights() {
	int a, b, c;

	a = getVarOrDirectByte(PARAM_1);
	b = fetchScriptByte();
	c = fetchScriptByte();

	if (c == 0) {
		if (_game.id == GID_MANIAC && _game.version == 1 && !(_game.platform == Common::kPlatformNES)) {
			// Convert older light mode values into
			// equivalent values of later games.
			// 0 Darkness
			// 1 Flashlight
			// 2 Lighted area
			if (a == 2)
				VAR(VAR_CURRENT_LIGHTS) = 11;
			else if (a == 1)
				VAR(VAR_CURRENT_LIGHTS) = 4;
			else
				VAR(VAR_CURRENT_LIGHTS) = 0;
		} else
			VAR(VAR_CURRENT_LIGHTS) = a;
	} else if (c == 1) {
		_flashlight.xStrips = a;
		_flashlight.yStrips = b;
	}
	_fullRedraw = true;
}

void ScummEngine_v2::o2_loadRoomWithEgo() {
	Actor *a;
	int obj, room, x, y, x2, y2, dir;

	obj = getVarOrDirectWord(PARAM_1);
	room = getVarOrDirectByte(PARAM_2);

	a = derefActor(VAR(VAR_EGO), "o2_loadRoomWithEgo");

	// The original interpreter sets the actors new room X/Y to the last rooms X/Y
	// This fixes a problem with MM: script 161 in room 12, the 'Oomph!' script
	// This scripts runs before the actor position is set to the correct room entry location
	if ((_game.id == GID_MANIAC) && (_game.platform != Common::kPlatformNES)) {
		a->putActor(a->getPos().x, a->getPos().y, room);
	} else {
		a->putActor(0, 0, room);
	}
	_egoPositioned = false;

	x = (int8)fetchScriptByte();
	y = (int8)fetchScriptByte();

	startScene(a->_room, a, obj);

	getObjectXYPos(obj, x2, y2, dir);
	AdjustBoxResult r = a->adjustXYToBeInBox(x2, y2);
	x2 = r.x;
	y2 = r.y;
	a->putActor(x2, y2, _currentRoom);
	a->setDirection(dir + 180);

	camera._dest.x = camera._cur.x = a->getPos().x;
	setCameraAt(a->getPos().x, a->getPos().y);
	setCameraFollows(a);

	_fullRedraw = true;

	resetSentence();

	if (x >= 0 && y >= 0) {
		a->startWalkActor(x, y, -1);
	}
	runScript(5, 0, 0, nullptr);
}

void ScummEngine_v2::o2_setOwnerOf() {
	int obj, owner;

	obj = getVarOrDirectWord(PARAM_1);
	owner = getVarOrDirectByte(PARAM_2);

	setOwnerOf(obj, owner);
}

void ScummEngine_v2::o2_delay() {
	int delay = fetchScriptByte();
	delay |= fetchScriptByte() << 8;
	delay |= fetchScriptByte() << 16;
	delay = 0xFFFFFF - delay;

	assert(_currentScript != 0xFF);
	vm.slot[_currentScript].delay = delay;
	vm.slot[_currentScript].status = ssPaused;
	o5_breakHere();
}

void ScummEngine_v2::o2_setCameraAt() {
	setCameraAtEx(getVarOrDirectByte(PARAM_1) * V12_X_MULTIPLIER);
}

void ScummEngine_v2::o2_roomOps() {
	int a = getVarOrDirectByte(PARAM_1);
	int b = getVarOrDirectByte(PARAM_2);

	_opcode = fetchScriptByte();
	switch (_opcode & 0x1F) {
	case 1:			// SO_ROOM_SCROLL
		a *= 8;
		b *= 8;
		if (a < (_screenWidth / 2))
			a = (_screenWidth / 2);
		if (b < (_screenWidth / 2))
			b = (_screenWidth / 2);
		if (a > _roomWidth - (_screenWidth / 2))
			a = _roomWidth - (_screenWidth / 2);
		if (b > _roomWidth - (_screenWidth / 2))
			b = _roomWidth - (_screenWidth / 2);
		VAR(VAR_CAMERA_MIN_X) = a;
		VAR(VAR_CAMERA_MAX_X) = b;
		break;
	case 2:			// SO_ROOM_COLOR
		if (_game.version == 1) {
			// V1 zak needs to know when room color is changed
			_roomPalette[0] = 255;
			_roomPalette[1] = a;
			_roomPalette[2] = b;
		} else {
			_roomPalette[b] = a;
		}
		_fullRedraw = true;
		break;
	default:
		break;
	}
}

void ScummEngine_v2::o2_cutscene() {
	vm.cutSceneData[0] = _userState | (_userPut ? 16 : 0);
	vm.cutSceneData[1] = (int16)VAR(VAR_CURSORSTATE);
	vm.cutSceneData[2] = _currentRoom;
	vm.cutSceneData[3] = camera._mode;

	VAR(VAR_CURSORSTATE) = 200;

	// Hide inventory, freeze scripts, hide cursor
	setUserState(USERSTATE_SET_IFACE |
		USERSTATE_SET_CURSOR |
		USERSTATE_SET_FREEZE | USERSTATE_FREEZE_ON);

	_sentenceNum = 0;
	stopScript(SENTENCE_SCRIPT);
	resetSentence();

	vm.cutScenePtr[0] = 0;
}

void ScummEngine_v2::o2_endCutscene() {
	vm.cutSceneStackPointer = 0;

	VAR(VAR_OVERRIDE) = 0;
	vm.cutSceneScript[0] = 0;
	vm.cutScenePtr[0] = 0;

	VAR(VAR_CURSORSTATE) = vm.cutSceneData[1];

	// Reset user state to values before cutscene
	setUserState(vm.cutSceneData[0] | USERSTATE_SET_IFACE | USERSTATE_SET_CURSOR | USERSTATE_SET_FREEZE);

	if (_game.id == GID_MANIAC && _game.version < 2 && _game.platform != Common::kPlatformNES) {
		camera._mode = (byte) vm.cutSceneData[3];
		if (camera._mode == kFollowActorCameraMode) {
			actorFollowCamera(VAR(VAR_EGO));
		} else if (vm.cutSceneData[2] != _currentRoom) {
			startScene(vm.cutSceneData[2], nullptr, 0);
		}
	} else {
		actorFollowCamera(VAR(VAR_EGO));
	}
}

void ScummEngine_v2::o2_beginOverride() {
	vm.cutScenePtr[0] = _scriptPointer - _scriptOrgPointer;
	vm.cutSceneScript[0] = _currentScript;

	// Skip the jump instruction following the override instruction
	fetchScriptByte();
	ScummEngine::fetchScriptWord();
}

void ScummEngine_v2::o2_chainScript() {
	int script = getVarOrDirectByte(PARAM_1);
	stopScript(vm.slot[_currentScript].number);
	_currentScript = 0xFF;
	runScript(script, 0, 0, nullptr);
}

void ScummEngine_v2::o2_pickupObject() {
	int obj = getVarOrDirectWord(PARAM_1);

	if (obj < 1) {
		error("pickupObject received invalid index %d (script %d)", obj, vm.slot[_currentScript].number);
	}

	if (getObjectIndex(obj) == -1)
		return;

	// Don't take an object twice
	if (whereIsObject(obj) == WIO_INVENTORY)
		return;

	addObjectToInventory(obj, _roomResource);
	markObjectRectAsDirty(obj);
	putOwner(obj, VAR(VAR_EGO));
	putState(obj, getState(obj) | kObjectStateIntrinsic | kObjectStateUntouchable);
	clearDrawObjectQueue();

	runInventoryScript(1);
	if (_game.platform == Common::kPlatformNES)
		_sound->triggerSound(51); // play 'pickup' sound (not using the queue; see Trac#2536)
}

void ScummEngine_v2::o2_cursorCommand() {	// TODO: Define the magic numbers
	uint16 cmd = getVarOrDirectWord(PARAM_1);
	byte state = cmd >> 8;

	if (cmd & 0xFF) {
		VAR(VAR_CURSORSTATE) = cmd & 0xFF;
	}

	setUserState(state);
}

void ScummEngine_v2::setUserState(byte state) {
	if (state & USERSTATE_SET_IFACE) {			// Userface
		if (_game.platform == Common::kPlatformNES)
			_userState = (_userState & ~USERSTATE_IFACE_ALL) | (state & USERSTATE_IFACE_ALL);
		else
			_userState = state & USERSTATE_IFACE_ALL;
	}

	if (state & USERSTATE_SET_FREEZE) {		// Freeze
		if (state & USERSTATE_FREEZE_ON)
			freezeScripts(0);
		else
			unfreezeScripts();
	}

	if (state & USERSTATE_SET_CURSOR) {			// Cursor Show/Hide
		if (_game.platform == Common::kPlatformNES)
			_userState = (_userState & ~USERSTATE_CURSOR_ON) | (state & USERSTATE_CURSOR_ON);
		if (state & USERSTATE_CURSOR_ON) {
			_userPut = 1;
			_cursor.state = 1;
		} else {
			_userPut = 0;
			_cursor.state = 0;
		}
	}

	// Hide all verbs and inventory
	Common::Rect rect;
	rect.top = _virtscr[kVerbVirtScreen].topline;
	rect.bottom = _virtscr[kVerbVirtScreen].topline + 8 * 88;
	rect.right = _virtscr[kVerbVirtScreen].w - 1;
	if (_game.platform == Common::kPlatformNES) {
		rect.left = 16;
	} else {
		rect.left = 0;
	}
	restoreBackground(rect);

	// Draw all verbs and inventory
	redrawVerbs();
	runInventoryScriptEx(1);
}

void ScummEngine_v2::o2_getActorWalkBox() {
	Actor *a;
	getResultPos();
	a = derefActor(getVarOrDirectByte(PARAM_1), "o2_getActorWalkbox");
	setResult(a->isInCurrentRoom() ? a->_walkbox: 0xFF);
}

void ScummEngine_v2::o2_dummy() {
	// Opcode 0xEE is used in maniac and zak but has no purpose
	if (_opcode != 0xEE)
		warning("o2_dummy invoked (opcode %d)", _opcode);
}

void ScummEngine_v2::o2_switchCostumeSet() {
	// NES version of maniac uses this to switch between the two
	// groups of costumes it has
	if (_game.platform == Common::kPlatformNES)
		NES_loadCostumeSet(fetchScriptByte());
	else if (_game.platform == Common::kPlatformC64)
		fetchScriptByte();
	else
		o2_dummy();
}

void ScummEngine_v2::resetSentence() {
	VAR(VAR_SENTENCE_VERB) = VAR(VAR_BACKUP_VERB);
	VAR(VAR_SENTENCE_OBJECT1) = 0;
	VAR(VAR_SENTENCE_OBJECT2) = 0;
	VAR(VAR_SENTENCE_PREPOSITION) = 0;
}

void ScummEngine_v2::runInventoryScript(int) {
	redrawV2Inventory();
}

void ScummEngine_v2::runInventoryScriptEx(int) {
	redrawV2Inventory();
	if (_game.version > 0)
		drawSentence();
}

} // End of namespace Scumm
