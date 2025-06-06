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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "drascula/drascula.h"
#include "graphics/cursorman.h"

#include "common/text-to-speech.h"

// The verbs are represented in-game as a picture, thus we are
// adding transcriptions here
// While only English, Spanish, Italian, and Russian are translated in-game,
// they are translated for the TTS system here
static const char *verbNamesEnglish[] = {
	"Walk",
	"Look",
	"Take",
	"Open",
	"Close",
	"Talk",
	"Push"
};

static const char *verbNamesSpanish[] = {
	"Ir a",
	"Mirar",
	"Coger",
	"Abrir",
	"Cerrar",
	"Hablar",
	"Mover"
};

static const char *verbNamesItalian[] = {
	"Vai",
	"Guarda",
	"Prendi",
	"Apri",
	"Chiudi",
	"Parla",
	"Premi"
};

static const char *verbNamesFrench[] = {
	"Marcher",
	"Regarder",
	"Ramasser",
	"Ouvrir",
	"Fermer",
	"Parler",
	"Pousser"
};

static const char *verbNamesGerman[] = {
	"Gehe",
	"Schau",
	"Nimm",
	"\231ffne",
	"Schlie\341e",
	"Rede",
	"Dr\201cke"
};

static const char *verbNamesRussian[] = {
	"\xc8\xe4\xf2\xe8",					// "Идти"
	"\xd1\xec\xee\xf2\xf0\xe5\xf2\xfc",	// "Смотреть"
	"\xc2\xe7\xff\xf2\xfc",				// "Взять"
	"\xce\xf2\xea\xf0\xfb\xf2\xfc",		// "Открыть"
	"\xc7\xe0\xea\xf0\xfb\xf2\xfc",		// "Закрыть"
	"\xc3\xee\xe2\xee\xf0\xe8\xf2\xfc",	// "Говорить"
	"\xd2\xee\xeb\xea\xe0\xf2\xfc"		// "Толкать"
};

static const int kConfirmExit = 1;

