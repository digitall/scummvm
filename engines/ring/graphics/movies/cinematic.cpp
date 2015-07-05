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

#include "ring/graphics/movies/cinematic.h"

namespace Ring {

#pragma region ReadStream

bool Cinematic::eos() const {
	if (!_stream)
		error("[Cinematic::eos] Not initialized properly!");

	return _stream->eos();
}

uint32 Cinematic::read(void *dataPtr, uint32 dataSize) {
	if (!_stream)
		error("[Cinematic::read] Not initialized properly!");

	return _stream->read(dataPtr, dataSize);
}

#pragma endregion

#pragma region SeekableReadStream

int32 Cinematic::pos() const {
	if (!_stream)
		error("[Cinematic::pos] Not initialized properly!");

	return _stream->pos();
}

int32 Cinematic::size() const {
	if (!_stream)
		error("[Cinematic::size] Not initialized properly!");

	return _stream->size();
}

bool Cinematic::seek(int32 offset, int whence) {
	if (!_stream)
		error("[Cinematic::seek] Not initialized properly!");

	return _stream->seek(offset, whence);
}

#pragma endregion

} // End of namespace Ring
