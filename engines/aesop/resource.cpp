#include "resource.h"

namespace Aesop {

Resource::Resource() : thunk(NULL) {
}

Resource::~Resource() {
	delete[] this->data;
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

	_currentDirBlockNumber = static_cast<uint32>(-1);

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
		res->data = new byte[res->size];
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
	_codeResources->setVal("load_string", AesopEngine::loadString);
	_codeResources->setVal("load_resource", AesopEngine::loadResource);
	_codeResources->setVal("copy_string", AesopEngine::copyString);
	_codeResources->setVal("string_force_lower", AesopEngine::stringForceLower);
	_codeResources->setVal("string_force_upper", AesopEngine::stringForceUpper);
	_codeResources->setVal("string_len", AesopEngine::stringLen);
	_codeResources->setVal("string_compare", AesopEngine::stringCompare);
	_codeResources->setVal("beep", AesopEngine::beep);
	_codeResources->setVal("strval", AesopEngine::strval);
	_codeResources->setVal("envval", AesopEngine::envval);
	_codeResources->setVal("pokemem", AesopEngine::pokemem);
	_codeResources->setVal("peekmem", AesopEngine::peekmem);
	_codeResources->setVal("rnd", AesopEngine::rnd);
	_codeResources->setVal("dice", AesopEngine::dice);
	_codeResources->setVal("absv", AesopEngine::absv);
	_codeResources->setVal("minv", AesopEngine::minv);
	_codeResources->setVal("maxv", AesopEngine::maxv);
	_codeResources->setVal("diagnose", AesopEngine::diagnose);
	_codeResources->setVal("heapfree", AesopEngine::heapfree);
	_codeResources->setVal("notify", AesopEngine::notify);
	_codeResources->setVal("cancel", AesopEngine::cancel);
	_codeResources->setVal("drain_event_queue", AesopEngine::drainEventQueue);
	_codeResources->setVal("post_event", AesopEngine::postEvent);
	_codeResources->setVal("send_event", AesopEngine::sendEvent);
	_codeResources->setVal("peek_event", AesopEngine::peekEvent);
	_codeResources->setVal("dispatch_event", AesopEngine::dispatchEvent);
	_codeResources->setVal("flush_event_queue", AesopEngine::flushEventQueue);
	_codeResources->setVal("flush_input_events", AesopEngine::flushInputEvents);
	_codeResources->setVal("init_interface", AesopEngine::initInterface);
	_codeResources->setVal("shutdown_interface", AesopEngine::shutdownInterface);
	_codeResources->setVal("set_mouse_pointer", AesopEngine::setMousePointer);
	_codeResources->setVal("set_wait_pointer", AesopEngine::setWaitPointer);
	_codeResources->setVal("standby_cursor", AesopEngine::standbyCursor);
	_codeResources->setVal("resume_cursor", AesopEngine::resumeCursor);
	_codeResources->setVal("show_mouse", AesopEngine::showMouse);
	_codeResources->setVal("hide_mouse", AesopEngine::hideMouse);
	_codeResources->setVal("mouse_XY", AesopEngine::mouseXY);
	_codeResources->setVal("mouse_in_window", AesopEngine::mouseInWindow);
	_codeResources->setVal("lock_mouse", AesopEngine::lockMouse);
	_codeResources->setVal("unlock_mouse", AesopEngine::unlockMouse);
	_codeResources->setVal("getkey", AesopEngine::getkey);
	_codeResources->setVal("init_graphics", AesopEngine::myInitGraphics);
	_codeResources->setVal("draw_dot", AesopEngine::drawDot);
	_codeResources->setVal("draw_line", AesopEngine::drawLine);
	_codeResources->setVal("line_to", AesopEngine::lineTo);
	_codeResources->setVal("draw_rectangle", AesopEngine::drawRectangle);
	_codeResources->setVal("fill_rectangle", AesopEngine::fillRectangle);
	_codeResources->setVal("hash_rectangle", AesopEngine::hashRectangle);
	_codeResources->setVal("get_bitmap_height", AesopEngine::getBitmapHeight);
	_codeResources->setVal("draw_bitmap", AesopEngine::drawBitmap);
	_codeResources->setVal("visible_bitmap_rect", AesopEngine::visibleBitmapRect);
	_codeResources->setVal("set_palette", AesopEngine::setPalette);
	_codeResources->setVal("refresh_window", AesopEngine::refreshWindow);
	_codeResources->setVal("wipe_window", AesopEngine::wipeWindow);
	_codeResources->setVal("shutdown_graphics", AesopEngine::shutdownGraphics);
	_codeResources->setVal("wait_vertical_retrace", AesopEngine::waitVerticalRetrace);
	_codeResources->setVal("read_palette", AesopEngine::readPalette);
	_codeResources->setVal("write_palette", AesopEngine::writePalette);
	_codeResources->setVal("pixel_fade", AesopEngine::pixelFade);
	_codeResources->setVal("color_fade", AesopEngine::colorFade);
	_codeResources->setVal("light_fade", AesopEngine::lightFade);
	_codeResources->setVal("assign_window", AesopEngine::assignWindow);
	_codeResources->setVal("assign_subwindow", AesopEngine::assignSubwindow);
	_codeResources->setVal("release_window", AesopEngine::releaseWindow);
	_codeResources->setVal("get_x1", AesopEngine::getX1);
	_codeResources->setVal("get_x2", AesopEngine::getX2);
	_codeResources->setVal("get_y1", AesopEngine::getY1);
	_codeResources->setVal("get_y2", AesopEngine::getY2);
	_codeResources->setVal("set_x1", AesopEngine::setX1);
	_codeResources->setVal("set_x2", AesopEngine::setX2);
	_codeResources->setVal("set_y1", AesopEngine::setY1);
	_codeResources->setVal("set_y2", AesopEngine::setY2);
	_codeResources->setVal("text_window", AesopEngine::textWindow);
	_codeResources->setVal("text_style", AesopEngine::textStyle);
	_codeResources->setVal("text_xy", AesopEngine::textXY);
	_codeResources->setVal("text_color", AesopEngine::textColor);
	_codeResources->setVal("text_refresh_window", AesopEngine::textRefreshWindow);
	_codeResources->setVal("get_text_x", AesopEngine::getTextX);
	_codeResources->setVal("get_text_y", AesopEngine::getTextY);
	_codeResources->setVal("home", AesopEngine::home);
	_codeResources->setVal("print", AesopEngine::print);
	_codeResources->setVal("sprint", AesopEngine::sprint);
	_codeResources->setVal("dprint", AesopEngine::dprint);
	_codeResources->setVal("aprint", AesopEngine::aprint);
	_codeResources->setVal("crout", AesopEngine::crout);
	_codeResources->setVal("char_width", AesopEngine::charWidth);
	_codeResources->setVal("font_height", AesopEngine::fontHeight);
	_codeResources->setVal("solid_bar_graph", AesopEngine::solidBarGraph);
	_codeResources->setVal("init_sound", AesopEngine::initSound);
	_codeResources->setVal("shutdown_sound", AesopEngine::shutdownSound);
	_codeResources->setVal("load_sound_block", AesopEngine::loadSoundBlock);
	_codeResources->setVal("sound_effect", AesopEngine::soundEffect);
	_codeResources->setVal("play_sequence", AesopEngine::playSequence);
	_codeResources->setVal("load_music", AesopEngine::loadMusic);
	_codeResources->setVal("unload_music", AesopEngine::unloadMusic);
	_codeResources->setVal("set_sound_status", AesopEngine::setSoundStatus);
	_codeResources->setVal("create_object", AesopEngine::createObject);
	_codeResources->setVal("create_program", AesopEngine::createProgram);
	_codeResources->setVal("destroy_object", AesopEngine::destroyObject);
	_codeResources->setVal("thrash_cache", AesopEngine::thrashCache);
	_codeResources->setVal("flush_cache", AesopEngine::flushCache);
	_codeResources->setVal("step_X", AesopEngine::stepX);
	_codeResources->setVal("step_Y", AesopEngine::stepY);
	_codeResources->setVal("step_FDIR", AesopEngine::stepFDIR);
	_codeResources->setVal("step_square_X", AesopEngine::stepSquareX);
	_codeResources->setVal("step_square_Y", AesopEngine::stepSquareY);
	_codeResources->setVal("step_region", AesopEngine::stepRegion);
	_codeResources->setVal("distance", AesopEngine::distance);
	_codeResources->setVal("seek_direction", AesopEngine::seekDirection);
	_codeResources->setVal("spell_request", AesopEngine::spellRequest);
	_codeResources->setVal("spell_list", AesopEngine::spellList);
	_codeResources->setVal("magic_field", AesopEngine::magicField);
	_codeResources->setVal("do_dots", AesopEngine::doDots);
	_codeResources->setVal("do_ice", AesopEngine::doIce);
	_codeResources->setVal("read_save_directory", AesopEngine::readSaveDirectory);
	_codeResources->setVal("savegame_title", AesopEngine::savegameTitle);
	_codeResources->setVal("write_save_directory", AesopEngine::writeSaveDirectory);
	_codeResources->setVal("save_game", AesopEngine::saveGame);
	_codeResources->setVal("suspend_game", AesopEngine::suspendGame);
	_codeResources->setVal("resume_items", AesopEngine::resumeItems);
	_codeResources->setVal("resume_level", AesopEngine::resumeLevel);
	_codeResources->setVal("change_level", AesopEngine::changeLevel);
	_codeResources->setVal("restore_items", AesopEngine::restoreItems);
	_codeResources->setVal("restore_level_objects", AesopEngine::restoreLevelObjects);
	_codeResources->setVal("read_initial_items", AesopEngine::readInitialItems);
	_codeResources->setVal("write_initial_tempfiles", AesopEngine::writeInitialTempfiles);
	_codeResources->setVal("create_initial_binary_files", AesopEngine::createInitialBinaryFiles);
	_codeResources->setVal("launch", AesopEngine::launch);
	_codeResources->setVal("open_transfer_file", AesopEngine::openTransferFile);
	_codeResources->setVal("close_transfer_file", AesopEngine::closeTransferFile);
	_codeResources->setVal("player_attrib", AesopEngine::playerAttrib);
	_codeResources->setVal("item_attrib", AesopEngine::itemAttrib);
	_codeResources->setVal("arrow_count", AesopEngine::arrowCount);

	_codeResources->setVal("build_clipping", AesopEngine::buildClipping);
	_codeResources->setVal("cat_string", AesopEngine::catString);
	_codeResources->setVal("close_feature_file", AesopEngine::closeFeatureFile);
	_codeResources->setVal("close_file", AesopEngine::closeFile);
	_codeResources->setVal("copy_window", AesopEngine::copyWindow);
	_codeResources->setVal("create_file", AesopEngine::createFile);
	_codeResources->setVal("delete_saves", AesopEngine::deleteSaves);
	_codeResources->setVal("draw_auto_square", AesopEngine::drawAutoSquare);
	_codeResources->setVal("draw_walls", AesopEngine::drawWalls);
	_codeResources->setVal("explode_save", AesopEngine::explodeSave);
	_codeResources->setVal("find_location_for_map", AesopEngine::findLocationForMap);
	_codeResources->setVal("get_feature_record", AesopEngine::getFeatureRecord);
	_codeResources->setVal("init_viewspace", AesopEngine::initViewspace);
	_codeResources->setVal("load_level_map", AesopEngine::loadLevelMap);
	_codeResources->setVal("load_visibility", AesopEngine::loadVisibility);
	_codeResources->setVal("lock_resource", AesopEngine::lockResource);
	_codeResources->setVal("long2hex", AesopEngine::long2hex);
	_codeResources->setVal("open_feature_file", AesopEngine::openFeatureFile);
	_codeResources->setVal("open_file", AesopEngine::openFile);
	_codeResources->setVal("output_time", AesopEngine::outputTime);
	_codeResources->setVal("page_flip", AesopEngine::pageFlip);
	_codeResources->setVal("pause", AesopEngine::pause);
	_codeResources->setVal("prepare_save", AesopEngine::prepareSave);
	_codeResources->setVal("printer_on_line", AesopEngine::printerOnLine);
	_codeResources->setVal("randomize_array", AesopEngine::randomizeArray);
	_codeResources->setVal("read_array_from_file", AesopEngine::readArrayFromFile);
	_codeResources->setVal("read_number_from_file", AesopEngine::readNumberFromFile);
	_codeResources->setVal("refresh_main_text_window", AesopEngine::refreshMainTextWindow);
	_codeResources->setVal("roll_chance", AesopEngine::rollChance);
	_codeResources->setVal("save_visibility", AesopEngine::saveVisibility);
	_codeResources->setVal("seed_random", AesopEngine::seedRandom);
	_codeResources->setVal("seek_in_file", AesopEngine::seekInFile);
	_codeResources->setVal("sequence_playing", AesopEngine::sequencePlaying);
	_codeResources->setVal("text_background", AesopEngine::textBackground);
	_codeResources->setVal("touch", AesopEngine::touch);
	_codeResources->setVal("transition", AesopEngine::transition);
	_codeResources->setVal("unlock_resource", AesopEngine::unlockResource);
	_codeResources->setVal("update_file", AesopEngine::updateFile);
	_codeResources->setVal("walkheap", AesopEngine::walkheap);
	_codeResources->setVal("window_core", AesopEngine::windowCore);
	_codeResources->setVal("write_array_to_file", AesopEngine::writeArrayToFile);
	_codeResources->setVal("write_long_to_file", AesopEngine::writeLongToFile);
	_codeResources->setVal("write_mapheader_to_file", AesopEngine::writeMapheaderToFile);
	_codeResources->setVal("write_resource_to_file", AesopEngine::writeResourceToFile);
	_codeResources->setVal("xmsallocated", AesopEngine::xmsallocated);
	_codeResources->setVal("xmsfree", AesopEngine::xmsfree);
}

CodeResource ResourceManager::getCodeResource(const char *codeResourceName) {
	return _codeResources->getVal(codeResourceName);
}

} // End of namespace Aesop
