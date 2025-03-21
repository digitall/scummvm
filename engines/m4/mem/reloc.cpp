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

#include "m4/mem/reloc.h"
#include "common/textconsole.h"

namespace M4 {

struct HR {
	void *_data;
};

MemHandle NewHandle(size_t size, const Common::String &str) {
	HR *result = (HR *)malloc(sizeof(HR));

	if (!result)
		error("Unable to allocate memory - %zd bytes for %s", size, str.c_str());

	result->_data = malloc(size);

	return (MemHandle)result;
}

bool mem_ReallocateHandle(MemHandle h, size_t size, const Common::String &) {
	HR *hr = (HR *)h;
	assert(!hr->_data);
	hr->_data = malloc(size);

	return true;
}

MemHandle MakeNewHandle(size_t size, const Common::String &name) {
	return NewHandle(size, name);
}

void DisposeHandle(MemHandle handle) {
	free(*handle);
	free(handle);
}

} // namespace M4
