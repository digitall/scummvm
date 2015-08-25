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

#include "graphics/surface.h"

class OSystem;

namespace Bolt {

struct Rect;

void decodeCLUT7(::Graphics::Surface &dst, int x, int y, int w, int h,
	const byte *src, int srcLen, bool transparency);

void decodeRL7(::Graphics::Surface &dst, int x, int y, int w, int h,
	const byte *src, int srcLen, bool transparency);

byte queryCLUT7(int x, int y, const byte *src, int srcLen, int w, int h);

byte queryRL7(int x, int y, const byte *src, int srcLen, int w, int h);

static const int VGA_SCREEN_WIDTH = 320;
static const int VGA_SCREEN_HEIGHT = 200;
static const int CDI_SCREEN_WIDTH = 384;
static const int CDI_SCREEN_HEIGHT = 240;

class Graphics;

class Plane {
public:
	Plane();
	~Plane();

	void init(Graphics *graphics, int width, int height, byte colorBase);

	::Graphics::Surface& getSurface() { return _surface; }
	byte getColorBase() const { return _colorBase; }

	void clear();
	void grabPalette(byte *colors, uint start, uint num);
	void setPalette(const byte *colors, uint start, uint num);

private:
	Graphics *_graphics;
	::Graphics::Surface _surface;
	byte _colorBase;
};

// CD-I-like graphics system. There is a foreground and a background plane.
// Each plane has a separate 128-color palette. Foreground color 0 is
// transparent.
class Graphics {
	friend class Plane;
public:
	Graphics();

	void init(OSystem *system);

	Plane& getBackPlane() { return _backPlane; }
	Plane& getForePlane() { return _forePlane; }

	void present();

private:
	OSystem *_system;

	static const byte BACK_COLOR_BASE = 0;
	static const byte FORE_COLOR_BASE = 128;

	Plane _backPlane;
	Plane _forePlane;
};

} // End of namespace Bolt

#endif
