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
		sceneId = BltId(READ_BE_UINT32(&src[0]));
		// FIXME: unknown field at offset 4
		bgPlaneId = BltId(READ_BE_UINT32(&src[6]));
		// FIXME: unknown field at offset 0xA
		numItems = src[0xB];
		itemListId = BltId(READ_BE_UINT32(&src[0xC]));
	}

	BltId sceneId;
	BltId bgPlaneId;
	byte numItems;
	BltId itemListId;
};

struct BltHubItem { // type 41
	BltHubItem(const byte *src) {
		// FIXME: unknown fields
		imageId = BltId(READ_BE_UINT32(&src[4]));
	}

	BltId imageId;
};

void HubCard::init(Graphics *graphics, Boltlib &boltlib, BltId resId) {
	_graphics = graphics;

	BltHub hubInfo(&BltResource(boltlib.loadResource(resId, kBltHub))[0]);

	MenuCard::init(_graphics, boltlib, hubInfo.sceneId);
	_scene.setBackPlane(boltlib, hubInfo.bgPlaneId);

	BltResourceList hubItemsList(boltlib, hubInfo.itemListId);
	_itemImages.alloc(hubInfo.numItems);
	for (uint i = 0; i < hubInfo.numItems; ++i) {
		BltHubItem hubItem(&BltResource(boltlib.loadResource(
			hubItemsList[i].value, kBltHubItem))[0]);
		_itemImages[i].load(boltlib, hubItem.imageId);
	}
}

void HubCard::enter(uint32 time) {
	MenuCard::enter(time);

	// Draw item images to back plane
	// FIXME: Only draw items that are unlocked.
	for (uint i = 0; i < _itemImages.size(); ++i) {
		_itemImages[i].drawAt(_graphics->getPlaneSurface(kBack), 0, 0, true);
	}
}

Card::Signal HubCard::handleButtonClick(int num) {
	if (num == -1) {
		// If no button was clicked, complete stage and transition to next hub.
		return kEnd;
	}
	else {
		return (Signal)(kEnterPuzzleX + num);
	}
}

} // End of namespace Bolt
