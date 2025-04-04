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

#include "fitd/aitd1.h"
#include "fitd/costable.h"
#include "fitd/common.h"
#include "fitd/game_time.h"
#include "fitd/gfx.h"
#include "fitd/hqr.h"
#include "fitd/pak.h"
#include "fitd/room.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"
#include "common/debug.h"
#include "common/scummsys.h"
#include "common/util.h"
#include "graphics/surface.h"
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

struct sphereVertex {
	float X;
	float Y;
	float Z;

	float U;
	float V;

	float centerX;
	float centerY;
	float size;
	float material;
};

primEntryStruct primTable[NUM_MAX_PRIM_ENTRY];

polyVertex noiseVertices[NUM_MAX_NOISE_VERTICES];
polyVertex flatVertices[NUM_MAX_FLAT_VERTICES];
polyVertex transparentVertices[NUM_MAX_TRANSPARENT_VERTICES];
polyVertex rampVertices[NUM_MAX_RAMP_VERTICES];
sphereVertex sphereVertices[NUM_MAX_SPHERES_VERTICES];

int numUsedFlatVertices = 0;
int numUsedNoiseVertices = 0;
int numUsedTransparentVertices = 0;
int numUsedRampVertices = 0;
int numUsedSpheres = 0;

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

int BBox3D1 = 0;
int BBox3D2 = 0;
int BBox3D3 = 0;
int BBox3D4 = 0;

int renderX;
int renderY;
int renderZ;

struct maskStruct {
	GLuint maskTexture = 0;
	GLuint vertexBuffer = 0;
	int maskX1;
	int maskY1;
	int maskX2;
	int maskY2;
};

Common::Array<Common::Array<maskStruct> > maskTextures; // [room][mask]

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

// TODO: fix this, today it's the same as flat PS
const char *noisePSSrc = R"(
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

const char *maskVSSrc = R"(
attribute vec2 a_position;
attribute vec2 a_texCoords;
varying vec2 v_texCoords;
void main() {
	gl_Position = vec4(a_position.x/160.0-1.0, 1.0-a_position.y/100.0, 0.0, 1.0);
	v_texCoords = a_texCoords;
})";

const char *maskPSSrc = R"(
varying vec2 v_texCoords;
uniform sampler2D u_maskPalette;
uniform sampler2D u_palette;
uniform sampler2D u_background;
void main()
{
	vec4 rawMask = texture(u_maskPalette, v_texCoords);
	if(rawMask.r == 0.0)
		discard;

	float cidx = texture(u_background, v_texCoords.xy).r;
	gl_FragColor.r = texture(u_palette, vec2(0.0, cidx)).r;
	gl_FragColor.g = texture(u_palette, vec2(0.5, cidx)).r;
	gl_FragColor.b = texture(u_palette, vec2(1.0, cidx)).r;
	gl_FragColor.a = 1.0;
})";

const char *sphereVSSrc = R"(
attribute vec3 a_position;
attribute vec2 a_texCoords;
attribute vec4 a_texCoords2;
varying vec2 v_texCoords;
varying vec4 v_sphereParams;
varying vec3 v_screenSpacePosition;

void main()
{
    gl_Position = vec4(a_position.x/160.0-1.0, 1.0-a_position.y/100.0, a_position.z/40960.0, 1.0);
    v_texCoords = a_texCoords;
    v_sphereParams = a_texCoords2;
    v_screenSpacePosition = a_position.xyz;
})";

const char *spherePSSrc = R"(
varying vec2 v_texCoords;
varying vec4 v_sphereParams;
varying vec3 v_screenSpacePosition;
uniform sampler2D u_palette;

