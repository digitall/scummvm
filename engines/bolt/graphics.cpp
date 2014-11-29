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

static void decodeCLUT7NoTransparency(byte *dst, int dstPitch, int dstW, int dstH,
	int x, int y, int w, int h, const byte *src, int srcLen) {

	assert(srcLen >= w * h);

	// Out-of-bounds check
	if (x >= dstW || (x + w) <= 0 || y >= dstH || (y + h) <= 0) {
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
	while (dstY < dstH && srcY < h) {
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
		memcpy(&dst[dstY * dstPitch + dstX], &src[srcY * w + srcX],
			MIN<int>(dstW - dstX, w - srcX));
		++dstY;
		++srcY;
	}
}

static void decodeCLUT7WithTransparency(byte *dst, int dstPitch, int dstW, int dstH,
	int x, int y, int w, int h, const byte *src, int srcLen) {

	// NOTE: srcLen is not checked. src must have w * h bytes.
	assert(srcLen >= w * h);

	// Out-of-bounds check
	if (x >= dstW || (x + w) <= 0 || y >= dstH || (y + h) <= 0) {
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
	while (dstY < dstH && srcY < h) {
		int srcX = 0;
		int dstX = x;
		// Left clipping
		if (dstX < 0) {
			srcX = -dstX;
			dstX = 0;
		}

		// TODO: Test all clipping cases
		int len = MIN<int>(dstW - dstX, w - srcX);
		for (int i = 0; i < len; ++i) {
			byte b = src[srcY * w + srcX];
			if (b != 0) {
				dst[dstY * dstPitch + dstX] = b;
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

void decodeCLUT7(byte *dst, int dstPitch, int dstW, int dstH,
	int x, int y, int w, int h, const byte *src, int srcLen,
	bool transparency) {

	if (transparency) {
		decodeCLUT7WithTransparency(dst, dstPitch, dstW, dstH,
			x, y, w, h, src, srcLen);
	}
	else {
		decodeCLUT7NoTransparency(dst, dstPitch, dstW, dstH,
			x, y, w, h, src, srcLen);
	}
}

template<bool transparency>
inline void decodeRL7Internal(byte *dst, int dstPitch, int dstW, int dstH,
	int x, int y, int w, int h, const byte *src, int srcLen) {

	// Out-of-bounds check
	if (x >= dstW || (x + w) <= 0 || y >= dstH || (y + h) <= 0) {
		return;
	}

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
	while (dstY < dstH && srcY < h) {

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
				if (dstX >= 0 && dstX < dstW && srcX < w && (!transparency || color != 0)) {
					dst[dstY * dstPitch + dstX] = color;
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
			if (dstX >= 0 && dstX < dstW && srcX < w && (!transparency || color != 0)) {
				dst[dstY * dstPitch + dstX] = color;
			}
			++dstX;
			++srcX;
		}
	}
}

void decodeRL7(byte *dst, int dstPitch, int dstW, int dstH,
	int x, int y, int w, int h, const byte *src, int srcLen, bool transparency) {

	if (transparency) {
		decodeRL7Internal<true>(dst, dstPitch, dstW, dstH, x, y, w, h, src, srcLen);
	}
	else {
		decodeRL7Internal<false>(dst, dstPitch, dstW, dstH, x, y, w, h, src, srcLen);
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

/**
 * Convert an array of CD-I colors to sRGB format.
 *
 * From Green Book section 4.4.1.2 RGB Levels (p. V-40):
 * All images, encoded by whatever method, are decoded [...] to a uniform
 * 8-bit linear representation of the Red, Green, and Blue color components.
 * For each component, black level is at 16 and nominal peak (white) level is
 * at 235.
 */
static void convertCdiColorsToSrgb(byte *dst, const byte *src, uint num) {

	// This table is generated by a Python script.
	// i in 16...235 -> BT.601 in 0...1 (clamped) -> Linear -> sRGB
	// FIXME: link to script (use pastebin or github)
	// UPDATE: THIS IS WRONG. I found out that the PC versions contain
	// different graphics resources from the CD-I versions, and they probably
	// have the colors already converted for display on PC monitors.
	// When/if we support the CD-I games, we should revisit color conversion.
	static const byte C_TABLE[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 3, 7, 10, 13, 16, 18, 20, 22, 24, 26, 27, 29, 30, 32, 33,
		34, 36, 37, 38, 39, 40, 41, 43, 44, 45, 46, 47, 48, 50, 51, 52,
		53, 54, 55, 56, 58, 59, 60, 61, 62, 63, 65, 66, 67, 68, 69, 70,
		71, 72, 74, 75, 76, 77, 78, 79, 80, 82, 83, 84, 85, 86, 87, 88,
		89, 90, 92, 93, 94, 95, 96, 97, 98, 99, 100, 102, 103, 104, 105, 106,
		107, 108, 109, 110, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 124,
		125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 136, 137, 138, 139, 140, 141,
		142, 143, 144, 145, 146, 147, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158,
		159, 160, 161, 162, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
		176, 177, 178, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
		193, 194, 195, 196, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209,
		210, 211, 212, 213, 214, 215, 216, 218, 219, 220, 221, 222, 223, 224, 225, 226,
		227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 242, 243,
		244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 255, 255, 255, 255,
		255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	};

	for (uint i = 0; i < num; ++i) {
		byte srcR = src[3 * i + 0];
		byte srcG = src[3 * i + 1];
		byte srcB = src[3 * i + 2];

		// FIXME: BT.601 color primaries correction? Probably not worth it.
		// BT.601 has two sets of RGB primaries for 525 and 625-line systems
		// (NTSC and PAL). It is not clear which one we should use. Besides,
		// the BT.709 primaries used by sRGB and HDTV sets is intermediary
		// between BT.601's two sets.
		dst[3 * i + 0] = C_TABLE[srcR];
		dst[3 * i + 1] = C_TABLE[srcG];
		dst[3 * i + 2] = C_TABLE[srcB];
	}
}

// FIXME: Make CD-I-like colors a runtime option, perhaps for an "enhanced
// CD-I graphics" mode. The PC versions of these games performed no color
// correction, as far as I know.
// The CD-I also had a cross-fade effect that the PC version lacked.
//#define CONVERT_CDI_COLORS 1

void Graphics::setBackPalette(const byte *colors, uint start, uint num) {
	assert(start < 128);
	assert(num <= 128);
	assert(start + num <= 128);

#if CONVERT_CDI_COLORS
	byte pal[128 * 3];
	convertCdiColorsToSrgb(pal, colors, num);
	_system->getPaletteManager()->setPalette(pal, BACK_PALETTE_START + start, num);
#else
	_system->getPaletteManager()->setPalette(colors,
		BACK_PALETTE_START + start, num);
#endif
}

void Graphics::decodeCLUT7ToBack(int x, int y, int w, int h, const byte *src, int srcLen,
	bool transparency) {

	decodeCLUT7(_backPlane, VGA_SCREEN_WIDTH, VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT,
		x, y, w, h, src, srcLen, transparency);
}

void Graphics::decodeRL7ToBack(int x, int y, int w, int h, const byte *src, int srcLen,
	bool transparency) {

	decodeRL7(_backPlane, VGA_SCREEN_WIDTH, VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT,
		x, y, w, h, src, srcLen, transparency);
}

void Graphics::clearBackground() {
	memset(&_backPlane[0], 0, VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT);
}

void Graphics::setForePalette(const byte *colors, uint start, uint num) {
	assert(start < 128);
	assert(num <= 128);
	assert(start + num <= 128);

#if CONVERT_CDI_COLORS
	byte pal[128 * 3];
	convertCdiColorsToSrgb(pal, colors, num);
	_system->getPaletteManager()->setPalette(pal, FORE_PALETTE_START + start, num);
#else
	_system->getPaletteManager()->setPalette(colors,
		FORE_PALETTE_START + start, num);
#endif
}

void Graphics::decodeRL7ToFore(int x, int y, int w, int h, const byte *src, int srcLen,
	bool transparency) {

	decodeRL7(_forePlane, VGA_SCREEN_WIDTH, VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT,
		x, y, w, h, src, srcLen, transparency);
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
