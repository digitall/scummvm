#include "aesop.h"
#include "eye.h"

namespace Aesop {

extern AesopEngine* g_engine;

Value loadString(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value loadResource(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value copyString(int argc, Value *argv) {
	char *src = reinterpret_cast<char *>(argv[0].address);
	char *dst = reinterpret_cast<char *>(argv[1].address);
	strcpy(dst, src);
	return Value(-1);
}

Value stringForceLower(int argc, Value *argv) {
	char *dst = reinterpret_cast<char *>(argv[0].address);
	strlwr(dst);
	return Value(-1);
}

Value stringForceUpper(int argc, Value *argv) {
	char *dst = reinterpret_cast<char *>(argv[0].address);
	strupr(dst);
	return Value(-1);
}

Value stringLen(int argc, Value *argv) {
	Value ret;
	char *string = reinterpret_cast<char *>(argv[0].address);
	ret.fullValue = strlen(string);
	return ret;
}

Value stringCompare(int argc, Value *argv) {
	Value ret;
	char *str1 = reinterpret_cast<char *>(argv[0].address);
	char *str2 = reinterpret_cast<char *>(argv[1].address);
	ret.fullValue = scumm_stricmp(str1, str2);
	return ret;
}

Value beep(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value strval(int argc, Value *argv) {
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

Value envval(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value pokemem(int argc, Value *argv) {
	Value ret;
	ret.fullValue = -1;
	return ret;
}

Value peekmem(int argc, Value *argv) {
	Value ret;
	ret.fullValue = -1;
	return ret;
}

Value rnd(int argc, Value  *argv) {
	Value ret;
	uint32 low = argv[0].fullValue;
	uint32 high = argv[1].fullValue;
	ret.fullValue = g_engine->getRandomNumberRng(low, high);
	return ret;
}

Value dice(int argc, Value *argv) {
	uint32 total;
	Value ret;

	uint32 ndice = argv[0].fullValue;
	uint32 nsides = argv[1].fullValue;
	uint32 bonus = argv[2].fullValue;

	total = bonus;
	for (int i = 0; i < ndice; i++) {
      total += g_engine->getRandomNumberRng(1, nsides); 
	}
	ret.fullValue = total;

	return ret;
}

Value absv(int argc, Value *argv) {
	Value ret;
	int32 val = argv[0].fullValue;
	ret.fullValue = (val < 0) ? -val : val;
	return ret;
}

Value minv(int argc, Value *argv) {
	Value ret;
	int32 val1 = argv[0].fullValue;
	int32 val2 = argv[1].fullValue;
	ret.fullValue = MIN(val1, val2);
	return ret;
}

Value maxv(int argc, Value *argv) {
	Value ret;
	int32 val1 = argv[0].fullValue;
	int32 val2 = argv[1].fullValue;
	ret.fullValue = MAX(val1, val2);
	return ret;
}

Value diagnose(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value heapfree(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value notify(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value cancel(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value drainEventQueue(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value postEvent(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value sendEvent(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value peekEvent(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value dispatchEvent(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value flushEventQueue(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value flushInputEvents(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value initInterface(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value shutdownInterface(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value setMousePointer(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value setWaitPointer(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value standbyCursor(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value resumeCursor(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value showMouse(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value hideMouse(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value mouseXY(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value mouseInWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value lockMouse(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value unlockMouse(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value getkey(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value initGraphics(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value drawDot(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value drawLine(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value lineTo(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value drawRectangle(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value fillRectangle(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value hashRectangle(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value getBitmapHeight(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value drawBitmap(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value visibleBitmapRect(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value setPalette(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value refreshWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value wipeWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value shutdownGraphics(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value waitVerticalRetrace(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value readPalette(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value writePalette(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value pixelFade(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value colorFade(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value lightFade(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value assignWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value assignSubwindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value releaseWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value getX1(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value getX2(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value getY1(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value getY2(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value setX1(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value setX2(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value setY1(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value setY2(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value textWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value textStyle(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value textXY(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value textColor(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value textRefreshWindow(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value getTextX(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value getTextY(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value home(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value print(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value sprint(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value dprint(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value aprint(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value crout(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value charWidth(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value fontHeight(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value solidBarGraph(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value initSound(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value shutdownSound(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value loadSoundBlock(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value soundEffect(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value playSequence(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value loadMusic(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value unloadMusic(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value setSoundStatus(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value createObject(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value createProgram(int argc, Value *argv) {
	return g_engine->createProgram(argv[0].fullValue, argv[1].fullValue);
}

Value destroyObject(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value thrashCache(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value flushCache(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value stepX(int argc, Value *argv) {
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

Value stepY(int argc, Value *argv) {
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

	ret.fullValue == yy & (LVL_Y - 1);
	return ret;
}

Value stepFDIR(int argc, Value *argv) {
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

Value stepSquareX(int argc, Value *argv) {
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

Value stepSquareY(int argc, Value *argv) {
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

Value stepRegion(int argc, Value *argv) {
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

Value distance(int argc, Value *argv) {
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

Value seekDirection(int argc, Value *argv) {
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

Value spellRequest(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value spellList(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value magicField(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value doDots(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value doIce(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value readSaveDirectory(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value savegameTitle(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value writeSaveDirectory(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value saveGame(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value suspendGame(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value resumeItems(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value resumeLevel(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value changeLevel(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value restoreItems(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value restoreLevelObjects(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value readInitialItems(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value writeInitialTempfiles(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value createInitialBinaryFiles(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value launch(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value openTransferFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value closeTransferFile(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value playerAttrib(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value itemAttrib(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

Value arrowCount(int argc, Value *argv) {
	__debugbreak();
	return Value(-1);
}

} // End of namespace Aesop
