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

#include "bolt/graphics.h"

#include "bolt/bolt.h"

#include "common/array.h"
#include "common/system.h"
#include "common/util.h"
#include "engines/util.h"
#include "graphics/palette.h"
#include "graphics/surface.h"

namespace Bolt {

static void decodeCLUT7NoTransparency(::Graphics::Surface &dst,
	int x, int y, int w, int h, const byte *src, int srcLen) {

	assert(srcLen >= w * h);
	assert(dst.format.bytesPerPixel == 1);

	byte *dstPixels = (byte*)dst.getPixels();

	// Out-of-bounds check
	if (x >= dst.w || (x + w) <= 0 || y >= dst.h || (y + h) <= 0) {
		return;
	}

	int srcY = 0;
	int dstY = y;

	// Top clipping
	if (dstY < 0) {
		srcY = -dstY;
		dstY = 0;
	}

	// Render visible lines
	while (dstY < dst.h && srcY < h) {
		int srcX = 0;
		int dstX = x;
		// Left clipping
		if (dstX < 0) {
			srcX = -dstX;
			dstX = 0;
		}
		// TODO: Test all clipping cases
		// NOTE: Out-of-bounds check above prevents trying to copy "negative"
		// amounts of memory.
		memcpy(&dstPixels[dstY * dst.pitch + dstX], &src[srcY * w + srcX],
			MIN<int>(dst.w - dstX, w - srcX));
		++dstY;
		++srcY;
	}
}

static void decodeCLUT7WithTransparency(::Graphics::Surface &dst,
	int x, int y, int w, int h, const byte *src, int srcLen) {

	assert(srcLen >= w * h);
	assert(dst.format.bytesPerPixel == 1);

	byte *dstPixels = (byte*)dst.getPixels();

	// Out-of-bounds check
	if (x >= dst.w || (x + w) <= 0 || y >= dst.h || (y + h) <= 0) {
		return;
	}

	int srcY = 0;
	int dstY = y;

	// Top clipping
	if (dstY < 0) {
		srcY = -dstY;
		dstY = 0;
	}

	// Render visible lines
	while (dstY < dst.h && srcY < h) {
		int srcX = 0;
		int dstX = x;
		// Left clipping
		if (dstX < 0) {
			srcX = -dstX;
			dstX = 0;
		}

		// TODO: Test all clipping cases
		int len = MIN<int>(dst.w - dstX, w - srcX);
		for (int i = 0; i < len; ++i) {
			byte b = src[srcY * w + srcX];
			if (b != 0) {
				dstPixels[dstY * dst.pitch + dstX] = b;
			}

			++dstX;
			++srcX;
		}

		// next line
		dstX = 0;
		srcX = 0;
		++dstY;
		++srcY;
	}
}

void decodeCLUT7(::Graphics::Surface &dst, int x, int y, int w, int h,
	const byte *src, int srcLen, bool transparency) {

	if (transparency) {
		decodeCLUT7WithTransparency(dst, x, y, w, h, src, srcLen);
	}
	else {
		decodeCLUT7NoTransparency(dst, x, y, w, h, src, srcLen);
	}
}

// Decodes an RL7 line from src to dst. dst is the start of a destination
// line. Returns number of bytes consumed from src.
template<bool draw, bool transparency>
inline int decodeRL7Line(byte *dst, int dstW, int x, int w,
	const byte *src, int srcLen) {

	int inCursor = 0;
	int dstX = x;
	int srcX = 0;
	bool end = false;
	while (!end)
	{
		// Fetch byte
		if (inCursor >= srcLen) {
			return inCursor;
		}
		byte inByte = src[inCursor];
		++inCursor;

		byte color = inByte & 0x7F;
		int length = 1;

		if ((inByte & 0x80) != 0) {
			// Run of pixels

			// Fetch byte
			if (inCursor >= srcLen) {
				return inCursor;
			}
			byte lengthByte = src[inCursor];
			++inCursor;

			if (lengthByte == 0) {
				// length 0 means run to end of line
				length = w - srcX;
				end = true;
			}
			else {
				length = lengthByte;
			}
		}

		if (draw && (!transparency || color != 0)) {

			// Draw clipped run
			int dstL = dstX;
			int dstR = dstX + length;
			int srcL = srcX;
			int srcR = srcX + length;

			if (dstL < 0) {
				srcL += -dstL;
				dstL = 0;
			}
			if (dstR > dstW) {
				srcR -= (dstR - dstW);
				dstR = dstW;
			}
			if (srcR > w) {
				dstR -= (w - srcR);
				srcR = w;
			}

			for (int i = dstL; i < dstR; ++i) {
				dst[i] = color;
			}
		}

		dstX += length;
		srcX += length;
	}

	return inCursor;
}

template<bool transparency>
inline void decodeRL7Internal(::Graphics::Surface &dst,
	int x, int y, int w, int h, const byte *src, int srcLen) {

	assert(dst.format.bytesPerPixel == 1);

	// Out-of-bounds check
	if (x >= dst.w || (x + w) <= 0 || y >= dst.h || (y + h) <= 0) {
		return;
	}

	byte *dstPixels = (byte*)dst.getPixels();

	// Source index var
	int inCursor = 0;

	// Pixel plotting vars
	int srcY = 0;
	int dstY = y;

	// Top clipping
	while (dstY < 0 && srcY < h) {

		// Skip lines
		inCursor += decodeRL7Line<false, false>(nullptr, dst.w, x, w,
			&src[inCursor], srcLen - inCursor);
		++dstY;
		++srcY;
	}


	// Draw visible lines
	int outCursor = dstY * dst.pitch;
	while (dstY < dst.h && srcY < h) {

		inCursor += decodeRL7Line<true, transparency>(
			&dstPixels[outCursor], dst.w, x, w,
			&src[inCursor], srcLen - inCursor
			);
		++dstY;
		++srcY;
		outCursor += dst.pitch;
	}
}

void decodeRL7(::Graphics::Surface &dst, int x, int y, int w, int h,
	const byte *src, int srcLen, bool transparency) {

	if (transparency) {
		decodeRL7Internal<true>(dst, x, y, w, h, src, srcLen);
	}
	else {
		decodeRL7Internal<false>(dst, x, y, w, h, src, srcLen);
	}
}

byte queryCLUT7(int x, int y, const byte *src, int srcLen, int w, int h) {
	assert(srcLen >= w * h);
	if (x < 0 || x >= w || y < 0 || y >= h) {
		// Outside image
		return 0;
	}

	return src[y * w + x];
}

byte queryRL7(int x, int y, const byte *src, int srcLen, int w, int h) {
	if (x < 0 || x >= w || y < 0 || y >= h) {
		// Outside image
		return 0;
	}

	int srcY = 0;
	int inCursor = 0;

	// Skip to line y
	// NOTE: The original program avoids this step by caching line offsets.
	while (srcY < y && srcY < h) {

		inCursor += decodeRL7Line<false, false>(nullptr, w, 0, w,
			&src[inCursor], srcLen - inCursor);
		++srcY;
	}

	// Decode line until x is reached
	byte color = 0;
	int srcX = 0;
	while (srcX < w) {

		if (inCursor >= srcLen) {
			return 0;
		}
		byte inByte = src[inCursor];
		++inCursor;

		color = inByte & 0x7F;

		if ((inByte & 0x80) != 0) {

			if (inCursor >= srcLen) {
				return 0;
			}
			byte lengthByte = src[inCursor];
			++inCursor;

			if (lengthByte == 0) {
				break;
			}
			else {
				srcX += lengthByte;
				if (srcX > x) {
					break;
				}
			}
		}
		else {
			++srcX;
			if (srcX > x) {
				break;
			}
		}
	}

	return color;
}

Plane::Plane()
	: _graphics(nullptr)
{ }

Plane::~Plane() {
	_surface.free();
}

void Plane::init(Graphics *graphics, int width, int height, byte colorBase) {
	_graphics = graphics;
	_surface.create(width, height, ::Graphics::PixelFormat::createFormatCLUT8());
	_colorBase = colorBase;
}

void Plane::clear() {
	memset(_surface.getPixels(), 0, _surface.w * _surface.h);
}

void Plane::grabPalette(byte *colors, uint start, uint num) {
	assert(start < 128);
	assert(num <= 128);
	assert(start + num <= 128);

	_graphics->_system->getPaletteManager()->grabPalette(colors,
		_colorBase + start, num);
}

void Plane::setPalette(const byte *colors, uint start, uint num) {
	assert(start < 128);
	assert(num <= 128);
	assert(start + num <= 128);

	_graphics->_system->getPaletteManager()->setPalette(colors,
		_colorBase + start, num);
}

Graphics::Graphics()
	: _system(nullptr),
	_dirty(false)
{ }

void Graphics::init(OSystem *system, uint32 time) {
	_system = system;
	_curTime = time;
	_colorCycleTime = time;
	_dirty = true;

	initGraphics(kVgaScreenWidth, kVgaScreenHeight, false);

	_backPlane.init(this, kVgaScreenWidth, kVgaScreenHeight, kBackColorBase);
	_forePlane.init(this, kVgaScreenWidth, kVgaScreenHeight, kForeColorBase);

	// XXX: Load a generic testing palette into both planes
	byte palette[128 * 3];
	for (int r = 0; r < 4; ++r) {
		for (int g = 0; g < 8; ++g) {
			for (int b = 0; b < 4; ++b) {
				int i = 8 * 4 * r + 4 * g + b;
				palette[3 * i + 0] = r * 255 / 3;
				palette[3 * i + 1] = g * 255 / 7;
				palette[3 * i + 2] = b * 255 / 3;
			}
		}
	}
	_backPlane.setPalette(palette, 0, 128);
	_forePlane.setPalette(palette, 0, 128);
}

void Graphics::setTime(uint32 time) {
	_curTime = time;

	// Handle color cycling
	uint32 diff = _curTime - _colorCycleTime;
	// FIXME: Cycling speed is probably configurable.
	if (diff >= kColorCycleMillis) {
		bool colorCyclesEnabled = false;
		// Cycle!
		for (int i = 0; i < kNumColorCycles; ++i) {
			if (_colorCycles[i].num > 0) {
				colorCyclesEnabled = true;

				byte colors[128 * 3];
				// FIXME: Both planes may have color cycles.
				// Front plane color cycles are used in the "bubbles" action puzzle.
				_backPlane.grabPalette(colors, _colorCycles[i].start,
					_colorCycles[i].num);

				// Rotate colors right by one
				byte r = colors[3 * (_colorCycles[i].num - 1) + 0];
				byte g = colors[3 * (_colorCycles[i].num - 1) + 1];
				byte b = colors[3 * (_colorCycles[i].num - 1) + 2];
				memmove(&colors[3], &colors[0], 3 * (_colorCycles[i].num - 1));
				colors[0] = r;
				colors[1] = g;
				colors[2] = b;

				_backPlane.setPalette(colors, _colorCycles[i].start, _colorCycles[i].num);
			}
		}

		if (colorCyclesEnabled) {
			// We only rotate once, that should be enough unless there are big delays
			_colorCycleTime += kColorCycleMillis;
			markDirty();
		}
	}
}

void Graphics::resetColorCycles() {
	for (int i = 0; i < kNumColorCycles; ++i) {
		_colorCycles[i].num = 0;
	}
}

void Graphics::setColorCycle(int slot, uint16 start, uint16 num) {
	assert(slot >= 0 && slot < kNumColorCycles);
	if (start >= 0 && (start + num) <= 128) {
		_colorCycles[slot].start = start;
		_colorCycles[slot].num = num;

		// Start cycling now
		_colorCycleTime = _curTime;
	}
	else {
		warning("Invalid color cycle start %d, num %d", (int)start, (int)num);
		_colorCycles[slot].num = 0;
	}
}

void Graphics::markDirty() {
	_dirty = true;
}

void Graphics::presentIfDirty() {
	if (_dirty) {
		// TODO: Track dirty rectangles for more efficiency
		::Graphics::Surface *dstSurface = _system->lockScreen();
		::Graphics::Surface &backSurface = _backPlane.getSurface();
		::Graphics::Surface &foreSurface = _forePlane.getSurface();

		byte *dstLine = (byte*)dstSurface->getPixels();
		const byte *backLine = (const byte*)backSurface.getPixels();
		const byte *foreLine = (const byte*)foreSurface.getPixels();
		for (int y = 0; y < kVgaScreenHeight; ++y) {
			for (int x = 0; x < kVgaScreenWidth; ++x) {
				dstLine[x] = (foreLine[x] != 0) ?
					(foreLine[x] + _forePlane.getColorBase()) :
					(backLine[x] + _backPlane.getColorBase());
			}
			dstLine += dstSurface->pitch;
			backLine += backSurface.pitch;
			foreLine += foreSurface.pitch;
		}
		_system->unlockScreen();

		_system->updateScreen();

		_dirty = false;
	}
}

struct BltImageHeader {
	static const int kSize = 0x18;
	BltImageHeader(const byte *src) {
		compression = src[0];
		// FIXME: unknown fields
		offset.x = READ_BE_INT16(&src[6]);
		offset.y = READ_BE_INT16(&src[8]);
		width = READ_BE_UINT16(&src[0xA]);
		height = READ_BE_UINT16(&src[0xC]);
	}

