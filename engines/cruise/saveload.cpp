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

#include "cruise/cruise_main.h"
#include "cruise/cruise.h"
#include "cruise/vars.h"

#include "common/serializer.h"
#include "common/savefile.h"
#include "common/system.h"
#include "common/textconsole.h"

#include "graphics/scaler.h"
#include "graphics/thumbnail.h"
#include "common/list.h"
#include "common/list_intern.h"

namespace Cruise {

struct overlayRestoreTemporary {
	int _sBssSize;
	uint8* _pBss;
	int _sNumObj;
	objectParams* _pObj;
};

overlayRestoreTemporary ovlRestoreData[90];

bool readSavegameHeader(Common::InSaveFile *in, CruiseSavegameHeader &header) {
	char saveIdentBuffer[6];
	header.thumbnail = NULL;

	// Validate the header Id
	in->read(saveIdentBuffer, 6);
	if (strcmp(saveIdentBuffer, "SVMCR"))
		return false;

	header.version = in->readByte();
	if (header.version != CRUISE_SAVEGAME_VERSION)
		return false;

	// Read in the string
	header.saveName.clear();
	char ch;
	while ((ch = (char)in->readByte()) != '\0') header.saveName += ch;

	// Get the thumbnail
	header.thumbnail = Graphics::loadThumbnail(*in);
	if (!header.thumbnail)
		return false;

	return true;
}

void writeSavegameHeader(Common::OutSaveFile *out, CruiseSavegameHeader &header) {
	// Write out a savegame header
	char saveIdentBuffer[6];
	strcpy(saveIdentBuffer, "SVMCR");
	out->write(saveIdentBuffer, 6);

	out->writeByte(CRUISE_SAVEGAME_VERSION);

	// Write savegame name
	out->write(header.saveName.c_str(), header.saveName.size() + 1);

	// Create a thumbnail and save it
	Graphics::Surface *thumb = new Graphics::Surface();
	::createThumbnail(thumb, globalScreen, 320, 200, workpal);
	Graphics::saveThumbnail(*out, *thumb);
	thumb->free();
	delete thumb;
}

static void syncPalette(Common::Serializer &s, uint8 *p) {
	// This is different from the original, where palette entries are 2 bytes each
	s.syncBytes(p, NBCOLORS * 3);
}

static void syncBasicInfo(Common::Serializer &s) {
	s.syncAsSint16LE(activeMouse);
	s.syncAsSint16LE(userEnabled);
	s.syncAsSint16LE(dialogueEnabled);
	s.syncAsSint16LE(dialogueOvl);
	s.syncAsSint16LE(dialogueObj);
	s.syncAsSint16LE(userDelay);
	s.syncAsSint16LE(sysKey);
	s.syncAsSint16LE(sysX);
	s.syncAsSint16LE(sysY);
	s.syncAsSint16LE(automoveInc);
	s.syncAsSint16LE(automoveMax);
	s.syncAsSint16LE(displayOn);
	s.syncAsSint16LE(isMessage);
	s.syncAsSint16LE(fadeFlag);
	s.syncAsSint16LE(automaticMode);
	s.syncAsSint16LE(titleColor);
	s.syncAsSint16LE(itemColor);
	s.syncAsSint16LE(selectColor);
	s.syncAsSint16LE(subColor);
	s.syncAsSint16LE(narratorOvl);
	s.syncAsSint16LE(narratorIdx);
	s.syncAsSint16LE(aniX);
	s.syncAsSint16LE(aniY);
	s.syncAsUint16LE(animationStart);
	s.syncAsSint16LE(masterScreen);
	s.syncAsSint16LE(switchPal);
	s.syncAsSint16LE(scroll);
	s.syncAsSint16LE(fadeFlag);
	s.syncAsSint16LE(doFade);
	s.syncAsSint16LE(numOfLoadedOverlay);
	s.syncAsSint16LE(stateID);
	s.syncAsSint16LE(fontFileIndex);
	s.syncAsSint16LE(currentActiveMenu);
	s.syncAsSint16LE(userWait);
	s.syncAsSint16LE(autoOvl);
	s.syncAsSint16LE(autoMsg);
	s.syncAsSint16LE(autoTrack);
	s.syncAsSint16LE(var39);
	s.syncAsSint16LE(var42);
	s.syncAsSint16LE(var45);
	s.syncAsSint16LE(var46);
	s.syncAsSint16LE(var47);
	s.syncAsSint16LE(var48);
	s.syncAsSint16LE(flagCt);
	s.syncAsSint16LE(var41);
	s.syncAsSint16LE(playerMenuEnabled);
	s.syncAsSint16LE(protectionCode);
}

static void syncBackgroundTable(Common::Serializer &s) {
	// restore backgroundTable
	for (int i = 0; i < 8; i++) {
		if (s.isSaving() && (strlen(backgrounds[i]._backgroundTable.name) > 8))
			warning("Saving a background resource that has too long a name");

		s.syncBytes((byte *)backgrounds[i]._backgroundTable.name, 9);
		s.syncBytes((byte *)backgrounds[i]._backgroundTable.extention, 6);
	}
}

static void syncPalScreen(Common::Serializer &s) {
	for (int i = 0; i < NBSCREENS; ++i) {
		for (int j = 0; j < NBCOLORS; ++j)
			s.syncAsUint16LE(palScreen[i][j]);
	}
}

static void syncSoundList(Common::Serializer &s) {
	for (int i = 0; i < 4; ++i) {
		SoundEntry &se = soundList[i];
		s.syncAsSint16LE(se.frameNum);
		s.syncAsUint16LE(se.frequency);
		s.syncAsSint16LE(se.volume);
	}
}

static void syncFilesDatabase(Common::Serializer &s) {
	uint8 dummyVal = 0;
	uint32 tmp;

	for (int i = 0; i < NUM_FILE_ENTRIES; i++) {
		dataFileEntry &fe = filesDatabase[i];

		s.syncAsUint16LE(fe.widthInColumn);
		s.syncAsUint16LE(fe.width);
		s.syncAsUint16LE(fe.resType);
		s.syncAsUint16LE(fe.height);

		// TODO: Have a look at the saving/loading of this pointer
		tmp = (fe.subData.ptr) ? 1 : 0;
		s.syncAsUint32LE(tmp);
		if (s.isLoading()) {
			fe.subData.ptr = (uint8 *)tmp;
		}

		s.syncAsSint16LE(fe.subData.index);
		s.syncBytes((byte *)fe.subData.name, 13);
		s.syncAsByte(dummyVal);

		s.syncAsSint16LE(fe.subData.transparency);

		// TODO: Have a look at the saving/loading of this pointer
		tmp = (fe.subData.ptrMask) ? 1 : 0;
		s.syncAsUint32LE(tmp);
		if (s.isLoading()) {
			fe.subData.ptrMask = (uint8 *)tmp;
		}

		s.syncAsUint16LE(fe.subData.resourceType);
		s.syncAsSint16LE(fe.subData.compression);
	}
}

static void syncPreloadData(Common::Serializer &s) {
	uint8 dummyByte = 0;
	uint32 dummyLong = 0;

	for (int i = 0; i < 64; i++) {
		preloadStruct &pe = preloadData[i];

		s.syncBytes((byte *)pe.name, 15);
		s.syncAsByte(dummyByte);
		s.syncAsUint32LE(pe.size);
		s.syncAsUint32LE(pe.sourceSize);
		s.syncAsUint32LE(dummyLong);
		s.syncAsUint16LE(pe.nofree);
		s.syncAsUint16LE(pe.protect);
		s.syncAsUint16LE(pe.ovl);
	}
}

static void syncOverlays1(Common::Serializer &s) {
	uint8 dummyByte = 0;
	uint32 dummyLong = 0;

	for (int i = 0; i < numOfLoadedOverlay; i++) {
		overlayStruct &oe = overlayTable[i];

		s.syncBytes((byte *)oe.overlayName, 13);
		s.syncAsByte(dummyByte);
		s.syncAsUint32LE(dummyLong);
		s.syncAsUint16LE(oe.alreadyLoaded);
		s.syncAsUint16LE(oe.state);
		s.syncAsUint32LE(dummyLong);
		s.syncAsUint32LE(dummyLong);
		s.syncAsUint32LE(dummyLong);
		s.syncAsUint32LE(dummyLong);
		s.syncAsUint16LE(oe.executeScripts);
	}
}

static void syncOverlays2(Common::Serializer &s) {

	for (int i = 1; i < numOfLoadedOverlay; i++) {

		if (s.isSaving()) {
			// Saving code
			if (!overlayTable[i].alreadyLoaded)
				continue;

			ovlDataStruct *ovlData = overlayTable[i].ovlData;

			// save BSS
			s.syncAsSint16LE(ovlData->sizeOfData4);
			if (ovlData->sizeOfData4)
				s.syncBytes(ovlData->data4Ptr, ovlData->sizeOfData4);

			// save variables
			s.syncAsSint16LE(ovlData->size9);
			for (int j = 0; j < ovlData->size9; j++) {
				s.syncAsSint16LE(ovlData->arrayObjVar[j].X);
				s.syncAsSint16LE(ovlData->arrayObjVar[j].Y);
				s.syncAsSint16LE(ovlData->arrayObjVar[j].Z);
				s.syncAsSint16LE(ovlData->arrayObjVar[j].frame);
				s.syncAsSint16LE(ovlData->arrayObjVar[j].scale);
				s.syncAsSint16LE(ovlData->arrayObjVar[j].state);
			}
		} else {
			// Loading code
			ovlRestoreData[i]._sBssSize = ovlRestoreData[i]._sNumObj = 0;
			ovlRestoreData[i]._pBss = NULL;
			ovlRestoreData[i]._pObj = NULL;

			if (overlayTable[i].alreadyLoaded) {
				s.syncAsSint16LE(ovlRestoreData[i]._sBssSize);

				if (ovlRestoreData[i]._sBssSize) {
					ovlRestoreData[i]._pBss = (uint8 *) mallocAndZero(ovlRestoreData[i]._sBssSize);
					ASSERT(ovlRestoreData[i]._pBss);

					s.syncBytes(ovlRestoreData[i]._pBss, ovlRestoreData[i]._sBssSize);
				}

				s.syncAsSint16LE(ovlRestoreData[i]._sNumObj);

				if (ovlRestoreData[i]._sNumObj) {
					ovlRestoreData[i]._pObj = (objectParams *) mallocAndZero(ovlRestoreData[i]._sNumObj * sizeof(objectParams));
					ASSERT(ovlRestoreData[i]._pObj);

					for (int j = 0; j < ovlRestoreData[i]._sNumObj; j++) {
						s.syncAsSint16LE(ovlRestoreData[i]._pObj[j].X);
						s.syncAsSint16LE(ovlRestoreData[i]._pObj[j].Y);
						s.syncAsSint16LE(ovlRestoreData[i]._pObj[j].Z);
						s.syncAsSint16LE(ovlRestoreData[i]._pObj[j].frame);
						s.syncAsSint16LE(ovlRestoreData[i]._pObj[j].scale);
						s.syncAsSint16LE(ovlRestoreData[i]._pObj[j].state);
					}
				}
			}
		}
	}
}

void syncScript(Common::Serializer &s, ScriptList *entry) {
	int numScripts = 0;
	uint32 dummyLong = 0;
	uint16 dummyWord = 0;

	if (s.isSaving()) {
		// Figure out the number of scripts to save
		numScripts = entry->size();
	}
	s.syncAsSint16LE(numScripts);

	ScriptInstance ptr;
	Common::List<ScriptInstance>::iterator iter = entry->begin();
	for (int i = 0; i < numScripts; ++iter, ++i) {
		if (s.isSaving())
			ptr = *iter;

		s.syncAsUint16LE(dummyWord);
		s.syncAsSint16LE(ptr._ccr);
		s.syncAsSint16LE(ptr._scriptOffset);
		s.syncAsUint32LE(dummyLong);
		s.syncAsSint16LE(ptr._dataSize);
		s.syncAsSint16LE(ptr._scriptNumber);
		s.syncAsSint16LE(ptr._overlayNumber);
		s.syncAsSint16LE(ptr._sysKey);
		s.syncAsSint16LE(ptr._freeze);
		s.syncAsSint16LE(ptr._type);
		s.syncAsSint16LE(ptr._var16);
		s.syncAsSint16LE(ptr._var18);
		s.syncAsSint16LE(ptr._var1A);

		s.syncAsSint16LE(ptr._dataSize);

		if (ptr._dataSize) {
			if (s.isLoading())
				ptr._data = (byte *)mallocAndZero(ptr._dataSize);
			s.syncBytes(ptr._data, ptr._dataSize);
		}

		if (s.isLoading()) {
			entry->add(ptr);
		} 
	}
	if (s.isLoading())
		currentScriptPtr = NULL;		//in case the load was called while a script runs.
}

static void syncCell(Common::Serializer &s) {
	int chunkCount = 0;
	CellListNode *t, *p;
	uint16 dummyWord = 0;

	if (s.isSaving()) {
		// Figure out the number of chunks to save
		t = cellHead._next;
		while (t) {
			++chunkCount;
			t = t->_next;
		}
	} else {
		cellHead._next = NULL; // Not in ASM code, but I guess the variable is defaulted in the EXE
	}
	s.syncAsSint16LE(chunkCount);

	t = s.isSaving() ? cellHead._next : &cellHead;
	for (int i = 0; i < chunkCount; ++i) {
		p = s.isSaving() ? t : new CellListNode;

		s.syncAsUint16LE(dummyWord);
		s.syncAsUint16LE(dummyWord);

		s.syncAsSint16LE(p->_cell->_idx);
		s.syncAsSint16LE(p->_cell->_type);
		s.syncAsSint16LE(p->_cell->_overlay);
		s.syncAsSint16LE(p->_cell->_X);
		s.syncAsSint16LE(p->_cell->_fieldC);
		s.syncAsSint16LE(p->_cell->_spriteIdx);
		s.syncAsSint16LE(p->_cell->_color);
		s.syncAsSint16LE(p->_cell->_backgroundPlane);
		s.syncAsSint16LE(p->_cell->_freeze);
		s.syncAsSint16LE(p->_cell->_parent);
		s.syncAsSint16LE(p->_cell->_parentOverlay);
		s.syncAsSint16LE(p->_cell->_parentType);
		s.syncAsSint16LE(p->_cell->_followObjectOverlayIdx);
		s.syncAsSint16LE(p->_cell->_followObjectIdx);
		s.syncAsSint16LE(p->_cell->_animStart);
		s.syncAsSint16LE(p->_cell->_animEnd);
		s.syncAsSint16LE(p->_cell->_animWait);
		s.syncAsSint16LE(p->_cell->_animStep);
		s.syncAsSint16LE(p->_cell->_animChange);
		s.syncAsSint16LE(p->_cell->_animType);
		s.syncAsSint16LE(p->_cell->_animSignal);
		s.syncAsSint16LE(p->_cell->_animCounter);
		s.syncAsSint16LE(p->_cell->_animLoop);
		s.syncAsUint16LE(dummyWord);

		if (s.isSaving())
			t = t->_next;
		else {
			p->_next = NULL;
			t->_next = p;
			p->_prev = cellHead._prev;
			cellHead._prev = p;
			t = p;
		}
	}
}

static void syncIncrust(Common::Serializer &s) {
	int numEntries = 0;
	BackgroundIncrustListNode *pl, *pl1;
	uint8 dummyByte = 0;
	uint16 dummyWord = 0;
	uint32 dummyLong = 0;

	if (s.isSaving()) {
		// Figure out the number of entries to save
		pl = backgroundIncrustListHead.next;
		while (pl) {
			++numEntries;
			pl = pl->next;
		}
	}
	s.syncAsSint16LE(numEntries);

	pl = s.isSaving() ? backgroundIncrustListHead.next : &backgroundIncrustListHead;
	pl1 = &backgroundIncrustListHead;

	for (int i = 0; i < numEntries; ++i) {
		BackgroundIncrustListNode *t = s.isSaving() ? pl :
			new BackgroundIncrustListNode;
		BackgroundIncrust *backgroundIncrust = t->backgroundIncrust;
		s.syncAsUint32LE(dummyLong);

		s.syncAsSint16LE(backgroundIncrust->_objectIdx);
		s.syncAsSint16LE(backgroundIncrust->_type);
		s.syncAsSint16LE(backgroundIncrust->_overlayIdx);
		s.syncAsSint16LE(backgroundIncrust->_X);
		s.syncAsSint16LE(backgroundIncrust->_Y);
		s.syncAsSint16LE(backgroundIncrust->_frame);
		s.syncAsSint16LE(backgroundIncrust->_scale);
		s.syncAsSint16LE(backgroundIncrust->_backgroundIdx);
		s.syncAsSint16LE(backgroundIncrust->_scriptNumber);
		s.syncAsSint16LE(backgroundIncrust->_scriptOverlayIdx);
		s.syncAsUint32LE(dummyLong);
		s.syncAsSint16LE(backgroundIncrust->_saveWidth);
		s.syncAsSint16LE(backgroundIncrust->_saveHeight);
		s.syncAsSint16LE(backgroundIncrust->_saveSize);
		s.syncAsSint16LE(backgroundIncrust->_savedX);
		s.syncAsSint16LE(backgroundIncrust->_savedY);
		s.syncBytes((byte *)backgroundIncrust->_name, 13);
		s.syncAsByte(dummyByte);
		s.syncAsSint16LE(backgroundIncrust->_spriteId);
		s.syncAsUint16LE(dummyWord);

		if (backgroundIncrust->_saveSize) {
			if (s.isLoading())
				backgroundIncrust->_ptr = (byte *)MemAlloc(backgroundIncrust->_saveSize);

			s.syncBytes(backgroundIncrust->_ptr, backgroundIncrust->_saveSize);
		}

		if (s.isSaving())
			pl = pl->next;
		else {
			t->next = NULL;
			pl->next = t;
			t->prev = pl1->prev;
			pl1->prev = t;
			pl = t;
		}
	}
}

static void syncActors(Common::Serializer &s) {
	int numEntries = 0;
	uint16 dummyLong = 0;

	if (s.isSaving()) {
		numEntries = actorHead.size();
	}
	s.syncAsSint16LE(numEntries);

	Common::List<Actor>::iterator iter = actorHead.begin();
	for (int i = 0; i < numEntries; ++i) {
		Actor *pActor;
		if(s.isSaving()){
		    pActor = &(*iter);
		} else {
		    pActor = new Actor;
		}

		s.syncAsUint32LE(dummyLong);
		s.syncAsSint16LE(pActor->_idx);
		s.syncAsSint16LE(pActor->_type);
		s.syncAsSint16LE(pActor->_overlayNumber);
		s.syncAsSint16LE(pActor->_xDest);
		s.syncAsSint16LE(pActor->_yDest);
		s.syncAsSint16LE(pActor->_x);
		s.syncAsSint16LE(pActor->_y);
		s.syncAsSint16LE(pActor->_startDirection);
		s.syncAsSint16LE(pActor->_nextDirection);
		s.syncAsSint16LE(pActor->_endDirection);
		s.syncAsSint16LE(pActor->_stepX);
		s.syncAsSint16LE(pActor->_stepY);
		s.syncAsSint16LE(pActor->_pathId);
		s.syncAsSint16LE(pActor->_phase);
		s.syncAsSint16LE(pActor->_counter);
		s.syncAsSint16LE(pActor->_poly);
		s.syncAsSint16LE(pActor->_flag);
		s.syncAsSint16LE(pActor->_start);
		s.syncAsSint16LE(pActor->_freeze);

		if (s.isLoading())
			actorHead.add(*pActor);
		
		iter++;
	}
}

static void syncSongs(Common::Serializer &s) {
	int size = 0;

	if (songLoaded) {
		// TODO: implement
		s.syncAsByte(size);
		if (s.isLoading()) {
			saveVar1 = size;
			if (saveVar1)
				s.syncBytes(saveVar2, saveVar1);
		}
	} else {
		s.syncAsByte(size);
	}
}

static void syncPerso(Common::Serializer &s, Perso &p) {
	s.syncAsSint16LE(p.inc_droite);
	s.syncAsSint16LE(p.inc_droite0);
	s.syncAsSint16LE(p.inc_chemin);

	for (int i = 0; i < 400; ++i) {
		s.syncAsSint16LE(p.coordinates[i].x);
		s.syncAsSint16LE(p.coordinates[i].y);
	}

	for (int i = 0; i < NUM_NODES + 3; ++i) {
		s.syncAsSint16LE(p.solution[i][0]);
		s.syncAsSint16LE(p.solution[i][1]);
	}

	s.syncAsSint16LE(p.inc_jo1);
	s.syncAsSint16LE(p.inc_jo2);
	s.syncAsSint16LE(p.dir_perso);
	s.syncAsSint16LE(p.inc_jo0);
}

static void syncCT(Common::Serializer &s) {
	int v = (_vm->_polyStruct) ? 1 : 0;
	s.syncAsSint32LE(v);
	if (s.isLoading())
		_vm->_polyStruct = (v != 0) ? &_vm->_polyStructNorm : NULL;

	if (v == 0)
		// There is no further data to load or save
		return;

	s.syncAsSint16LE(WalkboxCount);

	if (WalkboxCount) {
		for (int i = 0; i < WalkboxCount; ++i)
			s.syncAsSint16LE(walkboxes[i]._color);
		for (int i = 0; i < WalkboxCount; ++i)
			s.syncAsSint16LE(walkboxes[i]._state);
	}

	for (int i = 0; i < 10; i++) {
		v = 0;
		if (s.isSaving()) v = (persoTable[i]) ? 1 : 0;
		s.syncAsSint32LE(v);

		if (s.isLoading())
			// Set up the pointer for the next structure
			persoTable[i] = (v == 0) ? NULL : new Perso();

		if (v != 0)
			syncPerso(s, *persoTable[i]);
	}
}

static void DoSync(Common::Serializer &s) {
	syncBasicInfo(s);
	_vm->sound().doSync(s);

	syncPalette(s, newPal);
	syncPalette(s, workpal);

	s.syncBytes((byte *)currentCtpName, 40);

	syncBackgroundTable(s);
	syncPalScreen(s);
	syncSoundList(s);

	for (int i = 0; i < stateID; ++i)
		s.syncAsSint16LE(globalVars[i]);

	syncFilesDatabase(s);
	syncOverlays1(s);
	syncPreloadData(s);
	syncOverlays2(s);
	syncScript(s, &procScriptList);
	syncScript(s, &relScriptList);
	syncCell(s);
	syncIncrust(s);
	syncActors(s);
	syncSongs(s);
	syncCT(s);
}


void resetPreload() {
	for (unsigned long int i = 0; i < 64; i++) {
		if (strlen(preloadData[i].name)) {
			if (preloadData[i].ptr) {
				MemFree(preloadData[i].ptr);
				preloadData[i].ptr = NULL;
			}
			strcpy(preloadData[i].name, "");
			preloadData[i].nofree = 0;
		}
	}
}

void unloadOverlay(const char*name, int overlayNumber) {
	releaseOverlay(name);

	strcpy(overlayTable[overlayNumber].overlayName, "");
	overlayTable[overlayNumber].ovlData = NULL;
	overlayTable[overlayNumber].alreadyLoaded = 0;
}

void initVars() {
	closeAllMenu();
	resetFileEntryRange(0, NUM_FILE_ENTRIES);

	resetPreload();
	freeCTP();
	backgroundIncrustListHead.freeBackgroundIncrustList();

	cellHead.freezeCell(-1, -1, -1, -1, -1, 0);
	// TODO: unfreeze anims

	freeObjectList(&cellHead);
	actorHead.remove(-1, -1, -1);

	relScriptList.removeAll();
	procScriptList.removeAll();
	procScriptList.changeParam(-1, -1, -1, 0);
	procScriptList.removeFinished();

	relScriptList.changeParam(-1, -1, -1, 0);
	relScriptList.removeFinished();

	for (unsigned long int i = 0; i < 90; i++) {
		if (strlen(overlayTable[i].overlayName) && overlayTable[i].alreadyLoaded) {
			unloadOverlay(overlayTable[i].overlayName, i);
		}
	}

	// TODO:
	// stopSound();
	// removeSound();

	closeBase();
	closeCnf();

	initOverlayTable();

	stateID = 0;
	masterScreen = 0;

	freeDisk();

	soundList[0].frameNum = -1;
	soundList[1].frameNum = -1;
	soundList[2].frameNum = -1;
	soundList[3].frameNum = -1;

	for (unsigned long int i = 0; i < 8; i++) {
		menuTable[i] = NULL;
	}

	for (unsigned long int i = 0; i < 2000; i++) {
		globalVars[i] = 0;
	}

	for (unsigned long int i = 0; i < 8; i++) {
		backgrounds[i]._backgroundTable.name[0] = 0;
	}

	for (unsigned long int i = 0; i < NUM_FILE_ENTRIES; i++) {
		filesDatabase[i].subData.ptr = NULL;
		filesDatabase[i].subData.ptrMask = NULL;
	}

	initBigVar3();

	procScriptList.resetPtr2();
	relScriptList.resetPtr2();

	cellHead.resetPtr();

	actorHead.clear();
	backgroundIncrustListHead.resetBackgroundIncrustList();

	vblLimit = 0;
	remdo = 0;
	songLoaded = 0;
	songPlayed = 0;
	songLoop = 1;
	activeMouse = 0;
	userEnabled = 1;
	dialogueEnabled = 0;
	dialogueOvl = 0;
	dialogueObj = 0;
	userDelay = 0;
	sysKey = -1;
	sysX = 0;
	sysY = 0;
	automoveInc = 0;
	automoveMax = 0;
	displayOn = true;

	// here used to init clip

	isMessage = 0;
	fadeFlag = 0;
	automaticMode = 0;

	// video param (vga and mcga mode)

	titleColor = 2;
	itemColor = 1;
	selectColor = 3;
	subColor = 5;

	//

	narratorOvl = 0;
	narratorIdx = 0;
	aniX = 0;
	aniY = 0;
	animationStart = false;
	selectDown = 0;
	menuDown = 0;
	buttonDown = 0;
	var41 = 0;
	playerMenuEnabled = 0;
	PCFadeFlag = 0;
}

Common::Error saveSavegameData(int saveGameIdx, const Common::String &saveName) {
	const char *filename = _vm->getSavegameFile(saveGameIdx);
	Common::SaveFileManager *saveMan = g_system->getSavefileManager();
	Common::OutSaveFile *f = saveMan->openForSaving(filename);
	if (f == NULL)
		return Common::kNoGameDataFoundError;

	// Save the savegame header
	CruiseSavegameHeader header;
	header.saveName = saveName;
	writeSavegameHeader(f, header);

	if (f->err()) {
		delete f;
		saveMan->removeSavefile(filename);
		return Common::kWritingFailed;
	} else {
		// Create the remainder of the savegame
		Common::Serializer s(NULL, f);
		DoSync(s);

		f->finalize();
		delete f;
		return Common::kNoError;
	}
}

Common::Error loadSavegameData(int saveGameIdx) {
	int lowMemorySave;
	Common::String saveName;
	CellListNode *currentcellHead;

	Common::SaveFileManager *saveMan = g_system->getSavefileManager();
	Common::InSaveFile *f = saveMan->openForLoading(_vm->getSavegameFile(saveGameIdx));

	if (f == NULL) {
		printInfoBlackBox("Savegame not found...");
		waitForPlayerInput();
		return Common::kNoGameDataFoundError;
	}

	printInfoBlackBox("Loading in progress...");

	initVars();
	_vm->sound().stopMusic();

	// Skip over the savegame header
	CruiseSavegameHeader header;
	readSavegameHeader(f, header);
	delete header.thumbnail;

	// Synchronise the remaining data of the savegame
	Common::Serializer s(f, NULL);
	DoSync(s);

	delete f;

	// Post processing

	for (int j = 0; j < 64; j++)
		preloadData[j].ptr = NULL;

	for (int j = 1; j < numOfLoadedOverlay; j++) {
		if (overlayTable[j].alreadyLoaded) {
			overlayTable[j].alreadyLoaded = 0;
			loadOverlay(overlayTable[j].overlayName);

			if (overlayTable[j].alreadyLoaded) {
				ovlDataStruct *ovlData = overlayTable[j].ovlData;

				// overlay BSS

				if (ovlRestoreData[j]._sBssSize) {
					if (ovlData->data4Ptr) {
						MemFree(ovlData->data4Ptr);
					}

					ovlData->data4Ptr = ovlRestoreData[j]._pBss;
					ovlData->sizeOfData4 = ovlRestoreData[j]._sBssSize;
				}

				// overlay object data

				if (ovlRestoreData[j]._sNumObj) {
					if (ovlData->arrayObjVar) {
						MemFree(ovlData->arrayObjVar);
					}

					ovlData->arrayObjVar = ovlRestoreData[j]._pObj;
					ovlData->size9 = ovlRestoreData[j]._sNumObj;
				}

			}
		}
	}

	updateAllScriptsImports();

	lastAni[0] = 0;

	lowMemorySave = lowMemory;

	for (int i = 0; i < NUM_FILE_ENTRIES; i++) {
		if (filesDatabase[i].subData.ptr) {
			int j;
			int k;

			for (j = i + 1; j < NUM_FILE_ENTRIES && filesDatabase[j].subData.ptr && !strcmp(filesDatabase[i].subData.name, filesDatabase[j].subData.name) && (filesDatabase[j].subData.index == (j - i)); j++)
				;

			for (k = i; k < j; k++) {
				if (filesDatabase[k].subData.ptrMask)
					lowMemory = 0;

				filesDatabase[k].subData.ptr = NULL;
				filesDatabase[k].subData.ptrMask = NULL;
			}

			/*if (j < 2) {
				error("Unsupported mono file load");
				//loadFileMode1(filesDatabase[j].subData.name,filesDatabase[j].subData.var4);
			} else */
			if (strlen(filesDatabase[i].subData.name) > 0) {
				loadFileRange(filesDatabase[i].subData.name, filesDatabase[i].subData.index, i, j - i);
			} else {
				filesDatabase[i].subData.ptr = NULL;
				filesDatabase[i].subData.ptrMask = NULL;
			}

			i = j - 1;
			lowMemory = lowMemorySave;
		}
	}

	lastAni[0] = 0;

	currentcellHead = cellHead._next;

	while (currentcellHead) {
		if (currentcellHead->_cell->_type == 5) {
			uint8 *ptr = mainProc14(currentcellHead->_cell->_overlay, currentcellHead->_cell->_idx);

			ASSERT(0);

			if (ptr) {
				ASSERT(0);
				//*(int16 *)(currentcellHead->datas+0x2E) = getSprite(ptr,*(int16 *)(currentcellHead->datas+0xE));
			} else {
				ASSERT(0);
				//*(int16 *)(currentcellHead->datas+0x2E) = 0;
			}
		}

		currentcellHead = currentcellHead->_next;
	}

	if (strlen(currentCtpName)) {
		initCt(currentCtpName, true);

	}
	//prepareFadeOut();
	//gfxModuleData.gfxFunction8();

	for (int j = 0; j < 8; j++) {
		if (strlen((char *)backgrounds[j]._backgroundTable.name)) {
			loadBackground(backgrounds[j]._backgroundTable.name, j);
		}
	}

	backgroundIncrustListHead.regenerateBackgroundIncrustList();

	// to finish

	currentMouse.changeCursor(CURSOR_NORMAL);
	mainDraw(1);
	flipScreen();

	return Common::kNoError;
}

} // End of namespace Cruise
