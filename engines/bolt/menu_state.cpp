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

#include "common/events.h"
#include "common/system.h"
#include "graphics/surface.h"

#include "bolt/bolt.h"

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
		type = READ_BE_UINT16(&src[0]);
		rect = Rect(&src[2]);
		hoverInfoId = BltLongId(READ_BE_UINT32(&src[0x10]));
	}

	uint16 type;
	Rect rect;
	BltLongId hoverInfoId;
};

struct BltMenuHoverInfo { // type 30
	BltMenuHoverInfo(const byte *src) {
		type = READ_BE_UINT16(&src[0]);
		defaultId = BltLongId(READ_BE_UINT32(&src[2]));
		hoveredId = BltLongId(READ_BE_UINT32(&src[6]));
		idleId = BltLongId(READ_BE_UINT32(&src[0xA]));
	}

	uint16 type;
	BltLongId defaultId;
	BltLongId hoveredId;
	BltLongId idleId;
};

struct BltLocImage { // type 27
	BltLocImage(const byte *src) {
		x = (int16)READ_BE_UINT16(&src[0]);
		y = (int16)READ_BE_UINT16(&src[2]);
		imageId = BltLongId(READ_BE_UINT32(&src[4]));
	}

	int16 x;
	int16 y;
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
	self->init(engine, menuId);
	return self;
}

void MenuState::init(BoltEngine *engine, BltLongId menuId) {
	_engine = engine;

	// Load resources
	_menuBgInfo = engine->_boltlibBltFile.loadLongId(menuId);
	assert(_menuBgInfo->getType() == kBltMenuBgInfo);

	BltMenuBgInfo bgInfoStruct(&_menuBgInfo->getData()[0]);
	_menuBgImageAndPalette = engine->_boltlibBltFile.loadLongId(bgInfoStruct.imageAndPaletteId);
	if (_menuBgImageAndPalette) {
		assert(_menuBgImageAndPalette->getType() == kBltMenuBgImageAndPalette);

		BltMenuBgImageAndPalette imageAndPaletteStruct(&_menuBgImageAndPalette->getData()[0]);
		_menuBgImage = BltImage::load(&_engine->_boltlibBltFile,
			imageAndPaletteStruct.imageId);
		_menuBgPalette = engine->_boltlibBltFile.loadLongId(imageAndPaletteStruct.paletteId);
		assert(_menuBgPalette->getType() == kBltPalette);

		if (imageAndPaletteStruct.hotspotsId.value != 0xFFFFFFFFUL) {
			_hotspotsImage = BltImage::load(&_engine->_boltlibBltFile,
				imageAndPaletteStruct.hotspotsId);
		}
	}
	else {
		_menuBgImage.reset();
		_menuBgPalette.reset();
	}

	_menuButtonInfo = engine->_boltlibBltFile.loadLongId(bgInfoStruct.buttonInfoId);
	assert(_menuButtonInfo->getType() == kBltMenuButtonInfo);

	_menuButtons.resize(bgInfoStruct.numButtons);
	for (int i = 0; i < bgInfoStruct.numButtons; ++i) {
		BltMenuButtonInfo buttonInfo(
			&_menuButtonInfo->getData()[i * BltMenuButtonInfo::SIZE]);

		_menuButtons[i].hotspotType = (MenuButton::HotspotType)buttonInfo.type;
		_menuButtons[i].rect = buttonInfo.rect;
		if (_menuButtons[i].hotspotType == MenuButton::kHotspotRect) {
			_menuButtons[i].rect.translate(-bgInfoStruct.buttonOriginX, -bgInfoStruct.buttonOriginY);
		}

		BltResourcePtr hoverInfoRes = engine->_boltlibBltFile.loadLongId(buttonInfo.hoverInfoId);
		if (hoverInfoRes) {
			assert(hoverInfoRes->getType() == kBltMenuHoverInfo);
			BltMenuHoverInfo hoverInfo(&hoverInfoRes->getData()[0]);

			BltResourcePtr defaultRes = _engine->_boltlibBltFile.loadLongId(hoverInfo.defaultId);
			if (defaultRes) {
				assert(defaultRes->getType() == kBltLocImage);
				BltLocImage defaultLocImage(&defaultRes->getData()[0]);
				_menuButtons[i].defaultImagePos.x = defaultLocImage.x - bgInfoStruct.buttonOriginX;
				_menuButtons[i].defaultImagePos.y = defaultLocImage.y - bgInfoStruct.buttonOriginY;
				_menuButtons[i].defaultImage = BltImage::load(
					&_engine->_boltlibBltFile, defaultLocImage.imageId);
			}
			else {
				_menuButtons[i].defaultImage.reset();
			}

			if (hoverInfo.type == MenuButton::kGfxPaletteMods) {
				_menuButtons[i].gfxType = MenuButton::kGfxPaletteMods;

				BltResourcePtr activePalModRes = engine->_boltlibBltFile.loadLongId(hoverInfo.hoveredId);
				assert(activePalModRes->getType() == kBltMenuPaletteMod);
				BltMenuPaletteMod activePalMod(&activePalModRes->getData()[0]);
				_menuButtons[i].activePalStart = activePalMod.start;
				_menuButtons[i].activePalNum = activePalMod.num;
				_menuButtons[i].activePalColors = engine->_boltlibBltFile.loadLongId(activePalMod.colors);
				assert(_menuButtons[i].activePalColors->getType() == kBltColors);

				BltResourcePtr inactivePalModRes = engine->_boltlibBltFile.loadLongId(hoverInfo.idleId);
				assert(inactivePalModRes->getType() == kBltMenuPaletteMod);
				BltMenuPaletteMod inactivePalMod(&inactivePalModRes->getData()[0]);
				_menuButtons[i].inactivePalStart = inactivePalMod.start;
				_menuButtons[i].inactivePalNum = inactivePalMod.num;
				_menuButtons[i].inactivePalColors = engine->_boltlibBltFile.loadLongId(inactivePalMod.colors);
				assert(_menuButtons[i].inactivePalColors->getType() == kBltColors);
			}
			else if (hoverInfo.type == MenuButton::kGfxImages) {
				_menuButtons[i].gfxType = MenuButton::kGfxImages;

				BltResourcePtr activeLocImageRes = engine->_boltlibBltFile.loadLongId(hoverInfo.hoveredId);
				if (activeLocImageRes) {
					assert(activeLocImageRes->getType() == kBltLocImage);
					BltLocImage activeLocImage(&activeLocImageRes->getData()[0]);
					_menuButtons[i].hoveredImagePos.x = activeLocImage.x - bgInfoStruct.buttonOriginX;
					_menuButtons[i].hoveredImagePos.y = activeLocImage.y - bgInfoStruct.buttonOriginY;
					_menuButtons[i].hoveredImage = BltImage::load(&_engine->_boltlibBltFile,
						activeLocImage.imageId);
				}
				else {
					_menuButtons[i].hoveredImage.reset();
				}

				BltResourcePtr inactiveLocImageRes = engine->_boltlibBltFile.loadLongId(hoverInfo.idleId);
				if (inactiveLocImageRes) {
					assert(inactiveLocImageRes->getType() == kBltLocImage);
					BltLocImage inactiveLocImage(&inactiveLocImageRes->getData()[0]);
					_menuButtons[i].idleImagePos.x = inactiveLocImage.x - bgInfoStruct.buttonOriginX;
					_menuButtons[i].idleImagePos.y = inactiveLocImage.y - bgInfoStruct.buttonOriginY;
					_menuButtons[i].idleImage = BltImage::load(&_engine->_boltlibBltFile,
						inactiveLocImage.imageId);
				}
				else {
					_menuButtons[i].idleImage.reset();
				}
			}
			else {
				_menuButtons[i].gfxType = MenuButton::kGfxNone;
				warning("Unhandled button graphics type %d", (int)hoverInfo.type);
			}
		}
	}

	engine->_graphics.getForePlane().clear();
	render();
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
		_engine->_graphics.getBackPlane().setPalette(&_menuBgPalette->getData()[6], 0, 128);
	}

	if (_menuBgImage) {
		::Graphics::Surface surface = _engine->_graphics.getBackPlane().getSurface();
		_menuBgImage->drawAt(surface, 0, 0, false);
		_engine->_displayDirty = true;
	}

	for (size_t i = 0; i < _menuButtons.size(); ++i) {
		if (isButtonAtPoint(_menuButtons[i],
			_engine->getEventManager()->getMousePos())) {

			renderMenuButton(_menuButtons[i], true);
		}
		else {
			renderMenuButton(_menuButtons[i], false);
		}
	}

	_engine->_displayDirty = true;
}

