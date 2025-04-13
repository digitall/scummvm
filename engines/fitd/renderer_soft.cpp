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

#include "fitd/fitd.h"
#include "fitd/renderer.h"
#include "fitd/input.h"
#include "fitd/vars.h"
#include "engines/util.h"
#include "graphics/surface.h"

#define ROL16(x, b) (((x) << (b)) | ((x) >> (16 - (b))))

namespace Fitd {

enum {
	NUM_MAX_FLAT_VERTICES = 5000 * 3,
	MAX_POINTS_PER_POLY = 50,
	NUM_MAX_MASKS = 32,
	WIDTH = 320,
	HEIGHT = 200,
};

struct polyVertex {
	int16 X;
	int16 Y;
	int16 Z;
};

struct Mask {
	int16 roomId;
	int16 maskId;
	int16 maskX1;
	int16 maskY1;
	int16 maskX2;
	int16 maskY2;
	byte mask[WIDTH * HEIGHT];
};

struct ClipMask {
	int16 x;
	int16 y;
	int16 w;
	int16 h;
};

struct State {
	int16 tabVerticXmin[HEIGHT];
	int16 tabVerticXmax[HEIGHT];
	float tabVerticZmin[HEIGHT]; // TODO: improve his, this not necessary to store all these data
	float tabVerticZmax[HEIGHT];
	polyVertex flatVertices[NUM_MAX_FLAT_VERTICES];
	polyVertex tmpVertices[NUM_MAX_FLAT_VERTICES];
	byte physicalScreen[WIDTH * HEIGHT];
	byte physicalScreenRGB[WIDTH * HEIGHT * 3];
	float zBuffer[WIDTH * HEIGHT];
	byte RGB_Pal[256 * 3];

	int16 polyMinX;
	int16 polyMaxX;
	int16 polyMinY;
	int16 polyMaxY;

	ClipMask clipMask;
	Mask masks[NUM_MAX_MASKS];
	uint16 numMasks;
};

static State *_state = NULL;

static void renderer_init();
static void renderer_deinit();
static void renderer_startFrame();
static void renderer_drawBackground();
static void renderer_setPalette(const byte *palette);
static void renderer_copyBlockPhys(byte *videoBuffer, int left, int top, int right, int bottom);
static void renderer_fillPoly(const int16 *buffer, int numPoint, byte color, uint8 polyType);
static void renderer_refreshFrontTextureBuffer();
static void renderer_flushPendingPrimitives();
static void renderer_createMask(const uint8 *mask, int roomId, int maskId, byte *refImage, int maskX1, int maskY1, int maskX2, int maskY2);
static void renderer_setClip(float left, float top, float right, float bottom);
static void renderer_clearClip();
static void renderer_drawMask(int roomId, int maskId);
static void renderer_drawPoint(float X, float Y, float Z, uint8 color, uint8 material, float size);
static void renderer_updateScreen();
static Graphics::Surface *renderer_capture();

Renderer createSoftwareRenderer() {
	return Renderer{
		.init = renderer_init,
		.deinit = renderer_deinit,
		.startFrame = renderer_startFrame,
		.drawBackground = renderer_drawBackground,
		.setPalette = renderer_setPalette,
		.copyBlockPhys = renderer_copyBlockPhys,
		.fillPoly = renderer_fillPoly,
		.refreshFrontTextureBuffer = renderer_refreshFrontTextureBuffer,
		.flushPendingPrimitives = renderer_flushPendingPrimitives,
		.createMask = renderer_createMask,
		.setClip = renderer_setClip,
		.clearClip = renderer_clearClip,
		.drawMask = renderer_drawMask,
		.drawPoint = renderer_drawPoint,
		.updateScreen = renderer_updateScreen,
		.capture = renderer_capture,
	};
}

static void renderer_init() {
	_state = (State *)malloc(sizeof(State));
	_state->numMasks = 0;
	initGraphics(WIDTH, HEIGHT);
	g_engine->_screen = new Graphics::Screen(WIDTH, HEIGHT);
}

static void renderer_deinit() {
	delete g_engine->_screen;
	free(_state);
	_state = NULL;
}

static void renderer_startFrame() {
	g_engine->_screen->clear();
	g_engine->_screen->clearDirtyRects();
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		_state->zBuffer[i] = __FLT_MAX__;
	}
}

static void renderer_drawBackground() {
	g_engine->_screen->setPalette(_state->RGB_Pal);
	byte *screen = (byte *)g_engine->_screen->getBasePtr(0, 0);
	memcpy(screen, _state->physicalScreen, WIDTH * HEIGHT);
	g_engine->_screen->markAllDirty();
}

