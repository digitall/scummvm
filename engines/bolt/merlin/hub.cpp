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

#include "bolt/merlin/hub.h"

#include "bolt/merlin/merlin.h"

namespace Bolt {

struct BltHub { // type 40
	BltHub(const byte *src) {
		sceneId = BltLongId(READ_BE_UINT32(&src[0]));
		// FIXME: unknown field at offset 4
		bgPlaneId = BltLongId(READ_BE_UINT32(&src[6]));
		// FIXME: unknown field at offset 0xA
		numItems = src[0xB];
		itemListId = BltLongId(READ_BE_UINT32(&src[0xC]));
	}

	BltLongId sceneId;
	BltLongId bgPlaneId;
	byte numItems;
	BltLongId itemListId;
};

struct BltHubItem { // type 41
	BltHubItem(const byte *src) {
		// FIXME: unknown fields
		imageId = BltLongId(READ_BE_UINT32(&src[4]));
	}

	BltLongId imageId;
};

void HubCard::init(MerlinEngine *merlin, const HubEntry &entry) {
	_merlin = merlin;
	_hubEntry = &entry;

	BltHub hubInfo(&BltResource(merlin->_boltlib.loadResource(BltShortId(entry.hubId), kBltHub))[0]);

	MenuCard::init(merlin->_engine, merlin->_boltlib, hubInfo.sceneId);
	_scene.setBackPlane(merlin->_boltlib, hubInfo.bgPlaneId);

	BltResourceList hubItemsList(merlin->_boltlib, hubInfo.itemListId);
	_itemImages.alloc(hubInfo.numItems);
	for (uint i = 0; i < hubInfo.numItems; ++i) {
		BltHubItem hubItem(&BltResource(merlin->_boltlib.loadResource(
			hubItemsList[i].value, kBltHubItem))[0]);
		_itemImages[i].load(merlin->_boltlib, hubItem.imageId);
	}
}

void HubCard::enter() {
	MenuCard::enter();

	// Draw item images to back plane
	for (uint i = 0; i < _itemImages.size(); ++i) {
		_itemImages[i].drawAt(_merlin->getGraphics().getBackPlane().getSurface(), 0, 0, true);
	}
}

Card::Status HubCard::processButtonClick(int num) {
	if (num >= 0 && num < _hubEntry->numPuzzles) {
		_merlin->setCardEndCallback(MerlinEngine::puzzle, &_hubEntry->puzzles[num]);
		return Ended;
	}

	// If no button was clicked, complete stage and transition to next hub.
	return Ended;
}

} // End of namespace Bolt