namespace Drascula {

void DrasculaEngine::setCursor(int cursor) {
	switch (cursor) {
	case kCursorCrosshair:
		CursorMan.replaceCursor(crosshairCursor, 40, 25, 20, 17, 255);
		break;
	case kCursorCurrentItem:
		CursorMan.replaceCursor(mouseCursor, OBJWIDTH, OBJHEIGHT, 20, 17, 255);
	default:
		break;
	}
}

void DrasculaEngine::showCursor() {
	CursorMan.showMouse(true);
}

void DrasculaEngine::hideCursor() {
	CursorMan.showMouse(false);
}

bool DrasculaEngine::isCursorVisible() {
	return CursorMan.isVisible();
}

void DrasculaEngine::selectVerbFromBar() {
	for (int n = 0; n < 7; n++) {
		if (_mouseX > _verbBarX[n] && _mouseX < _verbBarX[n + 1] && n > 0) {
			selectVerb(n);
			return;
		}
	}

	// no verb selected
	selectVerb(kVerbNone);
}

void DrasculaEngine::selectVerb(int verb) {
	debug(4, "selectVerb(%d)", verb);

	int c = _menuScreen ? 0 : 171;

	if (currentChapter == 5) {
		if (takeObject == 1 && pickedObject != 16)
			addObject(pickedObject);
	} else {
		if (takeObject == 1)
			addObject(pickedObject);
	}

	for (int i = 0; i < OBJHEIGHT; i++)
		memcpy(mouseCursor + i * OBJWIDTH, cursorSurface + OBJWIDTH * verb + (c + i) * 320, OBJWIDTH);
	setCursor(kCursorCurrentItem);

	if (verb > 0) {
		takeObject = 1;
		pickedObject = verb;

		const char **verbNames;

		switch (_lang) {
		case kEnglish:
			verbNames = verbNamesEnglish;
			break;
		case kSpanish:
			verbNames = verbNamesSpanish;
			break;
		case kGerman:
			verbNames = verbNamesGerman;
			break;
		case kFrench:
			verbNames = verbNamesFrench;
			break;
		case kItalian:
			verbNames = verbNamesItalian;
			break;
		case kRussian:
			verbNames = verbNamesRussian;
			break;
		default:
			verbNames = verbNamesEnglish;
		}

		sayText(verbNames[verb], Common::TextToSpeechManager::INTERRUPT);
	} else {
		takeObject = 0;
		_hasName = false;
	}
}

bool DrasculaEngine::confirmExit() {
	byte key = 0;

	color_abc(kColorRed);
	updateRoom();
	centerText(_textsys[1], 160, 87);
	updateScreen();

	sayText(_textsys[kConfirmExit], Common::TextToSpeechManager::INTERRUPT);

	delay(100);
	while (!shouldQuit()) {
		key = getScan();
		if (key != 0)
			break;

		// This gives a better feedback to the user when he is asked to
		// confirm whether he wants to quit. It now still updates the room and
		// shows mouse cursor movement. Hopefully it will work in all
		// locations of the game.
		updateRoom();
		color_abc(kColorRed);
		centerText(_textsys[1], 160, 87);
		updateScreen();
	}

	if (key == Common::KEYCODE_ESCAPE || shouldQuit()) {
		stopMusic();
		return false;
	}

	return true;
}

void DrasculaEngine::showMenu() {
	int h, n, x;
	byte *srcSurface = (currentChapter == 6) ? tableSurface : frontSurface;
	x = whichObject();

	for (n = 1; n < ARRAYSIZE(inventoryObjects); n++) {
		h = inventoryObjects[n];

		if (h != 0) {
			copyBackground(_polX[n], _polY[n], _itemLocations[n].x, _itemLocations[n].y,
							OBJWIDTH, OBJHEIGHT, srcSurface, screenSurface);
		}
		copyRect(_x1d_menu[h], _y1d_menu[h], _itemLocations[n].x, _itemLocations[n].y,
				OBJWIDTH, OBJHEIGHT, cursorSurface, screenSurface);
	}

	if (x < 7) {
		sayText(iconName[x], Common::TextToSpeechManager::INTERRUPT);

		print_abc(iconName[x], _itemLocations[x].x - 2, _itemLocations[x].y - 7);
	}
}

void DrasculaEngine::clearMenu() {
	int n, verbActivated = 1;

	for (n = 0; n < 7; n++) {
		if (_mouseX > _verbBarX[n] && _mouseX < _verbBarX[n + 1]) {
			verbActivated = 0;

			const char **verbNames;

			switch (_lang) {
			case kEnglish:
				verbNames = verbNamesEnglish;
				break;
			case kSpanish:
				verbNames = verbNamesSpanish;
				break;
			case kGerman:
				verbNames = verbNamesGerman;
				break;
			case kFrench:
				verbNames = verbNamesFrench;
				break;
			case kItalian:
				verbNames = verbNamesItalian;
				break;
			case kRussian:
				verbNames = verbNamesRussian;
				break;
			default:
				verbNames = verbNamesEnglish;
			}

			sayText(verbNames[n], Common::TextToSpeechManager::INTERRUPT);
		}
		
		copyRect(OBJWIDTH * n, OBJHEIGHT * verbActivated, _verbBarX[n], 2,
						OBJWIDTH, OBJHEIGHT, cursorSurface, screenSurface);
		verbActivated = 1;
	}
}

bool DrasculaEngine::checkMenuFlags() {
	int n = whichObject();
	if (n != 0) {
		if (inventoryObjects[n] != 0 && checkAction(inventoryObjects[n]))
			return true;
	}

	return false;
}

void DrasculaEngine::showMap() {
	_hasName = false;

	for (int l = 0; l < numRoomObjs; l++) {
		if (_objectRect[l].contains(Common::Point(_mouseX, _mouseY)) && visible[l] == 1) {
			Common::strcpy_s(textName, objName[l]);
			_hasName = true;
		}
	}
}

} // End of namespace Drascula