static void renderer_setPalette(const byte *palette) {
	memcpy(_state->RGB_Pal, palette, 256 * 3);
	g_engine->_screen->setPalette(_state->RGB_Pal);
}

static void renderer_copyBlockPhys(byte *videoBuffer, int left, int top, int right, int bottom) {
	byte *out = _state->physicalScreenRGB;
	const byte *in = (const byte *)&videoBuffer[0] + left + top * WIDTH;

	while ((right - left) % 4) {
		right++;
	}

	while ((bottom - top) % 4) {
		bottom++;
	}

	for (int i = top; i < bottom; i++) {
		in = (const byte *)&videoBuffer[0] + left + i * WIDTH;
		byte *out2 = _state->physicalScreen + left + i * WIDTH;
		for (int j = left; j < right; j++) {
			byte color = *(in++);

			*(out++) = _state->RGB_Pal[color * 3];
			*(out++) = _state->RGB_Pal[color * 3 + 1];
			*(out++) = _state->RGB_Pal[color * 3 + 2];

			*(out2++) = color;
		}
	}
}

static void renderer_refreshFrontTextureBuffer() {
	byte *out = _state->physicalScreenRGB;
	const byte *in = _state->physicalScreen;

	for (int i = 0; i < HEIGHT * WIDTH; i++) {
		byte color = *(in++);
		*(out++) = _state->RGB_Pal[color * 3];
		*(out++) = _state->RGB_Pal[color * 3 + 1];
		*(out++) = _state->RGB_Pal[color * 3 + 2];
	}
}

static void renderer_flushPendingPrimitives() {
}

static void renderer_createMask(const uint8 *mask, int roomId, int maskId, byte *refImage, int maskX1, int maskY1, int maskX2, int maskY2) {
	Mask *pMask = NULL;
	size_t i;
	for (i = 0; i < _state->numMasks; i++) {
		pMask = &_state->masks[i];
		if (pMask->roomId == roomId && pMask->maskId == maskId) {
			break;
		}
	}

	// no existing mask match, create a new one
	if (i == _state->numMasks) {
		pMask = &_state->masks[_state->numMasks++];
		assert(_state->numMasks < NUM_MAX_MASKS);
	}

	pMask->roomId = roomId;
	pMask->maskId = maskId;
	pMask->maskX1 = maskX1;
	pMask->maskY1 = maskY1;
	pMask->maskX2 = maskX2;
	pMask->maskY2 = maskY2;
	memcpy(pMask->mask, mask, sizeof(byte) * WIDTH * HEIGHT);
}

static void renderer_setClip(float left, float top, float right, float bottom) {

	_state->clipMask.x = left;
	_state->clipMask.y = top;
	_state->clipMask.w = right - left + 1;
	_state->clipMask.h = bottom - top + 1;

	_state->clipMask.x = CLIP(_state->clipMask.x, (int16)0, (int16)WIDTH);
	_state->clipMask.y = CLIP(_state->clipMask.y, (int16)0, (int16)HEIGHT);

	_state->clipMask.w = CLIP(_state->clipMask.w, (int16)0, (int16)(WIDTH - _state->clipMask.x));
	_state->clipMask.h = CLIP(_state->clipMask.h, (int16)0, (int16)(HEIGHT - _state->clipMask.y));
}

static void renderer_clearClip() {
	_state->clipMask.x = 0;
	_state->clipMask.y = 0;
	_state->clipMask.x = WIDTH;
	_state->clipMask.w = HEIGHT;
}

static void renderer_drawMask(int roomId, int maskId) {
	for (size_t i = 0; i < _state->numMasks; i++) {
		Mask *pMask = &_state->masks[i];
		if (pMask->roomId == roomId && pMask->maskId == maskId) {
			byte *s = (byte *)g_engine->_screen->getBasePtr(_state->clipMask.x, _state->clipMask.y);
			byte *p = &_state->physicalScreen[_state->clipMask.y * WIDTH + _state->clipMask.x];
			byte *m = &pMask->mask[_state->clipMask.y * WIDTH + _state->clipMask.x];
			for (size_t h = 0; h < _state->clipMask.h; h++) {
				for (size_t w = 0; w < _state->clipMask.w; w++) {
					if (*m) {
						*s = *p;
					}
					m++;
					p++;
					s++;
				}
				m += WIDTH - _state->clipMask.w;
				p += WIDTH - _state->clipMask.w;
				s += WIDTH - _state->clipMask.w;
			}
			return;
		}
	}
}

