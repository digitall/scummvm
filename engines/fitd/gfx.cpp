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

#include "fitd/costable.h"
#include "fitd/common.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/vars.h"
#include "common/debug.h"
#include "graphics/opengl/context.h"
#include "graphics/opengl/debug.h"
#include "graphics/opengl/shader.h"
#include "graphics/opengl/system_headers.h"

namespace Fitd {

#define NUM_MAX_FLAT_VERTICES 5000 * 3
#define NUM_MAX_NOISE_VERTICES 2000 * 3
#define NUM_MAX_TRANSPARENT_VERTICES 1000 * 2
#define NUM_MAX_RAMP_VERTICES 3000 * 3
#define NUM_MAX_SPHERES_VERTICES 3000

#define NUM_MAX_VERTEX_IN_PRIM 64
#define NUM_MAX_PRIM_ENTRY 500

struct Vector2 {
	float x;
	float y;
};

struct Vertex {
	Vector2 pos;
	Vector2 texCoords;
};

// vertex buffers for rendering
struct polyVertex {
	float X;
	float Y;
	float Z;

	float U;
	float V;

	byte R;
	byte G;
	byte B;
	byte A;
};

typedef struct rendererPointStruct {
	float X;
	float Y;
	float Z;
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

polyVertex flatVertices[NUM_MAX_FLAT_VERTICES];
polyVertex rampVertices[NUM_MAX_RAMP_VERTICES];
int numUsedFlatVertices = 0;
int numUsedRampVertices = 0;
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

char *renderVar2 = NULL;

int modelFlags = 0;

char primBuffer[30000];

int cameraCenterX;
int cameraCenterY;
int cameraPerspective;
int cameraFovX;
int cameraFovY;

int BBox3D1 = 0;
int BBox3D2 = 0;
int BBox3D3 = 0;
int BBox3D4 = 0;

int renderX;
int renderY;
int renderZ;

const char *bgVSSrc = R"(
attribute vec2 a_position;
attribute vec2 a_texCoords;
varying vec2 v_texCoords;
void main() {
	gl_Position = vec4(a_position.xy, 0.0, 1.0);
	v_texCoords = a_texCoords;
})";

const char *bgPSShader = R"(
varying vec2 v_texCoords;
uniform sampler2D u_background;
uniform sampler2D u_palette;
void main() {
	float cidx = texture(u_background, v_texCoords.xy).r;
	float r = texture(u_palette, vec2(0.0, cidx)).r;
	float g = texture(u_palette, vec2(0.5, cidx)).r;
	float b = texture(u_palette, vec2(1.0, cidx)).r;
	gl_FragColor = vec4(r, g, b, 1.0);
})";

const char *flatVSSrc = R"(
attribute vec3 a_position;
attribute vec2 a_texCoords;
varying vec2 v_texCoords;

void main()
{
    gl_Position = vec4(a_position.x/160.0-1.0, 1.0-a_position.y/100.0, a_position.z/40960.0, 1.0);
    v_texCoords = a_texCoords;
})";

const char *flatPSSrc = R"(
varying vec2 v_texCoords;
uniform sampler2D u_palette;
void main()
{
    float color = v_texCoords.x * 15.0;
    float bank = v_texCoords.y * 15.0;
	float cidx = (bank * 16.0 + color) / 255.0;

    gl_FragColor.r = texture(u_palette, vec2(0.0, cidx)).r;
    gl_FragColor.g = texture(u_palette, vec2(0.5, cidx)).r;
    gl_FragColor.b = texture(u_palette, vec2(1.0, cidx)).r;
    gl_FragColor.a = 1.0;
})";

const char *rampPSSrc = R"(
	varying vec2 v_texCoords;
	uniform sampler2D u_palette;
	void main()
	{
		float color = v_texCoords.x;
		color = mod(color, 2.0);
		if(color > 1.0) {
			color = 1.0 - (color - 1.0);
		}

		color = color * 15.0;
		float bank = v_texCoords.y * 15.0;
		float cidx = (bank * 16.0 + color) / 255.0;

		gl_FragColor.r = texture(u_palette, vec2(0.0, cidx)).r;
		gl_FragColor.g = texture(u_palette, vec2(0.5, cidx)).r;
		gl_FragColor.b = texture(u_palette, vec2(1.0, cidx)).r;
		gl_FragColor.a = 1.0;
	})";

