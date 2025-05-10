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


#include "common/config-manager.h"
#include "common/debug.h"
#include "common/rendermode.h"
#include "common/scummsys.h"
#include "common/system.h"
#include "fitd/anim.h"
#include "fitd/common.h"
#include "fitd/costable.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/renderer.h"
#include "fitd/renderer_opengl.h"
#include "fitd/renderer_soft.h"
#include "fitd/vars.h"

namespace Fitd {

#define NUM_MAX_VERTEX_IN_PRIM 64
#define NUM_MAX_PRIM_ENTRY 500
#define NUM_MAX_BONES 50

int16 pointBuffer[NUM_MAX_POINT_IN_POINT_BUFFER * 3];

int numOfPoints;
int numOfBones;

int16 cameraSpaceBuffer[NUM_MAX_POINT_IN_POINT_BUFFER * 3];

bool boneRotateX;
bool boneRotateY;
bool boneRotateZ;

int boneRotateXCos;
int boneRotateXSin;
int boneRotateYCos;
int boneRotateYSin;
int boneRotateZCos;
int boneRotateZSin;

typedef struct rendererPointStruct {
	int16 X;
	int16 Y;
	int16 Z;
} rendererPointStruct;

typedef struct primEntryStruct {
	uint8 material;
	uint8 color;
	uint16 size;
	uint16 numOfVertices;
	primTypeEnum type;
	rendererPointStruct vertices[NUM_MAX_VERTEX_IN_PRIM];
} primEntryStruct;

primEntryStruct primTable[NUM_MAX_PRIM_ENTRY];

bool noModelRotation;

int modelCosAlpha;
int modelSinAlpha;
int modelCosBeta;
int modelSinBeta;
int modelCosGamma;
int modelSinGamma;

uint32 positionInPrimEntry = 0;

int renderVar1 = 0;

int numOfPrimitiveToRender = 0;

char renderBuffer[3261];

char *renderVar2 = nullptr;

int modelFlags = 0;

char primBuffer[30000];

int BBox3D1 = 0;
int BBox3D2 = 0;
int BBox3D3 = 0;
int BBox3D4 = 0;

int renderX;
int renderY;
int renderZ;
Renderer renderer;
byte frontBuffer[320 * 200];

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
	renderer.deinit();
}

void osystem_startFrame() {
	renderer.startFrame();
	osystem_drawBackground();
}

void osystem_drawBackground() {
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

static void osystem_fillPoly(int16 *buffer, int numPoint, unsigned char color, uint8 polyType) {
	renderer.fillPoly(buffer, numPoint, color, polyType);
}

void osystem_flushPendingPrimitives() {
	renderer.flushPendingPrimitives();
}

void setPosCamera(int x, int y, int z) {
	translateX = x;
	translateY = y;
	translateZ = z;
}

void setAngleCamera(int x, int y, int z) {
	transformX = x & 0x3FF;
	if (transformX) {
		transformXCos = cosTable[transformX];
		transformXSin = cosTable[transformX + 0x100 & 0x3FF];
		transformUseX = true;
	} else {
		transformUseX = false;
	}

	transformY = y & 0x3FF;
	if (transformY) {
		transformYCos = cosTable[transformY];
		transformYSin = cosTable[transformY + 0x100 & 0x3FF];
		transformUseY = true;
	} else {
		transformUseY = false;
	}

	transformZ = z & 0x3FF;
	if (transformZ) {
		transformZCos = cosTable[transformZ];
		transformZSin = cosTable[transformZ + 0x100 & 0x3FF];
		transformUseZ = true;
	} else {
		transformUseZ = false;
	}
}

void rotate(unsigned int x, unsigned int y, unsigned int z, int *xOut, int *yOut) {
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
		int *icx = &Z;

		{
			int x;
			int y;
			int z;

			if (transformUseY) {
				x = ((*iax * transformYSin - *icx * transformYCos) / 0x10000) << 1;
				z = ((*iax * transformYCos + *icx * transformYSin) / 0x10000) << 1;
			} else {
				x = *iax;
				z = *icx;
			}

			// si = x
			// ax = z

			if (transformUseX) {
				const int tempY = *ibx;
				const int tempZ = z;
				y = ((tempY * transformXSin - tempZ * transformXCos) / 0x10000) << 1;
				z = ((tempY * transformXCos + tempZ * transformXSin) / 0x10000) << 1;
			} else {
				y = *ibx;
			}

			// cx = y
			// bx = z

			if (transformUseZ) {
				const int tempX = x;
				const int tempY = y;
				x = ((tempX * transformZSin - tempY * transformZCos) / 0x10000) << 1;
				y = ((tempX * transformZCos + tempY * transformZSin) / 0x10000) << 1;
			}

			*iax = x;
			*ibx = y;
			*icx = z;
		}
	}

	*ax = (int16)X;
	*bx = (int16)Y;
	*cx = (int16)Z;
}

