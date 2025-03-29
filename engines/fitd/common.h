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

#ifndef FITD_COMMON_H
#define FITD_COMMON_H

#include "common/scummsys.h"

namespace Fitd {

//////////////////

enum enumCVars {
	SAMPLE_PAGE,
	BODY_FLAMME,
	MAX_WEIGHT_LOADABLE,
	TEXTE_CREDITS,
	SAMPLE_TONNERRE,
	INTRO_DETECTIVE,
	INTRO_HERITIERE,
	WORLD_NUM_PERSO,
	CHOOSE_PERSO,
	SAMPLE_CHOC,
	SAMPLE_PLOUF,
	REVERSE_OBJECT,
	KILLED_SORCERER,
	LIGHT_OBJECT,
	FOG_FLAG,
	DEAD_PERSO,
	JET_SARBACANE,
	TIR_CANON,
	JET_SCALPEL,
	POIVRE,
	DORTOIR,
	EXT_JACK,
	NUM_MATRICE_PROTECT_1,
	NUM_MATRICE_PROTECT_2,
	NUM_PERSO,
	TYPE_INVENTAIRE,
	PROLOGUE,
	POIGNARD,
	MATRICE_FORME,
	MATRICE_COULEUR,

	UNKNOWN_CVAR // for table padding, shouldn't be called !
};

typedef enum enumCVars enumCVars;

extern int AITD1KnownCVars[];
extern int* currentCVarTable;

int getCVarsIdx(int);

//////////////////////

#define SAMPLE_PAGE 0
#define BODY_FLAMME 1
#define MAX_WEIGHT_LOADABLE 2
#define TEXTE_CREDITS 3
#define SAMPLE_TONNERRE 4
#define INTRO_DETECTIVE 5
#define INTRO_HERITIERE 6
#define WORLD_NUM_PERSO 7
#define CHOOSE_PERSO 8
#define SAMPLE_CHOC 9
#define SAMPLE_PLOUF 10
#define REVERSE_OBJECT 11
#define KILLED_SORCERER 12
#define LIGHT_OBJECT 13
#define FOG_FLAG 14
#define DEAD_PERSO 15

//////////////////

// Endian safe read functions
inline uint16 READ_LE_U16(const void *p) {
	const uint8 *data = (const uint8 *)p;
	return (uint16)((data[1] << 8) | data[0]);
}

inline uint16 READ_LE_S16(const void *p) {
	return (uint16)READ_LE_U16(p);
}

inline uint32 READ_LE_U32(const void *p) {
	const uint8 *data = (const uint8 *)p;
	return (uint32)(((uint32)data[3] << 24) | ((uint32)data[2] << 16) | ((uint32)data[1] << 8) | (uint32)data[0]);
}

inline uint8 READ_LE_U8(void *ptr) {
	return *(uint8 *)ptr;
}

inline int8 READ_LE_S8(void *ptr) {
	return *(int8 *)ptr;
}

} // namespace Fitd

#endif
