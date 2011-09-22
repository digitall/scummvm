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
 * This is a utility for storing all the hardcoded data tables of 
 * Beneath A Steel Sky in a separate data file to be loaded by the game engine.
 * This avoids large binary sizes due to these static tables.
 */

// Disable symbol overrides so that we can use system headers.
#define FORBIDDEN_SYMBOL_ALLOW_ALL

// HACK to allow building with the SDL backend on MinGW
// see bug #1800764 "TOOLS: MinGW tools building broken"
#ifdef main
#undef main
#endif // main

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/scummsys.h"
#include "common/events.h"

enum CptTypeIds {
	CPT_NULL = 0,
	COMPACT,
	TURNTAB,
	ANIMSEQ,
	MISCBIN,
	GETTOTAB,
	ROUTEBUF,
	MAINLIST
};

#include "util.h"
#include "skydefs.h"
#include "struc.h"

extern Compact mini_so;
extern uint16 rs_foster_4_2[];
extern Compact junk1;
extern Compact loader;
extern uint16 sc30_joey_list[];
extern void *data_4[];
extern uint16 sc31_joey_list[];
extern uint16 sc32_joey_list[];
extern uint16 sc33_joey_list[];
extern uint16 sc36_fast_list[];
extern Compact danielle;
extern Compact spunky;
extern uint16 sc39_fast_list[];
extern Compact shades;
extern uint16 sc40_fast_list[];
extern uint16 sc41_fast_list[];
extern uint16 sc44_fast_list[];
extern uint16 sc46_fast_list[];
extern uint16 sc47_fast_list[];
extern Compact r_talk_s4;
extern void *data_2[];
extern void *data_5[];
extern Compact witness;
extern Compact foster;
extern void *data_0[];

#include "compacts/0compact.h"
#include "compacts/1compact.h"
#include "compacts/29comp.h"
#include "compacts/2compact.h"
#include "compacts/3compact.h"
#include "compacts/30comp.h"
#include "compacts/4compact.h"
#include "compacts/5compact.h"
#include "compacts/66comp.h"
#include "compacts/90comp.h"
#include "compacts/9compact.h"
#include "compacts/linc_gen.h"
#include "compacts/lincmenu.h"
#include "compacts/z_compac.h"
#include "compacts/savedata.h"
#include "talks.h"

#include "create_sky.h"

/*void writeTextArray(FILE *outFile, const char *textArray[], int nbrText) {
	int len, len1, pad;
	uint8 padBuf[DATAALIGNMENT];

	for (int i = 0; i < DATAALIGNMENT; i++)
		padBuf[i] = 0;

	writeUint16BE(outFile, nbrText);
	len = DATAALIGNMENT - 2;
	for (int i = 0; i < nbrText; i++) {
		len1 = strlen(textArray[i]) + 1;
		pad = DATAALIGNMENT - (len1 + 2) % DATAALIGNMENT;
		len += 2 + len1 + pad;
	}
	writeUint16BE(outFile, len);

	fwrite(padBuf, DATAALIGNMENT - 2, 1, outFile); // padding
	for (int i = 0; i < nbrText; i++) {
		len = strlen(textArray[i]) + 1;
		pad = DATAALIGNMENT - (len + 2) % DATAALIGNMENT;

		writeUint16BE(outFile, len + pad + 2);
		fwrite(textArray[i], len, 1, outFile);
		fwrite(padBuf, pad, 1, outFile);
	}
}
*/

