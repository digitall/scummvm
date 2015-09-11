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

#include "bolt/pf_file.h"

#include "common/debug.h"

namespace Bolt {

// PF probably stands for "packet file". PF files contain a collection of
// movies, each identified with a four character name. A movie is a stream
// of interleaved sound and video packets.
bool PfFile::load(const Common::String &filename) {

	debug(3, "opening %s", filename.c_str());

	// Open the file
	if (!_file.open(filename)) {
		warning("Failed to open %s", filename.c_str());
		return false;
	}

	// Read and check magic header value
	uint32 magic = _file.readUint32BE();
	if (magic != 0xBEAD9500UL) {
		warning("PF magic header value not found");
		return false;
	}

	// Read number of movies
	uint32 numMovies = _file.readUint32BE();

	// Read name and offset of each movie
	for (uint32 i = 0; i < numMovies; ++i) {
		uint32 name = _file.readUint32BE();
		uint32 offset = _file.readUint32BE();
		debug(3, "%c%c%c%c", (name >> 24) & 0xff, (name >> 16) & 0xff, (name >> 8) & 0xff, name & 0xff);
		_movies[name] = offset;
	}

	return true;
}

Common::File* PfFile::seekMovieAndGetFile(uint32 name) {
	_file.seek(_movies[name]);
	return &_file;
}

} // End of namespace Bolt
