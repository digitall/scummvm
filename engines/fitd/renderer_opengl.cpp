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

#include "fitd/renderer_opengl.h"
#include "fitd/fitd.h"
#include "fitd/renderer.h"
#include "common/system.h"
#include "engines/util.h"
#include "graphics/opengl/context.h"
#include "graphics/opengl/debug.h"
#include "graphics/opengl/shader.h"
#include "graphics/opengl/system_headers.h"
#include "graphics/surface.h"

#define NUM_MAX_FLAT_VERTICES 5000 * 3
#define NUM_MAX_NOISE_VERTICES 2000 * 3
#define NUM_MAX_TRANSPARENT_VERTICES 1000 * 2
#define NUM_MAX_RAMP_VERTICES 3000 * 3
#define NUM_MAX_SPHERES_VERTICES 3000

namespace Fitd {
static const char *bgVSSrc = R"(
		attribute vec2 a_position;
		attribute vec2 a_texCoords;
		varying vec2 v_texCoords;
		void main() {
			gl_Position = vec4(a_position.xy, 0.0, 1.0);
			v_texCoords = a_texCoords;
		})";

static const char *bgPSShader = R"(
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

static const char *flatVSSrc = R"(
		attribute vec3 a_position;
		attribute vec2 a_texCoords;
		varying vec2 v_texCoords;

		void main()
		{
			gl_Position = vec4(a_position.x/160.0-1.0, 1.0-a_position.y/100.0, a_position.z/40960.0, 1.0);
			v_texCoords = a_texCoords;
		})";

static const char *flatPSSrc = R"(
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

static const char *noiseVSSrc = R"(
			attribute vec3 a_position;
			attribute vec2 a_texCoords;
			varying vec2 v_position;
			varying vec2 v_texCoords;

			void main()
			{
				gl_Position = vec4(a_position.x/160.0-1.0, 1.0-a_position.y/100.0, a_position.z/40960.0, 1.0);
				v_texCoords = a_texCoords;
				v_position = a_position.xy;
			})";

static const char *noisePSSrc = R"(
		varying vec2 v_texCoords;
		varying vec2 v_position;
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

static const char *rampPSSrc = R"(
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

static const char *maskVSSrc = R"(
		attribute vec3 a_position;
		attribute vec2 a_texCoords;
		varying vec2 v_texCoords;
		void main() {
			gl_Position = vec4(a_position.x/160.0-1.0, 1.0-a_position.y/100.0, 0.0, 1.0);
			v_texCoords = a_texCoords;
		})";

static const char *maskPSSrc = R"(
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
			gl_FragColor.a = 0.5;
		})";

static const char *sphereVSSrc = R"(
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

