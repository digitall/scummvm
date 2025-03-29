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

#ifndef _FITD_FONT_H_
#define _FITD_FONT_H_

#include "common/scummsys.h"

namespace Fitd {

extern int fontHeight;
void extSetFont(char *fontData, int color);
void setFontSpace(int interWordSpace, int interLetterSpace);
int extGetSizeFont(char *string);
void renderText(int x, int y, char *surface, const char *string);
void selectedMessage(int x, int y, int index, int color1, int color2);
void simpleMessage(int x, int y, int index, int color);

} // namespace Fitd

#endif
