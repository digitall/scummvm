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

#include "common/translation.h"
#include "graphics/scaler.h"
#include "graphics/thumbnail.h"

#include "fitd/detection.h"
#include "fitd/fitd.h"
#include "fitd/gfx.h"
#include "fitd/system_menu.h"

namespace Fitd {

static const ADExtraGuiOptionsMap optionsList[] = {
	{
		GAMEOPTION_ORIGINAL_SAVELOAD,
		{
			_s("Use original save/load screens"),
			_s("Use the original save/load screens instead of the ScummVM ones"),
			"original_menus",
			false,
			0,
			0
		}
	},
	AD_EXTRA_GUI_OPTIONS_TERMINATOR
};

} // End of namespace Fitd

class FitdMetaEngine : public AdvancedMetaEngine<Fitd::FitdGameDescription> {
public:
	const char *getName() const override;
	Common::Error createInstance(OSystem *syst, Engine **engine, const Fitd::FitdGameDescription *desc) const override;
	bool hasFeature(MetaEngineFeature f) const override;
	const ADExtraGuiOptionsMap *getAdvancedExtraGuiOptions() const override;
	void getSavegameThumbnail(Graphics::Surface &thumb) override;
};

const char *FitdMetaEngine::getName() const {
	return "fitd";
}

const ADExtraGuiOptionsMap *FitdMetaEngine::getAdvancedExtraGuiOptions() const {
	return Fitd::optionsList;
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
	Graphics::Surface *scaledSavedScreen = scale(*Fitd::savedSurface, kThumbnailWidth, kThumbnailHeight2);
	assert(scaledSavedScreen);
	thumb.copyFrom(*scaledSavedScreen);

	scaledSavedScreen->free();
	delete scaledSavedScreen;
}

#if PLUGIN_ENABLED_DYNAMIC(FITD)
REGISTER_PLUGIN_DYNAMIC(FITD, PLUGIN_TYPE_ENGINE, FitdMetaEngine);
#else
REGISTER_PLUGIN_STATIC(FITD, PLUGIN_TYPE_ENGINE, FitdMetaEngine);
#endif