static int16 leftClip(polyVertex **polys, int16 num) {
	polyVertex *pVertex = polys[0];
	polyVertex *pTabPolyClip = polys[1];
	int16 newNbPoints = 0;

	// invert the pointers to continue on the clipped vertices in the next method
	polys[0] = pTabPolyClip;
	polys[1] = pVertex;

	for (int i = 0; i < num; i++, pVertex++) {
		const polyVertex *p0 = pVertex;
		const polyVertex *p1 = p0 + 1;

		// clipFlag :
		// 0x00 : none clipped
		// 0x01 : point 0 clipped
		// 0x02 : point 1 clipped
		// 0x03 : both clipped
		uint8 clipFlag = (p1->X < 0) ? 2 : 0;

		if (p0->X < 0) {
			if (clipFlag) {
				continue; // both clipped, skip point 0
			}
			clipFlag |= 1;
		} else {
			// point 0 not clipped, store it
			*pTabPolyClip++ = *pVertex;
			++newNbPoints;
		}

		if (clipFlag) {
			// point 0 or 1 is clipped, apply clipping
			if (p1->X >= p0->X) {
				p0 = p1;
				p1 = pVertex;
			}

			const int32 dx = p1->X - p0->X;
			const int32 dy = p1->Y - p0->Y;
			const int32 dz = p1->Z - p0->Z;
			const int32 dxClip = 0 - p0->X;

			pTabPolyClip->X = (int16)0;
			pTabPolyClip->Y = (int16)(p0->Y + ((dxClip * dy) / dx));
			pTabPolyClip->Z = (p0->Z + ((float)(dxClip * dz) / dx));

			++pTabPolyClip;
			++newNbPoints;
		}
	}

	// copy first vertex to the end
	*pTabPolyClip = polys[0][0];
	return newNbPoints;
}

static int16 rightClip(polyVertex **polys, int16 num) {
	polyVertex *pVertex = polys[0];
	polyVertex *pTabPolyClip = polys[1];
	int16 newNbPoints = 0;

	// invert the pointers to continue on the clipped vertices in the next method
	polys[0] = pTabPolyClip;
	polys[1] = pVertex;

	for (int i = 0; i < num; i++, pVertex++) {
		const polyVertex *p0 = pVertex;
		const polyVertex *p1 = p0 + 1;

		// clipFlag :
		// 0x00 : none clipped
		// 0x01 : point 0 clipped
		// 0x02 : point 1 clipped
		// 0x03 : both clipped
		uint8 clipFlag = (p1->X >= WIDTH) ? 2 : 0;

		if (p0->X >= WIDTH) {
			if (clipFlag) {
				continue; // both clipped, skip point 0
			}
			clipFlag |= 1;
		} else {
			// point 0 not clipped, store it
			*pTabPolyClip++ = *pVertex;
			++newNbPoints;
		}

		if (clipFlag) {
			// point 0 or 1 is clipped, apply clipping
			if (p1->X >= p0->X) {
				p0 = p1;
				p1 = pVertex;
			}

			const int32 dx = p1->X - p0->X;
			const int32 dy = p1->Y - p0->Y;
			const int32 dz = p1->Z - p0->Z;
			const int32 dxClip = (WIDTH - 1) - p0->X;

			pTabPolyClip->X = (int16)(WIDTH - 1);
			pTabPolyClip->Y = (int16)(p0->Y + ((dxClip * dy) / dx));
			pTabPolyClip->Z = (p0->Z + ((float)(dxClip * dz) / dx));

			++pTabPolyClip;
			++newNbPoints;
		}
	}

	// copy first vertex to the end
	*pTabPolyClip = polys[0][0];
	return newNbPoints;
}

static int16 topClip(polyVertex **polys, int16 num) {
	polyVertex *pVertex = polys[0];
	polyVertex *pTabPolyClip = polys[1];
	int16 newNbPoints = 0;

	// invert the pointers to continue on the clipped vertices in the next method
	polys[0] = pTabPolyClip;
	polys[1] = pVertex;

	for (int i = 0; i < num; i++, pVertex++) {
		const polyVertex *p0 = pVertex;
		const polyVertex *p1 = p0 + 1;

		// clipFlag :
		// 0x00 : none clipped
		// 0x01 : point 0 clipped
		// 0x02 : point 1 clipped
		// 0x03 : both clipped
		uint8 clipFlag = (p1->Y < 0) ? 2 : 0;

		if (p0->Y < 0) {
			if (clipFlag) {
				continue; // both clipped, skip point 0
			}
			clipFlag |= 1;
		} else {
			// point 0 not clipped, store it
			*pTabPolyClip++ = *pVertex;
			++newNbPoints;
		}

		if (clipFlag) {
			// point 0 or 1 is clipped, apply clipping
			if (p1->Y >= p0->Y) {
				p0 = p1;
				p1 = pVertex;
			}

			const int32 dx = p1->X - p0->X;
			const int32 dy = p1->Y - p0->Y;
			const int32 dz = p1->Z - p0->Z;
			const int32 dyClip = 0 - p0->Y;

			pTabPolyClip->X = (int16)(p0->X + ((dyClip * dx) / dy));
			pTabPolyClip->Y = (int16)0;
			pTabPolyClip->Z = (p0->Z + ((float)(dyClip * dz) / dy));

			++pTabPolyClip;
			++newNbPoints;
		}
	}

	// copy first vertex to the end
	*pTabPolyClip = polys[0][0];
	return newNbPoints;
}

