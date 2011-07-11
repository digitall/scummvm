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

#ifndef CRUISE_MENU_H
#define CRUISE_MENU_H

namespace Cruise {
struct menuElementSubStruct {
	struct menuElementSubStruct *pNext;
	int16 ovlIdx;
	int16 header;
};

struct menuElementStruct {
	struct menuElementStruct *next;
	const char *string;
	int x;
	int y;
	int varA;
	bool selected;
	unsigned char color;
	gfxEntryStruct *gfx;
	menuElementSubStruct *ptrSub;
};

class Menu {
public:
	const char *_stringPtr;
	gfxEntryStruct *_gfx;
	int _x;
	int _y;
	int _numElements;
	menuElementStruct *_ptrNextElement;

	Menu(int X, int Y, const char *menuName);
	~Menu();
	void addSelectableMenuEntry(int var0, int var1, int var2, int color,
								const char *menuText);
	void updateMouse(int mouseX, int mouseY);
	int process();
	menuElementSubStruct *getSelectedEntry(menuElementStruct* pSelectedElement);
	menuElementSubStruct *getSelectedEntry();
};

int playerMenu(int menuX, int menuY);

} // End of namespace Cruise

#endif
