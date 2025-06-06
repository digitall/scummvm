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

// Allow use of stuff in <time.h>
#define FORBIDDEN_SYMBOL_EXCEPTION_time_h

// Disable printf override in common/forbidden.h to avoid
// clashes with log.h from the Android SDK.
// That header file uses
//   __attribute__ ((format(printf, 3, 4)))
// which gets messed up by our override mechanism; this could
// be avoided by either changing the Android SDK to use the equally
// legal and valid
//   __attribute__ ((format(__printf__, 3, 4)))
// or by refining our printf override to use a varadic macro
// (which then wouldn't be portable, though).
// Anyway, for now we just disable the printf override globally
// for the Android port
#define FORBIDDEN_SYMBOL_EXCEPTION_printf

#include "backends/platform/android/android.h"
#include "backends/platform/android/jni-android.h"
#include "backends/fs/android/android-fs-factory.h"
#include "backends/fs/android/android-saf-fs.h"

#include "gui/browser.h"
#include "gui/gui-manager.h"
#include "gui/message.h"
#include "gui/ThemeEval.h"
#include "gui/widget.h"
#include "gui/widgets/list.h"
#include "gui/widgets/popup.h"

#include "common/translation.h"

enum {
	kRemoveCmd = 'RemS',
	kExportBackupCmd = 'ExpD',
	kImportBackupCmd = 'ImpD',
};

class AndroidOptionsWidget final : public GUI::OptionsContainerWidget {
public:
	explicit AndroidOptionsWidget(GuiObject *boss, const Common::String &name, const Common::String &domain);
	~AndroidOptionsWidget() override;

	// OptionsContainerWidget API
	void load() override;
	bool save() override;
	bool hasKeys() override;
	void setEnabled(bool e) override;

private:
	// OptionsContainerWidget API
	void defineLayout(GUI::ThemeEval &layouts, const Common::String &layoutName, const Common::String &overlayedLayout) const override;
	void handleCommand(GUI::CommandSender *sender, uint32 cmd, uint32 data) override;

	GUI::CheckboxWidget *_onscreenCheckbox;
	GUI::StaticTextWidget *_preferredTouchModeDesc;
	GUI::StaticTextWidget *_preferredTMMenusDesc;
	GUI::PopUpWidget *_preferredTMMenusPopUp;
	GUI::StaticTextWidget *_preferredTM2DGamesDesc;
	GUI::PopUpWidget *_preferredTM2DGamesPopUp;
	GUI::StaticTextWidget *_preferredTM3DGamesDesc;
	GUI::PopUpWidget *_preferredTM3DGamesPopUp;
	GUI::StaticTextWidget *_orientationDesc;
	GUI::StaticTextWidget *_orientationMenusDesc;
	GUI::PopUpWidget *_orientationMenusPopUp;
	GUI::StaticTextWidget *_orientationGamesDesc;
	GUI::PopUpWidget *_orientationGamesPopUp;

	bool _enabled;

	uint32 loadTouchMode(const Common::String &setting, bool acceptDefault, uint32 defaultValue);
	void saveTouchMode(const Common::String &setting, uint32 touchMode);
	uint32 loadOrientation(const Common::String &setting, bool acceptDefault, uint32 defaultValue);
	void saveOrientation(const Common::String &setting, uint32 orientation);
};

class SAFRemoveDialog : public GUI::Dialog {
public:
	SAFRemoveDialog();
	virtual ~SAFRemoveDialog();

	void open() override;
	void reflowLayout() override;

	void handleCommand(GUI::CommandSender *sender, uint32 cmd, uint32 data) override;

protected:
	GUI::ListWidget   *_safList;
	AbstractFSList    _safTrees;

	void clearListing();
	void updateListing();
};

enum {
	kTouchModeDefault = -1,
	kTouchModeTouchpad = 0,
	kTouchModeMouse,
	kTouchModeGamepad,
};

enum {
	kOrientationDefault = -1,
	kOrientationAuto = 0,
	kOrientationPortrait,
	kOrientationLandscape,
};