void main()
{
    vec2 sphereCenter = v_sphereParams.xy;
    float sphereSize = v_sphereParams.z;
    float fDistanceToCenter = length(v_screenSpacePosition.xy - sphereCenter);
    if(fDistanceToCenter > sphereSize)
        discard;

    float color = v_texCoords.x * 15.0;
    float bank = v_texCoords.y * 15.0;
    int material = int(v_sphereParams.w);

    if(material == 0) { // flat
        float cidx = (bank * 16.0 + color) / 255.0;
        gl_FragColor.r = texture(u_palette, vec2(0.0, cidx)).r;
        gl_FragColor.g = texture(u_palette, vec2(0.5, cidx)).r;
        gl_FragColor.b = texture(u_palette, vec2(1.0, cidx)).r;
        gl_FragColor.a = 1.0;
    } else if(material == 2) { // transparent
        float cidx = (bank * 16.0 + color) / 255.0;
        gl_FragColor.r = texture(u_palette, vec2(0.0, cidx)).r;
        gl_FragColor.g = texture(u_palette, vec2(0.5, cidx)).r;
        gl_FragColor.b = texture(u_palette, vec2(1.0, cidx)).r;
        gl_FragColor.a = 0.5;
    } else if(material == 3) { // marbre
        vec2 normalizedPosition = (v_screenSpacePosition.xy - sphereCenter.xy) / sphereSize;
        float angle = asin(normalizedPosition.y);
        float distanceToCircleOnScanline = cos(angle) - normalizedPosition.x; // value is in 0/2 range
        distanceToCircleOnScanline /= 2.0; // remap to 0 / 1
        color = (1.0 - distanceToCircleOnScanline) * 15.0;

        float cidx = (bank * 16.0 + color) / 255.0;
        gl_FragColor.r = texture(u_palette, vec2(0.0, cidx)).r;
        gl_FragColor.g = texture(u_palette, vec2(0.5, cidx)).r;
        gl_FragColor.b = texture(u_palette, vec2(1.0, cidx)).r;
        gl_FragColor.a = 1.0;
    } else {
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
})";

GLuint g_backgroundTexture = 0;
GLuint g_paletteTexture = 0;

byte RGB_Pal[256 * 3];
byte frontBuffer[320 * 200];
byte physicalScreen[320 * 200];
byte physicalScreenRGB[320 * 200 * 3];
OpenGL::Shader *backgroundShader = NULL;
OpenGL::Shader *flatShader = NULL;
OpenGL::Shader *noiseShader = NULL;
OpenGL::Shader *rampShader = NULL;
OpenGL::Shader *maskShader = NULL;
OpenGL::Shader *sphereShader = NULL;
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

	// create noise shader
	noiseShader = new OpenGL::Shader();
	noiseShader->loadFromStrings("flatShader", flatVSSrc, noisePSSrc, attributes, 110);
	noiseShader->enableVertexAttribute("a_position", vbo, 3, GL_FLOAT, GL_FALSE, sizeof(polyVertex), (uint32)0);
	noiseShader->enableVertexAttribute("a_texCoords", vbo, 2, GL_FLOAT, GL_FALSE, sizeof(polyVertex), (uint32)(3 * sizeof(float)));
	noiseShader->use();
	GL_CALL(glUniform1i(noiseShader->getUniformLocation("u_palette"), 0));

	// create ramp shader
	rampShader = new OpenGL::Shader();
	rampShader->loadFromStrings("rampShader", flatVSSrc, rampPSSrc, attributes, 110);
	rampShader->enableVertexAttribute("a_position", vbo, 3, GL_FLOAT, GL_FALSE, sizeof(polyVertex), (uint32)0);
	rampShader->enableVertexAttribute("a_texCoords", vbo, 2, GL_FLOAT, GL_FALSE, sizeof(polyVertex), (uint32)(3 * sizeof(float)));
	rampShader->use();
	GL_CALL(glUniform1i(rampShader->getUniformLocation("u_palette"), 0));

	// create mask shader
	maskShader = new OpenGL::Shader();
	maskShader->loadFromStrings("maskShader", maskVSSrc, maskPSSrc, attributes, 110);
	maskShader->use();
	GL_CALL(glUniform1i(maskShader->getUniformLocation("u_maskPalette"), 0));
	GL_CALL(glUniform1i(maskShader->getUniformLocation("u_palette"), 1));
	GL_CALL(glUniform1i(maskShader->getUniformLocation("u_background"), 2));

	// create sphere shader
	const char *sphere_attributes[] = {"a_position", "a_texCoords", "a_texCoords2", nullptr};
	sphereShader = new OpenGL::Shader();
	sphereShader->loadFromStrings("sphereShader", sphereVSSrc, spherePSSrc, sphere_attributes, 110);
	sphereShader->enableVertexAttribute("a_position", vbo, 3, GL_FLOAT, GL_FALSE, sizeof(sphereVertex), (uint32)0);
	sphereShader->enableVertexAttribute("a_texCoords", vbo, 2, GL_FLOAT, GL_FALSE, sizeof(sphereVertex), (uint32)(3 * sizeof(float)));
	sphereShader->enableVertexAttribute("a_texCoords2", vbo, 4, GL_FLOAT, GL_FALSE, sizeof(sphereVertex), (uint32)(5 * sizeof(float)));
	sphereShader->use();
	GL_CALL(glUniform1i(sphereShader->getUniformLocation("u_palette"), 0));
}