static const char *spherePSSrc = R"(
		varying vec2 v_texCoords;
		varying vec4 v_sphereParams;
		varying vec3 v_screenSpacePosition;
		uniform sampler2D u_palette;

		float noise(vec2 st) {
			return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
		}

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
			} else if(material == 1) { // dither
				float n = noise(v_screenSpacePosition.xy);
				color = ((v_texCoords.x + n) * 15.0) / 2.0;
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

struct maskStruct {
	GLuint maskTexture = 0;
	GLuint vertexBuffer = 0;
	int maskX1;
	int maskY1;
	int maskX2;
	int maskY2;
};

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

static struct State {
	polyVertex noiseVertices[NUM_MAX_NOISE_VERTICES];
	polyVertex flatVertices[NUM_MAX_FLAT_VERTICES];
	polyVertex transparentVertices[NUM_MAX_TRANSPARENT_VERTICES];
	polyVertex rampVertices[NUM_MAX_RAMP_VERTICES];
	sphereVertex sphereVertices[NUM_MAX_SPHERES_VERTICES];
	int numUsedFlatVertices;
	int numUsedNoiseVertices;
	int numUsedTransparentVertices;
	int numUsedRampVertices;
	int numUsedSpheres;
	GLuint g_backgroundTexture = 0;
	GLuint g_paletteTexture = 0;
	OpenGL::Shader *backgroundShader = nullptr;
	OpenGL::Shader *flatShader = nullptr;
	OpenGL::Shader *noiseShader = nullptr;
	OpenGL::Shader *rampShader = nullptr;
	OpenGL::Shader *maskShader = nullptr;
	OpenGL::Shader *sphereShader = nullptr;
	GLuint vbo = 0;
	GLuint ebo = 0;
	Common::Array<Common::Array<maskStruct> > maskTextures; // [room][mask]
	byte physicalScreen[320 * 200];
	byte physicalScreenRGB[320 * 200 * 3];
	byte RGB_Pal[256 * 3];

} *_state;

static void renderer_init();
static void renderer_deinit();
static void renderer_startFrame();
static void renderer_drawBackground();
static void renderer_setPalette(const byte *palette);
static void renderer_copyBlockPhys(byte *videoBuffer, int left, int top, int right, int bottom);
static void renderer_fillPoly(const int16 *buffer, int numPoint, unsigned char color, uint8 polyType);
static void renderer_refreshFrontTextureBuffer();
static void renderer_flushPendingPrimitives();
static void renderer_createMask(const uint8 *mask, int roomId, int maskId, unsigned char *refImage, int maskX1, int maskY1, int maskX2, int maskY2);
static void renderer_setClip(float left, float top, float right, float bottom);
static void renderer_clearClip();
static void renderer_drawMask(int roomId, int maskId);
static void renderer_drawPoint(float X, float Y, float Z, uint8 color, uint8 material, float size);
static void renderer_updateScreen();
static Graphics::Surface *renderer_capture();

Renderer createOpenGLRenderer() {
	Renderer r;
	r.init = renderer_init;
	r.deinit = renderer_deinit;
	r.startFrame = renderer_startFrame;
	r.drawBackground = renderer_drawBackground;
	r.setPalette = renderer_setPalette;
	r.copyBlockPhys = renderer_copyBlockPhys;
	r.fillPoly = renderer_fillPoly;
	r.refreshFrontTextureBuffer = renderer_refreshFrontTextureBuffer;
	r.flushPendingPrimitives = renderer_flushPendingPrimitives;
	r.createMask = renderer_createMask;
	r.setClip = renderer_setClip;
	r.clearClip = renderer_clearClip;
	r.drawMask = renderer_drawMask;
	r.drawPoint = renderer_drawPoint;
	r.updateScreen = renderer_updateScreen;
	r.capture = renderer_capture;
	return r;
}

static void renderer_init() {
	initGraphics3d(320 * 4, 200 * 4);

	_state = new State;
	_state->numUsedFlatVertices = 0;
	_state->numUsedNoiseVertices = 0;
	_state->numUsedTransparentVertices = 0;
	_state->numUsedRampVertices = 0;
	_state->numUsedSpheres = 0;
	_state->g_backgroundTexture = 0;
	_state->g_paletteTexture = 0;
	_state->g_backgroundTexture = 0;
	_state->g_paletteTexture = 0;
	_state->backgroundShader = nullptr;
	_state->flatShader = nullptr;
	_state->noiseShader = nullptr;
	_state->rampShader = nullptr;
	_state->maskShader = nullptr;
	_state->sphereShader = nullptr;
	_state->vbo = 0;
	_state->ebo = 0;

	GL_CALL(glGenBuffers(1, &_state->vbo));
	GL_CALL(glGenBuffers(1, &_state->ebo));

	// create background texture
	GL_CALL(glGenTextures(1, &_state->g_backgroundTexture));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_backgroundTexture));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 320, 200, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr));

	// create palette texture
	GL_CALL(glGenTextures(1, &_state->g_paletteTexture));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_paletteTexture));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 3, 256, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr));

	// create background shader
	_state->backgroundShader = new OpenGL::Shader();
	const char *attributes[] = {"a_position", "a_texCoords", nullptr};
	_state->backgroundShader->loadFromStrings("backgroundShader", bgVSSrc, bgPSShader, attributes, 110);
	_state->backgroundShader->enableVertexAttribute("a_position", _state->vbo, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	_state->backgroundShader->enableVertexAttribute("a_texCoords", _state->vbo, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 2 * sizeof(float));
	_state->backgroundShader->use();
	GL_CALL(glUniform1i(_state->backgroundShader->getUniformLocation("u_background"), 0));
	GL_CALL(glUniform1i(_state->backgroundShader->getUniformLocation("u_palette"), 1));

	// create flat shader
	_state->flatShader = new OpenGL::Shader();
	_state->flatShader->loadFromStrings("flatShader", flatVSSrc, flatPSSrc, attributes, 110);
	_state->flatShader->enableVertexAttribute("a_position", _state->vbo, 3, GL_FLOAT, GL_FALSE, sizeof(polyVertex), 0);
	_state->flatShader->enableVertexAttribute("a_texCoords", _state->vbo, 2, GL_FLOAT, GL_FALSE, sizeof(polyVertex), 3 * sizeof(float));
	_state->flatShader->use();
	GL_CALL(glUniform1i(_state->flatShader->getUniformLocation("u_palette"), 0));

	// create noise shader
	_state->noiseShader = new OpenGL::Shader();
	_state->noiseShader->loadFromStrings("flatShader", noiseVSSrc, noisePSSrc, attributes, 110);
	_state->noiseShader->enableVertexAttribute("a_position", _state->vbo, 3, GL_FLOAT, GL_FALSE, sizeof(polyVertex), 0);
	_state->noiseShader->enableVertexAttribute("a_texCoords", _state->vbo, 2, GL_FLOAT, GL_FALSE, sizeof(polyVertex), 3 * sizeof(float));
	_state->noiseShader->use();
	GL_CALL(glUniform1i(_state->noiseShader->getUniformLocation("u_palette"), 0));

	// create ramp shader
	_state->rampShader = new OpenGL::Shader();
	_state->rampShader->loadFromStrings("rampShader", flatVSSrc, rampPSSrc, attributes, 110);
	_state->rampShader->enableVertexAttribute("a_position", _state->vbo, 3, GL_FLOAT, GL_FALSE, sizeof(polyVertex), 0);
	_state->rampShader->enableVertexAttribute("a_texCoords", _state->vbo, 2, GL_FLOAT, GL_FALSE, sizeof(polyVertex), 3 * sizeof(float));
	_state->rampShader->use();
	GL_CALL(glUniform1i(_state->rampShader->getUniformLocation("u_palette"), 0));

	// create mask shader
	_state->maskShader = new OpenGL::Shader();
	_state->maskShader->loadFromStrings("maskShader", maskVSSrc, maskPSSrc, attributes, 110);
	_state->maskShader->use();
	GL_CALL(glUniform1i(_state->maskShader->getUniformLocation("u_maskPalette"), 0));
	GL_CALL(glUniform1i(_state->maskShader->getUniformLocation("u_palette"), 1));
	GL_CALL(glUniform1i(_state->maskShader->getUniformLocation("u_background"), 2));

	// create sphere shader
	const char *sphere_attributes[] = {"a_position", "a_texCoords", "a_texCoords2", nullptr};
	_state->sphereShader = new OpenGL::Shader();
	_state->sphereShader->loadFromStrings("sphereShader", sphereVSSrc, spherePSSrc, sphere_attributes, 110);
	_state->sphereShader->enableVertexAttribute("a_position", _state->vbo, 3, GL_FLOAT, GL_FALSE, sizeof(sphereVertex), 0);
	_state->sphereShader->enableVertexAttribute("a_texCoords", _state->vbo, 2, GL_FLOAT, GL_FALSE, sizeof(sphereVertex), 3 * sizeof(float));
	_state->sphereShader->enableVertexAttribute("a_texCoords2", _state->vbo, 4, GL_FLOAT, GL_FALSE, sizeof(sphereVertex), 5 * sizeof(float));
	_state->sphereShader->use();
	GL_CALL(glUniform1i(_state->sphereShader->getUniformLocation("u_palette"), 0));
}

