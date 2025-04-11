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

// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include "stb_image_write.h"

#include "fitd/fitd.h"
#include "fitd/renderer.h"
#include "fitd/input.h"
#include "fitd/vars.h"
#include "engines/util.h"
#include "graphics/surface.h"

namespace Fitd {

enum {
	NUM_MAX_FLAT_VERTICES = 5000 * 3,
	MAX_POINTS_PER_POLY = 50,
	NUM_MAX_MASKS = 32,
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
	byte mask[320 * 200];
};

struct ClipMask {
	int16 x;
	int16 y;
	int16 w;
	int16 h;
};

struct State {
	int16 tabVerticXmin[200];
	int16 tabVerticXmax[200];
	float tabVerticZmin[200]; // TODO: improve his, this not necessary to store all these data
	float tabVerticZmax[200];
	polyVertex flatVertices[NUM_MAX_FLAT_VERTICES];
	polyVertex tmpVertices[NUM_MAX_FLAT_VERTICES];
	byte physicalScreen[320 * 200];
	byte physicalScreenRGB[320 * 200 * 3];
	float zBuffer[320 * 200];
	float tmpZBuffer[320 * 200];
	byte RGB_Pal[256 * 3];

	int16 polyMinX;
	int16 polyMaxX;
	int16 polyMinY;
	int16 polyMaxY;

	ClipMask clipMask;
	Mask masks[NUM_MAX_MASKS];
	uint16 numMasks = 0;
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
Graphics::Surface *renderer_capture();

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
	};
}

static void renderer_init() {
	_state = (State *)malloc(sizeof(State));
	initGraphics(320, 200);
	g_engine->_screen = new Graphics::Screen(320, 200);

	memset(_state->tabVerticXmin, 0, sizeof(int16) * 200);
	memset(_state->tabVerticXmax, 0, sizeof(int16) * 200);
}

static void renderer_deinit() {
	delete g_engine->_screen;
	free(_state);
	_state = NULL;
}

static void renderer_startFrame() {
	g_engine->_screen->clear();
	g_engine->_screen->clearDirtyRects();
	for (int i = 0; i < 320 * 200; i++) {
		_state->zBuffer[i] = __FLT_MAX__;
	}
}

static void renderer_drawBackground() {
	g_engine->_screen->setPalette(_state->RGB_Pal);
	byte *screen = (byte *)g_engine->_screen->getBasePtr(0, 0);
	memcpy(screen, _state->physicalScreen, 320 * 200);
	g_engine->_screen->markAllDirty();
}

static void renderer_setPalette(const byte *palette) {
	memcpy(_state->RGB_Pal, palette, 256 * 3);
	g_engine->_screen->setPalette(_state->RGB_Pal);
}

