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

#ifndef RING_PIXEL_H
#define RING_PIXEL_H

namespace Ring {

// Pixel manipulation
class Pixel {
public:
	struct PixelData {
		float a1;
		float a2;
		float a3;

		PixelData() {
			reset();
		}

		void reset() {
			a1 = 0.0f;
			a2 = 0.0f;
			a3 = 0.0f;
		}
	};

	struct PixelTriplet {
		PixelData p1;
		PixelData p2;
		PixelData p3;

		PixelTriplet() {
			p1.a1 = 1.0f;
			p2.a2 = 1.0f;
			p3.a3 = 1.0f;
		}

		void reset() {
			p1.reset();
			p2.reset();
			p3.reset();
		}
	};

	static void set(int *pixel, int* val);
	static void set(int *val, int *pixel, int offset);
	static void set(int *pixel, int val1, int val2, int val3);
	static void set(PixelTriplet *from, PixelData *pixel, float a1, float a2, float a3);
	static void set(PixelTriplet *from, PixelData *pixel, PixelData *pixel2);
	static void add(float *pixel, float val);
	static void add(PixelData *triplet, PixelData *from, float a1, float a2, float a3);
	static void substract(int *pixel, int val);
	static void multiply(float *pixel, float val);
	static void multiply(PixelData *from, PixelData *pixel, float val);
	static void divide(PixelData *pixel);
};

} // End of namespace Ring

#endif // RING_PIXEL_H