static void TranslateGroupe(int transX, int transY, int transZ, const sGroup *ptr) {
	int16 *ptrSource = &pointBuffer[ptr->m_start * 3];

	for (int i = 0; i < ptr->m_numVertices; i++) {
		*ptrSource++ += transX;
		*ptrSource++ += transY;
		*ptrSource++ += transZ;
	}
}

static void ZoomGroupe(int zoomX, int zoomY, int zoomZ, const sGroup *ptr) {
	int16 *ptrSource = &pointBuffer[ptr->m_start * 3];

	for (int i = 0; i < ptr->m_numVertices; i++) {
		*ptrSource = *ptrSource * (zoomX + 256) / 256;
		ptrSource++;
		*ptrSource = *ptrSource * (zoomY + 256) / 256;
		ptrSource++;
		*ptrSource = *ptrSource * (zoomZ + 256) / 256;
		ptrSource++;
	}
}

static void InitGroupeRot(int transX, int transY, int transZ) {
	if (transX) {
		boneRotateXCos = cosTable[transX & 0x3FF];
		boneRotateXSin = cosTable[transX + 0x100 & 0x3FF];

		boneRotateX = true;
	} else {
		boneRotateX = false;
	}

	if (transY) {
		boneRotateYCos = cosTable[transY & 0x3FF];
		boneRotateYSin = cosTable[transY + 0x100 & 0x3FF];

		boneRotateY = true;
	} else {
		boneRotateY = false;
	}

	if (transZ) {
		boneRotateZCos = cosTable[transZ & 0x3FF];
		boneRotateZSin = cosTable[transZ + 0x100 & 0x3FF];

		boneRotateZ = true;
	} else {
		boneRotateZ = false;
	}
}

