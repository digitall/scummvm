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

#ifndef FITD_TATOU_H
#define FITD_TATOU_H

namespace Fitd {

int make3dTatou(void);
void startChrono(unsigned int* chrono);
int evalChrono(unsigned int* chrono);
void process_events(void);
void makeBlackPalette();
void paletteFill(void *palette, byte r, byte g, byte b);
void copyPalette(byte* source, byte* dest);
void fastCopyScreen(void* source, void* dest);
void fadeInPhys(int step, int start);
void fadeOutPhys(int var1, int var2);
void playSound(int num);

}

#endif