void gfx_deinit() {
	GL_CALL(glDeleteTextures(1, &g_backgroundTexture));
	GL_CALL(glDeleteTextures(1, &g_paletteTexture));
	GL_CALL(glDeleteBuffers(1, &ebo));
	GL_CALL(glDeleteBuffers(1, &vbo));
	delete backgroundShader;
	delete flatShader;
	delete noiseShader;
	delete rampShader;
	delete maskShader;
	delete sphereShader;
}

void osystem_startFrame() {
	GL_CALL(glClearColor(0.f, 0.f, 0.f, 1.f));
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	osystem_drawBackground();
}

void osystem_drawBackground() {
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
		polyVertex *pVertex = &noiseVertices[numUsedNoiseVertices];
		numUsedNoiseVertices += (numPoint - 2) * 3;
		assert(numUsedNoiseVertices < NUM_MAX_NOISE_VERTICES);

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
	case 2: // trans
	{
		polyVertex *pVertex = &transparentVertices[numUsedTransparentVertices];
		numUsedTransparentVertices += (numPoint - 2) * 3;
		assert(numUsedTransparentVertices < NUM_MAX_TRANSPARENT_VERTICES);

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

			pVertex->R = RGB_Pal[color * 3];
			pVertex->G = RGB_Pal[color * 3 + 1];
			pVertex->B = RGB_Pal[color * 3 + 2];
			pVertex->A = 128;
			pVertex++;
		}
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

	if (numUsedNoiseVertices) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(polyVertex) * numUsedNoiseVertices, noiseVertices, GL_STREAM_DRAW));

		noiseShader->use();

		GL_CALL(glActiveTexture(GL_TEXTURE0));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, g_paletteTexture));

		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, numUsedNoiseVertices));
		GL_CALL(glActiveTexture(GL_TEXTURE0));

		noiseShader->unbind();

		glDisable(GL_DEPTH_TEST);
		numUsedNoiseVertices = 0;
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

	if (numUsedSpheres) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertex) * numUsedSpheres, sphereVertices, GL_STREAM_DRAW));

		sphereShader->use();

		GL_CALL(glActiveTexture(GL_TEXTURE0));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, g_paletteTexture));

		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, numUsedSpheres));
		GL_CALL(glActiveTexture(GL_TEXTURE0));

		sphereShader->unbind();

		glDisable(GL_DEPTH_TEST);

		numUsedSpheres = 0;
	}

	numUsedTransparentVertices = 0;
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