AndroidOptionsWidget::AndroidOptionsWidget(GuiObject *boss, const Common::String &name, const Common::String &domain) :
		OptionsContainerWidget(boss, name, "AndroidOptionsDialog", domain), _enabled(true) {

	const bool inAppDomain = domain.equalsIgnoreCase(Common::ConfigManager::kApplicationDomain);;

	_onscreenCheckbox = new GUI::CheckboxWidget(widgetsBoss(), "AndroidOptionsDialog.OnScreenControl", _("Show On-screen control"));
	_preferredTouchModeDesc = new GUI::StaticTextWidget(widgetsBoss(), "AndroidOptionsDialog.PreferredTouchModeText", _("Choose the preferred touch mode:"));
	if (inAppDomain) {
		_preferredTMMenusDesc = new GUI::StaticTextWidget(widgetsBoss(), "AndroidOptionsDialog.TMMenusText", _("In menus"));
		_preferredTMMenusPopUp = new GUI::PopUpWidget(widgetsBoss(), "AndroidOptionsDialog.TMMenus");
		_preferredTMMenusPopUp->appendEntry(_("Touchpad emulation"), kTouchModeTouchpad);
		_preferredTMMenusPopUp->appendEntry(_("Direct mouse"), kTouchModeMouse); // TODO: Find a better name
		_preferredTMMenusPopUp->appendEntry(_("Gamepad emulation"), kTouchModeGamepad);
	} else {
		_preferredTMMenusDesc = nullptr;
		_preferredTMMenusPopUp = nullptr;
	}

	_preferredTM2DGamesDesc = new GUI::StaticTextWidget(widgetsBoss(), "AndroidOptionsDialog.TM2DGamesText", _("In 2D games"));
	_preferredTM2DGamesPopUp = new GUI::PopUpWidget(widgetsBoss(), "AndroidOptionsDialog.TM2DGames");
	_preferredTM3DGamesDesc = new GUI::StaticTextWidget(widgetsBoss(), "AndroidOptionsDialog.TM3DGamesText", _("In 3D games"));
	_preferredTM3DGamesPopUp = new GUI::PopUpWidget(widgetsBoss(), "AndroidOptionsDialog.TM3DGames");

	if (!inAppDomain) {
		_preferredTM2DGamesPopUp->appendEntry(_("<default>"), kTouchModeDefault);
		_preferredTM3DGamesPopUp->appendEntry(_("<default>"), kTouchModeDefault);
	}

	_preferredTM2DGamesPopUp->appendEntry(_("Touchpad emulation"), kTouchModeTouchpad);
	_preferredTM3DGamesPopUp->appendEntry(_("Touchpad emulation"), kTouchModeTouchpad);

	_preferredTM2DGamesPopUp->appendEntry(_("Direct mouse"), kTouchModeMouse); // TODO: Find a better name
	_preferredTM3DGamesPopUp->appendEntry(_("Direct mouse"), kTouchModeMouse);

	_preferredTM2DGamesPopUp->appendEntry(_("Gamepad emulation"), kTouchModeGamepad);
	_preferredTM3DGamesPopUp->appendEntry(_("Gamepad emulation"), kTouchModeGamepad);

	_orientationDesc = new GUI::StaticTextWidget(widgetsBoss(), "AndroidOptionsDialog.OrientationText", _("Select the orientation:"));
	if (inAppDomain) {
		_orientationMenusDesc = new GUI::StaticTextWidget(widgetsBoss(), "AndroidOptionsDialog.OMenusText", _("In menus"));
		_orientationMenusPopUp = new GUI::PopUpWidget(widgetsBoss(), "AndroidOptionsDialog.OMenus");
		_orientationMenusPopUp->appendEntry(_("Automatic"), kOrientationAuto);
		_orientationMenusPopUp->appendEntry(_("Portrait"), kOrientationPortrait);
		_orientationMenusPopUp->appendEntry(_("Landscape"), kOrientationLandscape);
	} else {
		_orientationMenusDesc = nullptr;
		_orientationMenusPopUp = nullptr;
	}

	_orientationGamesDesc = new GUI::StaticTextWidget(widgetsBoss(), "AndroidOptionsDialog.OGamesText", _("In games"));
	_orientationGamesPopUp = new GUI::PopUpWidget(widgetsBoss(), "AndroidOptionsDialog.OGames");

	if (!inAppDomain) {
		_orientationGamesPopUp->appendEntry(_("<default>"), kOrientationDefault);
	}

	_orientationGamesPopUp->appendEntry(_("Automatic"), kOrientationAuto);
	_orientationGamesPopUp->appendEntry(_("Portrait"), kOrientationPortrait);
	_orientationGamesPopUp->appendEntry(_("Landscape"), kOrientationLandscape);

	if (inAppDomain) {
		// Only show these buttons in Options (via Options... in the launcher), and not at game domain level (via Edit Game...)
		(new GUI::ButtonWidget(widgetsBoss(), "AndroidOptionsDialog.ExportDataButton", _("Export backup"), _("Export a backup of the configuration and save files"), kExportBackupCmd))->setTarget(this);
		(new GUI::ButtonWidget(widgetsBoss(), "AndroidOptionsDialog.ImportDataButton", _("Import backup"), _("Import a previously exported backup file"), kImportBackupCmd))->setTarget(this);
		if (AndroidFilesystemFactory::instance().hasSAF()) {
			// I18N: This button opens a list of all folders added for Android Storage Attached Framework
			(new GUI::ButtonWidget(widgetsBoss(), "AndroidOptionsDialog.ForgetSAFButton", _("Remove folder authorizations..."), Common::U32String(), kRemoveCmd))->setTarget(this);
		}
	}
}

