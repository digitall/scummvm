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

#ifndef RING_AQUATOR_H
#define RING_AQUATOR_H

#include "ring/shared.h"

#include "common/rect.h"

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

	void reset();
	void *allocBuffer(bool hasAdditionnalData) const;
	void initData();
	void copyToSurface(int16 *data, int width, int height, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, byte *surface, int pitch);
};

class ImageHeader {
public:
	ImageHeader();
	~ImageHeader();

	void reset();

	void init(Common::SeekableReadStream *stream);
	void update(ImageHeaderEntry* entry);

	bool hasEntries() { return _entries.size() != 0; }
	uint32 getField4() { return _field_4; }
	ImageHeaderEntry *get(uint32 index);
	ImageHeaderEntry *getCurrent() { return _current; }

private:
	uint32 _field_4;
	Common::Array<ImageHeaderEntry *> _entries;
	ImageHeaderEntry *_current;
	int32 _field_4C;
};

class AquatorImageHeader {
public:
	AquatorImageHeader();
	~AquatorImageHeader();

	void setField0(bool state) { _field_0 = state; }
	void setField4(uint32 val) { _field_4 = val; }
	void setField8(uint32 val) { _field_8 = val; }
	void setChannel(uint32 val);

	bool getField0() { return _field_0; }
	uint32 getField4() { return _field_4; }
	uint32 getField8() { return _field_8; }
	ImageHeader *getHeader() { return _header; }
	uint32 getChannel() { return _channel; }

private:
	bool _field_0;
	uint32 _field_4;
	uint32 _field_8;
	ImageHeader *_header;
	uint32 _channel;
};

class AquatorStream {
public:
	AquatorStream(uint32 count, Common::String path);
	~AquatorStream();

	void alloc(bool isCompressed, const Graphics::PixelFormat &format, uint32 size);
	void dealloc();

	uint32 sub_410F50(uint32 index);
	void updateEntries(float timeOffset);
	void sub_411530(uint32 index, uint32 a2);
	void setChannel(uint32 index, uint32 channel);

	ImageHeaderEntry *getEntry() { return _entry; }
	uint32 getCount() { return _headers.size(); }
	uint32 getChannel(uint32 index);

	bool isInitialized() { return _entry->isInitialized(); }

private:
	Common::Array<AquatorImageHeader *> _headers;
	Common::String _path;
	ImageHeaderEntry *_entry;

	void initNode(Common::SeekableReadStream *stream, const Graphics::PixelFormat &format);
	void initChannel(Common::SeekableReadStream *stream, uint32 index);

	// Decompressed aquator
	void initStream(uint32 index);
};

} // End of namespace Ring

#endif // RING_AQUATOR_H