	byte compression;
	Common::Point offset;
	uint16 width;
	uint16 height;
};

void BltImage::load(Boltlib &bltFile, BltId id) {
	_res.reset(bltFile.loadResource(id, kBltImage));
}

void BltImage::draw(::Graphics::Surface &surface, bool transparency) const {
	drawWithTopLeftAnchor(surface, 0, 0, transparency);
}

void BltImage::drawAt(::Graphics::Surface &surface, int x, int y,
	bool transparency) const {
	assert(_res);

	BltImageHeader header(&_res[0]);
	int topLeftX = x + header.offset.x;
	int topLeftY = y + header.offset.y;

	drawWithTopLeftAnchor(surface, topLeftX, topLeftY, transparency);
}

void BltImage::drawWithTopLeftAnchor(
	::Graphics::Surface &surface, int x, int y, bool transparency) const {

	assert(_res);

	BltImageHeader header(&_res[0]);
	const byte *imageData = &_res[BltImageHeader::kSize];
	int imageDataSize = _res.size() - BltImageHeader::kSize;

	if (header.compression) {
		decodeRL7(surface, x, y, header.width, header.height,
			imageData, imageDataSize, transparency);
	}
	else {
		decodeCLUT7(surface, x, y, header.width, header.height,
			imageData, imageDataSize, transparency);
	}
}

byte BltImage::query(int x, int y) const {
	BltImageHeader header(&_res[0]);
	const byte *src = &_res[BltImageHeader::kSize];
	int srcLen = _res.size() - BltImageHeader::kSize;
	return header.compression ?
		queryRL7(x, y, src, srcLen, header.width, header.height) :
		queryCLUT7(x, y, src, srcLen, header.width, header.height);
}

uint16 BltImage::getWidth() const {
	BltImageHeader header(&_res[0]);
	return header.width;
}

uint16 BltImage::getHeight() const {
	BltImageHeader header(&_res[0]);
	return header.height;
}

Common::Point BltImage::getOffset() const {
	BltImageHeader header(&_res[0]);
	return header.offset;
}

struct BltPaletteHeader {
	static const uint32 kSize = 6;
	BltPaletteHeader(const byte *src) {
		target = READ_BE_UINT16(&src[0]);
		bottom = READ_BE_UINT16(&src[2]);
		top = READ_BE_UINT16(&src[4]);
	}