AndroidOptionsWidget::~AndroidOptionsWidget() {
}

void AndroidOptionsWidget::defineLayout(GUI::ThemeEval &layouts, const Common::String &layoutName, const Common::String &overlayedLayout) const {
	const bool inAppDomain = _domain.equalsIgnoreCase(Common::ConfigManager::kApplicationDomain);;

	layouts.addDialog(layoutName, overlayedLayout)
	        .addLayout(GUI::ThemeLayout::kLayoutVertical)
	            .addPadding(0, 0, 0, 0)
	            .addWidget("OnScreenControl", "Checkbox")
	            .addWidget("PreferredTouchModeText", "", -1, layouts.getVar("Globals.Line.Height"));

	if (inAppDomain) {
		layouts.addLayout(GUI::ThemeLayout::kLayoutHorizontal)
			.addPadding(16, 16, 0, 0)
			.addWidget("TMMenusText", "OptionsLabel")
			.addWidget("TMMenus", "PopUp")
		.closeLayout();
	}
	layouts.addLayout(GUI::ThemeLayout::kLayoutHorizontal)
			.addPadding(16, 16, 0, 0)
			.addWidget("TM2DGamesText", "OptionsLabel")
			.addWidget("TM2DGames", "PopUp")
		.closeLayout()
		.addLayout(GUI::ThemeLayout::kLayoutHorizontal)
			.addPadding(16, 16, 0, 0)
			.addWidget("TM3DGamesText", "OptionsLabel")
			.addWidget("TM3DGames", "PopUp")
		.closeLayout();

	layouts.addWidget("OrientationText", "", -1, layouts.getVar("Globals.Line.Height"));
	if (inAppDomain) {
		layouts.addLayout(GUI::ThemeLayout::kLayoutHorizontal)
			.addPadding(16, 16, 0, 0)
			.addWidget("OMenusText", "OptionsLabel")
			.addWidget("OMenus", "PopUp")
		.closeLayout();
	}
	layouts.addLayout(GUI::ThemeLayout::kLayoutHorizontal)
			.addPadding(16, 16, 0, 0)
			.addWidget("OGamesText", "OptionsLabel")
			.addWidget("OGames", "PopUp")
		.closeLayout();

	if (inAppDomain) {
		layouts.addWidget("ExportDataButton", "WideButton");
		layouts.addWidget("ImportDataButton", "WideButton");
		if (AndroidFilesystemFactory::instance().hasSAF()) {
			layouts.addWidget("ForgetSAFButton", "WideButton");
		}
	}
	layouts.closeLayout()
	    .closeDialog();
}

void AndroidOptionsWidget::handleCommand(GUI::CommandSender *sender, uint32 cmd, uint32 data) {
	switch (cmd) {
	case kRemoveCmd: {
		if (!AndroidFilesystemFactory::instance().hasSAF()) {
			break;
		}
		SAFRemoveDialog removeDlg;
		removeDlg.runModal();
		break;
	}
	case kExportBackupCmd:
	{
		Common::U32String prompt(_("Select backup destination"));
		int ret = JNI::exportBackup(prompt);
		if (ret == 1) {
			// BackupManager.ERROR_CANCELLED
			break;
		}

		if (ret == 0 && AndroidFilesystemFactory::instance().hasSAF()) {
			prompt = _("The backup has been saved successfully.");
		} else if (ret == 0) {
			prompt = _("The backup has been saved successfully to the Downloads folder.");
		} else if (ret == -2) {
			prompt = _("The game saves couldn't be backed up");
		} else {
			prompt = _("An error occured while saving the backup.");
		}
		g_system->displayMessageOnOSD(prompt);
		break;
	}
	case kImportBackupCmd:
	{
		GUI::MessageDialog alert(_("Restoring a backup will erase the current configuration and overwrite existing saves. Do you want to proceed?"), _("Proceed"), _("Cancel"));
		if (alert.runModal() != GUI::kMessageOK) {
			break;
		}

		Common::U32String prompt(_("Select backup file"));
		Common::Path path;
		if (!AndroidFilesystemFactory::instance().hasSAF()) {
			GUI::BrowserDialog browser(prompt, false);
			if (browser.runModal() <= 0) {
				break;
			}

			path = browser.getResult().getPath();
		}
		int ret = JNI::importBackup(prompt, path.toString());
		if (ret == 1) {
			// BackupManager.ERROR_CANCELLED
			break;
		}

		if (ret == 0) {
			prompt = _("The backup has been restored successfully.");
		} else if (ret == -2) {
			prompt = _("The game saves couldn't be backed up");
		} else {
			prompt = _("An error occured while restoring the backup.");
		}
		g_system->displayMessageOnOSD(prompt);
		break;
	}
	default:
		GUI::OptionsContainerWidget::handleCommand(sender, cmd, data);
	}
}

