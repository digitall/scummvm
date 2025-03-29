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

#ifndef FITD_GFX_H
#define FITD_GFX_H

#include "common/scummsys.h"

namespace Fitd {

#define INFO_TRI 1
#define INFO_ANIM 2
#define INFO_TORTUE 4
#define INFO_OPTIMISE 8

extern byte currentGamePalette[256*3];
extern byte frontBuffer[320 * 200];

void gfx_init();
void gfx_draw();
void gfx_setPalette(const byte *palette);
void gfx_copyBlockPhys(byte *videoBuffer, int left, int top, int right, int bottom);
void gfx_refreshFrontTextureBuffer();

void setPosCamera(int x, int y, int z);
void setAngleCamera(int x, int y, int z);
void setupCameraProjection(int centerX, int centerY, int x, int y, int z);
void setCameraTarget(int x, int y, int z, int alpha, int beta, int gamma, int time);
int affObjet(int x, int y, int z, int alpha, int beta, int gamma, void *modelPtr);
void setClip(int left, int top, int right, int bottom);
void affSpfI(int x, int y, int param, char* gfx);
void fillBox(int x1, int y1, int x2, int y2, char color); // fast recode. No RE
void flushScreen(void);
void setupCamera();

void osystem_drawBackground();
void osystem_fillPoly(float *buffer, int numPoint, unsigned char color, byte polyType);
void osystem_flushPendingPrimitives();
void osystem_createMask(const uint8 *mask, int roomId, int maskId, unsigned char *refImage, int maskX1, int maskY1, int maskX2, int maskY2);
void osystem_drawMask(int roomId, int maskId);
void osystem_setClip(float left, float top, float right, float bottom);
void osystem_clearClip();
void osystem_stopModelRender();

extern int BBox3D1;
extern int BBox3D2;
extern int BBox3D3;
extern int BBox3D4;

#define NUM_MAX_POINT_IN_POINT_BUFFER 800
#define NUM_MAX_BONES 50

extern int16 pointBuffer[NUM_MAX_POINT_IN_POINT_BUFFER*3];
}; // namespace Fitd

#endif
