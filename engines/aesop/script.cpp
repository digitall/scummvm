#include "script.h"
#include "aesop.h"

namespace Aesop {

// FIXME: global variables bad!
extern AesopEngine *g_engine;
extern byte *g_stackBase;
extern Value *g_stackPointer;
extern Value *g_framePointer;
extern byte *g_instructionPointer;

Object::Object(uint32 objectId, Thunk *thunk) : _objectId(objectId), _thunk(thunk) {
	_thunk->useCount++;
}

Object::~Object() {
	_thunk->useCount--;
	if(_thunk->useCount == 0) {
		delete _thunk;
	}
}

uint32 Object::execute(uint32 messageNumber, uint32 vector) {
	Value temp;
	Value *tempPtr;
	int numberOfCases;
	int idx;
	int argc;
	Value *argv;
	uint32 msg;
	uint16 autoSize;
	byte *autoVars;
	
	// FIXME: needed??
	if(vector == -1) {
		// TODO: get vector for messageNumber
	}

	// TODO save pointers before setting (ip, sp, etc.)

	g_instructionPointer = getMessageHandlerAddress(messageNumber, autoSize);
	autoVars = static_cast<byte *>(malloc(autoSize));

	while(true) {
		switch(*g_instructionPointer) {
		case OP_BRT:
			if((g_stackPointer->value.low | g_stackPointer->value.high) != 0)
			{
				idx = *reinterpret_cast<uint16 *>(g_instructionPointer + 1);
				g_instructionPointer = _thunk->codeBase + idx;
				continue;
			}
			g_instructionPointer += 2;
			break;
		case OP_BRF:
			if((g_stackPointer->value.low | g_stackPointer->value.high) == 0)
			{
				idx = *reinterpret_cast<uint16 *>(g_instructionPointer + 1);
				g_instructionPointer = _thunk->codeBase + idx;
				continue;
			}
			g_instructionPointer += 2;
			break;
		case OP_BRA:
			idx = *reinterpret_cast<uint16 *>(g_instructionPointer + 1);
			g_instructionPointer = _thunk->codeBase + idx;
			continue;
		case OP_CASE:
			temp = *g_stackPointer;
			g_instructionPointer++;
			numberOfCases = *(reinterpret_cast<uint16 *>(g_instructionPointer));
			g_instructionPointer += 2;
			if(numberOfCases != 0)
			{
				for(int i = numberOfCases; i > 0; i--)
				{
					if(temp.fullValue == *(reinterpret_cast<uint32 *>(g_instructionPointer)))
					{
						g_instructionPointer += 4;
						idx = *reinterpret_cast<uint16 *>(g_instructionPointer);
						g_instructionPointer = _thunk->codeBase + idx;
						continue;
					}
					else 
					{
						g_instructionPointer += 6;
					}
				}
			}
			// case default
			idx = *reinterpret_cast<uint16 *>(g_instructionPointer);
			g_instructionPointer = _thunk->codeBase + idx;
			continue;
		case OP_PUSH:
			g_stackPointer--;
			g_stackPointer->fullValue = 0;
			break;
		case OP_DUP:
			temp = *g_stackPointer;
			g_stackPointer--;
			*g_stackPointer = temp;
			break;
		case OP_NOT:
			temp.value.low = g_stackPointer->value.low | g_stackPointer->value.high;
			temp.value.high = 0;
			g_stackPointer->fullValue = temp.fullValue;
			break;
		case OP_SETB:
			temp.value.low = !(g_stackPointer->value.low | g_stackPointer->value.high);
			temp.value.high = 0;
			g_stackPointer->fullValue = temp.fullValue;
			break;
		case OP_NEG:
			g_stackPointer->fullValue *= -1;
			break;
		case OP_ADD:
			temp.fullValue = g_stackPointer->fullValue;
			g_stackPointer++;
			g_stackPointer->fullValue += temp.fullValue;
			break;
		case OP_SUB:
			temp.fullValue = g_stackPointer->fullValue;
			g_stackPointer++;
			g_stackPointer->fullValue -= temp.fullValue;
			break;
		case OP_MUL:
			temp.fullValue = g_stackPointer->fullValue;
			g_stackPointer++;
			g_stackPointer->fullValue *= temp.fullValue;
			break;
		case OP_DIV:
			temp.fullValue = g_stackPointer->fullValue;
			g_stackPointer++;
			g_stackPointer->fullValue /= temp.fullValue;
			break;
		case OP_MOD:
			temp.fullValue = g_stackPointer->fullValue;
			g_stackPointer++;
			g_stackPointer->fullValue %= temp.fullValue;
			break;
		case OP_EXP:
			temp.value.low = g_stackPointer->value.low;
			g_stackPointer++;
			g_stackPointer->fullValue = pow(static_cast<double>(g_stackPointer->fullValue), static_cast<int>(temp.value.low));
			break;
		case OP_BAND:
			temp.fullValue = g_stackPointer->fullValue;
			g_stackPointer++;
			g_stackPointer->fullValue &= temp.fullValue;
			break;
		case OP_BOR:
			temp.fullValue = g_stackPointer->fullValue;
			g_stackPointer++;
			g_stackPointer->fullValue |= temp.fullValue;
			break;
		case OP_XOR:
			temp.fullValue = g_stackPointer->fullValue;
			g_stackPointer++;
			g_stackPointer->fullValue ^= temp.fullValue;
			break;
		case OP_BNOT:
			g_stackPointer->fullValue = ~g_stackPointer->fullValue;
			break;
		case OP_SHL:
			temp.value.low = g_stackPointer->value.low;
			g_stackPointer++;
			g_stackPointer->fullValue <<= temp.value.low;
			break;
		case OP_SHR:
			temp.value.low = g_stackPointer->value.low;
			g_stackPointer++;
			g_stackPointer->fullValue >>= temp.value.low;
			break;
		case OP_LT:
			(g_stackPointer + 1)->fullValue = g_stackPointer->fullValue < (g_stackPointer + 1)->fullValue;
			g_stackPointer++;
			break;
		case OP_LE:
			(g_stackPointer + 1)->fullValue = g_stackPointer->fullValue <= (g_stackPointer + 1)->fullValue;
			g_stackPointer++;
			break;
		case OP_EQ:
			(g_stackPointer + 1)->fullValue = g_stackPointer->fullValue == (g_stackPointer + 1)->fullValue;
			g_stackPointer++;
			break;
		case OP_NE:
			(g_stackPointer + 1)->fullValue = g_stackPointer->fullValue != (g_stackPointer + 1)->fullValue;
			g_stackPointer++;
			break;
		case OP_GE:
			(g_stackPointer + 1)->fullValue = g_stackPointer->fullValue >= (g_stackPointer + 1)->fullValue;
			g_stackPointer++;
			break;
		case OP_GT:
			(g_stackPointer + 1)->fullValue = g_stackPointer->fullValue > (g_stackPointer + 1)->fullValue;
			g_stackPointer++;
			break;
		case OP_INC:
			g_stackPointer->fullValue++;
			break;
		case OP_DEC:
			g_stackPointer->fullValue--;
			break;
		case OP_SHTC:
			g_stackPointer->fullValue = *(g_instructionPointer + 1);
			g_instructionPointer++;
			break;
		case OP_INTC:
			g_stackPointer->fullValue = *reinterpret_cast<uint16 *>(g_instructionPointer + 1);	// FIXME: should these be cast to uint16* ??
			g_instructionPointer += 2;
			break;
		case OP_LNGC:
			g_stackPointer->fullValue = *reinterpret_cast<uint32 *>(g_instructionPointer + 1);	// FIXME: should these be cast to uint16* ??
			g_instructionPointer += 4;
			break;
		case OP_RCRS:
			// We are dividing here because the original virtual machine
			// used offsets into an array of 32-bit code pointers and this gets
			// us the index into the array.
			idx = *reinterpret_cast<uint16 *>(g_instructionPointer + 1);
			g_stackPointer->address = reinterpret_cast<uintptr_t>(_thunk->externalCodeResources[idx / 4]);
			g_instructionPointer += 2;
			break;
		case OP_CALL:
			argc = *(g_instructionPointer + 1);
			g_instructionPointer++;
			argv = new Value[argc];
			// Arguments have been pushed in reverse order (I think)
			for(int i = argc - 1; i >= 0; i--)
			{
				argv[i] = *g_stackPointer;
				g_stackPointer++;
			}
			*g_stackPointer = reinterpret_cast<CodeResource>(g_stackPointer->address)(argc, argv);
			delete argv;
			break;
		case OP_SEND:
			argc = *(g_instructionPointer + 1);
			// Set up arguments in reverse order;
			tempPtr = g_stackPointer;
			for(int i = argc - 1; i >= 0; i--)
			{
				tempPtr--;
				temp = *g_stackPointer;
				*tempPtr = temp;
				g_stackPointer++;
			}
			msg = *reinterpret_cast<uint16 *>(g_instructionPointer + 2);
			idx = g_stackPointer->fullValue;
			g_stackPointer->fullValue = g_engine->execute(idx, msg, -1);
			g_instructionPointer += 3;
			break;
		case OP_PASS:
			__debugbreak();
			break;
		case OP_JSR:
			__debugbreak();
			break;
		case OP_RTS:
			__debugbreak();
			break;
		case OP_AIM:
			temp.value.low = g_stackPointer->value.low * *reinterpret_cast<uint16 *>(g_instructionPointer + 1);
			g_stackPointer++;
			g_stackPointer->fullValue = temp.fullValue;
			g_instructionPointer += 2;
			break;
		case OP_AIS:
			temp.value.low = g_stackPointer->value.low;
			temp.fullValue <<= *(g_instructionPointer + 1);
			g_stackPointer++;
			g_stackPointer->fullValue = temp.fullValue;
			g_instructionPointer++;
			break;
		case OP_LTBA:
			break;
		case OP_LTWA:
			break;
		case OP_LTDA:
			break;
		case OP_LETA:
			break;
		case OP_LAB:
			idx = *reinterpret_cast<uint16 *>(g_instructionPointer + 1);
			g_instructionPointer += 2;
			g_stackPointer->value.low = *(autoVars + idx);
			break;
		case OP_LAW:
			idx = *reinterpret_cast<uint16 *>(g_instructionPointer + 1);
			g_instructionPointer += 2;
			g_stackPointer->value.low = *reinterpret_cast<uint16 *>(autoVars + idx);
			break;
		case OP_LAD:
			idx = *reinterpret_cast<uint16 *>(g_instructionPointer + 1);
			g_instructionPointer += 2;
			g_stackPointer->fullValue = *reinterpret_cast<uint32 *>(autoVars + idx);
			break;
		case OP_SAB:
			idx = *reinterpret_cast<uint16 *>(g_instructionPointer + 1);
			g_instructionPointer += 2;
			*(autoVars + idx) = static_cast<byte>(g_stackPointer->value.low);
		case OP_SAW:
			idx = *reinterpret_cast<uint16 *>(g_instructionPointer + 1);
			g_instructionPointer += 2;
			*reinterpret_cast<uint16 *>(autoVars + idx) = g_stackPointer->value.low;
			break;
		case OP_SAD:
			idx = *reinterpret_cast<uint16 *>(g_instructionPointer + 1);
			g_instructionPointer += 2;
			*reinterpret_cast<uint32 *>(autoVars + idx) = g_stackPointer->fullValue;
			break;
		case OP_LABA:
			//g_stackPointer->fullValue = *(g_framePointer - *reinterpret_cast<uint16 *>(g_instructionPointer + 1) + g_stackPointer->value.low);
			//g_instructionPointer += 2;
			break;
		case OP_LAWA:
			//g_stackPointer->fullValue = *reinterpret_cast<uint16*>((g_framePointer - *reinterpret_cast<uint16 *>(g_instructionPointer + 1) + g_stackPointer->value.low));
			//g_instructionPointer += 2;
			break;
		case OP_LADA:
			//g_stackPointer->fullValue = *reinterpret_cast<uint32*>((g_framePointer - *reinterpret_cast<uint16 *>(g_instructionPointer + 1) + g_stackPointer->value.low));
			//g_instructionPointer += 2;
			break;
		case OP_SABA:
			break;
		case OP_SAWA:
			break;
		case OP_SADA:
			break;
		case OP_LEAA:
			break;
		case OP_LSB:
			break;
		case OP_LSW:
			break;
		case OP_LSD:
			break;
		case OP_SSB:
			break;
		case OP_SSW:
			break;
		case OP_SSD:
			break;
		case OP_LSBA:
			break;
		case OP_LSWA:
			break;
		case OP_LSDA:
			break;
		case OP_SSBA:
			break;
		case OP_SSWA:
			break;
		case OP_SSDA:
			break;
		case OP_LESA:
			break;
		case OP_LXB:
			break;
		case OP_LXW:
			break;
		case OP_LXD:
			break;
		case OP_SXB:
			break;
		case OP_SXW:
			break;
		case OP_SXD:
			break;
		case OP_LXBA:
			break;
		case OP_LXWA:
			break;
		case OP_LXDA:
			break;
		case OP_SXBA:
			break;
		case OP_SXWA:
			break;
		case OP_SXDA:
			break;
		case OP_LEXA:
			break;
		case OP_SXAS:
			break;
		case OP_LECA:
			break;
		case OP_SOLE:
			break;
		case OP_END:
			break;
		case OP_BRK:
			break;
		default:
			__debugbreak();
		}
		g_instructionPointer++;
	}

	// TODO restore pointers before exiting

	free(autoVars);

	return 0; // FIXME
}

byte* Object::getMessageHandlerAddress(int messageNumber, uint16 &autoSize) {
	byte *addr = _thunk->messageHandlers.getVal(messageNumber);
	autoSize = *reinterpret_cast<uint16 *>(addr);
	return addr + 2;
}

}  // End of namespace Aesop
