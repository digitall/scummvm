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

#ifndef RING_IMAGE_HEADER_ENTRY_H
#define RING_IMAGE_HEADER_ENTRY_H

#include "ring/shared.h"

namespace Ring {

class ImageHeaderEntry {
public:
	struct Header {
		int32 field_0;
		int32 field_4;
		int32 field_8;
		int32 field_C;
		float field_10;
		float field_14;
		float field_18;
		int32 field_1C;
		int32 field_20;
		int32 field_24;
		int32 field_28;
		int32 field_2C;
		int32 field_30;

		Header();
		void update(const Header &header);
		void load(Common::SeekableReadStream *stream);
	};

	ImageHeaderEntry();
	~ImageHeaderEntry();

	void init(Common::SeekableReadStream *stream, bool hasAdditionnalData);
	void init(ImageHeaderEntry *entry);
	void update(ImageHeaderEntry *entry, bool updateCaller = true);
	void reset();

	// Buffer
	void prepareBuffer();
	void process();
	void drawBuffer(Graphics::Surface *surface);
	void computeCoordinates(Common::Point *point);
	void adjustCoordinates(Common::Point *point);
	void updateData(float a1, float a2, float a3, float a4, float a5, float a6, float a7, float a8);

	bool isInitialized() { return _buffer != nullptr; }
	const Header &getHeader() { return _header; }
	void *getBuffer() { return _buffer; }

private:
	struct InternalData {
		float field_0;
		float field_4;
		float field_8;
		float field_C;
		float field_10;
		float field_14;
		float field_18;
		float field_1C;

		InternalData() {
			field_0  = 0;
			field_4  = 0;
			field_8  = 0;
			field_C  = 0;
			field_10 = 0;
			field_14 = 0;
			field_18 = 0;
			field_1C = 0;
		}

		void init(float a1, float a2, float a3, float a4, float a5, float a6);
	};

	Header  _header;
	void   *_buffer;
	void   *_bufferData;
	bool    _hasAdditionnalData;

	void *allocBuffer(bool hasAdditionnalData) const;
	void initData();
	void copyToSurface(int16 *data, int width, int height, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, byte *surface, int pitch);
};

} // End of namespace Ring

#endif // RING_IMAGE_HEADER_ENTRY_H
