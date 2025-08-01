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

#ifndef GRAPHICS_BLIT_H
#define GRAPHICS_BLIT_H

#include "graphics/pixelformat.h"
#include "graphics/transform_struct.h"

namespace Common {
struct Point;
}

class BlendBlitUnfilteredTestSuite;

namespace Graphics {

/**
 * @defgroup graphics_blit Blitting
 * @ingroup graphics
 *
 * @brief Graphics blitting operations.
 *
 * @{
 */

struct TransformStruct;

/** Converting a palette for use with crossBlitMap(). */
inline static void convertPaletteToMap(uint32 *dst, const byte *src, uint colors, const Graphics::PixelFormat &format) {
	while (colors-- > 0) {
		*dst++ = format.RGBToColor(src[0], src[1], src[2]);
		src += 3;
	}
}

/**
 * Blits a rectangle.
 * Cautions: 
 *  source & destination buffers must have same bpp
 *  blit function has no protection against buffer overruns; w.bpp & h must not exceed respective byte dimensions (pitch & height) of either buffer
 *
 * @param dst			the buffer which will receive the converted graphics data
 * @param src			the buffer containing the original graphics data
 * @param dstPitch		width in bytes of one full line of the dest buffer
 * @param srcPitch		width in bytes of one full line of the source buffer
 * @param w				the width of the graphics data
 * @param h				the height of the graphics data
 * @param bytesPerPixel	the number of bytes per pixel
 */
void copyBlit(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint w, const uint h,
			   const uint bytesPerPixel);

/**
 * Blits a rectangle with a transparent color key.
 *
 * @param dst			the buffer which will receive the converted graphics data
 * @param src			the buffer containing the original graphics data
 * @param dstPitch		width in bytes of one full line of the dest buffer
 * @param srcPitch		width in bytes of one full line of the source buffer
 * @param w				the width of the graphics data
 * @param h				the height of the graphics data
 * @param bytesPerPixel	the number of bytes per pixel
 * @param key			the transparent color key
 */
bool keyBlit(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint w, const uint h,
			   const uint bytesPerPixel, const uint32 key);

/**
 * Blits a rectangle with a transparent color mask.
 *
 * A mask is a separate CLUT8 surface where for each pixel in the mask,
 * 0 means the corresponding pixel in the source is transparent, while
 * a non-zero value means that the corresponding pixel is opaque.
 *
 * @param dst			the buffer which will receive the converted graphics data
 * @param src			the buffer containing the original graphics data
 * @param mask			the buffer containing the mask
 * @param dstPitch		width in bytes of one full line of the dest buffer
 * @param srcPitch		width in bytes of one full line of the source buffer
 * @param maskPitch		width in bytes of one full line of the mask buffer
 * @param w				the width of the graphics data
 * @param h				the height of the graphics data
 * @param bytesPerPixel	the number of bytes per pixel
 */
bool maskBlit(byte *dst, const byte *src, const byte *mask,
			   const uint dstPitch, const uint srcPitch, const uint maskPitch,
			   const uint w, const uint h,
			   const uint bytesPerPixel);

/**
 * Blits a rectangle from one graphical format to another.
 *
 * @param dst		the buffer which will receive the converted graphics data
 * @param src		the buffer containing the original graphics data
 * @param dstPitch	width in bytes of one full line of the dest buffer
 * @param srcPitch	width in bytes of one full line of the source buffer
 * @param w			the width of the graphics data
 * @param h			the height of the graphics data
 * @param dstFmt	the desired pixel format
 * @param srcFmt	the original pixel format
 * @return			true if conversion completes successfully,
 *					false if there is an error.
 *
 * @note This can convert a surface in place, regardless of the
 *       source and destination format, as long as there is enough
 *       space for the destination. The dstPitch / srcPitch ratio
 *       must at least equal the dstBpp / srcBpp ratio for
 *       dstPitch >= srcPitch and at most dstBpp / srcBpp for
 *       dstPitch < srcPitch though.
 */
bool crossBlit(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint w, const uint h,
			   const Graphics::PixelFormat &dstFmt, const Graphics::PixelFormat &srcFmt);

/**
 * Blits a rectangle from one graphical format to another with a transparent color key.
 *
 * @param dst		the buffer which will receive the converted graphics data
 * @param src		the buffer containing the original graphics data
 * @param dstPitch	width in bytes of one full line of the dest buffer
 * @param srcPitch	width in bytes of one full line of the source buffer
 * @param w			the width of the graphics data
 * @param h			the height of the graphics data
 * @param dstFmt	the desired pixel format
 * @param srcFmt	the original pixel format
 * @param key			the transparent color key in the original pixel format
 * @return			true if conversion completes successfully,
 *					false if there is an error.
 *
 * @note This can convert a surface in place, regardless of the
 *       source and destination format, as long as there is enough
 *       space for the destination. The dstPitch / srcPitch ratio
 *       must at least equal the dstBpp / srcBpp ratio for
 *       dstPitch >= srcPitch and at most dstBpp / srcBpp for
 *       dstPitch < srcPitch though.
 */
bool crossKeyBlit(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint w, const uint h,
			   const Graphics::PixelFormat &dstFmt, const Graphics::PixelFormat &srcFmt, const uint32 key);

/**
 * Blits a rectangle from one graphical format to another with a transparent color mask.
 *
 * A mask is a separate CLUT8 surface where for each pixel in the mask,
 * 0 means the corresponding pixel in the source is transparent, while
 * a non-zero value means that the corresponding pixel is opaque.
 *
 * @param dst		the buffer which will receive the converted graphics data
 * @param src		the buffer containing the original graphics data
 * @param mask		the buffer containing the mask
 * @param dstPitch	width in bytes of one full line of the dest buffer
 * @param srcPitch	width in bytes of one full line of the source buffer
 * @param maskPitch	width in bytes of one full line of the mask buffer
 * @param w			the width of the graphics data
 * @param h			the height of the graphics data
 * @param dstFmt	the desired pixel format
 * @param srcFmt	the original pixel format
 * @return			true if conversion completes successfully,
 *					false if there is an error.
 *
 * @note This can convert a surface in place, regardless of the
 *       source and destination format, as long as there is enough
 *       space for the destination. The dstPitch / srcPitch ratio
 *       must at least equal the dstBpp / srcBpp ratio for
 *       dstPitch >= srcPitch and at most dstBpp / srcBpp for
 *       dstPitch < srcPitch though.
 */
bool crossMaskBlit(byte *dst, const byte *src, const byte *mask,
			   const uint dstPitch, const uint srcPitch, const uint maskPitch,
			   const uint w, const uint h,
			   const Graphics::PixelFormat &dstFmt, const Graphics::PixelFormat &srcFmt);

bool crossBlitMap(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint w, const uint h,
			   const uint bytesPerPixel, const uint32 *map);

bool crossKeyBlitMap(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint w, const uint h,
			   const uint bytesPerPixel, const uint32 *map, const uint32 key);

bool crossMaskBlitMap(byte *dst, const byte *src, const byte *mask,
			   const uint dstPitch, const uint srcPitch, const uint maskPitch,
			   const uint w, const uint h,
			   const uint bytesPerPixel, const uint32 *map);

typedef void (*FastBlitFunc)(byte *, const byte *, const uint, const uint, const uint, const uint);

/**
 * Look up optimised routines for converting between pixel formats.
 *
 * @param dstFmt	the desired pixel format
 * @param srcFmt	the original pixel format
 * @return			a function pointer to an optimised routine,
 *					or nullptr if none are available.
 *
 * @note Not all combinations of pixel formats are supported on
 *       all platforms. Users of this function should provide a
 *       fallback using crossBlit() if no optimised functions
 *       can be found.
 * @note This can convert a surface in place, regardless of the
 *       source and destination format, as long as there is enough
 *       space for the destination. The dstPitch / srcPitch ratio
 *       must at least equal the dstBpp / srcBpp ratio for
 *       dstPitch >= srcPitch and at most dstBpp / srcBpp for
 *       dstPitch < srcPitch though.
 */
FastBlitFunc getFastBlitFunc(const PixelFormat &dstFmt, const PixelFormat &srcFmt);

bool scaleBlit(byte *dst, const byte *src,
			   const uint dstPitch, const uint srcPitch,
			   const uint dstW, const uint dstH,
			   const uint srcW, const uint srcH,
			   const Graphics::PixelFormat &fmt,
			   const byte flip = 0);

bool scaleBlitBilinear(byte *dst, const byte *src,
					   const uint dstPitch, const uint srcPitch,
					   const uint dstW, const uint dstH,
					   const uint srcW, const uint srcH,
					   const Graphics::PixelFormat &fmt,
					   const byte flip = 0);

bool rotoscaleBlit(byte *dst, const byte *src,
				   const uint dstPitch, const uint srcPitch,
				   const uint dstW, const uint dstH,
				   const uint srcW, const uint srcH,
				   const Graphics::PixelFormat &fmt,
				   const TransformStruct &transform,
				   const Common::Point &newHotspot);

bool rotoscaleBlitBilinear(byte *dst, const byte *src,
						   const uint dstPitch, const uint srcPitch,
						   const uint dstW, const uint dstH,
						   const uint srcW, const uint srcH,
						   const Graphics::PixelFormat &fmt,
						   const TransformStruct &transform,
						   const Common::Point &newHotspot);

bool applyColorKey(byte *dst, const byte *src,
                   const uint dstPitch, const uint srcPitch,
                   const uint w, const uint h,
                   const Graphics::PixelFormat &format, const bool overwriteAlpha,
                   const uint8 rKey, const uint8 gKey, const uint8 bKey,
                   const uint8 rNew, const uint8 gNew, const uint8 bNew);

bool setAlpha(byte *dst, const byte *src,
              const uint dstPitch, const uint srcPitch,
              const uint w, const uint h,
              const Graphics::PixelFormat &format,
              const bool skipTransparent, const uint8 alpha);

// This is a class so that we can declare certain things as private
class BlendBlit {
private:
	struct Args {
		bool rgbmod, alphamod;
		int xp, yp;
		int inStep, inoStep;
		const byte *ino;
		byte *outo;
	
