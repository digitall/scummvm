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
	memset(_surface.getPixels(), 0, VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT);
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
	: _system(nullptr)
{ }

void Graphics::init(OSystem *system) {
	_system = system;

	initGraphics(VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT, false);

	_backPlane.init(this, VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT, BACK_COLOR_BASE);
	_forePlane.init(this, VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT, FORE_COLOR_BASE);

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

void Graphics::present() {
	// TODO: Track dirty rectangles for more efficiency

	// Render display

	::Graphics::Surface *dstSurface = _system->lockScreen();

	::Graphics::Surface &backSurface = _backPlane.getSurface();
	::Graphics::Surface &foreSurface = _forePlane.getSurface();

	byte *dstLine = (byte*)dstSurface->getPixels();
	const byte *backLine = (const byte*)backSurface.getPixels();
	const byte *foreLine = (const byte*)foreSurface.getPixels();

	for (int y = 0; y < VGA_SCREEN_HEIGHT; ++y) {
		for (int x = 0; x < VGA_SCREEN_WIDTH; ++x) {
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
}

} // End of namespace Bolt