static int16 bottomClip(polyVertex **polys, int16 num) {
	polyVertex *pVertex = polys[0];
	polyVertex *pTabPolyClip = polys[1];
	int16 newNbPoints = 0;

	// invert the pointers to continue on the clipped vertices in the next method
	polys[0] = pTabPolyClip;
	polys[1] = pVertex;

	for (int i = 0; i < num; i++, pVertex++) {
		const polyVertex *p0 = pVertex;
		const polyVertex *p1 = p0 + 1;

		// clipFlag :
		// 0x00 : none clipped
		// 0x01 : point 0 clipped
		// 0x02 : point 1 clipped
		// 0x03 : both clipped
		uint8 clipFlag = (p1->Y >= HEIGHT) ? 2 : 0;

		if (p0->Y >= HEIGHT) {
			if (clipFlag) {
				continue; // both clipped, skip point 0
			}
			clipFlag |= 1;
		} else {
			// point 0 not clipped, store it
			*pTabPolyClip++ = *pVertex;
			++newNbPoints;
		}

		if (clipFlag) {
			// point 0 or 1 is clipped, apply clipping
			if (p1->Y >= p0->Y) {
				p0 = p1;
				p1 = pVertex;
			}

			const int32 dx = p1->X - p0->X;
			const int32 dy = p1->Y - p0->Y;
			const int32 dz = p1->Z - p0->Z;
			const int32 dyClip = (HEIGHT - 1) - p0->Y;

			pTabPolyClip->X = (int16)(p0->X + ((dyClip * dx) / dy));
			pTabPolyClip->Y = (int16)(HEIGHT - 1);
			pTabPolyClip->Z = (p0->Z + ((float)(dyClip * dz) / dy));

			++pTabPolyClip;
			++newNbPoints;
		}
	}

	// copy first vertex to the end
	*pTabPolyClip = polys[0][0];
	return newNbPoints;
}

static int16 poly_clip(polyVertex **polys, int16 num) {
	bool hasBeenClipped = false;
	int32 clippedNum = num;
	if (_state->polyMinX < 0) {
		clippedNum = leftClip(polys, clippedNum);
		if (!clippedNum) {
			return 0;
		}
		_state->polyMinX = 0;
		hasBeenClipped = true;
	}
	if (_state->polyMaxX >= WIDTH) {
		clippedNum = rightClip(polys, clippedNum);
		if (!clippedNum) {
			return 0;
		}
		_state->polyMaxX = (WIDTH - 1);
		hasBeenClipped = true;
	}
	if (_state->polyMinY < 0) {
		clippedNum = topClip(polys, clippedNum);
		if (!clippedNum) {
			return 0;
		}
		hasBeenClipped = true;
	}
	if (_state->polyMaxY >= HEIGHT) {
		clippedNum = bottomClip(polys, clippedNum);
		if (!clippedNum) {
			return 0;
		}
		hasBeenClipped = true;
	}

	if (hasBeenClipped) {
		// search the new Ymin or Ymax
		_state->polyMinY = 32767;
		_state->polyMaxY = -32768;

		for (int32 n = 0; n < clippedNum; ++n) {
			if (polys[0][n].Y < _state->polyMinY) {
				_state->polyMinY = polys[0][n].Y;
			}

			if (polys[0][n].Y > _state->polyMaxY) {
				_state->polyMaxY = polys[0][n].Y;
			}
		}

		if (_state->polyMinY >= _state->polyMaxY) {
			return 0; // No valid polygon after clipping
		}
	}

	return clippedNum;
}