static void RotateList(int16 *pointPtr, int numOfPoint) {
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

static void RotateGroupeOptimise(const sGroup *ptr) {
	if (ptr->m_numGroup) // if group number is 0
	{
		const int baseBone = ptr->m_start;
		const int numPoints = ptr->m_numVertices;

		RotateList(pointBuffer + baseBone * 3, numPoints);
	}
}

static void RotateGroupe(sGroup *ptr) {
	const int baseBone = ptr->m_start;
	const int numPoints = ptr->m_numVertices;

	RotateList(pointBuffer + baseBone * 3, numPoints);

	const int temp = ptr->m_numGroup; // group number

	int temp2 = numOfBones - temp;

	do {
		if (ptr->m_orgGroup == temp) // is it on of this group child
		{
			RotateGroupe(ptr); // yes, so apply the transformation to him
		}

		ptr++;
	} while (--temp2);
}

static int animNuage(int x, int y, int z, int alpha, int beta, int gamma, sBody *pBody) {
	renderX = x - translateX;
	renderY = y;
	renderZ = z - translateZ;

	assert(pBody->m_vertices.size() < NUM_MAX_POINT_IN_POINT_BUFFER);

	for (uint i = 0; i < pBody->m_vertices.size(); i++) {
		pointBuffer[i * 3 + 0] = pBody->m_vertices[i].x;
		pointBuffer[i * 3 + 1] = pBody->m_vertices[i].y;
		pointBuffer[i * 3 + 2] = pBody->m_vertices[i].z;
	}

	numOfPoints = pBody->m_vertices.size();
	numOfBones = pBody->m_groupOrder.size();
	assert(numOfBones < NUM_MAX_BONES);

	if (pBody->m_flags & INFO_OPTIMISE) {
		for (uint i = 0; i < pBody->m_groupOrder.size(); i++) {
			const sGroup *pGroup = &pBody->m_groups[pBody->m_groupOrder[i]];

			switch (pGroup->m_state.m_type) {
			case 1:
				if (pGroup->m_state.m_delta[0] || pGroup->m_state.m_delta[1] || pGroup->m_state.m_delta[2]) {
					TranslateGroupe(pGroup->m_state.m_delta[0], pGroup->m_state.m_delta[1], pGroup->m_state.m_delta[2], pGroup);
				}
				break;
			case 2:
				if (pGroup->m_state.m_delta[0] || pGroup->m_state.m_delta[1] || pGroup->m_state.m_delta[2]) {
					ZoomGroupe(pGroup->m_state.m_delta[0], pGroup->m_state.m_delta[1], pGroup->m_state.m_delta[2], pGroup);
				}
				break;
			}

			InitGroupeRot(pGroup[0].m_state.m_rotateDelta[0], pGroup[0].m_state.m_rotateDelta[1], pGroup[0].m_state.m_rotateDelta[2]);
			RotateGroupeOptimise(pGroup);
		}
	} else {
		pBody->m_groups[0].m_state.m_delta[0] = alpha;
		pBody->m_groups[0].m_state.m_delta[1] = beta;
		pBody->m_groups[0].m_state.m_delta[2] = gamma;

		for (uint i = 0; i < pBody->m_groups.size(); i++) {
			sGroup *pGroup = &pBody->m_groups[pBody->m_groupOrder[i]];

			const int transX = pGroup->m_state.m_delta[0];
			const int transY = pGroup->m_state.m_delta[1];
			const int transZ = pGroup->m_state.m_delta[2];

			if (transX || transY || transZ) {
				switch (pGroup->m_state.m_type) {
				case 0: {
					InitGroupeRot(transX, transY, transZ);
					RotateGroupe(pGroup);
					break;
				}
				case 1: {
					TranslateGroupe(transX, transY, transZ, pGroup);
					break;
				}
				case 2: {
					ZoomGroupe(transX, transY, transZ, pGroup);
					break;
				}
				}
			}
		}
	}

	for (uint i = 0; i < pBody->m_groups.size(); i++) {
		const sGroup *pGroup = &pBody->m_groups[i];

		int point1 = pGroup->m_baseVertices * 6;
		int point2 = pGroup->m_start * 6;

		assert(point1 % 2 == 0);
		assert(point2 % 2 == 0);

		point1 /= 2;
		point2 /= 2;

		assert(point1 / 3 < NUM_MAX_POINT_IN_POINT_BUFFER);
		assert(point2 / 3 < NUM_MAX_POINT_IN_POINT_BUFFER);

		const int16 *ptr1 = &pointBuffer[point1];
		int16 *ptr2 = &pointBuffer[point2];

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
		InitGroupeRot(alpha, beta, gamma);
		RotateList(pointBuffer, numOfPoints);
	}

	{
		char *ptr = (char *)pointBuffer;
		int16 *outPtr = cameraSpaceBuffer;
		int k = numOfPoints;

		for (int i = 0; i < numOfPoints; i++) {
			int16 X = *(int16 *)ptr;
			int16 Y = *(int16 *)(ptr + 2);
			int16 Z = *(int16 *)(ptr + 4);
			ptr += 6;

			X += renderX;
			Y += renderY;
			Z += renderZ;

			if (Y > 10000) // height clamp
			{
				*outPtr++ = -10000;
				*outPtr++ = -10000;
				*outPtr++ = -10000;
			} else {
				Y -= translateY;

				transformPoint(&X, &Y, &Z);

				*outPtr++ = X;
				*outPtr++ = Y;
				*outPtr++ = Z;
			}
		}

		ptr = (char *)cameraSpaceBuffer;
		int16 *outPtr2 = renderPointList;

		do {

			const int16 X = *(int16 *)ptr;
			ptr += 2;
			const int16 Y = *(int16 *)ptr;
			ptr += 2;
			int16 Z = *(int16 *)ptr;
			ptr += 2;

			Z += cameraPerspective;

			if (Z <= 50) // clipping
			{
				*outPtr2++ = -10000;
				*outPtr2++ = -10000;
				*outPtr2++ = -10000;
			} else {
				const int16 transformedX = X * cameraFovX / Z + cameraCenterX;

				*outPtr2++ = transformedX;

				if (transformedX < BBox3D1)
					BBox3D1 = (int)transformedX;

				if (transformedX > BBox3D3)
					BBox3D3 = (int)transformedX;

				const int16 transformedY = Y * cameraFovY / Z + cameraCenterY;

				*outPtr2++ = transformedY;

				if (transformedY < BBox3D2)
					BBox3D2 = (int)transformedY;

				if (transformedY > BBox3D4)
					BBox3D4 = (int)transformedY;

				*outPtr2++ = Z;
			}

			k--;
			if (k == 0) {
				return 1;
			}

		} while (renderVar1 == 0);
	}

	return 0;
}

static int rotateNuage(int x, int y, int z, int alpha, int beta, int gamma, sBody *pBody) {

	renderX = x - translateX;
	renderY = y;
	renderZ = z - translateZ;

	if (!alpha && !beta && !gamma) {
		noModelRotation = true;
	} else {
		noModelRotation = false;

		modelCosAlpha = cosTable[alpha & 0x3FF];
		modelSinAlpha = cosTable[alpha + 0x100 & 0x3FF];

		modelCosBeta = cosTable[beta & 0x3FF];
		modelSinBeta = cosTable[beta + 0x100 & 0x3FF];

		modelCosGamma = cosTable[gamma & 0x3FF];
		modelSinGamma = cosTable[gamma + 0x100 & 0x3FF];
	}

	int16 *outPtr = renderPointList;

	for (uint i = 0; i < pBody->m_vertices.size(); i++) {
		int16 X = pBody->m_vertices[i].x;
		int16 Y = pBody->m_vertices[i].y;
		int16 Z = pBody->m_vertices[i].z;

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
		Z += renderZ;

		if (Y > 10000) // height clamp
		{
			*outPtr++ = -10000;
			*outPtr++ = -10000;
			*outPtr++ = -10000;
		} else {

			Y -= translateY;

			transformPoint(&X, &Y, &Z);

			Z += cameraPerspective;
			if (Z == 0)
				Z = 1;

			const int16 transformedX = X * cameraFovX / Z + cameraCenterX;

			*outPtr++ = transformedX;

			if (transformedX < BBox3D1)
				BBox3D1 = (int)transformedX;

			if (transformedX > BBox3D3)
				BBox3D3 = (int)transformedX;

			const int16 transformedY = Y * cameraFovY / Z + cameraCenterY;

			*outPtr++ = transformedY;

			if (transformedY < BBox3D2)
				BBox3D2 = (int)transformedY;

			if (transformedY > BBox3D4)
				BBox3D4 = (int)transformedY;

			*outPtr++ = Z;

			/*  *(outPtr++) = X;
			 *(outPtr++) = Y;
			 *(outPtr++) = Z; */
		}
	}
	return 1;
}

static void processPrim_Line(int primType, sPrimitive *ptr, char **out) {
}

static void processPrim_Poly(int primType, sPrimitive *ptr, char **out) {
	primEntryStruct *pCurrentPrimEntry = &primTable[positionInPrimEntry];

	assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);

	pCurrentPrimEntry->type = primTypeEnum_Poly;
	pCurrentPrimEntry->numOfVertices = ptr->m_points.size();
	pCurrentPrimEntry->color = ptr->m_color;
	pCurrentPrimEntry->material = ptr->m_material;

	float depth = 32000.f;
	assert(pCurrentPrimEntry->numOfVertices < NUM_MAX_VERTEX_IN_PRIM);
	for (int i = 0; i < pCurrentPrimEntry->numOfVertices; i++) {

		const uint16 pointIndex = ptr->m_points[i] * 6;

		assert(pointIndex % 2 == 0);

		pCurrentPrimEntry->vertices[i].X = renderPointList[pointIndex / 2];
		pCurrentPrimEntry->vertices[i].Y = renderPointList[pointIndex / 2 + 1];
		pCurrentPrimEntry->vertices[i].Z = renderPointList[pointIndex / 2 + 2];

		if (pCurrentPrimEntry->vertices[i].Z < depth)
			depth = pCurrentPrimEntry->vertices[i].Z;
	}

	if (depth > 100) {
		positionInPrimEntry++;

		numOfPrimitiveToRender++;
		assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);
	}
}

