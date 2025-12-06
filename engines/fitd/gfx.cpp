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

#include "fitd/gfx.h"
#include "common/config-manager.h"
#include "common/debug.h"
#include "common/rendermode.h"
#include "common/scummsys.h"
#include "common/system.h"
#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/costable.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/hqr.h"
#include "fitd/renderer.h"
#include "fitd/renderer_opengl.h"
#include "fitd/renderer_soft.h"
#include "fitd/vars.h"

namespace Fitd {

#define NUM_MAX_VERTEX_IN_PRIM 64
#define NUM_MAX_PRIM_ENTRY 500
#define NUM_MAX_BONES 50

static int numOfBones;

static int16 cameraSpaceBuffer[NUM_MAX_POINT_IN_POINT_BUFFER * 3];

static bool boneRotateX;
static bool boneRotateY;
static bool boneRotateZ;

static int boneRotateXCos;
static int boneRotateXSin;
static int boneRotateYCos;
static int boneRotateYSin;
static int boneRotateZCos;
static int boneRotateZSin;

typedef struct RendererPoint {
	int16 X;
	int16 Y;
	int16 Z;
} RendererPoint;

typedef struct PrimEntry {
	uint8 material;
	uint8 color;
	uint16 size;
	uint16 numOfVertices;
	PrimType type;
	RendererPoint vertices[NUM_MAX_VERTEX_IN_PRIM];
	float depth;
} PrimEntry;

static PrimEntry primTable[NUM_MAX_PRIM_ENTRY];

static bool noModelRotation;

static int modelCosAlpha;
static int modelSinAlpha;
static int modelCosBeta;
static int modelSinBeta;
static int modelCosGamma;
static int modelSinGamma;

static uint32 positionInPrimEntry = 0;

static int numOfPrimitiveToRender = 0;

static int modelFlags = 0;

static char primBuffer[30000];

static int renderX;
static int renderY;
static int renderZ;
static Renderer renderer;

void gfx_init() {
	const Common::RenderMode configRenderMode = Common::parseRenderMode(ConfMan.get("render_mode").c_str());
#if defined(USE_OPENGL_SHADERS)
	if (configRenderMode == Common::kRenderVGA || !g_system->hasFeature(OSystem::kFeatureShadersForGame)) {
		renderer = createSoftwareRenderer();
	} else {
		renderer = createOpenGLRenderer();
	}
#else
	renderer = createSoftwareRenderer();
#endif
	renderer.init();
}

void gfx_deinit() {
	renderer.destroy();
}

void startFrame() {
	renderer.startFrame();
	drawBackground();
}

void drawBackground() {
	renderer.drawBackground();
}

void gfx_setPalette(const byte *palette) {
	renderer.setPalette(palette);
}

void gfx_copyBlockPhys(byte *videoBuffer, int left, int top, int right, int bottom) {
	renderer.copyBlockPhys(videoBuffer, left, top, right, bottom);
}

void gfx_refreshFrontTextureBuffer() {
	renderer.refreshFrontTextureBuffer();
}

static void osystem_fillPoly(int16 *buffer, int numPoint, byte color, uint8 polyType) {
	renderer.fillPoly(buffer, numPoint, color, polyType);
}

void flushPendingPrimitives() {
	renderer.flushPendingPrimitives();
}

void setPosCamera(int x, int y, int z) {
	g_engine->_engine->translateX = x;
	g_engine->_engine->translateY = y;
	g_engine->_engine->translateZ = z;
}

void setAngleCamera(int x, int y, int z) {
	g_engine->_engine->transformX = x & 0x3FF;
	if (g_engine->_engine->transformX) {
		g_engine->_engine->transformXCos = cosTable[g_engine->_engine->transformX];
		g_engine->_engine->transformXSin = cosTable[(g_engine->_engine->transformX + 0x100) & 0x3FF];
		g_engine->_engine->transformUseX = true;
	} else {
		g_engine->_engine->transformUseX = false;
	}

	g_engine->_engine->transformY = y & 0x3FF;
	if (g_engine->_engine->transformY) {
		g_engine->_engine->transformYCos = cosTable[g_engine->_engine->transformY];
		g_engine->_engine->transformYSin = cosTable[(g_engine->_engine->transformY + 0x100) & 0x3FF];
		g_engine->_engine->transformUseY = true;
	} else {
		g_engine->_engine->transformUseY = false;
	}

	g_engine->_engine->transformZ = z & 0x3FF;
	if (g_engine->_engine->transformZ) {
		g_engine->_engine->transformZCos = cosTable[g_engine->_engine->transformZ];
		g_engine->_engine->transformZSin = cosTable[(g_engine->_engine->transformZ + 0x100) & 0x3FF];
		g_engine->_engine->transformUseZ = true;
	} else {
		g_engine->_engine->transformUseZ = false;
	}
}

void rotate(uint x, uint y, uint z, int *xOut, int *yOut) {
	if (x) {
		const int var1 = (((cosTable[(x + 0x100) & 0x3FF] * y) << 1) & 0xFFFF0000) - (((cosTable[x & 0x3FF] * z) << 1) & 0xFFFF0000);
		const int var2 = (((cosTable[x & 0x3FF] * y) << 1) & 0xFFFF0000) + (((cosTable[(x + 0x100) & 0x3FF] * z) << 1) & 0xFFFF0000);

		*yOut = var1 >> 16;
		*xOut = var2 >> 16;
	} else {
		*xOut = z;
		*yOut = y;
	}
}

void setCameraTarget(int x, int y, int z, int alpha, int beta, int gamma, int time) {
	int x1;
	int y1;
	int x2;
	int y2;

	rotate(alpha + 0x200, -time, 0, &x1, &y1);
	rotate(beta + 0x200, y1, 0, &x2, &y2);

	setPosCamera(x2 + x, -x1 + y, y2 + z);
	setAngleCamera(alpha, beta, gamma);
}

static void transformPoint(int16 *ax, int16 *bx, int16 *cx) {
	int X = *ax;
	int Y = *bx;
	int Z = *cx;
	{
		int *iax = &X;
		int *ibx = &Y;

		{
			int *icx = &Z;
			int x;
			int y;
			int z;

			if (g_engine->_engine->transformUseY) {
				x = ((*iax * g_engine->_engine->transformYSin - *icx * g_engine->_engine->transformYCos) / 0x10000) << 1;
				z = ((*iax * g_engine->_engine->transformYCos + *icx * g_engine->_engine->transformYSin) / 0x10000) << 1;
			} else {
				x = *iax;
				z = *icx;
			}

			// si = x
			// ax = z

			if (g_engine->_engine->transformUseX) {
				const int tempY = *ibx;
				const int tempZ = z;
				y = ((tempY * g_engine->_engine->transformXSin - tempZ * g_engine->_engine->transformXCos) / 0x10000) << 1;
				z = ((tempY * g_engine->_engine->transformXCos + tempZ * g_engine->_engine->transformXSin) / 0x10000) << 1;
			} else {
				y = *ibx;
			}

			// cx = y
			// bx = z

			if (g_engine->_engine->transformUseZ) {
				const int tempX = x;
				const int tempY = y;
				x = ((tempX * g_engine->_engine->transformZSin - tempY * g_engine->_engine->transformZCos) / 0x10000) << 1;
				y = ((tempX * g_engine->_engine->transformZCos + tempY * g_engine->_engine->transformZSin) / 0x10000) << 1;
			}

			*iax = x;
			*ibx = y;
			*icx = z;
		}
	}

	*ax = static_cast<int16>(X);
	*bx = static_cast<int16>(Y);
	*cx = static_cast<int16>(Z);
}

static void translateGroupe(int transX, int transY, int transZ, const Group *ptr) {
	int16 *ptrSource = &g_engine->_engine->pointBuffer[ptr->m_start * 3];

	for (int i = 0; i < ptr->m_numVertices; i++) {
		*ptrSource++ += transX;
		*ptrSource++ += transY;
		*ptrSource++ += transZ;
	}
}

static void zoomGroupe(int zoomX, int zoomY, int zoomZ, const Group *ptr) {
	int16 *ptrSource = &g_engine->_engine->pointBuffer[ptr->m_start * 3];

	for (int i = 0; i < ptr->m_numVertices; i++) {
		*ptrSource = *ptrSource * (zoomX + 256) / 256;
		ptrSource++;
		*ptrSource = *ptrSource * (zoomY + 256) / 256;
		ptrSource++;
		*ptrSource = *ptrSource * (zoomZ + 256) / 256;
		ptrSource++;
	}
}

static void initGroupeRot(int transX, int transY, int transZ) {
	if (transX) {
		boneRotateXCos = cosTable[transX & 0x3FF];
		boneRotateXSin = cosTable[(transX + 0x100) & 0x3FF];

		boneRotateX = true;
	} else {
		boneRotateX = false;
	}

	if (transY) {
		boneRotateYCos = cosTable[transY & 0x3FF];
		boneRotateYSin = cosTable[(transY + 0x100) & 0x3FF];

		boneRotateY = true;
	} else {
		boneRotateY = false;
	}

	if (transZ) {
		boneRotateZCos = cosTable[transZ & 0x3FF];
		boneRotateZSin = cosTable[(transZ + 0x100) & 0x3FF];

		boneRotateZ = true;
	} else {
		boneRotateZ = false;
	}
}

static void rotateList(int16 *pointPtr, int numOfPoint) {
	for (int i = 0; i < numOfPoint; i++) {
		int x = *pointPtr;
		int y = *(pointPtr + 1);
		int z = *(pointPtr + 2);

		if (boneRotateY) {
			const int tempX = x;
			const int tempZ = z;

			x = ((tempX * boneRotateYSin - tempZ * boneRotateYCos) >> 16) << 1;
			z = ((tempX * boneRotateYCos + tempZ * boneRotateYSin) >> 16) << 1;
		}

		if (boneRotateX) {
			const int tempY = y;
			const int tempZ = z;
			y = ((tempY * boneRotateXSin - tempZ * boneRotateXCos) >> 16) << 1;
			z = ((tempY * boneRotateXCos + tempZ * boneRotateXSin) >> 16) << 1;
		}

		if (boneRotateZ) {
			const int tempX = x;
			const int tempY = y;
			x = ((tempX * boneRotateZSin - tempY * boneRotateZCos) >> 16) << 1;
			y = ((tempX * boneRotateZCos + tempY * boneRotateZSin) >> 16) << 1;
		}

		*pointPtr = x;
		*(pointPtr + 1) = y;
		*(pointPtr + 2) = z;

		pointPtr += 3;
	}
}

static void rotateGroupeOptimise(const Group *ptr) {
	if (ptr->m_numGroup) // if group number is 0
	{
		const int baseBone = ptr->m_start;
		const int numPoints = ptr->m_numVertices;

		rotateList(g_engine->_engine->pointBuffer + baseBone * 3, numPoints);
	}
}

static void rotateGroupe(Group *ptr) {
	const int baseBone = ptr->m_start;
	const int numPoints = ptr->m_numVertices;

	rotateList(g_engine->_engine->pointBuffer + baseBone * 3, numPoints);

	const int temp = ptr->m_numGroup; // group number

	int temp2 = numOfBones - temp;

	do {
		if (ptr->m_orgGroup == temp) // is it on of this group child
		{
			rotateGroupe(ptr); // yes, so apply the transformation to him
		}

		ptr++;
	} while (--temp2);
}

static int animNuage(int x, int y, int z, int alpha, int beta, int gamma, Body *pBody) {
	renderX = x - g_engine->_engine->translateX;
	renderY = y;
	renderZ = z - g_engine->_engine->translateZ;

	assert(pBody->m_vertices.size() < NUM_MAX_POINT_IN_POINT_BUFFER);

	for (uint i = 0; i < pBody->m_vertices.size(); i++) {
		g_engine->_engine->pointBuffer[i * 3 + 0] = pBody->m_vertices[i].x;
		g_engine->_engine->pointBuffer[i * 3 + 1] = pBody->m_vertices[i].y;
		g_engine->_engine->pointBuffer[i * 3 + 2] = pBody->m_vertices[i].z;
	}

	g_engine->_engine->numOfPoints = pBody->m_vertices.size();
	numOfBones = pBody->m_groupOrder.size();
	assert(numOfBones < NUM_MAX_BONES);

	if (pBody->m_flags & INFO_OPTIMISE) {
		for (uint i = 0; i < pBody->m_groupOrder.size(); i++) {
			const Group *pGroup = &pBody->m_groups[pBody->m_groupOrder[i]];

			switch (pGroup->m_state.m_type) {
			case 1:
				if (pGroup->m_state.m_delta[0] || pGroup->m_state.m_delta[1] || pGroup->m_state.m_delta[2]) {
					translateGroupe(pGroup->m_state.m_delta[0], pGroup->m_state.m_delta[1], pGroup->m_state.m_delta[2], pGroup);
				}
				break;
			case 2:
				if (pGroup->m_state.m_delta[0] || pGroup->m_state.m_delta[1] || pGroup->m_state.m_delta[2]) {
					zoomGroupe(pGroup->m_state.m_delta[0], pGroup->m_state.m_delta[1], pGroup->m_state.m_delta[2], pGroup);
				}
				break;
			}

			initGroupeRot(pGroup[0].m_state.m_rotateDelta[0], pGroup[0].m_state.m_rotateDelta[1], pGroup[0].m_state.m_rotateDelta[2]);
			rotateGroupeOptimise(pGroup);
		}
	} else {
		pBody->m_groups[0].m_state.m_delta[0] = alpha;
		pBody->m_groups[0].m_state.m_delta[1] = beta;
		pBody->m_groups[0].m_state.m_delta[2] = gamma;

		for (uint i = 0; i < pBody->m_groups.size(); i++) {
			Group *pGroup = &pBody->m_groups[pBody->m_groupOrder[i]];

			const int transX = pGroup->m_state.m_delta[0];
			const int transY = pGroup->m_state.m_delta[1];
			const int transZ = pGroup->m_state.m_delta[2];

			if (transX || transY || transZ) {
				switch (pGroup->m_state.m_type) {
				case 0: {
					initGroupeRot(transX, transY, transZ);
					rotateGroupe(pGroup);
					break;
				}
				case 1: {
					translateGroupe(transX, transY, transZ, pGroup);
					break;
				}
				case 2: {
					zoomGroupe(transX, transY, transZ, pGroup);
					break;
				}
				}
			}
		}
	}

	for (uint i = 0; i < pBody->m_groups.size(); i++) {
		const Group *pGroup = &pBody->m_groups[i];

		int point1 = pGroup->m_baseVertices * 6;
		int point2 = pGroup->m_start * 6;

		assert(point1 % 2 == 0);
		assert(point2 % 2 == 0);

		point1 /= 2;
		point2 /= 2;

		assert(point1 / 3 < NUM_MAX_POINT_IN_POINT_BUFFER);
		assert(point2 / 3 < NUM_MAX_POINT_IN_POINT_BUFFER);

		const int16 *ptr1 = &g_engine->_engine->pointBuffer[point1];
		int16 *ptr2 = &g_engine->_engine->pointBuffer[point2];

		const int number = pGroup->m_numVertices;

		const int ax = ptr1[0];
		const int bx = ptr1[1];
		const int dx = ptr1[2];

		for (int j = 0; j < number; j++) {
			*ptr2++ += ax;
			*ptr2++ += bx;
			*ptr2++ += dx;
		}
	}

	if (modelFlags & INFO_OPTIMISE) {
		initGroupeRot(alpha, beta, gamma);
		rotateList(g_engine->_engine->pointBuffer, g_engine->_engine->numOfPoints);
	}

	{
		byte *ptr = (byte *)g_engine->_engine->pointBuffer;
		int16 *outPtr = cameraSpaceBuffer;
		int k = g_engine->_engine->numOfPoints;

		for (int i = 0; i < g_engine->_engine->numOfPoints; i++) {
			int16 X = *(int16 *)ptr;
			int16 Y = *(int16 *)(ptr + 2);
			int16 Z = *(int16 *)(ptr + 4);
			ptr += 6;

			X += renderX;
			Y += renderY;
			Z += renderZ;

			if (Y > g_engine->_engine->waterHeight) // height clamp
			{
				*outPtr++ = -10000;
				*outPtr++ = -10000;
				*outPtr++ = -10000;
			} else {
				Y -= g_engine->_engine->translateY;

				transformPoint(&X, &Y, &Z);

				*outPtr++ = X;
				*outPtr++ = Y;
				*outPtr++ = Z;
			}
		}

		ptr = (byte *)cameraSpaceBuffer;
		int16 *outPtr2 = g_engine->_engine->renderPointList;

		do {

			const int16 X = *(int16 *)ptr;
			ptr += 2;
			const int16 Y = *(int16 *)ptr;
			ptr += 2;
			int16 Z = *(int16 *)ptr;
			ptr += 2;

			Z += g_engine->_engine->cameraPerspective;

			if (Z <= 50) // clipping
			{
				*outPtr2++ = -10000;
				*outPtr2++ = -10000;
				*outPtr2++ = -10000;
			} else {
				const int16 transformedX = X * g_engine->_engine->cameraFovX / Z + g_engine->_engine->cameraCenterX;

				*outPtr2++ = transformedX;

				if (transformedX < g_engine->_engine->BBox3D1)
					g_engine->_engine->BBox3D1 = static_cast<int>(transformedX);

				if (transformedX > g_engine->_engine->BBox3D3)
					g_engine->_engine->BBox3D3 = static_cast<int>(transformedX);

				const int16 transformedY = Y * g_engine->_engine->cameraFovY / Z + g_engine->_engine->cameraCenterY;

				*outPtr2++ = transformedY;

				if (transformedY < g_engine->_engine->BBox3D2)
					g_engine->_engine->BBox3D2 = static_cast<int>(transformedY);

				if (transformedY > g_engine->_engine->BBox3D4)
					g_engine->_engine->BBox3D4 = static_cast<int>(transformedY);

				*outPtr2++ = Z;
			}

			k--;
			if (k == 0) {
				return 1;
			}

		} while (true);
	}

	return 0;
}

int rotateNuage2(int x, int y, int z, int alpha, int beta, int gamma, int16 num, int16 *vertices) {

	renderX = x - g_engine->_engine->translateX;
	renderY = y;
	renderZ = z - g_engine->_engine->translateZ;

	if (!alpha && !beta && !gamma) {
		noModelRotation = true;
	} else {
		noModelRotation = false;

		modelCosAlpha = cosTable[alpha & 0x3FF];
		modelSinAlpha = cosTable[(alpha + 0x100) & 0x3FF];

		modelCosBeta = cosTable[beta & 0x3FF];
		modelSinBeta = cosTable[(beta + 0x100) & 0x3FF];

		modelCosGamma = cosTable[gamma & 0x3FF];
		modelSinGamma = cosTable[(gamma + 0x100) & 0x3FF];
	}

	int16 *outPtr = g_engine->_engine->renderPointList;

	for (int16 i = 0; i < num; i++) {
		int16 X = vertices[i * 3];
		int16 Y = vertices[i * 3 + 1];
		int16 Z = vertices[i * 3 + 2];

		if (!noModelRotation) {
			// Y rotation
			{
				const int16 tempX = X;
				const int16 tempZ = Z;

				X = (modelSinBeta * tempX - modelCosBeta * tempZ) / 65536.f * 2.f;
				Z = (modelCosBeta * tempX + modelSinBeta * tempZ) / 65536.f * 2.f;
			}

			// Z rotation
			{
				const int16 tempX = X;
				const int16 tempY = Y;

				X = (modelSinGamma * tempX - modelCosGamma * tempY) / 65536.f * 2.f;
				Y = (modelCosGamma * tempX + modelSinGamma * tempY) / 65536.f * 2.f;
			}

			// X rotation
			{
				const int16 tempY = Y;
				const int16 tempZ = Z;

				Y = (modelSinAlpha * tempY - modelCosAlpha * tempZ) / 65536.f * 2.f;
				Z = (modelCosAlpha * tempY + modelSinAlpha * tempZ) / 65536.f * 2.f;
			}
		}

		X += renderX;
		Y += renderY;

		if (Y > g_engine->_engine->waterHeight) // height clamp
		{
			*outPtr++ = -10000;
			*outPtr++ = -10000;
			*outPtr++ = -10000;
		} else {

			Y -= g_engine->_engine->translateY;
			Z += renderZ;

			transformPoint(&X, &Y, &Z);

			Z += g_engine->_engine->cameraPerspective;
			if (Z == 0)
				Z = 1;

			const int16 transformedX = X * g_engine->_engine->cameraFovX / Z + g_engine->_engine->cameraCenterX;

			*outPtr++ = transformedX;

			if (transformedX < g_engine->_engine->BBox3D1)
				g_engine->_engine->BBox3D1 = static_cast<int>(transformedX);

			if (transformedX > g_engine->_engine->BBox3D3)
				g_engine->_engine->BBox3D3 = static_cast<int>(transformedX);

			const int16 transformedY = Y * g_engine->_engine->cameraFovY / Z + g_engine->_engine->cameraCenterY;

			*outPtr++ = transformedY;

			if (transformedY < g_engine->_engine->BBox3D2)
				g_engine->_engine->BBox3D2 = static_cast<int>(transformedY);

			if (transformedY > g_engine->_engine->BBox3D4)
				g_engine->_engine->BBox3D4 = static_cast<int>(transformedY);

			*outPtr++ = Z;

			/*  *(outPtr++) = X;
			 *(outPtr++) = Y;
			 *(outPtr++) = Z; */
		}
	}
	return 1;
}

int rotateNuage(int x, int y, int z, int alpha, int beta, int gamma, Body *pBody) {
	return rotateNuage2(x, y, z, alpha, beta, gamma, pBody->m_vertices.size(), &pBody->m_vertices[0].x);
}

static void processPrim_Line(int primType, Primitive *ptr, char **out) {
	PrimEntry *pCurrentPrimEntry = &primTable[positionInPrimEntry];

	assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);