uint32 AndroidOptionsWidget::loadTouchMode(const Common::String &setting, bool acceptDefault, uint32 defaultValue) {
	if (!acceptDefault || ConfMan.hasKey(setting, _domain)) {
		Common::String preferredTouchMode = ConfMan.get(setting, _domain);
		if (preferredTouchMode == "mouse") {
			return kTouchModeMouse;
		} else if (preferredTouchMode == "gamepad") {
			return kTouchModeGamepad;
		} else if (preferredTouchMode == "touchpad") {
			return kTouchModeTouchpad;
		} else {
			return defaultValue;
		}
	} else {
		return kTouchModeDefault;
	}
}

uint32 AndroidOptionsWidget::loadOrientation(const Common::String &setting, bool acceptDefault, uint32 defaultValue) {
	if (!acceptDefault || ConfMan.hasKey(setting, _domain)) {
		Common::String orientation = ConfMan.get(setting, _domain);
		if (orientation == "auto") {
			return kOrientationAuto;
		} else if (orientation == "portrait") {
			return kOrientationPortrait;
		} else if (orientation == "landscape") {
			return kOrientationLandscape;
		} else {
			return defaultValue;
		}
	} else {
		return kOrientationDefault;
	}
}

void AndroidOptionsWidget::load() {
	const bool inAppDomain = _domain.equalsIgnoreCase(Common::ConfigManager::kApplicationDomain);

	_onscreenCheckbox->setState(ConfMan.getBool("onscreen_control", _domain));

	// When in application domain, we don't have default entry so we must have a value
	if (inAppDomain) {
		_preferredTMMenusPopUp->setSelectedTag(loadTouchMode("touch_mode_menus", !inAppDomain, kTouchModeMouse));
	}
	_preferredTM2DGamesPopUp->setSelectedTag(loadTouchMode("touch_mode_2d_games", !inAppDomain, kTouchModeTouchpad));
	_preferredTM3DGamesPopUp->setSelectedTag(loadTouchMode("touch_mode_3d_games", !inAppDomain, kTouchModeGamepad));

	if (inAppDomain) {
		_orientationMenusPopUp->setSelectedTag(loadOrientation("orientation_menus", !inAppDomain, kOrientationAuto));
	}
	_orientationGamesPopUp->setSelectedTag(loadOrientation("orientation_games", !inAppDomain, kOrientationAuto));
}

void AndroidOptionsWidget::saveTouchMode(const Common::String &setting, uint32 touchMode) {
	switch (touchMode) {
	case kTouchModeTouchpad:
		ConfMan.set(setting, "touchpad", _domain);
		break;
	case kTouchModeMouse:
		ConfMan.set(setting, "mouse", _domain);
		break;
	case kTouchModeGamepad:
		ConfMan.set(setting, "gamepad", _domain);
		break;
	default:
		// default
		ConfMan.removeKey(setting, _domain);
		break;
	}
}

void AndroidOptionsWidget::saveOrientation(const Common::String &setting, uint32 orientation) {
	switch (orientation) {
	case kOrientationAuto:
		ConfMan.set(setting, "auto", _domain);
		break;
	case kOrientationPortrait:
		ConfMan.set(setting, "portrait", _domain);
		break;
	case kOrientationLandscape:
		ConfMan.set(setting, "landscape", _domain);
		break;
	default:
		// default
		ConfMan.removeKey(setting, _domain);
		break;
	}
}

