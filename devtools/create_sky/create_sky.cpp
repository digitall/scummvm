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

#include "util.h"

#include "compact.h"
#include "skydefs.h"
#include "struc.h"
#include "compacts/0compact.h"
#include "compacts/1compact.h"
#include "compacts/29comp.h"
#include "compacts/2compact.h"
#include "compacts/30comp.h"
#include "compacts/3compact.h"
#include "compacts/4compact.h"
#include "compacts/5compact.h"
#include "compacts/66comp.h"
#include "compacts/90comp.h"
#include "compacts/9compact.h"
#include "compacts/linc_gen.h"
#include "compacts/lincmenu.h"
//#include "compacts/z_compac.h"
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

	const uint16 _numDataLists = 9;
	writeUint16LE(outFile, _numDataLists);

	const uint16 _dataListLen[_numDataLists] = { 1046, 272, 353, 412, 487, 608, 257, 198, 2};

	//_cptNames	  = (char***)malloc(_numDataLists * sizeof(char**));
	//_cptSizes	  = (uint16 **)malloc(_numDataLists * sizeof(uint16*));
	//_cptTypes	  = (uint16 **)malloc(_numDataLists * sizeof(uint16*));
	//_compacts	  = (Compact***)malloc(_numDataLists * sizeof(Compact**));

	for (int i = 0; i < _numDataLists; i++) {
		writeUint16LE(outFile, _dataListLen[i]);
		
		//_cptNames[i] = (char**)malloc(_dataListLen[i] * sizeof(char*));
		//_cptSizes[i] = (uint16 *)malloc(_dataListLen[i] * sizeof(uint16));
		//_cptTypes[i] = (uint16 *)malloc(_dataListLen[i] * sizeof(uint16));
		//_compacts[i] = (Compact**)malloc(_dataListLen[i] * sizeof(Compact*));
	}

	uint32 rawSize = 148603; // In units of uint16 i.e. total size in bytes / sizeof(uint16)
	writeUint32LE(outFile, rawSize);
	//uint16 *rawPos = _rawBuf = (uint16*)malloc(rawSize);

	uint32 srcSize = 155496; // In units of uint16 i.e. total size in bytes / sizeof(uint16)
	writeUint32LE(outFile, srcSize);
	
	//uint16 *srcBuf = (uint16*)malloc(srcSize);
	//uint16 *srcPos = srcBuf;
	/*
	_cptFile->read(srcBuf, srcSize);

	uint32 asciiSize = XXX;
	writeUint32LE(outFile, asciiSize);
	
	char *asciiPos = _asciiBuf = (char*)malloc(asciiSize);
	_cptFile->read(_asciiBuf, asciiSize);

	// and fill them with the compact data
	uint32 debcnt = 0;
	for (uint32 lcnt = 0; lcnt < _numDataLists; lcnt++) {
		for (uint32 ecnt = 0; ecnt < _dataListLen[lcnt]; ecnt++) {
			_cptSizes[lcnt][ecnt] = READ_LE_UINT16(srcPos++);
			if (_cptSizes[lcnt][ecnt]) {
				_cptTypes[lcnt][ecnt] = READ_LE_UINT16(srcPos++);				
				_compacts[lcnt][ecnt] = (Compact*)rawPos;
				_cptNames[lcnt][ecnt] = asciiPos;
				asciiPos += strlen(asciiPos) + 1;

				for (uint16 elemCnt = 0; elemCnt < _cptSizes[lcnt][ecnt]; elemCnt++)
					*rawPos++ = READ_LE_UINT16(srcPos++);
			} else {
				_cptTypes[lcnt][ecnt] = 0;
				_compacts[lcnt][ecnt] = 0;
				_cptNames[lcnt][ecnt] = 0;
			}
		}
	}
	free(srcBuf);

	uint16 numDlincs = _cptFile->readUint16LE();
	uint16 *dlincBuf = (uint16*)malloc(numDlincs * 2 * sizeof(uint16));
	uint16 *dlincPos = dlincBuf;
	_cptFile->read(dlincBuf, numDlincs * 2 * sizeof(uint16));
	// these compacts don't actually exist but only point to other ones...
	for (uint16 cnt = 0; cnt < numDlincs; cnt++) {
		uint16 dlincId = READ_LE_UINT16(dlincPos++);
		uint16 destId = READ_LE_UINT16(dlincPos++);
		assert(((dlincId >> 12) < _numDataLists) && ((dlincId & 0xFFF) < _dataListLen[dlincId >> 12]) && (_compacts[dlincId >> 12][dlincId & 0xFFF] == NULL));
		_compacts[dlincId >> 12][dlincId & 0xFFF] = _compacts[destId >> 12][destId & 0xFFF];

		assert(_cptNames[dlincId >> 12][dlincId & 0xFFF] == NULL);
		_cptNames[dlincId >> 12][dlincId & 0xFFF] = asciiPos;
		asciiPos += strlen(asciiPos) + 1;
	}
	free(dlincBuf);

	// if this is v0.0288, parse this diff data
	uint16 numDiffs = _cptFile->readUint16LE();
	uint16 diffSize = _cptFile->readUint16LE();
	uint16 *diffBuf = (uint16*)malloc(diffSize * sizeof(uint16));
	_cptFile->read(diffBuf, diffSize * sizeof(uint16));
	if (SkyEngine::_systemVars.gameVersion == 288) {
		uint16 *diffPos = diffBuf;
		for (uint16 cnt = 0; cnt < numDiffs; cnt++) {
			uint16 cptId = READ_LE_UINT16(diffPos++);
			uint16 *rawCpt = (uint16*)fetchCpt(cptId);
			rawCpt += READ_LE_UINT16(diffPos++);
			uint16 len = READ_LE_UINT16(diffPos++);
			for (uint16 elemCnt = 0; elemCnt < len; elemCnt++)
				rawCpt[elemCnt] = READ_LE_UINT16(diffPos++);
		}
		assert(diffPos == (diffBuf + diffSize));
	}
	free(diffBuf);

	// these are the IDs that have to be saved into savegame files.
	_numSaveIds = _cptFile->readUint16LE();
	_saveIds = (uint16*)malloc(_numSaveIds * sizeof(uint16));
	_cptFile->read(_saveIds, _numSaveIds * sizeof(uint16));
	for (uint16 cnt = 0; cnt < _numSaveIds; cnt++)
		_saveIds[cnt] = FROM_LE_16(_saveIds[cnt]);
		_resetDataPos = _cptFile->pos();
	}
	*/

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