static void processPrim_Point(primTypeEnum primType, sPrimitive *ptr, char **out) {
	primEntryStruct *pCurrentPrimEntry = &primTable[positionInPrimEntry];

	assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);

	pCurrentPrimEntry->type = primType;
	pCurrentPrimEntry->numOfVertices = 1;
	pCurrentPrimEntry->color = ptr->m_color;
	pCurrentPrimEntry->material = ptr->m_material;

	float depth = 32000.f;
	{
		const uint16 pointIndex = ptr->m_points[0] * 6;
		assert(pointIndex % 2 == 0);
		pCurrentPrimEntry->vertices[0].X = renderPointList[pointIndex / 2];
		pCurrentPrimEntry->vertices[0].Y = renderPointList[pointIndex / 2 + 1];
		pCurrentPrimEntry->vertices[0].Z = renderPointList[pointIndex / 2 + 2];

		depth = pCurrentPrimEntry->vertices[0].Z;
	}

	if (depth > 100) {
		positionInPrimEntry++;

		numOfPrimitiveToRender++;
		assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);
	}
}

void computeScreenBox(int x, int y, int z, int alpha, int beta, int gamma, char *bodyPtr) {
	sBody *pBody = getBodyFromPtr(bodyPtr);

	BBox3D1 = 0x7FFF;
	BBox3D2 = 0x7FFF;

	BBox3D3 = -0x7FFF;
	BBox3D4 = -0x7FFF;

	renderVar1 = 0;

	numOfPrimitiveToRender = 0;

	renderVar2 = renderBuffer;

	modelFlags = pBody->m_flags;

	if (modelFlags & INFO_ANIM) {
		pBody->sync();
		animNuage(x, y, z, alpha, beta, gamma, pBody);
	}
}