bool AndroidOptionsWidget::save() {
	const bool inAppDomain = _domain.equalsIgnoreCase(Common::ConfigManager::kApplicationDomain);

	if (_enabled) {
		ConfMan.setBool("onscreen_control", _onscreenCheckbox->getState(), _domain);

		if (inAppDomain) {
			saveTouchMode("touch_mode_menus", _preferredTMMenusPopUp->getSelectedTag());
		}
		saveTouchMode("touch_mode_2d_games", _preferredTM2DGamesPopUp->getSelectedTag());
		saveTouchMode("touch_mode_3d_games", _preferredTM3DGamesPopUp->getSelectedTag());

		if (inAppDomain) {
			saveOrientation("orientation_menus", _orientationMenusPopUp->getSelectedTag());
		}
		saveOrientation("orientation_games", _orientationGamesPopUp->getSelectedTag());
	} else {
		ConfMan.removeKey("onscreen_control", _domain);

		if (inAppDomain) {
			ConfMan.removeKey("touch_mode_menus", _domain);
		}
		ConfMan.removeKey("touch_mode_2d_games", _domain);
		ConfMan.removeKey("touch_mode_3d_games", _domain);

		if (inAppDomain) {
			ConfMan.removeKey("orientation_menus", _domain);
		}
		ConfMan.removeKey("orientation_games", _domain);
	}

	return true;
}

bool AndroidOptionsWidget::hasKeys() {
	return ConfMan.hasKey("onscreen_control", _domain) ||
	       (_domain.equalsIgnoreCase(Common::ConfigManager::kApplicationDomain) && ConfMan.hasKey("touch_mode_menus", _domain)) ||
	       ConfMan.hasKey("touch_mode_2d_games", _domain) ||
	       ConfMan.hasKey("touch_mode_3d_games", _domain) ||
	       (_domain.equalsIgnoreCase(Common::ConfigManager::kApplicationDomain) && ConfMan.hasKey("orientation_menus", _domain)) ||
	       ConfMan.hasKey("orientation_games", _domain);
}

void AndroidOptionsWidget::setEnabled(bool e) {
	const bool inAppDomain = _domain.equalsIgnoreCase(Common::ConfigManager::kApplicationDomain);

	_enabled = e;

	_onscreenCheckbox->setEnabled(e);

	if (inAppDomain) {
		_preferredTMMenusDesc->setEnabled(e);
		_preferredTMMenusPopUp->setEnabled(e);
	}
	_preferredTM2DGamesDesc->setEnabled(e);
	_preferredTM2DGamesPopUp->setEnabled(e);
	_preferredTM3DGamesDesc->setEnabled(e);
	_preferredTM3DGamesPopUp->setEnabled(e);
	if (inAppDomain) {
		_orientationMenusDesc->setEnabled(e);
		_orientationMenusPopUp->setEnabled(e);
	}
	_orientationGamesDesc->setEnabled(e);
	_orientationGamesPopUp->setEnabled(e);
}


GUI::OptionsContainerWidget *OSystem_Android::buildBackendOptionsWidget(GUI::GuiObject *boss, const Common::String &name, const Common::String &target) const {
	return new AndroidOptionsWidget(boss, name, target);
}

void OSystem_Android::registerDefaultSettings(const Common::String &target) const {
	ConfMan.registerDefault("onscreen_control", true);
	ConfMan.registerDefault("touch_mode_menus", "mouse");
	ConfMan.registerDefault("touch_mode_2d_games", "touchpad");
	ConfMan.registerDefault("touch_mode_3d_games", "gamepad");
	ConfMan.registerDefault("orientation_menus", "auto");
	ConfMan.registerDefault("orientation_games", "auto");
}

void OSystem_Android::applyTouchSettings(bool _3dMode, bool overlayShown) {
	Common::String setting;
	int defaultMode;

	if (overlayShown) {
		setting = "touch_mode_menus";
		defaultMode = TOUCH_MODE_MOUSE;
	} else if (_3dMode) {
		setting = "touch_mode_3d_games";
		defaultMode = TOUCH_MODE_GAMEPAD;
	} else {
		setting = "touch_mode_2d_games";
		defaultMode = TOUCH_MODE_TOUCHPAD;
	}

	Common::String preferredTouchMode = ConfMan.get(setting);
	if (preferredTouchMode == "mouse") {
		JNI::setTouchMode(TOUCH_MODE_MOUSE);
	} else if (preferredTouchMode == "gamepad") {
		JNI::setTouchMode(TOUCH_MODE_GAMEPAD);
	} else if (preferredTouchMode == "touchpad") {
		JNI::setTouchMode(TOUCH_MODE_TOUCHPAD);
	} else {
		JNI::setTouchMode(defaultMode);
	}
}

