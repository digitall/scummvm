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

#ifndef BOLT_PF_FILE_H
#define BOLT_PF_FILE_H

#include "audio/mixer.h"
#include "common/file.h"

namespace Bolt {

class Graphics;

class PfFile {
public:
	bool load(const Common::String &filename);
	Common::File* seekMovieAndGetFile(uint32 name);

private:
	Common::File _file;
	Common::HashMap<uint32, uint32> _movies; // map 4-char movie names to file offsets
};

} // End of namespace Bolt

#endif
