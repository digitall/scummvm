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

#include "glk/zcode/detection.h"
#include "glk/zcode/detection_tables.h"
#include "glk/zcode/quetzal.h"
#include "glk/blorb.h"
#include "common/debug.h"
#include "common/file.h"
#include "common/md5.h"
#include "common/translation.h"

namespace Glk {
namespace ZCode {

void ZCodeMetaEngine::getSupportedGames(PlainGameList &games) {
	for (const PlainGameDescriptor *pd = INFOCOM_GAME_LIST; pd->gameId; ++pd)
		games.push_back(*pd);
	for (const PlainGameDescriptor *pd = ZCODE_GAME_LIST; pd->gameId; ++pd)
		games.push_back(*pd);
}

const GlkDetectionEntry* ZCodeMetaEngine::getDetectionEntries() {
	static Common::Array<GlkDetectionEntry> entries;
	for (const FrotzGameDescription *entry = FROTZ_GAMES; entry->_gameId; ++entry) {
		GlkDetectionEntry detection = {
			entry->_gameId,
			entry->_extra,
			entry->_md5,
			entry->_filesize,
			entry->_language,
			Common::kPlatformUnknown
		};
		entries.push_back(detection);
	}

	entries.push_back({nullptr,
					   nullptr,
					   nullptr,
					   0,
					   Common::UNK_LANG,
					   Common::kPlatformUnknown});

	return entries.data();
}

GameDescriptor ZCodeMetaEngine::findGame(const char *gameId) {
	for (const PlainGameDescriptor *pd = INFOCOM_GAME_LIST; pd->gameId; ++pd) {
		if (!strcmp(gameId, pd->gameId)) {
			GameDescriptor gd(*pd);
			gd._options |= OPTION_INFOCOM;

			if (!strcmp(gameId, "questforexcalibur") ||
				!strcmp(gameId, "journey") ||
				!strcmp(gameId, "shogun") ||
				!strcmp(gameId, "zork0"))
				gd._supportLevel = kUnstableGame;

			return gd;
		}
	}
	for (const PlainGameDescriptor *pd = ZCODE_GAME_LIST; pd->gameId; ++pd) {
		if (!strcmp(gameId, pd->gameId)) {
			GameDescriptor gd = *pd;
			/*
			 * Tested against ScummVM 2.8.0git, following entries are confirmed not to be playable
			 */
			if (!strcmp(gameId, "bureaucrocy_zcode") ||
				!strcmp(gameId, "scopa") ||
				!strcmp(gameId, "sunburst"))
				gd._supportLevel = kUnstableGame;

			return gd;
		}
	}

	return GameDescriptor::empty();
}

bool ZCodeMetaEngine::detectGames(const Common::FSList &fslist, DetectedGames &gameList) {
	const char *const EXTENSIONS[] = { ".z1", ".z2", ".z3", ".z4", ".z5", ".z6", ".z7", ".z8",
		".dat", ".data", ".zip", nullptr };

	// Loop through the files of the folder
	for (Common::FSList::const_iterator file = fslist.begin(); file != fslist.end(); ++file) {
		// Check for a recognised filename
		if (file->isDirectory())
			continue;
		Common::String filename = file->getName();
		bool hasExt = Blorb::hasBlorbExt(filename), isBlorb = false;
		for (const char *const *ext = &EXTENSIONS[0]; *ext && !hasExt; ++ext)
			hasExt = filename.hasSuffixIgnoreCase(*ext);
		if (!hasExt)
			continue;

		// Open up the file and calculate the md5, and get the serial
		Common::File gameFile;
		if (!gameFile.open(*file))
			continue;
		Common::String md5 = Common::computeStreamMD5AsString(gameFile, 5000);
		size_t filesize = gameFile.size();
		char serial[9] = "";
		bool emptyBlorb = false;
		gameFile.seek(0);
		isBlorb = Blorb::isBlorb(gameFile, ID_ZCOD);

		if (!isBlorb) {
			if (Blorb::hasBlorbExt(filename)) {
				gameFile.close();
				continue;
			}
			gameFile.seek(18);
			Common::strcpy_s(&serial[0], sizeof(serial), "\"");
			gameFile.read(&serial[1], 6);
			Common::strcpy_s(&serial[7], sizeof(serial)-7, "\"");
		} else {
			Blorb b(*file, INTERPRETER_ZCODE);
			Common::SeekableReadStream *f = b.createReadStreamForMember("game");
			emptyBlorb = f == nullptr;

			if (!emptyBlorb) {
				f->seek(18);
				Common::strcpy_s(&serial[0], sizeof(serial), "\"");
				f->read(&serial[1], 6);
				Common::strcpy_s(&serial[7], sizeof(serial) - 7, "\"");
				delete f;
			}
		}
		gameFile.close();

		// Check for known games. Note that there has been some variation in exact filesizes
		// for Infocom games due to padding at the end of files. So we match on md5s for the
		// first 5Kb, and only worry about filesize for more recent Blorb based Zcode games
		const FrotzGameDescription *p = FROTZ_GAMES;
		while (p->_gameId && p->_md5 && (md5 != p->_md5 ||
				(filesize != p->_filesize && isBlorb)))
			++p;

		if (!p->_gameId) {
			// Generic .dat/.data/.zip files don't get reported as matches unless they have a known md5
			if (filename.hasSuffixIgnoreCase(".dat") || filename.hasSuffixIgnoreCase(".data") || filename.hasSuffixIgnoreCase(".zip") || emptyBlorb)
				continue;

			const PlainGameDescriptor &desc = ZCODE_GAME_LIST[0];
			gameList.push_back(GlkDetectedGame(desc.gameId, desc.description, filename, md5, filesize));
		} else {
			GameDescriptor gameDesc = findGame(p->_gameId);
			DetectedGame gd = DetectedGame("glk", p->_gameId, gameDesc._description, p->_language, Common::kPlatformUnknown, p->_extra);
			gd.setGUIOptions(p->_guiOptions);

			gd.addExtraEntry("filename", filename);
			gameList.push_back(gd);
		}
	}

	return !gameList.empty();
}

void ZCodeMetaEngine::detectClashes(Common::StringMap &map) {
	for (int idx = 0; idx < 2; ++idx) {
		for (const PlainGameDescriptor *pd = (idx == 0) ? INFOCOM_GAME_LIST : ZCODE_GAME_LIST; pd->gameId; ++pd) {
			if (map.contains(pd->gameId))
				error("Duplicate game Id found - %s", pd->gameId);
			map[pd->gameId] = "";
		}
	}
}

} // End of namespace ZCode
} // End of namespace Glk