		int scaleX, scaleY, scaleXoff, scaleYoff;
		uint dstPitch;
		uint width, height;
		uint32 color;
		int flipping;
		
		Args(byte *dst, const byte *src,
			 const uint dstPitch, const uint srcPitch,
			 const int posX, const int posY,
			 const uint width, const uint height,
			 const int scaleX, const int scaleY,
			 const int scaleXsrcOff, const int scaleYsrcOff,
			 const uint32 colorMod, const uint flipping);
	};

#ifdef SCUMMVM_NEON
	static void blitNEON(Args &args, const TSpriteBlendMode &blendMode, const AlphaType &alphaType);
#endif
#ifdef SCUMMVM_SSE2
	static void blitSSE2(Args &args, const TSpriteBlendMode &blendMode, const AlphaType &alphaType);
#endif
#ifdef SCUMMVM_AVX2
	static void blitAVX2(Args &args, const TSpriteBlendMode &blendMode, const AlphaType &alphaType);
#endif
	static void blitGeneric(Args &args, const TSpriteBlendMode &blendMode, const AlphaType &alphaType);
	template<class T>
	static void blitT(Args &args, const TSpriteBlendMode &blendMode, const AlphaType &alphaType);

	typedef void(*BlitFunc)(Args &, const TSpriteBlendMode &, const AlphaType &);
	static BlitFunc blitFunc;