static void renderer_deinit() {
	GL_CALL(glDeleteTextures(1, &_state->g_backgroundTexture));
	GL_CALL(glDeleteTextures(1, &_state->g_paletteTexture));
	GL_CALL(glDeleteBuffers(1, &_state->ebo));
	GL_CALL(glDeleteBuffers(1, &_state->vbo));
	delete _state->backgroundShader;
	delete _state->flatShader;
	delete _state->noiseShader;
	delete _state->rampShader;
	delete _state->maskShader;
	delete _state->sphereShader;
	delete _state;
}

static void renderer_startFrame() {
	GL_CALL(glClearColor(0.f, 0.f, 0.f, 1.f));
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

static void renderer_drawBackground() {
	const Vertex vertices[] = {
		{{-1.f, -1.f}, {0.f, 1.f}},
		{{1.f, 1.f}, {1.f, 0.f}},
		{{1.f, -1.f}, {1.f, 1.f}},
		{{-1.f, 1.f}, {0.f, 0.f}}};
	const uint32 indices[] = {
		0, 1, 2,
		0, 3, 1};
	assert(sizeof(Vertex) == 4 * sizeof(float));

	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _state->vbo));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices, GL_STREAM_DRAW));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _state->ebo));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * 6, indices, GL_STREAM_DRAW));

	_state->backgroundShader->use();

	GL_CALL(glActiveTexture(GL_TEXTURE0));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_backgroundTexture));
	GL_CALL(glActiveTexture(GL_TEXTURE1));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_paletteTexture));

	GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
	GL_CALL(glActiveTexture(GL_TEXTURE0));

	_state->backgroundShader->unbind();
}

