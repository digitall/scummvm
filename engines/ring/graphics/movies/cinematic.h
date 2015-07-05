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

#ifndef RING_CINEMATIC_H
#define RING_CINEMATIC_H

#include "ring/shared.h"

#include "common/stream.h"

namespace Ring {

class Cinematic : public Common::SeekableReadStream {
public:
	virtual ~Cinematic() {}

	virtual void skipFrame() = 0;
	virtual bool tControl() = 0;
	virtual void setState(bool state) = 0;

	// ReadStream
	bool eos() const override;
	uint32 read(void *dataPtr, uint32 dataSize) override;

	// SeekableReadStream
	int32 pos() const override;
	int32 size() const override;
	bool seek(int32 offset, int whence = SEEK_SET) override;

protected:
	Common::SeekableReadStream *_stream;    ///< The movie file stream
};

} // End of namespace Ring

#endif // RING_CINEMATIC_H