GLuint g_backgroundTexture = 0;
GLuint g_paletteTexture = 0;

byte currentGamePalette[256 * 3];
byte RGB_Pal[256 * 3];
byte frontBuffer[320 * 200];
byte physicalScreen[320 * 200];
byte physicalScreenRGB[320 * 200 * 3];
OpenGL::Shader *backgroundShader = NULL;
OpenGL::Shader *flatShader = NULL;
OpenGL::Shader *rampShader = NULL;
GLuint vbo = 0;
GLuint ebo = 0;

void gfx_init() {
	GL_CALL(glGenBuffers(1, &vbo));
	GL_CALL(glGenBuffers(1, &ebo));

	// create background texture
	GL_CALL(glGenTextures(1, &g_backgroundTexture));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, g_backgroundTexture));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 320, 200, 0, GL_RED, GL_UNSIGNED_BYTE, 0));

	// create palette texture
	GL_CALL(glGenTextures(1, &g_paletteTexture));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, g_paletteTexture));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 3, 256, 0, GL_RED, GL_UNSIGNED_BYTE, 0));

	// create background shader
	backgroundShader = new OpenGL::Shader();
	const char *attributes[] = {"a_position", "a_texCoords", nullptr};
	backgroundShader->loadFromStrings("backgroundShader", bgVSSrc, bgPSShader, attributes, 110);
	backgroundShader->enableVertexAttribute("a_position", vbo, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (uint32)0);
	backgroundShader->enableVertexAttribute("a_texCoords", vbo, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (uint32)(2 * sizeof(float)));
	backgroundShader->use();
	GL_CALL(glUniform1i(backgroundShader->getUniformLocation("u_background"), 0));
	GL_CALL(glUniform1i(backgroundShader->getUniformLocation("u_palette"), 1));

	// create flat shader
	flatShader = new OpenGL::Shader();
	flatShader->loadFromStrings("flatShader", flatVSSrc, flatPSSrc, attributes, 110);
	flatShader->enableVertexAttribute("a_position", vbo, 3, GL_FLOAT, GL_FALSE, sizeof(polyVertex), (uint32)0);
	flatShader->enableVertexAttribute("a_texCoords", vbo, 2, GL_FLOAT, GL_FALSE, sizeof(polyVertex), (uint32)(3 * sizeof(float)));
	flatShader->use();
	GL_CALL(glUniform1i(flatShader->getUniformLocation("u_palette"), 0));

	// create ramp shader
	rampShader = new OpenGL::Shader();
	rampShader->loadFromStrings("rampShader", flatVSSrc, rampPSSrc, attributes, 110);
	rampShader->enableVertexAttribute("a_position", vbo, 3, GL_FLOAT, GL_FALSE, sizeof(polyVertex), (uint32)0);
	rampShader->enableVertexAttribute("a_texCoords", vbo, 2, GL_FLOAT, GL_FALSE, sizeof(polyVertex), (uint32)(3 * sizeof(float)));
	rampShader->use();
	GL_CALL(glUniform1i(rampShader->getUniformLocation("u_palette"), 0));
}

void gfx_deinit() {
	GL_CALL(glDeleteTextures(1, &g_backgroundTexture));
	GL_CALL(glDeleteTextures(1, &g_paletteTexture));
	GL_CALL(glDeleteBuffers(1, &ebo));
	GL_CALL(glDeleteBuffers(1, &vbo));
	delete flatShader;
	delete backgroundShader;
}

