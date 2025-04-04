/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "fitd/common.h"
#include "fitd/fitd.h"
#include "fitd/gfx.h"
#include "fitd/pak.h"
#include "fitd/sequence.h"
#include "fitd/tatou.h"
#include "fitd/vars.h"

namespace Fitd {

void convertPaletteIfRequired(unsigned char *lpalette) {
	if(g_engine->getGameId() >= GID_JACK && g_engine->getGameId() < GID_AITD3)
	{
	    int i;
	    unsigned char* ptr2 = lpalette;
	    for(i=0;i<256;i++)
	    {
	        int j;
	        for(j=0;j<3;j++)
	        {
	            unsigned int composante = *(ptr2);
	            composante*=255;
	            composante/=63;
	            *(ptr2++) = composante&0xFF;
	        }
	    }
	}
}

const char *sequenceListAITD2[] =
	{
		"BATL",
		"GRAP",
		"CLE1",
		"CLE2",
		"COOK",
		"EXPL",
		"FALA",
		"FAL2",
		"GLIS",
		"GREN",
		"JEND",
		"MANI",
		"MER",
		"TORD",
		"PANT",
		"VERE",
		"PL21",
		"PL22",
		"ENDX",
		"SORT",
		"EFER",
		"STAR",
		"MEDU",
		"PROL",
		"GRAS",
		"STRI",
		"ITRO",
		"BILL",
		"PIRA",
		"PIR2",
		"VENT",
		"FIN",
		"LAST"};

void unapckSequenceFrame(unsigned char *source, unsigned char *dest) {
	unsigned char byteCode;

	byteCode = *(source++);

	while (byteCode) {
		if (!(--byteCode)) // change pixel or skip pixel
		{
			unsigned char changeColor;

			changeColor = *(source++);

			if (changeColor) {
				*(dest++) = changeColor;
			} else {
				dest++;
			}
		} else if (!(--byteCode)) // change 2 pixels or skip 2 pixels
		{
			unsigned char changeColor;

			changeColor = *(source++);

			if (changeColor) {
				*(dest++) = changeColor;
				*(dest++) = changeColor;
			} else {
				dest += 2;
			}
		} else if (!(--byteCode)) // fill or skip
		{
			unsigned char size;
			unsigned char fillColor;

			size = *(source++);
			fillColor = *(source++);

			if (fillColor) {
				int i;

				for (i = 0; i < size; i++) {
					*(dest++) = fillColor;
				}
			} else {
				dest += size;
			}
		} else // large fill of skip
		{
			uint16 size;
			unsigned char fillColor;

			size = READ_LE_U16(source);
			source += 2;
			fillColor = *(source++);

			if (fillColor) {
				int i;

				for (i = 0; i < size; i++) {
					*(dest++) = fillColor;
				}
			} else {
				dest += size;
			}
		}

		byteCode = *(source++);
	}
}

void playSequence(int sequenceIdx, int fadeStart, int fadeOutVar) {

	int frames = 0; /* Number of frames displayed */

	int var_4 = 1;
	int quitPlayback = 0;
	int nextFrame = 1;
	unsigned char localPalette[0x300];

	Common::String buffer;
	if (g_engine->getGameId() == GID_AITD2)
	{
	    buffer = Common::String::format("%s.PAK", sequenceListAITD2[sequenceIdx]);
	}
	else if (g_engine->getGameId() == GID_AITD3)
	{
	    buffer = Common::String::format("AN%d.PAK", sequenceIdx);
	}
	else
	{
		assert(0);
	}

	int numMaxFrames = PAK_getNumFiles(buffer.c_str());

	while (!quitPlayback) {
		int currentFrameId = 0;
		int sequenceParamIdx;

		while (currentFrameId < nextFrame) {
			frames++;

			timer = timeGlobal;

			if (currentFrameId >= numMaxFrames) {
				quitPlayback = 1;
				break;
			}

			if (!loadPak(buffer.c_str(), currentFrameId, logicalScreen)) {
				error("Error loading pak %s", buffer.c_str());
			}

			if (!currentFrameId) // first frame
			{
				memcpy(localPalette, logicalScreen, 0x300); // copy palette
				memcpy(aux, logicalScreen + 0x300, 64000);
				nextFrame = READ_LE_U16(logicalScreen + 64768);

				convertPaletteIfRequired(localPalette);

				if (var_4 != 0) {
					/*      if(fadeStart & 1)
					{
					fadeOut(0x10,0);
					}
					if(fadeStart & 4)
					{
					//memset(palette,0,0); // fade from black
					fadeInSub1(localPalette);
					flipOtherPalette(palette);
					} */

					gfx_setPalette(localPalette);
					copyPalette(localPalette, currentGamePalette);
				}
			} else // not first frame
			{
				uint32 frameSize;

				frameSize = READ_LE_U32(logicalScreen);

				if (frameSize < 64000) // key frame
				{
					unapckSequenceFrame((unsigned char *)logicalScreen + 4, (unsigned char *)aux);
				} else // delta frame
				{
					fastCopyScreen(logicalScreen, aux);
				}
			}

			for (sequenceParamIdx = 0; sequenceParamIdx < numSequenceParam; sequenceParamIdx++) {
				if (sequenceParams[sequenceParamIdx].frame == currentFrameId) {
					playSound(sequenceParams[sequenceParamIdx].sample);
				}
			}

			// TODO: here, timming management
			// TODO: fade management

			gfx_copyBlockPhys((unsigned char *)aux, 0, 0, 320, 200);

			osystem_drawBackground();

			currentFrameId++;

			for (int i = 0; i < 5; i++) // display the frame 5 times (original seems to wait 5 sync)
			{
				process_events();
			}

			if (key) {
				// stopSample();
				quitPlayback = 1;
				break;
			}
		}

		fadeOutVar--;

		if (fadeOutVar == 0) {
			quitPlayback = 1;
		}
	}

	flagInitView = 2;
}
} // namespace Fitd
