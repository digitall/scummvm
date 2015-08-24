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

#ifndef BOLT_UTIL_H
#define BOLT_UTIL_H

namespace Bolt {

template<class T>
class ScopedArray {
public:
	struct Movable
	{
		Movable() : data(nullptr), size(0) { }
		T* data;
		uint size;
	};

	explicit ScopedArray(Movable o = Movable())
		: _internal(o)
	{ }

	~ScopedArray() {
		delete[] _internal.data;
	}

	operator bool() const {
		return _internal.data;
	}

	uint size() const {
		return _internal.size;
	}

	void reset(Movable o = Movable()) {
		delete[] _internal.data;
		_internal = o;
	}

	void reset(uint size) {
		delete[] _internal.data;
		_internal.data = new T[size];
		_internal.size = size;
	}

	T& operator[](uint idx) {
		return _internal.data[idx];
	}

	const T& operator[](uint idx) const {
		return _internal.data[idx];
	}

	Movable release() {
		Movable result = _internal;
		_internal = Movable();
		return result;
	}

private:
	Movable _internal;
};

} // End of namespace Bolt

#endif