void rotate(unsigned int x, unsigned int y, unsigned int z, int *xOut, int *yOut) {
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

static void TranslateGroupe(int transX, int transY, int transZ, const sGroup *ptr) {
	int16 *ptrSource = &pointBuffer[ptr->m_start * 3];

	for (int i = 0; i < ptr->m_numVertices; i++) {
		*(ptrSource++) += transX;
		*(ptrSource++) += transY;
		*(ptrSource++) += transZ;
	}
}

static void ZoomGroupe(int zoomX, int zoomY, int zoomZ, const sGroup *ptr) {
	int16 *ptrSource = &pointBuffer[ptr->m_start * 3];

	for (int i = 0; i < ptr->m_numVertices; i++) {
		*(ptrSource++) = (*(ptrSource) * (zoomX + 256)) / 256;
		*(ptrSource++) = (*(ptrSource) * (zoomY + 256)) / 256;
		*(ptrSource++) = (*(ptrSource) * (zoomZ + 256)) / 256;
	}
}

static void InitGroupeRot(int transX, int transY, int transZ) {
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

static void RotateList(int16 *pointPtr, int numOfPoint) {
	for (int i = 0; i < numOfPoint; i++) {
		int x = *(int16 *)pointPtr;
		int y = *(int16 *)(pointPtr + 1);
		int z = *(int16 *)(pointPtr + 2);

		if (boneRotateY) {
			int tempX = x;
			int tempZ = z;

			x = ((((tempX * boneRotateYSin) - (tempZ * boneRotateYCos))) >> 16) << 1;
			z = ((((tempX * boneRotateYCos) + (tempZ * boneRotateYSin))) >> 16) << 1;
		}

		if (boneRotateX) {
			int tempY = y;
			int tempZ = z;
			y = ((((tempY * boneRotateXSin) - (tempZ * boneRotateXCos))) >> 16) << 1;
			z = ((((tempY * boneRotateXCos) + (tempZ * boneRotateXSin))) >> 16) << 1;
		}

		if (boneRotateZ) {
			int tempX = x;
			int tempY = y;
			x = ((((tempX * boneRotateZSin) - (tempY * boneRotateZCos))) >> 16) << 1;
			y = ((((tempX * boneRotateZCos) + (tempY * boneRotateZSin))) >> 16) << 1;
		}

		*(int16 *)(pointPtr) = x;
		*(int16 *)(pointPtr + 1) = y;
		*(int16 *)(pointPtr + 2) = z;

		pointPtr += 3;
	}
}

static void RotateGroupeOptimise(const sGroup *ptr) {
	if (ptr->m_numGroup) // if group number is 0
	{
		int baseBone = ptr->m_start;
		int numPoints = ptr->m_numVertices;

		RotateList(pointBuffer + baseBone * 3, numPoints);
	}
}

static void RotateGroupe(sGroup *ptr) {
	int baseBone = ptr->m_start;
	int numPoints = ptr->m_numVertices;
	int temp;
	int temp2;

	RotateList(pointBuffer + baseBone * 3, numPoints);

	temp = ptr->m_numGroup; // group number

	temp2 = numOfBones - temp;

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

	for (int i = 0; i < pBody->m_vertices.size(); i++) {
		pointBuffer[i * 3 + 0] = pBody->m_vertices[i].x;
		pointBuffer[i * 3 + 1] = pBody->m_vertices[i].y;
		pointBuffer[i * 3 + 2] = pBody->m_vertices[i].z;
	}

	numOfPoints = pBody->m_vertices.size();
	numOfBones = pBody->m_groupOrder.size();
	assert(numOfBones < NUM_MAX_BONES);

	if (pBody->m_flags & INFO_OPTIMISE) {
		for (int i = 0; i < pBody->m_groupOrder.size(); i++) {
			int boneDataOffset = pBody->m_groupOrder[i];
			sGroup *pGroup = &pBody->m_groups[pBody->m_groupOrder[i]];

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

		for (int i = 0; i < pBody->m_groups.size(); i++) {
			int boneDataOffset = pBody->m_groupOrder[i];
			sGroup *pGroup = &pBody->m_groups[pBody->m_groupOrder[i]];

			int transX = pGroup->m_state.m_delta[0];
			int transY = pGroup->m_state.m_delta[1];
			int transZ = pGroup->m_state.m_delta[2];

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

	for (int i = 0; i < pBody->m_groups.size(); i++) {
		const sGroup *pGroup = &pBody->m_groups[i];

		int j;

		int point1;
		int point2;

		const int16 *ptr1;
		int16 *ptr2;

		int number;

		int ax;
		int bx;
		int dx;

		point1 = pGroup->m_baseVertices * 6;
		point2 = pGroup->m_start * 6;

		assert(point1 % 2 == 0);
		assert(point2 % 2 == 0);

		point1 /= 2;
		point2 /= 2;

		assert(point1 / 3 < NUM_MAX_POINT_IN_POINT_BUFFER);
		assert(point2 / 3 < NUM_MAX_POINT_IN_POINT_BUFFER);

		ptr1 = (int16 *)&pointBuffer[point1];
		ptr2 = (int16 *)&pointBuffer[point2];

		number = pGroup->m_numVertices;

		ax = ptr1[0];
		bx = ptr1[1];
		dx = ptr1[2];

		for (j = 0; j < number; j++) {
			*(ptr2++) += ax;
			*(ptr2++) += bx;
			*(ptr2++) += dx;
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

		float *outPtr2;

		for (int i = 0; i < numOfPoints; i++) {
			float X = *(int16 *)ptr;
			float Y = *(int16 *)(ptr + 2);
			float Z = *(int16 *)(ptr + 4);
			ptr += 6;

			X += renderX;
			Y += renderY;
			Z += renderZ;

#if defined(AITD_UE4)
			*(outPtr++) = (int16)X;
			*(outPtr++) = (int16)Y;
			*(outPtr++) = (int16)Z;
#else
			if (Y > 10000) // height clamp
			{
				*(outPtr++) = -10000;
				*(outPtr++) = -10000;
				*(outPtr++) = -10000;
			} else {
				Y -= translateY;

				transformPoint(&X, &Y, &Z);

				*(outPtr++) = (int16)X;
				*(outPtr++) = (int16)Y;
				*(outPtr++) = (int16)Z;
			}
#endif
		}

		ptr = (char *)cameraSpaceBuffer;
		outPtr2 = renderPointList;

		do {
			float X;
			float Y;
			float Z;

			X = *(int16 *)ptr;
			ptr += 2;
			Y = *(int16 *)ptr;
			ptr += 2;
			Z = *(int16 *)ptr;
			ptr += 2;

#if defined(AITD_UE4)
			*(outPtr2++) = X;
			*(outPtr2++) = Y;
			*(outPtr2++) = Z;
#else
			Z += cameraPerspective;

			if (Z <= 50) // clipping
			{
				*(outPtr2++) = -10000;
				*(outPtr2++) = -10000;
				*(outPtr2++) = -10000;
			} else {
				float transformedX = ((X * cameraFovX) / Z) + cameraCenterX;
				float transformedY;

				*(outPtr2++) = transformedX;

				if (transformedX < BBox3D1)
					BBox3D1 = (int)transformedX;

				if (transformedX > BBox3D3)
					BBox3D3 = (int)transformedX;

				transformedY = ((Y * cameraFovY) / Z) + cameraCenterY;

				*(outPtr2++) = transformedY;

				if (transformedY < BBox3D2)
					BBox3D2 = (int)transformedY;

				if (transformedY > BBox3D4)
					BBox3D4 = (int)transformedY;

				*(outPtr2++) = Z;
			}
#endif

			k--;
			if (k == 0) {
				return (1);
			}

		} while (renderVar1 == 0);
	}

	return (0);
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
	primEntryStruct *pCurrentPrimEntry = &primTable[positionInPrimEntry];

	assert(positionInPrimEntry < NUM_MAX_PRIM_ENTRY);

	pCurrentPrimEntry->type = primType;
	pCurrentPrimEntry->numOfVertices = 1;
	pCurrentPrimEntry->color = ptr->m_color;
	pCurrentPrimEntry->material = ptr->m_material;

	float depth = 32000.f;
	{
		uint16 pointIndex;
		pointIndex = ptr->m_points[0] * 6;
		assert((pointIndex % 2) == 0);
		pCurrentPrimEntry->vertices[0].X = renderPointList[pointIndex / 2];
		pCurrentPrimEntry->vertices[0].Y = renderPointList[(pointIndex / 2) + 1];
		pCurrentPrimEntry->vertices[0].Z = renderPointList[(pointIndex / 2) + 2];

		depth = pCurrentPrimEntry->vertices[0].Z;
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
		uint16 pointIndex;
		pointIndex = ptr->m_points[0] * 6;
		assert((pointIndex % 2) == 0);
		pCurrentPrimEntry->vertices[0].X = renderPointList[pointIndex / 2];
		pCurrentPrimEntry->vertices[0].Y = renderPointList[(pointIndex / 2) + 1];
		pCurrentPrimEntry->vertices[0].Z = renderPointList[(pointIndex / 2) + 2];

		depth = pCurrentPrimEntry->vertices[0].Z;
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
	float pointSize = 20.f;
	float transformedSize = ((pointSize * (float)cameraFovX) / (float)(pEntry->vertices[0].Z + cameraPerspective));

	osystem_drawPoint(pEntry->vertices[0].X, pEntry->vertices[0].Y, pEntry->vertices[0].Z, pEntry->color, pEntry->material, transformedSize);
}

void renderPoint(primEntryStruct *pEntry) // point
{
	float pointSize = 0.3f; // TODO: better way to compute that?
	osystem_drawPoint(pEntry->vertices[0].X, pEntry->vertices[0].Y, pEntry->vertices[0].Z, pEntry->color, pEntry->material, pointSize);
}

void renderBigPoint(primEntryStruct *pEntry) // point
{
	float bigPointSize = 2.f; // TODO: better way to compute that?
	osystem_drawPoint(pEntry->vertices[0].X, pEntry->vertices[0].Y, pEntry->vertices[0].Z, pEntry->color, pEntry->material, bigPointSize);
}

void renderSphere(primEntryStruct *pEntry) // sphere
{
	float transformedSize;

	transformedSize = (((float)pEntry->size * (float)cameraFovX) / (float)(pEntry->vertices[0].Z + cameraPerspective));

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
			 *(int16*)(renderBuffer+10*bestIdx) = -32000;
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

void osystem_createMask(const uint8 *mask, int roomId, int maskId, unsigned char *refImage, int maskX1, int maskY1, int maskX2, int maskY2) {
	if (maskTextures.size() < roomId + 1) {
		maskTextures.resize(roomId + 1);
	}
	if (maskTextures[roomId].size() < maskId + 1) {
		maskTextures[roomId].resize(maskId + 1);
	}

	if (maskTextures[roomId][maskId].maskTexture) {
		glDeleteTextures(1, &maskTextures[roomId][maskId].maskTexture);
		maskTextures[roomId][maskId].maskTexture = 0;
	}

	if (maskTextures[roomId][maskId].vertexBuffer) {
		glDeleteBuffers(1, &maskTextures[roomId][maskId].vertexBuffer);
		maskTextures[roomId][maskId].vertexBuffer = 0;
	}

	GL_CALL(glGenTextures(1, &maskTextures[roomId][maskId].maskTexture));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, maskTextures[roomId][maskId].maskTexture));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 320, 200, 0, GL_RED, GL_UNSIGNED_BYTE, mask));

	maskTextures[roomId][maskId].maskX1 = maskX1;
	maskTextures[roomId][maskId].maskX2 = maskX2 + 1;
	maskTextures[roomId][maskId].maskY1 = maskY1;
	maskTextures[roomId][maskId].maskY2 = maskY2 + 1;

	struct sVertice {
		float position[3];
		float texcoord[2];
	} vertexBuffer[4];

	float X1 = maskTextures[roomId][maskId].maskX1;
	float X2 = maskTextures[roomId][maskId].maskX2;
	float Y1 = maskTextures[roomId][maskId].maskY1;
	float Y2 = maskTextures[roomId][maskId].maskY2;

	float maskZ = 0.f;

	sVertice *pVertices = vertexBuffer;
	pVertices->position[0] = X1;
	pVertices->position[1] = Y2;
	pVertices->position[2] = maskZ;
	pVertices->texcoord[0] = X1 / 320.f;
	pVertices->texcoord[1] = Y2 / 200.f;
	pVertices++;
	pVertices->position[0] = X1;
	pVertices->position[1] = Y1;
	pVertices->position[2] = maskZ;
	pVertices->texcoord[0] = X1 / 320.f;
	pVertices->texcoord[1] = Y1 / 200.f;
	pVertices++;
	pVertices->position[0] = X2;
	pVertices->position[1] = Y2;
	pVertices->position[2] = maskZ;
	pVertices->texcoord[0] = X2 / 320.f;
	pVertices->texcoord[1] = Y2 / 200.f;
	pVertices++;
	pVertices->position[0] = X2;
	pVertices->position[1] = Y1;
	pVertices->position[2] = maskZ;
	pVertices->texcoord[0] = X2 / 320.f;
	pVertices->texcoord[1] = Y1 / 200.f;

	GL_CALL(glGenBuffers(1, &maskTextures[roomId][maskId].vertexBuffer));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, maskTextures[roomId][maskId].vertexBuffer));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(sVertice) * 4, vertexBuffer, GL_STREAM_DRAW));
}