	pCurrentPrimEntry->type = primTypeEnum_Line;
	pCurrentPrimEntry->numOfVertices = ptr->m_points.size();
	pCurrentPrimEntry->color = ptr->m_color;
	pCurrentPrimEntry->material = ptr->m_material;

	float minDepth = 32000.f;
	float depth = -32000.f;
	for (int i = 0; i < pCurrentPrimEntry->numOfVertices; i++) {
		const uint16 pointIndex = ptr->m_points[i] * 6;
		assert(pointIndex % 2 == 0);
		pCurrentPrimEntry->vertices[i].X = g_engine->_engine->renderPointList[pointIndex / 2];
		pCurrentPrimEntry->vertices[i].Y = g_engine->_engine->renderPointList[pointIndex / 2 + 1];
		pCurrentPrimEntry->vertices[i].Z = g_engine->_engine->renderPointList[pointIndex / 2 + 2];

		depth = MAX(depth, static_cast<float>(pCurrentPrimEntry->vertices[i].Z));
		minDepth = MIN(minDepth, static_cast<float>(pCurrentPrimEntry->vertices[i].Z));
	}
	pCurrentPrimEntry->depth = depth;

	if (minDepth > 100) {
		positionInPrimEntry++;

		numOfPrimitiveToRender++;
		assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);
	}
}

