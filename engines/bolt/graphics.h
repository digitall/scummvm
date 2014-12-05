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

#ifndef BOLT_GRAPHICS_H
#define BOLT_GRAPHICS_H

#include "common/scummsys.h"

class OSystem;

namespace Graphics {
struct Surface;
}

namespace Bolt {

struct Rect;

void decodeCLUT7(::Graphics::Surface &dst, int x, int y, int w, int h,
	const byte *src, int srcLen, bool transparency);

void decodeRL7(::Graphics::Surface &dst, int x, int y, int w, int h,
	const byte *src, int srcLen, bool transparency);

byte queryCLUT7(int x, int y, const byte *src, int srcLen, int w, int h);

byte queryRL7(int x, int y, const byte *src, int srcLen, int w, int h);

// CD-I-like graphics system. There is a foreground and a background plane.
// Each plane has a separate 128-color palette. Foreground color index 0 is
// transparent.
class Graphics {
public:
	static const int VGA_SCREEN_WIDTH = 320;
	static const int VGA_SCREEN_HEIGHT = 200;
	static const int CDI_SCREEN_WIDTH = 384;
	static const int CDI_SCREEN_HEIGHT = 240;

	Graphics();

	void init(OSystem *system);

	void setBackPalette(const byte *colors, uint start, uint num);
	void clearBackground();
	::Graphics::Surface getBackSurface();

	void setForePalette(const byte *colors, uint start, uint num);
	void clearForeground();
	::Graphics::Surface getForeSurface();

	void present();

private:
	OSystem *_system;

	static const byte BACK_PALETTE_START = 0;
	static const byte FORE_PALETTE_START = 128;

	byte _backPlane[VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT];
	byte _forePlane[VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT];
};

} // End of namespace Bolt

#endif