void osystem_drawBackground() {
	GL_CALL(glClearColor(0.f, 0.f, 0.f, 1.f));
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	Vertex vertices[] = {
		{{-1.f, -1.f}, {0.f, 1.f}},
		{{1.f, 1.f}, {1.f, 0.f}},
		{{1.f, -1.f}, {1.f, 1.f}},
		{{-1.f, 1.f}, {0.f, 0.f}}};
	uint32 indices[] = {
		0, 1, 2,
		0, 3, 1};
	assert(sizeof(Vertex) == 4 * sizeof(float));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices, GL_STREAM_DRAW));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * 6, indices, GL_STREAM_DRAW));

	backgroundShader->use();

	GL_CALL(glActiveTexture(GL_TEXTURE0));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, g_backgroundTexture));
	GL_CALL(glActiveTexture(GL_TEXTURE1));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, g_paletteTexture));

	GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	GL_CALL(glActiveTexture(GL_TEXTURE0));

	backgroundShader->unbind();
}

void gfx_setPalette(const byte *palette) {
	memcpy(RGB_Pal, palette, 256 * 3);

	GL_CALL(glBindTexture(GL_TEXTURE_2D, g_paletteTexture));
	GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 3, 256, GL_RED, GL_UNSIGNED_BYTE, RGB_Pal));
}

void gfx_copyBlockPhys(byte *videoBuffer, int left, int top, int right, int bottom) {
	unsigned char *out = physicalScreenRGB;
	unsigned char *in = (unsigned char *)&videoBuffer[0] + left + top * 320;

	int i;
	int j;

	while ((right - left) % 4) {
		right++;
	}

	while ((bottom - top) % 4) {
		bottom++;
	}

	for (i = top; i < bottom; i++) {
		in = (unsigned char *)&videoBuffer[0] + left + i * 320;
		unsigned char *out2 = physicalScreen + left + i * 320;
		for (j = left; j < right; j++) {
			unsigned char color = *(in++);

			*(out++) = RGB_Pal[color * 3];
			*(out++) = RGB_Pal[color * 3 + 1];
			*(out++) = RGB_Pal[color * 3 + 2];

			*(out2++) = color;
		}
	}

	GL_CALL(glBindTexture(GL_TEXTURE_2D, g_backgroundTexture));
	GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320, 200, GL_RED, GL_UNSIGNED_BYTE, physicalScreen));
}

void gfx_refreshFrontTextureBuffer() {
	byte *out = physicalScreenRGB;
	byte *in = physicalScreen;

	for (int i = 0; i < 200 * 320; i++) {
		unsigned char color = *(in++);
		*(out++) = RGB_Pal[color * 3];
		*(out++) = RGB_Pal[color * 3 + 1];
		*(out++) = RGB_Pal[color * 3 + 2];
	}

	GL_CALL(glBindTexture(GL_TEXTURE_2D, g_backgroundTexture));
	GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320, 200, GL_RED, GL_UNSIGNED_BYTE, physicalScreen));
}

