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

void Scene::load(Graphics *graphics, Boltlib &boltlib, BltId sceneId)
{
	_graphics = graphics;

	BltScene sceneInfo(&BltResource(boltlib.loadResource(sceneId, kBltScene))[0]);

	_origin = sceneInfo.origin;
	_forePlane.load(boltlib, sceneInfo.forePlaneId);
	_backPlane.load(boltlib, sceneInfo.backPlaneId);
	_sprites.load(boltlib, sceneInfo.spritesId);
	_buttons.load(boltlib, sceneInfo.buttonsId);

	_colorCycles.reset();
	if (sceneInfo.colorCyclesId.isValid()) {
		_colorCycles.reset(new BltColorCycles);
		_colorCycles->load(boltlib, sceneInfo.colorCyclesId);
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

	_graphics->resetColorCycles();
	if (_colorCycles) {
		for (int i = 0; i < 4; ++i) {
			BltColorCycleSlot *slot = (*_colorCycles)->slots[i].get();
			if ((*_colorCycles)->numSlots[i] == 1 && slot) {
				if ((*slot)->frames <= 0) {
					warning("Invalid color cycle frames");
				}
				else {
					if ((*slot)->end < (*slot)->start) {
						warning("color cycle end > start; backwards cycle? not supported.");
					}
					if ((*slot)->plane != 0) {
						warning("Color cycle plane was not 0");
					}
					_graphics->setColorCycle(i, (*slot)->start, (*slot)->end - (*slot)->start + 1,
						(*slot)->frames * 1000 / 60);
				}
			}
		}
	}

	// Draw sprites
	for (size_t i = 0; i < _sprites.size(); ++i) {
		Common::Point pos = _sprites[i].pos - _origin;
		// FIXME: Are sprites drawn to back or fore plane? Is it somehow selectable?
		::Graphics::Surface surface = _graphics->getForePlane().getSurface();
		_sprites[i].image.drawAt(surface, pos.x, pos.y, true);
	}

	_graphics->markDirty();
}

void Scene::handleHover(const Common::Point &pt) {
	// Draw buttons
	int hoveredButton = getButtonAtPoint(pt);
	for (uint i = 0; i < _buttons.size(); ++i) {
		drawButton(_buttons[i], (int)i == hoveredButton);
	}

	_graphics->markDirty();
}

void Scene::setBackPlane(Boltlib &bltFile, BltId id) {
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
