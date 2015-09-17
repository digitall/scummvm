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
#include "common/system.h"

#include "bolt/bolt.h"

namespace Bolt {

struct BltScene { // type 32
	BltScene(const byte *src) {
		forePlaneId = BltLongId(READ_BE_UINT32(&src[0]));
		backPlaneId = BltLongId(READ_BE_UINT32(&src[4]));
		numSprites = src[0x8];
		spritesId = BltLongId(READ_BE_UINT32(&src[0xA]));
		// FIXME: unknown fields
		colorCyclesId = BltLongId(READ_BE_UINT32(&src[0x16]));
		numButtons = READ_BE_UINT16(&src[0x1A]);
		buttonsId = BltLongId(READ_BE_UINT32(&src[0x1C]));
		origin.x = (int16)READ_BE_UINT16(&src[0x20]);
		origin.y = (int16)READ_BE_UINT16(&src[0x22]);
	}

	BltLongId forePlaneId;
	BltLongId backPlaneId;
	uint8 numSprites;
	BltLongId spritesId;
	BltLongId colorCyclesId;
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

struct BltSprite { // type 27
	static const int SIZE = 0x8;
	BltSprite(const byte *src) {
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

struct BltColorCycleSlot { // type 12
	BltColorCycleSlot(const byte *src) {
		start = READ_BE_UINT16(&src[0]);
		end = READ_BE_UINT16(&src[2]);
		delay = src[4];
	}

	uint16 start;
	uint16 end;
	byte delay;
};

struct BltColorCycles { // type 11
	BltColorCycles(const byte *src) {
		for (int i = 0; i < 4; ++i) {
			numSlots[i] = READ_BE_UINT16(&src[i * 2]);
		}
		for (int i = 0; i < 4; ++i) {
			slotIds[i] = BltLongId(READ_BE_UINT32(&src[8 + i * 4]));
		}
	}

	uint16 numSlots[4];
	BltLongId slotIds[4];
};

void Scene::load(BoltEngine *engine, BltFile &bltFile, BltLongId sceneId)
{
	_engine = engine;

	BltResource sceneInfoRes(bltFile.loadResource(sceneId, kBltScene));
	BltScene sceneInfo(&sceneInfoRes[0]);

	_origin = sceneInfo.origin;

	// Load planes
	loadPlane(_forePlane, bltFile, sceneInfo.forePlaneId);
	loadPlane(_backPlane, bltFile, sceneInfo.backPlaneId);

	// Load sprites
	if (sceneInfo.spritesId.isValid()) {
		BltResource spritesRes(bltFile.loadResource(sceneInfo.spritesId, kBltSprites));
		_sprites.alloc(sceneInfo.numSprites);
		for (int i = 0; i < sceneInfo.numSprites; ++i) {
			BltSprite s(&spritesRes[i * BltSprite::SIZE]);
			_sprites[i].pos = s.pos;
			_sprites[i].image.init(bltFile, s.imageId);
		}
	}

	// Load buttons
	BltResource buttonsRes(bltFile.loadResource(sceneInfo.buttonsId, kBltButtons));
	_buttons.alloc(sceneInfo.numButtons);
	for (uint16 i = 0; i < sceneInfo.numButtons; ++i) {
		loadButton(_buttons[i], bltFile, &buttonsRes[i * BltButton::SIZE]);
	}

	// Load color cycles
	for (int i = 0; i < NUM_COLOR_CYCLES; ++i) {
		_colorCycles[i] = ColorCycle();
	}
	if (sceneInfo.colorCyclesId.isValid()) {
		BltResource cyclesRes(bltFile.loadResource(sceneInfo.colorCyclesId, kBltColorCycles));
		BltColorCycles cyclesInfo(&cyclesRes[0]);
		for (int i = 0; i < NUM_COLOR_CYCLES; ++i) {
			if (cyclesInfo.slotIds[i].isValid()) {
				if (cyclesInfo.numSlots[i] != 1) {
					warning("Cycle slot does not have one palette region (it has %d)...", cyclesInfo.numSlots[i]);
				}
				else if (cyclesInfo.numSlots[i] == 0) {
					// No slots; skip
				}
				else {
					BltResource cycleSlotRes(bltFile.loadResource(cyclesInfo.slotIds[i], kBltColorCycleSlot));
					BltColorCycleSlot cycleSlotInfo(&cycleSlotRes[0]);
					_colorCycles[i].start = cycleSlotInfo.start;
					_colorCycles[i].num = cycleSlotInfo.end - cycleSlotInfo.start + 1;
					// FIXME: How fast are color cycles really? Is this correct at all?
					_colorCycles[i].delay = cycleSlotInfo.delay * 10;
				}
			}
		}
	}
}

void Scene::enter() {

	// Draw back plane
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

	// Draw fore plane
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

	// Draw sprites
	for (size_t i = 0; i < _sprites.size(); ++i) {
		Common::Point pos = _sprites[i].pos;
		pos.x -= _origin.x;
		pos.y -= _origin.y;
		// FIXME: Are sprites drawn to back or fore plane? Is it somehow selectable?
		::Graphics::Surface surface = _engine->_graphics.getForePlane().getSurface();
		_sprites[i].image.drawAt(surface, pos.x, pos.y, true);
	}

	// Reset color cycles
	uint32 enterTime = _engine->getTotalPlayTime();
	for (int i = 0; i < NUM_COLOR_CYCLES; ++i) {
		_colorCycles[i].curTime = enterTime;
	}

	process();

	_engine->scheduleDisplayUpdate();
}

void Scene::process() {

	uint32 curTime = _engine->getTotalPlayTime();

	// Update color cycles
	for (int i = 0; i < NUM_COLOR_CYCLES; ++i) {
		if (_colorCycles[i].num > 0) {
			uint32 timeDiff = curTime - _colorCycles[i].curTime;
			if (timeDiff >= _colorCycles[i].delay) {

				byte colors[128 * 3];
				// FIXME: Should we cycle fore or back plane? Is it selectable?
				_engine->_graphics.getBackPlane().grabPalette(colors,
					_colorCycles[i].start, _colorCycles[i].num);

				// Rotate colors right by one
				byte r = colors[3 * (_colorCycles[i].num - 1) + 0];
				byte g = colors[3 * (_colorCycles[i].num - 1) + 1];
				byte b = colors[3 * (_colorCycles[i].num - 1) + 2];
				memmove(&colors[3], &colors[0], 3 * (_colorCycles[i].num - 1));
				colors[0] = r;
				colors[1] = g;
				colors[2] = b;

				_engine->_graphics.getBackPlane().setPalette(colors,
					_colorCycles[i].start, _colorCycles[i].num);

				_colorCycles[i].curTime += _colorCycles[i].delay;
			}
		}
	}

	// Draw buttons
	int hoveredButton = getButtonAtPoint(_engine->getEventManager()->getMousePos());
	for (uint i = 0; i < _buttons.size(); ++i) {
		drawButton(_buttons[i], (int)i == hoveredButton);
	}

	_engine->scheduleDisplayUpdate();
}

void Scene::setBackPlane(BltFile &bltFile, BltLongId id) {
	loadPlane(_backPlane, bltFile, id);
}

int Scene::getButtonAtPoint(const Common::Point &pt) {
	for (int i = 0; i < (int)_buttons.size(); ++i) {
		if (isButtonAtPoint(_buttons[i], pt)) {
			return i;
		}
	}

	return -1;
}

void Scene::loadPlane(Plane &plane, BltFile &bltFile, BltLongId planeId) {
	if (!planeId.isValid()) {
		return;
	}

	BltResource planeRes(bltFile.loadResource(planeId, kBltPlane));
	BltPlane planeInfo(&planeRes[0]);
	plane.image.init(bltFile, planeInfo.imageId);
	plane.palette.reset(bltFile.loadResource(planeInfo.paletteId, kBltPalette));
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

void Scene::loadButton(Button &button, BltFile &bltFile, const byte *src) {
	BltButton buttonInfo(&src[0]);
	button.hotspotType = (Button::HotspotType)buttonInfo.type;
	button.rect = buttonInfo.rect;
	button.plane = buttonInfo.plane;

	if (button.hotspotType == Button::kHotspotRect) {
		button.rect.translate(-_origin.x, -_origin.y);
	}

	if (buttonInfo.graphicsId.isValid()) {
		BltResource buttonGfxRes(bltFile.loadResource(buttonInfo.graphicsId, kBltButtonGraphics));
		// FIXME: Some buttons have multiple graphics entries corresponding to
		// different states of the button.
		BltButtonGraphics buttonGfx(&buttonGfxRes[0]);

		button.graphicsType = (Button::GraphicsType)buttonGfx.type;

		if (buttonGfx.defaultId.isValid()) {
			BltResource defaultRes(bltFile.loadResource(buttonGfx.defaultId, kBltSprites));
			BltSprite defaultButtonImage(&defaultRes[0]);
			button.defaultImagePos = defaultButtonImage.pos;
			button.defaultImagePos.x -= _origin.x;
			button.defaultImagePos.y -= _origin.y;
			button.defaultImage.init(bltFile, defaultButtonImage.imageId);
		}

		if (button.graphicsType == Button::kGfxImages) {
			if (buttonGfx.hoveredId.isValid()) {
				// TODO: Factor out repetitive code
				BltResource hoveredRes(bltFile.loadResource(buttonGfx.hoveredId, kBltSprites));
				BltSprite hoveredButtonImage(&hoveredRes[0]);
				button.hoveredImagePos = hoveredButtonImage.pos;
				button.hoveredImagePos.x -= _origin.x;
				button.hoveredImagePos.y -= _origin.y;
				button.hoveredImage.init(bltFile, hoveredButtonImage.imageId);
			}
			if (buttonGfx.idleId.isValid()) {
				BltResource idleRes(bltFile.loadResource(buttonGfx.idleId, kBltSprites));
				BltSprite idleButtonImage(&idleRes[0]);
				button.idleImagePos = idleButtonImage.pos;
				button.idleImagePos.x -= _origin.x;
				button.idleImagePos.y -= _origin.y;
				button.idleImage.init(bltFile, idleButtonImage.imageId);
			}
		}
		else if (button.graphicsType == Button::kGfxPaletteMods) {
			if (buttonGfx.hoveredId.isValid()) {
				// TODO: Factor out repetitive code
				BltResource hoveredRes(bltFile.loadResource(buttonGfx.hoveredId, kBltButtonPaletteMod));
				BltButtonPaletteMod hoveredPalMod(&hoveredRes[0]);
				button.hoveredPalStart = hoveredPalMod.start;
				button.hoveredPalNum = hoveredPalMod.num;
				button.hoveredColors.reset(bltFile.loadResource(hoveredPalMod.colorsId, kBltButtonColors));
			}
			if (buttonGfx.idleId.isValid()) {
				BltResource idleRes(bltFile.loadResource(buttonGfx.idleId, kBltButtonPaletteMod));
				BltButtonPaletteMod idlePalMod(&idleRes[0]);
				button.idlePalStart = idlePalMod.start;
				button.idlePalNum = idlePalMod.num;
				button.idleColors.reset(bltFile.loadResource(idlePalMod.colorsId, kBltButtonColors));
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