	static void fillGeneric(Args &args, const TSpriteBlendMode &blendMode);
	template<class T>
	static void fillT(Args &args, const TSpriteBlendMode &blendMode);

	typedef void(*FillFunc)(Args &, const TSpriteBlendMode &);
	static FillFunc fillFunc;

	friend class ::BlendBlitUnfilteredTestSuite;
	friend class BlendBlitImpl_Default;
	friend class BlendBlitImpl_NEON;
	friend class BlendBlitImpl_SSE2;
	friend class BlendBlitImpl_AVX2;

public:
	static const int SCALE_THRESHOLD = 0x100;
	static const int kBModShift = 8;
	static const int kGModShift = 16;
	static const int kRModShift = 24;
	static const int kAModShift = 0;
	
	static const uint32 kBModMask = 0x0000ff00;
	static const uint32 kGModMask = 0x00ff0000;
	static const uint32 kRModMask = 0xff000000;
	static const uint32 kAModMask = 0x000000ff;
	static const uint32 kRGBModMask = (kRModMask | kGModMask | kBModMask);
	
#ifdef SCUMM_LITTLE_ENDIAN
	static const int kAIndex = 0;
	static const int kBIndex = 1;
	static const int kGIndex = 2;
	static const int kRIndex = 3;
#else
	static const int kAIndex = 3;
	static const int kBIndex = 2;
	static const int kGIndex = 1;
	static const int kRIndex = 0;
#endif