static void processPrim_Poly(int primType, Primitive *ptr, char **out) {
	PrimEntry *pCurrentPrimEntry = &primTable[positionInPrimEntry];

	assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);

	pCurrentPrimEntry->type = primTypeEnum_Poly;
	pCurrentPrimEntry->numOfVertices = ptr->m_points.size();
	pCurrentPrimEntry->color = ptr->m_color;
	pCurrentPrimEntry->material = ptr->m_material;

	float minDepth = 32000.f;
	float depth = -32000.f;
	assert(pCurrentPrimEntry->numOfVertices < NUM_MAX_VERTEX_IN_PRIM);
	for (int i = 0; i < pCurrentPrimEntry->numOfVertices; i++) {

		const uint16 pointIndex = ptr->m_points[i] * 6;

		assert(pointIndex % 2 == 0);

		pCurrentPrimEntry->vertices[i].X = g_engine->_engine->renderPointList[pointIndex / 2];
		pCurrentPrimEntry->vertices[i].Y = g_engine->_engine->renderPointList[pointIndex / 2 + 1];
		pCurrentPrimEntry->vertices[i].Z = g_engine->_engine->renderPointList[pointIndex / 2 + 2];

		if (pCurrentPrimEntry->vertices[i].Z > depth) {
			depth = pCurrentPrimEntry->vertices[i].Z;
		}
		if (pCurrentPrimEntry->vertices[i].Z < minDepth) {
			minDepth = pCurrentPrimEntry->vertices[i].Z;
		}
	}
	pCurrentPrimEntry->depth = depth;

	if (minDepth > 100) {
		positionInPrimEntry++;

		numOfPrimitiveToRender++;
		assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);
	}
}

