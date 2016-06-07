/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "ring/graphics/pixel.h"

#include "common/scummsys.h"

namespace Ring {

void Pixel::set(int *pixel, int *val) {
	*pixel = *val;
}

void Pixel::set(int *val, int *pixel, int offset) {
	*pixel = *val - offset;
}

void Pixel::set(int *pixel, int val1, int val2, int val3) {
	if (val1 < val2) {
		*pixel = val2;
	} else if (val1 > val3) {
		*pixel = val3;
	} else {
		*pixel = val1;
	}
}

void Pixel::set(PixelTriplet *triplet, PixelData *from, float a1, float a2, float a3) {
	triplet->reset();

	// Compute pixels
	PixelData pixel1, pixel2;

	float val = from->a1 * a1 + from->a2 * a2 + from->a3 * a3;
	pixel2.a1 = a1 - val * from->a1;
	pixel2.a2 = a2 - val * from->a2;
	pixel2.a3 = a3 - val * from->a3;

	divide(&pixel2);

	pixel1.a1 = pixel2.a2 * from->a3 - pixel2.a3 * from->a2;
	pixel1.a2 = pixel2.a3 * from->a1 - pixel2.a1 * from->a3;
	pixel1.a3 = pixel2.a1 * from->a2 - pixel2.a2 * from->a1;

	// Update triplet
	triplet->p3 = *from;
	triplet->p2 = pixel2;
	triplet->p1 = pixel1;
}

void Pixel::set(PixelTriplet *from, PixelData *pixel, PixelData *pixel2) {
	float a1 = pixel2->a1;
	float a2 = pixel2->a2;
	float a3 = pixel2->a3;

	pixel->a1 = (a1 * from->p1.a1) + (a2 * from->p2.a1) + (a3 * from->p3.a1);
	pixel->a2 = (a1 * from->p1.a2) + (a2 * from->p2.a2) + (a3 * from->p3.a2);
	pixel->a3 = (a1 * from->p1.a3) + (a2 * from->p2.a3) + (a3 * from->p3.a3);
}

void Pixel::add(float *pixel, float val) {
	*pixel += val;
}

void Pixel::add(PixelData *from, PixelData *pixel, float a1, float a2, float a3) {
	pixel->a1 = from->a1 + a1;
	pixel->a2 = from->a2 + a2;
	pixel->a3 = from->a3 + a3;
}

void Pixel::substract(int *pixel, int val) {
	*pixel -= val;
}

void Pixel::multiply(float *pixel, float val) {
	*pixel = val * 65536.0f;
}

void Pixel::multiply(PixelData *from, PixelData *pixel, float val) {
	pixel->a1 = from->a1 * val;
	pixel->a2 = from->a2 * val;
	pixel->a3 = from->a3 * val;
}

void Pixel::divide(PixelData *pixel) {
	float val = sqrt(pixel->a1 * pixel->a1 + pixel->a2 * pixel->a2 + pixel->a3 * pixel->a3);
	if (val == 0.0f)
		return;

	pixel->a1 = pixel->a1 / val;
	pixel->a2 = pixel->a2 / val;
	pixel->a3 = pixel->a3 / val;
}

} // End of namespace Ring
