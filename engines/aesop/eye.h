#ifndef EYE_H
#define EYE_H

#include "common/scummsys.h"

namespace Aesop {

enum SaveFileType {
	SF_TXT,
	SF_BIN
};

enum CardinalDirections {
	DIR_N = 0,
	DIR_E = 1,
	DIR_S = 2,
	DIR_W = 3
};

enum MovementTypeEquate {
	MTYP_INIT = 0,	
	MTYP_TL = 1,	// Turn left
	MTYP_F = 2,		// Forward
	MTYP_TR = 3,	// Turn right
	MTYP_L = 4,		// Slide left
	MTYP_B = 5,		// Retreat
	MTYP_R = 6,		// Slide right
	MTYP_ML = 7,	// Left maze passage
	MTYP_MM = 8,	// Middle maze passage
	MTYP_MR = 9		// Right maze passage
};

const int LVL_Y = 32;	// Max. Y-dimension of level map = 32 (must be 2^^n)
const int LVL_X = 32;	// Max. X-dimension of level map = 32 (must be 2^^n)

const char SAVEDIR_FN[] = "SAVEGAME\\SAVEGAME.DIR";
const char items_bin[] = "SAVEGAME\\ITEMS_yy.BIN";
const char items_txt[] = "SAVEGAME\\ITEMS_yy.TXT";
const char lvl_bin[] = "SAVEGAME\\LVLxx_yy.BIN";
const char lvl_txt[] = "SAVEGAME\\LVLxx_yy.TXT";
const char lvl_tmp[] = "SAVEGAME\\LVLxx.TMP";
const char itm_tmp[] = "SAVEGAME\\ITEMS.TMP";

//char *savegame_dir[NUM_SAVEGAMES][SAVE_LEN+1];

const int8 DX_offset[6][4] = { {0, 0, 0, 0},
						 {0, 1, 0, -1},
						 {0, 0, 0, 0},
						 {-1, 0, 1, 0},
						 {0, -1, 0, 1},
						 {1, 0, -1, 0} };

const int8 DY_offset[6][4] = { {0, 0, 0, 0},
						 {-1, 0, 1, 0},
						 {0, 0, 0, 0},
						 {0, -1, 0, 1},
						 {1, 0, -1, 0},
						 {0, 1, 0, -1} };

Value loadString(int argc, Value *argv);
Value loadResource(int argc, Value *argv);
Value copyString(int argc, Value *argv);
Value stringForceLower(int argc, Value *argv);
Value stringForceUpper(int argc, Value *argv);
Value stringLen(int argc, Value *argv);
Value stringCompare(int argc, Value *argv);
Value beep(int argc, Value *argv);
Value strval(int argc, Value *argv);
Value envval(int argc, Value *argv);
Value pokemem(int argc, Value *argv);
Value peekmem(int argc, Value *argv);
Value rnd(int argc, Value  *argv);
Value dice(int argc, Value *argv);
Value absv(int argc, Value *argv);
Value minv(int argc, Value *argv);
Value maxv(int argc, Value *argv);
Value diagnose(int argc, Value *argv);
Value heapfree(int argc, Value *argv);

Value notify(int argc, Value *argv);
Value cancel(int argc, Value *argv);
Value drainEventQueue(int argc, Value *argv);
Value postEvent(int argc, Value *argv);
Value sendEvent(int argc, Value *argv);
Value peekEvent(int argc, Value *argv);
Value dispatchEvent(int argc, Value *argv);
Value flushEventQueue(int argc, Value *argv);
Value flushInputEvents(int argc, Value *argv);

Value initInterface(int argc, Value *argv);
Value shutdownInterface(int argc, Value *argv);
Value setMousePointer(int argc, Value *argv);
Value setWaitPointer(int argc, Value *argv);
Value standbyCursor(int argc, Value *argv);
Value resumeCursor(int argc, Value *argv);
Value showMouse(int argc, Value *argv);
Value hideMouse(int argc, Value *argv);
Value mouseXY(int argc, Value *argv);
Value mouseInWindow(int argc, Value *argv);
Value lockMouse(int argc, Value *argv);
Value unlockMouse(int argc, Value *argv);
Value getkey(int argc, Value *argv);

Value initGraphics(int argc, Value *argv);
Value drawDot(int argc, Value *argv);
Value drawLine(int argc, Value *argv);
Value lineTo(int argc, Value *argv);
Value drawRectangle(int argc, Value *argv);
Value fillRectangle(int argc, Value *argv);
Value hashRectangle(int argc, Value *argv);
Value getBitmapHeight(int argc, Value *argv);
Value drawBitmap(int argc, Value *argv);
Value visibleBitmapRect(int argc, Value *argv);
Value setPalette(int argc, Value *argv);
Value refreshWindow(int argc, Value *argv);
Value wipeWindow(int argc, Value *argv);
Value shutdownGraphics(int argc, Value *argv);
Value waitVerticalRetrace(int argc, Value *argv);
Value readPalette(int argc, Value *argv);
Value writePalette(int argc, Value *argv);
Value pixelFade(int argc, Value *argv);
Value colorFade(int argc, Value *argv);
Value lightFade(int argc, Value *argv);

Value assignWindow(int argc, Value *argv);
Value assignSubwindow(int argc, Value *argv);
Value releaseWindow(int argc, Value *argv);
Value getX1(int argc, Value *argv);
Value getX2(int argc, Value *argv);
Value getY1(int argc, Value *argv);
Value getY2(int argc, Value *argv);
Value setX1(int argc, Value *argv);
Value setX2(int argc, Value *argv);
Value setY1(int argc, Value *argv);
Value setY2(int argc, Value *argv);

Value textWindow(int argc, Value *argv);
Value textStyle(int argc, Value *argv);
Value textXY(int argc, Value *argv);
Value textColor(int argc, Value *argv);
Value textRefreshWindow(int argc, Value *argv);

Value getTextX(int argc, Value *argv);
Value getTextY(int argc, Value *argv);

Value home(int argc, Value *argv);

Value print(int argc, Value *argv);
Value sprint(int argc, Value *argv);
Value dprint(int argc, Value *argv);
Value aprint(int argc, Value *argv);
Value crout(int argc, Value *argv);
Value charWidth(int argc, Value *argv);
Value fontHeight(int argc, Value *argv);

Value solidBarGraph(int argc, Value *argv);

Value initSound(int argc, Value *argv);
Value shutdownSound(int argc, Value *argv);
Value loadSoundBlock(int argc, Value *argv);
Value soundEffect(int argc, Value *argv);
Value playSequence(int argc, Value *argv);
Value loadMusic(int argc, Value *argv);
Value unloadMusic(int argc, Value *argv);
Value setSoundStatus(int argc, Value *argv);

Value createObject(int argc, Value *argv);
Value createProgram(int argc, Value *argv);
Value destroyObject(int argc, Value *argv);
Value thrashCache(int argc, Value *argv);
Value flushCache(int argc, Value *argv);

Value stepX(int argc, Value *argv);
Value stepY(int argc, Value *argv);
Value stepFDIR(int argc, Value *argv);

Value stepSquareX(int argc, Value *argv);
Value stepSquareY(int argc, Value *argv);
Value stepRegion(int argc, Value *argv);

Value distance(int argc, Value *argv);
Value seekDirection(int argc, Value *argv);

Value spellRequest(int argc, Value *argv);
Value spellList(int argc, Value *argv);
Value magicField(int argc, Value *argv);
Value doDots(int argc, Value *argv);
Value doIce(int argc, Value *argv);

Value readSaveDirectory(int argc, Value *argv);
Value savegameTitle(int argc, Value *argv);
Value writeSaveDirectory(int argc, Value *argv);

Value saveGame(int argc, Value *argv);
Value suspendGame(int argc, Value *argv);
Value resumeItems(int argc, Value *argv);
Value resumeLevel(int argc, Value *argv);
Value changeLevel(int argc, Value *argv);
Value restoreItems(int argc, Value *argv);
Value restoreLevelObjects(int argc, Value *argv);
Value readInitialItems(int argc, Value *argv);
Value writeInitialTempfiles(int argc, Value *argv);
Value createInitialBinaryFiles(int argc, Value *argv);
Value launch(int argc, Value *argv);

Value openTransferFile(int argc, Value *argv);
Value closeTransferFile(int argc, Value *argv);
Value playerAttrib(int argc, Value *argv);
Value itemAttrib(int argc, Value *argv);
Value arrowCount(int argc, Value *argv);

} // End of namespace Aesop

#endif
