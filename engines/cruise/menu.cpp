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

#include "cruise/cruise.h"
#include "cruise/cruise_main.h"
#include "cruise/staticres.h"

#include "engines/metaengine.h"
#include "gui/saveload.h"
#include "common/system.h"
#include "common/translation.h"

namespace Cruise {

extern int currentMouseButton;

Menu::Menu(int X, int Y, const char *menuName) {
	_x = X - 160 / 2;
	_y = Y;
	_stringPtr = menuName;
	_numElements = 0;
	_ptrNextElement = NULL;
	_gfx = renderText(160, menuName);
}

void Menu::addSelectableMenuEntry(int ovlIdx, int headerIdx, int param2, int color, const char *menuText) {
	menuElementStruct *pCurrentElement;
	menuElementStruct *pNewElement;
	menuElementSubStruct *pNewSubStruct;
	menuElementSubStruct *pCurrentSubStruct;

	if (_numElements <= 48) {
		pCurrentElement = _ptrNextElement;

		while (pCurrentElement && pCurrentElement->next) {
			if (param2) {
				if (!strcmp(pCurrentElement->string, menuText)) {
					pNewSubStruct = (menuElementSubStruct *)allocAndZero(sizeof(menuElementSubStruct));
					ASSERT(pNewSubStruct);

					pNewSubStruct->pNext = NULL;
					pNewSubStruct->ovlIdx = ovlIdx;
					pNewSubStruct->header = headerIdx;

					pCurrentSubStruct = pCurrentElement->ptrSub;

					if (!pCurrentSubStruct) {
						pCurrentElement->ptrSub = pNewSubStruct;
						return;
					}

					while (pCurrentSubStruct->pNext) {
						pCurrentSubStruct = pCurrentSubStruct->pNext;
					}

					pCurrentSubStruct->pNext = pNewSubStruct;
					return;
				}
			}
			pCurrentElement = pCurrentElement->next;
		}

		pNewElement = (menuElementStruct *)allocAndZero(sizeof(menuElementStruct));
		ASSERT(pNewElement);
		pNewSubStruct = (menuElementSubStruct *)allocAndZero(sizeof(menuElementSubStruct));
		ASSERT(pNewSubStruct);

		pNewElement->string = menuText;
		pNewElement->next = NULL;
		pNewElement->selected = false;
		pNewElement->color = color;
		pNewElement->gfx = renderText(160, menuText);

		if (pCurrentElement == NULL) {
			_ptrNextElement = pNewElement;
		} else {
			pCurrentElement->next = pNewElement;
		}

		pNewElement->ptrSub = pNewSubStruct;

		pNewSubStruct->pNext = NULL;
		pNewSubStruct->ovlIdx = ovlIdx;
		pNewSubStruct->header = headerIdx;

		_numElements++;
	}
}

void Menu::updateMouse(int mouseX, int mouseY) {
	if (_gfx) {
		int height = _gfx->height;	// rustine
		int var_2 = 0;
		menuElementStruct *pCurrentEntry = _ptrNextElement;

		while (pCurrentEntry) {
			pCurrentEntry->selected = false;

			if (var_2 == 0) {
				if ((mouseX > pCurrentEntry->x) && ((pCurrentEntry->x + 160) >= mouseX)) {
					if ((mouseY > pCurrentEntry->y) && ((pCurrentEntry->y + height) >= mouseY)) {
						var_2 = 1;
						pCurrentEntry->selected = true;
					}
				}
			}

			pCurrentEntry = pCurrentEntry->next;
		}
	}
}

bool manageEvents();

int Menu::process() {
	int16 mouseX;
	int16 mouseY;
	int16 mouseButton;
	int di;
	int si;
	currentActiveMenu = 0;

	mainDraw(1);
	flipScreen();

	di = 0;
	si = 0;

	do {
		currentMouse.getStatus(&main10, &mouseX, &mouseButton, &mouseY);

		updateMouse(mouseX, mouseY);

		if (mouseButton) {
			if (di) {
				si = 1;
			}
		} else {
			di = 1;
		}

		mainDraw(1);
		flipScreen();

		manageEvents();
		g_system->delayMillis(10);

//    readKeyboard();
	} while (!si);

	currentActiveMenu = -1;

	mainDraw(1);
	flipScreen();

	if (mouseButton & 1) {
		menuElementSubStruct* pSelectedEntry = getSelectedEntry();

		if (pSelectedEntry) {
			return pSelectedEntry->header;
		} else {
			return -1;
		}
	}

	return -1;
}

static void handleSaveLoad(bool saveFlag) {
	const EnginePlugin *plugin = 0;
	EngineMan.findGame(_vm->getGameId(), &plugin);
	GUI::SaveLoadChooser *dialog;
	if (saveFlag)
		dialog = new GUI::SaveLoadChooser(_("Save game:"), _("Save"));
	else
		dialog = new GUI::SaveLoadChooser(_("Load game:"), _("Load"));

	dialog->setSaveMode(saveFlag);
	int slot = dialog->runModalWithPluginAndTarget(plugin, ConfMan.getActiveDomainName());

	if (slot >= 0) {
		if (!saveFlag)
			_vm->loadGameState(slot);
		else {
			Common::String result(dialog->getResultString());
			if (result.empty()) {
				// If the user was lazy and entered no save name, come up with a default name.
				result = Common::String::format("Save %d", slot + 1);
			}

			_vm->saveGameState(slot, result);
		}
	}

	delete dialog;
}

// this function does not look like it belongs to here.
int playerMenu(int menuX, int menuY) {
	int retourMenu;
	//int restartGame = 0;

	if (playerMenuEnabled && displayOn) {
		if (remdo) {
			_vm->sound().stopMusic();
			freeStuff2();
		}
		/*
		    if (currentMenu) {
		      freeMenu(currentMenu);
		      currentMenu = 0;
		      selectDown = 0;
		      menuDown = 0;
		      main9 = -1;
		    }

		    if (inventoryMenu) {
		      freeMenu(inventoryMenu);
		      inventoryMenu = 0;
		      selectDown = 0;
		      menuDown = 0;
		      main9 = -1;
		    }*/

		/*    if (mouseVar2) {
		      free3(mouseVar2);
		    } */

		/*    mouseVar2 = 0;
		    linkedRelation = 0; */
		freeDisk();

		menuTable[0] = new Menu(menuX, menuY, _vm->langString(ID_PLAYER_MENU));
		ASSERT(menuTable[0]);

		//addSelectableMenuEntry(0, 3, menuTable[0], 1, -1, "Save game disk");
		if (userEnabled) {
			menuTable[0]->addSelectableMenuEntry(0, 4, 1, -1, _vm->langString(ID_SAVE));
		}
		menuTable[0]->addSelectableMenuEntry(0, 5, 1, -1, _vm->langString(ID_LOAD));
		menuTable[0]->addSelectableMenuEntry(0, 6, 1, -1, _vm->langString(ID_RESTART));
		menuTable[0]->addSelectableMenuEntry(0, 7, 1, -1, _vm->langString(ID_QUIT));

		retourMenu = menuTable[0]->process();

		delete menuTable[0];
		menuTable[0] = NULL;
		currentMouse._button = 0;

		switch (retourMenu) {
		case 3: // select save drive
			break;
		case 4: // save
		case 5: // load
			handleSaveLoad(retourMenu == 4);
			break;
		case 6: // restart
			_vm->sound().fadeOutMusic();
			Op_FadeOut();
			memset(globalScreen, 0, 320 * 200);
			initVars();
			_vm->initAllData();
			currentMouse.changeCursor(CURSOR_NORMAL);
			userEnabled = 0;
			break;
		case 7: // exit
			return 1;
		}
	}

	return 0;
}

Menu::~Menu() {
	menuElementStruct *pElement = _ptrNextElement;

	while (pElement) {
		menuElementStruct *next;
		menuElementSubStruct *pSub = pElement->ptrSub;

		next = pElement->next;

		while (pSub) {
			menuElementSubStruct *nextSub;

			nextSub = pSub->pNext;

			MemFree(pSub);

			pSub = nextSub;
		}

		if (pElement->gfx) {
			freeGfx(pElement->gfx);
		}

		MemFree(pElement);

		pElement = next;
	}

	freeGfx(_gfx);
}

menuElementSubStruct *Menu::getSelectedEntry(menuElementStruct &pSelectedElement) {
	menuElementStruct *pMenuElement;
	if (_numElements == 0) {
		return NULL;
	}

	pMenuElement = _ptrNextElement;

	while (pMenuElement) {
		if (pMenuElement->selected) {
			pSelectedElement = *pMenuElement;
			return pMenuElement->ptrSub;
		}

		pMenuElement = pMenuElement->next;
	}
	return NULL;
}

menuElementSubStruct *Menu::getSelectedEntry() {
	menuElementStruct dummy;
	return getSelectedEntry(dummy);
}

} // End of namespace Cruise
