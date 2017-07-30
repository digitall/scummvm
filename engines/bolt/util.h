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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef BOLT_UTIL_H
#define BOLT_UTIL_H

#include "common/queue.h"
#include "common/endian.h"
#include "common/textconsole.h"

namespace Bolt {

// DataView's:
//
// Lightweight classes for safely reading data fields out of an array of bytes,
// inspired by JavaScript DataView's.
//
// These classes do not own the data they point to - the data must be kept alive
// while the DataView is in use, and the data will not be freed when the DataView
// is destroyed.
//
// DataViews with size 0 are allowed, but they cannot be read from or written to.

// DataView with dynamic size that can only be read
class ConstDataView {
public:
	ConstDataView(const byte* const data, const uint dataSize)
		: _data(data), _dataSize(dataSize)
	{ }

	const byte* ptr() const {
		return _data;
	}

	uint size() const {
		return _dataSize;
	}

	// TODO

private:
	const byte* _data;
	uint _dataSize;
};

// DataView with static size that can only be read
template<uint S>
class ConstSizedDataView {
public:
	ConstSizedDataView(const ConstDataView &dv, const uint offset = 0)
		: _data(&dv.ptr()[offset])
	{
		// Don't "assert" here. This error can be caused by bad data files, not
		// necessarily by a programming error in ScummVM. Therefore, this check
		// should occur in Release builds.
		if ((offset + S) > dv.size()) {
			error("Data access out of bounds");
		}
	}

	const byte* ptr() const {
		return _data;
	}

	uint size() const {
		return S;
	}

	ConstDataView slice(const uint offset = 0) const {
		assert(offset <= S);
		return ConstDataView(&_data[offset], S - offset);
	}

	int8 readInt8(const uint offset = 0) const {
		assert((offset + sizeof(int8)) <= S);
		return _data[offset];
	}

	uint8 readUint8(const uint offset = 0) const {
		assert((offset + sizeof(uint8)) <= S);
		return _data[offset];
	}

	int16 readInt16BE(const uint offset = 0) const {
		assert((offset + sizeof(int16)) <= S);
		return READ_BE_INT16(&_data[offset]);
	}

	uint16 readUint16BE(const uint offset = 0) const {
		assert((offset + sizeof(uint16)) <= S);
		return READ_BE_UINT16(&_data[offset]);
	}

	uint32 readUint32BE(const uint offset = 0) const {
		assert((offset + sizeof(uint32)) <= S);
		return READ_BE_UINT32(&_data[offset]);
	}

private:
	const byte* _data;
};

template<class T>
class ScopedArray {
public:
	struct Movable
	{
		friend class ScopedArray;
	public:
		Movable() : data(nullptr), size(0) { }
	private:
		T* data;
		uint size;
	};

	explicit ScopedArray(Movable o = Movable())
		: _internal(o)
	{ }

	~ScopedArray() {
		delete[] _internal.data;
		_internal.data = nullptr;
	}

	operator bool() const {
		return _internal.data;
	}

	uint size() const {
		return _internal.size;
	}

	void reset(Movable o = Movable()) {
		if (o.data != _internal.data) {
			delete[] _internal.data;
			_internal = o;
		}
	}

	void alloc(uint arraySize) {
		delete[] _internal.data;
		_internal.data = nullptr;
		_internal.size = arraySize;
		if (arraySize > 0) {
			_internal.data = new T[arraySize];
		}
	}

	T& operator[](uint idx) {
		assert(idx < _internal.size);
		return _internal.data[idx];
	}

	const T& operator[](uint idx) const {
		assert(idx < _internal.size);
		return _internal.data[idx];
	}

	Movable release() {
		Movable result = _internal;
		_internal = Movable();
		return result;
	}

	ConstDataView slice(const uint offset = 0) const {
		assert(offset <= _internal.size);
		return ConstDataView(&_internal.data[offset], _internal.size - offset);
	}

private:
	// Prevent accidentally copying a ScopedArray
	// XXX: Class is made noncopyable here (instead of inheriting from
	// Common::NonCopyable) because VS2015 emits very unhelpful error messages.
	ScopedArray(const ScopedArray&);
	ScopedArray& operator=(const ScopedArray&);

	Movable _internal;
};

template<class T>
class ScopedArrayQueue
{
public:
	ScopedArrayQueue() { }
	~ScopedArrayQueue() {
		// Correctly delete all contents
		while (!_queue.empty()) {
			ScopedArray<T> item(_queue.pop());
			item.reset();
		}
	}

	bool empty() const {
		return _queue.empty();
	}

	void clear() {
		_queue.clear();
	}

	void push(typename ScopedArray<T>::Movable item) {
		_queue.push(item);
	}

	typename ScopedArray<T>::Movable pop() {
		return _queue.pop();
	}

private:
	ScopedArrayQueue(const ScopedArrayQueue&);
	ScopedArrayQueue& operator=(const ScopedArrayQueue&);

	Common::Queue<typename ScopedArray<T>::Movable> _queue;
};

} // End of namespace Bolt

#endif