void processPrim_Sphere(int primType, sPrimitive *ptr, char **out) {
	primEntryStruct *pCurrentPrimEntry = &primTable[positionInPrimEntry];

	assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);

	pCurrentPrimEntry->type = primTypeEnum_Sphere;
	pCurrentPrimEntry->numOfVertices = 1;
	pCurrentPrimEntry->color = ptr->m_color;
	pCurrentPrimEntry->material = ptr->m_material;
	pCurrentPrimEntry->size = ptr->m_size;

	float depth = 32000.f;
	{
		const uint16 pointIndex = ptr->m_points[0] * 6;
		assert(pointIndex % 2 == 0);
		pCurrentPrimEntry->vertices[0].X = renderPointList[pointIndex / 2];
		pCurrentPrimEntry->vertices[0].Y = renderPointList[pointIndex / 2 + 1];
		pCurrentPrimEntry->vertices[0].Z = renderPointList[pointIndex / 2 + 2];

		depth = pCurrentPrimEntry->vertices[0].Z;
	}

	if (depth > 100) {
		positionInPrimEntry++;

		numOfPrimitiveToRender++;
		assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);
	}
}

typedef void (*renderFunction)(primEntryStruct *buffer);

void renderLine(primEntryStruct *pEntry) // line
{
	// TODO:
	assert(false);
}

void renderPoly(primEntryStruct *pEntry) // poly
{
	osystem_fillPoly((int16 *)pEntry->vertices, pEntry->numOfVertices, pEntry->color, pEntry->material);
}

