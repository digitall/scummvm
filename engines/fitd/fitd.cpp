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

#include "fitd/fitd.h"
#include "common/config-manager.h"
#include "common/file.h"
#include "common/scummsys.h"
#include "common/system.h"
#include "fitd/common.h"
#include "fitd/detection.h"
#include "fitd/gfx.h"
#include "fitd/save.h"
#include "fitd/system_menu.h"

namespace Fitd {

FitdEngine *g_engine;
int *currentCVarTable = nullptr;

FitdEngine::FitdEngine(OSystem *syst, const FitdGameDescription *gameDesc)
	: Engine(syst), _gameDescription(gameDesc), _randomSource("Fitd") {
	g_engine = this;
}

FitdEngine::~FitdEngine() {
}

FitdGameId FitdEngine::getGameId() const {
	return _gameDescription->gameId;
}

Common::Error FitdEngine::run() {
	runGame();

	return Common::kNoError;
}

uint32 FitdEngine::getRandomNumber(uint maxNum) {
	return _randomSource.getRandomNumber(maxNum);
}

bool FitdEngine::hasFeature(EngineFeature f) const {
	return f == kSupportsLoadingDuringRuntime ||
		   f == kSupportsSavingDuringRuntime ||
		   f == kSupportsReturnToLauncher ||
		   f == kSupportsChangingOptionsDuringRuntime;
};

bool FitdEngine::canLoadGameStateCurrently(Common::U32String *msg) {
	return true;
}
bool FitdEngine::canSaveGameStateCurrently(Common::U32String *msg) {
	return true;
}

Common::Error FitdEngine::loadGameStream(Common::SeekableReadStream *stream) {
	return loadSave(stream) == 1 ? Common::kNoError : Common::kReadingFailed;
}

Common::Error FitdEngine::saveGameStream(Common::WriteStream *stream, bool isAutosave) {
	savedSurface = gfx_capture();
	// Default to returning an error when not implemented
	return makeSave(stream) == 1 ? Common::kNoError : Common::kWritingFailed;
}

} // End of namespace Fitd
