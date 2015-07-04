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

#include "ring/graphics/image.h"

#include "ring/base/object.h"

#include "ring/graphics/imageLoader.h"

#include "ring/debug.h"
#include "ring/helpers.h"
#include "ring/ring.h"

namespace Ring {

#pragma region ImageHandle

ImageHandle::ImageHandle() : ImageSurface() {
	_isActive = false;
	_drawType = kDrawTypeInvalid;
	_priority = 0;
	_imageCount = 0;
	_field_6C = 0;
	_imageType = kImageTypeBMP;
	_objectPresentation = NULL;
	_animation = NULL;
	_zone = kZoneNone;
	_loadFrom = kLoadFromInvalid;
	_archiveType = kArchiveInvalid;

	_index = 0;
}

ImageHandle::ImageHandle(Common::String nameId, const Common::Point &point, bool active, DrawType drawType, uint32 priority, byte imageCount, ZoneId zone, LoadFrom loadFrom, ImageType imageType, ArchiveType archiveType) : ImageSurface() {
	init(nameId, point, active, drawType, priority, imageCount, zone, loadFrom, imageType, archiveType);
}

ImageHandle::ImageHandle(Common::String nameId, const Common::Point &point, bool active, ZoneId zone, LoadFrom loadFrom, ImageType imageType, ArchiveType archiveType) : ImageSurface() {
	init(nameId, point, active, kDrawTypeNormal, 0, 0, zone, loadFrom, imageType, archiveType);
}

ImageHandle::~ImageHandle() {
	// Zero-out passed pointers
	_objectPresentation = NULL;
	_animation = NULL;
}

void ImageHandle::init(Common::String nameId, const Common::Point &point, bool active, DrawType drawType, uint32 priority, byte imageCount, ZoneId zone, LoadFrom loadFrom, ImageType imageType, ArchiveType archiveType) {
	_nameId = nameId;
	_coordinates = point;
	_originalCoordinates = point;
	_isActive = active;
	_drawType = drawType;
	_priority = priority;
	_imageCount = imageCount;
	_field_6C = 1;
	_imageType = imageType;
	_objectPresentation = NULL;
	_animation = NULL;
	_zone = zone;
	_loadFrom = loadFrom;
	_archiveType = archiveType;

	_filename = "";
	_directory = "";
	_surface = NULL;

	_index = 0;
}

void ImageHandle::loadImage() {
	if (_nameId.empty())
		return;

	// Compute file path
	Common::String filename = Common::String::format("%s%s", _directory.c_str(), _nameId.c_str());

	if (!load(filename, _archiveType, _zone, _loadFrom, _drawType))
		error("[ImageHandle::load] Cannot load image (%s)", filename.c_str());
}

#pragma region Serializable

void ImageHandle::saveLoadWithSerializer(Common::Serializer &s) {
	s.syncString(_directory);
	s.syncString(_nameId);
	s.syncAsSint16LE(_coordinates.x);
	s.syncAsSint16LE(_coordinates.y);
	s.syncAsByte(_isActive);
	s.syncAsUint32LE(_zone);
	s.syncAsUint32LE(_loadFrom);
	s.syncAsUint32LE(_imageType);
	s.syncAsUint32LE(_archiveType);
}

#pragma endregion

#pragma endregion

#pragma region Image

ImageSurface::ImageSurface() {
	_surface = NULL;
}

ImageSurface::~ImageSurface() {
	destroy();
}

void ImageSurface::create(byte depth, uint32, uint32 width, uint32 height) {
	if (_surface)
		_surface->free();

	Graphics::PixelFormat format = g_system->getScreenFormat();
	format.bytesPerPixel = depth / 8;

	_surface = new Graphics::Surface();
	_surface->create((uint16)width, (uint16)height, format);
}

void ImageSurface::destroy() {
	if (_surface)
		_surface->free();

	SAFE_DELETE(_surface);
}

bool ImageSurface::load(Common::String filename, ArchiveType type, ZoneId zone, LoadFrom loadFrom, DrawType drawType) {
	if (filename.empty())
		return false;

	debugC(kRingDebugGraphics, "Loading image: %s", filename.c_str());
	_filename = filename;

	ImageLoader *loader = getLoader(filename, type);
	if (!loader)
		error("[ImageSurface::load] Cannot find an image loader for file %s", filename.c_str());

	if (!loader->load(this, type, zone, loadFrom, drawType)) {
		delete loader;

		return false;
	}

	delete loader;

	return true;
}

ImageLoader *ImageSurface::getLoader(Common::String filename, ArchiveType type) const {
	switch (((RingEngine *)g_engine)->getGameType()) {
	default:
	case GameTypeRing:
		break;

	// Games released after Ring uses CI2 compression for all files stored in archives
	case GameTypeFaust:
	case GameTypePompeii:
	case GameTypePilgrim2:
	case GameTypePilgrim3:
	case GameTypeJerusalem:
		if (type == kArchiveArt)
			return new ImageLoaderCI2();
		break;
	}

	ImageLoader *loader = NULL;
	filename.toLowercase();

	// Special case for files with no extension
	if (filename.size() < 4) {
		if (type == kArchiveFile)
			loader = new ImageLoaderBMP();
		else
			loader = new ImageLoaderBMA();
	} else if (filename.hasSuffix(".bmp")) {
		// BMP images in art files are always compressed
		if (type == kArchiveFile)
			loader = new ImageLoaderBMP();
		else
			loader = new ImageLoaderBMA();
	} else if (filename.hasSuffix(".cnm")) {
		if (type == kArchiveArt)
			error("[ImageSurface::load] Archive files do not contains cinematic files (%s)", filename.c_str());

		loader = new ImageLoaderCIN();
	} else if (filename.hasSuffix(".tga")) {
		// TGA images in art files are always compressed
		if (type == kArchiveFile)
			loader = new ImageLoaderTGA();
		else
			loader = new ImageLoaderTGC();
	} else if (filename.hasSuffix(".bma")) {
		loader = new ImageLoaderBMA();
	} else if (filename.hasSuffix(".tgc")) {
		loader = new ImageLoaderTGC();
	} else if (filename.hasSuffix(".ci2")) {
		loader = new ImageLoaderCI2();
	} else {
		// Defaults to BMP or BMA
		if (type == kArchiveFile)
			loader = new ImageLoaderBMP();
		else
			loader = new ImageLoaderBMA();
	}

	return loader;
}

ImageSurface *ImageSurface::zoom(float xZoom, float yZoom) {
	if (!isInitialized())
		error("[ImageSurface::zoom] ImageSurface not initialized!");

	if (getBPP() != 24)
		error("[ImageSurface::zoom] Not a 24bpp image!");

	// Create new image to store zoomed image
	uint32 width = (uint32)(getWidth() * xZoom);
	uint32 height = (uint32)(getHeight() * yZoom);

	ImageSurface *image = new ImageSurface();
	image->create(24, 2, width, height);

	byte *zoomedData = (byte *)image->getSurface()->getBasePtr(0, 0);

	for (uint32 j = 0; j < height; j++) {
		for (uint32 i = 0; i < width; i++) {
			byte *data = (byte *)_surface->getBasePtr((uint32)(i / xZoom), (uint32)(j / yZoom));

			zoomedData[0] = data[0];
			zoomedData[1] = data[1];
			zoomedData[2] = data[2];

			zoomedData += 3;
		}
	}

	return image;
}

Common::Rect ImageSurface::draw(Graphics::Surface *screen, const Common::Point &dest, bool useAlpha) {
	return draw(screen, dest, _surface->w, _surface->h, 0, 0, useAlpha);
}

Common::Rect ImageSurface::draw(Graphics::Surface *screen, const Common::Point &dest, uint32 srcWidth, uint32 srcHeight, int32 srcX, int32 offset, bool useAlpha) {
	if (!_surface)
		error("[ImageSurface::draw] ImageSurface surface not initialized properly");

	// Compute destination rectangle
	Common::Rect destRect(dest.x, dest.y, dest.x + (int16)srcWidth, dest.y + (int16)srcHeight);
	destRect.clip(640, 480);

	//////////////////////////////////////////////////////////////////////////
	// DEBUG: Draw destination rect
	//screen->fillRect(destRect, Color(255, 0, 0).getColor());
	//////////////////////////////////////////////////////////////////////////

	if (!crossBlit((byte *)screen->getBasePtr(dest.x, dest.y),
	               (byte *)_surface->getBasePtr(srcX, 0 + offset),
	               screen->pitch,
	               _surface->pitch,
	               destRect.width(),
	               destRect.height(),
	               offset,
	               screen->format,
	               _surface->format,
	               true,
	               useAlpha))
		error("[ImageSurface::draw] Cannot convert image to proper screen format");

	return destRect;
}

// Function to blit a rect from one color format to another (copied from conversion.cpp)
// Added support for downsampling and copying data from bottom to top (our images are decoded that way)
bool ImageSurface::crossBlit(byte *dst, const byte *src, int dstpitch, int srcpitch, int w, int h, int offset, const Graphics::PixelFormat &dstFmt, const Graphics::PixelFormat &srcFmt, bool topDown, bool useAlpha) {

	// Error out if conversion is impossible
	if ((srcFmt.bytesPerPixel == 1)
	 || (dstFmt.bytesPerPixel == 1)
	 || (!srcFmt.bytesPerPixel)
	 || (!dstFmt.bytesPerPixel))
		return false;

	// Don't perform unnecessary conversion
	if (srcFmt == dstFmt) {
		if (dst == src)
			return true;

		for (int i = 0; i < h; i++) {
			memcpy(dst, src, (size_t)(w * dstFmt.bytesPerPixel));
			dst += dstpitch;
			src += (topDown ? srcpitch : -srcpitch);
		}

		return true;
	}

	// Faster, but larger, to provide optimized handling for each case.
	int srcDelta, dstDelta;
	srcDelta = (srcpitch - w * srcFmt.bytesPerPixel);
	dstDelta = (dstpitch - w * dstFmt.bytesPerPixel);

	// TODO: optimized cases for dstDelta of 0
	uint8 r, g, b, a;
	if (dstFmt.bytesPerPixel == 2) {
		switch (srcFmt.bytesPerPixel) {
		default:
			warning("[ImageSurface::crossBlit] Unsupported bit depth for downsampling to 16bpp (%d)", srcFmt.bytesPerPixel * 8);
			return false;

		case 2:
			for (int y = offset; y < h; y++) {
				for (int x = 0; x < w; x++, src += 2, dst += 2) {
					uint16 color = *(const uint16 *)src;
					srcFmt.colorToARGB(color, a, r, g, b);
					color = (uint16)dstFmt.ARGBToColor(a, r, g, b);
					*(uint16 *)dst = color;
				}
				src += srcDelta + (topDown ? 0 : -srcpitch * 2);
				dst += dstDelta;
			}
			break;

		case 4:
			for (int y = offset; y < h; y++) {
				for (int x = 0; x < w; x++, src += 4, dst += 2) {
					uint32 color = *(const uint32 *)src;
					srcFmt.colorToARGB(color, a, r, g, b);

					// Do not draw when fully transparent
					if (a == 0)
						continue;

					if (useAlpha) {
						uint8 r_dst, g_dst, b_dst, a_dst;
						uint16 color_dst = *(const uint16 *)dst;
						dstFmt.colorToARGB(color_dst, a_dst, r_dst, g_dst, b_dst);

						uint8 ialpha = (255 - a);
						color = dstFmt.RGBToColor((r * a + (r_dst * ialpha)) >> 8,
						                          (g * a + (g_dst * ialpha)) >> 8,
						                          (b * a + (b_dst * ialpha)) >> 8);

						*(uint16 *)dst = (uint16)color;
					} else {
						// Ignore alpha transparency
						color = dstFmt.ARGBToColor(a, r, g, b);
						*(uint16 *)dst = (uint16)color;
					}

				}
				src += srcDelta + (topDown ? 0 : -srcpitch * 2);
				dst += dstDelta;
			}
			break;
		}
	} else if (dstFmt.bytesPerPixel == 3) {
		if (srcFmt.bytesPerPixel > dstFmt.bytesPerPixel)
			error("[ImageSurface::crossBlit] Downsampling to 24bpp not implemented");

		uint32 color;
		uint8 *col = (uint8 *) &color;
#ifdef SCUMM_BIG_ENDIAN
		col++;
#endif
		if (srcFmt.bytesPerPixel == 2) {
			for (int y = offset; y < h; y++) {
				for (int x = 0; x < w; x++, src += 2, dst += 3) {
					color = *(const uint16 *)src;
					srcFmt.colorToARGB(color, a, r, g, b);
					color = dstFmt.ARGBToColor(a, r, g, b);
					memcpy(dst, col, 3);
				}
				src += srcDelta + (topDown ? 0 : -srcpitch * 2);
				dst += dstDelta;
			}
		} else {
			for (int y = offset; y < h; y++) {
				for (int x = 0; x < w; x++, src += 3, dst += 3) {
					memcpy(col, src, 3);
					srcFmt.colorToARGB(color, a, r, g, b);
					color = dstFmt.ARGBToColor(a, r, g, b);
					memcpy(dst, col, 3);
				}
				src += srcDelta + (topDown ? 0 : -srcpitch * 2);
				dst += dstDelta;
			}
		}
	} else if (dstFmt.bytesPerPixel == 4) {
		if (srcFmt.bytesPerPixel > dstFmt.bytesPerPixel) {
			warning("[Image::crossBlit] Downsampling to 32bpp not implemented");
			return false;
		}

		uint32 color;
		if (srcFmt.bytesPerPixel == 2) {
			for (int y = offset; y < h; y++) {
				for (int x = 0; x < w; x++, src += 2, dst += 4) {
					color = *(const uint16 *)src;
					srcFmt.colorToARGB(color, a, r, g, b);
					color = dstFmt.ARGBToColor(a, r, g, b);
					*(uint32 *)dst = color;
				}
				src += srcDelta + (topDown ? 0 : -srcpitch * 2);
				dst += dstDelta;
			}
		} else if (srcFmt.bytesPerPixel == 3) {
			uint8 *col = (uint8 *)&color;
#ifdef SCUMM_BIG_ENDIAN
			col++;
#endif
			for (int y = offset; y < h; y++) {
				for (int x = 0; x < w; x++, src += 2, dst += 4) {
					memcpy(col, src, 3);
					srcFmt.colorToARGB(color, a, r, g, b);
					color = dstFmt.ARGBToColor(a, r, g, b);
					*(uint32 *)dst = color;
				}
				src += srcDelta + (topDown ? 0 : -srcpitch * 2);
				dst += dstDelta;
			}
		} else {
			for (int y = offset; y < h; y++) {
				for (int x = 0; x < w; x++, src += 4, dst += 4) {
					color = *(const uint32 *)src;
					srcFmt.colorToARGB(color, a, r, g, b);
					color = dstFmt.ARGBToColor(a, r, g, b);
					*(uint32 *)dst = color;
				}
				src += srcDelta + (topDown ? 0 : -srcpitch * 2);
				dst += dstDelta;
			}
		}
	} else {
		return false;
	}
	return true;
}

void ImageSurface::setSurface(Graphics::Surface *surface) {
	destroy();

	_surface = surface;
}

#pragma endregion

} // End of namespace Ring
