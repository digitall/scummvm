#ifndef AESOP_H
#define AESOP_H

#include "common/random.h"
#include "engines/engine.h"
#include "gui/debugger.h"

#include "script.h"

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

const int VM_STACK_SIZE = 4096 * sizeof(void *); // 16K for 32-bit binary, 32K for 64-bit binary
const int BOOTSTRAP	= 2000;		// Object which launches AESOP application
const int NUM_OBJECTS = 2048;	// Up to 2,048 objects in Eye III universe
const int NUM_ENTITIES = 2000;	// Up to 2,000 physical entities in Eye III universe

class Console;
class ResourceManager;

class AesopEngine : public Engine {
public:
	AesopEngine(OSystem *syst);
	~AesopEngine();
	
	virtual Common::Error run();
	uint32 execute(int index, uint32 messageNumber, uint32 vector);
	void setStackPointer(byte *sp);

	// Eye of the Beholder III code resource functions
	static Value loadString(int argc, Value *argv);
	static Value loadResource(int argc, Value *argv);
	static Value copyString(int argc, Value *argv);
	static Value stringForceLower(int argc, Value *argv);
	static Value stringForceUpper(int argc, Value *argv);
	static Value stringLen(int argc, Value *argv);
	static Value stringCompare(int argc, Value *argv);
	static Value beep(int argc, Value *argv);
	static Value strval(int argc, Value *argv);
	static Value envval(int argc, Value *argv);
	static Value pokemem(int argc, Value *argv);
	static Value peekmem(int argc, Value *argv);
	static Value rnd(int argc, Value  *argv);
	static Value dice(int argc, Value *argv);
	static Value absv(int argc, Value *argv);
	static Value minv(int argc, Value *argv);
	static Value maxv(int argc, Value *argv);
	static Value diagnose(int argc, Value *argv);
	static Value heapfree(int argc, Value *argv);
	static Value notify(int argc, Value *argv);
	static Value cancel(int argc, Value *argv);
	static Value drainEventQueue(int argc, Value *argv);
	static Value postEvent(int argc, Value *argv);
	static Value sendEvent(int argc, Value *argv);
	static Value peekEvent(int argc, Value *argv);
	static Value dispatchEvent(int argc, Value *argv);
	static Value flushEventQueue(int argc, Value *argv);
	static Value flushInputEvents(int argc, Value *argv);
	static Value initInterface(int argc, Value *argv);
	static Value shutdownInterface(int argc, Value *argv);
	static Value setMousePointer(int argc, Value *argv);
	static Value setWaitPointer(int argc, Value *argv);
	static Value standbyCursor(int argc, Value *argv);
	static Value resumeCursor(int argc, Value *argv);
	static Value showMouse(int argc, Value *argv);
	static Value hideMouse(int argc, Value *argv);
	static Value mouseXY(int argc, Value *argv);
	static Value mouseInWindow(int argc, Value *argv);
	static Value lockMouse(int argc, Value *argv);
	static Value unlockMouse(int argc, Value *argv);
	static Value getkey(int argc, Value *argv);
	static Value initGraphics(int argc, Value *argv);
	static Value drawDot(int argc, Value *argv);
	static Value drawLine(int argc, Value *argv);
	static Value lineTo(int argc, Value *argv);
	static Value drawRectangle(int argc, Value *argv);
	static Value fillRectangle(int argc, Value *argv);
	static Value hashRectangle(int argc, Value *argv);
	static Value getBitmapHeight(int argc, Value *argv);
	static Value drawBitmap(int argc, Value *argv);
	static Value visibleBitmapRect(int argc, Value *argv);
	static Value setPalette(int argc, Value *argv);
	static Value refreshWindow(int argc, Value *argv);
	static Value wipeWindow(int argc, Value *argv);
	static Value shutdownGraphics(int argc, Value *argv);
	static Value waitVerticalRetrace(int argc, Value *argv);
	static Value readPalette(int argc, Value *argv);
	static Value writePalette(int argc, Value *argv);
	static Value pixelFade(int argc, Value *argv);
	static Value colorFade(int argc, Value *argv);
	static Value lightFade(int argc, Value *argv);
	static Value assignWindow(int argc, Value *argv);
	static Value assignSubwindow(int argc, Value *argv);
	static Value releaseWindow(int argc, Value *argv);
	static Value getX1(int argc, Value *argv);
	static Value getX2(int argc, Value *argv);
	static Value getY1(int argc, Value *argv);
	static Value getY2(int argc, Value *argv);
	static Value setX1(int argc, Value *argv);
	static Value setX2(int argc, Value *argv);
	static Value setY1(int argc, Value *argv);
	static Value setY2(int argc, Value *argv);
	static Value textWindow(int argc, Value *argv);
	static Value textStyle(int argc, Value *argv);
	static Value textXY(int argc, Value *argv);
	static Value textColor(int argc, Value *argv);
	static Value textRefreshWindow(int argc, Value *argv);
	static Value getTextX(int argc, Value *argv);
	static Value getTextY(int argc, Value *argv);
	static Value home(int argc, Value *argv);
	static Value print(int argc, Value *argv);
	static Value sprint(int argc, Value *argv);
	static Value dprint(int argc, Value *argv);
	static Value aprint(int argc, Value *argv);
	static Value crout(int argc, Value *argv);
	static Value charWidth(int argc, Value *argv);
	static Value fontHeight(int argc, Value *argv);
	static Value solidBarGraph(int argc, Value *argv);
	static Value initSound(int argc, Value *argv);
	static Value shutdownSound(int argc, Value *argv);
	static Value loadSoundBlock(int argc, Value *argv);
	static Value soundEffect(int argc, Value *argv);
	static Value playSequence(int argc, Value *argv);
	static Value loadMusic(int argc, Value *argv);
	static Value unloadMusic(int argc, Value *argv);
	static Value setSoundStatus(int argc, Value *argv);
	static Value createObject(int argc, Value *argv);
	static Value createProgram(int argc, Value *argv);
	static Value destroyObject(int argc, Value *argv);
	static Value thrashCache(int argc, Value *argv);
	static Value flushCache(int argc, Value *argv);
	static Value stepX(int argc, Value *argv);
	static Value stepY(int argc, Value *argv);
	static Value stepFDIR(int argc, Value *argv);
	static Value stepSquareX(int argc, Value *argv);
	static Value stepSquareY(int argc, Value *argv);
	static Value stepRegion(int argc, Value *argv);
	static Value distance(int argc, Value *argv);
	static Value seekDirection(int argc, Value *argv);
	static Value spellRequest(int argc, Value *argv);
	static Value spellList(int argc, Value *argv);
	static Value magicField(int argc, Value *argv);
	static Value doDots(int argc, Value *argv);
	static Value doIce(int argc, Value *argv);
	static Value readSaveDirectory(int argc, Value *argv);
	static Value savegameTitle(int argc, Value *argv);
	static Value writeSaveDirectory(int argc, Value *argv);
	static Value saveGame(int argc, Value *argv);
	static Value suspendGame(int argc, Value *argv);
	static Value resumeItems(int argc, Value *argv);
	static Value resumeLevel(int argc, Value *argv);
	static Value changeLevel(int argc, Value *argv);
	static Value restoreItems(int argc, Value *argv);
	static Value restoreLevelObjects(int argc, Value *argv);
	static Value readInitialItems(int argc, Value *argv);
	static Value writeInitialTempfiles(int argc, Value *argv);
	static Value createInitialBinaryFiles(int argc, Value *argv);
	static Value launch(int argc, Value *argv);
	static Value openTransferFile(int argc, Value *argv);
	static Value closeTransferFile(int argc, Value *argv);
	static Value playerAttrib(int argc, Value *argv);
	static Value itemAttrib(int argc, Value *argv);
	static Value arrowCount(int argc, Value *argv);