void osystem_fillPoly(float *buffer, int numPoint, unsigned char color, uint8 polyType) {
#define MAX_POINTS_PER_POLY 50
	// float UVArray[MAX_POINTS_PER_POLY];

	assert(numPoint < MAX_POINTS_PER_POLY);

	// compute the polygon bounding box
	float polyMinX = 320.f;
	float polyMaxX = 0.f;
	float polyMinY = 200.f;
	float polyMaxY = 0.f;

	for (int i = 0; i < numPoint; i++) {
		float X = buffer[3 * i + 0];
		float Y = buffer[3 * i + 1];

		if (X > polyMaxX)
			polyMaxX = X;
		if (X < polyMinX)
			polyMinX = X;

		if (Y > polyMaxY)
			polyMaxY = Y;
		if (Y < polyMinY)
			polyMinY = Y;
	}

	float polyWidth = polyMaxX - polyMinX;
	float polyHeight = polyMaxY - polyMinY;

	if (polyWidth <= 0.f)
		polyWidth = 1;
	if (polyHeight <= 0.f)
		polyHeight = 1;

	switch (polyType) {
	default:
	case 0: // flat (triste)
	{
		polyVertex *pVertex = &flatVertices[numUsedFlatVertices];
		numUsedFlatVertices += (numPoint - 2) * 3;
		assert(numUsedFlatVertices < NUM_MAX_FLAT_VERTICES);

		for (int i = 0; i < numPoint; i++) {
			if (i >= 3) {
				memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
				pVertex++;
				memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
				pVertex++;
			}

			pVertex->X = buffer[i * 3 + 0];
			pVertex->Y = buffer[i * 3 + 1];
			pVertex->Z = buffer[i * 3 + 2];

			int bank = (color & 0xF0) >> 4;
			int startColor = color & 0xF;
			float colorf = startColor;
			pVertex->U = colorf / 15.f;
			pVertex->V = bank / 15.f;
			pVertex++;
		}
		break;
	}
	case 1: // dither (pierre/tele)
	{
		// polyVertex *pVertex = &noiseVertices[numUsedNoiseVertices];
		// numUsedNoiseVertices += (numPoint - 2) * 3;
		// assert(numUsedNoiseVertices < NUM_MAX_NOISE_VERTICES);

		// for (int i = 0; i < numPoint; i++) {
		// 	if (i >= 3) {
		// 		memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
		// 		pVertex++;
		// 		memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
		// 		pVertex++;
		// 	}

		// 	pVertex->X = buffer[i * 3 + 0];
		// 	pVertex->Y = buffer[i * 3 + 1];
		// 	pVertex->Z = buffer[i * 3 + 2];

		// 	pVertex->U = (pVertex->X / 320.f) * 50.f + polyMinX * 1.2f + polyMaxX;
		// 	pVertex->V = (pVertex->Y / 200.f) * 50.f + polyMinY * 0.7f + polyMaxY;

		// 	int bank = (color & 0xF0) >> 4;
		// 	int startColor = color & 0xF;
		// 	float colorf = startColor;
		// 	pVertex->U = colorf / 15.f;
		// 	pVertex->V = bank / 15.f;
		// 	pVertex++;
		// }
		assert(false);
		break;
	}
	case 2: // trans
	{
		assert(false);
		// polyVertex *pVertex = &transparentVertices[numUsedTransparentVertices];
		// numUsedTransparentVertices += (numPoint - 2) * 3;
		// assert(numUsedTransparentVertices < NUM_MAX_TRANSPARENT_VERTICES);

		// for (int i = 0; i < numPoint; i++) {
		// 	if (i >= 3) {
		// 		memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
		// 		pVertex++;
		// 		memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
		// 		pVertex++;
		// 	}

		// 	pVertex->X = buffer[i * 3 + 0];
		// 	pVertex->Y = buffer[i * 3 + 1];
		// 	pVertex->Z = buffer[i * 3 + 2];

		// 	pVertex->R = RGB_Pal[color * 3];
		// 	pVertex->G = RGB_Pal[color * 3 + 1];
		// 	pVertex->B = RGB_Pal[color * 3 + 2];
		// 	pVertex->A = 128;
		// 	pVertex++;
		// }
		break;
	}
	case 4: // copper (ramps top to bottom)
	case 5: // copper2 (ramps top to bottom, 2 scanline per color)
	{
		polyVertex *pVertex = &rampVertices[numUsedRampVertices];
		numUsedRampVertices += (numPoint - 2) * 3;
		assert(numUsedRampVertices < NUM_MAX_RAMP_VERTICES);

		int bank = (color & 0xF0) >> 4;
		int startColor = color & 0xF;
		float colorStep = 1; // TODO: this should be the scanline ratio for the current resolution to original resolution
		if (polyType == 5) {
			colorStep *= 0.5; // to stretch the ramp by 2 for copper2
		}

		for (int i = 0; i < numPoint; i++) {
			if (i >= 3) {
				memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
				pVertex++;
				memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
				pVertex++;
			}

			pVertex->X = buffer[i * 3 + 0];
			pVertex->Y = buffer[i * 3 + 1];
			pVertex->Z = buffer[i * 3 + 2];

			float colorf = startColor + colorStep * (pVertex->Y - polyMinY);

			pVertex->U = colorf / 15.f;
			pVertex->V = bank / 15.f;
			pVertex++;
		}
		break;
	}
	case 3: // marbre (ramp left to right)
	{
		polyVertex *pVertex = &rampVertices[numUsedRampVertices];
		numUsedRampVertices += (numPoint - 2) * 3;
		assert(numUsedRampVertices < NUM_MAX_RAMP_VERTICES);

		float colorStep = 15.f / polyWidth;

		int bank = (color & 0xF0) >> 4;
		int startColor = color & 0xF;

		assert(startColor == 0);

		for (int i = 0; i < numPoint; i++) {
			if (i >= 3) {
				memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
				pVertex++;
				memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
				pVertex++;
			}

			pVertex->X = buffer[i * 3 + 0];
			pVertex->Y = buffer[i * 3 + 1];
			pVertex->Z = buffer[i * 3 + 2];

			float colorf = startColor + colorStep * (pVertex->X - polyMinX);

			pVertex->U = colorf / 15.f;
			pVertex->V = bank / 15.f;
			pVertex++;
		}
		break;
	}
	case 6: // marbre2 (ramp right to left)
	{
		polyVertex *pVertex = &rampVertices[numUsedRampVertices];
		numUsedRampVertices += (numPoint - 2) * 3;
		assert(numUsedRampVertices < NUM_MAX_RAMP_VERTICES);

		float colorStep = 15.f / polyWidth;

		int bank = (color & 0xF0) >> 4;
		int startColor = color & 0xF;

		assert(startColor == 0);

		for (int i = 0; i < numPoint; i++) {
			if (i >= 3) {
				memcpy(pVertex, &pVertex[-3], sizeof(polyVertex));
				pVertex++;
				memcpy(pVertex, &pVertex[-2], sizeof(polyVertex));
				pVertex++;
			}

			pVertex->X = buffer[i * 3 + 0];
			pVertex->Y = buffer[i * 3 + 1];
			pVertex->Z = buffer[i * 3 + 2];

			float colorf = startColor + colorStep * (pVertex->X - polyMinX);

			pVertex->U = 1.f - colorf / 15.f;
			pVertex->V = bank / 15.f;
			pVertex++;
		}
		break;
	}
	}
}

