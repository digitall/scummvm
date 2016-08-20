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
#include "bolt/boltlib/palette.h"

namespace Bolt {

struct BltScene { // type 32
	static const uint32 kType = kBltScene;
	static const uint32 kSize = 0x24;
	void load(const ConstSizedDataView<kSize> src, Boltlib &boltlib) {
		forePlaneId = BltId(src.readUint32BE(0));
		backPlaneId = BltId(src.readUint32BE(4));
		numSprites = src.readUint8(0x8);
		spritesId = BltId(src.readUint32BE(0xA));
		// FIXME: unknown fields at 0xD..0x16
		colorCyclesId = BltId(src.readUint32BE(0x16));
		numButtons = src.readUint16BE(0x1A);
		buttonsId = BltId(src.readUint32BE(0x1C));
		origin.x = src.readInt16BE(0x20);
		origin.y = src.readInt16BE(0x22);
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

	BltScene sceneInfo;
	loadBltResource(sceneInfo, boltlib, sceneId);

	_origin = sceneInfo.origin;
	loadBltResource(_forePlane, boltlib, sceneInfo.forePlaneId);
	loadBltResource(_backPlane, boltlib, sceneInfo.backPlaneId);
	loadBltResourceArray(_sprites, boltlib, sceneInfo.spritesId);
	loadBltResourceArray(_buttons, boltlib, sceneInfo.buttonsId);

	_colorCycles.reset();
	if (sceneInfo.colorCyclesId.isValid()) {
		_colorCycles.reset(new BltColorCycles);
		loadBltResource(*_colorCycles, boltlib, sceneInfo.colorCyclesId);
	}
}

void Scene::enter() {
	applyPalette(_graphics, kBack, _backPlane.palette);
	if (_backPlane.image) {
		_backPlane.image.drawAt(_graphics->getPlaneSurface(kBack), 0, 0, false);
	}
	else {
		_graphics->clearPlane(kBack);
	}

	applyPalette(_graphics, kFore, _forePlane.palette);
	if (_forePlane.image) {
		_forePlane.image.drawAt(_graphics->getPlaneSurface(kFore), 0, 0, false);
	}
	else {
		_graphics->clearPlane(kFore);
	}

	applyColorCycles(_graphics, _colorCycles.get());

	// Draw sprites
	for (size_t i = 0; i < _sprites.size(); ++i) {
		Common::Point pos = _sprites[i].pos - _origin;
		// FIXME: Are sprites drawn to back or fore plane? Is it somehow selectable?
		_sprites[i].image.drawAt(_graphics->getPlaneSurface(kFore), pos.x, pos.y, true);
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

void Scene::setBackPlane(Boltlib &boltlib, BltId id) {
	loadBltResource(_backPlane, boltlib, id);
}

int Scene::getButtonAtPoint(const Common::Point &pt) {
	byte foreHotspotColor = 0;
	if (_forePlane.hotspots) {
		foreHotspotColor = _forePlane.hotspots.query(pt.x, pt.y);
	}

	byte backHotspotColor = 0;
	if (_backPlane.hotspots) {
		backHotspotColor = _backPlane.hotspots.query(pt.x, pt.y);
	}

	for (int i = 0; i < (int)_buttons.size(); ++i) {
		const BltButtonElement &button = _buttons[i];
		if (button.type == BltButtonElement::Rectangle) {
			if (button.rect.contains(_origin + pt)) {
				return i;
			}
		}
		else if (button.type == BltButtonElement::HotspotQuery) {
			byte color = button.plane ? backHotspotColor : foreHotspotColor;
			if (color >= button.rect.left && color <= button.rect.right) {
				return i;
			}
		}
	}

	return -1;
}

void Scene::drawButton(const BltButtonElement &button, bool hovered) {
	if (button.graphics) {
		const BltButtonGraphicElement &buttonGfx = button.graphics[0]; // TODO: support states other than 0
		if (buttonGfx.type == BltButtonGraphicElement::PaletteMods) {
			const BltPaletteMods &paletteMod = hovered ? buttonGfx.hoveredPaletteMod : buttonGfx.idlePaletteMod;
			applyPaletteMod(_graphics, button.plane, paletteMod, 0);
		}
		else if (buttonGfx.type == BltButtonGraphicElement::Sprites) {
			const BltSpriteList &spriteList = hovered ? buttonGfx.hoveredSprites : buttonGfx.idleSprites;
			if (spriteList) {
				const BltSpriteElement &sprite = spriteList[0];
				Common::Point pos = sprite.pos - _origin;
				if (sprite.image) {
					sprite.image.drawAt(_graphics->getPlaneSurface(button.plane), pos.x, pos.y, true);
				}
			}
		}
	}
}

} // End of namespace Bolt