static void renderer_copyBlockPhys(byte *videoBuffer, int left, int top, int right, int bottom) {
	byte *out = _state->physicalScreenRGB;
	const byte *in = (const byte *)&videoBuffer[0] + left + top * 320;

	while ((right - left) % 4) {
		right++;
	}

	while ((bottom - top) % 4) {
		bottom++;
	}

	for (int i = top; i < bottom; i++) {
		in = (const byte *)&videoBuffer[0] + left + i * 320;
		byte *out2 = _state->physicalScreen + left + i * 320;
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

	for (int i = 0; i < 200 * 320; i++) {
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
	memcpy(pMask->mask, mask, sizeof(byte) * 320 * 200);
}

static void renderer_setClip(float left, float top, float right, float bottom) {

	_state->clipMask.x = left - 1;
	_state->clipMask.y = top - 1;
	_state->clipMask.w = right - left + 2;
	_state->clipMask.h = bottom - top + 2;

	_state->clipMask.x = MAX(_state->clipMask.x, (int16)0);
	_state->clipMask.y = MAX(_state->clipMask.y, (int16)0);
}

static void renderer_clearClip() {
	_state->clipMask.x = 0;
	_state->clipMask.y = 0;
	_state->clipMask.x = 320;
	_state->clipMask.w = 200;
}

static void renderer_drawMask(int roomId, int maskId) {
	for (size_t i = 0; i < _state->numMasks; i++) {
		Mask *pMask = &_state->masks[i];
		if (pMask->roomId == roomId && pMask->maskId == maskId) {
			byte *s = (byte *)g_engine->_screen->getBasePtr(_state->clipMask.x, _state->clipMask.y);
			byte *p = &_state->physicalScreen[_state->clipMask.y * 320 + _state->clipMask.x];
			byte *m = &pMask->mask[_state->clipMask.y * 320 + _state->clipMask.x];
			for (size_t h = 0; h < _state->clipMask.h; h++) {
				for (size_t w = 0; w < _state->clipMask.w; w++) {
					if (*m) {
						*s = *p;
					}
					m++;
					p++;
					s++;
				}
				m += 320 - _state->clipMask.w;
				p += 320 - _state->clipMask.w;
				s += 320 - _state->clipMask.w;
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
		uint8 clipFlag = (p1->X > 319) ? 2 : 0;

		if (p0->X > 319) {
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
			const int32 dxClip = 319 - p0->X;

			pTabPolyClip->X = (int16)319;
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
		uint8 clipFlag = (p1->Y > 199) ? 2 : 0;

		if (p0->Y > 199) {
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
			const int32 dyClip = 199 - p0->Y;

			pTabPolyClip->X = (int16)(p0->X + ((dyClip * dx) / dy));
			pTabPolyClip->Y = (int16)199;
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
	if (_state->polyMaxX > 319) {
		clippedNum = rightClip(polys, clippedNum);
		if (!clippedNum) {
			return 0;
		}
		_state->polyMaxX = 319;
		hasBeenClipped = true;
	}
	if (_state->polyMinY < 0) {
		clippedNum = topClip(polys, clippedNum);
		if (!clippedNum) {
			return 0;
		}
		_state->polyMinY = 0;
		hasBeenClipped = true;
	}
	if (_state->polyMaxY > 199) {
		clippedNum = bottomClip(polys, clippedNum);
		if (!clippedNum) {
			return 0;
		}
		_state->polyMaxY = 199;
		hasBeenClipped = true;
	}

	if (hasBeenClipped) {
		if (_state->polyMinY >= _state->polyMaxY) {
			return 0; // No valid polygon after clipping
		}
	}

	return clippedNum;
}

static void poly_setMinMax(polyVertex *pTabPoly, int16 num) {
	int32 incY = -1;
	int32 dx, dy, x, y;
	int32 step, reminder;
	const polyVertex *p0;
	const polyVertex *p1;
	int16 *pVertic;
	float *pZ;
	for (int i = 0; i < num; i++, pTabPoly++) {
		p0 = pTabPoly;
		p1 = p0 + 1;

		dy = p1->Y - p0->Y;
		if (dy == 0) {
			// forget same Y points
			continue;
		} else if (dy > 0) {
			// Y therefore goes down left buffer
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

		dx = (p1->X - p0->X) << 16;
		step = dx / dy;
		reminder = ((dx % dy) >> 1) + 0x7FFF;
		float dz = (float)(p1->Z - p0->Z) / dy;

		dx = step >> 16; // recovery part high division (entire)
		step &= 0xFFFF;  // preserves lower part (mantissa)
		x = p0->X;
		float z = p0->Z;

		for (y = dy; y >= 0; --y) {
			*pVertic = (int16)x;
			*pZ = z;
			assert(x >= 0);
			assert(x < 320);
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
	if (_state->polyMinY > _state->polyMaxY || _state->polyMaxX < 0 || _state->polyMinX > 319 || _state->polyMaxY < 0 || _state->polyMinY > 199) {
		return 0;
	}

	pTabPoly[num] = pTabPoly[0];
	polyVertex *polys[] = {pTabPoly, _state->tmpVertices};

	num = poly_clip(polys, num);
	if (!num)
		return 0;

	pVertex = polys[0];

	poly_setMinMax(pVertex, num);
	return num;
}

static void renderer_fillPoly(const int16 *buffer, int numPoint, byte color, uint8 polyType) {
	assert(numPoint < MAX_POINTS_PER_POLY);

	if (g_engine->shouldQuit())
		return;

	switch (polyType) {
	default:
	case 0: // flat (triste)
	{
		if (!prepare_poly(buffer, _state->flatVertices, numPoint))
			return;

		assert(_state->polyMinY >= 0);
		assert(_state->polyMaxY < 200);
		assert(_state->polyMinX >= 0);
		assert(_state->polyMaxX < 320);
		int16 y = _state->polyMinY;
		byte *pDestLine = (uint8 *)g_engine->_screen->getBasePtr(0, y);
		const int16 *pVerticXmin = &_state->tabVerticXmin[y];
		const int16 *pVerticXmax = &_state->tabVerticXmax[y];
		const float *pVerticZmin = &_state->tabVerticZmin[y];
		const float *pVerticZmax = &_state->tabVerticZmax[y];
		float *zBuffer = &_state->zBuffer[y * 320];

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

			pDestLine += 320;
			zBuffer += 320;
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
		// case 1: // dither (pierre/tele)
		// {
		// 	polyVertex *pVertex = &noiseVertices[numUsedNoiseVertices];
		// 	numUsedNoiseVertices += (numPoint - 2) * 3;
		// 	assert(numUsedNoiseVertices < NUM_MAX_NOISE_VERTICES);

		// 	for (int i = 0; i < numPoint; i++) {
		// 		if (i >= 3) {
		// 			memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
		// 			pVertex++;
		// 			memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
		// 			pVertex++;
		// 		}

		// 		pVertex->X = buffer[i * 3 + 0];
		// 		pVertex->Y = buffer[i * 3 + 1];
		// 		pVertex->Z = buffer[i * 3 + 2];

		// 		// int bank = (color & 0xF0) >> 4;
		// 		// int startColor = color & 0xF;
		// 		// float colorf = startColor;
		// 		// pVertex->U = colorf / 15.f;
		// 		// pVertex->V = bank / 15.f;
		// 		pVertex->R = RGB_Pal[color * 3];
		// 		pVertex->G = RGB_Pal[color * 3 + 1];
		// 		pVertex->B = RGB_Pal[color * 3 + 2];
		// 		pVertex++;
		// 	}
		// 	break;
		// }
		// case 2: // trans
		// {
		// 	polyVertex *pVertex = &transparentVertices[numUsedTransparentVertices];
		// 	numUsedTransparentVertices += (numPoint - 2) * 3;
		// 	assert(numUsedTransparentVertices < NUM_MAX_TRANSPARENT_VERTICES);

		// 	for (int i = 0; i < numPoint; i++) {
		// 		if (i >= 3) {
		// 			memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
		// 			pVertex++;
		// 			memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
		// 			pVertex++;
		// 		}

		// 		pVertex->X = buffer[i * 3 + 0];
		// 		pVertex->Y = buffer[i * 3 + 1];
		// 		pVertex->Z = buffer[i * 3 + 2];

		// 		pVertex->R = RGB_Pal[color * 3];
		// 		pVertex->G = RGB_Pal[color * 3 + 1];
		// 		pVertex->B = RGB_Pal[color * 3 + 2];
		// 		pVertex->A = 128;
		// 		pVertex++;
		// 	}
		// 	break;
		// }
		// case 4: // copper (ramps top to bottom)
		// case 5: // copper2 (ramps top to bottom, 2 scanline per color)
		// {
		// 	if(!prepare_poly(buffer, _state->flatVertices, numPoint))
		// 	return;

		// 	assert(_state->polyMinY >= 0);
		// 	assert(_state->polyMaxY < 200);
		// 	assert(_state->polyMinX >= 0);
		// 	assert(_state->polyMaxX < 320);
		// 	int16 y = _state->polyMinY;
		// 	byte *pDestLine = (uint8 *)g_engine->_screen->getBasePtr(0, y);
		// 	const int16 *pVerticXmin = &_state->tabVerticXmin[y];
		// 	const int16 *pVerticXmax = &_state->tabVerticXmax[y];

		// 	for (; y <= _state->polyMaxY; y++) {
		// 		int16 xMin = *pVerticXmin++;
		// 		const int16 xMax = *pVerticXmax++;
		// 		byte *pDest = pDestLine + xMin;

		// 		for (int x = xMin; x <= xMax; x++) {
		// 			*pDest++ = (byte)color;
		// 		}

		// 		pDestLine += 320;
		// 		// if (xMin <= xMax) {
		// 		// 	g_engine->_screen->addDirtyRect(Common::Rect(Common::Point(xMin, y2), xMax - xMin + 1, 1));
		// 		// 	renderer_updateScreen();
		// 		// 	readKeyboard();
		// 		// 	if (g_engine->shouldQuit())
		// 		// 		return;
		// 		// }
		// 	}
		// 	break;
		// }
		// case 3: // marbre (ramp left to right)
		// {
		// 	polyVertex *pVertex = &rampVertices[numUsedRampVertices];
		// 	numUsedRampVertices += (numPoint - 2) * 3;
		// 	assert(numUsedRampVertices < NUM_MAX_RAMP_VERTICES);

		// 	float colorStep = 15.f / polyWidth;

		// 	int bank = (color & 0xF0) >> 4;
		// 	int startColor = color & 0xF;

		// 	assert(startColor == 0);

		// 	for (int i = 0; i < numPoint; i++) {
		// 		if (i >= 3) {
		// 			memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
		// 			pVertex++;
		// 			memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
		// 			pVertex++;
		// 		}

		// 		pVertex->X = buffer[i * 3 + 0];
		// 		pVertex->Y = buffer[i * 3 + 1];
		// 		pVertex->Z = buffer[i * 3 + 2];

		// 		float colorf = startColor + colorStep * (pVertex->X - polyMinX);

		// 		pVertex->U = colorf / 15.f;
		// 		pVertex->V = bank / 15.f;
		// 		pVertex++;
		// 	}
		// 	break;
		// }
		// case 6: // marbre2 (ramp right to left)
		// {
		// 	polyVertex *pVertex = &rampVertices[numUsedRampVertices];
		// 	numUsedRampVertices += (numPoint - 2) * 3;
		// 	assert(numUsedRampVertices < NUM_MAX_RAMP_VERTICES);

		// 	float colorStep = 15.f / polyWidth;

		// 	int bank = (color & 0xF0) >> 4;
		// 	int startColor = color & 0xF;

		// 	assert(startColor == 0);

		// 	for (int i = 0; i < numPoint; i++) {
		// 		if (i >= 3) {
		// 			memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
		// 			pVertex++;
		// 			memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
		// 			pVertex++;
		// 		}

		// 		pVertex->X = buffer[i * 3 + 0];
		// 		pVertex->Y = buffer[i * 3 + 1];
		// 		pVertex->Z = buffer[i * 3 + 2];

		// 		float colorf = startColor + colorStep * (pVertex->X - polyMinX);

		// 		pVertex->U = 1.f - colorf / 15.f;
		// 		pVertex->V = bank / 15.f;
		// 		pVertex++;
		// 	}
		// 	break;
		// }
	}
}

static void renderer_drawPoint(float X, float Y, float Z, uint8 color, uint8 material, float size) {
}

static void renderer_updateScreen() {
	g_engine->_screen->update();
}

} // namespace Fitd