static void poly_setMinMax(polyVertex *pTabPoly, int16 num) {
	int32 incY = -1;
	float *pZ;
	for (int i = 0; i < num; i++, pTabPoly++) {
		const polyVertex *p0 = pTabPoly;
		const polyVertex *p1 = p0 + 1;
		int16 *pVertic;

		int32 dy = p1->Y - p0->Y;
		if (dy == 0) {
			// forget same Y points
			continue;
		} else if (dy > 0) {
			// Y therefore goes down x max buffer
			if (p0->X <= p1->X) {
				incY = 1;
			} else {
				p0 = p1;
				p1 = pTabPoly;
				incY = -1;
			}

			pVertic = &_state->tabVerticXmax[p0->Y];
			pZ = &_state->tabVerticZmax[p0->Y];
		} else if (dy < 0) {
			dy = -dy;

			if (p0->X <= p1->X) {
				p0 = p1;
				p1 = pTabPoly;
				incY = 1;
			} else {
				incY = -1;
			}

			pVertic = &_state->tabVerticXmin[p0->Y];
			pZ = &_state->tabVerticZmin[p0->Y];
		}

		int32 dx = (p1->X - p0->X) << 16;
		int32 step = dx / dy;
		int32 reminder = ((dx % dy) >> 1) + 0x7FFF;
		float dz = (float)(p1->Z - p0->Z) / dy;

		dx = step >> 16; // recovery part high division (entire)
		step &= 0xFFFF;  // preserves lower part (mantissa)
		int32 x = p0->X;
		float z = p0->Z;

		for (int32 y = 0; y <= dy; y++) {
			*pVertic = (int16)x;
			*pZ = z;
			assert(x >= 0);
			assert(x < WIDTH);
			assert(z >= 0);
			assert(z < 32000);
			pVertic += incY;
			x += dx;
			reminder += step;
			if (reminder & 0xFFFF0000) {
				x += reminder >> 16;
				reminder &= 0xFFFF;
			}
			pZ += incY;
			z += dz;
		}
	}
}

static void set_boundingBox(const polyVertex *pTabPoly, int16 num) {
	// compute the polygon bounding box
	_state->polyMinX = INT16_MAX;
	_state->polyMaxX = INT16_MIN;
	_state->polyMinY = INT16_MAX;
	_state->polyMaxY = INT16_MIN;

	for (int i = 0; i < num; i++, pTabPoly++) {
		const int16 X = pTabPoly->X;
		const int16 Y = pTabPoly->Y;

		if (X > _state->polyMaxX)
			_state->polyMaxX = X;
		if (X < _state->polyMinX)
			_state->polyMinX = X;

		if (Y > _state->polyMaxY)
			_state->polyMaxY = Y;
		if (Y < _state->polyMinY)
			_state->polyMinY = Y;
	}
}

static int16 prepare_poly(const int16 *buffer, polyVertex *pTabPoly, int16 num) {
	polyVertex *pVertex = pTabPoly;

	for (int i = 0; i < num; i++) {
		pVertex->X = buffer[i * 3 + 0];
		pVertex->Y = buffer[i * 3 + 1];
		pVertex->Z = buffer[i * 3 + 2];
		pVertex++;
	}

	// num = 4;
	// polyVertex *pVertex = pTabPoly;
	// pVertex->X = -24;
	// pVertex->Y = 1;
	// pVertex->Z = 379;
	// pVertex++;
	// pVertex->X = 0;
	// pVertex->Y = 10;
	// pVertex->Z = 439;
	// pVertex++;
	// pVertex->X = -10;
	// pVertex->Y = 13;
	// pVertex->Z = 411;
	// pVertex++;
	// pVertex->X = -23;
	// pVertex->Y = 15;
	// pVertex->Z = 381;
	// pVertex++;

	set_boundingBox(pTabPoly, num);

	// no vertices
	if (_state->polyMinY > _state->polyMaxY || _state->polyMaxX < 0 || _state->polyMinX >= WIDTH || _state->polyMaxY < 0 || _state->polyMinY >= HEIGHT) {
		return 0;
	}

	// copy the first point to the end of the polygon
	pTabPoly[num] = pTabPoly[0];
	polyVertex *polys[] = {pTabPoly, _state->tmpVertices};

	num = poly_clip(polys, num);
	if (!num)
		return 0;

	poly_setMinMax(polys[0], num);
	return num;
}

