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

#ifndef RING_MOVIE_H
#define RING_MOVIE_H

#include "ring/shared.h"

#include "common/rect.h"

namespace Ring {

class CinematicSound;
class ImageLoaderMovie;
class ScreenManager;

class Movie {
public:
	Movie(ScreenManager *screen);
	~Movie();

	bool init(const Common::String &path, Common::String filename, const Common::String &languageFolder, uint32 channel);
	void deinit();

	void play(const Common::Point &point);

	uint32 playNextFrame(const Common::Point &point, DrawType drawType);
	uint32 getNumberOfFrames();

	void setSynchroOff();

	void setFramerate(float rate) { _framerate = rate; }

private:
	enum ChunkType {
		kChunkA = 65,
		kChunkB = 66,
		kChunkS = 83,
		kChunkT = 84,
		kChunkU = 85,
		kChunkZ = 90
	};

	ImageLoaderMovie *_image;
	ScreenManager    *_screen;
	CinematicSound   *_sound;
	bool              _isSoundInitialized;
	bool              _enableFrameSkipping;
	float             _framerate;
	bool              _hasDialog;
	uint32            _channel;
	bool              _isCI2;

	// Sound
	bool readSound();
	bool skipSound();
};

} // End of namespace Ring

#endif // RING_MOVIE_H
