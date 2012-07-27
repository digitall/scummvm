#include "resource.h"

namespace Aesop {

Resource::Resource() : thunk(NULL) {
}

Resource::~Resource() {
	free(this->data);
}

ResourceManager::~ResourceManager() {
	delete _file;
	delete _header;
	delete _resourceNames;
	delete _resourceCache;
}

void ResourceManager::init(const char *filename) {
	RESTableHeader tbl;
	int32 beginningOfTable;
	uint16 stringLength;
	char *string = NULL;
	char *number = NULL;
	
	_file = new Common::File;
	_file->open(filename);
	
	_header = new RESGlobalHeader;
	_file->read(_header, sizeof(RESGlobalHeader));
	assert(strcmp(_header->signature, "AESOP/16 V1.00") == 0);

	_currentDirBlockNumber = -1;

	// load resource names (i.e. Table 0)
	_resourceNames = new ResourceMap;
	this->seek(0);
	beginningOfTable = _file->pos();
	_file->read(&tbl, sizeof(RESTableHeader));
	for(int i = 0; i < 512; i++) {
		_file->seek(beginningOfTable + tbl.offsets[i], SEEK_SET);
		_file->read(&stringLength, sizeof(stringLength));
		while(stringLength > 0) {
			if(string == NULL) {
				string = new char[stringLength];
				_file->read(string, stringLength);
			}
			else {
				number = new char[stringLength];
				_file->read(number, stringLength);
				_resourceNames->setVal(string, atoi(number));
				string = NULL;
				number = NULL;
			}
			_file->read(&stringLength, sizeof(stringLength));
		}
	}

	_resourceCache = new ResourceCache;
	loadCodeResources();
}

uint32 ResourceManager::getResourceId(const char *resourceName) {
	return _resourceNames->getVal(resourceName);
}

// FIXME: how should we free resources from the cache?
Resource* ResourceManager::getResource(uint32 resourceId) {
	Resource *res;

	if(_resourceCache->contains(resourceId)) {
		res = _resourceCache->getVal(resourceId);
	}
	else {
		res = new Resource;
		res->id = resourceId;
		res->size = this->seek(resourceId);
		res->data = static_cast<byte *>(malloc(res->size));
		_file->read(res->data, res->size);
		_resourceCache->setVal(resourceId, res);
	}

	return res;
}

uint32 ResourceManager::seek(uint32 resourceId) {
	uint32 blockNumber, nextBlock;
	uint16 entryNumber;
	RESEntryHeader entry;

	blockNumber = resourceId / 128;	// 128 is number of entries in a directory block
	entryNumber = resourceId % 128;

	if(_currentDirBlockNumber != blockNumber) {
		_currentDirBlockNumber = blockNumber;
		nextBlock = _header->firstDirectoryBlock;

		do {
			_file->seek(nextBlock, SEEK_SET);
			_file->read(&_currentDirBlock, sizeof(RESDirectoryBlock));
			nextBlock = _currentDirBlock.nextDirectoryBlock;

		} while(blockNumber--);
		_currentDirBlockPos = _file->pos();
	}
	else {
		_file->seek(_currentDirBlockPos, SEEK_SET);
	}

	_file->seek(_currentDirBlock.entryHeaderIndex[entryNumber], SEEK_SET);
	_file->read(&entry, sizeof(RESEntryHeader));

	return entry.dataSize;
}

void ResourceManager::loadCodeResources() {
	_codeResources = new CodeResourceMap;
	_codeResources->setVal("load_string", loadString);
	_codeResources->setVal("load_resource", loadResource);
	_codeResources->setVal("copy_string", copyString);
	_codeResources->setVal("string_force_lower", stringForceLower);
	_codeResources->setVal("string_force_upper", stringForceUpper);
	_codeResources->setVal("string_len", stringLen);
	_codeResources->setVal("string_compare", stringCompare);
	_codeResources->setVal("beep", beep);
	_codeResources->setVal("strval", strval);
	_codeResources->setVal("envval", envval);
	_codeResources->setVal("pokemem", pokemem);
	_codeResources->setVal("peekmem", peekmem);
	_codeResources->setVal("rnd", rnd);
	_codeResources->setVal("dice", dice);
	_codeResources->setVal("absv", absv);
	_codeResources->setVal("minv", minv);
	_codeResources->setVal("maxv", maxv);
	_codeResources->setVal("diagnose", diagnose);
	_codeResources->setVal("heapfree", heapfree);
	_codeResources->setVal("notify", notify);
	_codeResources->setVal("cancel", cancel);
	_codeResources->setVal("drain_event_queue", drainEventQueue);
	_codeResources->setVal("post_event", postEvent);
	_codeResources->setVal("send_event", sendEvent);
	_codeResources->setVal("peek_event", peekEvent);
	_codeResources->setVal("dispatch_event", dispatchEvent);
	_codeResources->setVal("flush_event_queue", flushEventQueue);
	_codeResources->setVal("flush_input_events", flushInputEvents);
	_codeResources->setVal("init_interface", initInterface);
	_codeResources->setVal("shutdown_interface", shutdownInterface);
	_codeResources->setVal("set_mouse_pointer", setMousePointer);
	_codeResources->setVal("set_wait_pointer", setWaitPointer);
	_codeResources->setVal("standby_cursor", standbyCursor);
	_codeResources->setVal("resume_cursor", resumeCursor);
	_codeResources->setVal("show_mouse", showMouse);
	_codeResources->setVal("hide_mouse", hideMouse);
	_codeResources->setVal("mouse_XY", mouseXY);
	_codeResources->setVal("mouse_in_window", mouseInWindow);
	_codeResources->setVal("lock_mouse", lockMouse);
	_codeResources->setVal("unlock_mouse", unlockMouse);
	_codeResources->setVal("getkey", getkey);
	_codeResources->setVal("init_graphics", initGraphics);
	_codeResources->setVal("draw_dot", drawDot);
	_codeResources->setVal("draw_line", drawLine);
	_codeResources->setVal("line_to", lineTo);
	_codeResources->setVal("draw_rectangle", drawRectangle);
	_codeResources->setVal("fill_rectangle", fillRectangle);
	_codeResources->setVal("hash_rectangle", hashRectangle);
	_codeResources->setVal("get_bitmap_height", getBitmapHeight);
	_codeResources->setVal("draw_bitmap", drawBitmap);
	_codeResources->setVal("visible_bitmap_rect", visibleBitmapRect);
	_codeResources->setVal("set_palette", setPalette);
	_codeResources->setVal("refresh_window", refreshWindow);
	_codeResources->setVal("wipe_window", wipeWindow);
	_codeResources->setVal("shutdown_graphics", shutdownGraphics);
	_codeResources->setVal("wait_vertical_retrace", waitVerticalRetrace);
	_codeResources->setVal("read_palette", readPalette);
	_codeResources->setVal("write_palette", writePalette);
	_codeResources->setVal("pixel_fade", pixelFade);
	_codeResources->setVal("color_fade", colorFade);
	_codeResources->setVal("light_fade", lightFade);
	_codeResources->setVal("assign_window", assignWindow);
	_codeResources->setVal("assign_subwindow", assignSubwindow);
	_codeResources->setVal("release_window", releaseWindow);
	_codeResources->setVal("get_x1", getX1);
	_codeResources->setVal("get_x2", getX2);
	_codeResources->setVal("get_y1", getY1);
	_codeResources->setVal("get_y2", getY2);
	_codeResources->setVal("set_x1", setX1);
	_codeResources->setVal("set_x2", setX2);
	_codeResources->setVal("set_y1", setY1);
	_codeResources->setVal("set_y2", setY2);
	_codeResources->setVal("text_window", textWindow);
	_codeResources->setVal("text_style", textStyle);
	_codeResources->setVal("text_xy", textXY);
	_codeResources->setVal("text_color", textColor);
	_codeResources->setVal("text_refresh_window", textRefreshWindow);
	_codeResources->setVal("get_text_x", getTextX);
	_codeResources->setVal("get_text_y", getTextY);
	_codeResources->setVal("home", home);
	_codeResources->setVal("print", print);
	_codeResources->setVal("sprint", sprint);
	_codeResources->setVal("dprint", dprint);
	_codeResources->setVal("aprint", aprint);
	_codeResources->setVal("crout", crout);
	_codeResources->setVal("char_width", charWidth);
	_codeResources->setVal("font_height", fontHeight);
	_codeResources->setVal("solid_bar_graph", solidBarGraph);
	_codeResources->setVal("init_sound", initSound);
	_codeResources->setVal("shutdown_sound", shutdownSound);
	_codeResources->setVal("load_sound_block", loadSoundBlock);
	_codeResources->setVal("sound_effect", soundEffect);
	_codeResources->setVal("play_sequence", playSequence);
	_codeResources->setVal("load_music", loadMusic);
	_codeResources->setVal("unload_music", unloadMusic);
	_codeResources->setVal("set_sound_status", setSoundStatus);
	_codeResources->setVal("create_object", createObject);
	_codeResources->setVal("create_program", createProgram);
	_codeResources->setVal("destroy_object", destroyObject);
	_codeResources->setVal("thrash_cache", thrashCache);
	_codeResources->setVal("flush_cache", flushCache);
	_codeResources->setVal("step_X", stepX);
	_codeResources->setVal("step_Y", stepY);
	_codeResources->setVal("step_FDIR", stepFDIR);
	_codeResources->setVal("step_square_X", stepSquareX);
	_codeResources->setVal("step_square_Y", stepSquareY);
	_codeResources->setVal("step_region", stepRegion);
	_codeResources->setVal("distance", distance);
	_codeResources->setVal("seek_direction", seekDirection);
	_codeResources->setVal("spell_request", spellRequest);
	_codeResources->setVal("spell_list", spellList);
	_codeResources->setVal("magic_field", magicField);
	_codeResources->setVal("do_dots", doDots);
	_codeResources->setVal("do_ice", doIce);
	_codeResources->setVal("read_save_directory", readSaveDirectory);
	_codeResources->setVal("savegame_title", savegameTitle);
	_codeResources->setVal("write_save_directory", writeSaveDirectory);
	_codeResources->setVal("save_game", saveGame);
	_codeResources->setVal("suspend_game", suspendGame);
	_codeResources->setVal("resume_items", resumeItems);
	_codeResources->setVal("resume_level", resumeLevel);
	_codeResources->setVal("change_level", changeLevel);
	_codeResources->setVal("restore_items", restoreItems);
	_codeResources->setVal("restore_level_objects", restoreLevelObjects);
	_codeResources->setVal("read_initial_items", readInitialItems);
	_codeResources->setVal("write_initial_tempfiles", writeInitialTempfiles);
	_codeResources->setVal("create_initial_binary_files", createInitialBinaryFiles);
	_codeResources->setVal("launch", launch);
	_codeResources->setVal("open_transfer_file", openTransferFile);
	_codeResources->setVal("close_transfer_file", closeTransferFile);
	_codeResources->setVal("player_attrib", playerAttrib);
	_codeResources->setVal("item_attrib", itemAttrib);
	_codeResources->setVal("arrow_count", arrowCount);

#ifdef DUNGEON_HACK
	_codeResources->setVal("build_clipping", buildClipping);
	_codeResources->setVal("cat_string", catString);
	_codeResources->setVal("close_feature_file", closeFeatureFile);
	_codeResources->setVal("close_file", closeFile);
	_codeResources->setVal("copy_window", copyWindow);
	_codeResources->setVal("create_file", createFile);
	_codeResources->setVal("delete_saves", deleteSaves);
	_codeResources->setVal("draw_auto_square", drawAutoSquare);
	_codeResources->setVal("draw_walls", drawWalls);
	_codeResources->setVal("explode_save", explodeSave);
	_codeResources->setVal("find_location_for_map", findLocationForMap);
	_codeResources->setVal("get_feature_record", getFeatureRecord);
	_codeResources->setVal("init_viewspace", initViewspace);
	_codeResources->setVal("load_level_map", loadLevelMap);
	_codeResources->setVal("load_visibility", loadVisibility);
	_codeResources->setVal("lock_resource", lockResource);
	_codeResources->setVal("long2hex", long2hex);
	_codeResources->setVal("open_feature_file", openFeatureFile);
	_codeResources->setVal("open_file", openFile);
	_codeResources->setVal("output_time", outputTime);
	_codeResources->setVal("page_flip", pageFlip);
	_codeResources->setVal("pause", pause);
	_codeResources->setVal("prepare_save", prepareSave);
	_codeResources->setVal("printer_on_line", printerOnLine);
	_codeResources->setVal("randomize_array", randomizeArray);
	_codeResources->setVal("read_array_from_file", readArrayFromFile);
	_codeResources->setVal("read_number_from_file", readNumberFromFile);
	_codeResources->setVal("refresh_main_text_window", refreshMainTextWindow);
	_codeResources->setVal("roll_chance", rollChance);
	_codeResources->setVal("save_visibility", saveVisibility);
	_codeResources->setVal("seed_random", seedRandom);
	_codeResources->setVal("seek_in_file", seekInFile);
	_codeResources->setVal("sequence_playing", sequencePlaying);
	_codeResources->setVal("text_background", textBackground);
	_codeResources->setVal("touch", touch);
	_codeResources->setVal("transition", transition);
	_codeResources->setVal("unlock_resource", unlockResource);
	_codeResources->setVal("update_file", updateFile);
	_codeResources->setVal("walkheap", walkheap);
	_codeResources->setVal("window_core", windowCore);
	_codeResources->setVal("write_array_to_file", writeArrayToFile);
	_codeResources->setVal("write_long_to_file", writeLongToFile);
	_codeResources->setVal("write_mapheader_to_file", writeMapheaderToFile);
	_codeResources->setVal("write_resource_to_file", writeResourceToFile);
	_codeResources->setVal("xmsallocated", xmsallocated);
	_codeResources->setVal("xmsfree", xmsfree);
#endif
}

CodeResource ResourceManager::getCodeResource(const char *codeResourceName) {
	return _codeResources->getVal(codeResourceName);
}

} // End of namespace Aesop
