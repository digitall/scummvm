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

#ifndef RING_SCREEN_H
#define RING_SCREEN_H

#include "graphics/surface.h"

#include "ring/shared.h"

#include "common/rect.h"

namespace Ring {

class ImageSurface;
class Text;

class ScreenManager {
public:
	ScreenManager();
	~ScreenManager();

	void init();
	//void createFont()
	void clear();
	void drawAndUpdate(ImageSurface *image);
	void drawAndUpdate(ImageSurface *image, const Common::Point &point);
	void draw(ImageSurface *image, const Common::Point &point, DrawType type);
	void draw(ImageSurface *image, DrawType type);
	void drawImage(ImageSurface *image, const Common::Point &dest, uint32 srcWidth, uint32 srcHeight, int32 srcX, int32 offset);
	void drawRectangle(const Common::Rect &rect, uint32 color);
	//
	//void readPalette();
	//
	void drawText(Common::String text, const Common::Point &coords, Color color);
	void drawText(Text *text);
	void drawText(Text *text, const Common::Rect &exclude, const Common::Rect &exclude2);

	void copySurface(ImageSurface *image) const;

	void updateScreen();

	void setOffset(Common::Point offset) { _offset = offset; }
	Graphics::Surface *getSurface() { return &_screen; }
private:
	Graphics::Surface _screen;

	Common::Point _offset;

	void debugDrawing();
};

} // End of namespace Ring

#endif // RING_SCREEN_H
