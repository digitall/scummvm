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

#ifdef _XBOX

#include <xtl.h>

#include "common/scummsys.h"
#include "common/config-manager.h"
#include "backends/platform/sdl/xbox/xbox.h"
#include "backends/saves/default/default-saves.h"
#include "backends/events/xboxsdl/xboxsdl-events.h"
#include "backends/fs/windows/windows-fs-factory.h"

#define DEFAULT_CONFIG_FILE "scummvm.ini"

void OSystem_Xbox::init() {
	// Initialize File System Factory
	_fsFactory = new WindowsFilesystemFactory();

	// Invoke parent implementation of this method
	OSystem_SDL::init();
}

void OSystem_Xbox::initBackend() {
	ConfMan.set("joystick_num", 0);
	ConfMan.registerDefault("aspect_ratio", true);
	ConfMan.registerDefault("savepath", "D:\\saves");

	// Event source
	if (_eventSource == 0)
		_eventSource = new XboxSdlEventSource();

	// Invoke parent implementation of this method
	OSystem_SDL::initBackend();
}

Common::String OSystem_Xbox::getDefaultConfigFileName() {
	char configFile[MAXPATHLEN];

	strcpy(configFile, "D:\\");
	strcat(configFile, DEFAULT_CONFIG_FILE);

	return configFile;
}

Common::WriteStream *OSystem_Xbox::createLogFile() {
	// Start out by resetting _logFilePath, so that in case
	// of a failure, we know that no log file is open.
	_logFilePath.clear();

	char logFile[MAXPATHLEN];

	strcpy(logFile, "D:\\");
	strcat(logFile, "scummvm.log");
	Common::FSNode file(logFile);
	return file.createWriteStream();
}

#endif