bool MenuState::isButtonAtPoint(const MenuButton &button, const Common::Point &pt) const {
	if (button.hotspotType == MenuButton::kHotspotRect) {
		return button.rect.contains(pt);
	}
	else if (button.hotspotType == MenuButton::kHotspotImageQuery) {
		byte color = _hotspotsImage->query(pt.x, pt.y);
		return color >= button.rect.left && color <= button.rect.right;
	}

	return false;
}

void MenuState::renderMenuButton(const MenuButton &button, bool active) {
	if (button.gfxType == MenuButton::kGfxPaletteMods) {
		if (button.defaultImage) {
			button.defaultImage->drawAt(
				_engine->_graphics.getBackPlane().getSurface(),
				button.defaultImagePos.x, button.defaultImagePos.y, true);
		}

		if (active) {
			// apply hovered colors
			_engine->_graphics.getBackPlane().setPalette(&button.activePalColors->getData()[0],
				button.activePalStart, button.activePalNum);
		}
		else {
			// apply idle colors
			_engine->_graphics.getBackPlane().setPalette(&button.inactivePalColors->getData()[0],
				button.inactivePalStart, button.inactivePalNum);
		}
	}
	else if (button.gfxType == MenuButton::kGfxImages) {
		::Graphics::Surface surface = _engine->_graphics.getBackPlane().getSurface();
		if (active) {
			// apply hovered image
			if (button.hoveredImage) {
				button.hoveredImage->drawAt(surface,
					button.hoveredImagePos.x, button.hoveredImagePos.y, true);
				_engine->_displayDirty = true;
			}
		}
		else {
			// apply idle image
			if (button.idleImage) {
				button.idleImage->drawAt(surface,
					button.idleImagePos.x, button.idleImagePos.y, true);
				_engine->_displayDirty = true;
			}
		}
	}
}

} // End of namespace Bolt