void OSystem_Android::applyOrientationSettings() {
	const Common::String activeDomain = ConfMan.getActiveDomainName();
	const bool inAppDomain = activeDomain.empty() ||
		activeDomain.equalsIgnoreCase(Common::ConfigManager::kApplicationDomain);

	Common::String setting;

	if (inAppDomain) {
		setting = "orientation_menus";
	} else {
		setting = "orientation_games";
	}

	Common::String orientation = ConfMan.get(setting);
	if (orientation == "portrait") {
		JNI::setOrientation(SCREEN_ORIENTATION_PORTRAIT);
	} else if (orientation == "landscape") {
		JNI::setOrientation(SCREEN_ORIENTATION_LANDSCAPE);
	// auto and everything else
	} else {
		JNI::setOrientation(SCREEN_ORIENTATION_UNSPECIFIED);
	}
}

void OSystem_Android::applyBackendSettings() {
	updateOnScreenControls();
}

SAFRemoveDialog::SAFRemoveDialog() : GUI::Dialog("SAFBrowser") {

	// Add file list
	_safList = new GUI::ListWidget(this, "SAFBrowser.List");
	_safList->setNumberingMode(GUI::kListNumberingOff);
	_safList->setEditable(false);

	_backgroundType = GUI::ThemeEngine::kDialogBackgroundPlain;

	// Buttons
	new GUI::ButtonWidget(this, "SAFBrowser.Close", _("Close"), Common::U32String(), GUI::kCloseCmd);
	new GUI::ButtonWidget(this, "SAFBrowser.Remove", _("Remove"), Common::U32String(), kRemoveCmd);
}

SAFRemoveDialog::~SAFRemoveDialog() {
	clearListing();
}

void SAFRemoveDialog::open() {
	// Call super implementation
	Dialog::open();

	updateListing();
}

void SAFRemoveDialog::reflowLayout() {
	GUI::ThemeEval &layouts = *g_gui.xmlEval();
	layouts.addDialog(_name, "GlobalOptions", -1, -1, 16)
	        .addLayout(GUI::ThemeLayout::kLayoutVertical)
	            .addPadding(16, 16, 16, 16)
	            .addWidget("List", "")
		    .addLayout(GUI::ThemeLayout::kLayoutVertical)
			.addPadding(0, 0, 16, 0)
			.addLayout(GUI::ThemeLayout::kLayoutHorizontal)
				.addPadding(0, 0, 0, 0)
				.addWidget("Remove", "Button")
				.addSpace(-1)
				.addWidget("Close", "Button")
			.closeLayout()
		    .closeLayout()
		.closeLayout()
	.closeDialog();

	layouts.setVar("Dialog.SAFBrowser.Shading", 1);

	Dialog::reflowLayout();
}

void SAFRemoveDialog::handleCommand(GUI::CommandSender *sender, uint32 cmd, uint32 data) {
	switch (cmd) {
	case kRemoveCmd:
	{
		int id = _safList->getSelected();
		if (id == -1) {
			break;
		}

		AndroidSAFFilesystemNode *node = reinterpret_cast<AndroidSAFFilesystemNode *>(_safTrees[id]);
		node->removeTree();

		updateListing();
		break;
	}
	default:
		GUI::Dialog::handleCommand(sender, cmd, data);
	}
}

void SAFRemoveDialog::clearListing() {
	for (AbstractFSList::iterator it = _safTrees.begin(); it != _safTrees.end(); it++) {
		delete *it;
	}
	_safTrees.clear();
}

void SAFRemoveDialog::updateListing() {
	int oldSel = _safList->getSelected();

	clearListing();

	AndroidFilesystemFactory::instance().getSAFTrees(_safTrees, false);

	Common::U32StringArray list;
	list.reserve(_safTrees.size());
	for (AbstractFSList::iterator it = _safTrees.begin(); it != _safTrees.end(); it++) {
		list.push_back((*it)->getDisplayName());
	}

	_safList->setList(list);
	if (oldSel >= 0 && (size_t)oldSel < list.size()) {
		_safList->setSelected(oldSel);
	} else {
		_safList->scrollTo(0);
	}

	// Finally, redraw
	g_gui.scheduleTopDialogRedraw();
}