	// Dungeon Hack code resource functions
	static Value buildClipping(int argc, Value *argv);
	static Value catString(int argc, Value *argv);
	static Value closeFeatureFile(int argc, Value *argv);
	static Value closeFile(int argc, Value *argv);
	static Value copyWindow(int argc, Value *argv);
	static Value createFile(int argc, Value *argv);
	static Value deleteSaves(int argc, Value *argv);
	static Value drawAutoSquare(int argc, Value *argv);
	static Value drawWalls(int argc, Value *argv);
	static Value explodeSave(int argc, Value *argv);
	static Value findLocationForMap(int argc, Value *argv);
	static Value getFeatureRecord(int argc, Value *argv);
	static Value initViewspace(int argc, Value *argv);
	static Value loadLevelMap(int argc, Value *argv);
	static Value loadVisibility(int argc, Value *argv);
	static Value lockResource(int argc, Value *argv);
	static Value long2hex(int argc, Value *argv);
	static Value openFeatureFile(int argc, Value *argv);
	static Value openFile(int argc, Value *argv);
	static Value outputTime(int argc, Value *argv);
	static Value pageFlip(int argc, Value *argv);
	static Value pause(int argc, Value *argv);
	static Value prepareSave(int argc, Value *argv);
	static Value printerOnLine(int argc, Value *argv);
	static Value randomizeArray(int argc, Value *argv);
	static Value readArrayFromFile(int argc, Value *argv);
	static Value readNumberFromFile(int argc, Value *argv);
	static Value refreshMainTextWindow(int argc, Value *argv);
	static Value rollChance(int argc, Value *argv);
	static Value saveVisibility(int argc, Value *argv);
	static Value seedRandom(int argc, Value *argv);
	static Value seekInFile(int argc, Value *argv);
	static Value sequencePlaying(int argc, Value *argv);
	static Value textBackground(int argc, Value *argv);
	static Value touch(int argc, Value *argv);
	static Value transition(int argc, Value *argv);
	static Value unlockResource(int argc, Value *argv);
	static Value updateFile(int argc, Value *argv);
	static Value walkheap(int argc, Value *argv);
	static Value windowCore(int argc, Value *argv);
	static Value writeArrayToFile(int argc, Value *argv);
	static Value writeLongToFile(int argc, Value *argv);
	static Value writeMapheaderToFile(int argc, Value *argv);
	static Value writeResourceToFile(int argc, Value *argv);
	static Value xmsallocated(int argc, Value *argv);
	static Value xmsfree(int argc, Value *argv);
private:
	void initObjectList();
	uint32 createProgram(int index, const char *resourceName);
	uint32 createProgram(int index, uint32 resource);
	int findFreeEntry(int min, int end);
	void createSOPInstance(uint32 objectId, int index);
	Object* createInstance(uint32 objectId);
	Thunk* constructThunk(uint32 objectId);

	static AesopEngine *s_engine;

	Console *_console;
	Common::RandomSource *_rnd;
	ResourceManager *_resMan;
	byte objectFlags[NUM_OBJECTS];
	Object* objectList[NUM_OBJECTS];
	byte *_stackBase;
	byte *_stackPointer;
};

class Console : public GUI::Debugger {
public:
	Console(AesopEngine *vm) {}
	virtual ~Console() {}
};

} // End of namespace Aesop

#endif