	uint16 target; // ??? (usually 2)
	uint16 bottom; // ??? (usually 0)
	uint16 top; // ??? (usually 127)
};

void BltPalette::load(Boltlib &bltFile, BltId id) {
	_res.reset(bltFile.loadResource(id, kBltPalette));
}

void BltPalette::set(Graphics &graphics, Target target) const {
	BltPaletteHeader header(&_res[0]);
	if (header.target == 0) {
		debug(3, "setting palette with target type 0 to both planes");
		// Both fore and back planes
		if (header.bottom != 0) {
			warning("palette target 0 bottom color is not 0");
		}
		if (header.top != 255) {
			warning("palette target 0 top color is not 255");
		}

		graphics.getBackPlane().setPalette(&_res[BltPaletteHeader::kSize], 0, 128);
		graphics.getForePlane().setPalette(&_res[BltPaletteHeader::kSize + 128 * 3], 0, 128);
	}
	else if (header.target == 2) {
		// Auto? Back or fore not specified in palette resource.
		Plane *plane = nullptr;
		if (target == kBack) {
			debug(3, "setting palette with target type 2 to back");
			plane = &graphics.getBackPlane();
		}
		else if (target == kFore) {
			debug(3, "setting palette with target type 2 to fore");
			plane = &graphics.getForePlane();
		}
		else {
			assert(false); // Unreachable, target must be valid
		}

		uint num = header.top - header.bottom + 1;
		plane->setPalette(&_res[BltPaletteHeader::kSize],
			header.bottom, num);
	}
	else {
		warning("Unknown palette target %d", (int)header.target);
	}
}

} // End of namespace Bolt
