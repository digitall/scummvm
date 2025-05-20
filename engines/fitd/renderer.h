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

#ifndef FITD_RENDERER_H
#define FITD_RENDERER_H

#include "common/scummsys.h"

namespace Graphics {
struct Surface;
}

namespace Fitd {

struct Renderer {
	void (*init)();
	void (*deinit)();
	void (*startFrame)();
	void (*drawBackground)();
	void (*setPalette)(const byte *palette);
	void (*copyBlockPhys)(byte *videoBuffer, int left, int top, int right, int bottom);
	void (*renderLine)(int16 x1, int16 y1, int16 z1, int16 x2, int16 y2, int16 z2, uint8 color);
	void (*fillPoly)(const int16 *buffer, int numPoint, unsigned char color, uint8 polyType);
	void (*refreshFrontTextureBuffer)();
	void (*flushPendingPrimitives)();
	void (*createMask)(const uint8 *mask, int roomId, int maskId, unsigned char *refImage, int maskX1, int maskY1, int maskX2, int maskY2);
	void (*setClip)(float left, float top, float right, float bottom);
	void (*clearClip)();
	void (*drawMask)(int roomId, int maskId);
	void (*drawPoint)(float X, float Y, float Z, uint8 color, uint8 material, float size);
	void (*copyBoxLogPhys)(int left, int top, int right, int bottom);
	void (*updateScreen)();
};

} // namespace Fitd

#endif
