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

#include "common/scummsys.h"

namespace Fitd {

void hline(int x1, int x2, int y, byte c);
void line(int x1, int y1, int x2, int y2, byte c);
void pixel(int x, int y, byte c);

#define SCREENHEIGHT 200
#define MAXPTS 10

#define putdot(x, y)                    \
	if ((y >= 0) && (y < SCREENHEIGHT)) \
		dots[y][counters[y]++] = x;

void swapFunc(int *a, int *b);
#define swap(a, b) (swapFunc(&a, &b))

void fillpoly(int16 *datas, int n, byte c) {
	static int dots[SCREENHEIGHT][MAXPTS];
	static int counters[SCREENHEIGHT];
	int16 x1, y1;
	int i, j, dir = -2;

	if (n <= 2) {
		switch (n) {
		case 0:
			return;
		case 1:
			pixel(datas[0], datas[1], c);
			return;
		case 2:
			line(datas[0], datas[1], datas[2], datas[3], c);
			return;
		}
	}

	// Reinit array counters

	for (i = 0; i < SCREENHEIGHT; i++) {
		counters[i] = 0;
	}

	// Drawing lines

	int16 x2 = datas[n * 2 - 2];
	int16 y2 = datas[n * 2 - 1];

	for (i = 0; i < n; i++) {
		x1 = x2;
		y1 = y2;
		x2 = datas[i * 2];
		y2 = datas[i * 2 + 1];

		//  line(x1, y1, x2, y2, c);
		//  continue;

		if (y1 == y2) {
			//      printf("Horizontal line. x1: %i, y1: %i, x2: %i, y2: %i\n", x1, y1, x2, y2);
			if (!dir)
				continue;
			putdot(x1, y1);
			dir = 0;
			continue;
		}

		const double step = static_cast<double>(x2 - x1) / (y2 - y1);

		//  printf("x1: %i, y1 = %i, x2 = %i, y2 = %i, step: %f\n", x1, y1, x2, y2, step);

		double curx = x1;

		if (y1 < y2) {
			for (j = y1; j < y2; j++, curx += step) {
				//    printf("j = %i, curx = %f\n", j, curx);
				putdot(static_cast<int>(curx + 0.5), j);
			}
			if (dir == -1) {
				//    printf("Adding extra (%i, %i)\n", x1, y1);
				putdot(x1, y1);
			}
			dir = 1;
		} else {
			for (j = y1; j > y2; j--, curx -= step) {
				//    printf("j = %i, curx = %f\n", j, curx);
				putdot(static_cast<int>(curx + 0.5), j);
			}
			if (dir == 1) {
				//    printf("Adding extra (%i, %i)\n", x1, y1);
				putdot(x1, y1);
			}
			dir = -1;
		}
	}

	x1 = x2;
	y1 = y2;
	x2 = datas[0];
	y2 = datas[1];

	if ((y1 < y2 && dir == -1) || (y1 > y2 && dir == 1) || (y1 == y2 && dir == 0)) {
		//  printf("Adding final extra (%i, %i)\n", x1, y1);
		putdot(x1, y1);
	}

	// NOTE: all counters should be even now. If not, this is a bad (C) thing :-P

	// Sorting datas

	for (i = 0; i < SCREENHEIGHT; i++) {
		// Very bad sorting... but arrays are very small (0, 2 or 4), so it's no quite use...
		for (j = 0; j < counters[i] - 1; j++) {
			for (int k = 0; k < counters[i] - 1; k++) {
				if (dots[i][k] > dots[i][k + 1])
					swap(dots[i][k], dots[i][k + 1]);
			}
		}
	}

	// Drawing.

	for (i = 0; i < SCREENHEIGHT; i++) {
		if (counters[i]) {
			//      printf("%i dots on line %i\n", counters[i], i);
			for (j = 0; j < counters[i] - 1; j += 2) {
				//    printf("Drawing line (%i, %i)-%i\n", dots[i][j], dots[i][j + 1], i);
				hline(dots[i][j], dots[i][j + 1], i, c);
			}
		}
	}
}

#undef swap
} // namespace Fitd