static void renderer_setPalette(const byte *palette) {
	memcpy(_state->RGB_Pal, palette, 256 * 3);

	GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_paletteTexture));
	GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 3, 256, GL_RED, GL_UNSIGNED_BYTE, _state->RGB_Pal));
}

static void renderer_copyBlockPhys(byte *videoBuffer, int left, int top, int right, int bottom) {
	unsigned char *out = _state->physicalScreenRGB;
	const unsigned char *in = &videoBuffer[0] + left + top * 320;

	while ((right - left) % 4) {
		right++;
	}

	while ((bottom - top) % 4) {
		bottom++;
	}

	for (int i = top; i < bottom; i++) {
		in = &videoBuffer[0] + left + i * 320;
		unsigned char *out2 = _state->physicalScreen + left + i * 320;
		for (int j = left; j < right; j++) {
			const unsigned char color = *in++;

			*out++ = _state->RGB_Pal[color * 3];
			*out++ = _state->RGB_Pal[color * 3 + 1];
			*out++ = _state->RGB_Pal[color * 3 + 2];

			*out2++ = color;
		}
	}

	GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_backgroundTexture));
	GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320, 200, GL_RED, GL_UNSIGNED_BYTE, _state->physicalScreen));
}

static void renderer_refreshFrontTextureBuffer() {
	byte *out = _state->physicalScreenRGB;
	const byte *in = _state->physicalScreen;

	for (int i = 0; i < 200 * 320; i++) {
		const unsigned char color = *in++;
		*out++ = _state->RGB_Pal[color * 3];
		*out++ = _state->RGB_Pal[color * 3 + 1];
		*out++ = _state->RGB_Pal[color * 3 + 2];
	}

	GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_backgroundTexture));
	GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 320, 200, GL_RED, GL_UNSIGNED_BYTE, _state->physicalScreen));
}