void renderZixel(primEntryStruct *pEntry) // point
{
	const float pointSize = 20.f;
	const float transformedSize = pointSize * (float)cameraFovX / (float)(pEntry->vertices[0].Z + cameraPerspective);

	osystem_drawPoint(pEntry->vertices[0].X, pEntry->vertices[0].Y, pEntry->vertices[0].Z, pEntry->color, pEntry->material, transformedSize);
}

void renderPoint(primEntryStruct *pEntry) // point
{
	const float pointSize = 0.3f; // TODO: better way to compute that?
	osystem_drawPoint(pEntry->vertices[0].X, pEntry->vertices[0].Y, pEntry->vertices[0].Z, pEntry->color, pEntry->material, pointSize);
}

void renderBigPoint(primEntryStruct *pEntry) // point
{
	const float bigPointSize = 2.f; // TODO: better way to compute that?
	osystem_drawPoint(pEntry->vertices[0].X, pEntry->vertices[0].Y, pEntry->vertices[0].Z, pEntry->color, pEntry->material, bigPointSize);
}

void renderSphere(primEntryStruct *pEntry) // sphere
{

	const float transformedSize = (float)pEntry->size * (float)cameraFovX / (float)(pEntry->vertices[0].Z + cameraPerspective);

	osystem_drawSphere(pEntry->vertices[0].X, pEntry->vertices[0].Y, pEntry->vertices[0].Z, pEntry->color, pEntry->material, transformedSize);
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
	const primEntryStruct *p1 = (const primEntryStruct *)prim1;
	const primEntryStruct *p2 = (const primEntryStruct *)prim2;
	if (p1->material == p2->material)
		return 0;
	if (p1->material == 2)
		return 1;
	if (p2->material == 2)
		return -1;
	if (p1->material > p2->material)
		return -1;
	return 1;
}

