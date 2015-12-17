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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef BOLT_GRAPHICS_H
#define BOLT_GRAPHICS_H

#include "common/rect.h"
#include "common/scummsys.h"

#include "graphics/surface.h"
#include "bolt/boltlib/boltlib.h"

class OSystem;

namespace Bolt {

struct Rect;

void decodeCLUT7(::Graphics::Surface &dst, int x, int y, int w, int h,
	const byte *src, int srcLen, bool transparency);

void decodeRL7(::Graphics::Surface &dst, int x, int y, int w, int h,
	const byte *src, int srcLen, bool transparency);

byte queryCLUT7(int x, int y, const byte *src, int srcLen, int w, int h);

byte queryRL7(int x, int y, const byte *src, int srcLen, int w, int h);

static const int kVgaScreenWidth = 320;
static const int kVgaScreenHeight = 200;
static const int kCdiScreenWidth = 384;
static const int kCdiScreenHeight = 240;

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

	void init(OSystem *system, uint32 time);

	Plane& getBackPlane() { return _backPlane; }
	Plane& getForePlane() { return _forePlane; }

	// TODO: better system for timing
	void setTime(uint32 time);
	void resetColorCycles();
	void setColorCycle(int slot, uint16 start, uint16 end, int delay);
	void markDirty();
	void presentIfDirty();

private:
	OSystem *_system;

	static const byte kBackColorBase = 0;
	static const byte kForeColorBase = 128;

	Plane _backPlane;
	Plane _forePlane;

	struct ColorCycle {
		ColorCycle() : start(0), end(0), delay(0) { }
		uint16 start;
		uint16 end;
		int delay; // 0 means this cycle is inactive
		uint32 curTime; // Time of last color rotation
	};

	static const int kNumColorCycles = 4;
	ColorCycle _colorCycles[kNumColorCycles];
	uint32 _curTime; // Time of current event

	bool _dirty;
};

class BltImage { // type 8
public:
	operator bool() const {
		return _res;
	}

	void load(Boltlib &bltFile, BltId id);

	void draw(::Graphics::Surface &surface, bool transparency) const;
	void drawAt(::Graphics::Surface &surface, int x, int y, bool transparency) const;
	byte query(int x, int y) const;

	Common::Rect getRect(const Common::Point &pos = Common::Point(0, 0)) const;
	uint16 getWidth() const;
	uint16 getHeight() const;
	Common::Point getOffset() const;

private:
	void drawWithTopLeftAnchor(::Graphics::Surface &surface, int x, int y, bool transparency) const;

	BltResource _res;
};

} // End of namespace Bolt

#endif
