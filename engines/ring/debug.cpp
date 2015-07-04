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
 */

#include "ring/debug.h"

#include "ring/base/application.h"
#include "ring/base/art.h"

#include "ring/graphics/visual/visual_encyclopedia.h"
#include "ring/graphics/image.h"
#include "ring/graphics/screen.h"

#include "ring/ring.h"

#include "common/debug-channels.h"
#include "common/file.h"

#ifdef RING_DUMP
// For mkdir

#ifdef WIN32
#include <direct.h>
#include <sys/stat.h>
#define my_mkdir(folder, mode) recursive_mkdir(folder)
#else
#include <unistd.h>
#define my_mkdir(folder, mode) recursive_mkdir(folder, mode)
#endif

#include <errno.h>
#include <sys/types.h>

static int recursive_mkdir(const char *dir) {
	char tmp[256];
	char *p;
	size_t len;

	snprintf(tmp, sizeof(tmp),"%s",dir);
	len = strlen(tmp);
	if(tmp[len - 1] == '\\')
		tmp[len - 1] = 0;
	for (p = tmp + 1; *p; p++) {
		if(*p == '\\') {
			*p = 0;
#ifdef WIN32
			int ret = mkdir(tmp);
#else
			int ret = mkdir(tmp, S_IRWXU);
#endif
			if (ret == -1 && errno != EEXIST)
				return ret;

			*p = '\\';
		}
	}
#ifdef WIN32
	return mkdir(tmp);
#else
	return mkdir(tmp, S_IRWXU);
#endif
}

#endif