static void renderer_fillPoly(const int16 *buffer, int numPoint, byte color, uint8 polyType) {
	assert(numPoint < MAX_POINTS_PER_POLY);

	if (g_engine->shouldQuit())
		return;

	if (!prepare_poly(buffer, _state->flatVertices, numPoint))
		return;

	if (_state->polyMinY == _state->polyMaxY && _state->polyMinX == _state->polyMaxX)
		return;

	assert(_state->polyMinY >= 0);
	assert(_state->polyMaxY < HEIGHT);
	assert(_state->polyMinX >= 0);
	assert(_state->polyMaxX < WIDTH);
	int16 y = _state->polyMinY;
	byte *pDestLine = (uint8 *)g_engine->_screen->getBasePtr(0, y);
	const int16 *pVerticXmin = &_state->tabVerticXmin[y];
	const int16 *pVerticXmax = &_state->tabVerticXmax[y];
	const float *pVerticZmin = &_state->tabVerticZmin[y];
	const float *pVerticZmax = &_state->tabVerticZmax[y];
	float *zBuffer = &_state->zBuffer[y * WIDTH];

	switch (polyType) {
	case 0: // flat (triste)
	{
		for (; y <= _state->polyMaxY; y++) {
			int16 xMin = *pVerticXmin++;
			const int16 xMax = *pVerticXmax++;
			float zMin = *pVerticZmin++;
			const float zMax = *pVerticZmax++;
			float dz = (zMax - zMin) / MAX(1, xMax - xMin);
			assert(zMin >= 0);
			assert(zMax >= 0);

			byte *pDest = pDestLine + xMin;
			float z = zMin;
			for (int16 x = xMin; x <= xMax; x++) {
				if (z < zBuffer[x]) {
					*pDest = (byte)color;
					zBuffer[x] = z;
				}
				pDest++;
				z += dz;
			}

			pDestLine += WIDTH;
			zBuffer += WIDTH;
			if (Debug && xMin <= xMax) {
				g_engine->_screen->addDirtyRect(Common::Rect(Common::Point(xMin, y), xMax - xMin + 1, 1));
				renderer_updateScreen();
				readKeyboard();
				if (g_engine->shouldQuit())
					return;
			}
		}
		break;
	}
	case 1: // dither (pierre/tele)
	{
		int16 acc = 17371;

		for (; y <= _state->polyMaxY; y++) {
			int16 xMin = *pVerticXmin++;
			const int16 xMax = *pVerticXmax++;
			float zMin = *pVerticZmin++;
			const float zMax = *pVerticZmax++;
			float dz = (zMax - zMin) / MAX(1, xMax - xMin);
			assert(zMin >= 0);
			assert(zMin < 32000);
			assert(zMax >= 0);
			assert(zMax < 32000);
			byte col = xMin;

			byte *pDest = pDestLine + xMin;
			float z = zMin;
			for (int16 x = xMin; x <= xMax; x++) {
				col = ((col + acc) & 0xFF03) + (uint16)color;
				acc = ROL16(acc, 2) + 1;
				if (z < zBuffer[x]) {
					*pDest = (byte)col;
					zBuffer[x] = z;
				}
				pDest++;
				z += dz;
			}

			pDestLine += WIDTH;
			zBuffer += WIDTH;
			if (Debug && xMin <= xMax) {
				g_engine->_screen->addDirtyRect(Common::Rect(Common::Point(xMin, y), xMax - xMin + 1, 1));
				renderer_updateScreen();
				readKeyboard();
				if (g_engine->shouldQuit())
					return;
			}
		}
		break;
	}
	case 2: // trans
	{
		// TODO: fix this, it should be drawn last
		// actually, we should render all polygons in flush method and draw transparent polygons last
		for (; y <= _state->polyMaxY; y++) {
			int16 xMin = *pVerticXmin++;
			const int16 xMax = *pVerticXmax++;
			float zMin = *pVerticZmin++;
			const float zMax = *pVerticZmax++;
			float dz = (zMax - zMin) / MAX(1, xMax - xMin);
			assert(zMin >= 0);
			assert(zMin < 32000);
			assert(zMax >= 0);
			assert(zMax < 32000);

			byte *pDest = pDestLine + xMin;
			float z = zMin;
			for (int16 x = xMin; x <= xMax; x++) {
				if (z < zBuffer[x]) {
					*pDest = (byte)color | (*pDest & 0x0F);
					zBuffer[x] = z;
				}
				pDest++;
				z += dz;
			}

			pDestLine += WIDTH;
			zBuffer += WIDTH;
			if (Debug && xMin <= xMax) {
				g_engine->_screen->addDirtyRect(Common::Rect(Common::Point(xMin, y), xMax - xMin + 1, 1));
				renderer_updateScreen();
				readKeyboard();
				if (g_engine->shouldQuit())
					return;
			}
		}
		break;
	}
	case 4: // copper (ramps top to bottom)
	{
		int32 sens = 1;
		for (; y <= _state->polyMaxY; y++) {
			int16 xMin = *pVerticXmin++;
			const int16 xMax = *pVerticXmax++;
			float zMin = *pVerticZmin++;
			const float zMax = *pVerticZmax++;
			float dz = (zMax - zMin) / MAX(1, xMax - xMin);
			assert(zMin >= 0);
			assert(zMin < 32000);
			assert(zMax >= 0);
			assert(zMax < 32000);

			byte *pDest = pDestLine + xMin;
			float z = zMin;
			for (int16 x = xMin; x <= xMax; x++) {
				if (z < zBuffer[x]) {
					*pDest = (byte)color;
					zBuffer[x] = z;
				}
				pDest++;
				z += dz;
			}

			color += sens;
			if (!(color & 0xF)) {
				sens = -sens;
				if (sens < 0) {
					color += sens;
				}
			}

			pDestLine += WIDTH;
			zBuffer += WIDTH;
			if (Debug && xMin <= xMax) {
				g_engine->_screen->addDirtyRect(Common::Rect(Common::Point(xMin, y), xMax - xMin + 1, 1));
				renderer_updateScreen();
				readKeyboard();
				if (g_engine->shouldQuit())
					return;
			}
		}
		break;
	}
	case 5: // copper2 (ramps top to bottom, 2 scanline per color)
	{
		int32 sens = 1;
		int32 line = 2;

		for (; y <= _state->polyMaxY; y++) {
			int16 xMin = *pVerticXmin++;
			const int16 xMax = *pVerticXmax++;
			float zMin = *pVerticZmin++;
			const float zMax = *pVerticZmax++;
			float dz = (zMax - zMin) / MAX(1, xMax - xMin);
			assert(zMin >= 0);
			assert(zMin < 32000);
			assert(zMax >= 0);
			assert(zMax < 32000);

			byte *pDest = pDestLine + xMin;
			float z = zMin;
			for (int16 x = xMin; x <= xMax; x++) {
				if (z < zBuffer[x]) {
					*pDest++ = (byte)color;
					zBuffer[x] = z;
				}
				z += dz;
			}

			line--;
			if (!line) {
				line = 2;
				color += sens;
				if (!(color & 0xF)) {
					sens = -sens;
					if (sens < 0) {
						color += sens;
					}
				}
			}

			pDestLine += WIDTH;
			zBuffer += WIDTH;
			if (Debug && xMin <= xMax) {
				g_engine->_screen->addDirtyRect(Common::Rect(Common::Point(xMin, y), xMax - xMin + 1, 1));
				renderer_updateScreen();
				readKeyboard();
				if (g_engine->shouldQuit())
					return;
			}
		}
		break;
	}
	case 3: // marbre (ramp left to right)
	{
		byte start = (color & 0x0F);
		byte bank = color & 0xF0;
		for (; y <= _state->polyMaxY; y++) {
			int16 xMin = *pVerticXmin++;
			const int16 xMax = *pVerticXmax++;
			float zMin = *pVerticZmin++;
			const float zMax = *pVerticZmax++;
			float dz = (zMax - zMin) / MAX(1, xMax - xMin);
			assert(zMin >= 0);
			assert(zMin < 32000);
			assert(zMax >= 0);
			assert(zMax < 32000);

			byte *pDest = pDestLine + xMin;

			int32 step;
			int32 dx = xMax - xMin;
			float z = zMin;

			if (dx == 0) {
				if (z < zBuffer[xMin]) {
					// just one
					*pDest = bank | start;
				}
				pDest++;
			} else if (dx > 0) {
				step = 15 / (dx + 1);
				color = start;

				for (int16 x = xMin; x <= xMax; x++) {
					if (z < zBuffer[x]) {
						*pDest = bank | color;
						zBuffer[x] = z;
					}
					color += step;
					pDest++;
					z += dz;
				}
			}

			pDestLine += WIDTH;
			zBuffer += WIDTH;
			if (Debug && xMin <= xMax) {
				g_engine->_screen->addDirtyRect(Common::Rect(Common::Point(xMin, y), xMax - xMin + 1, 1));
				renderer_updateScreen();
				readKeyboard();
				if (g_engine->shouldQuit())
					return;
			}
		}
		break;
	}
	case 6: // marbre2 (ramp right to left)
	{
		byte start = (color & 0x0F);
		byte bank = color & 0xF0;
		for (; y <= _state->polyMaxY; y++) {
			int16 xMin = *pVerticXmin++;
			const int16 xMax = *pVerticXmax++;
			float zMin = *pVerticZmin++;
			const float zMax = *pVerticZmax++;
			float dz = (zMax - zMin) / MAX(1, xMax - xMin);
			assert(zMin >= 0);
			assert(zMin < 32000);
			assert(zMax >= 0);
			assert(zMax < 32000);

			byte *pDest = pDestLine + xMin;

			int32 step;
			int32 dx = xMax - xMin;
			float z = zMin;

			if (dx == 0) {
				if (z < zBuffer[xMin]) {
					// just one
					*pDest = bank | start;
				}
				pDest++;
			} else if (dx > 0) {
				step = 15 / (dx + 1);
				color = start;

				for (int16 x = xMin; x <= xMax; x++) {
					if (z < zBuffer[x]) {
						*pDest = bank | (15 - color);
						zBuffer[x] = z;
					}
					color += step;
					pDest++;
					z += dz;
				}
			}

			pDestLine += WIDTH;
			zBuffer += WIDTH;
			if (Debug && xMin <= xMax) {
				g_engine->_screen->addDirtyRect(Common::Rect(Common::Point(xMin, y), xMax - xMin + 1, 1));
				renderer_updateScreen();
				readKeyboard();
				if (g_engine->shouldQuit())
					return;
			}
		}
		break;
	}
	}
}

