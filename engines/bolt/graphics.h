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
#include "common/rational.h"

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

enum { // plane numbers
	kFore = 0,
	kBack = 1,
};

// CD-I-like graphics system. There is a foreground and a background plane.
// Each plane has a separate 128-color palette. Foreground color 0 is
// transparent.
class Graphics {
public:
	Graphics();

	void init(OSystem *system, uint32 time);

	::Graphics::Surface& getPlaneSurface(int plane);
	void setPlanePalette(int plane, const byte *colors, int first, int num);
	void clearPlane(int plane);
	void drawRect(int plane, const Rect &rc, byte color);

	// TODO: better system for timing
	void setTime(uint32 time);
	void resetColorCycles();
	void setColorCycle(int slot, uint16 start, uint16 end, int delay);
	void setFade(Common::Rational fade);
	void markDirty();
	void presentIfDirty();

private:
	OSystem *_system;

	static const int kNumVgaColors = 256;
	static const int kNumPlaneColors = 128;
	static const byte kForeVgaFirst = 0;
	static const byte kBackVgaFirst = 128;

	struct Plane {
		~Plane();

		::Graphics::Surface surface;
		byte vgaFirst;
	};

	Plane* getPlaneObject(int plane);
	void initPlane(Plane &plane, int width, int height, byte vgaFirst);
	void grabPlanePalette(int plane, byte *colors, int first, int num);
	void grabVgaPalette(byte *colors, int first, int num);
	void setVgaPalette(const byte *colors, int first, int num);
	void commitVgaPalette(int first, int num);

	Plane _forePlane;
	Plane _backPlane;
	byte _vgaPalette[kNumVgaColors * 3]; // Unfaded

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

	Common::Rational _fade;

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
