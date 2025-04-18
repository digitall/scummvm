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

#include "common/algorithm.h"
#include "common/util.h"
#include "graphics/primitives.h"

namespace Graphics {

void Primitives::drawLine(int x0, int y0, int x1, int y1, uint32 color, void *data) {
	// Bresenham's line algorithm, as described by Wikipedia
	const bool steep = ABS(y1 - y0) > ABS(x1 - x0);

	if (steep) {
		SWAP(x0, y0);
		SWAP(x1, y1);
	}

	const int delta_x = ABS(x1 - x0);
	const int delta_y = ABS(y1 - y0);
	const int delta_err = delta_y;
	int x = x0;
	int y = y0;
	int err = 0;

	const int x_step = (x0 < x1) ? 1 : -1;
	const int y_step = (y0 < y1) ? 1 : -1;

	if (steep)
		drawPoint(y, x, color, data);
	else
		drawPoint(x, y, color, data);

	while (x != x1) {
		x += x_step;
		err += delta_err;
		if (2 * err > delta_x) {
			y += y_step;
			err -= delta_x;
		}
		if (steep)
			drawPoint(y, x, color, data);
		else
			drawPoint(x, y, color, data);
	}
}

void Primitives::drawHLine(int x1, int x2, int y, uint32 color, void *data) {
	if (x1 > x2)
		SWAP(x1, x2);

	for (int x = x1; x <= x2; x++)
		drawPoint(x, y, color, data);
}

void Primitives::drawVLine(int x, int y1, int y2, uint32 color, void *data) {
	if (y1 > y2)
		SWAP(y1, y2);

	for (int y = y1; y <= y2; y++)
		drawPoint(x, y, color, data);
}

void Primitives::drawThickLine(int x0, int y0, int x1, int y1, int penX, int penY, uint32 color, void *data) {
	assert(penX > 0 && penY > 0);

	// Shortcut
	if (penX == 1 && penY == 1) {
		drawLine(x0, y0, x1, y1, color, data);
		return;
	}

	// TODO: Optimize this. It currently is a very naive way of handling
	// thick lines since quite often it will be drawing to the same pixel
	// multiple times.
	for (int x = 0; x < penX; x++)
		for (int y = 0; y < penY; y++)
			drawLine(x0 + x, y0 + y, x1 + x, y1 + y, color, data);
}

/* Bresenham as presented in Foley & Van Dam */
/* Code is based on GD lib http://libgd.github.io/ */
void Primitives::drawThickLine2(int x1, int y1, int x2, int y2, int thick, uint32 color, void *data) {
	int incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
	int wid;
	int w, wstart;

	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);

	if (dx == 0) {
		int xn = x1 - thick / 2;
		Common::Rect r(xn, MIN(y1, y2), xn + thick - 1, MAX(y1, y2));
		drawFilledRect1(r, color, data);
		return;
	} else if (dy == 0) {
		int yn = y1 - thick / 2;
		Common::Rect r(MIN(x1, x2), yn, MAX(x1, x2), yn + thick - 1);
		drawFilledRect1(r, color, data);
		return;
	}

	if (dy <= dx) {
		/* More-or-less horizontal. use wid for vertical stroke */

		/* 2.0.12: Michael Schwartz: divide rather than multiply;
			  TBB: but watch out for /0! */
		if (dx != 0 && thick != 0) {
			double ac_recip = 1.0/dx * sqrt((double)(dx * dx + dy * dy)); // 1 / cos(atan2((double)dy, (double)dx));
			wid = thick * ac_recip;
		} else {
			wid = 1;
		}

		d = 2 * dy - dx;
		incr1 = 2 * dy;
		incr2 = 2 * (dy - dx);
		if (x1 > x2) {
			x = x2;
			y = y2;
			ydirflag = (-1);
			xend = x1;
		} else {
			x = x1;
			y = y1;
			ydirflag = 1;
			xend = x2;
		}

		/* Set up line thickness */
		wstart = y - wid / 2;
		for (w = wstart; w < wstart + wid; w++)
			drawPoint(x, w, color, data);

		if (((y2 - y1) * ydirflag) > 0) {
			while (x < xend) {
				x++;
				if (d < 0) {
					d += incr1;
				} else {
					y++;
					d += incr2;
				}
				wstart = y - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					drawPoint(x, w, color, data);
			}
		} else {
			while (x < xend) {
				x++;
				if (d < 0) {
					d += incr1;
				} else {
					y--;
					d += incr2;
				}
				wstart = y - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					drawPoint(x, w, color, data);
			}
		}
	} else {
		/* More-or-less vertical. use wid for horizontal stroke */
		/* 2.0.12: Michael Schwartz: divide rather than multiply;
		   TBB: but watch out for /0! */
		if (dy != 0 && thick != 0) {
			double as_recip = 1.0/dy * sqrt((double)(dx * dx + dy * dy)); // 1 / sin(atan2((double)dy, (double)dx));
			wid = thick * as_recip;
		} else {
			wid = 1;
		}

		d = 2 * dx - dy;
		incr1 = 2 * dx;
		incr2 = 2 * (dx - dy);
		if (y1 > y2) {
			y = y2;
			x = x2;
			yend = y1;
			xdirflag = (-1);
		} else {
			y = y1;
			x = x1;
			yend = y2;
			xdirflag = 1;
		}

		/* Set up line thickness */
		wstart = x - wid / 2;
		for (w = wstart; w < wstart + wid; w++)
			drawPoint(w, y, color, data);

		if (((x2 - x1) * xdirflag) > 0) {
			while (y < yend) {
				y++;
				if (d < 0) {
					d += incr1;
				} else {
					x++;
					d += incr2;
				}
				wstart = x - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					drawPoint(w, y, color, data);
			}
		} else {
			while (y < yend) {
				y++;
				if (d < 0) {
					d += incr1;
				} else {
					x--;
					d += incr2;
				}
				wstart = x - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					drawPoint(w, y, color, data);
			}
		}
	}
}