static bool inCircle(int pX, int pY, int cX, int cY, float radius) {
	int dx = ABS(pX - cX);
	int dy = ABS(pY - cY);
	return (dx * dx + dy * dy <= radius * radius);
}

static bool computeSphere(float sx, float sy, float sz, float radius) {
	if (radius <= 0) {
		return false;
	}
	int16 left = (int16)(sx - radius);
	int16 right = (int16)(sx + radius);
	int16 bottom = (int16)(sy + radius);
	int16 top = (int16)(sy - radius);
	const Common::Rect &clip = Common::Rect(0, 0, WIDTH - 1, HEIGHT - 1);
	int16 cleft = clip.left;
	int16 cright = clip.right;
	int16 ctop = clip.top;
	int16 cbottom = clip.bottom;

	if (left < cleft) {
		left = cleft;
	}
	if (bottom > cbottom) {
		bottom = cbottom;
	}
	if (right > cright) {
		right = cright;
	}
	if (top < ctop) {
		top = ctop;
	}

	for (int16 y = top; y <= bottom; y++) {
		_state->tabVerticXmin[y] = INT16_MAX;
		_state->tabVerticXmax[y] = INT16_MIN;
		_state->tabVerticZmin[y] = sz;
		_state->tabVerticZmax[y] = sz;
		bool inside = false;
		for (int16 x = left; x <= right; x++) {
			if (inCircle(x, y, sx, sy, radius)) {
				inside = true;
				_state->tabVerticXmin[y] = MIN(x, _state->tabVerticXmin[y]);
				_state->tabVerticXmax[y] = MAX(x, _state->tabVerticXmax[y]);
			}
		}
		if(!inside) {
			_state->tabVerticXmin[y] = 0;
			_state->tabVerticXmax[y] = 0;
			_state->tabVerticZmin[y] = __FLT_MAX__;
			_state->tabVerticZmax[y] = __FLT_MAX__;
		}
	}

	_state->polyMinY = top;
	_state->polyMaxY = bottom;

	return true;
}