void osystem_flushPendingPrimitives() {

	if (numUsedFlatVertices) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(polyVertex) * numUsedFlatVertices, flatVertices, GL_STREAM_DRAW));

		flatShader->use();

		GL_CALL(glActiveTexture(GL_TEXTURE0));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, g_paletteTexture));

		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, numUsedFlatVertices));
		GL_CALL(glActiveTexture(GL_TEXTURE0));

		flatShader->unbind();

		glDisable(GL_DEPTH_TEST);
		numUsedFlatVertices = 0;
	}

	if (numUsedRampVertices) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(polyVertex) * numUsedRampVertices, rampVertices, GL_STREAM_DRAW));

		rampShader->use();

		GL_CALL(glActiveTexture(GL_TEXTURE0));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, g_paletteTexture));

		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, numUsedRampVertices));
		GL_CALL(glActiveTexture(GL_TEXTURE0));

		rampShader->unbind();

		glDisable(GL_DEPTH_TEST);

		numUsedRampVertices = 0;
	}
}

static void setPosCamera(int x, int y, int z) {
	translateX = x;
	translateY = y;
	translateZ = z;
}

static void setAngleCamera(int x, int y, int z) {
	transformX = x & 0x3FF;
	if (transformX) {
		transformXCos = cosTable[transformX];
		transformXSin = cosTable[(transformX + 0x100) & 0x3FF];
		transformUseX = true;
	} else {
		transformUseX = false;
	}

	transformY = y & 0x3FF;
	if (transformY) {
		transformYCos = cosTable[transformY];
		transformYSin = cosTable[(transformY + 0x100) & 0x3FF];
		transformUseY = true;
	} else {
		transformUseY = false;
	}

	transformZ = z & 0x3FF;
	if (transformZ) {
		transformZCos = cosTable[transformZ];
		transformZSin = cosTable[(transformZ + 0x100) & 0x3FF];
		transformUseZ = true;
	} else {
		transformUseZ = false;
	}
}