static void processPrim_Point(PrimType primType, Primitive *ptr, char **out) {
	PrimEntry *pCurrentPrimEntry = &primTable[positionInPrimEntry];

	assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);

	pCurrentPrimEntry->type = primType;
	pCurrentPrimEntry->numOfVertices = 1;
	pCurrentPrimEntry->color = ptr->m_color;
	pCurrentPrimEntry->material = ptr->m_material;

	float depth = -32000.f;
	{
		const uint16 pointIndex = ptr->m_points[0] * 6;
		assert(pointIndex % 2 == 0);
		pCurrentPrimEntry->vertices[0].X = g_engine->_engine->renderPointList[pointIndex / 2];
		pCurrentPrimEntry->vertices[0].Y = g_engine->_engine->renderPointList[pointIndex / 2 + 1];
		pCurrentPrimEntry->vertices[0].Z = g_engine->_engine->renderPointList[pointIndex / 2 + 2];

		depth = MAX(depth, static_cast<float>(pCurrentPrimEntry->vertices[0].Z));
		pCurrentPrimEntry->depth = depth;
	}

	if (depth > 100) {
		positionInPrimEntry++;

		numOfPrimitiveToRender++;
		assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);
	}
}

void computeScreenBox(int x, int y, int z, int alpha, int beta, int gamma, byte *bodyPtr) {
	Body *pBody = getBodyFromPtr(bodyPtr);

	g_engine->_engine->BBox3D1 = 0x7FFF;
	g_engine->_engine->BBox3D2 = 0x7FFF;

	g_engine->_engine->BBox3D3 = -0x7FFF;
	g_engine->_engine->BBox3D4 = -0x7FFF;

	numOfPrimitiveToRender = 0;

	modelFlags = pBody->m_flags;

	if (modelFlags & INFO_ANIM) {
		pBody->sync();
		animNuage(x, y, z, alpha, beta, gamma, pBody);
	}
}