static void renderer_flushPendingPrimitives() {
	if (_state->numUsedFlatVertices) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _state->vbo));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(polyVertex) * _state->numUsedFlatVertices, _state->flatVertices, GL_STREAM_DRAW));

		_state->flatShader->use();

		GL_CALL(glActiveTexture(GL_TEXTURE0));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_paletteTexture));

		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, _state->numUsedFlatVertices));
		GL_CALL(glActiveTexture(GL_TEXTURE0));

		_state->flatShader->unbind();

		glDisable(GL_DEPTH_TEST);
		_state->numUsedFlatVertices = 0;
	}

	if (_state->numUsedNoiseVertices) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _state->vbo));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(polyVertex) * _state->numUsedNoiseVertices, _state->noiseVertices, GL_STREAM_DRAW));

		_state->noiseShader->use();

		GL_CALL(glActiveTexture(GL_TEXTURE0));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_paletteTexture));

		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, _state->numUsedNoiseVertices));
		GL_CALL(glActiveTexture(GL_TEXTURE0));

		_state->noiseShader->unbind();

		glDisable(GL_DEPTH_TEST);
		_state->numUsedNoiseVertices = 0;
	}

	if (_state->numUsedRampVertices) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _state->vbo));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(polyVertex) * _state->numUsedRampVertices, _state->rampVertices, GL_STREAM_DRAW));

		_state->rampShader->use();

		GL_CALL(glActiveTexture(GL_TEXTURE0));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_paletteTexture));

		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, _state->numUsedRampVertices));
		GL_CALL(glActiveTexture(GL_TEXTURE0));

		_state->rampShader->unbind();

		glDisable(GL_DEPTH_TEST);

		_state->numUsedRampVertices = 0;
	}

	if (_state->numUsedSpheres) {
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _state->vbo));
		GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertex) * _state->numUsedSpheres, _state->sphereVertices, GL_STREAM_DRAW));

		_state->sphereShader->use();

		GL_CALL(glActiveTexture(GL_TEXTURE0));
		GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_paletteTexture));

		GL_CALL(glDrawArrays(GL_TRIANGLES, 0, _state->numUsedSpheres));
		GL_CALL(glActiveTexture(GL_TEXTURE0));

		_state->sphereShader->unbind();

		glDisable(GL_DEPTH_TEST);

		_state->numUsedSpheres = 0;
	}

	_state->numUsedTransparentVertices = 0;
}

static void renderer_createMask(const uint8 *mask, int roomId, int maskId, unsigned char *refImage, int maskX1, int maskY1, int maskX2, int maskY2) {
	if (_state->maskTextures.size() < (uint)(roomId + 1)) {
		_state->maskTextures.resize(roomId + 1);
	}
	if (_state->maskTextures[roomId].size() < (uint)(maskId + 1)) {
		_state->maskTextures[roomId].resize(maskId + 1);
	}

	if (_state->maskTextures[roomId][maskId].maskTexture) {
		glDeleteTextures(1, &_state->maskTextures[roomId][maskId].maskTexture);
		_state->maskTextures[roomId][maskId].maskTexture = 0;
	}

	if (_state->maskTextures[roomId][maskId].vertexBuffer) {
		glDeleteBuffers(1, &_state->maskTextures[roomId][maskId].vertexBuffer);
		_state->maskTextures[roomId][maskId].vertexBuffer = 0;
	}

	GL_CALL(glGenTextures(1, &_state->maskTextures[roomId][maskId].maskTexture));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->maskTextures[roomId][maskId].maskTexture));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 320, 200, 0, GL_RED, GL_UNSIGNED_BYTE, mask));

	_state->maskTextures[roomId][maskId].maskX1 = maskX1;
	_state->maskTextures[roomId][maskId].maskX2 = maskX2 + 1;
	_state->maskTextures[roomId][maskId].maskY1 = maskY1;
	_state->maskTextures[roomId][maskId].maskY2 = maskY2 + 1;

	struct sVertice {
		float position[3];
		float texcoord[2];
	} vertexBuffer[4];

	const float X1 = _state->maskTextures[roomId][maskId].maskX1;
	const float X2 = _state->maskTextures[roomId][maskId].maskX2;
	const float Y1 = _state->maskTextures[roomId][maskId].maskY1;
	const float Y2 = _state->maskTextures[roomId][maskId].maskY2;

	const float maskZ = 0.f;

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

	GL_CALL(glGenBuffers(1, &_state->maskTextures[roomId][maskId].vertexBuffer));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _state->maskTextures[roomId][maskId].vertexBuffer));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(sVertice) * 4, vertexBuffer, GL_STREAM_DRAW));
}