static void rotate(unsigned int x, unsigned int y, unsigned int z, int *xOut, int *yOut) {
	if (x) {
		int var1 = (((cosTable[(x + 0x100) & 0x3FF] * y) << 1) & 0xFFFF0000) - (((cosTable[x & 0x3FF] * z) << 1) & 0xFFFF0000);
		int var2 = (((cosTable[x & 0x3FF] * y) << 1) & 0xFFFF0000) + (((cosTable[(x + 0x100) & 0x3FF] * z) << 1) & 0xFFFF0000);

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

static void transformPoint(float *ax, float *bx, float *cx) {
	int X = (int)*ax;
	int Y = (int)*bx;
	int Z = (int)*cx;
	{
		int *ax = &X;
		int *bx = &Y;
		int *cx = &Z;

		{
			int x;
			int y;
			int z;

			if (transformUseY) {
				x = (((((*ax) * transformYSin) - ((*cx) * transformYCos))) / 0x10000) << 1;
				z = (((((*ax) * transformYCos) + ((*cx) * transformYSin))) / 0x10000) << 1;
			} else {
				x = (*ax);
				z = (*cx);
			}

			// si = x
			// ax = z

			if (transformUseX) {
				int tempY = (*bx);
				int tempZ = z;
				y = ((((tempY * transformXSin) - (tempZ * transformXCos))) / 0x10000) << 1;
				z = ((((tempY * transformXCos) + (tempZ * transformXSin))) / 0x10000) << 1;
			} else {
				y = (*bx);
			}

			// cx = y
			// bx = z

			if (transformUseZ) {
				int tempX = x;
				int tempY = y;
				x = ((((tempX * transformZSin) - (tempY * transformZCos))) / 0x10000) << 1;
				y = ((((tempX * transformZCos) + (tempY * transformZSin))) / 0x10000) << 1;
			}

			*ax = x;
			*bx = y;
			*cx = z;
		}
	}

	*ax = (float)X;
	*bx = (float)Y;
	*cx = (float)Z;
}

static int animNuage(int x, int y, int z, int alpha, int beta, int gamma, sBody *pBody) {
	// TODO:
	assert(false);
}

static int rotateNuage(int x, int y, int z, int alpha, int beta, int gamma, sBody *pBody) {
	float *outPtr;

	renderX = x - translateX;
	renderY = y;
	renderZ = z - translateZ;

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

	outPtr = renderPointList;

	for (int i = 0; i < pBody->m_vertices.size(); i++) {
		float X = pBody->m_vertices[i].x;
		float Y = pBody->m_vertices[i].y;
		float Z = pBody->m_vertices[i].z;

		if (!noModelRotation) {
			// Y rotation
			{
				float tempX = X;
				float tempZ = Z;

				X = (((modelSinBeta * tempX) - (modelCosBeta * tempZ)) / 65536.f) * 2.f;
				Z = (((modelCosBeta * tempX) + (modelSinBeta * tempZ)) / 65536.f) * 2.f;
			}

			// Z rotation
			{
				float tempX = X;
				float tempY = Y;

				X = (((modelSinGamma * tempX) - (modelCosGamma * tempY)) / 65536.f) * 2.f;
				Y = (((modelCosGamma * tempX) + (modelSinGamma * tempY)) / 65536.f) * 2.f;
			}

			// X rotation
			{
				float tempY = Y;
				float tempZ = Z;

				Y = (((modelSinAlpha * tempY) - (modelCosAlpha * tempZ)) / 65536.f) * 2.f;
				Z = (((modelCosAlpha * tempY) + (modelSinAlpha * tempZ)) / 65536.f) * 2.f;
			}
		}

		X += renderX;
		Y += renderY;
		Z += renderZ;

#if defined(AITD_UE4)
		*(outPtr++) = X;
		*(outPtr++) = Y;
		*(outPtr++) = Z;
#else
		if (Y > 10000) // height clamp
		{
			*(outPtr++) = -10000;
			*(outPtr++) = -10000;
			*(outPtr++) = -10000;
		} else {
			float transformedX;
			float transformedY;

			Y -= translateY;

			transformPoint(&X, &Y, &Z);

			Z += cameraPerspective;

			transformedX = ((X * cameraFovX) / Z) + cameraCenterX;

			*(outPtr++) = transformedX;

			if (transformedX < BBox3D1)
				BBox3D1 = (int)transformedX;

			if (transformedX > BBox3D3)
				BBox3D3 = (int)transformedX;

			transformedY = ((Y * cameraFovY) / Z) + cameraCenterY;

			*(outPtr++) = transformedY;

			if (transformedY < BBox3D2)
				BBox3D2 = (int)transformedY;

			if (transformedY > BBox3D4)
				BBox3D4 = (int)transformedY;

			*(outPtr++) = Z;

			/*  *(outPtr++) = X;
			 *(outPtr++) = Y;
			 *(outPtr++) = Z; */
		}
#endif
	}
	return (1);
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
		uint16 pointIndex;

		pointIndex = ptr->m_points[i] * 6;

		assert((pointIndex % 2) == 0);

		pCurrentPrimEntry->vertices[i].X = renderPointList[pointIndex / 2];
		pCurrentPrimEntry->vertices[i].Y = renderPointList[(pointIndex / 2) + 1];
		pCurrentPrimEntry->vertices[i].Z = renderPointList[(pointIndex / 2) + 2];

		if (pCurrentPrimEntry->vertices[i].Z < depth)
			depth = pCurrentPrimEntry->vertices[i].Z;
	}

#if !defined(AITD_UE4)
	if (depth > 100)
#endif
	{
		positionInPrimEntry++;

		numOfPrimitiveToRender++;
		assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);
	}
}