void processPrim_Sphere(int primType, Primitive *ptr, char **out) {
	PrimEntry *pCurrentPrimEntry = &primTable[positionInPrimEntry];

	assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);

	pCurrentPrimEntry->type = primTypeEnum_Sphere;
	pCurrentPrimEntry->numOfVertices = 1;
	pCurrentPrimEntry->color = ptr->m_color;
	pCurrentPrimEntry->material = ptr->m_material;
	pCurrentPrimEntry->size = ptr->m_size;

	float depth = -32000.f;
	{
		const uint16 pointIndex = ptr->m_points[0] * 6;
		assert(pointIndex % 2 == 0);
		pCurrentPrimEntry->vertices[0].X = g_engine->_engine->renderPointList[pointIndex / 2];
		pCurrentPrimEntry->vertices[0].Y = g_engine->_engine->renderPointList[pointIndex / 2 + 1];
		pCurrentPrimEntry->vertices[0].Z = g_engine->_engine->renderPointList[pointIndex / 2 + 2];

		depth = MAX(depth, static_cast<float>(pCurrentPrimEntry->vertices[0].Z));
		pCurrentPrimEntry->depth = depth;
	}

	if (depth > 100) {
		positionInPrimEntry++;

		numOfPrimitiveToRender++;
		assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);
	}
}

