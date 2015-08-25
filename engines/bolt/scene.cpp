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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "bolt/scene.h"

#include "common/events.h"

#include "bolt/bolt.h"

namespace Bolt {

struct BltScene { // type 32
	BltScene(const byte *src) {
		forePlaneId = BltLongId(READ_BE_UINT32(&src[0]));
		backPlaneId = BltLongId(READ_BE_UINT32(&src[4]));
		// FIXME: unknown fields
		numButtons = READ_BE_UINT16(&src[0x1A]);
		buttonsId = BltLongId(READ_BE_UINT32(&src[0x1C]));
		origin.x = (int16)READ_BE_UINT16(&src[0x20]);
		origin.y = (int16)READ_BE_UINT16(&src[0x22]);
	}

	BltLongId forePlaneId;
	BltLongId backPlaneId;
	uint16 numButtons;
	BltLongId buttonsId;
	Common::Point origin;
};

struct BltButton { // type 31
	static const int SIZE = 0x14;
	BltButton(const byte *src) {
		type = READ_BE_UINT16(&src[0]);
		rect = Rect(&src[2]);
		plane = READ_BE_UINT16(&src[0xA]);
		numGraphics = READ_BE_UINT16(&src[0xC]);
		// FIXME: unknown field at 0xE. Always 0 in game data.
		graphicsId = BltLongId(READ_BE_UINT32(&src[0x10]));
	}

	uint16 type;
	Rect rect;
	uint16 plane;
	uint16 numGraphics;
	BltLongId graphicsId;
};

struct BltButtonGraphics { // type 30
	BltButtonGraphics(const byte *src) {
		type = READ_BE_UINT16(&src[0]);
		// FIXME: The purpose of defaultId is unknown. It is invalid most of the
		// time, but it is used in the buttons on sliding puzzles.
		// When it is valid, it refers to a ButtonImage.
		defaultId = BltLongId(READ_BE_UINT32(&src[2]));
		hoveredId = BltLongId(READ_BE_UINT32(&src[6]));
		idleId = BltLongId(READ_BE_UINT32(&src[0xA]));
	}

	uint16 type;
	BltLongId defaultId;
	BltLongId hoveredId;
	BltLongId idleId;
};

struct BltButtonPaletteMod { // type 29
	BltButtonPaletteMod(const byte *src) {
		start = src[0];
		num = src[1];
		colorsId = BltLongId(READ_BE_UINT32(&src[2]));
	}

	byte start;
	byte num;
	BltLongId colorsId;
};

struct BltButtonImage { // type 27
	BltButtonImage(const byte *src) {
		pos.x = (int16)READ_BE_UINT16(&src[0]);
		pos.y = (int16)READ_BE_UINT16(&src[2]);
		imageId = BltLongId(READ_BE_UINT32(&src[4]));
	}

	Common::Point pos;
	BltLongId imageId;
};

struct BltPlane { // type 26
	BltPlane(const byte *src) {
		imageId = BltLongId(READ_BE_UINT32(&src[0]));
		paletteId = BltLongId(READ_BE_UINT32(&src[4]));
		hotspotsId = BltLongId(READ_BE_UINT32(&src[8]));
	}