static void renderer_drawPoint(float sX, float sY, float sZ, uint8 color, uint8 material, float size) {
	if (computeSphere(sX, sY, sZ, size)) {
		int16 y = _state->polyMinY;
		byte *pDestLine = (uint8 *)g_engine->_screen->getBasePtr(0, y);
		const float *pVerticZmin = &_state->tabVerticZmin[y];
		const float *pVerticZmax = &_state->tabVerticZmax[y];
		float *zBuffer = &_state->zBuffer[y * WIDTH];
		const int16 *xMins = &_state->tabVerticXmin[y];
		const int16 *xMaxs = &_state->tabVerticXmax[y];

		for (; y <= _state->polyMaxY; y++) {
			const int16 xMin = *xMins++;
			const int16 xMax = *xMaxs++;
			assert(xMin >= 0);
			assert(xMin < WIDTH);
			assert(xMax >= 0);
			assert(xMax < WIDTH);
			float zMin = *pVerticZmin++;
			const float zMax = *pVerticZmax++;
			float dz = (zMax - zMin) / MAX(1, xMax - xMin);

			byte *pDest = pDestLine + xMin;
			float z = zMin;
			for (int16 x = xMin; x <= xMax; x++) {
				if (z < zBuffer[x]) {
					*pDest = (byte)color;
					zBuffer[x] = z;
				}
				pDest++;
				z += dz;
			}

			pDestLine += WIDTH;
			zBuffer += WIDTH;
		}
	}
}

static void renderer_updateScreen() {
	g_engine->_screen->update();
}

Graphics::Surface *renderer_capture() {
	Graphics::Surface *s = new Graphics::Surface();
#ifdef SCUMM_BIG_ENDIAN
	Graphics::PixelFormat format = Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0);
#else
	Graphics::PixelFormat format = Graphics::PixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24);
#endif
	s->create(WIDTH, HEIGHT, format);
	byte *src = _state->physicalScreen;
	byte *dst = (byte *)s->getPixels();
	byte *pal = _state->RGB_Pal;
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		dst[0] = pal[*src * 3];
		dst[1] = pal[*src * 3 + 1];
		dst[2] = pal[*src * 3 + 2];
		dst[3] = 0xFF;
		dst += 4;
		src++;
	}
	return s;
}

} // namespace Fitd