namespace Ring {

Debugger::Debugger(RingEngine *engine) : _engine(engine), _action(kActionNone) {

	//////////////////////////////////////////////////////////////////////////
	// Register the debugger commands

	// General
	registerCmd("help",      WRAP_METHOD(Debugger, cmdHelp));

	// Data
	registerCmd("ls",        WRAP_METHOD(Debugger, cmdListFiles));

#ifdef RING_DUMP
	registerCmd("dump",      WRAP_METHOD(Debugger, cmdDumpArchive));
#endif

	// Widgets
	registerCmd("enc",       WRAP_METHOD(Debugger, cmdEncyclopedia));

	// Graphics
	registerCmd("clear",     WRAP_METHOD(Debugger, cmdClear));
	registerCmd("show",      WRAP_METHOD(Debugger, cmdShow));
}

Debugger::~Debugger() {
	DebugMan.clearAllDebugChannels();

	// Zero passed pointers
	_engine = NULL;
}

void Debugger::postEnter() {
	switch (_action) {
	default:
	case kActionNone:
		break;

	case kActionEncyclopedia: {
		// Create widget
		VisualObjectEncyclopedia *enc = new VisualObjectEncyclopedia(5);
		enc->init(_filename, kArchiveFile);
		enc->setType(5);
		enc->setVisible(true);

		// Setup and load page
		enc->alloc();
		enc->setParameters(Common::Point(20, 20), Common::Rect(50, 50, 580, 428), true);

		// Draw
		Common::Event ev;
		bool play = true;
		while (play) {

			while (g_system->getEventManager()->pollEvent(ev)) {
				switch (ev.type) {
				default:
					break;

				case Common::EVENT_KEYDOWN:
					// Exit on escape
					if (ev.kbd.keycode == Common::KEYCODE_ESCAPE)
						play = false;
					else
						enc->handleKey(ev.kbd.keycode);
					break;

				case Common::EVENT_MOUSEMOVE:
					enc->handleUpdate(ev.mouse);
					break;

				case Common::EVENT_LBUTTONDOWN:
					enc->handleLeftButtonDown(ev.mouse);
					break;

				case Common::EVENT_LBUTTONUP:
					enc->handleLeftButtonUp(ev.mouse);
					break;

				case Common::EVENT_QUIT:
					play = false;
					break;
				}

				// Skip remaining events when we need to exit
				if (!play)
					break;
			}

			// Draw the widget
			_engine->getApplication()->getScreenManager()->clear();
			enc->draw();
			_engine->getApplication()->getScreenManager()->updateScreen();

			// Update the screen
			g_system->updateScreen();
			g_system->delayMillis(100);
		}


		// Cleanup
		_filename.clear();
		enc->dealloc();
		delete enc;

		}
		break;
	}

	// Clear action
	_action = kActionNone;

	_engine->pauseEngine(false);
}

int Debugger::getNumber(const char *arg) const {
	return strtol(arg, (char **)NULL, 0);
}

bool Debugger::cmdHelp(int, const char **) {
	debugPrintf("Debug flags\n");
	debugPrintf("-----------\n");
	debugPrintf(" debugflag_list - Lists the available debug flags and their status\n");
	debugPrintf(" debugflag_enable - Enables a debug flag\n");
	debugPrintf(" debugflag_disable - Disables a debug flag\n");
	debugPrintf("\n");
	debugPrintf("Commands\n");
	debugPrintf("--------\n");
	debugPrintf(" ls   - list files in the archive\n");
	debugPrintf(" dump - dump the files from an archive\n");
	debugPrintf("\n");
	debugPrintf(" enc - load the encyclopedia");
	debugPrintf("\n");
	debugPrintf(" clear - clear the screen\n");
	debugPrintf(" show  - show an image\n");
	debugPrintf("\n");
	return true;
}

#pragma region Data

bool Debugger::cmdListFiles(int argc, const char **argv) {
	if (argc == 2) {
		Common::String filter(const_cast<char *>(argv[1]));

		Common::ArchiveMemberList list;
		int count = SearchMan.listMatchingMembers(list, filter);

		debugPrintf("Number of matches: %d\n", count);
		for (Common::ArchiveMemberList::iterator it = list.begin(); it != list.end(); ++it)
			debugPrintf(" %s\n", (*it)->getName().c_str());
	} else if (argc == 3) {
		Common::String filter(const_cast<char *>(argv[1]));
		Common::String filename(const_cast<char *>(argv[2]));

		filename.toLowercase();
		if (!filename.contains(".at2"))
			filename += ".at2";

		if (!SearchMan.hasFile(filename)) {
			debugPrintf("Cannot find file: %s\n", filename.c_str());
			return true;
		}

		// Load MUL archive
		Art *archive = new Art();
		archive->init(filename, kZoneNone, kLoadFromInvalid);

		Common::ArchiveMemberList list;
		int count = archive->listMatchingMembers(list, filter);

		debugPrintf("Number of matches: %d\n", count);
		for (Common::ArchiveMemberList::iterator it = list.begin(); it != list.end(); ++it)
			debugPrintf(" %s\n", (*it)->getName().c_str());

		delete archive;
	} else {
		debugPrintf("Syntax: ls <filter> (<archive>) (use * for all) \n");
	}

	return true;
}

#ifdef RING_DUMP
bool Debugger::cmdDumpArchive(int argc, const char **argv) {
	if (argc == 2) {
		Common::String filename(const_cast<char *>(argv[1]));

		if (filename == "*") {
			Common::ArchiveMemberList list;
			int count = SearchMan.listMatchingMembers(list, "*.at2");

			debugPrintf("Dumping %d archives\n", count);
			for (Common::ArchiveMemberList::iterator it = list.begin(); it != list.end(); ++it)
				dumpFile((*it)->getName());
		} else
			dumpFile(filename);
	} else {
		debugPrintf("Syntax: dump <filename>.at2 (use * to dump all archives) \n");
	}

	return true;
}

void Debugger::dumpFile(Common::String filename) {
#define CREATE_FOLDER(name) { \
	int ret = my_mkdir(name.c_str(), 600); \
	if (ret == -1 && errno != EEXIST) { \
		debugPrintf("Cannot create folder: %s (error: %s)", name.c_str(), strerror(errno)); \
		delete archive; \
		return; \
	} \
}

	filename.toLowercase();
	if (!filename.contains(".at2"))
		filename += ".at2";

	if (!SearchMan.hasFile(filename)) {
		debugPrintf("Cannot find file: %s\n", filename.c_str());
		return;
	}

	// Load MUL archive
	Art *archive = new Art();
	archive->init(filename, kZoneNone, kLoadFromInvalid);
	Common::ArchiveMemberList list;
	int count = archive->listMembers(list);

	// Get the current working folder
	char buffer[512];
	getcwd((char *)&buffer, 512);
	Common::String folder(buffer);

