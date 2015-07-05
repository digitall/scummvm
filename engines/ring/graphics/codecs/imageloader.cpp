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

#include "ring/graphics/codecs/imageloader.h"

#include "ring/graphics/image.h"

namespace Ring {

void ImageLoader::invertSurface(Graphics::Surface *out, const Graphics::Surface &in) {

	// Images should be decoded with origin at the top
	out->create(in.w, in.h, in.format);

	switch (in.format.bytesPerPixel) {
	default:
		error("[ImageLoader::copySurface] Unsupported pixel depth (%s)", _filename.c_str());

	// FIXME Handle endianess
	case 2:
		for (int i = 0; i < out->h; i++) {
			uint16 *dst = static_cast<uint16 *>(out->getBasePtr(0, out->h - i - 1));
			const uint16 *orig = static_cast<const uint16 *>(in.getBasePtr(0, i));

			for (int j = 0; j < out->w; j++)
				*dst++ = *orig++;
		}
		break;

	case 4:
		for (int i = 0; i < out->h; i++) {
			uint32 *dst = static_cast<uint32 *>(out->getBasePtr(0, out->h - i - 1));
			const uint32 *orig = static_cast<const uint32 *>(in.getBasePtr(0, i));

			for (int j = 0; j < out->w; j++)
				*dst++ = *orig++;
		}
		break;
	}
}

} // End of namespace Ring
