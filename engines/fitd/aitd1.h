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

#ifndef FITD_AITD1_H
#define FITD_AITD1_H

// ITD_RESS mapping
#define AITD1_TATOU_3DO		0
#define AITD1_TATOU_PAL		1
#define AITD1_TATOU_MCG		2
#define AITD1_PALETTE_JEU	3
#define AITD1_CADRE_SPF		4
#define AITD1_ITDFONT		5
#define AITD1_LETTRE		6
#define AITD1_LIVRE			7
#define AITD1_CARNET		8
#define AITD1_TEXT_GRAPH	9
#define AITD1_PERSO_CHOICE	10
#define AITD1_GRENOUILLE	11
#define AITD1_DEAD_END		12
#define AITD1_TITRE			13
#define AITD1_FOND_INTRO	14
#define AITD1_CAM07000		15
#define AITD1_CAM07001		16
#define AITD1_CAM06000		17
#define AITD1_CAM06005		18
#define AITD1_CAM06008		19

namespace Fitd {

void startAITD1();
// void aitd1_readBook(int index, int type);

}

#endif
