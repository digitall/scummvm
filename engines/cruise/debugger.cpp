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

#include "cruise/debugger.h"
#include "cruise/cell.h"
#include "cruise/cruise_main.h"
#include "cruise/object.h"
#include "cruise/overlay.h"

namespace Cruise {

Debugger::Debugger(): GUI::Debugger() {
	DCmd_Register("continue",			WRAP_METHOD(Debugger, Cmd_Exit));
	DCmd_Register("hotspots",				WRAP_METHOD(Debugger, cmd_hotspots));
	DCmd_Register("items",				WRAP_METHOD(Debugger, cmd_items));
}

/**
 * Preliminary command to list the currently loaded hotspots
 */
bool Debugger::cmd_hotspots(int argc, const char **argv) {
	const char *pObjType;
	objectParamsQuery params;

	CellListNode *currentObject = cellHead._prev;

	while (currentObject) {
		if (currentObject->_cell->_overlay > 0 && overlayTable[currentObject->_cell->_overlay].alreadyLoaded &&
			(currentObject->_cell->_type == OBJ_TYPE_SPRITE || currentObject->_cell->_type == OBJ_TYPE_MASK ||
			currentObject->_cell->_type == OBJ_TYPE_EXIT || currentObject->_cell->_type == OBJ_TYPE_VIRTUAL)) {
			const char *pObjectName = getObjectName(currentObject->_cell->_idx, overlayTable[currentObject->_cell->_overlay].ovlData->arrayNameObj);

			switch (currentObject->_cell->_type) {
			case OBJ_TYPE_SPRITE:
				pObjType = "SPRITE";
				break;
			case OBJ_TYPE_MASK:
				pObjType = "MASK";
				break;
			case OBJ_TYPE_EXIT:
				pObjType = "EXIT";
				break;
			case OBJ_TYPE_VIRTUAL:
				pObjType = "VIRTUAL";
				break;
			default:
				pObjType = "UNKNOWN";
				break;
			}

			if (*pObjectName) {
				getMultipleObjectParam(currentObject->_cell->_overlay, currentObject->_cell->_idx, &params);

				DebugPrintf("%s %s - %d,%d\n", pObjectName, pObjType, params.X, params.Y);
			}
		}

		currentObject = currentObject->_prev;
	}

	return true;
}

/**
 * Preliminary command to list the current items in the player's inventory
 */
bool Debugger::cmd_items(int argc, const char **argv) {
	for (int i = 1; i < numOfLoadedOverlay; i++) {
		ovlDataStruct *pOvlData = overlayTable[i].ovlData;

		if (overlayTable[i].alreadyLoaded) {
			if (overlayTable[i].ovlData->arrayObject) {
				for (int j = 0; j < pOvlData->numObj; j++) {
					if (getObjectClass(i, j) != 3) {
						int16 returnVar;

						getSingleObjectParam(i, j, 5, &returnVar);

						if (returnVar < -1)
							DebugPrintf("%s\n", getObjectName(j, pOvlData->arrayNameObj));
					}
				}
			}
		}
	}

	return true;
}

} // End of namespace Cruise