	// HACK replace path separators
	for (uint32 i = 0; i < filename.size(); i++) {
		if (filename[i] == '/')
			filename.setChar('\\', i);
	}

	// Create folder for dumping data
	Common::String dumpPath = folder + "\\dumps\\ring\\" + filename;
	CREATE_FOLDER(dumpPath);

	// Dump all members
	debugPrintf("Dumping %d files from archive %s\n", count, filename.c_str());
	for (Common::ArchiveMemberList::iterator it = list.begin(); it != list.end(); ++it) {
		Common::String name = (*it)->getName();
		Common::SeekableReadStream *stream = archive->createReadStreamForMember(name);

		byte *data = (byte *)calloc((size_t)stream->size(), 1);
		if (!data) {
			debugPrintf("Cannot allocated data for file %s (size: %d)", name.c_str(), stream->size());
			delete archive;
			delete stream;
			return;
		}

		memset(data, 0, (size_t)stream->size());
		stream->read(data, (size_t)stream->size());

		Common::String outPath = dumpPath;
		Common::String outFilename = name;

		// Create path if needed
		if (name.contains('\\')) {
			// Compute path & filename
			outFilename = lastPathComponent(name, '\\');
			outPath = outPath + name;

			for (uint j = 0; j < outFilename.size(); ++j)
				outPath.deleteLastChar();

			my_mkdir(outPath.c_str(), 600); \
		}

		Common::DumpFile out;
		if (out.open(outPath + '\\' + outFilename)) {
			out.write(data, (uint32)stream->size());
			out.close();
		}

		free(data);

		delete stream;

		debugPrintf("  - %s\n", name.c_str());
	}

	debugPrintf("\n");

	delete archive;

#undef CREATE_FOLDER
}
#endif

#pragma endregion

#pragma region Widgets

//////////////////////////////////////////////////////////////////////////
bool Debugger::cmdEncyclopedia(int argc, const char **argv) {
	if (argc == 1 || argc == 2) {

		// Setup action
		_action = kActionEncyclopedia;

		// Setup filename
		_filename = "E001.out";
		if (argc == 2)
			_filename = const_cast<char *>(argv[1]);

		return cmdExit(0, 0);
	} else {
		debugPrintf("Syntax: %s <filename> - load the encyclopedia\n", argv[0]);
	}

	return true;
}

#pragma endregion

#pragma region Graphics

//////////////////////////////////////////////////////////////////////////
bool Debugger::cmdClear(int argc, const char **) {
	if (argc == 1) {
		_engine->getApplication()->_screenManager->clear();
	} else {
		debugPrintf("Syntax: clear - clear the screen\n");
	}

	return true;
}

bool Debugger::cmdShow(int argc, const char ** argv) {
	if (argc == 2) {
		Common::String filename(const_cast<char *>(argv[1]));

		Image *image = new Image();
		if (!image->load(filename, kArchiveFile, kZoneNone, kLoadFromDisk, kDrawTypeNormal)) {
			debugPrintf("Cannot load image: %s", filename.c_str());

			delete image;
			return true;
		}

		_engine->getApplication()->_screenManager->drawImage(image, Common::Point(0, 0), image->getWidth(), image->getHeight(), 0, 0);
		delete image;

		// Refresh screen
		_engine->getApplication()->_screenManager->updateScreen();
		g_system->updateScreen();

	} else if (argc == 3) {
		Common::String filename(const_cast<char *>(argv[1]));

		Image *image = new Image();
		if (!image->load(filename, kArchiveArt, (ZoneId)getNumber(argv[2]), kLoadFromDisk, kDrawTypeNormal)) {
			debugPrintf("Cannot load image: %s", filename.c_str());

			delete image;
			return true;
		}

		_engine->getApplication()->_screenManager->drawImage(image, Common::Point(0, 0), image->getWidth(), image->getHeight(), 0, 0);
		delete image;

		// Refresh screen
		_engine->getApplication()->_screenManager->updateScreen();
		g_system->updateScreen();

	} else {
		debugPrintf("Syntax: show <filename> (<zone>)- Show an image\n");
	}

	return true;
}

#pragma endregion

} // End of namespace Ring
