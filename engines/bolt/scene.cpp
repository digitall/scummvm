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

#include "bolt/bolt.h"

namespace Bolt {

struct BltScene { // type 32
	BltScene(const byte *src) {
		forePlaneId = BltId(READ_BE_UINT32(&src[0]));
		backPlaneId = BltId(READ_BE_UINT32(&src[4]));
		numSprites = src[0x8];
		spritesId = BltId(READ_BE_UINT32(&src[0xA]));
		// FIXME: unknown fields
		colorCyclesId = BltId(READ_BE_UINT32(&src[0x16]));
		numButtons = READ_BE_UINT16(&src[0x1A]);
		buttonsId = BltId(READ_BE_UINT32(&src[0x1C]));
		origin.x = READ_BE_INT16(&src[0x20]);
		origin.y = READ_BE_INT16(&src[0x22]);
	}

	BltId forePlaneId;
	BltId backPlaneId;
	uint8 numSprites;
	BltId spritesId;
	BltId colorCyclesId;
	uint16 numButtons;
	BltId buttonsId;
	Common::Point origin;
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
			slotIds[i] = BltId(READ_BE_UINT32(&src[8 + i * 4]));
		}
	}

	uint16 numSlots[4];
	BltId slotIds[4];
};

void Scene::load(Graphics *graphics, BltFile &boltlib, BltId sceneId)
{
	_graphics = graphics;

	BltScene sceneInfo(&BltResource(boltlib.loadResource(sceneId, kBltScene))[0]);

	_origin = sceneInfo.origin;
	_forePlane.load(boltlib, sceneInfo.forePlaneId);
	_backPlane.load(boltlib, sceneInfo.backPlaneId);
	_sprites.load(boltlib, sceneInfo.spritesId);
	_buttons.load(boltlib, sceneInfo.buttonsId);

	for (int i = 0; i < NUM_COLOR_CYCLES; ++i) {
		_colorCycles[i] = ColorCycle();
	}
	if (sceneInfo.colorCyclesId.isValid()) {
		BltColorCycles cyclesInfo(&BltResource(boltlib.loadResource(
			sceneInfo.colorCyclesId, kBltColorCycles))[0]);
		for (int i = 0; i < NUM_COLOR_CYCLES; ++i) {
			if (cyclesInfo.slotIds[i].isValid()) {
				if (cyclesInfo.numSlots[i] != 1) {
					warning("Cycle slot does not have one palette region (it has %d)...", cyclesInfo.numSlots[i]);
				}
				else if (cyclesInfo.numSlots[i] == 0) {
					// No slots; skip
				}
				else {
					BltColorCycleSlot cycleSlotInfo(&BltResource(boltlib.loadResource(
						cyclesInfo.slotIds[i], kBltColorCycleSlot))[0]);
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
	if (_backPlane->palette) {
		_backPlane->palette.set(*_graphics, BltPalette::kBack);
	}
	if (_backPlane->image) {
		::Graphics::Surface surface = _graphics->getBackPlane().getSurface();
		_backPlane->image.drawAt(surface, 0, 0, false);
	}
	else {
		_graphics->getBackPlane().clear();
	}

	// Draw fore plane
	if (_forePlane->palette) {
		_forePlane->palette.set(*_graphics, BltPalette::kFore);
	}
	if (_forePlane->image) {
		::Graphics::Surface surface = _graphics->getForePlane().getSurface();
		_forePlane->image.drawAt(surface, 0, 0, false);
	}
	else {
		_graphics->getForePlane().clear();
	}

	// Draw sprites
	for (size_t i = 0; i < _sprites.size(); ++i) {
		Common::Point pos = _sprites[i].pos - _origin;
		// FIXME: Are sprites drawn to back or fore plane? Is it somehow selectable?
		::Graphics::Surface surface = _graphics->getForePlane().getSurface();
		_sprites[i].image.drawAt(surface, pos.x, pos.y, true);
	}

	// TODO: move color cycling to Graphics class
#if 0
	// Reset color cycles
	uint32 enterTime = _engine->getTotalPlayTime();
	for (int i = 0; i < NUM_COLOR_CYCLES; ++i) {
		_colorCycles[i].curTime = enterTime;
	}
#endif

	_graphics->markDirty();
}

void Scene::handleHover(const Common::Point &pt) {

	// TODO: move color cycling to Graphics class
#if 0
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
#endif

	// Draw buttons
	int hoveredButton = getButtonAtPoint(pt);
	for (uint i = 0; i < _buttons.size(); ++i) {
		drawButton(_buttons[i], (int)i == hoveredButton);
	}

	_graphics->markDirty();
}

void Scene::setBackPlane(BltFile &bltFile, BltId id) {
	_backPlane.load(bltFile, id);
}

int Scene::getButtonAtPoint(const Common::Point &pt) {
	byte foreHotspotColor = 0;
	if (_forePlane->hotspots) {
		foreHotspotColor = _forePlane->hotspots.query(pt.x, pt.y);
	}

	byte backHotspotColor = 0;
	if (_backPlane->hotspots) {
		backHotspotColor = _backPlane->hotspots.query(pt.x, pt.y);
	}

	for (int i = 0; i < (int)_buttons.size(); ++i) {
		const BltButtonStruct &button = _buttons[i];
		if (button.type == BltButtonStruct::Rectangle) {
			if (button.rect.contains(_origin + pt)) {
				return i;
			}
		}
		else if (button.type == BltButtonStruct::HotspotQuery) {
			byte color = button.plane ? backHotspotColor : foreHotspotColor;
			if (color >= button.rect.left && color <= button.rect.right) {
				return i;
			}
		}
	}

	return -1;
}

Bolt::Plane& Scene::getGraphicsPlane(uint16 num) {
	return num ? _graphics->getBackPlane() : _graphics->getForePlane();
}

void Scene::drawButton(const BltButtonStruct &button, bool hovered) {
	if (button.graphics) {
		const BltButtonGraphicsStruct &buttonGfx = button.graphics[0]; // TODO: support states other than 0
		if (buttonGfx.type == BltButtonGraphicsStruct::PaletteMods) {
			const BltButtonPaletteMod &paletteMod = hovered ? buttonGfx.hoveredPaletteMod : buttonGfx.idlePaletteMod;
			if (paletteMod.colors) {
				getGraphicsPlane(button.plane).setPalette(&paletteMod.colors[0], paletteMod.start, paletteMod.num);
			}
		}
		else if (buttonGfx.type == BltButtonGraphicsStruct::Sprites) {
			::Graphics::Surface surface = getGraphicsPlane(button.plane).getSurface();
			const BltSpriteList &spriteList = hovered ? buttonGfx.hoveredSprites : buttonGfx.idleSprites;
			if (spriteList) {
				const BltSpriteStruct &sprite = spriteList[0];
				Common::Point pos = sprite.pos - _origin;
				if (sprite.image) {
					sprite.image.drawAt(surface, pos.x, pos.y, true);
				}
			}
		}
	}
}

} // End of namespace Bolt
