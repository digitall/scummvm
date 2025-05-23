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

#ifndef GLK_RAW_DECODER_H
#define GLK_RAW_DECODER_H

#include "graphics/palette.h"
#include "graphics/surface.h"
#include "image/image_decoder.h"

namespace Glk {

/**
 * This image decoder class implements loading of a simplified image format.
 * It's used for sub-engines like Frotz with custom picture formats. They can
 * expose their picture archives using Common::Archive, and have the individual
 * picture files stored in the format for this decoder to load
 * Format:
 * width		2 bytes
 * height		2 bytes
 * pal size		1 byte
 * palette		3 bytes * pal size
 * pixels		width * height pixels
 */
class RawDecoder : public Image::ImageDecoder {
private:
	Graphics::Surface _surface;
	Graphics::Palette _palette;
	uint32 _transColor;
public:
	RawDecoder();
	~RawDecoder() override;

	bool loadStream(Common::SeekableReadStream &stream) override;
	void destroy() override;
	const Graphics::Surface *getSurface() const override { return &_surface; }
	const Graphics::Palette &getPalette() const override { return _palette; }
	bool hasTransparentColor() const override { return true; }
	uint32 getTransparentColor() const override { return _transColor; }
};

} // End of namespace Glk

#endif