void osystem_stopModelRender() {
	osystem_flushPendingPrimitives();
}

void gameScreenToViewport(float *X, float *Y) {
	(*X) = (*X) * 4.f;
	(*Y) = (*Y) * 4.f;

	(*Y) = 800.f - (*Y);
}

void osystem_setClip(float left, float top, float right, float bottom) {
	float x1 = left - 1;
	float y1 = bottom + 1;
	float x2 = right + 1;
	float y2 = top - 1;

	gameScreenToViewport(&x1, &y1);
	gameScreenToViewport(&x2, &y2);

	float width = x2 - x1;
	float height = y2 - y1;

	float currentScissor[4];
	currentScissor[0] = ((left - 1) / 320.f) * 320.f;
	currentScissor[1] = ((top - 1) / 200.f) * 200.f;
	currentScissor[2] = ((right - left + 2) / 320.f) * 320.f;
	currentScissor[3] = ((bottom - top + 2) / 200.f) * 200.f;

	currentScissor[0] = MAX(currentScissor[0], 0.f);
	currentScissor[1] = MAX(currentScissor[1], 0.f);

	return;
	// TODO: later
	glEnable(GL_SCISSOR_TEST);
	glScissor(currentScissor[0], currentScissor[1], currentScissor[2], currentScissor[3]);
}