typedef void (*renderFunction)(PrimEntry *buffer);

void renderLine(PrimEntry *pEntry) // line
{
	renderer.renderLine(pEntry->vertices[0].X, pEntry->vertices[0].Y, pEntry->vertices[0].Z, pEntry->vertices[1].X, pEntry->vertices[1].Y, pEntry->vertices[1].Z, pEntry->color);
}

void renderPoly(PrimEntry *pEntry) // poly
{
	osystem_fillPoly((int16 *)pEntry->vertices, pEntry->numOfVertices, pEntry->color, pEntry->material);
}

void renderZixel(PrimEntry *pEntry) // point
{
	const float pointSize = 20.f;
	const float transformedSize = pointSize * static_cast<float>(g_engine->_engine->cameraFovX) / static_cast<float>(pEntry->vertices[0].Z + g_engine->_engine->cameraPerspective);

	drawZixel(pEntry->vertices[0].X, pEntry->vertices[0].Y, pEntry->vertices[0].Z, pEntry->color, pEntry->material, transformedSize);
}

void renderPoint(PrimEntry *pEntry) // point
{
	drawPoint(pEntry->vertices[0].X, pEntry->vertices[0].Y, pEntry->vertices[0].Z, pEntry->color);
}

void renderBigPoint(PrimEntry *pEntry) // point
{
	drawBigPoint(pEntry->vertices[0].X, pEntry->vertices[0].Y, pEntry->vertices[0].Z, pEntry->color);
}