static void renderer_setClip(float left, float top, float right, float bottom) {

	float currentScissor[4];
	currentScissor[0] = (left - 1) / 320.f * 1280.f;
	currentScissor[1] = (top - 1) / 200.f * 800.f;
	currentScissor[2] = (right - left + 2) / 320.f * 1280.f;
	currentScissor[3] = (bottom - top + 2) / 200.f * 800.f;

	currentScissor[0] = MAX(currentScissor[0], 0.f);
	currentScissor[1] = MAX(currentScissor[1], 0.f);

	glEnable(GL_SCISSOR_TEST);
	glScissor(currentScissor[0], 800.f - currentScissor[3] - currentScissor[1], currentScissor[2], currentScissor[3]);
}

static void renderer_clearClip() {
	glScissor(0, 0, 320.f * 4.f, 200.f * 4.f);
	glDisable(GL_SCISSOR_TEST);
}

static void renderer_drawMask(int roomId, int maskId) {
	if (g_engine->getGameId() == GID_TIMEGATE)
		return;

	if (!_state->maskTextures[roomId][maskId].maskTexture)
		return;

	if (!_state->maskTextures[roomId][maskId].vertexBuffer)
		return;

	glDepthMask(GL_FALSE);
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _state->maskTextures[roomId][maskId].vertexBuffer));

	_state->maskShader->enableVertexAttribute("a_position", _state->maskTextures[roomId][maskId].vertexBuffer, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	_state->maskShader->enableVertexAttribute("a_texCoords", _state->maskTextures[roomId][maskId].vertexBuffer, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 3 * sizeof(float));
	_state->maskShader->use();

	GL_CALL(glActiveTexture(GL_TEXTURE0));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->maskTextures[roomId][maskId].maskTexture));

	GL_CALL(glActiveTexture(GL_TEXTURE1));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_paletteTexture));

	GL_CALL(glActiveTexture(GL_TEXTURE2));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, _state->g_backgroundTexture));

	GL_CALL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GL_CALL(glActiveTexture(GL_TEXTURE0));

	_state->maskShader->unbind();
	glDepthMask(GL_TRUE);
}

