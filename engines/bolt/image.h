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
 *
 */

#ifndef BOLT_IMAGE_H
#define BOLT_IMAGE_H

#include "common/ptr.h"
#include "common/rect.h"

#include "bolt/blt_file.h"

namespace Bolt {

class Graphics;

typedef Common::SharedPtr<class BltImage> BltImagePtr;

class BltImage {
public:
	static BltImagePtr load(BltFile *bltFile, BltLongId id);

	void drawToBack(Graphics *graphics, int x, int y, bool transparency) const;

	const byte* getImageData() const;
	uint16 getWidth() const { return _width; }
	uint16 getHeight() const { return _height; }
	const Common::Point& getOffset() const { return _offset; }

private:
	BltImage();

	void init(BltFile *bltFile, BltLongId id);

	BltResourcePtr _res;
	byte _compression;
	uint16 _width;
	uint16 _height;
	Common::Point _offset;
};

} // End of namespace Bolt

#endif