void osystem_drawMask(int roomId, int maskId) {
	// if (g_gameId == TIMEGATE)
	//     return;

	if (!maskTextures[roomId][maskId].maskTexture)
		return;

	if (!maskTextures[roomId][maskId].vertexBuffer)
		return;

	glDepthMask(GL_FALSE);
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, maskTextures[roomId][maskId].vertexBuffer));

	maskShader->enableVertexAttribute("a_position", maskTextures[roomId][maskId].vertexBuffer, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (uint32)0);
	maskShader->enableVertexAttribute("a_texCoords", maskTextures[roomId][maskId].vertexBuffer, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (uint32)(3 * sizeof(float)));
	maskShader->use();

	GL_CALL(glActiveTexture(GL_TEXTURE0));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, maskTextures[roomId][maskId].maskTexture));

	GL_CALL(glActiveTexture(GL_TEXTURE1));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, g_paletteTexture));

	GL_CALL(glActiveTexture(GL_TEXTURE2));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, g_backgroundTexture));

	GL_CALL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GL_CALL(glActiveTexture(GL_TEXTURE0));

	maskShader->unbind();
	glDepthMask(GL_TRUE);
}

void osystem_clearClip() {
	glScissor(0, 0, 320, 200);
	glDisable(GL_SCISSOR_TEST);
}

