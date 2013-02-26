#include "common/scummsys.h"

#include "common/config-manager.h"
#include "common/debug.h"
#include "common/debug-channels.h"
#include "common/error.h"
#include "common/EventRecorder.h"
#include "common/file.h"
#include "common/fs.h"
#include "common/memstream.h"

#include "engines/util.h"

#include "graphics/palette.h"

#include "aesop/aesop.h"
#include "aesop/resource.h"

namespace Aesop {

AesopEngine* AesopEngine::s_engine = NULL;
Resource* AesopEngine::s_soundEffects[64];

char* AesopEngine::SAVEDIR_FN = "SAVEGAME\\SAVEGAME.DIR";
char* AesopEngine::items_bin = "SAVEGAME\\ITEMS_yy.BIN";
char* AesopEngine::items_txt = "SAVEGAME\\ITEMS_yy.TXT";
char* AesopEngine::lvl_bin = "SAVEGAME\\LVLxx_yy.BIN";
char* AesopEngine::lvl_txt = "SAVEGAME\\LVLxx_yy.TXT";
char* AesopEngine::lvl_tmp = "SAVEGAME\\LVLxx.TMP";
char* AesopEngine::itm_tmp = "SAVEGAME\\ITEMS.TMP";

const int FBEG = 0;
const int FCNT = 192;
const int BLU_BEG = 0x55;
const int BLU_NUM = 11;
const int GRN_BEG = 0x70;
const int GRN_NUM = 11;
const int GRY_BEG = 0x67;
const int GRY_NUM = 9;
const int RED_BEG = 0x22;
const int RED_NUM = 9;
const int BRN_BEG = 0x96;
const int BRN_NUM = 7;
const int FIX_WHT = 0x0B;
const int PAL_FIXED = 0;
const int PAL_WALLS = 1;
const int PAL_M1 = 2;
const int PAL_M2 = 3;
const int PAL_OUT = 4;
const int F_BLU = 11;
const int F_GRN = 12;
const int F_RED = 13;
const int F_GRY = 14;
const int M_GRY = 11;
const int M_WHT = 12;
const int M_GRN = 13;
const int M_BLU = 14;
const int M_BRN = 15;
const int XCOLOR = 0;
const int DK_GRN = 1;
const int LT_GRN = 2;
const int GRN = 2;
const int YEL = 3;
const int LT_RED = 4;
const int DK_RED = 6;
const int RED = 4;
const int BLU = 8;
const int BLK = 10;
const int WHT = 11;
const int VIO = 0x80;
const int CYN = 0x57;
const int LT_CYN = 7;
const int LT_BLU = 8;
const int BRN_1 = 0x12;
const int BRN_2 = 0x14;
const int BRN_3 = 0x16;
const int MAX_BITMAP_WIDTH = 2500;
const int MAX_BITMAP_HEIGHT = 2500;

byte F_fade[11][256];
byte W_fade[11][16];
byte M1_fade[11][32];
byte M2_fade[11][32];

byte M1_gry[32];
byte M1_wht[32];
byte M2_gry[32];
byte M2_wht[32];
byte M1_blu[32];
byte M2_blu[32];
byte M1_grn[32];
byte M2_grn[32];
byte M1_brn[32];
byte M2_brn[32];
byte F_grn[256];
byte F_blu[256];
byte F_red[256];
byte F_gry[256];

uint16 first_color[5] = { 0x00, 0xb0, 0xc0, 0xe0, 0xb0 };
uint16 num_colors[5] = { 256, 16, 32, 32, 80 };
byte *fade_tables[5][16] = { { F_fade[0], F_fade[1], F_fade[2], F_fade[3],
							   F_fade[4], F_fade[5], F_fade[6], F_fade[7],
							   F_fade[8], F_fade[9], F_fade[10],
							   F_blu, F_grn, F_red, F_gry, NULL },
							 { W_fade[0], W_fade[1], W_fade[2], W_fade[3],
							   W_fade[4], W_fade[5], W_fade[6], W_fade[7],
							   W_fade[8], W_fade[9], W_fade[10],
							   NULL, NULL, NULL, NULL, NULL },
							 { M1_fade[0], M1_fade[1], M1_fade[2], M1_fade[3],
							   M1_fade[4], M1_fade[5], M1_fade[6], M1_fade[7],
							   M1_fade[8], M1_fade[9], M1_fade[10],
							   M1_gry, M1_wht, M1_grn, M1_blu, M1_brn },
							 { M2_fade[0], M2_fade[1], M2_fade[2], M2_fade[3],
							   M2_fade[4], M2_fade[5], M2_fade[6], M2_fade[7],
							   M2_fade[8], M2_fade[9], M2_fade[10],
							   M2_gry, M2_wht, M2_grn, M2_blu, M2_brn },
							 { NULL, NULL, NULL, NULL,
							   NULL, NULL, NULL, NULL,
							   NULL, NULL, NULL,
							   NULL, NULL, NULL, NULL, NULL } };

byte blu_inten[BLU_NUM];
byte grn_inten[GRN_NUM];
byte gry_inten[GRY_NUM];
byte red_inten[RED_NUM];
byte brn_inten[BRN_NUM];

byte text_colors[9] = { DK_GRN, LT_GRN, YEL, LT_RED, DK_RED, BLK, WHT, WHT, WHT };

AesopEngine::AesopEngine(OSystem *syst) : Engine(syst) {
	const Common::FSNode gameDataDir(ConfMan.get("path"));
	
	DebugMan.addDebugChannel(kAesopDebugCodeResource, "codeResource", "debug code resource calls");
	DebugMan.addDebugChannel(kAesopDebugOpcode, "opcode", "debug opcode execution");

	_rnd = new Common::RandomSource("aesop");

	s_engine = this;
	_stackBase = new byte[VM_STACK_SIZE];
	_stackPointer = _stackBase + VM_STACK_SIZE;

	debug("AesopEngine::AesopEngine");
}

AesopEngine::~AesopEngine() {
	debug("AesopEngine::~AesopEngine");
	
	// Remove all of our debug levels here
	DebugMan.clearAllDebugChannels();

	delete _rnd;
	delete [] _stackBase;
}

Common::Error AesopEngine::run() {
	uint32 ret;
	
	_console = new Console(this);
	_resMan = new ResourceManager();
	_resMan->init("eye.res");
	ret = createProgram(BOOTSTRAP, "start");
	
	return Common::kNoError;
}

void AesopEngine::initalizeGraphics() {
	initGraphics(320, 200, false);
}

void AesopEngine::initObjectList() {
	for(int i = 0; i < NUM_OBJECTS; i++) {
		objectList[i] = NULL;
	}
}

uint32 AesopEngine::createProgram(int index, const char *resourceName) {
	return createProgram(index, _resMan->getResourceId(resourceName));
}

uint32 AesopEngine::createProgram(int index, uint32 programId) {
	if (index == -1) {
		index = findFreeEntry(NUM_ENTITIES, NUM_OBJECTS);
	}

	if (index != -1) {
		createSOPInstance(programId, index);
	}

	return index;
}

int AesopEngine::findFreeEntry(int min, int end) {
   int i;

   for (i = min; i < end; i++) {
      if (objectList[i] == NULL) {
         break;
	  }
   }

   if (i == end) {
      return -1;
   }
   else {
      return i;
   }
}

void AesopEngine::createSOPInstance(uint32 objectId, int index) {
   objectList[index] = createInstance(objectId, index);
   objectList[index]->execute(MSG_CREATE, static_cast<uint32>(-1), _stackPointer);
}

Object* AesopEngine::createInstance(uint32 objectId, int index) {
	Resource *res = _resMan->getResource(objectId);
	
	if(res->thunk == NULL) {
		res->thunk = constructThunk(objectId);
	}

	return new Object(objectId, index, res->thunk);
}

Thunk* AesopEngine::constructThunk(uint32 objectId) {
	uint32 classId;
	Resource* code[MAX_G];
	Resource* importCode[MAX_G];
	Resource* exportCode[MAX_G];
	uint32 exports[MAX_G];
	int depth;
	byte* prg;
	ProgramHeader* prgHeader;
	Common::MemoryReadStream *memStream;
	uint16 stringLength;
	char *tag = NULL;
	char *def = NULL;
	uint32 offset;

	Thunk *thunk = new Thunk;
	thunk->engine = this;
	thunk->useCount = 0;

	depth = 0;
	classId = objectId;

	while(classId != -1) {
		code[depth] = _resMan->getResource(classId);
		assert(code[depth]);

		// FIXME: I don't think this will work for calling into other objects
		// because the branches will be calculated wrong!!
		if(depth == 0) {
			thunk->codeBase = code[depth]->data;
		}

		prg = code[depth]->data;
		prgHeader = reinterpret_cast<ProgramHeader *>(prg);
		exports[depth] = prgHeader->exports;
		importCode[depth] = _resMan->getResource(prgHeader->imports);
		exportCode[depth] = _resMan->getResource(prgHeader->exports);
		classId = prgHeader->parent;
		depth++;
		assert(depth <= MAX_G);

		int i = depth-1;

		while (i >= 0)
		{
			memStream = new Common::MemoryReadStream(importCode[i]->data, importCode[i]->size);
			// skip 6 bytes...must be some sort of dictionary header
			memStream->seek(6, SEEK_CUR);
			memStream->read(&stringLength, sizeof(stringLength));
			while(stringLength > 0) {
				if(tag == NULL) {
					tag = new char[stringLength];
					memStream->read(tag, stringLength);
				}
				else {
					def = new char[stringLength];
					memStream->read(def, stringLength);

					switch(tag[0]) {
					case 'C':	// Code
						thunk->externalCodeResources.setVal(atoi(def), _resMan->getCodeResource(&tag[2]));
						break;
					case 'B':	// Byte
					case 'W':	// Word
					case 'L':	// Long
						ExternalReference *xref = new ExternalReference;
						xref->variableType = tag[0];
						xref->variableName = &tag[2];
						xref->classReference = atoi(strchr(def, ',') + 1);
					}

					tag = NULL;
					def = NULL;
				}
				memStream->read(&stringLength, sizeof(stringLength));
			}
			delete memStream;

			i--;
		}

		// TODO: sort MV list by ascending message number
		// TODO: sort MV list by ascending class
		// TODO: how are we going to implement OP_PASS?
		for(int k = 0; k < depth; k++)
		{
			memStream = new Common::MemoryReadStream(exportCode[k]->data, exportCode[k]->size);
			// skip 6 bytes...must be some sort of dictionary header
			memStream->seek(6, SEEK_CUR);
			memStream->read(&stringLength, sizeof(stringLength));
			while(stringLength > 0) {
				if(tag == NULL) {
					tag = new char[stringLength];
					memStream->read(tag, stringLength);
				}
				else {
					def = new char[stringLength];
					memStream->read(def, stringLength);

					switch(tag[0]) {
					case 'M':
						thunk->messageHandlers.setVal(atoi(&tag[2]), prg + atoi(def));
						break;
					}

					tag = NULL;
					def = NULL;
				}
				memStream->read(&stringLength, sizeof(stringLength));
			}
			delete memStream;
		}
	}

	return thunk;
}

uint32 AesopEngine::execute(int index, uint32 messageNumber, uint32 vector) {
	return objectList[index]->execute(messageNumber, vector, _stackPointer);
}

void AesopEngine::setStackPointer(byte *sp) {
	_stackPointer = sp;
}

void AesopEngine::setSaveSlotnum(int slot) {
	char num[3];

	sprintf(num, "%02u", slot);

	strncpy(&items_bin[15], num, 2);
	strncpy(&items_txt[15], num, 2);
	strncpy(&lvl_bin[15], num, 2);
	strncpy(&lvl_txt[15], num, 2);
}

void AesopEngine::translateFile(const char *txtFilename, const char *binFilename, int first, int last) {
	
}

// Eye of the Beholder III code resource functions

Value AesopEngine::loadString(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::loadResource(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::copyString(int argc, Value *argv) {
	char *src = reinterpret_cast<char *>(argv[0].address);
	char *dst = reinterpret_cast<char *>(argv[1].address);
	strcpy(dst, src);
	return Value(-1);
}

Value AesopEngine::stringForceLower(int argc, Value *argv) {
	char *dst = reinterpret_cast<char *>(argv[0].address);
	strlwr(dst);
	return Value(-1);
}

Value AesopEngine::stringForceUpper(int argc, Value *argv) {
	char *dst = reinterpret_cast<char *>(argv[0].address);
	strupr(dst);
	return Value(-1);
}

Value AesopEngine::stringLen(int argc, Value *argv) {
	Value ret;
	char *string = reinterpret_cast<char *>(argv[0].address);
	ret.fullValue = strlen(string);
	return ret;
}

Value AesopEngine::stringCompare(int argc, Value *argv) {
	Value ret;
	char *str1 = reinterpret_cast<char *>(argv[0].address);
	char *str2 = reinterpret_cast<char *>(argv[1].address);
	ret.fullValue = scumm_stricmp(str1, str2);
	return ret;
}

Value AesopEngine::beep(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::strval(int argc, Value *argv) {
	Value ret;
	char *string = reinterpret_cast<char *>(argv[0].address);
	if(string != NULL) {
		ret.fullValue = atoi(string);
	}
	else {
		ret.fullValue = -1;
	}
	return ret;
}

Value AesopEngine::envval(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::pokemem(int argc, Value *argv) {
	Value ret;
	ret.fullValue = -1;
	return ret;
}

Value AesopEngine::peekmem(int argc, Value *argv) {
	Value ret;
	ret.fullValue = -1;
	return ret;
}

Value AesopEngine::rnd(int argc, Value  *argv) {
	Value ret;
	uint32 low = argv[0].fullValue;
	uint32 high = argv[1].fullValue;
	ret.fullValue = s_engine->_rnd->getRandomNumberRng(low, high);
	return ret;
}

Value AesopEngine::dice(int argc, Value *argv) {
	uint32 total;
	Value ret;

	uint32 ndice = argv[0].fullValue;
	uint32 nsides = argv[1].fullValue;
	uint32 bonus = argv[2].fullValue;

	total = bonus;
	for (uint32 i = 0; i < ndice; i++) {
      total += s_engine->_rnd->getRandomNumberRng(1, nsides); 
	}
	ret.fullValue = total;

	return ret;
}

Value AesopEngine::absv(int argc, Value *argv) {
	Value ret;
	int32 val = argv[0].fullValue;
	ret.fullValue = (val < 0) ? -val : val;
	return ret;
}

Value AesopEngine::minv(int argc, Value *argv) {
	Value ret;
	int32 val1 = argv[0].fullValue;
	int32 val2 = argv[1].fullValue;
	ret.fullValue = MIN(val1, val2);
	return ret;
}

Value AesopEngine::maxv(int argc, Value *argv) {
	Value ret;
	int32 val1 = argv[0].fullValue;
	int32 val2 = argv[1].fullValue;
	ret.fullValue = MAX(val1, val2);
	return ret;
}

Value AesopEngine::diagnose(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::heapfree(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::notify(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::cancel(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::drainEventQueue(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::postEvent(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::sendEvent(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::peekEvent(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::dispatchEvent(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::flushEventQueue(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::flushInputEvents(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::initInterface(int argc, Value *argv) {
	// Yet another no-op. Looks to initalize mouse
	// and callback functions in original engine.
	return Value(-1);
}

Value AesopEngine::shutdownInterface(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::setMousePointer(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::setWaitPointer(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::standbyCursor(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::resumeCursor(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::showMouse(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::hideMouse(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::mouseXY(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::mouseInWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::lockMouse(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::unlockMouse(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::getkey(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::myInitGraphics(int argc, Value *argv) {
	s_engine->initalizeGraphics();
	return Value(-1);
}

Value AesopEngine::drawDot(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::drawLine(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::lineTo(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::drawRectangle(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::fillRectangle(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::hashRectangle(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::getBitmapHeight(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::drawBitmap(int argc, Value *argv) {
	uint32 page = argv[0].fullValue;
	uint32 table = argv[1].fullValue;
	uint32 number = argv[2].fullValue;
	int32 x = argv[3].fullValue;
	int32 y = argv[4].fullValue;
	uint32 scale = argv[5].fullValue;
	uint32 flip = argv[6].fullValue;
	uint32 fade_table = argv[7].fullValue;
	uint32 fade_level = argv[8].fullValue;

	static Resource *lastResource = NULL;
	static uint32 lastTable;
	Resource *res;
	byte *lookaside;

	if(table == lastTable) {
		res = lastResource;
	}
	else {
		res = s_engine->_resMan->getResource(table);
		lastResource = res;
		lastTable = table;
	}

	if((fade_level > 10) && (!scale)) {
		scale = 256;
	}

	lookaside = fade_tables[fade_table][fade_level] - first_color[fade_table];

	// Convert SHP format data to bitmap
	uint32 size = res->data[0] | (res->data[1] << 8) | (res->data[2] << 16) | (res->data[3] << 24);
	assert(size == res->size);

	int subPictures = res->data[4] | (res->data[5] << 8);
	int offset = res->data[6 + number * 4] | (res->data[6 + number * 4 + 1] << 8) | (res->data[6 + number * 4 + 2 ] << 16) | (res->data[6 + number * 4 + 3] << 24);
	int width = res->data[offset] | (res->data[offset + 1] << 8);
	assert(width > 0 && width <= MAX_BITMAP_WIDTH);
	
	int height = res->data[offset + 2] | (res->data[offset + 3] << 8);
	assert(height > 0 && height <= MAX_BITMAP_HEIGHT);

	offset += 4;
	byte *bitmap = new byte[width * height];
	while(true) {
		int y = res->data[offset];
		if(y == 0xff) {
			break;
		}
		assert(y >= 0 && y < height);
		offset++;

		while(true) {
			int x;
			int isLast;
			int RLEwidth;
			int RLEbytes;

			x = res->data[offset];
			offset++;

			isLast = res->data[offset];
			offset++;

			RLEwidth = res->data[offset];
			offset++;

			RLEbytes = res->data[offset];
			offset++;

			while(RLEwidth > 0) {
				int mode = res->data[offset] & 1;
				int amount = (res->data[offset] >> 1) + 1;
				offset++;

				if(mode == 0) {			// Copy
					memcpy(bitmap + x + y * width, res->data + offset, amount);
					offset += amount;
				}
				else if(mode == 1) {	// Fill
					int value = res->data[offset];
					offset++;
					memset(bitmap + x + y * width, value, amount);
				}
				x += amount;
				RLEwidth -= amount;
			}

			assert(RLEwidth == 0);

			if(isLast == 0x80) {
				break;
			}
		}
	}

	// FIXME
	//GIL2VFXDrawBitmap(page, x, y, flip, scale, lookaside, res, number);
	//s_engine->_system->copyRectToScreen(bitmap, 0, 0, 0, 320, 200);
	//s_engine->_system->updateScreen();

	return Value(-1);
}

Value AesopEngine::visibleBitmapRect(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::setPalette(int argc, Value *argv) {
	uint32 region = argv[0].fullValue;
	uint32 resource = argv[1].fullValue;

	Resource *res = s_engine->_resMan->getResource(resource);
	PaletteHeader *phdr = reinterpret_cast<PaletteHeader *>(res->data);
	int f, n, m, d, dm, j;

	if(region == PAL_FIXED
		|| region == PAL_WALLS
		|| region == PAL_M1
		|| region == PAL_M2) {
		for(int i = 0; i < 11; i++) {
			byte *fade = res->data + phdr->fade[i];
			for(int j = 0; j < phdr->ncolors; j++) {
				fade_tables[region][i][j] = first_color[region] + fade[j];
			}
		}
	}

	
	// FIXME: is this call correct??
	s_engine->_system->getPaletteManager()->setPalette(res->data + phdr->RGB, 0, phdr->ncolors);
	RGB *array = reinterpret_cast<RGB *>(res->data + phdr->RGB);

	switch(region) {
	case PAL_FIXED:
		for(int n = 0, i = BLU_BEG; n < BLU_NUM; n++, i++) {
			blu_inten[n] = array[i].r + array[i].g + array[i].b;
		}
		for(int n = 0, i = RED_BEG; n < RED_NUM; n++, i ++) {
			red_inten[n] = array[i].r + array[i].g + array[i].b;
		}
		for(int n = 0, i = GRN_BEG; n < GRN_NUM; n++, i++) {
			grn_inten[n] = array[i].r + array[i].g + array[i].b;
		}
		for(int n = 0, i = GRY_BEG; n < GRY_NUM; n++, i++) {
			gry_inten[n] = array[i].r + array[i].g + array[i].b;
		}
		for(int n = 0, i = BRN_BEG; n < BRN_NUM; n++, i++) {
			brn_inten[n] = array[i].r + array[i].g + array[i].b;
		}

		f = first_color[region];
		n = num_colors[region];
	
		for(int i = 0; i < n; i++) {
			j = array[i].r + array[i].g + array[i].b;
			m = 0;
			dm = 32767;
			for(int k = 0; k < BLU_NUM; k++) {
				d = abs(j - blu_inten[k]);
				if(d < dm) {
					dm = d;
					m = k;
				}
			}

			F_blu[i] = BLU_BEG + m;

			m = 0;
			dm = 32767;
			for(int k = 0; k < GRN_NUM; k++) {
				d = abs(j - grn_inten[k]);
				if(d < dm) {
					dm = d;
					m = k;
				}
			}

			F_grn[i] = GRN_BEG + m;

			m = 0;
			dm = 32767;
			for(int k = 0; k < RED_NUM; k++) {
				d = abs(j - red_inten[k]);
				if(d < dm) {
					dm = d;
					m = k;
				}
			}

			F_red[i] = RED_BEG + m;

			m = 0;
			dm = 32767;
			for(int k = 0; k < GRY_NUM; k++) {
				d = abs(j - gry_inten[k]);
				if(d < dm) {
					dm = d;
					m = k;
				}
			}

			F_gry[i] = GRY_BEG + m;
		}
		break;
	case PAL_M1:
	case PAL_M2:
		f = first_color[region];
		n = num_colors[region];
		for(int i = 0; i < n; i++) {
			j = array[i].r + array[i].g + array[i].b;

			m = 0;
			dm = 32767;
			for(int k = 0; k < BRN_NUM; k++) {
				d = abs(j - brn_inten[k]);
				if(d < dm) {
					dm = d;
					m = k;
				}
			}

			if(region == PAL_M1) {
				M1_brn[i] = BRN_BEG + m;
			}
			else {
				M2_brn[i] = BRN_BEG + m;
			}

			m = 0;
			dm = 32767;
			for(int k = 0; k < GRY_NUM; k++) {
				d = abs(j - gry_inten[k]);
				if(d < dm) {
					dm = d;
					m = k;
				}
			}

			if(region == PAL_M1) {
				M1_gry[i] = GRY_BEG + m;
			}
			else {
				M2_gry[i] = GRY_BEG + m;
			}

			m = 0;
			dm = 32767;
			for(int k = 0; k < GRN_NUM; k++) {
				d = abs(j - grn_inten[k]);
				if(d < dm) {
					dm = d;
					m = k;
				}
			}

			if(region == PAL_M1) {
				M1_grn[i] = GRN_BEG + m;
			}
			else {
				M2_grn[i] = GRN_BEG + m;
			}

			m = 0;
			dm = 32767;
			for(int k = 0; k < BLU_NUM; k++) {
				d = abs(j - blu_inten[k]);
				if(d < dm) {
					dm = d;
					m = k;
				}
			}

			if(region == PAL_M1) {
				M1_blu[i] = BLU_BEG + m;
			}
			else {
				M2_blu[i] = BLU_BEG + m;
			}

			if(region == PAL_M1) {
				M1_wht[i] = FIX_WHT;
			}
			else {
				M2_wht[i] = FIX_WHT;
			}
		}
		break;
	}

	return Value(-1);
}

Value AesopEngine::refreshWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::wipeWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::shutdownGraphics(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::waitVerticalRetrace(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::readPalette(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::writePalette(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::pixelFade(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::colorFade(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::lightFade(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::assignWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::assignSubwindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::releaseWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::getX1(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::getX2(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::getY1(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::getY2(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::setX1(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::setX2(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::setY1(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::setY2(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::textWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::textStyle(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::textXY(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::textColor(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::textRefreshWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::getTextX(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::getTextY(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::home(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::print(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::sprint(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::dprint(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::aprint(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::crout(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::charWidth(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::fontHeight(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::solidBarGraph(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::initSound(int argc, Value *argv) {
	// I think another no-op, because ScummVM's audio
	// mixer should already be present.
	return Value(-1);
}

Value AesopEngine::shutdownSound(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::loadSoundBlock(int argc, Value *argv) {
	uint32 firstBlock = argv[0].fullValue;
	uint32 lastBlock = argv[1].fullValue;
	uint32* array = reinterpret_cast<uint32 *>(argv[2].address);

	int index = (firstBlock == BLK_COMMON) ? FIRST_COMMON : FIRST_LEVEL;
	while(*array != 0) {
		if(index > 63) {
			// there are only 64 spaces in the array
			__debugbreak();
		}
		s_soundEffects[index] = s_engine->_resMan->getResource(*array);
		array++;
		index++;
	}
	
	return Value(-1);
}

Value AesopEngine::soundEffect(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::playSequence(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::loadMusic(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::unloadMusic(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::setSoundStatus(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::createObject(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::createProgram(int argc, Value *argv) {
	return s_engine->createProgram(argv[0].fullValue, argv[1].fullValue);
}

Value AesopEngine::destroyObject(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::thrashCache(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::flushCache(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::stepX(int argc, Value *argv) {
	Value ret;
	uint32 x = argv[0].fullValue;
	uint32 fdir = argv[1].fullValue;
	uint32 mtype = argv[2].fullValue;
	uint32 distance = argv[3].fullValue;

	byte xx = static_cast<byte>(x);
	
	if(!distance) {
		ret.fullValue = x;
		return ret;
	}

	if(mtype == MTYP_ML) {
		xx += DX_offset[MTYP_F-1][fdir];
		xx += DX_offset[MTYP_L-1][fdir];
	}
	else if(mtype == MTYP_MR) {
		xx += DX_offset[MTYP_F-1][fdir];
		xx += DX_offset[MTYP_R-1][fdir];
	}
	else if(mtype == MTYP_MM) {
		xx += 2 * DX_offset[MTYP_F-1][fdir];
	}
	else if(mtype != MTYP_INIT) {
		xx += static_cast<byte>(distance * DX_offset[mtype-1][fdir]);
	}

	ret.fullValue = xx & (LVL_X - 1);
	return ret;
}

Value AesopEngine::stepY(int argc, Value *argv) {
	Value ret;
	uint32 y = argv[0].fullValue;
	uint32 fdir = argv[1].fullValue;
	uint32 mtype = argv[2].fullValue;
	uint32 distance = argv[3].fullValue;

	byte yy = static_cast<byte>(y);

	if(!distance) {
		ret.fullValue = y;
		return ret;
	}

	if(mtype == MTYP_ML) {
		yy += DY_offset[MTYP_F-1][fdir];
		yy += DY_offset[MTYP_L-1][fdir];
	}
	else if(mtype == MTYP_MR) {
		yy += DY_offset[MTYP_F-1][fdir];
		yy += DY_offset[MTYP_R-1][fdir];
	}
	else if(mtype == MTYP_MM) {
		yy += 2 * DY_offset[MTYP_F-1][fdir];
	}
	else if(mtype != MTYP_INIT) {
		yy += static_cast<byte>(distance * DY_offset[mtype-1][fdir]);
	}

	ret.fullValue = yy & (LVL_Y - 1);
	return ret;
}

Value AesopEngine::stepFDIR(int argc, Value *argv) {
	Value ret;
	uint32 fdir = argv[0].fullValue;
	uint32 mtype = argv[1].fullValue;

	byte f = static_cast<byte>(fdir);

	switch(mtype) {
	case MTYP_TL:
		ret.fullValue = f ? (f - 1) : 3;
		break;
	case MTYP_TR:
		ret.fullValue = f == 3 ? 0 : (f + 1);
		break;
	}

	return ret;
}

Value AesopEngine::stepSquareX(int argc, Value *argv) {
	Value ret;
	uint32 x = argv[0].fullValue;
	uint32 r = argv[1].fullValue;
	uint32 dir = argv[2].fullValue;

	Value param[4];
	switch(dir) {
	case DIR_E:
		param[0].fullValue = x;
		param[1].fullValue = dir;
		param[2].fullValue = MTYP_F;
		param[3].fullValue = r & 0x01;
		x = stepX(0, param).fullValue;
		break;
	case DIR_W:
		param[0].fullValue = x;
		param[1].fullValue = dir;
		param[2].fullValue = MTYP_F;
		param[3].fullValue = !(r & 0x01);
		x = stepX(0, param).fullValue;
		break;
	}

	ret.fullValue = x;
	return ret;
}

Value AesopEngine::stepSquareY(int argc, Value *argv) {
	Value ret;
	uint32 y = argv[0].fullValue;
	uint32 r = argv[1].fullValue;
	uint32 dir = argv[2].fullValue;

	Value param[4];
	switch(dir) {
	case DIR_N:
		param[0].fullValue = y;
		param[1].fullValue = dir;
		param[2].fullValue = MTYP_F;
		param[3].fullValue = r < 2;
		y = stepY(0, param).fullValue;
		break;
	case DIR_S:
		param[0].fullValue = y;
		param[1].fullValue = dir;
		param[2].fullValue = MTYP_F;
		param[3].fullValue = r >= 2;
		y = stepY(0, param).fullValue;
		break;
	}

	ret.fullValue = y;
	return ret;
}

Value AesopEngine::stepRegion(int argc, Value *argv) {
	Value ret;
	uint32 r = argv[0].fullValue;
	uint32 dir = argv[1].fullValue;

	switch(dir)
	{
	case DIR_N:
	case DIR_S:
		r ^= 2;
		break;
	case DIR_E:
	case DIR_W:
		r ^= 1;
		break;
	}

	ret.fullValue = r;
	return ret;
}

Value AesopEngine::distance(int argc, Value *argv) {
	static int square_root[32] = 
	{
		0, 1, 4, 9, 16, 25, 36, 49, 64, 81, 100, 121, 144, 169, 196, 225, 256,
		289, 324, 361, 400, 441, 484, 529, 576, 625, 676, 729, 784, 841, 900, 961
	};

	Value ret;
	uint32 x1 = argv[0].fullValue;
	uint32 y1 = argv[1].fullValue;
	uint32 x2 = argv[2].fullValue;
	uint32 y2 = argv[3].fullValue;

	Value param;
	param.fullValue = x1 - x2;
	uint32 dx = absv(0, &param).fullValue;
	
	param.fullValue = y1 - y2;
	uint32 dy = absv(0, &param).fullValue;

	int32 num = (dx * dx) + (dy * dy);
	int32 root;
	for(root = 0; root < 31; root++) {
		if(square_root[root] >= num) {
			break;
		}
	}

	ret.fullValue = root;
	return ret;
}

Value AesopEngine::seekDirection(int argc, Value *argv) {
	Value ret;
	uint32 cur_x = argv[0].fullValue;
	uint32 cur_y = argv[1].fullValue;
	uint32 dest_x = argv[2].fullValue;
	uint32 dest_y = argv[3].fullValue;

	int32 dx = dest_x - cur_x;
	int32 dy = dest_y - cur_y;

	if(dx < 0) {
		if(dy > 0) {
			ret.fullValue = 5;
		}
		else if (dy < 0) {
			ret.fullValue = 7;
		}
		else {
			ret.fullValue = 6;
		}
	}
	else if(dx > 0) {
		if(dy > 0) {
			ret.fullValue = 3;
		}
		else if(dy < 0) {
			ret.fullValue = 1;
		}
		else {
			ret.fullValue = 2;
		}
	}
	else {
		if(dy > 0) {
			ret.fullValue = 4;
		}
		else if(dy < 0) {
			ret.fullValue = 0;
		}
		else {
			ret.fullValue = -1;
		}
	}

	return ret;
}

Value AesopEngine::spellRequest(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::spellList(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::magicField(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::doDots(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::doIce(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::readSaveDirectory(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::savegameTitle(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::writeSaveDirectory(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::saveGame(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::suspendGame(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::resumeItems(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::resumeLevel(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::changeLevel(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::restoreItems(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::restoreLevelObjects(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::readInitialItems(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::writeInitialTempfiles(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::createInitialBinaryFiles(int argc, Value *argv) {
	/*setSaveSlotnum(0);

	// if(file_time(items_txt) >= file_time(items_bin)) then the below
	debug("Translating %s to %s\n", items_txt, items_bin);
	translateFile(items_txt, items_bin, FIRST_ITEM, LAST_ITEM);

	for(int lvl = 1; lvl <= NUM_LEVELS; lvl++)
	{
		setSaveSlotnum(lvl);
		//if(file_time(lvl_txt) < file_time(lvl_bin)) continue;
		debug("Translating %s to %s\n", lvl_txt, lvl_bin);
		translateFile(lvl_txt, lvl_bin, FIRST_LVL_OBJ, LAST_LVL_OBJ);
	}*/

	// I actually think this might be a no-op. The game appears to already
	// come with these binary files and I can't find the source text files.
	// This may have just been a development thing in the engine.

	return Value(-1);
}

Value AesopEngine::launch(int argc, Value *argv) {
	// Another no-op.
	// FIXME: need to intercept calls to cine.exe and chargen.exe
	// and do the appropriate thing
	return Value(-1);
}

Value AesopEngine::openTransferFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::closeTransferFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::playerAttrib(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::itemAttrib(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::arrowCount(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

// Dungeon Hack code resource functions

Value AesopEngine::buildClipping(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::catString(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::closeFeatureFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::closeFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::copyWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::createFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::deleteSaves(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::drawAutoSquare(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::drawWalls(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::explodeSave(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::findLocationForMap(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::getFeatureRecord(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::initViewspace(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::loadLevelMap(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::loadVisibility(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::lockResource(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::long2hex(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::openFeatureFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::openFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::outputTime(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::pageFlip(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::pause(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::prepareSave(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::printerOnLine(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::randomizeArray(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::readArrayFromFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::readNumberFromFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::refreshMainTextWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::rollChance(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::saveVisibility(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::seedRandom(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::seekInFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::sequencePlaying(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::textBackground(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::touch(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::transition(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::unlockResource(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::updateFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::walkheap(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::windowCore(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::writeArrayToFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::writeLongToFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::writeMapheaderToFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::writeResourceToFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::xmsallocated(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::xmsfree(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

} // End of namespace Aesop
