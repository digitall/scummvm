#ifndef AESOP_H
#define AESOP_H

#include "common/random.h"
#include "engines/engine.h"
#include "gui/debugger.h"

#include "resource.h"
#include "script.h"

namespace Aesop {

const int VM_STACK_SIZE = 4096 * sizeof(void *); // 16K for 32-bit binary, 32K for 64-bit binary
const int BOOTSTRAP	= 2000;		// Object which launches AESOP application
const int NUM_OBJECTS = 2048;	// Up to 2,048 objects in Eye III universe
const int NUM_ENTITIES = 2000;	// Up to 2,000 physical entities in Eye III universe

class Console;

class AesopEngine : public Engine {
public:
	AesopEngine(OSystem *syst);
	~AesopEngine();
	
	virtual Common::Error run();
	uint32 execute(int index, uint32 messageNumber, uint32 vector);
	uint32 createProgram(int index, uint32 resource);
	uint getRandomNumberRng(uint min, uint max);
private:
	void initObjectList();
	uint32 createProgram(int index, const char *resourceName);
	int findFreeEntry(int min, int end);
	void createSOPInstance(uint32 objectId, int index);
	Object* createInstance(uint32 objectId);
	Thunk* constructThunk(uint32 objectId);

	Console *_console;
	Common::RandomSource *_rnd;
	ResourceManager *_resMan;
	byte objectFlags[NUM_OBJECTS];
	Object* objectList[NUM_OBJECTS];
};

class Console : public GUI::Debugger {
public:
	Console(AesopEngine *vm) {}
	virtual ~Console() {}
};

} // End of namespace Aesop

#endif
