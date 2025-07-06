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

#include "backends/keymapper/action.h"
#include "backends/keymapper/keymapper.h"
#include "backends/keymapper/standard-actions.h"
#include "common/savefile.h"
#include "common/system.h"
#include "common/translation.h"
#include "graphics/managed_surface.h"
#include "graphics/scaler.h"

#include "fitd/actions.h"
#include "fitd/detection.h"
#include "fitd/engine.h"
#include "fitd/fitd.h"
#include "fitd/system_menu.h"

class FitdMetaEngine final : public AdvancedMetaEngine<Fitd::FitdGameDescription> {
public:
	const char *getName() const override;
	Common::Error createInstance(OSystem *syst, Engine **engine, const Fitd::FitdGameDescription *desc) const override;
	bool hasFeature(MetaEngineFeature f) const override;
	Common::Array<Common::Keymap *> initKeymaps(const char *target) const override;
	void registerDefaultSettings(const Common::String &) const override;
	SaveStateDescriptor querySaveMetaInfos(const char *target, int slot) const override;
	void getSavegameThumbnail(Graphics::Surface &thumb) override;
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
		   f == kSupportsLoadingDuringStartup;
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
		{"ACTION", _s("Action (Fight/Open/Search/Shut/Push/Jump)"), Fitd::kDefaultAction, "JOY_A", Common::EVENT_INVALID, Common::KEYCODE_SPACE},
		{"VALIDATE", _s("Validate"), Fitd::kValidate, "JOY_B", Common::EVENT_INVALID, Common::KEYCODE_RETURN},
		{"PARAMETER_SCREEN", _s("Parameter screen"), Fitd::kParameterScreen, "JOY_LEFT_TRIGGER", Common::EVENT_INVALID, Common::KEYCODE_ESCAPE},
		{Common::kStandardActionMoveLeft, _s("Left"), Fitd::kMoveLeft, "LEFT|JOY_LEFT_STICK_X-", Common::EVENT_INVALID, Common::KEYCODE_LEFT},
		{Common::kStandardActionMoveRight, _s("Right"), Fitd::kMoveRight, "RIGHT|JOY_LEFT_STICK_X+", Common::EVENT_INVALID, Common::KEYCODE_RIGHT},
		{Common::kStandardActionMoveUp, _s("Up"), Fitd::kMoveUp, "UP|JOY_LEFT_STICK_Y-", Common::EVENT_INVALID, Common::KEYCODE_UP},
		{Common::kStandardActionMoveDown, _s("Down"), Fitd::kMoveDown, "DOWN|JOY_LEFT_STICK_Y+", Common::EVENT_INVALID, Common::KEYCODE_DOWN},
		{nullptr, nullptr, Fitd::kDefaultAction, {}, Common::EVENT_INVALID, Common::KEYCODE_INVALID}};

	for (int i = 0; actions[i].name; i++) {
		Common::Action *act = new Common::Action(actions[i].name, _(actions[i].desc));
		act->setCustomEngineActionEvent(actions[i].action);
		const char *strToken = strtok(actions[i].inputs, "|");
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

SaveStateDescriptor FitdMetaEngine::querySaveMetaInfos(const char *target, int slot) const {
	SaveStateDescriptor desc = MetaEngine::querySaveMetaInfos(target, slot);
	if (desc.isValid())
		return desc;

	Common::String saveGameFileName(getSavegameFile(slot, target));
	Common::InSaveFile *f = g_system->getSavefileManager()->openForLoading(saveGameFileName);
	if (f) {

		desc = SaveStateDescriptor(this, slot, "?");

		byte thumbnailData[80 * 50];
		byte palette[768];
		Common::String savegameDesc;
		if (!strcmp(target, "aitd1")) {
			const uint32 thumbOffset = f->readUint32BE();       // offset to thumbnail
			const uint32 descriptionOffset = f->readUint32BE(); // offset to description
			f->seek(thumbOffset, SEEK_SET);
			f->read(thumbnailData, sizeof(thumbnailData));

			f->seek(descriptionOffset, SEEK_SET);
			savegameDesc = f->readString();

			memcpy(palette, Fitd::g_engine->_engine->currentGamePalette, 768);
		} else {
			const uint32 thumbOffset = f->readUint32BE(); // offset to thumbnail
			const uint32 palOffset = f->readUint32BE();   // offset to palette
			const uint32 descOffset = f->readUint32BE();  // offset to description
			f->seek(thumbOffset, SEEK_SET);
			f->read(thumbnailData, sizeof(thumbnailData));
			f->seek(palOffset, SEEK_SET);
			f->read(palette, sizeof(palette));
			f->seek(descOffset, SEEK_SET);
			savegameDesc = f->readString();
		}

		Graphics::ManagedSurface thumbnail;
		thumbnail.create(80, 50, Graphics::PixelFormat::createFormatCLUT8());
		thumbnail.setPalette(palette, 0, 256);
		memcpy(thumbnail.getBasePtr(0, 0), thumbnailData, sizeof(thumbnailData));

		Graphics::Surface *thumbnailSmall = new Graphics::Surface();
		createThumbnail(thumbnailSmall, &thumbnail);
		desc.setThumbnail(thumbnailSmall);
		desc.setDescription(savegameDesc);

		delete f;

		return desc;
	}

	return SaveStateDescriptor();
}

void FitdMetaEngine::getSavegameThumbnail(Graphics::Surface &thumb) {
	Graphics::ManagedSurface screen;
	screen.create(320, 200, Graphics::PixelFormat::createFormatCLUT8());
	screen.setPalette(Fitd::g_engine->_engine->currentGamePalette, 0, 256);
	Fitd::scaleDownImage(320, 200, 0, 0, Fitd::g_engine->_engine->aux2, static_cast<byte *>(screen.getBasePtr(0, 0)), 320);
	const Common::ScopedPtr<Graphics::ManagedSurface> scaledScreen(screen.scale(kThumbnailWidth, kThumbnailHeight2));
	thumb.copyFrom(*scaledScreen);
	screen.free();
	scaledScreen->free();
}

#if PLUGIN_ENABLED_DYNAMIC(FITD)
REGISTER_PLUGIN_DYNAMIC(FITD, PLUGIN_TYPE_ENGINE, FitdMetaEngine);
#else
REGISTER_PLUGIN_STATIC(FITD, PLUGIN_TYPE_ENGINE, FitdMetaEngine);
#endif
