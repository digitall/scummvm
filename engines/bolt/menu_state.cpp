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
 *
 */

#include "bolt/menu_state.h"

#include "bolt/bolt.h"

#include "common/events.h"
#include "common/system.h"

namespace Bolt {

struct BltMenuBgInfo { // type 32
	BltMenuBgInfo(const byte *src) {
		imageAndPaletteId = BltLongId(READ_BE_UINT32(&src[4]));
		// FIXME: unknown fields
		numButtons = READ_BE_UINT16(&src[0x1A]);
		buttonInfoId = BltLongId(READ_BE_UINT32(&src[0x1C]));
		buttonOriginX = (int16)READ_BE_UINT16(&src[0x20]);
		buttonOriginY = (int16)READ_BE_UINT16(&src[0x22]);
	}

	BltLongId imageAndPaletteId;
	uint16 numButtons;
	BltLongId buttonInfoId;
	int16 buttonOriginX;
	int16 buttonOriginY;
};

struct BltMenuBgImageAndPalette { // type 26
	BltMenuBgImageAndPalette(const byte *src) {
		imageId = BltLongId(READ_BE_UINT32(&src[0]));
		paletteId = BltLongId(READ_BE_UINT32(&src[4]));
		hotspotsId = BltLongId(READ_BE_UINT32(&src[8]));
	}

	BltLongId imageId;
	BltLongId paletteId;
	BltLongId hotspotsId;
};

struct BltMenuButtonInfo { // type 31

	static const int SIZE = 0x14;
	BltMenuButtonInfo(const byte *src) {
		// FIXME: unknown fields. unknown whether this is correct.
		// I believe field 0 is a type. type 1 indicates a rectangular frame.
		rect = Rect(&src[2]);
		hoverInfoId = BltLongId(READ_BE_UINT32(&src[0x10]));
	}

	Rect rect;
	BltLongId hoverInfoId;
};

struct BltMenuHoverInfo { // type 30
	BltMenuHoverInfo(const byte *src) {
		action = READ_BE_UINT16(&src[0]);
		param1Id = BltLongId(READ_BE_UINT32(&src[2]));
		param2Id = BltLongId(READ_BE_UINT32(&src[6]));
		param3Id = BltLongId(READ_BE_UINT32(&src[0xA]));
	}

	uint16 action;
	BltLongId param1Id;
	BltLongId param2Id;
	BltLongId param3Id;
};

struct BltLocImage { // type 27
	BltLocImage(const byte *src) {
		// FIXME: signed?
		x = READ_BE_UINT16(&src[0]);
		y = READ_BE_UINT16(&src[2]);
		imageId = BltLongId(READ_BE_UINT32(&src[4]));
	}

	uint16 x;
	uint16 y;
	BltLongId imageId;
};

struct BltMenuPaletteMod {
	BltMenuPaletteMod(const byte *src) {
		start = src[0];
		num = src[1];
		colors = BltLongId(READ_BE_UINT32(&src[2]));
	}

