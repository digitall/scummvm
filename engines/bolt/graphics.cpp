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

Graphics::Graphics()
	: _system(nullptr),
	_dirty(false)
{ }

void Graphics::init(OSystem *system, IBoltEventLoop *eventLoop) {
	_system = system;
	_eventLoop = eventLoop;

	_fade = Common::Rational(1);
	_dirty = true;

	initGraphics(kVgaScreenWidth, kVgaScreenHeight, false);

	initPlane(_backPlane, kVgaScreenWidth, kVgaScreenHeight, kBackVgaFirst);
	initPlane(_forePlane, kVgaScreenWidth, kVgaScreenHeight, kForeVgaFirst);
}

::Graphics::Surface& Graphics::getPlaneSurface(int plane) {
	Plane *p = getPlaneObject(plane);
	assert(p);

	return p->surface;
}

void Graphics::setPlanePalette(int plane, const byte *colors, int first, int num) {
	Plane *p = getPlaneObject(plane);
	if (!p) {
		return;
	}

	if (first < 0 || num < 0 || (first + num) > 128) {
		warning("Invalid plane palette range first %d num %d", first, num);
		return;
	}

	setVgaPalette(colors, p->vgaFirst + first, num);
}

void Graphics::clearPlane(int plane) {
	Plane *p = getPlaneObject(plane);
	if (!p) {
		return;
	}

	memset(p->surface.getPixels(), 0, p->surface.w * p->surface.h);
}

void Graphics::drawRect(int plane, const Rect &rc, byte color) {
	Plane *p = getPlaneObject(plane);
	if (!p) {
		return;
	}

	p->surface.frameRect(rc, color);
}

static void rotateColorsForward(byte *colors, int num) {
	byte rgb[3];
	memcpy(rgb, &colors[3 * (num - 1)], 3);
	memmove(&colors[3], &colors[0], 3 * (num - 1));
	memcpy(&colors[0], rgb, 3);
}

static void rotateColorsBackward(byte *colors, int num) {
	byte rgb[3];
	memcpy(rgb, &colors[0], 3);
	memmove(&colors[0], &colors[3], 3 * (num - 1));
	memcpy(&colors[3 * (num - 1)], rgb, 3);
}

void Graphics::handleEvent(const BoltEvent &event) {
	if (event.type == BoltEvent::kHover) {
		// Draw cursor at new position
		// TODO: markDirty only if cursor is visible (there is currently no way to query
		// cursor visibility status...)
		markDirty();
	} else if (event.type == BoltEvent::kDrive) {
		// TODO: eliminate Drive events in favor of Timers
		// Drive color cycles
		for (int i = 0; i < kNumColorCycles; ++i) {
			if (_colorCycles[i].delay > 0) {
				uint32 diff = event.eventTime - _colorCycles[i].curTime;
				if (diff >= (uint32)_colorCycles[i].delay) {
					bool backwards = _colorCycles[i].end < _colorCycles[i].start;

					uint16 firstColor;
					uint16 numColors;
					if (backwards) {
						firstColor = _colorCycles[i].end;
						numColors = _colorCycles[i].start - _colorCycles[i].end + 1;
					}
					else {
						firstColor = _colorCycles[i].start;
						numColors = _colorCycles[i].end - _colorCycles[i].start + 1;
					}

					// Rotate colors
					byte colors[128 * 3];
					// FIXME: Both planes may have color cycles. Front plane color
					// cycles are used in the "bubbles" action puzzle.
					grabPlanePalette(kBack, colors, firstColor, numColors);
					if (backwards) {
						rotateColorsBackward(colors, numColors);
					}
					else {
						rotateColorsForward(colors, numColors);
					}
					setPlanePalette(kBack, colors, firstColor, numColors);

					markDirty();

					_colorCycles[i].curTime += _colorCycles[i].delay;
				}
			}
		}
	}
}

void Graphics::resetColorCycles() {
	for (int i = 0; i < kNumColorCycles; ++i) {
		_colorCycles[i].delay = 0;
	}
}

void Graphics::setColorCycle(int slot, uint16 start, uint16 end, int delay) {
	assert(slot >= 0 && slot < kNumColorCycles);
	assert(delay >= 0);
	if (start < 128 && end < 128) {
		_colorCycles[slot].start = start;
		_colorCycles[slot].end = end;
		_colorCycles[slot].delay = delay;
		// Start cycling now
		_colorCycles[slot].curTime = _eventLoop->getEventTime();
	}
	else {
		warning("Invalid color cycle start %d, end %d", (int)start, (int)end);
		_colorCycles[slot].delay = 0;
	}
}

void Graphics::setFade(Common::Rational fade) {
	_fade = fade;
	commitVgaPalette(0, 256);
	markDirty();
}

void Graphics::markDirty() {
	_dirty = true;
}

