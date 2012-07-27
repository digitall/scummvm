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

namespace Aesop {

// FIXME: global variables bad!
AesopEngine *g_engine;
byte *g_stackBase;
Value *g_stackPointer;
Value *g_framePointer;
byte *g_instructionPointer;

AesopEngine::AesopEngine(OSystem *syst) : Engine(syst) {
	debug("AesopEngine::AesopEngine");
	const Common::FSNode gameDataDir(ConfMan.get("path"));
	
	_rnd = new Common::RandomSource("aesop");

	g_engine = this;
	g_stackBase = static_cast<byte *>(malloc(VM_STACK_SIZE));
	g_stackPointer = reinterpret_cast<Value *>(g_stackBase + VM_STACK_SIZE);
}

AesopEngine::~AesopEngine() {
	debug("AesopEngine::~AesopEngine");
	
	delete _rnd;
	free(g_stackBase);
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
   objectList[index]->execute(MSG_CREATE, -1);
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
	return objectList[index]->execute(messageNumber, vector);
}

uint AesopEngine::getRandomNumberRng(uint min, uint max) {
	return _rnd->getRandomNumberRng(min, max);
}

} // End of namespace Aesop