	BltLongId imageId;
	BltLongId paletteId;
	BltLongId hotspotsId;
};

void Scene::init(BoltEngine *engine, BltFile *bltFile, BltLongId sceneId)
{
	_engine = engine;

	BltResource sceneInfoRes(bltFile->loadLongId(sceneId, kBltScene));
	BltScene sceneInfo(&sceneInfoRes[0]);

	_origin = sceneInfo.origin;

	loadPlane(_forePlane, bltFile, sceneInfo.forePlaneId);
	loadPlane(_backPlane, bltFile, sceneInfo.backPlaneId);

	BltResource buttonsRes(bltFile->loadLongId(sceneInfo.buttonsId, kBltButtons));
	_buttons.reserve(sceneInfo.numButtons);
	for (uint16 i = 0; i < sceneInfo.numButtons; ++i) {
		ButtonPtr newButton(new Button);
		loadButton(*newButton, bltFile, &buttonsRes[i * BltButton::SIZE]);
		_buttons.push_back(newButton);
	}
}

void Scene::draw() {
	if (_backPlane.palette) {
		_engine->_graphics.getBackPlane().setPalette(&_backPlane.palette[6], 0, 128);
	}
	if (_backPlane.image.isLoaded()) {
		::Graphics::Surface surface = _engine->_graphics.getBackPlane().getSurface();
		_backPlane.image.drawAt(surface, 0, 0, false);
	}
	else {
		_engine->_graphics.getBackPlane().clear();
	}

	if (_forePlane.palette) {
		_engine->_graphics.getForePlane().setPalette(&_forePlane.palette[6], 0, 128);
	}
	if (_forePlane.image.isLoaded()) {
		::Graphics::Surface surface = _engine->_graphics.getForePlane().getSurface();
		_forePlane.image.drawAt(surface, 0, 0, false);
	}
	else {
		_engine->_graphics.getForePlane().clear();
	}

	for (size_t i = 0; i < _buttons.size(); ++i) {
		if (isButtonAtPoint(*_buttons[i],
			_engine->getEventManager()->getMousePos())) {

			drawButton(*_buttons[i], true);
		}
		else {
			drawButton(*_buttons[i], false);
		}
	}

	_engine->_displayDirty = true;
}

void Scene::loadPlane(Plane &plane, BltFile *bltFile, BltLongId planeId) {
	if (!planeId.isValid()) {
		return;
	}

	BltResource planeRes(bltFile->loadLongId(planeId, kBltPlane));
	BltPlane planeInfo(&planeRes[0]);
	plane.image.init(bltFile, planeInfo.imageId);
	plane.palette.reset(bltFile->loadLongId(planeInfo.paletteId, kBltPalette));
	plane.hotspots.init(bltFile, planeInfo.hotspotsId);
}

Scene::Plane& Scene::getScenePlane(uint16 num) {
	return num ? _backPlane : _forePlane;
}

const Scene::Plane& Scene::getScenePlane(uint16 num) const {
	return num ? _backPlane : _forePlane;
}

Bolt::Plane& Scene::getGraphicsPlane(uint16 num) {
	return num ? _engine->_graphics.getBackPlane() : _engine->_graphics.getForePlane();
}

void Scene::loadButton(Button &button, BltFile *bltFile, const byte *src) {
	BltButton buttonInfo(&src[0]);
	button.hotspotType = (Button::HotspotType)buttonInfo.type;
	button.rect = buttonInfo.rect;
	button.plane = buttonInfo.plane;

	if (button.hotspotType == Button::kHotspotRect) {
		button.rect.translate(-_origin.x, -_origin.y);
	}

	if (buttonInfo.graphicsId.isValid()) {
		BltResource buttonGfxRes(bltFile->loadLongId(buttonInfo.graphicsId, kBltButtonGraphics));
		// FIXME: Some buttons have multiple graphics entries corresponding to
		// different states of the button.
		BltButtonGraphics buttonGfx(&buttonGfxRes[0]);

		button.graphicsType = (Button::GraphicsType)buttonGfx.type;

		if (buttonGfx.defaultId.isValid()) {
			BltResource defaultRes(bltFile->loadLongId(buttonGfx.defaultId, kBltSprites));
			BltButtonImage defaultButtonImage(&defaultRes[0]);
			button.defaultImagePos = defaultButtonImage.pos;
			button.defaultImagePos.x -= _origin.x;
			button.defaultImagePos.y -= _origin.y;
			button.defaultImage.init(bltFile, defaultButtonImage.imageId);
		}

		if (button.graphicsType == Button::kGfxImages) {
			if (buttonGfx.hoveredId.isValid()) {
				// TODO: Factor out repetitive code
				BltResource hoveredRes(bltFile->loadLongId(buttonGfx.hoveredId, kBltSprites));
				BltButtonImage hoveredButtonImage(&hoveredRes[0]);
				button.hoveredImagePos = hoveredButtonImage.pos;
				button.hoveredImagePos.x -= _origin.x;
				button.hoveredImagePos.y -= _origin.y;
				button.hoveredImage.init(bltFile, hoveredButtonImage.imageId);
			}
			if (buttonGfx.idleId.isValid()) {
				BltResource idleRes(bltFile->loadLongId(buttonGfx.idleId, kBltSprites));
				BltButtonImage idleButtonImage(&idleRes[0]);
				button.idleImagePos = idleButtonImage.pos;
				button.idleImagePos.x -= _origin.x;
				button.idleImagePos.y -= _origin.y;
				button.idleImage.init(bltFile, idleButtonImage.imageId);
			}
		}
		else if (button.graphicsType == Button::kGfxPaletteMods) {
			if (buttonGfx.hoveredId.isValid()) {
				// TODO: Factor out repetitive code
				BltResource hoveredRes(bltFile->loadLongId(buttonGfx.hoveredId, kBltButtonPaletteMod));
				BltButtonPaletteMod hoveredPalMod(&hoveredRes[0]);
				button.hoveredPalStart = hoveredPalMod.start;
				button.hoveredPalNum = hoveredPalMod.num;
				button.hoveredColors.reset(bltFile->loadLongId(hoveredPalMod.colorsId, kBltButtonColors));
			}
			if (buttonGfx.idleId.isValid()) {
				BltResource idleRes(bltFile->loadLongId(buttonGfx.idleId, kBltButtonPaletteMod));
				BltButtonPaletteMod idlePalMod(&idleRes[0]);
				button.idlePalStart = idlePalMod.start;
				button.idlePalNum = idlePalMod.num;
				button.idleColors.reset(bltFile->loadLongId(idlePalMod.colorsId, kBltButtonColors));
			}
		}
		else {
			warning("Unknown button graphics type %d", (int)button.graphicsType);
		}
	}
}

bool Scene::isButtonAtPoint(const Button &button, const Common::Point &pt) const {
	if (button.hotspotType == Button::kHotspotRect) {
		return button.rect.contains(pt);
	}
	else if (button.hotspotType == Button::kHotspotImageQuery) {
		byte color = getScenePlane(button.plane).hotspots.query(pt.x, pt.y);
		return color >= button.rect.left && color <= button.rect.right;
	}

	return false;
}

void Scene::drawButton(const Button &button, bool hovered) {
	if (button.graphicsType == Button::kGfxPaletteMods) {
		if (button.defaultImage.isLoaded()) {
			// FIXME: This might not be the correct usage of defaultImage.
			button.defaultImage.drawAt(
				getGraphicsPlane(button.plane).getSurface(),
				button.defaultImagePos.x, button.defaultImagePos.y, true);
		}

		if (hovered) {
			// apply hovered colors
			if (button.hoveredColors) {
				getGraphicsPlane(button.plane).setPalette(&button.hoveredColors[0],
					button.hoveredPalStart, button.hoveredPalNum);
			}
		}
		else {
			// apply idle colors
			if (button.idleColors) {
				getGraphicsPlane(button.plane).setPalette(&button.idleColors[0],
					button.idlePalStart, button.idlePalNum);
			}
		}
	}
	else if (button.graphicsType == Button::kGfxImages) {
		::Graphics::Surface surface = getGraphicsPlane(button.plane).getSurface();
		if (hovered) {
			// apply hovered image
			if (button.hoveredImage.isLoaded()) {
				button.hoveredImage.drawAt(surface,
					button.hoveredImagePos.x, button.hoveredImagePos.y, true);
			}
		}
		else {
			// apply idle image
			if (button.idleImage.isLoaded()) {
				button.idleImage.drawAt(surface,
					button.idleImagePos.x, button.idleImagePos.y, true);
			}
		}
	}
}

} // End of namespace Bolt