void Graphics::presentIfDirty() {
	if (_dirty) {
		// TODO: Only update dirty rectangles
		// TODO: Use hardware acceleration if possible
		::Graphics::Surface *dstSurface = _system->lockScreen();

		byte *dstLine = (byte*)dstSurface->getPixels();
		const byte *backLine = (const byte*)_backPlane.surface.getPixels();
		const byte *foreLine = (const byte*)_forePlane.surface.getPixels();
		for (int y = 0; y < kVgaScreenHeight; ++y) {
			for (int x = 0; x < kVgaScreenWidth; ++x) {
				dstLine[x] = (foreLine[x] != 0) ?
					(foreLine[x] + _forePlane.vgaFirst) :
					(backLine[x] + _backPlane.vgaFirst);
			}
			dstLine += dstSurface->pitch;
			backLine += _backPlane.surface.pitch;
			foreLine += _forePlane.surface.pitch;
		}
		_system->unlockScreen();

		_system->updateScreen();

		_dirty = false;
	}
}

Graphics::Plane::~Plane() {
	surface.free();
}

Graphics::Plane* Graphics::getPlaneObject(int plane) {
	if (plane == kFore) {
		return &_forePlane;
	}
	else if (plane == kBack) {
		return &_backPlane;
	}
	else {
		warning("Invalid plane number %d", plane);
		return nullptr;
	}
}

void Graphics::initPlane(Plane &plane, int width, int height, byte vgaFirst) {
	plane.surface.create(width, height, ::Graphics::PixelFormat::createFormatCLUT8());
	plane.vgaFirst = vgaFirst;
}

void Graphics::grabPlanePalette(int plane, byte *colors, int first, int num) {
	assert(first >= 0);
	assert(num >= 0);
	assert(first + num <= kNumPlaneColors);

	Plane *p = getPlaneObject(plane);
	if (!p) {
		return;
	}

	grabVgaPalette(colors, p->vgaFirst + first, num);
}

void Graphics::grabVgaPalette(byte *colors, int first, int num) {
	assert(first >= 0);
	assert(num >= 0);
	assert(first + num <= kNumVgaColors);

	memcpy(colors, &_vgaPalette[3 * first], 3 * num);
}

void Graphics::setVgaPalette(const byte *colors, int first, int num) {
	assert(first >= 0);
	assert(num >= 0);
	assert(first + num <= kNumVgaColors);

	memcpy(&_vgaPalette[3 * first], colors, 3 * num);
	commitVgaPalette(first, num);
}

void Graphics::commitVgaPalette(int first, int num) {
	assert(first >= 0);
	assert(num >= 0);
	assert(first + num <= kNumVgaColors);

	if (_fade >= 1) {
		_system->getPaletteManager()->setPalette(&_vgaPalette[3 * first], first, num);
	}
	else {
		byte faded[256 * 3];
		for (int i = 0; i < num * 3; ++i) {
			faded[i] = (_vgaPalette[3 * first + i] * _fade).toInt();
		}
		_system->getPaletteManager()->setPalette(faded, first, num);
	}
}

struct BltImageHeader {
	static const int kSize = 0x18;
	BltImageHeader(const ConstSizedDataView<kSize> src) {
		compression = src.readUint8(0);
		// FIXME: unknown fields at 1..6
		offset.x = src.readInt16BE(6);
		offset.y = src.readInt16BE(8);
		width = src.readUint16BE(0xA);
		height = src.readUint16BE(0xC);
		// FIXME: unknown fields at 0xE..0x18
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

	BltImageHeader header(_res.slice(0));
	int topLeftX = x + header.offset.x;
	int topLeftY = y + header.offset.y;

	drawWithTopLeftAnchor(surface, topLeftX, topLeftY, transparency);
}

void BltImage::drawWithTopLeftAnchor(
	::Graphics::Surface &surface, int x, int y, bool transparency) const {

	assert(_res);

	BltImageHeader header(_res.slice(0));
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
	BltImageHeader header(_res.slice(0));
	const byte *src = &_res[BltImageHeader::kSize];
	int srcLen = _res.size() - BltImageHeader::kSize;
	return header.compression ?
		queryRL7(x, y, src, srcLen, header.width, header.height) :
		queryCLUT7(x, y, src, srcLen, header.width, header.height);
}

Common::Rect BltImage::getRect(const Common::Point &pos) const {
	BltImageHeader header(_res.slice(0));
	Common::Rect result(0, 0, header.width, header.height);
	result.translate(header.offset.x, header.offset.y);
	result.translate(pos.x, pos.y);
	return result;
}

uint16 BltImage::getWidth() const {
	BltImageHeader header(_res.slice(0));
	return header.width;
}

uint16 BltImage::getHeight() const {
	BltImageHeader header(_res.slice(0));
	return header.height;
}

Common::Point BltImage::getOffset() const {
	BltImageHeader header(_res.slice(0));
	return header.offset;
}

} // End of namespace Bolt
