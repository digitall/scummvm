#include "script.h"
#include "aesop.h"

namespace Aesop {

Object::Object(uint32 objectId, Thunk *thunk) : _objectId(objectId), _thunk(thunk) {
	_thunk->useCount++;
}

Object::~Object() {
	_thunk->useCount--;
	if(_thunk->useCount == 0) {
		delete _thunk;
	}
}

uint32 Object::execute(uint32 messageNumber, uint32 vector, byte *stackPointer) {
	// FIXME: needed??
	if(vector == -1) {
		// TODO: get vector for messageNumber
	}

	// TODO save pointers before setting (ip, sp, etc.)
	// Does below code take care of this since I am using native stack to pass pointers?

	uint16 autoSize;
	byte *instructionPointer = getMessageHandlerAddress(messageNumber, autoSize);
	return execute(instructionPointer, stackPointer, autoSize);
}

uint32 Object::execute(byte *instructionPointer, byte *stackPointer, uint16 autoSize) {
	byte *framePointer = stackPointer;	// framePointer is the stackPointer at entry
	// TODO: initialize manifest THIS var
	stackPointer -= autoSize;		// make room for auto variables
	stackPointer -= sizeof(Value);	// point to first free stack location

	while(true) {
		switch(*instructionPointer) {
		case OP_BRT:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				if((valuePointer->value.low | valuePointer->value.high) != 0)
				{
					int idx = *reinterpret_cast<uint16 *>(instructionPointer + 1);
					instructionPointer = _thunk->codeBase + idx;
					continue;
				}
				instructionPointer += 2;
			}
			break;
		case OP_BRF:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				if((valuePointer->value.low | valuePointer->value.high) == 0)
				{
					int idx = *reinterpret_cast<uint16 *>(instructionPointer + 1);
					instructionPointer = _thunk->codeBase + idx;
					continue;
				}
				instructionPointer += 2;
			}
			break;
		case OP_BRA:
			{
				int idx = *reinterpret_cast<uint16 *>(instructionPointer + 1);
				instructionPointer = _thunk->codeBase + idx;
			}
			continue;
		case OP_CASE:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				Value temp = *valuePointer;
				instructionPointer++;
				int numberOfCases = *(reinterpret_cast<uint16 *>(instructionPointer));
				instructionPointer += 2;
				int idx;
				if(numberOfCases != 0)
				{
					for(int i = numberOfCases; i > 0; i--)
					{
						if(temp.fullValue == *(reinterpret_cast<uint32 *>(instructionPointer)))
						{
							instructionPointer += 4;
							idx = *reinterpret_cast<uint16 *>(instructionPointer);
							instructionPointer = _thunk->codeBase + idx;
							continue;
						}
						else 
						{
							instructionPointer += 6;
						}
					}
				}
				// case default
				idx = *reinterpret_cast<uint16 *>(instructionPointer);
				instructionPointer = _thunk->codeBase + idx;
			}
			continue;
		case OP_PUSH:
			{
				stackPointer -= sizeof(Value);
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue = 0;
			}
			break;
		case OP_DUP:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				Value temp = *valuePointer;
				stackPointer -= sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				*valuePointer = temp;
			}
			break;
		case OP_NOT:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.value.low = valuePointer->value.low | valuePointer->value.high;
				temp.value.high = 0;
				valuePointer->fullValue = temp.fullValue;
			}
			break;
		case OP_SETB:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.value.low = !(valuePointer->value.low | valuePointer->value.high);
				temp.value.high = 0;
				valuePointer->fullValue = temp.fullValue;
			}
			break;
		case OP_NEG:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue *= -1;
			}
			break;
		case OP_ADD:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.fullValue = valuePointer->fullValue;
				stackPointer += sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue += temp.fullValue;
			}
			break;
		case OP_SUB:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.fullValue = valuePointer->fullValue;
				stackPointer += sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue -= temp.fullValue;
			}
			break;
		case OP_MUL:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.fullValue = valuePointer->fullValue;
				stackPointer += sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue *= temp.fullValue;
			}
			break;
		case OP_DIV:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.fullValue = valuePointer->fullValue;
				stackPointer += sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue /= temp.fullValue;
			}
			break;
		case OP_MOD:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.fullValue = valuePointer->fullValue;
				stackPointer += sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue %= temp.fullValue;
			}
			break;
		case OP_EXP:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.value.low = valuePointer->value.low;
				stackPointer += sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue = pow(static_cast<double>(valuePointer->fullValue), static_cast<int>(temp.value.low));
			}
			break;
		case OP_BAND:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.fullValue = valuePointer->fullValue;
				stackPointer += sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue &= temp.fullValue;
			}
			break;
		case OP_BOR:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.fullValue = valuePointer->fullValue;
				stackPointer += sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue |= temp.fullValue;
			}
			break;
		case OP_XOR:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.fullValue = valuePointer->fullValue;
				stackPointer += sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue ^= temp.fullValue;
			}
			break;
		case OP_BNOT:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue = ~valuePointer->fullValue;
			}
			break;
		case OP_SHL:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.fullValue = valuePointer->fullValue;
				stackPointer += sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue <<= temp.fullValue;
			}
			break;
		case OP_SHR:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.fullValue = valuePointer->fullValue;
				stackPointer += sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue >>= temp.fullValue;
			}
			break;
		case OP_LT:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				(valuePointer + 1)->fullValue = valuePointer->fullValue < (valuePointer + 1)->fullValue;
				stackPointer += sizeof(Value);
			}
			break;
		case OP_LE:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				(valuePointer + 1)->fullValue = valuePointer->fullValue <= (valuePointer + 1)->fullValue;
				stackPointer += sizeof(Value);
			}
			break;
		case OP_EQ:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				(valuePointer + 1)->fullValue = valuePointer->fullValue == (valuePointer + 1)->fullValue;
				stackPointer += sizeof(Value);
			}
			break;
		case OP_NE:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				(valuePointer + 1)->fullValue = valuePointer->fullValue != (valuePointer + 1)->fullValue;
				stackPointer += sizeof(Value);
			}
			break;
		case OP_GE:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				(valuePointer + 1)->fullValue = valuePointer->fullValue >= (valuePointer + 1)->fullValue;
				stackPointer += sizeof(Value);
			}
			break;
		case OP_GT:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				(valuePointer + 1)->fullValue = valuePointer->fullValue > (valuePointer + 1)->fullValue;
				stackPointer += sizeof(Value);
			}
			break;
		case OP_INC:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue++;
			}
			break;
		case OP_DEC:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue--;
			}
			break;
		case OP_SHTC:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue = *(instructionPointer + 1);
				instructionPointer++;
			}
			break;
		case OP_INTC:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue = *reinterpret_cast<int16 *>(instructionPointer + 1);	// FIXME: should these be cast to uint16* ??
				instructionPointer += 2;
			}
			break;
		case OP_LNGC:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue = *reinterpret_cast<int32 *>(instructionPointer + 1);	// FIXME: should these be cast to uint16* ??
				instructionPointer += 4;
			}
			break;
		case OP_RCRS:
			{
				// We are dividing here because the original virtual machine
				// used offsets into an array of 32-bit code pointers and this gets
				// us the index into the array.
				int idx = *reinterpret_cast<uint16 *>(instructionPointer + 1);
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->address = reinterpret_cast<uintptr_t>(_thunk->externalCodeResources[idx / 4]);
				instructionPointer += 2;
			}
			break;
		case OP_CALL:
			{
				int argc = *(instructionPointer + 1);
				instructionPointer++;
				Value* argv = new Value[argc];
				// Arguments have been pushed in reverse order (I think)
				for(int i = argc - 1; i >= 0; i--)
				{
					argv[i] = *reinterpret_cast<Value *>(stackPointer);
					stackPointer += sizeof(Value);
				}
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);

				// FIXME: hackish way to save stack pointer context
				_thunk->engine->setStackPointer(stackPointer);
				byte *oldSp = stackPointer;
				*valuePointer = reinterpret_cast<CodeResource>(valuePointer->address)(argc, argv);
				_thunk->engine->setStackPointer(oldSp);

				delete argv;
			}
			break;
		case OP_SEND:
			{
				int argc = *(instructionPointer + 1);
				int msg = *reinterpret_cast<uint16 *>(instructionPointer + 2);
							
				// Set up arguments in reverse order
				Value *valuePointer;
				// FIXME: the below is f---ed up and doesn't work.
				// I am currently debugging case for one parameter so just skip for now
				/*for(int i = argc - 1; i >= 0; i--)
				{
					byte *tempPtr = stackPointer;
					tempPtr -= sizeof(Value);
					Value temp;
					valuePointer = reinterpret_cast<Value *>(stackPointer);
					temp.fullValue = valuePointer->fullValue;
					valuePointer = reinterpret_cast<Value *>(tempPtr);
					valuePointer->fullValue = temp.fullValue;
					stackPointer += sizeof(Value);
				}*/
				
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				int idx = valuePointer->fullValue;

				// FIXME: hackish way to save stack pointer context
				_thunk->engine->setStackPointer(stackPointer);
				byte *oldSp = stackPointer;
				valuePointer->fullValue = _thunk->engine->execute(idx, msg, -1);
				_thunk->engine->setStackPointer(oldSp);
				
				instructionPointer += 3;
			}
			break;
		case OP_PASS:
			// FIXME: hackish way to save stack pointer context as well?
			__debugbreak();
			break;
		case OP_JSR:
			{
				byte *hdr = instructionPointer + *reinterpret_cast<uint16 *>(instructionPointer + 1);
				int newAutoSize = *reinterpret_cast<uint16 *>(hdr);
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				// FIXME: are we jumping to the right place?
				valuePointer->fullValue = execute(hdr + 2, stackPointer, newAutoSize);
				instructionPointer += 2;
			}
			break;
		case OP_RTS:
			{
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				return valuePointer->fullValue;
			}
			break;
		case OP_AIM:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.value.low = valuePointer->value.low * *reinterpret_cast<uint16 *>(instructionPointer + 1);
				stackPointer += sizeof(Value);
				valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue = temp.fullValue;
				instructionPointer += 2;
			}
			break;
		case OP_AIS:
			{
				Value temp;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				temp.value.low = valuePointer->value.low;
				temp.fullValue <<= *(instructionPointer + 1);
				stackPointer += sizeof(Value);
				valuePointer->fullValue = temp.fullValue;
				instructionPointer++;
			}
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
			{
				int idx = *reinterpret_cast<uint16 *>(instructionPointer + 1);
				instructionPointer += 2;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->value.low = *(framePointer + idx);
			}
			break;
		case OP_LAW:
			{
				int idx = *reinterpret_cast<uint16 *>(instructionPointer + 1);
				instructionPointer += 2;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->value.low = *reinterpret_cast<uint16 *>(framePointer + idx);
			}
			break;
		case OP_LAD:
			{
				int idx = *reinterpret_cast<uint16 *>(instructionPointer + 1);
				instructionPointer += 2;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->fullValue = *reinterpret_cast<uint32 *>(framePointer + idx);
			}
			break;
		case OP_SAB:
			{
				int idx = *reinterpret_cast<uint16 *>(instructionPointer + 1);
				instructionPointer += 2;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				*(framePointer + idx) = static_cast<byte>(valuePointer->value.low);
			}
		case OP_SAW:
			{
				int idx = *reinterpret_cast<uint16 *>(instructionPointer + 1);
				instructionPointer += 2;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				*reinterpret_cast<uint16 *>(framePointer + idx) = valuePointer->value.low;
			}
			break;
		case OP_SAD:
			{
				int idx = *reinterpret_cast<uint16 *>(instructionPointer + 1);
				instructionPointer += 2;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				*reinterpret_cast<uint32 *>(framePointer + idx) = valuePointer->fullValue;
			}
			break;
		case OP_LABA:
			__debugbreak();
			//stackPointer->fullValue = *(g_framePointer - *reinterpret_cast<uint16 *>(instructionPointer + 1) + stackPointer->value.low);
			//instructionPointer += 2;
			break;
		case OP_LAWA:
			__debugbreak();
			//stackPointer->fullValue = *reinterpret_cast<uint16*>((g_framePointer - *reinterpret_cast<uint16 *>(instructionPointer + 1) + stackPointer->value.low));
			//instructionPointer += 2;
			break;
		case OP_LADA:
			__debugbreak();
			//stackPointer->fullValue = *reinterpret_cast<uint32*>((g_framePointer - *reinterpret_cast<uint16 *>(instructionPointer + 1) + stackPointer->value.low));
			//instructionPointer += 2;
			break;
		case OP_SABA:
			__debugbreak();
			break;
		case OP_SAWA:
			__debugbreak();
			break;
		case OP_SADA:
			__debugbreak();
			break;
		case OP_LEAA:
			__debugbreak();
			break;
		case OP_LSB:
			__debugbreak();
			break;
		case OP_LSW:
			__debugbreak();
			break;
		case OP_LSD:
			__debugbreak();
			break;
		case OP_SSB:
			__debugbreak();
			break;
		case OP_SSW:
			__debugbreak();
			break;
		case OP_SSD:
			__debugbreak();
			break;
		case OP_LSBA:
			__debugbreak();
			break;
		case OP_LSWA:
			__debugbreak();
			break;
		case OP_LSDA:
			__debugbreak();
			break;
		case OP_SSBA:
			__debugbreak();
			break;
		case OP_SSWA:
			__debugbreak();
			break;
		case OP_SSDA:
			__debugbreak();
			break;
		case OP_LESA:
			__debugbreak();
			break;
		case OP_LXB:
			__debugbreak();
			break;
		case OP_LXW:
			__debugbreak();
			break;
		case OP_LXD:
			__debugbreak();
			break;
		case OP_SXB:
			__debugbreak();
			break;
		case OP_SXW:
			__debugbreak();
			break;
		case OP_SXD:
			__debugbreak();
			break;
		case OP_LXBA:
			__debugbreak();
			break;
		case OP_LXWA:
			__debugbreak();
			break;
		case OP_LXDA:
			__debugbreak();
			break;
		case OP_SXBA:
			__debugbreak();
			break;
		case OP_SXWA:
			__debugbreak();
			break;
		case OP_SXDA:
			__debugbreak();
			break;
		case OP_LEXA:
			__debugbreak();
			break;
		case OP_SXAS:
			__debugbreak();
			break;
		case OP_LECA:
			{
				int offset = *reinterpret_cast<uint16 *>(instructionPointer + 1);
				instructionPointer += 2;
				Value *valuePointer = reinterpret_cast<Value *>(stackPointer);
				valuePointer->address = reinterpret_cast<uintptr_t>(_thunk->codeBase + offset); 
			}
			break;
		case OP_SOLE:
			__debugbreak();
			break;
		case OP_END:
			__debugbreak();
			break;
		case OP_BRK:
			__debugbreak();
			break;
		default:
			__debugbreak();
		}
		instructionPointer++;
	}

	// TODO restore pointers before exiting

	return 0; // FIXME: or -1 for "no value"?
}

byte* Object::getMessageHandlerAddress(int messageNumber, uint16 &autoSize) {
	byte *addr = _thunk->messageHandlers.getVal(messageNumber);
	autoSize = *reinterpret_cast<uint16 *>(addr);
	return addr + 2;
}

}  // End of namespace Aesop