static void processPrim_Point(primTypeEnum primType, sPrimitive *ptr, char **out) {
	// TODO:
	assert(false);
}

static void processPrim_Sphere(int primType, sPrimitive *ptr, char **out) {
	// TODO:
	assert(false);
}

typedef void (*renderFunction)(primEntryStruct *buffer);

void renderLine(primEntryStruct *pEntry) // line
{
	// TODO:
	assert(false);
}

void renderPoly(primEntryStruct *pEntry) // poly
{
	osystem_fillPoly((float *)pEntry->vertices, pEntry->numOfVertices, pEntry->color, pEntry->material);
}

void renderZixel(primEntryStruct *pEntry) // point
{
	// TODO:
	assert(false);
}

void renderPoint(primEntryStruct *pEntry) // point
{
	// TODO:
	assert(false);
}

void renderBigPoint(primEntryStruct *pEntry) // point
{
	// TODO:
	assert(false);
}

void renderSphere(primEntryStruct *pEntry) // sphere
{
	// TODO:
	assert(false);
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

int affObjet(int x, int y, int z, int alpha, int beta, int gamma, void *modelPtr) {
	sBody *pBody = getBodyFromPtr(modelPtr);
	char *ptr = (char *)modelPtr;
	int numPrim;
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
			return (2);
		}
		return 0;
	} else if (!(modelFlags & INFO_TORTUE)) {
		if (!rotateNuage(x, y, z, alpha, beta, gamma, pBody)) {
			BBox3D3 = -32000;
			BBox3D4 = -32000;
			BBox3D1 = 32000;
			BBox3D2 = 32000;
			return (2);
		}
	} else {
		debug("unsupported model type prerenderFlag4 in renderer !\n");

		BBox3D3 = -32000;
		BBox3D4 = -32000;
		BBox3D1 = 32000;
		BBox3D2 = 32000;
		return (2);
	}

	numPrim = pBody->m_primitives.size();

	if (!numPrim) {
		BBox3D3 = -32000;
		BBox3D4 = -32000;
		BBox3D1 = 32000;
		BBox3D2 = 32000;
		return (2);
	}

	out = primBuffer;

	// create the list of all primitives to render
	for (i = 0; i < numPrim; i++) {
		sPrimitive *pPrimitive = &pBody->m_primitives[i];
		primTypeEnum primType = pPrimitive->m_type;

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
			assert(0);
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
			 *(s16*)(renderBuffer+10*bestIdx) = -32000;
			 outBuffer+=10;
		 }
		 source = sortedBuffer;

