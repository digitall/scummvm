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
	int in_cursor = 0;

	// Pixel plotting vars
	int srcX = 0;
	int srcY = 0;
	int dstX = x;
	int dstY = y;

	// Top clipping
	while (dstY < 0 && srcY < h) {

		// Fetch byte
		if (in_cursor >= srcLen) {
			return;
		}
		byte in_byte = src[in_cursor];
		++in_cursor;

		if ((in_byte & 0x80) != 0) {

			// Fetch byte
			if (in_cursor >= srcLen) {
				return;
			}
			byte lengthByte = src[in_cursor];
			++in_cursor;

			if (lengthByte == 0) {
				// length 0 means end-of-line
				++dstY;
				++srcY;
			}
		}
	}

	// Render visible lines
	while (dstY < dst.h && srcY < h) {

		// Fetch byte
		if (in_cursor >= srcLen) {
			return;
		}
		byte in_byte = src[in_cursor];
		++in_cursor;

		byte color = in_byte & 0x7F;

		if ((in_byte & 0x80) != 0) {
			// Run of pixels

			// Fetch byte
			if (in_cursor >= srcLen) {
				return;
			}
			byte lengthByte = src[in_cursor];
			++in_cursor;

			// lengthByte 0 means continue until end of line, then go to next
			// line. This is REQUIRED at the end of every line.
			int length = (lengthByte == 0) ? (w - srcX) : lengthByte;
			for (int i = 0; i < length; ++i) {
				if (dstX >= 0 && dstX < dst.w && srcX < w && (!transparency || color != 0)) {
					dstPixels[dstY * dst.pitch + dstX] = color;
				}
				++dstX;
				++srcX;
			}
			if (lengthByte == 0) {
				dstX = x;
				srcX = 0;
				++dstY;
				++srcY;
			}
		}
		else {
			// Single pixel
			if (dstX >= 0 && dstX < dst.w && srcX < w && (!transparency || color != 0)) {
				dstPixels[dstY * dst.pitch + dstX] = color;
			}
			++dstX;
			++srcX;
		}
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

Graphics::Graphics()
	: _system(nullptr)
{ }

void Graphics::init(OSystem *system) {
	_system = system;

	initGraphics(VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT, false);

	clearBackground();
	clearForeground();

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
	setBackPalette(palette, 0, 128);
	setForePalette(palette, 0, 128);
}

void Graphics::setBackPalette(const byte *colors, uint start, uint num) {
	assert(start < 128);
	assert(num <= 128);
	assert(start + num <= 128);

	_system->getPaletteManager()->setPalette(colors,
		BACK_PALETTE_START + start, num);
}

void Graphics::decodeCLUT7ToBack(int x, int y, int w, int h, const byte *src, int srcLen,
	bool transparency) {

	::Graphics::Surface surface;
	surface.init(VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT, VGA_SCREEN_WIDTH,
		_backPlane, ::Graphics::PixelFormat::createFormatCLUT8());

	decodeCLUT7(surface, x, y, w, h, src, srcLen, transparency);
}

void Graphics::decodeRL7ToBack(int x, int y, int w, int h, const byte *src, int srcLen,
	bool transparency) {

	::Graphics::Surface surface;
	surface.init(VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT, VGA_SCREEN_WIDTH,
		_backPlane, ::Graphics::PixelFormat::createFormatCLUT8());

	decodeRL7(surface, x, y, w, h, src, srcLen, transparency);
}

void Graphics::clearBackground() {
	memset(&_backPlane[0], 0, VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT);
}

void Graphics::setForePalette(const byte *colors, uint start, uint num) {
	assert(start < 128);
	assert(num <= 128);
	assert(start + num <= 128);

	_system->getPaletteManager()->setPalette(colors,
		FORE_PALETTE_START + start, num);
}

void Graphics::decodeRL7ToFore(int x, int y, int w, int h, const byte *src, int srcLen,
	bool transparency) {

	::Graphics::Surface surface;
	surface.init(VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT, VGA_SCREEN_WIDTH,
		_forePlane, ::Graphics::PixelFormat::createFormatCLUT8());

	decodeRL7(surface, x, y, w, h, src, srcLen, transparency);
}

void Graphics::clearForeground() {
	memset(&_forePlane[0], 0, VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT);
}

void Graphics::drawRectToBack(const Rect &rect, byte color) {

	::Graphics::Surface surface;
	surface.init(VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT, VGA_SCREEN_WIDTH,
		_backPlane, ::Graphics::PixelFormat::createFormatCLUT8());

	surface.frameRect(rect, color);
}

void Graphics::present() {
	// TODO: Track dirty rectangles for more efficiency

	// Render display
	::Graphics::Surface *surface = _system->lockScreen();

	byte *dst = (byte*)surface->getPixels();
	for (int y = 0; y < VGA_SCREEN_HEIGHT; ++y) {
		for (int x = 0; x < VGA_SCREEN_WIDTH; ++x) {
			int dstIndex = y * surface->pitch + x;
			int srcIndex = y * VGA_SCREEN_WIDTH + x;
			dst[dstIndex] = (_forePlane[srcIndex] != 0) ?
				(_forePlane[srcIndex] + FORE_PALETTE_START) :
				(_backPlane[srcIndex] + BACK_PALETTE_START);
		}
	}

	_system->unlockScreen();

	_system->updateScreen();
}

} // End of namespace Bolt
