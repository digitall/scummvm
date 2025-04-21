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

#include "base/plugins.h"
#include "common/config-manager.h"
#include "common/file.h"
#include "common/util.h"
#include "fitd/detection.h"
#include "fitd/detection_tables.h"

class FitdMetaEngineDetection : public AdvancedMetaEngineDetection<Fitd::FitdGameDescription> {
	static const DebugChannelDef debugFlagList[];

public:
	FitdMetaEngineDetection();
	~FitdMetaEngineDetection() override {}

	const char *getName() const override {
		return "fitd";
	}

	const char *getEngineName() const override {
		return "Free in the dark";
	}

	const char *getOriginalCopyright() const override {
		return "Alone in the dark (C)";
	}

	const DebugChannelDef *getDebugChannels() const override {
		return debugFlagList;
	}

	DetectedGame toDetectedGame(const ADDetectedGame &adGame, ADDetectedGameExtraInfo *extraInfo) const override {
		DetectedGame game = AdvancedMetaEngineDetection::toDetectedGame(adGame, extraInfo);
		game.appendGUIOptions(Common::getGameGUIOptionsDescriptionLanguage(Common::EN_ANY));
		game.appendGUIOptions(Common::getGameGUIOptionsDescriptionLanguage(Common::FR_FRA));
		game.appendGUIOptions(Common::getGameGUIOptionsDescriptionLanguage(Common::IT_ITA));
		game.appendGUIOptions(Common::getGameGUIOptionsDescriptionLanguage(Common::DE_DEU));
		game.appendGUIOptions(Common::getGameGUIOptionsDescriptionLanguage(Common::ES_ESP));
		return game;
	}
};

const DebugChannelDef FitdMetaEngineDetection::debugFlagList[] = {
	{Fitd::kDebugConsole, "imgui", "Show ImGui debug window (if available)"},
	DEBUG_CHANNEL_END
};

FitdMetaEngineDetection::FitdMetaEngineDetection() : AdvancedMetaEngineDetection(
	Fitd::gameDescriptions, Fitd::fitdGames) {
}

REGISTER_PLUGIN_STATIC(FITD_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, FitdMetaEngineDetection);
