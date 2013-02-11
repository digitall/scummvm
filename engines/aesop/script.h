#ifndef SCRIPT_H
#define SCRIPT_H

#include "common/scummsys.h"
#include "common/list.h"
#include "common/hashmap.h"

namespace Aesop {

// FIXME: probably not needed
const int MAX_G = 16;	// Maximum depth of "family trees"

union Value {
	Value() { }
	Value(int32 i) : fullValue(i) { }
	uintptr_t address;
	int32 fullValue;	// FIXME: should these be signed? (to make the addition, etc. opcodes work correctly)
	struct {
		uint16 low;
		uint16 high;
	} value;
};

#include "common/pack-start.h"

struct ProgramHeader {
	uint16 staticSize;
	uint32 imports;
	uint32 exports;
	uint32 parent;
} PACKED_STRUCT;

#include "common/pack-end.h"

enum MessageTokens {	// predefined message tokens (sent by system)
	MSG_CREATE = 0,
	MSG_DESTROY = 1,
	MSG_RESTORE = 2
};

enum Opcodes {
	OP_BRT = 0x00,
	OP_BRF = 0x01,
	OP_BRA = 0x02,
	OP_CASE = 0x03,
	OP_PUSH = 0x04,
	OP_DUP = 0x05,
	OP_NOT = 0x06,
	OP_SETB = 0x07,
	OP_NEG = 0x08,
	OP_ADD = 0x09,
	OP_SUB = 0x0A,
	OP_MUL = 0x0B,
	OP_DIV = 0x0C,
	OP_MOD = 0x0D,
	OP_EXP = 0x0E,
	OP_BAND = 0x0F,
	OP_BOR = 0x10,
	OP_XOR = 0x11,
	OP_BNOT = 0x12,
	OP_SHL = 0x13,
	OP_SHR = 0x14,
	OP_LT = 0x15,
	OP_LE = 0x16,
	OP_EQ = 0x17,
	OP_NE = 0x18,
	OP_GE = 0x19,
	OP_GT = 0x1A,
	OP_INC = 0X1B,
	OP_DEC = 0x1C,
	OP_SHTC = 0x1D,
	OP_INTC = 0x1E,
	OP_LNGC = 0x1F,
	OP_RCRS = 0x20,
	OP_CALL = 0x21,
	OP_SEND = 0x22,
	OP_PASS = 0x23,
	OP_JSR = 0x24,
	OP_RTS = 0x25,
	OP_AIM = 0x26,
	OP_AIS = 0x27,
	OP_LTBA = 0x28,
	OP_LTWA = 0x29,
	OP_LTDA = 0x2A,
	OP_LETA = 0x2B,
	OP_LAB = 0x2C,
	OP_LAW = 0x2D,
	OP_LAD = 0x2E,
	OP_SAB = 0x2F,
	OP_SAW = 0x30,
	OP_SAD = 0x31,
	OP_LABA = 0x32,
	OP_LAWA = 0x33,
	OP_LADA = 0x34,
	OP_SABA = 0x35,
	OP_SAWA = 0x36,
	OP_SADA = 0x37,
	OP_LEAA = 0x38,
	OP_LSB = 0x39,
	OP_LSW = 0x3A,
	OP_LSD = 0x3B,
	OP_SSB = 0x3C,
	OP_SSW = 0x3D,
	OP_SSD = 0x3E,
	OP_LSBA = 0x3F,
	OP_LSWA = 0x40,
	OP_LSDA = 0x41,
	OP_SSBA = 0x42,
	OP_SSWA = 0x43,
	OP_SSDA = 0x44,
	OP_LESA = 0x45,
	OP_LXB = 0x46,
	OP_LXW = 0x47,
	OP_LXD = 0x48,
	OP_SXB = 0x49,
	OP_SXW = 0x4A,
	OP_SXD = 0x4B,
	OP_LXBA = 0x4C,
	OP_LXWA = 0x4D,
	OP_LXDA = 0x4E,
	OP_SXBA = 0x4F,
	OP_SXWA = 0x50,
	OP_SXDA = 0x51,
	OP_LEXA = 0x52,
	OP_SXAS = 0x53,
	OP_LECA = 0x54,
	OP_SOLE = 0x55,
	OP_END = 0x56,
	OP_BRK = 0x57
};               

struct ExternalReference {
	char variableType;
	char *variableName;
	int classReference;
};

typedef Value (*CodeResource)(int argc, Value *argv);

class AesopEngine;

class Thunk {
public:
	Thunk() { }

	int useCount;
	Common::HashMap<uint32, CodeResource> externalCodeResources;
	Common::Array<ExternalReference> externalReferences;
	Common::HashMap<uint32, byte *> messageHandlers;
	byte* codeBase;
	AesopEngine *engine;
};

class Object {
public:
	Object(uint32 objectId, int index, Thunk *thunk);
	~Object();

	uint32 execute(uint32 messageNumber, uint32 vector, byte *stackPointer);
private:
	uint32 execute(byte *instructionPointer, byte *stackPointer, uint16 autoSize);
	byte* getMessageHandlerAddress(int messageNumber, uint16 &autoSize);

	uint32 _objectId;
	uint32 _index;
	Thunk *_thunk;
};

} // End of namespace Aesop

#endif