static void renderer_fillPoly(const int16 *buffer, int numPoint, unsigned char color, uint8 polyType) {
#define MAX_POINTS_PER_POLY 50
	// float UVArray[MAX_POINTS_PER_POLY];

	assert(numPoint < MAX_POINTS_PER_POLY);

	// compute the polygon bounding box
	float polyMinX = 320.f;
	float polyMaxX = 0.f;
	float polyMinY = 200.f;
	float polyMaxY = 0.f;

	for (int i = 0; i < numPoint; i++) {
		const float X = buffer[3 * i + 0];
		const float Y = buffer[3 * i + 1];

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
		polyVertex *pVertex = &_state->flatVertices[_state->numUsedFlatVertices];
		_state->numUsedFlatVertices += (numPoint - 2) * 3;
		assert(_state->numUsedFlatVertices < NUM_MAX_FLAT_VERTICES);

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

			const int bank = (color & 0xF0) >> 4;
			const int startColor = color & 0xF;
			const float colorf = startColor;
			pVertex->U = colorf / 15.f;
			pVertex->V = bank / 15.f;
			pVertex++;
		}
		break;
	}
	case 1: // dither (pierre/tele)
	{
		polyVertex *pVertex = &_state->noiseVertices[_state->numUsedNoiseVertices];
		_state->numUsedNoiseVertices += (numPoint - 2) * 3;
		assert(_state->numUsedNoiseVertices < NUM_MAX_NOISE_VERTICES);

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

			const int bank = (color & 0xF0) >> 4;
			const int startColor = color & 0xF;
			const float colorf = startColor;
			pVertex->U = colorf / 15.f;
			pVertex->V = bank / 15.f;
			pVertex++;
		}
		break;
	}
	case 2: // trans
	{
		polyVertex *pVertex = &_state->transparentVertices[_state->numUsedTransparentVertices];
		_state->numUsedTransparentVertices += (numPoint - 2) * 3;
		assert(_state->numUsedTransparentVertices < NUM_MAX_TRANSPARENT_VERTICES);

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

			pVertex->R = _state->RGB_Pal[color * 3];
			pVertex->G = _state->RGB_Pal[color * 3 + 1];
			pVertex->B = _state->RGB_Pal[color * 3 + 2];
			pVertex->A = 128;
			pVertex++;
		}
		break;
	}
	case 4: // copper (ramps top to bottom)
	case 5: // copper2 (ramps top to bottom, 2 scanline per color)
	{
		polyVertex *pVertex = &_state->rampVertices[_state->numUsedRampVertices];
		_state->numUsedRampVertices += (numPoint - 2) * 3;
		assert(_state->numUsedRampVertices < NUM_MAX_RAMP_VERTICES);

		const int bank = (color & 0xF0) >> 4;
		const int startColor = color & 0xF;
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

			const float colorf = startColor + colorStep * (pVertex->Y - polyMinY);

			pVertex->U = colorf / 15.f;
			pVertex->V = bank / 15.f;
			pVertex++;
		}
		break;
	}
	case 3: // marbre (ramp left to right)
	{
		polyVertex *pVertex = &_state->rampVertices[_state->numUsedRampVertices];
		_state->numUsedRampVertices += (numPoint - 2) * 3;
		assert(_state->numUsedRampVertices < NUM_MAX_RAMP_VERTICES);

		const float colorStep = 15.f / polyWidth;

		const int bank = (color & 0xF0) >> 4;
		const int startColor = color & 0xF;

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

			const float colorf = startColor + colorStep * (pVertex->X - polyMinX);

			pVertex->U = colorf / 15.f;
			pVertex->V = bank / 15.f;
			pVertex++;
		}
		break;
	}
	case 6: // marbre2 (ramp right to left)
	{
		polyVertex *pVertex = &_state->rampVertices[_state->numUsedRampVertices];
		_state->numUsedRampVertices += (numPoint - 2) * 3;
		assert(_state->numUsedRampVertices < NUM_MAX_RAMP_VERTICES);

		const float colorStep = 15.f / polyWidth;

		const int bank = (color & 0xF0) >> 4;
		const int startColor = color & 0xF;

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

			const float colorf = startColor + colorStep * (pVertex->X - polyMinX);

			pVertex->U = 1.f - colorf / 15.f;
			pVertex->V = bank / 15.f;
			pVertex++;
		}
		break;
	}
	}
}

static void renderer_drawPoint(float X, float Y, float Z, uint8 color, uint8 material, float size) {
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
		sphereVertex *pVertex = &_state->sphereVertices[_state->numUsedSpheres];
		_state->numUsedSpheres++;
		assert(_state->numUsedSpheres < NUM_MAX_SPHERES_VERTICES);

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

static void renderer_updateScreen() {
	g_system->updateScreen();
}

Graphics::Surface *renderer_capture() {
	Graphics::Surface *s = new Graphics::Surface();
#ifdef SCUMM_BIG_ENDIAN
	Graphics::PixelFormat format = Graphics::PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0);
#else
	Graphics::PixelFormat format = Graphics::PixelFormat(4, 8, 8, 8, 8, 0, 8, 16, 24);
#endif
	s->create(320 * 4, 200 * 4, format);
	glReadPixels(0, 0, 320 * 4, 200 * 4, GL_RGBA, GL_UNSIGNED_BYTE, s->getPixels());
	const Common::Rect rect(0, 0, 320 * 4, 200 * 4);
	s->flipVertical(rect);
	return s;
}

} // namespace Fitd
