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

} // End of namespace Aesop

#endif