	byte start;
	byte num;
	BltLongId colors;
};

MenuStatePtr MenuState::create(BoltEngine *engine, BltLongId menuId) {

	MenuStatePtr self(new MenuState());

	self->_engine = engine;

	// Load resources
	self->_menuBgInfo = engine->_boltlibBltFile.loadLongId(menuId);
	assert(self->_menuBgInfo->getType() == kBltMenuBgInfo);

	BltMenuBgInfo bgInfoStruct(&self->_menuBgInfo->getData()[0]);
	self->_menuBgImageAndPalette = engine->_boltlibBltFile.loadLongId(bgInfoStruct.imageAndPaletteId);
	if (self->_menuBgImageAndPalette) {
		assert(self->_menuBgImageAndPalette->getType() == kBltMenuBgImageAndPalette);

		BltMenuBgImageAndPalette imageAndPaletteStruct(&self->_menuBgImageAndPalette->getData()[0]);
		self->_menuBgImage = engine->_boltlibBltFile.loadLongId(imageAndPaletteStruct.imageId);
		assert(self->_menuBgImage->getType() == kBltImage);
		self->_menuBgPalette = engine->_boltlibBltFile.loadLongId(imageAndPaletteStruct.paletteId);
		assert(self->_menuBgPalette->getType() == kBltPalette);
	}
	else {
		self->_menuBgImage.reset();
		self->_menuBgPalette.reset();
	}

	self->_menuButtonInfo = engine->_boltlibBltFile.loadLongId(bgInfoStruct.buttonInfoId);
	assert(self->_menuButtonInfo->getType() == kBltMenuButtonInfo);

	self->_menuButtons.resize(bgInfoStruct.numButtons);
	for (int i = 0; i < bgInfoStruct.numButtons; ++i) {

		BltMenuButtonInfo buttonInfo(
			&self->_menuButtonInfo->getData()[i * BltMenuButtonInfo::SIZE]);

		self->_menuButtons[i].rect = buttonInfo.rect;
		self->_menuButtons[i].rect.translate(-bgInfoStruct.buttonOriginX, -bgInfoStruct.buttonOriginY);
		self->_menuButtons[i].hoverAction = MenuButton::kNone;

		BltResourcePtr hoverInfoRes = engine->_boltlibBltFile.loadLongId(buttonInfo.hoverInfoId);
		if (hoverInfoRes) {
			assert(hoverInfoRes->getType() == kBltMenuHoverInfo);
			BltMenuHoverInfo hoverInfo(&hoverInfoRes->getData()[0]);
			if (hoverInfo.action == 0) {
				// image
				self->_menuButtons[i].hoverAction = MenuButton::kImage;
				// TODO: handle
				warning("Image hover actions not handled");
			}
			else if (hoverInfo.action == 1) {
				// palette mod
				self->_menuButtons[i].hoverAction = MenuButton::kPaletteMod;

				BltResourcePtr activePalModRes = engine->_boltlibBltFile.loadLongId(hoverInfo.param2Id);
				assert(activePalModRes->getType() == kBltMenuPaletteMod);
				BltMenuPaletteMod activePalMod(&activePalModRes->getData()[0]);
				self->_menuButtons[i].activePalStart = activePalMod.start;
				self->_menuButtons[i].activePalNum = activePalMod.num;
				self->_menuButtons[i].activePalColors = engine->_boltlibBltFile.loadLongId(activePalMod.colors);
				assert(self->_menuButtons[i].activePalColors->getType() == kBltColors);

				BltResourcePtr inactivePalModRes = engine->_boltlibBltFile.loadLongId(hoverInfo.param3Id);
				assert(inactivePalModRes->getType() == kBltMenuPaletteMod);
				BltMenuPaletteMod inactivePalMod(&inactivePalModRes->getData()[0]);
				self->_menuButtons[i].inactivePalStart = inactivePalMod.start;
				self->_menuButtons[i].inactivePalNum = inactivePalMod.num;
				self->_menuButtons[i].inactivePalColors = engine->_boltlibBltFile.loadLongId(inactivePalMod.colors);
				assert(self->_menuButtons[i].inactivePalColors->getType() == kBltColors);
			}
			else if (hoverInfo.action == 2) {
				// dual image
				self->_menuButtons[i].hoverAction = MenuButton::kDualImage;

				BltResourcePtr activeLocImageRes = engine->_boltlibBltFile.loadLongId(hoverInfo.param2Id);
				if (activeLocImageRes) {
					assert(activeLocImageRes->getType() == kBltLocImage);
					BltLocImage activeLocImage(&activeLocImageRes->getData()[0]);
					self->_menuButtons[i].activeImageX = activeLocImage.x - bgInfoStruct.buttonOriginX;
					self->_menuButtons[i].activeImageY = activeLocImage.y - bgInfoStruct.buttonOriginY;
					self->_menuButtons[i].activeImage = engine->_boltlibBltFile.loadLongId(
						activeLocImage.imageId);
					assert(self->_menuButtons[i].activeImage->getType() == kBltImage);
				}
				else {
					self->_menuButtons[i].activeImage.reset();
				}

				BltResourcePtr inactiveLocImageRes = engine->_boltlibBltFile.loadLongId(hoverInfo.param3Id);
				if (inactiveLocImageRes) {
					assert(inactiveLocImageRes->getType() == kBltLocImage);
					BltLocImage inactiveLocImage(&inactiveLocImageRes->getData()[0]);
					self->_menuButtons[i].inactiveImageX = inactiveLocImage.x - bgInfoStruct.buttonOriginX;
					self->_menuButtons[i].inactiveImageY = inactiveLocImage.y - bgInfoStruct.buttonOriginY;
					self->_menuButtons[i].inactiveImage = engine->_boltlibBltFile.loadLongId(
						inactiveLocImage.imageId);
					assert(self->_menuButtons[i].inactiveImage->getType() == kBltImage);
				}
				else {
					self->_menuButtons[i].inactiveImage.reset();
				}
			}
			else {
				self->_menuButtons[i].hoverAction = MenuButton::kNone;
				warning("Unhandled hover action type %d", (int)hoverInfo.action);
			}

			debug(3, "Hover action type %d params 0x%.08X, 0x%.08X, 0x%.08X",
				(int)hoverInfo.action, hoverInfo.param1Id, hoverInfo.param2Id,
				hoverInfo.param3Id);
		}
	}

	engine->_graphics.clearForeground();

	self->render();

	return self;
}

MenuState::MenuState()
{ }

void MenuState::process(const Common::Event &event) {

	if (event.type == Common::EVENT_MOUSEMOVE) {
		render();
	}

	// XXX: on click, leave menu
	// TODO: process buttons
	if (event.type == Common::EVENT_LBUTTONDOWN) {
		_engine->endCard();
	}
}

void MenuState::render() {

	if (_menuBgPalette) {
		_engine->_graphics.setBackPalette(&_menuBgPalette->getData()[6], 0, 128);
	}

	if (_menuBgImage) {
		_engine->renderBltImageToBack(_menuBgImage, 0, 0, false);
	}

	for (size_t i = 0; i < _menuButtons.size(); ++i) {
		Common::Point mousePos = _engine->getEventManager()->getMousePos();
		bool active = _menuButtons[i].rect.contains(mousePos);
		renderMenuButton(_menuButtons[i], active);
	}

	for (size_t i = 0; i < _menuButtons.size(); ++i) {
		_engine->_graphics.drawRectToBack(_menuButtons[i].rect, 0x7F);
	}

	_engine->_displayDirty = true;
}

void MenuState::renderMenuButton(const MenuButton &button, bool active) {
	if (button.hoverAction == MenuButton::kPaletteMod) {
		if (active) {
			// apply active colors
			_engine->_graphics.setBackPalette(&button.activePalColors->getData()[0],
				button.activePalStart, button.activePalNum);
		}
		else {
			// apply inactive colors
			_engine->_graphics.setBackPalette(&button.inactivePalColors->getData()[0],
				button.inactivePalStart, button.inactivePalNum);
		}
	}
	else if (button.hoverAction == MenuButton::kDualImage) {
		if (active) {
			// apply active image
			if (button.activeImage) {
				_engine->renderBltImageToBack(button.activeImage, button.activeImageX,
					button.activeImageY, true);
			}
		}
		else {
			// apply inactive image
			if (button.inactiveImage) {
				_engine->renderBltImageToBack(button.inactiveImage, button.inactiveImageX,
					button.inactiveImageY, true);
			}
		}
	}
}

} // End of namespace Bolt
