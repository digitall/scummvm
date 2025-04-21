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

#include "backends/keymapper/keymapper.h"
#include "backends/keymapper/action.h"
#include "backends/keymapper/standard-actions.h"
#include "common/translation.h"
#include "graphics/scaler.h"
#include "graphics/thumbnail.h"

#include "fitd/actions.h"
#include "fitd/detection.h"
#include "fitd/fitd.h"
#include "fitd/gfx.h"
#include "fitd/system_menu.h"

class FitdMetaEngine : public AdvancedMetaEngine<Fitd::FitdGameDescription> {
public:
	const char *getName() const override final;
	Common::Error createInstance(OSystem *syst, Engine **engine, const Fitd::FitdGameDescription *desc) const override final;
	bool hasFeature(MetaEngineFeature f) const override final;
	void getSavegameThumbnail(Graphics::Surface &thumb) override final;
	Common::Array<Common::Keymap *> initKeymaps(const char *target) const override final;
	void registerDefaultSettings(const Common::String &) const override final;
};

const char *FitdMetaEngine::getName() const {
	return "fitd";
}

Common::Error FitdMetaEngine::createInstance(OSystem *syst, Engine **engine, const Fitd::FitdGameDescription *desc) const {
	*engine = new Fitd::FitdEngine(syst, desc);
	return Common::kNoError;
}

bool FitdMetaEngine::hasFeature(MetaEngineFeature f) const {
	return checkExtendedSaves(f) ||
		   (f == kSupportsLoadingDuringStartup);
}

void FitdMetaEngine::getSavegameThumbnail(Graphics::Surface &thumb) {
	assert(Fitd::savedSurface);
	Graphics::Surface *scaledSavedScreen = scale(*Fitd::savedSurface, kThumbnailWidth, kThumbnailHeight2);
	assert(scaledSavedScreen);
	thumb.copyFrom(*scaledSavedScreen);

	scaledSavedScreen->free();
	delete scaledSavedScreen;
}

Common::Array<Common::Keymap *> FitdMetaEngine::initKeymaps(const char *target) const {
	Common::Keymap *engineKeyMap = new Common::Keymap(Common::Keymap::kKeymapTypeGame, target, "Alone in the Dark keymap");

	struct {
		const char *name;
		const char *desc;
		Fitd::FitdAction action;
		char inputs[32];
		Common::EventType event;
		Common::KeyCode key;
	} actions[] = {
		{"ACTION", _s("Action (Fight/Open/Search/Shut/Push/Jump)"), Fitd::kDefaultAction, "MOUSE_LEFT|JOY_A", Common::EVENT_LBUTTONDOWN, Common::KEYCODE_SPACE},
		{"VALIDATE", _s("Validate"), Fitd::kValidate, "MOUSE_RIGHT|JOY_B", Common::EVENT_LBUTTONDOWN, Common::KEYCODE_RETURN},
		{"PARAMETER_SCREEN", _s("Parameter screen"), Fitd::kParameterScreen, "JOY_LEFT_TRIGGER", Common::EVENT_INVALID, Common::KEYCODE_ESCAPE},
		{Common::kStandardActionMoveLeft, _s("Left"), Fitd::kMoveLeft, "LEFT|JOY_LEFT_STICK_X-", Common::EVENT_INVALID, Common::KEYCODE_LEFT},
		{Common::kStandardActionMoveRight, _s("Right"), Fitd::kMoveRight, "RIGHT|JOY_LEFT_STICK_X+", Common::EVENT_INVALID, Common::KEYCODE_RIGHT},
		{Common::kStandardActionMoveUp, _s("Up"), Fitd::kMoveUp, "UP|JOY_LEFT_STICK_Y-", Common::EVENT_INVALID, Common::KEYCODE_UP},
		{Common::kStandardActionMoveDown, _s("Down"), Fitd::kMoveDown, "DOWN|JOY_LEFT_STICK_Y+", Common::EVENT_INVALID, Common::KEYCODE_DOWN},
		{0, 0, Fitd::kDefaultAction, {}, Common::EVENT_INVALID, Common::KEYCODE_INVALID}};

	Common::Action *act;
	for (int i = 0; actions[i].name; i++) {
		act = new Common::Action(actions[i].name, _(actions[i].desc));
		act->setCustomEngineActionEvent(actions[i].action);
		char *strToken = strtok(actions[i].inputs, "|");
		while (strToken) {
			act->addDefaultInputMapping(strToken);
			strToken = strtok(nullptr, "|");
		}
		if (actions[i].event != Common::EVENT_INVALID) {
			act->setEvent(actions[i].event);
		}
		if (actions[i].key != Common::KEYCODE_INVALID) {
			act->setKeyEvent(actions[i].key);
		}
		engineKeyMap->addAction(act);
	}

	return Common::Keymap::arrayOf(engineKeyMap);
}

void FitdMetaEngine::registerDefaultSettings(const Common::String &) const {
	ConfMan.registerDefault("language", "en");
}

#if PLUGIN_ENABLED_DYNAMIC(FITD)
REGISTER_PLUGIN_DYNAMIC(FITD, PLUGIN_TYPE_ENGINE, FitdMetaEngine);
#else
REGISTER_PLUGIN_STATIC(FITD, PLUGIN_TYPE_ENGINE, FitdMetaEngine);
#endif
