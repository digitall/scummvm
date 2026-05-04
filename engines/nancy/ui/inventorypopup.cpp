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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "engines/nancy/cursor.h"
#include "engines/nancy/graphics.h"
#include "engines/nancy/input.h"
#include "engines/nancy/nancy.h"
#include "engines/nancy/resource.h"
#include "engines/nancy/sound.h"

#include "engines/nancy/state/scene.h"

#include "engines/nancy/ui/inventorypopup.h"

namespace Nancy {
namespace UI {

InventoryPopup::InventoryPopup() :
		// z=12: above the viewport (6) and the taskbar (7). All Nancy
		// 10+ taskbar popups render on top of the entire scene UI.
		RenderObject(12),
		_uiivData(nullptr),
		_invData(nullptr),
		_isOpen(false),
		_activeFilter(kFilterAll),
		_currentPage(0) {
	for (uint i = 0; i < kSlotsPerPage; ++i) {
		_slotItemIDs[i] = -1;
	}
}

void InventoryPopup::init() {
	_uiivData = GetEngineData(UIIV);
	assert(_uiivData);

	_invData = GetEngineData(INV);
	assert(_invData);

	// Popup background image (e.g. "UIInv_OVL").
	g_nancy->_resource->loadImage(_uiivData->header.imageName, _overlayImage);

	// Per-item icon sheet shared with the legacy InventoryBox.
	g_nancy->_resource->loadImage(_invData->inventoryBoxIconsImageName, _itemIcons);

	// Position the popup using the "normal-size" rects from the popup
	// header (popup state 2).
	Common::Rect popupRect = _uiivData->header.normalDestRect;
	if (_uiivData->header.overlayInGameFrame) {
		const VIEW *view = GetEngineData(VIEW);
		if (view) {
			popupRect.translate(view->screenPosition.left, view->screenPosition.top);
		}
	}
	moveTo(popupRect);
	Common::Rect bounds = _screenPosition;
	bounds.moveTo(0, 0);
	_drawSurface.create(bounds.width(), bounds.height(), g_nancy->_graphics->getInputPixelFormat());

	drawBackground();
	drawFilterTabs();

	setTransparent(false);
	setVisible(false);

	RenderObject::init();
}

void InventoryPopup::registerGraphics() {
	RenderObject::registerGraphics();
}

void InventoryPopup::open() {
	if (_isOpen) {
		return;
	}
	_isOpen = true;
	_currentPage = 0;

	rebuildVisibleList();
	refreshGrid();

	setVisible(true);

	if (!_uiivData->header.sounds[0].name.empty()) {
		g_nancy->_sound->loadSound(_uiivData->header.sounds[0]);
		g_nancy->_sound->playSound(_uiivData->header.sounds[0]);
	}
}

void InventoryPopup::close() {
	if (!_isOpen) {
		return;
	}
	_isOpen = false;

	setVisible(false);

	if (!_uiivData->header.sounds[1].name.empty()) {
		g_nancy->_sound->loadSound(_uiivData->header.sounds[1]);
		g_nancy->_sound->playSound(_uiivData->header.sounds[1]);
	}
}

void InventoryPopup::refreshGrid() {
	rebuildVisibleList();

	// Re-blit the popup background; this also wipes any previous slot icons.
	drawBackground();
	drawFilterTabs();

	for (uint i = 0; i < kSlotsPerPage; ++i) {
		const uint listIndex = _currentPage * kSlotsPerPage + i;
		const int16 itemID = (listIndex < _visibleItems.size()) ? _visibleItems[listIndex] : -1;
		_slotItemIDs[i] = itemID;
		drawSlot(i, itemID);
	}

	_needsRedraw = true;
}

void InventoryPopup::rebuildVisibleList() {
	_visibleItems.clear();

	const uint16 numItems = MIN<uint16>(g_nancy->getStaticData().numItems,
										_invData->itemDescriptions.size());

	for (uint16 id = 0; id < numItems; ++id) {
		if (NancySceneState.hasItem(id) != g_nancy->_true) {
			continue;
		}

		const INV::ItemDescription &desc = _invData->itemDescriptions[id];
		bool include;

		switch (_activeFilter) {
		case kFilterDocuments:
			include = (desc.keepItem == 3);
			break;
		case kFilterUsable:
			include = (desc.keepItem <= 2);
			break;
		case kFilterSpecial1:
		case kFilterSpecial2:
		case kFilterSpecial3:
			// Specialty filters
			// TODO: Show nothing, for now
			include = false;
			break;
		case kFilterAll:
		default:
			include = true;
			break;
		}

		if (include) {
			_visibleItems.push_back(id);
		}
	}
}

void InventoryPopup::drawBackground() {
	Common::Rect src = _uiivData->header.normalSrcRect;
	_drawSurface.blitFrom(_overlayImage, src, Common::Point(0, 0));
}

void InventoryPopup::drawFilterTabs() {
	for (const auto &filter : _uiivData->filters) {
		if (!filter.enabled || filter.button.sourceRects[0].isEmpty()) {
			continue;
		}

		Common::Rect dst = filter.button.destRect;
		dst.translate(-_screenPosition.left, -_screenPosition.top);

		// Draw the idle sprite from the button's primary image; fall back
		// to the overlay image if no primary image is loaded for the slot.
		_drawSurface.blitFrom(_overlayImage, filter.button.sourceRects[0], Common::Point(dst.left, dst.top));
	}
}

void InventoryPopup::drawSlot(uint slotIndex, int16 itemID) {
	if (slotIndex >= _uiivData->slotDestRects.size()) {
		return;
	}

	Common::Rect dst = _uiivData->slotDestRects[slotIndex];
	dst.translate(-_screenPosition.left, -_screenPosition.top);

	if (itemID < 0 || itemID >= (int16)_invData->itemDescriptions.size()) {
		// Empty slot — leave the popup-background pixels in place.
		return;
	}

	const INV::ItemDescription &desc = _invData->itemDescriptions[itemID];
	if (desc.sourceRect.isEmpty()) {
		return;
	}

	_drawSurface.blitFrom(_itemIcons, desc.sourceRect, Common::Point(dst.left, dst.top));
}

void InventoryPopup::handleInput(NancyInput &input) {
	if (!_isOpen) {
		return;
	}

	// Hit-test slots against the on-screen DEST rects (UIIV stores them in
	// absolute screen coords, same as button rects).
	int newHovered = -1;
	for (uint i = 0; i < kSlotsPerPage; ++i) {
		if (i >= _uiivData->slotDestRects.size()) {
			break;
		}
		if (_slotItemIDs[i] < 0) {
			continue;
		}
		if (_uiivData->slotDestRects[i].contains(input.mousePos)) {
			newHovered = (int)i;
			break;
		}
	}

	if (newHovered != -1) {
		g_nancy->_cursor->setCursorType(CursorManager::kHotspotArrow);

		if (input.input & NancyInput::kLeftMouseButtonUp) {
			const int16 itemID = _slotItemIDs[newHovered];
			if (itemID >= 0) {
				// Pick the item up: it leaves the inventory and becomes
				// the held item (cursor sprite)
				NancySceneState.removeItemFromInventory(itemID, true);
				refreshGrid();
				close();
				input.eatMouseInput();
				return;
			}
		}
	}

	// Filter tabs: clicking switches the active filter.
	for (const auto &filter : _uiivData->filters) {
		if (!filter.enabled) {
			continue;
		}
		if (!filter.button.destRect.contains(input.mousePos)) {
			continue;
		}

		g_nancy->_cursor->setCursorType(CursorManager::kHotspotArrow);

		if (input.input & NancyInput::kLeftMouseButtonUp) {
			_activeFilter = static_cast<FilterID>(filter.id);
			_currentPage = 0;
			refreshGrid();
			input.eatMouseInput();
			return;
		}
		break;
	}

	// While the popup is open, swallow clicks that fall on the popup so
	// the underlying scene/viewport doesn't react.
	if (_screenPosition.contains(input.mousePos)) {
		input.eatMouseInput();
	}
}

} // End of namespace UI
} // End of namespace Nancy