	static inline int getScaleFactor(int srcSize, int dstSize) {
		return SCALE_THRESHOLD * srcSize / dstSize;
	}

	/**
	 * Returns the pixel format all operations of BlendBlit::blit support.
	 *
	 * Use MS_ARGB and MS_RGB to quickly make a color in this format.
	 * MS_ARGB/RGB are found in graphics/transform_struct.h
	 *
	 * @return Supported pixel format.
	 */
	static inline PixelFormat getSupportedPixelFormat() {
		return PixelFormat(4, 8, 8, 8, 8, 24, 16, 8, 0);
	}

	/**
	 * Optimized version of doBlit to be used with alpha blended blitting
	 * NOTE: Can only be used with BlendBlit::getSupportedPixelFormat format
	 * @param dst a pointer to the destination buffer (can be offseted by pixels)
	 * @param src a pointer to the source buffer (can be offseted by pixels)
	 * @param dstPitch destination pitch
	 * @param srcPitch source pitch
	 * @param posX where src will be blitted to (onto dest)
	 * @param posY where src will be blitted to (onto dest)
	 * @param width width of the input surface
	 * @param height height of the input surface
	 * @param scaleX scale factor to use when blitting (src / dst) (0.5 for 2x scale) use BlendBlit::SCALE_THRESHOLD
	 * @param scaleY scale factor to use when blitting (src / dst) (0.5 for 2x scale) use BlendBlit::SCALE_THRESHOLD
	 * @param scaleXsrcOff since you can only offset the *src pointer to effectivly
	 *     get a different part of the source image rendered, it can only go in
	 *     1 pixel chunks, so this fixes that by added a little offset
	 * @param scaleYsrcOff same as the X one
	 * @param colorMod the color to multiply by. (0xffffffff does no multiplication and has 0 overhead usually)
	 * @param flipping flipping flags used with Graphics::FLIP_FLAGS
	 * @param blendMode the blending mode to be used
	 * @param alphaType the alpha mixing mode to be used
	 */
	static void blit(byte *dst, const byte *src,
			  const uint dstPitch, const uint srcPitch,
			  const int posX, const int posY,
			  const uint width, const uint height,
			  const int scaleX, const int scaleY,
			  const int scaleXsrcOff, const int scaleYsrcOff,
			  const uint32 colorMod, const uint flipping,
			  const TSpriteBlendMode blendMode,
			  const AlphaType alphaType);

	/**
	 * Optimized version of doFill to be used with alpha blended fills
	 * NOTE: Can only be used with BlendBlit::getSupportedPixelFormat format
	 * @param dst a pointer to the destination buffer (can be offseted by pixels)
	 * @param dstPitch destination pitch
	 * @param width width of the input surface
	 * @param height height of the input surface
	 * @param colorMod the color to multiply by. (0xffffffff does no multiplication and has 0 overhead usually)
	 * @param blendMode the blending mode to be used
	 */
	static void fill(byte *dst, const uint dstPitch,
			  const uint width, const uint height,
			  const uint32 colorMod,
			  const TSpriteBlendMode blendMode);

}; // End of class BlendBlit

/** @} */
} // End of namespace Graphics

#endif // GRAPHICS_BLIT_H
