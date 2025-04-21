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

#ifndef FITD_H
#define FITD_H

#include "common/scummsys.h"
#include "common/random.h"
#include "engines/engine.h"
#include "fitd/detection.h"

namespace Graphics {
class Screen;
}

namespace Fitd {

struct FitdGameDescription;

class FitdEngine : public Engine {
private:
	const FitdGameDescription *_gameDescription;
	Common::RandomSource _randomSource;
protected:
	// Engine APIs
	Common::Error run() override;
public:
	Graphics::Screen *_screen = nullptr;
	FitdEngine(OSystem *syst, const FitdGameDescription *gameDesc);
	~FitdEngine() override;

	FitdGameId getGameId() const;
	uint32 getRandomNumber(uint maxNum);

	bool hasFeature(EngineFeature f) const override final;

	bool canLoadGameStateCurrently(Common::U32String *msg = nullptr) override final;
	bool canSaveGameStateCurrently(Common::U32String *msg = nullptr) override final;

private:
	Common::Error loadGameStream(Common::SeekableReadStream *stream) override final;
	Common::Error saveGameStream(Common::WriteStream *stream, bool isAutosave = false) override final;
};

extern FitdEngine *g_engine;
#define SHOULD_QUIT ::Fitd::g_engine->shouldQuit()

} // End of namespace Fitd

#endif // FITD_H
