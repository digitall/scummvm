/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

// Disable symbol overrides so that we can use system headers.
#define FORBIDDEN_SYMBOL_ALLOW_ALL

#include "common/scummsys.h"

#ifdef _XBOX

#include <xtl.h>

#include "backends/platform/sdl/xbox/xbox.h"
#include "backends/plugins/sdl/sdl-provider.h"
#include "base/main.h"

#define CUSTOM_LAUNCH_MAGIC 0xEE456777

typedef struct {
	DWORD magic;
	CHAR szFilename[300];
	CHAR szLaunchXBEOnExit[100];
	CHAR szRemap_D_As[350];
	BYTE country;
	BYTE launchInsertedMedia;
	BYTE executionType;
	CHAR reserved[MAX_LAUNCH_DATA_SIZE-757];
} CUSTOM_LAUNCH_DATA, *PCUSTOM_LAUNCH_DATA;

int main(int argc, char *argv[]) {
#ifndef _XBOX360
	char *x_argv[100];
	int x_argc = 1;
	DWORD dwLaunchType;
	LAUNCH_DATA LaunchData;

	x_argv[0] = strdup("D:\\default.xbe");
	XGetLaunchInfo(&dwLaunchType, &LaunchData);

	if (dwLaunchType == LDT_TITLE && ((PCUSTOM_LAUNCH_DATA)&LaunchData)->magic == CUSTOM_LAUNCH_MAGIC)
	{		
		x_argv[x_argc] = strtok(((PCUSTOM_LAUNCH_DATA)&LaunchData)->szFilename, " ");
		while (x_argv[x_argc] != NULL) {
			x_argc++;
			x_argv[x_argc] = strtok(NULL, " ");
		}
	}
#endif

	// Create our OSystem instance
	g_system = new OSystem_Xbox();
	assert(g_system);

	// Pre initialize the backend
	((OSystem_Xbox *)g_system)->init();


	// Invoke the actual ScummVM main entry point:
#ifndef _XBOX360
	int res = scummvm_main(x_argc, x_argv);
#else
	int res = scummvm_main(argc, argv);
#endif

	// Free OSystem
	delete (OSystem_Xbox *)g_system;

	exit(res);
}

#endif