void renderSphere(PrimEntry *pEntry) // sphere
{
	const float transformedSize = static_cast<float>(pEntry->size) * static_cast<float>(g_engine->_engine->cameraFovX) / static_cast<float>(pEntry->vertices[0].Z + g_engine->_engine->cameraPerspective);

	drawSphere(pEntry->vertices[0].X, pEntry->vertices[0].Y, pEntry->vertices[0].Z, pEntry->color, pEntry->material, transformedSize);
}

renderFunction renderFunctions[] = {
	renderLine,   // line
	renderPoly,   // poly
	renderPoint,  // point
	renderSphere, // sphere
	nullptr,
	nullptr,
	renderBigPoint,
	renderZixel,
};

/// @brief Sort primitives by material, the transparent primitives have to be last.
/// @param prim1 First primitive to test.
/// @param prim2 Second primitive to test.
/// @return 0 if equals, -1 if primitive has to be first else 1.
static int primCompare(const void *prim1, const void *prim2) {
	const PrimEntry *p1 = static_cast<const PrimEntry *>(prim1);
	const PrimEntry *p2 = static_cast<const PrimEntry *>(prim2);
	return static_cast<int>(p2->depth) - static_cast<int>(p1->depth);
}

int affObjet(int x, int y, int z, int alpha, int beta, int gamma, void *modelPtr) {
	Body *pBody = getBodyFromPtr(modelPtr);
	const byte *ptr = static_cast<byte *>(modelPtr);
	int i;
	char *out;

	// reinit the 2 static tables
	positionInPrimEntry = 0;
	//

	g_engine->_engine->BBox3D1 = 0x7FFF;
	g_engine->_engine->BBox3D2 = 0x7FFF;

	g_engine->_engine->BBox3D3 = -0x7FFF;
	g_engine->_engine->BBox3D4 = -0x7FFF;

	numOfPrimitiveToRender = 0;

	modelFlags = READ_LE_S16(ptr);
	ptr += 2;
	ptr += 12; // skip the ZV

	ptr += READ_LE_S16(ptr) + 2; // skip scratch buffer

	if (modelFlags & INFO_ANIM) {
		pBody->sync();
		if (!animNuage(x, y, z, alpha, beta, gamma, pBody)) {
			g_engine->_engine->BBox3D3 = -32000;
			g_engine->_engine->BBox3D4 = -32000;
			g_engine->_engine->BBox3D1 = 32000;
			g_engine->_engine->BBox3D2 = 32000;
			return 2;
		}
	} else if (!(modelFlags & INFO_TORTUE)) {
		if (!rotateNuage(x, y, z, alpha, beta, gamma, pBody)) {
			g_engine->_engine->BBox3D3 = -32000;
			g_engine->_engine->BBox3D4 = -32000;
			g_engine->_engine->BBox3D1 = 32000;
			g_engine->_engine->BBox3D2 = 32000;
			return 2;
		}
	} else {
		debug("unsupported model type prerenderFlag4 in renderer !\n");

		g_engine->_engine->BBox3D3 = -32000;
		g_engine->_engine->BBox3D4 = -32000;
		g_engine->_engine->BBox3D1 = 32000;
		g_engine->_engine->BBox3D2 = 32000;
		return 2;
	}

	const int numPrim = pBody->m_primitives.size();

	if (!numPrim) {
		g_engine->_engine->BBox3D3 = -32000;
		g_engine->_engine->BBox3D4 = -32000;
		g_engine->_engine->BBox3D1 = 32000;
		g_engine->_engine->BBox3D2 = 32000;
		return 2;
	}

	out = primBuffer;

	// create the list of all primitives to render
	for (i = 0; i < numPrim; i++) {
		Primitive *pPrimitive = &pBody->m_primitives[i];
		const PrimType primType = pPrimitive->m_type;

		switch (primType) {
		case primTypeEnum_Line:
			processPrim_Line(primType, pPrimitive, &out);
			break;
		case primTypeEnum_Poly:
			processPrim_Poly(primType, pPrimitive, &out);
			break;
		case primTypeEnum_Point:
		case primTypeEnum_BigPoint:
		case primTypeEnum_Zixel:
			processPrim_Point(primType, pPrimitive, &out);
			break;
		case primTypeEnum_Sphere:
			processPrim_Sphere(primType, pPrimitive, &out);
			break;
		case processPrim_PolyTexture9:
		case processPrim_PolyTexture10:
			processPrim_Poly(primType, pPrimitive, &out);
			ptr += 3 * 2;
			break;
		default:
			return 0;
		}
	}

	if (numOfPrimitiveToRender) {
		qsort(primTable, numOfPrimitiveToRender, sizeof(PrimEntry), primCompare);
	}

	if (!numOfPrimitiveToRender) {
		g_engine->_engine->BBox3D3 = -32000;
		g_engine->_engine->BBox3D4 = -32000;
		g_engine->_engine->BBox3D1 = 32000;
		g_engine->_engine->BBox3D2 = 32000;
		return 1; // model ok, but out of screen
	}

	for (i = 0; i < numOfPrimitiveToRender; i++) {
		renderFunctions[primTable[i].type](&primTable[i]);
	}

	flushPendingPrimitives();
	return 0;
}