void Primitives::drawFilledRect(const Common::Rect &rect, uint32 color, void *data) {
	for (int y = rect.top; y < rect.bottom; y++)
		drawHLine(rect.left, rect.right - 1, y, color, data);
}

/**
 * @brief Draws filled rectangle _with_ right and bottom edges
 */
void Primitives::drawFilledRect1(const Common::Rect &rect, uint32 color, void *data) {
	for (int y = rect.top; y <= rect.bottom; y++)
		drawHLine(rect.left, rect.right, y, color, data);
}

void Primitives::drawRect(const Common::Rect &rect, uint32 color, void *data) {
	drawHLine(rect.left, rect.right - 1, rect.top, color, data);
	drawHLine(rect.left, rect.right - 1, rect.bottom - 1, color, data);
	drawVLine(rect.left, rect.top, rect.bottom - 1, color, data);
	drawVLine(rect.right - 1, rect.top, rect.bottom - 1, color, data);
}

/**
 * @brief Draws rectangle outline _with_ right and bottom edges
 */
void Primitives::drawRect1(const Common::Rect &rect, uint32 color, void *data) {
	drawHLine(rect.left + 1, rect.right - 1, rect.top, color, data);
	drawHLine(rect.left + 1, rect.right - 1, rect.bottom, color, data);
	drawVLine(rect.left, rect.top, rect.bottom, color, data);
	drawVLine(rect.right, rect.top, rect.bottom, color, data);
}

void Primitives::drawRoundRect(const Common::Rect &rect, int arc, uint32 color, bool filled, void *data) {
	Common::Rect r(rect.left, rect.top, rect.right - 1, rect.bottom - 1);

	drawRoundRect1(r, arc, color, filled, data);
}

