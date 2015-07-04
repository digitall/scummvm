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

#include "ring/base/preferences.h"

#include "ring/base/application.h"
#include "ring/base/dialog.h"

#include "ring/sound/soundhandler.h"

#include "engines/engine.h"

#include "common/archive.h"
#include "common/config-manager.h"

namespace Ring {

PreferenceHandler::PreferenceHandler(Application *application) : _app(application) {
	_volume = 0;
	_volumeDialog = 0;
	_reverseStereo = false;
	_showSubtitles = false;
}

PreferenceHandler::~PreferenceHandler() {
	// Zero-out passed pointers
	_app = nullptr;
}

void PreferenceHandler::load() {
	loadDefaults();

	// Register default values
	ConfMan.registerDefault("music_volume", _volume);
	ConfMan.registerDefault("speech_volume", _volumeDialog);
	ConfMan.registerDefault("subtitles", _showSubtitles);
	ConfMan.registerDefault("reverse_stereo", _reverseStereo);

	// Get values from ScummVM preferences
	_volume = ConfMan.getInt("music_volume");
	_volumeDialog = ConfMan.getInt("speech_volume");
	_showSubtitles = ConfMan.getBool("subtitles");
	_reverseStereo = ConfMan.getInt("reverse_stereo");

	// We need to convert volume from ScummVM values (0-255) to in-game values (0-100)
	_volume       = (int32)(_volume * 100.0f/255.0f);
	_volumeDialog = (int32)(_volumeDialog * 100.0f/255.0f);

	_volume       = CLIP(_volume, 0, 100);
	_volumeDialog = CLIP(_volumeDialog, 0, 100);
}

void PreferenceHandler::loadDefaults() {
	// First, try opening per-user preferences
	// TODO: Get per-user preference file name
	Common::SeekableReadStream *archive = SearchMan.createReadStreamForMember("DATA/SAVE/aPre.ini");
	if (!archive) {
		warning("[PreferenceHandler::load] Cannot found per-user configuration file (aPre.ini)");

		// Open a stream to the global configuration file
		archive = SearchMan.createReadStreamForMember("aPre.ini");
		if (!archive)
			error("[PreferenceHandler::load] Error opening global configuration file (aPre.ini)");
	}

	Common::String line = archive->readLine();
	if (archive->err()) {
		delete archive;
		error("[PreferenceHandler::load] Error reading from file");
	}

	delete archive;

	// Read preferences
	int32 volume;
	int32 volumeDialog;
	int32 reverseStereo;
	int32 showSubtitles;
	if (sscanf(line.c_str(), "%d %d %d %d", &volume, &volumeDialog, &reverseStereo, &showSubtitles) != 4)
		error("[PreferenceHandler::load] Invalid configuration format");

	set(volume, volumeDialog, reverseStereo, (bool)showSubtitles);
	setup();
}

void PreferenceHandler::save(int32 volume, int32 volumeDialog, int32 reverseStereo, bool showSubtitles) {
	set(volume, volumeDialog, reverseStereo, showSubtitles);
	setup();

	// We need to convert volume values (0-100) to ScummVM values (0-255)
	ConfMan.setInt("music_volume", (int)(_volume * 255.0f/100.0f));
	ConfMan.setInt("speech_volume", (int)(_volumeDialog * 255.0f/100.0f));
	ConfMan.setBool("subtitles", _showSubtitles);
	ConfMan.setInt("reverse_stereo", _reverseStereo);
}

void PreferenceHandler::set(int32 volume, int32 volumeDialog, int32 reverseStereo, bool showSubtitles) {
	_volume = volume;
	_volumeDialog = volumeDialog;
	_reverseStereo = reverseStereo;
	_showSubtitles = showSubtitles;
}

void PreferenceHandler::setup() {
	_app->soundSetMultiplierIfNotType(kSoundTypeDialog, _volume);
	_app->soundSetMultiplier(kSoundTypeDialog, _volumeDialog);

	if (_app->getSoundHandler())
		_app->getSoundHandler()->setReverseStereo(_reverseStereo);

	if (_app->getDialogHandler())
		_app->getDialogHandler()->showSubtitles(_showSubtitles);
}

} // End of namespace Ring