int affObjet(int x, int y, int z, int alpha, int beta, int gamma, void *modelPtr) {
	sBody *pBody = getBodyFromPtr(modelPtr);
	const char *ptr = (char *)modelPtr;
	int i;
	char *out;

	// reinit the 2 static tables
	positionInPrimEntry = 0;
	//

	BBox3D1 = 0x7FFF;
	BBox3D2 = 0x7FFF;

	BBox3D3 = -0x7FFF;
	BBox3D4 = -0x7FFF;

	renderVar1 = 0;

	numOfPrimitiveToRender = 0;

	renderVar2 = renderBuffer;

	modelFlags = READ_LE_S16(ptr);
	ptr += 2;
	ptr += 12; // skip the ZV

	ptr += READ_LE_S16(ptr) + 2; // skip scratch buffer

	if (modelFlags & INFO_ANIM) {
		pBody->sync();
		if (!animNuage(x, y, z, alpha, beta, gamma, pBody)) {
			BBox3D3 = -32000;
			BBox3D4 = -32000;
			BBox3D1 = 32000;
			BBox3D2 = 32000;
			return 2;
		}
	} else if (!(modelFlags & INFO_TORTUE)) {
		if (!rotateNuage(x, y, z, alpha, beta, gamma, pBody)) {
			BBox3D3 = -32000;
			BBox3D4 = -32000;
			BBox3D1 = 32000;
			BBox3D2 = 32000;
			return 2;
		}
	} else {
		debug("unsupported model type prerenderFlag4 in renderer !\n");

		BBox3D3 = -32000;
		BBox3D4 = -32000;
		BBox3D1 = 32000;
		BBox3D2 = 32000;
		return 2;
	}

	const int numPrim = pBody->m_primitives.size();

	if (!numPrim) {
		BBox3D3 = -32000;
		BBox3D4 = -32000;
		BBox3D1 = 32000;
		BBox3D2 = 32000;
		return 2;
	}

	out = primBuffer;

	// create the list of all primitives to render
	for (i = 0; i < numPrim; i++) {
		sPrimitive *pPrimitive = &pBody->m_primitives[i];
		const primTypeEnum primType = pPrimitive->m_type;

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

#if 0
		 // TODO: poly sorting by depth
#ifdef USE_GL2
		 source = renderBuffer;
#else
		 inBuffer = renderBuffer;
		 outBuffer = sortedBuffer;

		 for(i=0;i<numOfPolyToRender;i++)
		 {
			 int j;
			 int bestIdx;
			 int bestDepth = -32000;
			 char* readBuffer = renderBuffer;

			 for(j=0;j<numOfPolyToRender;j++)
			 {
				 int depth = READ_LE_S16(readBuffer);

				 if(depth>bestDepth)
				 {
					 bestIdx = j;
					 bestDepth = depth;
				 }

				 readBuffer+=10;
			 }

			 memcpy(outBuffer,renderBuffer+10*bestIdx,10);
			 *(int16*)(renderBuffer+10*bestIdx) = -32000;
			 outBuffer+=10;
		 }
		 source = sortedBuffer;

#endif
#endif
	if (numOfPrimitiveToRender) {
		qsort(primTable, numOfPrimitiveToRender, sizeof(primEntryStruct), primCompare);
	}

	//

	if (!numOfPrimitiveToRender) {
		BBox3D3 = -32000;
		BBox3D4 = -32000;
		BBox3D1 = 32000;
		BBox3D2 = 32000;
		return 1; // model ok, but out of screen
	}

	//  source += 10 * 1;
	for (i = 0; i < numOfPrimitiveToRender; i++) {
		renderFunctions[primTable[i].type](&primTable[i]);
	}

	// DEBUG
	/*  for(i=0;i<numPointInPoly;i++)
	{
	int x;
	int y;

	x = renderPointList[i*3];
	y = renderPointList[i*3+1];

	if(x>=0 && x < 319 && y>=0 && y<199)
	{
	screen[y*320+x] = 15;
	}
	}*/
	//

	osystem_flushPendingPrimitives();
	return 0;
}

void setupCameraProjection(int centerX, int centerY, int x, int y, int z) {
	cameraCenterX = centerX;
	cameraCenterY = centerY;

	cameraPerspective = x;
	cameraFovX = y;
	cameraFovY = z;
}

void setClip(int left, int top, int right, int bottom) {
	clipLeft = left;
	clipTop = top;
	clipRight = right;
	clipBottom = bottom;
}

void fillBox(int x1, int y1, int x2, int y2, char color) // fast recode. No RE
{
	x1 = CLIP(x1, 0, 320);
	x2 = CLIP(x2, 0, 320);
	y1 = CLIP(y1, 0, 199);
	y2 = CLIP(y2, 0, 199);
	const int width = x2 - x1 + 1;
	const int height = y2 - y1 + 1;

	char *dest = logicalScreen + y1 * 320 + x1;

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
			*(logicalScreen + i * 320 + j) = 0;
		}
	}
}

void osystem_createMask(const uint8 *mask, int roomId, int maskId, unsigned char *refImage, int maskX1, int maskY1, int maskX2, int maskY2) {
	renderer.createMask(mask, roomId, maskId, refImage, maskX1, maskY1, maskX2, maskY2);
}

void osystem_stopModelRender() {
	osystem_flushPendingPrimitives();
}

void osystem_setClip(float left, float top, float right, float bottom) {

	renderer.setClip(left, top, right, bottom);
}

void osystem_clearClip() {
	renderer.clearClip();
}

void osystem_drawMask(int roomId, int maskId) {
	renderer.drawMask(roomId, maskId);
}

void osystem_flip(unsigned char *videoBuffer) {
	osystem_flushPendingPrimitives();
}

void osystem_drawPoint(float X, float Y, float Z, uint8 color, uint8 material, float size) {
	renderer.drawPoint(X, Y, Z, color, material, size);
}

void osystem_drawSphere(float X, float Y, float Z, uint8 color, uint8 material, float size) {
	osystem_drawPoint(X, Y, Z, color, material, size);
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

void osystem_updateScreen() {
	renderer.updateScreen();
}

Graphics::Surface *gfx_capture() {
	return renderer.capture();
}

} // namespace Fitd