#endif
#endif

	//

	if (!numOfPrimitiveToRender) {
		BBox3D3 = -32000;
		BBox3D4 = -32000;
		BBox3D1 = 32000;
		BBox3D2 = 32000;
		return (1); // model ok, but out of screen
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
	return (0);
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
	int width = x2 - x1 + 1;
	int height = y2 - y1 + 1;

	char *dest = logicalScreen + y1 * 320 + x1;

	int i;
	int j;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			*(dest++) = color;
		}

		dest += 320 - width;
	}
}

void flushScreen(void) {
	for (int i = 0; i < 200; i++) {
		for (int j = 0; j < 320; j++) {
			*(logicalScreen + i * 320 + j) = 0;
		}
	}
}

void setupCamera() {
	assert(0);
	// 	int x;
	// 	int y;
	// 	int z;
	// 	cameraDataStruct* pCamera;

	// 	freezeTime();

	// 	currentCamera = startGameVar1;

	// 	assert(startGameVar1 < roomDataTable[currentRoom].numCameraInRoom);

	// 	loadCamera(roomDataTable[currentRoom].cameraIdxTable[startGameVar1]);
	// 	if(g_gameId >= JACK)
	// 	{
	// 		loadMask(roomDataTable[currentRoom].cameraIdxTable[startGameVar1]);
	// 	}
	// 	else
	// 	{
	// 		createAITD1Mask();
	// 	}
	// 	cameraBackgroundChanged = true;

	// 	pCamera = cameraDataTable[currentCamera];

	// 	SetAngleCamera(pCamera->alpha,pCamera->beta,pCamera->gamma);

	// #ifdef FITD_DEBUGGER
	// 	if(debuggerVar_topCamera)
	// 		SetAngleCamera(0x100,0,0);
	// #endif

	// 	x = (pCamera->x - roomDataTable[currentRoom].worldX)*10;
	// 	y = (roomDataTable[currentRoom].worldY - pCamera->y)*10;
	// 	z = (roomDataTable[currentRoom].worldZ - pCamera->z)*10;

	// #ifdef FITD_DEBUGGER
	// 	if(debuggerVar_topCamera)
	// 	{
	// 		if(currentCameraTargetActor != -1)
	// 		{
	// 			x = objectTable[currentCameraTargetActor].worldX + objectTable[currentCameraTargetActor].stepX;
	// 			y = debufferVar_topCameraZoom;
	// 			z = objectTable[currentCameraTargetActor].worldZ + objectTable[currentCameraTargetActor].stepZ;
	// 		}
	// 	}
	// #endif
	// 	SetPosCamera(x,y,z); // setup camera position

	// 	setupCameraProjection(160,100,pCamera->focal1,pCamera->focal2,pCamera->focal3); // setup focale

	// #ifdef FITD_DEBUGGER
	// 	if(debuggerVar_topCamera)
	// 		setupCameraProjection(160,100,1000,100,100); // setup focale
	// #endif

	// 	setupCameraSub1();
	// 	updateAllActorAndObjects();
	// 	createActorList();
	// 	//  setupCameraSub3();
	// 	setupCameraSub4();
	// 	/*  setupCameraSub5();
	// 	*/
	// 	if(flagInitView==2)
	// 	{
	// 		flagRedraw = 2;
	// 	}
	// 	else
	// 	{
	// 		if(flagRedraw!=2)
	// 		{
	// 			flagRedraw = 1;
	// 		}
	// 	}

	// 	flagInitView = 0;
	// 	unfreezeTime();
}

} // namespace Fitd
