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

#include "aesop/aesop.h"
#include "aesop/resource.h"

namespace Aesop {

AesopEngine* AesopEngine::s_engine = NULL;

AesopEngine::AesopEngine(OSystem *syst) : Engine(syst) {
	debug("AesopEngine::AesopEngine");
	const Common::FSNode gameDataDir(ConfMan.get("path"));
	
	_rnd = new Common::RandomSource("aesop");

	s_engine = this;
	_stackBase = new byte[VM_STACK_SIZE];
	_stackPointer = _stackBase + VM_STACK_SIZE;
}

AesopEngine::~AesopEngine() {
	debug("AesopEngine::~AesopEngine");
	
	delete _rnd;
	delete [] _stackBase;
}

Common::Error AesopEngine::run() {
	uint32 ret;
	
	//initGraphics(320, 200, false);
	_console = new Console(this);
	_resMan = new ResourceManager();
	_resMan->init("eye.res");
	ret = createProgram(BOOTSTRAP, "start");
	
	return Common::kNoError;
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
   objectList[index] = createInstance(objectId);
   objectList[index]->execute(MSG_CREATE, -1, _stackPointer);
}

Object* AesopEngine::createInstance(uint32 objectId) {
	Resource *res = _resMan->getResource(objectId);
	
	if(res->thunk == NULL) {
		res->thunk = constructThunk(objectId);
	}

	return new Object(objectId, res->thunk);
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
						thunk->externalCodeResources.push_back(_resMan->getCodeResource(&tag[2]));
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
	for (int i = 0; i < ndice; i++) {
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
	__debugbreak();
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

Value AesopEngine::initGraphics(int argc, Value *argv) {
	__debugbreak();
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
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::visibleBitmapRect(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::setPalette(int argc, Value *argv) {
	__debugbreak();
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
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::shutdownSound(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::loadSoundBlock(int argc, Value *argv) {
	__debugbreak();
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
	__debugbreak();
	return Value(-1);
}

Value AesopEngine::launch(int argc, Value *argv) {
	__debugbreak();
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
