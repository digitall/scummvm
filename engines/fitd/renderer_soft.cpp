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

#include "engines/util.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/lines.h"
#include "fitd/renderer.h"
#include "graphics/screen.h"

#define ROL16(x, b) (((x) << (b)) | ((x) >> (16 - (b))))

namespace Fitd {

enum {
	NUM_MAX_FLAT_VERTICES = 5000 * 3,
	MAX_POINTS_PER_POLY = 50,
	NUM_MAX_MASKS = 32,
	WIDTH = 320,
	HEIGHT = 200,
};

struct PolyVertex {
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
	PolyVertex flatVertices[NUM_MAX_FLAT_VERTICES];
	PolyVertex tmpVertices[NUM_MAX_FLAT_VERTICES];
	byte physicalScreen[WIDTH * HEIGHT];
	byte RGB_Pal[256 * 3];

	int16 polyMinX;
	int16 polyMaxX;
	int16 polyMinY;
	int16 polyMaxY;

	ClipMask clipMask;
	Mask masks[NUM_MAX_MASKS];
	uint16 numMasks;
};

static State *_state = nullptr;

static void renderer_init();
static void renderer_destroy();
static void renderer_startFrame();
static void renderer_drawBackground();
static void renderer_setPalette(const byte *palette);
static void renderer_copyBlockPhys(byte *videoBuffer, int left, int top, int right, int bottom);
static void renderer_fillPoly(const int16 *buffer, int numPoint, byte color, uint8 polyType);
static void renderer_renderLine(int16 x1, int16 y1, int16 z1, int16 x2, int16 y2, int16 z2, uint8 color);
static void renderer_refreshFrontTextureBuffer();
static void renderer_flushPendingPrimitives();
static void renderer_createMask(const uint8 *mask, int roomId, int maskId, byte *refImage, int maskX1, int maskY1, int maskX2, int maskY2);
static void renderer_setClip(float left, float top, float right, float bottom);
static void renderer_clearClip();
static void renderer_drawMask(int roomId, int maskId);
static void renderer_drawPoint(float X, float Y, float Z, uint8 color);
static void renderer_drawBigPoint(float X, float Y, float Z, uint8 color);
static void renderer_drawSphere(float X, float Y, float Z, uint8 color, uint8 material, float size);
static void renderer_updateScreen();
static void renderer_copyBoxLogPhys(int left, int top, int right, int bottom);

Renderer createSoftwareRenderer() {
	Renderer r{};
	r.init = renderer_init;
	r.destroy = renderer_destroy;
	r.startFrame = renderer_startFrame;
	r.drawBackground = renderer_drawBackground;
	r.setPalette = renderer_setPalette;
	r.copyBlockPhys = renderer_copyBlockPhys;
	r.fillPoly = renderer_fillPoly;
	r.renderLine = renderer_renderLine;
	r.refreshFrontTextureBuffer = renderer_refreshFrontTextureBuffer;
	r.flushPendingPrimitives = renderer_flushPendingPrimitives;
	r.createMask = renderer_createMask;
	r.setClip = renderer_setClip;
	r.clearClip = renderer_clearClip;
	r.drawMask = renderer_drawMask;
	r.drawPoint = renderer_drawPoint;
	r.drawBigPoint = renderer_drawBigPoint;
	r.drawSphere = renderer_drawSphere;
	r.drawZixel = renderer_drawSphere;
	r.updateScreen = renderer_updateScreen;
	r.copyBoxLogPhys = renderer_copyBoxLogPhys;
	return r;
}

static void renderer_init() {
	_state = static_cast<State *>(malloc(sizeof(State)));
	_state->numMasks = 0;
	renderer_clearClip();
	initGraphics(WIDTH, HEIGHT);
	g_engine->_screen = new Graphics::Screen(WIDTH, HEIGHT);
}

static void renderer_destroy() {
	delete g_engine->_screen;
	free(_state);
	_state = nullptr;
}

static void renderer_startFrame() {
	g_engine->_screen->clear();
	g_engine->_screen->clearDirtyRects();
}

static void renderer_drawBackground() {
	g_engine->_screen->setPalette(_state->RGB_Pal);
	byte *screen = static_cast<byte *>(g_engine->_screen->getBasePtr(0, 0));
	memcpy(screen, _state->physicalScreen, WIDTH * HEIGHT);
	g_engine->_screen->markAllDirty();
}

static void renderer_setPalette(const byte *palette) {
	memcpy(_state->RGB_Pal, palette, 256 * 3);
	g_engine->_screen->setPalette(_state->RGB_Pal);
}

static void renderer_copyBlockPhys(byte *videoBuffer, int left, int top, int right, int bottom) {

	while ((right - left) % 4) {
		right++;
	}

	while ((bottom - top) % 4) {
		bottom++;
	}

	for (int i = top; i < bottom; i++) {
		const byte *in = static_cast<const byte *>(&videoBuffer[0]) + left + i * WIDTH;
		byte *out = _state->physicalScreen + left + i * WIDTH;

		for (int j = left; j < right; j++) {
			const byte color = *in++;
			*out++ = color;
		}
	}
}

static void renderer_refreshFrontTextureBuffer() {
	// TODO: remove ?
}

static void renderer_flushPendingPrimitives() {
	// TODO: remove ?
}

static void renderer_createMask(const uint8 *mask, int roomId, int maskId, byte *refImage, int maskX1, int maskY1, int maskX2, int maskY2) {
	Mask *pMask = nullptr;
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

	assert(pMask);
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

	_state->clipMask.x = CLIP(_state->clipMask.x, static_cast<int16>(0), static_cast<int16>(WIDTH));
	_state->clipMask.y = CLIP(_state->clipMask.y, static_cast<int16>(0), static_cast<int16>(HEIGHT));

	_state->clipMask.w = CLIP(_state->clipMask.w, static_cast<int16>(0), static_cast<int16>(WIDTH - _state->clipMask.x));
	_state->clipMask.h = CLIP(_state->clipMask.h, static_cast<int16>(0), static_cast<int16>(HEIGHT - _state->clipMask.y));
}

static void renderer_clearClip() {
	_state->clipMask.x = 0;
	_state->clipMask.y = 0;
	_state->clipMask.w = WIDTH;
	_state->clipMask.h = HEIGHT;
}

static void renderer_drawMask(int roomId, int maskId) {
	if (maskId == 666) {
		for (uint16 i = 0; i < _state->numMasks; i++) {
			const Mask *pMask = &_state->masks[i];
			if (pMask->roomId == roomId && pMask->maskId == maskId) {
				byte *s = static_cast<byte *>(g_engine->_screen->getBasePtr(_state->clipMask.x, _state->clipMask.y));
				const byte *m = &pMask->mask[_state->clipMask.y * WIDTH + _state->clipMask.x];
				for (int16 h = 0; h < _state->clipMask.h; h++) {
					for (int16 w = 0; w < _state->clipMask.w; w++) {
						if (!*m) {
							*s = 0;
						}
						m++;
						s++;
					}
					m += WIDTH - _state->clipMask.w;
					s += WIDTH - _state->clipMask.w;
				}
				return;
			}
		}
	}

	for (uint16 i = 0; i < _state->numMasks; i++) {
		const Mask *pMask = &_state->masks[i];
		if (pMask->roomId == roomId && pMask->maskId == maskId) {
			byte *s = static_cast<byte *>(g_engine->_screen->getBasePtr(_state->clipMask.x, _state->clipMask.y));
			const byte *p = &_state->physicalScreen[_state->clipMask.y * WIDTH + _state->clipMask.x];
			const byte *m = &pMask->mask[_state->clipMask.y * WIDTH + _state->clipMask.x];
			for (int16 h = 0; h < _state->clipMask.h; h++) {
				for (int16 w = 0; w < _state->clipMask.w; w++) {
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

static int16 leftClip(PolyVertex **polys, int16 num) {
	PolyVertex *pVertex = polys[0];
	PolyVertex *pTabPolyClip = polys[1];
	int16 newNbPoints = 0;

	// invert the pointers to continue on the clipped vertices in the next method
	polys[0] = pTabPolyClip;
	polys[1] = pVertex;

	for (int i = 0; i < num; i++, pVertex++) {
		const PolyVertex *p0 = pVertex;
		const PolyVertex *p1 = p0 + 1;

		// clipFlag :
		// 0x00 : none clipped
		// 0x01 : point 0 clipped
		// 0x02 : point 1 clipped
		// 0x03 : both clipped
		uint8 clipFlag = p1->X < 0 ? 2 : 0;

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

			pTabPolyClip->X = static_cast<int16>(0);
			pTabPolyClip->Y = static_cast<int16>(p0->Y + dxClip * dy / dx);
			pTabPolyClip->Z = p0->Z + static_cast<float>(dxClip * dz) / dx;

			++pTabPolyClip;
			++newNbPoints;
		}
	}

	// copy first vertex to the end
	*pTabPolyClip = polys[0][0];
	return newNbPoints;
}

static int16 rightClip(PolyVertex **polys, int16 num) {
	PolyVertex *pVertex = polys[0];
	PolyVertex *pTabPolyClip = polys[1];
	int16 newNbPoints = 0;

	// invert the pointers to continue on the clipped vertices in the next method
	polys[0] = pTabPolyClip;
	polys[1] = pVertex;

	for (int i = 0; i < num; i++, pVertex++) {
		const PolyVertex *p0 = pVertex;
		const PolyVertex *p1 = p0 + 1;

		// clipFlag :
		// 0x00 : none clipped
		// 0x01 : point 0 clipped
		// 0x02 : point 1 clipped
		// 0x03 : both clipped
		uint8 clipFlag = p1->X >= WIDTH ? 2 : 0;

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
			const int32 dxClip = WIDTH - 1 - p0->X;

			pTabPolyClip->X = static_cast<int16>(WIDTH - 1);
			pTabPolyClip->Y = static_cast<int16>(p0->Y + dxClip * dy / dx);
			pTabPolyClip->Z = p0->Z + static_cast<float>(dxClip * dz) / dx;

			++pTabPolyClip;
			++newNbPoints;
		}
	}

	// copy first vertex to the end
	*pTabPolyClip = polys[0][0];
	return newNbPoints;
}

static int16 topClip(PolyVertex **polys, int16 num) {
	PolyVertex *pVertex = polys[0];
	PolyVertex *pTabPolyClip = polys[1];
	int16 newNbPoints = 0;

	// invert the pointers to continue on the clipped vertices in the next method
	polys[0] = pTabPolyClip;
	polys[1] = pVertex;

	for (int i = 0; i < num; i++, pVertex++) {
		const PolyVertex *p0 = pVertex;
		const PolyVertex *p1 = p0 + 1;

		// clipFlag :
		// 0x00 : none clipped
		// 0x01 : point 0 clipped
		// 0x02 : point 1 clipped
		// 0x03 : both clipped
		uint8 clipFlag = p1->Y < 0 ? 2 : 0;

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

			pTabPolyClip->X = static_cast<int16>(p0->X + dyClip * dx / dy);
			pTabPolyClip->Y = static_cast<int16>(0);
			pTabPolyClip->Z = p0->Z + static_cast<float>(dyClip * dz) / dy;

			++pTabPolyClip;
			++newNbPoints;
		}
	}

	// copy first vertex to the end
	*pTabPolyClip = polys[0][0];
	return newNbPoints;
}

static int16 bottomClip(PolyVertex **polys, int16 num) {
	PolyVertex *pVertex = polys[0];
	PolyVertex *pTabPolyClip = polys[1];
	int16 newNbPoints = 0;

	// invert the pointers to continue on the clipped vertices in the next method
	polys[0] = pTabPolyClip;
	polys[1] = pVertex;

	for (int i = 0; i < num; i++, pVertex++) {
		const PolyVertex *p0 = pVertex;
		const PolyVertex *p1 = p0 + 1;

		// clipFlag :
		// 0x00 : none clipped
		// 0x01 : point 0 clipped
		// 0x02 : point 1 clipped
		// 0x03 : both clipped
		uint8 clipFlag = p1->Y >= HEIGHT ? 2 : 0;

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
			const int32 dyClip = HEIGHT - 1 - p0->Y;

			pTabPolyClip->X = static_cast<int16>(p0->X + dyClip * dx / dy);
			pTabPolyClip->Y = static_cast<int16>(HEIGHT - 1);
			pTabPolyClip->Z = p0->Z + static_cast<float>(dyClip * dz) / dy;

			++pTabPolyClip;
			++newNbPoints;
		}
	}

	// copy first vertex to the end
	*pTabPolyClip = polys[0][0];
	return newNbPoints;
}

static int16 poly_clip(PolyVertex **polys, int16 num) {
	bool hasBeenClipped = false;
	int32 clippedNum = num;
	if (_state->polyMinX < 0) {
		clippedNum = leftClip(polys, clippedNum);
		if (!clippedNum) {
			return 0;
		}
		hasBeenClipped = true;
	}
	if (_state->polyMaxX >= WIDTH) {
		clippedNum = rightClip(polys, clippedNum);
		if (!clippedNum) {
			return 0;
		}
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

static void poly_setMinMax(PolyVertex *pPolys, int16 num) {
	PolyVertex *pTabPoly = pPolys;
	int32 incY = -1;
	for (int i = 0; i < num; i++, pTabPoly++) {
		const PolyVertex *p0 = pTabPoly;
		const PolyVertex *p1 = p0 + 1;
		int16 *pVertic = nullptr;

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
		}

		int32 dx = (p1->X - p0->X) << 16;
		int32 step = dx / dy;
		int32 reminder = ((dx % dy) >> 1) + 0x7FFF;

		dx = step >> 16; // recovery part high division (entire)
		step &= 0xFFFF;  // preserves lower part (mantissa)
		int32 x = p0->X;

		for (int32 y = 0; y <= dy; y++) {
			*pVertic = static_cast<int16>(x);
			assert(x >= 0);
			assert(x < WIDTH);
			pVertic += incY;
			x += dx;
			reminder += step;
			if (reminder & 0xFFFF0000) {
				x += reminder >> 16;
				reminder &= 0xFFFF;
			}
		}
	}

	// fix issue when vertices have the same Y
	if (_state->polyMinY == _state->polyMaxY) {
		pTabPoly = pPolys;
		_state->tabVerticXmin[_state->polyMinY] = INT16_MAX;
		_state->tabVerticXmax[_state->polyMinY] = INT16_MIN;
		for (int i = 0; i < num; i++, pTabPoly++) {
			const PolyVertex *p0 = pTabPoly;
			if (p0->X < _state->tabVerticXmin[_state->polyMinY])
				_state->tabVerticXmin[_state->polyMinY] = p0->X;
			if (p0->X > _state->tabVerticXmax[_state->polyMinY])
				_state->tabVerticXmax[_state->polyMinY] = p0->X;
		}
		assert(_state->tabVerticXmin[_state->polyMinY] >= 0);
		assert(_state->tabVerticXmin[_state->polyMaxY] < WIDTH);
		assert(_state->tabVerticXmax[_state->polyMinY] >= 0);
		assert(_state->tabVerticXmax[_state->polyMaxY] < WIDTH);
	}
}

static void set_boundingBox(const PolyVertex *pTabPoly, int16 num) {
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

static int16 prepare_poly(const int16 *buffer, PolyVertex *pTabPoly, int16 num) {
	PolyVertex *pVertex = pTabPoly;

	for (int i = 0; i < num; i++) {
		pVertex->X = buffer[i * 3 + 0];
		pVertex->Y = buffer[i * 3 + 1];
		pVertex->Z = buffer[i * 3 + 2];
		pVertex++;
	}

	set_boundingBox(pTabPoly, num);

	// no vertices
	if (_state->polyMinY > _state->polyMaxY || _state->polyMaxX < 0 || _state->polyMinX >= WIDTH || _state->polyMaxY < 0 || _state->polyMinY >= HEIGHT) {
		return 0;
	}

	// copy the first point to the end of the polygon
	pTabPoly[num] = pTabPoly[0];
	PolyVertex *polys[] = {pTabPoly, _state->tmpVertices};

	num = poly_clip(polys, num);
	if (!num)
		return 0;

	poly_setMinMax(polys[0], num);
	return num;
}

struct FlatRenderState {
	byte color;
} _flatState;

struct CopyPolyRenderState {
	byte *ptr;
} _copyPolyState;

static void copypoly_init(byte c) {
}

static void copypoly_render(byte *dst) {
	*dst = 0xFF;
}

static void none_nextLine(int16 xMin, int16 xMax) {
}

static void flat_init(byte c) {
	_flatState.color = c;
}

static void flat_render(byte *dst) {
	*dst = _flatState.color;
}

static void trans_init(byte c) {
	_flatState.color = c;
}

static void trans_render(byte *dst) {
	*dst = *dst | _flatState.color;
}

struct DitherRenderState {
	int16 acc;
	byte initColor;
	uint16 color;
} _ditherState;

void dither_init(byte c) {
	_ditherState.acc = 17371;
	_ditherState.initColor = c;
	_ditherState.color = c;
}

void dither_render(byte *dst) {
	_ditherState.color = ((_ditherState.color + _ditherState.acc) & 0xFF03) + _ditherState.initColor;
	_ditherState.acc = ROL16(_ditherState.acc, 2) + 1;

	*dst = _ditherState.color;
}

struct MarbleRenderState {
	byte start;
	byte bank;
	float color;
	float step;
} _marbleState;

void marble_init(byte c) {
	_marbleState.start = c & 0x0F;
	_marbleState.bank = c & 0xF0;
}

void marble_nextLine(int16 xMin, int16 xMax) {
	const int dx = xMax - xMin;
	_marbleState.step = 15.f / static_cast<float>(dx + 1);
	_marbleState.color = _marbleState.start;
}

void marble_render(byte *dst) {
	*dst = _marbleState.bank | ((static_cast<byte>(_marbleState.color)) & 0x0F);
	_marbleState.color += _marbleState.step;
}

void marble2_render(byte *dst) {
	*dst = _marbleState.bank | (15 - ((static_cast<byte>(_marbleState.color)) & 0x0F));
	_marbleState.color += _marbleState.step;
}

struct CopperRenderState {
	int8 sens;
	byte color;
	byte bank;
} _copperState;

void copper_init(byte c) {
	_copperState.sens = 1;
	_copperState.bank = c & 0xF0;
	_copperState.color = (c - 1) & 0x0F;
}

void copper_nextLine(int16 xMin, int16 xMax) {
	_copperState.color += _copperState.sens;
	if (_copperState.color == 16) {
		_copperState.color = 15;
		_copperState.sens = -1;
	} else if (_copperState.color == 0) {
		_copperState.sens = 1;
	}
}

void copper_render(byte *dst) {
	*dst = _copperState.bank | _copperState.color;
}

struct Copper2RenderState {
	int8 sens;
	byte color;
	int32 line;
} _copper2State;

void copper2_init(byte c) {
	_copper2State.sens = 1;
	_copper2State.color = c;
	_copper2State.line = 2;
}

void copper2_nextLine(int16 xMin, int16 xMax) {
	_copper2State.line--;
	if (!_copper2State.line) {
		_copper2State.line = 2;
		_copper2State.color += _copper2State.sens;
		if (!(_copper2State.color & 0xF)) {
			_copper2State.sens = -_copper2State.sens;
			if (_copper2State.sens < 0) {
				_copper2State.color += _copper2State.sens;
			}
		}
	}
}

void copper2_render(byte *dst) {
	*dst = _copper2State.color;
}

struct MaterialRender {
	void (*init)(byte color);
	void (*render)(byte *dst);
	void (*nextLine)(int16 xMin, int16 xMax);
};

static void render(byte color, uint8 material) {
	assert(_state->polyMinY >= 0);
	assert(_state->polyMaxY < HEIGHT);
	int16 y = _state->polyMinY;
	byte *pDestLine = static_cast<uint8 *>(material == 7 ? g_engine->_engine->frontBuffer + y * 320 : g_engine->_screen->getBasePtr(0, y));
	const int16 *pVerticXmin = &_state->tabVerticXmin[y];
	const int16 *pVerticXmax = &_state->tabVerticXmax[y];
	MaterialRender matRender;

	if (!g_engine->_engine->detailToggle) {
		// if low details -> flat
		material = 0;
	}

	switch (material) {
	case 0: {
		// flat (triste)
		matRender.init = flat_init;
		matRender.render = flat_render;
		matRender.nextLine = none_nextLine;
		break;
	}
	case 1: {
		// dither (pierre/tele)
		matRender.init = dither_init;
		matRender.render = dither_render;
		matRender.nextLine = none_nextLine;
		break;
	}
	case 2: {
		// trans
		matRender.init = trans_init;
		matRender.render = trans_render;
		matRender.nextLine = none_nextLine;
		break;
	}
	case 3: {
		// marbre (ramp left to right)
		matRender.init = marble_init;
		matRender.render = marble_render;
		matRender.nextLine = marble_nextLine;
		break;
	}
	case 4: {
		// copper (ramps top to bottom)
		matRender.init = copper_init;
		matRender.render = copper_render;
		matRender.nextLine = copper_nextLine;
		break;
	}
	case 5: {
		// copper2 (ramps top to bottom, 2 scanline per color)
		matRender.init = copper2_init;
		matRender.render = copper2_render;
		matRender.nextLine = copper2_nextLine;
		break;
	}
	case 6: {
		// marbre (ramp right to left)
		matRender.init = marble_init;
		matRender.render = marble2_render;
		matRender.nextLine = marble_nextLine;
		break;
	}
	case 7: {
		// copypoly
		matRender.init = copypoly_init;
		matRender.render = copypoly_render;
		matRender.nextLine = none_nextLine;
		break;
	}
	default: {
		// flat (triste)
		matRender.init = flat_init;
		matRender.render = flat_render;
		matRender.nextLine = none_nextLine;
		break;
	}
	}

	matRender.init(color);
	for (; y <= _state->polyMaxY; y++) {
		const int16 xMin = *pVerticXmin++;
		const int16 xMax = *pVerticXmax++;

		if (xMin < 0 || xMin >= WIDTH || xMax < 0 || xMax >= WIDTH) {
			pDestLine += WIDTH;
			continue;
		}
		byte *pDest = pDestLine + xMin;
		if (xMin <= xMax) {
			matRender.nextLine(xMin, xMax);
		}
		for (int16 x = xMin; x <= xMax; x++) {
			matRender.render(pDest);
			pDest++;
		}

		pDestLine += WIDTH;
		//  if (Debug && xMin <= xMax) {
		//  	g_engine->_screen->addDirtyRect(Common::Rect(Common::Point(xMin, y), xMax - xMin + 1, 1));
		//  	renderer_updateScreen();
		//  	readKeyboard();
		//  	if (g_engine->shouldQuit())
		//  		return;
		//  }
	}
}

static void renderer_fillPoly(const int16 *buffer, int numPoint, byte color, uint8 polyType) {
	assert(numPoint < MAX_POINTS_PER_POLY);

	if (g_engine->shouldQuit())
		return;

	if (!prepare_poly(buffer, _state->flatVertices, numPoint))
		return;

	if (_state->polyMinY == _state->polyMaxY && _state->polyMinX == _state->polyMaxX)
		return;

	render(color, polyType);
}

static void renderer_renderLine(int16 x1, int16 y1, int16 z1, int16 x2, int16 y2, int16 z2, uint8 color) {
	byte *pDestLine = static_cast<uint8 *>(g_engine->_screen->getBasePtr(0, 0));
	g_engine->_engine->polyBackBuffer = pDestLine;
	line(x1, y1, x2, y2, color);
}

static bool inCircle(int pX, int pY, int cX, int cY, float radius) {
	const int dx = ABS(pX - cX);
	const int dy = ABS(pY - cY);
	return dx * dx + dy * dy <= radius * radius;
}

static bool computeSphere(float sx, float sy, float radius) {
	if (radius <= 0) {
		return false;
	}
	int16 left = static_cast<int16>(sx - radius);
	int16 right = static_cast<int16>(sx + radius);
	int16 bottom = static_cast<int16>(sy + radius);
	int16 top = static_cast<int16>(sy - radius);
	const Common::Rect &clip = Common::Rect(0, 0, WIDTH - 1, HEIGHT - 1);
	const int16 cleft = clip.left;
	const int16 cright = clip.right;
	const int16 ctop = clip.top;
	const int16 cbottom = clip.bottom;

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
		for (int16 x = left; x <= right; x++) {
			if (inCircle(x, y, sx, sy, radius)) {
				_state->tabVerticXmin[y] = MIN(x, _state->tabVerticXmin[y]);
				_state->tabVerticXmax[y] = MAX(x, _state->tabVerticXmax[y]);
			}
		}
	}

	_state->polyMinY = top;
	_state->polyMaxY = bottom;

	return true;
}

static void drawPoint(int16 x, int16 y, uint8 color) {
	if (x >= g_engine->_engine->clipLeft && x < g_engine->_engine->clipRight && y >= g_engine->_engine->clipTop && y < g_engine->_engine->clipBottom) {
		g_engine->_screen->setPixel(x, y, color);
		g_engine->_screen->addDirtyRect(Common::Rect(Common::Point(x, y), 1, 1));
	}
}

static void renderer_drawBigPoint(float sX, float sY, float sZ, uint8 color) {
	drawPoint(sX, sY, color);
	drawPoint(sX + 1, sY, color);
	drawPoint(sX + 1, sY + 1, color);
	drawPoint(sX, sY + 1, color);
}

static void renderer_drawPoint(float sX, float sY, float sZ, uint8 color) {
	drawPoint(sX, sY, color);
}

static void renderer_drawSphere(float sX, float sY, float sZ, uint8 color, uint8 material, float size) {
	if (computeSphere(sX, sY, size)) {
		render(color, material);
	}
}

static void renderer_updateScreen() {
	g_engine->_screen->update();
}

static void renderer_copyBoxLogPhys(int left, int top, int right, int bottom) {
	byte *dst = static_cast<byte *>(g_engine->_screen->getBasePtr(0, 0));

	while ((right - left) % 4) {
		right++;
	}

	while ((bottom - top) % 4) {
		bottom++;
	}

	for (int i = top; i < bottom; i++) {
		const byte *in = static_cast<const byte *>(&g_engine->_engine->frontBuffer[0]) + left + i * 320;
		byte *out2 = dst + left + i * 320;

		for (int j = left; j < right; j++) {
			const byte color = *in++;

			*out2++ = color;
		}
	}
	g_engine->_screen->addDirtyRect(Common::Rect(left, top, right, bottom));
	g_engine->_screen->update();
}

} // namespace Fitd