int main(int argc, char *argv[]) {
	FILE *outFile;

	outFile = fopen("sky.cpt", "wb");

	uint16 fileVersion = SKY_DAT_VER;
	writeUint16LE(outFile, fileVersion);

	const uint16 _dataListLen[9] = { 1046, 272, 353, 412, 487, 608, 257, 198, 2 };
	const void *_dataListPointer[9] = { data_0, data_1, data_2, data_3, data_4, data_5, data_6, animTalkTablePtr, move_list };
	const uint16 _numDataLists = ARRAYSIZE(_dataListLen);

	writeUint16LE(outFile, _numDataLists);
	for (int i = 0; i < _numDataLists; i++)
		writeUint16LE(outFile, _dataListLen[i]);

	uint32 rawSize = 148603; // In units of uint16 i.e. total size in bytes / sizeof(uint16)
	writeUint32LE(outFile, rawSize);

	uint32 srcSize = 155496; // In units of uint16 i.e. total size in bytes / sizeof(uint16)
	writeUint32LE(outFile, srcSize);
	
	// and fill them with the compact data
	// Temp - Fill With Zeros
	for (uint32 i = 0; i < srcSize; i++)
		writeUint16LE(outFile, 0x0000);
	/*for (uint32 lcnt = 0; lcnt < _numDataLists; lcnt++) {
		for (uint32 ecnt = 0; ecnt < _dataListLen[lcnt]; ecnt++) {
			writeUint16LE(outFile, _cptSizes[lcnt][ecnt]);
			if (_cptSizes[lcnt][ecnt]) {
				writeUint16LE(outFile, _cptTypes[lcnt][ecnt]);
				for (uint16 elemCnt = 0; elemCnt < _cptSizes[lcnt][ecnt]; elemCnt++)
					writeUint16LE(outFile, 0x0000); // Elements of Compact
			}
		}
	}*/

	uint32 asciiSize = 42395;
	writeUint32LE(outFile, asciiSize);
/*
	for (uint32 lcnt = 0; lcnt < _numDataLists; lcnt++) {
		for (uint32 ecnt = 0; ecnt < _dataListLen[lcnt]; ecnt++) {
			if (_cptSizes[lcnt][ecnt]) {
				fwrite(_cptNames[lcnt][ecnt], strlen(_cptNames[lcnt][ecnt]), 1, outFile);
			}
		}
	}

	// these compacts don't actually exist but only point to other ones...
	uint16 numDlincs = XXX;
	writeUint16LE(outFile, numDlincs);
	for (uint16 cnt = 0; cnt < numDlincs; cnt++) {
		uint16 dlincId = XXX;
		uint16 destId = XXX;

		writeUint16LE(outFile, dlincId);
		writeUint16LE(outFile, destId);

		//assert(((dlincId >> 12) < _numDataLists) && ((dlincId & 0xFFF) < _dataListLen[dlincId >> 12]) && (_compacts[dlincId >> 12][dlincId & 0xFFF] == NULL));
		//_compacts[dlincId >> 12][dlincId & 0xFFF] = _compacts[destId >> 12][destId & 0xFFF];

		//assert(_cptNames[dlincId >> 12][dlincId & 0xFFF] == NULL);
		//_cptNames[dlincId >> 12][dlincId & 0xFFF] = asciiPos;
		//asciiPos += strlen(asciiPos) + 1;
	}

	// Diff Data (Reset?)
	uint16 numDiffs = XXX;
	writeUint16LE(outFile, numDiffs);
	uint16 diffSize = XXX;
	writeUint16LE(outFile, diffSize); // In units of uint16 i.e. total size in bytes / sizeof(uint16)
	for (uint16 cnt = 0; cnt < numDiffs; cnt++) {
		uint16 cptId = XXX;
		writeUint16LE(outFile, cptId);
		uint16 cptOffset = XXX;
		writeUint16LE(outFile, cptOffset);
		uint16 len = XXX;
		writeUint16LE(outFile, len);
		for (uint16 elemCnt = 0; elemCnt < len; elemCnt++)
			writeUint16LE(outFile, rawCpt[elemCnt]);
	}

	// these are the IDs that have to be saved into savegame files.
	uint16 _numSaveIds = XXX;
	writeUint16LE(outFile, _numSaveIds);
	for (uint16 cnt = 0; cnt < _numSaveIds; cnt++)
		writeUint16LE(outFile, _saveIds[cnt]);

	/*
	uint8 *SkyCompact::createResetData(uint16 gameVersion) {
		_cptFile->seek(_resetDataPos);
		uint32 dataSize = _cptFile->readUint16LE() * sizeof(uint16);
		uint16 *resetBuf = (uint16*)malloc(dataSize);
		_cptFile->read(resetBuf, dataSize);
		uint16 numDiffs = _cptFile->readUint16LE();
		for (uint16 cnt = 0; cnt < numDiffs; cnt++) {
			uint16 version = _cptFile->readUint16LE();
			uint16 diffFields = _cptFile->readUint16LE();
			if (version == gameVersion) {
				for (uint16 diffCnt = 0; diffCnt < diffFields; diffCnt++) {
					uint16 pos = _cptFile->readUint16LE();
					resetBuf[pos] = _cptFile->readUint16LE();
				}
				return (uint8*)resetBuf;
			} else
				_cptFile->seek(diffFields * 2 * sizeof(uint16), SEEK_CUR);
		}
		free(resetBuf);
		error("Unable to find reset data for Beneath a Steel Sky Version 0.0%03d", gameVersion);
	}
	*/

	fclose(outFile);
	return 0;
}
