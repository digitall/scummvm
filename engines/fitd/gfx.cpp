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
#include "graphics/opengl/context.h"
#include "graphics/opengl/debug.h"
#include "graphics/opengl/shader.h"
#include "graphics/opengl/system_headers.h"

namespace Fitd {

struct Vector2 {
	float x;
	float y;
};

struct Vertex {
	Vector2 pos;
	Vector2 texCoords;
};

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

GLuint g_backgroundTexture = 0;
GLuint g_paletteTexture = 0;

byte currentGamePalette[256 * 3];
byte RGB_Pal[256 * 3];
byte frontBuffer[320 * 200];
byte physicalScreen[320 * 200];
byte physicalScreenRGB[320 * 200 * 3];
OpenGL::Shader *backgroundShader = NULL;
GLuint vbo = 0;
GLuint ebo = 0;
GLint u_background = 0;
GLint u_palette = 0;

void gfx_init() {
	GL_CALL(glGenBuffers(1, &vbo));
	GL_CALL(glGenBuffers(1, &ebo));

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
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices, GL_STATIC_DRAW));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * 6, indices, GL_STATIC_DRAW));

	// create background shader
	backgroundShader = new OpenGL::Shader();
	const char *attributes[] = {"a_position", "a_texCoords", nullptr};
	backgroundShader->loadFromStrings("backgroundShader", bgVSSrc, bgPSShader, attributes, 110);
	backgroundShader->enableVertexAttribute("a_position", vbo, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (uint32)0);
	backgroundShader->enableVertexAttribute("a_texCoords", vbo, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (uint32)(2 * sizeof(float)));

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

	backgroundShader->use();
	u_background = backgroundShader->getUniformLocation("u_background");
	GL_CALL(glUniform1i(u_background, 0));

	u_palette = backgroundShader->getUniformLocation("u_palette");
	GL_CALL(glUniform1i(u_palette, 1));
}

void gfx_deinit() {
	GL_CALL(glDeleteTextures(1, &g_backgroundTexture));
	GL_CALL(glDeleteTextures(1, &g_paletteTexture));
	GL_CALL(glDeleteBuffers(1, &ebo));
	GL_CALL(glDeleteBuffers(1, &vbo));
	delete backgroundShader;
}

void gfx_draw() {
	GL_CALL(glClearColor(0.f, 0.f, 0.f, 1.f));
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

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

} // namespace Fitd