// http://members.chello.at/easyfilter/bresenham.html
void Primitives::drawRoundRect1(const Common::Rect &rect, int arc, uint32 color, bool filled, void *data) {
	if (rect.height() < rect.width()) {
		int x = -arc, y = 0, err = 2-2*arc; /* II. Quadrant */
		int dy = rect.height() - arc * 2;
		int r = arc;
		int stop = 0;
		int lastx = 0, lasty = 0;
		if (dy < 0)
			stop = -dy / 2;

		do {
			if (filled) {
				drawHLine(rect.left + x + r, rect.right - x - r, rect.top    - y + r - stop, color, data);
				drawHLine(rect.left + x + r, rect.right - x - r, rect.bottom + y - r + stop, color, data);
			} else {
				drawPoint(rect.left  + x + r, rect.top    - y + r - stop, color, data);
				drawPoint(rect.right - x - r, rect.top    - y + r - stop, color, data);
				drawPoint(rect.left  + x + r, rect.bottom + y - r + stop, color, data);
				drawPoint(rect.right - x - r, rect.bottom + y - r + stop, color, data);

				lastx = x;
				lasty = y;
			}
			arc = err;
			if (arc <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
			if (arc > x || err > y) err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
			if (stop && y > stop)
				break;
		} while (x < 0);

		if (!filled) {
			x = lastx;
			y = lasty;

			drawHLine(rect.left + x + r, rect.right - x - r, rect.top    - y + r - stop, color, data);
			drawHLine(rect.left + x + r, rect.right - x - r, rect.bottom + y - r + stop, color, data);
		}

		for (int i = 1; i < dy; i++) {
			if (filled) {
				drawHLine(rect.left, rect.right, rect.top + r + i, color, data);
			} else {
				drawPoint(rect.left,  rect.top + r + i, color, data);
				drawPoint(rect.right, rect.top + r + i, color, data);
			}
		}
	} else {
		int y = -arc, x = 0, err = 2-2*arc; /* II. Quadrant */
		int dx = rect.width() - arc * 2;
		int r = arc;
		int stop = 0;
		int lastx = 0, lasty = 0;
		if (dx < 0)
			stop = -dx / 2;

		do {
			if (filled) {
				drawVLine(rect.left  - x + r - stop, rect.top + y + r, rect.bottom - y - r, color, data);
				drawVLine(rect.right + x - r + stop, rect.top + y + r, rect.bottom - y - r, color, data);
			} else {
				drawPoint(rect.left  - x + r - stop, rect.top    + y + r, color, data);
				drawPoint(rect.left  - x + r - stop, rect.bottom - y - r, color, data);
				drawPoint(rect.right + x - r + stop, rect.top    + y + r, color, data);
				drawPoint(rect.right + x - r + stop, rect.bottom - y - r, color, data);

				lastx = x;
				lasty = y;
			}

			arc = err;
			if (arc <= x) err += ++x*2+1;           /* e_xy+e_y < 0 */
			if (arc > y || err > x) err += ++y*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
			if (stop && x > stop)
				break;
		} while (y < 0);

		if (!filled) {
			x = lastx;
			y = lasty;
			drawVLine(rect.left  - x + r - stop, rect.top + y + r, rect.bottom - y - r, color, data);
			drawVLine(rect.right + x - r + stop, rect.top + y + r, rect.bottom - y - r, color, data);
		}

		for (int i = 1; i < dx; i++) {
			if (filled) {
				drawVLine(rect.left + r + i, rect.top, rect.bottom, color, data);
			} else {
				drawPoint(rect.left + r + i, rect.top,    color, data);
				drawPoint(rect.left + r + i, rect.bottom, color, data);
			}
		}
	}
}

// Based on public-domain code by Darel Rex Finley, 2007
// http://alienryderflex.com/polygon_fill/
void Primitives::drawPolygonScan(const int *polyX, const int *polyY, int npoints, const Common::Rect &bbox, uint32 color, void *data) {
	int *nodeX = (int *)calloc(npoints, sizeof(int));
	int i, j;

	//  Loop through the rows of the image.
	for (int pixelY = bbox.top; pixelY < bbox.bottom; pixelY++) {
		//  Build a list of nodes.
		int nodes = 0;
		j = npoints - 1;

		for (i = 0; i < npoints; i++) {
			if ((polyY[i] < pixelY && polyY[j] >= pixelY) || (polyY[j] < pixelY && polyY[i] >= pixelY)) {
				nodeX[nodes++] = (int)(polyX[i] + (double)(pixelY - polyY[i]) / (double)(polyY[j]-polyY[i]) *
														(double)(polyX[j] - polyX[i]) + 0.5);
			}
			j = i;
		}

		//  Sort the nodes
		Common::sort(nodeX, &nodeX[nodes]);

		//  Fill the pixels between node pairs.
		for (i = 0; i < nodes; i += 2) {
			if (nodeX[i  ] >= bbox.right)
				break;
			if (nodeX[i + 1] > bbox.left) {
				nodeX[i] = MAX<int16>(nodeX[i], bbox.left);
				nodeX[i + 1] = MIN<int16>(nodeX[i + 1], bbox.right);

				drawHLine(nodeX[i], nodeX[i + 1], pixelY, color, data);
			}
		}
	}

	free(nodeX);
}

// http://members.chello.at/easyfilter/bresenham.html
void Primitives::drawEllipse(int x0, int y0, int x1, int y1, uint32 color, bool filled, void *data) {
	int a = abs(x1 - x0), b = abs(y1 - y0), b1 = b & 1; /* values of diameter */
	long dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a; /* error increment */
	long err = dx + dy + b1 * a * a, e2; /* error of 1.step */

	if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
	if (y0 > y1) y0 = y1; /* .. exchange them */
	y0 += (b + 1) / 2; y1 = y0 - b1;   /* starting pixel */
	a *= 8 * a; b1 = 8 * b * b;

	do {
		if (filled) {
			drawHLine(x0, x1, y0, color, data);
			drawHLine(x0, x1, y1, color, data);
		} else {
			drawPoint(x1, y0, color, data); /*   I. Quadrant */
			drawPoint(x0, y0, color, data); /*  II. Quadrant */
			drawPoint(x0, y1, color, data); /* III. Quadrant */
			drawPoint(x1, y1, color, data); /*  IV. Quadrant */
		}
		e2 = 2*err;
		if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */
		if (e2 >= dx || 2*err > dy) { x0++; x1--; err += dx += b1; } /* x step */
	} while (x0 <= x1);

	while (y0-y1 < b) {  /* too early stop of flat ellipses a=1 */
		if (filled) {
			drawHLine(x0 - 1, x0 - 1, y0, color, data); /* -> finish tip of ellipse */
			drawHLine(x1 + 1, x1 + 1, y0, color, data);
			drawHLine(x0 - 1, x0 - 1, y1, color, data);
			drawHLine(x1 + 1, x1 + 1, y1, color, data);
		} else {
			drawPoint(x0 - 1, y0, color, data); /* -> finish tip of ellipse */
			drawPoint(x1 + 1, y0, color, data);
			drawPoint(x0 - 1, y1, color, data);
			drawPoint(x1 + 1, y1, color, data);
		}
		y0++;
		y1--;
	}
}

} // End of namespace Graphics