void setupCameraProjection(int centerX, int centerY, int x, int y, int z) {
	g_engine->_engine->cameraCenterX = centerX;
	g_engine->_engine->cameraCenterY = centerY;

	g_engine->_engine->cameraPerspective = x;
	g_engine->_engine->cameraFovX = y;
	g_engine->_engine->cameraFovY = z;
}

void setClip(int left, int top, int right, int bottom) {
	g_engine->_engine->clipLeft = left;
	g_engine->_engine->clipTop = top;
	g_engine->_engine->clipRight = right;
	g_engine->_engine->clipBottom = bottom;
}

void fillBox(int x1, int y1, int x2, int y2, char color) // fast recode. No RE
{
	x1 = CLIP(x1, 0, 320);
	x2 = CLIP(x2, 0, 320);
	y1 = CLIP(y1, 0, 199);
	y2 = CLIP(y2, 0, 199);
	const int width = x2 - x1 + 1;
	const int height = y2 - y1 + 1;

	byte *dest = g_engine->_engine->logicalScreen + y1 * 320 + x1;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			*dest++ = color;
		}

		dest += 320 - width;
	}
}

void flushScreen() {
	for (int i = 0; i < 200; i++) {
		for (int j = 0; j < 320; j++) {
			*(g_engine->_engine->logicalScreen + i * 320 + j) = 0;
		}
	}
}

void createMask(const uint8 *mask, int roomId, int maskId, byte *refImage, int maskX1, int maskY1, int maskX2, int maskY2) {
	renderer.createMask(mask, roomId, maskId, refImage, maskX1, maskY1, maskX2, maskY2);
}

void setClip() {
	renderer.setClip(g_engine->_engine->clipLeft, g_engine->_engine->clipTop, g_engine->_engine->clipRight, g_engine->_engine->clipBottom);
}

void clearClip() {
	renderer.clearClip();
}

void drawMask(int roomId, int maskId) {
	renderer.drawMask(roomId, maskId);
}

void flip() {
	flushPendingPrimitives();
}

void drawZixel(float X, float Y, float Z, uint8 color, uint8 material, float size) {
	renderer.drawZixel(X, Y, Z, color, material, size);
}

void drawPoint(float X, float Y, float Z, uint8 color) {
	renderer.drawPoint(X, Y, Z, color);
}

void drawBigPoint(float X, float Y, float Z, uint8 color) {
	renderer.drawPoint(X, Y, Z, color);
}

void drawSphere(float X, float Y, float Z, uint8 color, uint8 material, float size) {
	renderer.drawSphere(X, Y, Z, color, material, size);
}

void copyBoxLogPhys(int left, int top, int right, int bottom) {
	renderer.copyBoxLogPhys(left, top, right, bottom);
}

void copyBlock(byte *in, byte *out, int left, int top, int right, int bottom) {

	while ((right - left) % 4) {
		right++;
	}

	while ((bottom - top) % 4) {
		bottom++;
	}

	for (int i = top; i < bottom; i++) {
		const byte *in2 = in + left + i * 320;
		byte *out2 = out + left + i * 320;

		for (int j = left; j < right; j++) {
			const byte color = *in2++;
			*out2++ = color;
		}
	}
}

void updateScreen() {
	renderer.updateScreen();
}

} // namespace Fitd