void osystem_flip(unsigned char *videoBuffer) {
	osystem_flushPendingPrimitives();
}

void osystem_drawPoint(float X, float Y, float Z, uint8 color, uint8 material, float size) {
	sphereVertex corners[4];
	corners[0].X = X + size;
	corners[0].Y = Y + size;
	corners[0].Z = Z;

	corners[1].X = X + size;
	corners[1].Y = Y - size;
	corners[1].Z = Z;

	corners[2].X = X - size;
	corners[2].Y = Y - size;
	corners[2].Z = Z;

	corners[3].X = X - size;
	corners[3].Y = Y + size;
	corners[3].Z = Z;

	const int mapping[] = {
		0, 1, 2,
		0, 2, 3};

	for (int i = 0; i < 2 * 3; i++) {
		sphereVertex *pVertex = &sphereVertices[numUsedSpheres];
		numUsedSpheres++;
		assert(numUsedSpheres < NUM_MAX_SPHERES_VERTICES);

		pVertex->X = corners[mapping[i]].X;
		pVertex->Y = corners[mapping[i]].Y;
		pVertex->Z = corners[mapping[i]].Z;
		pVertex->U = (color & 0xF) / 15.f;
		pVertex->V = ((color & 0xF0) >> 4) / 15.f;
		pVertex->size = size;
		pVertex->centerX = X;
		pVertex->centerY = Y;
		pVertex->material = material;
	}
}

void osystem_drawSphere(float X, float Y, float Z, uint8 color, uint8 material, float size) {
	osystem_drawPoint(X, Y, Z, color, material, size);
}

Graphics::Surface *gfx_capture() {
	Graphics::Surface *s = new Graphics::Surface();
#ifdef SCUMM_BIG_ENDIAN
	Graphics::PixelFormat format = Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0);
#else
	Graphics::PixelFormat format = Graphics::PixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24);
#endif
	s->create(320 * 4, 200 * 4, format);
	glReadPixels(0, 0, 320 * 4, 200 * 4, GL_RGBA, GL_UNSIGNED_BYTE, s->getPixels());
	Common::Rect rect(0, 0, 320 * 4, 200 * 4);
	s->flipVertical(rect);
	return s;
}

} // namespace Fitd
