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
	typedef Common::Array<T>* Movable;

	ScopedArray(Movable o = nullptr)
		: _ptr(o)
	{ }

	operator bool() const {
		return _ptr && !_ptr->empty();
	}

	uint size() const {
		assert(_ptr);
		return _ptr->size();
	}

	void reset(Movable o = nullptr) {
		_ptr.reset(o);
	}

	void resize(uint sz) {
		if (!_ptr) {
			_ptr.reset(new Common::Array<T>());
		}
		_ptr->resize(sz);
	}

	T& operator[](uint idx) {
		assert(*this);
		return (*_ptr)[idx];
	}

	const T& operator[](uint idx) const {
		assert(*this);
		return (*_ptr)[idx];
	}

	Movable release() {
		return _ptr.release();
	}

private:
	typedef Common::Array<T> InternalArray;
	typedef Common::ScopedPtr<InternalArray> InternalPtr;
	InternalPtr _ptr;
};

} // End of namespace Bolt

#endif
