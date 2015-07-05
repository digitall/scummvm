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

#include "ring/graphics/visual/visual_encyclopedia.h"

#include "ring/base/application.h"
#include "ring/base/text.h"

#include "ring/graphics/hotspot.h"
#include "ring/graphics/image.h"
#include "ring/graphics/movies/movie.h"
#include "ring/graphics/screen.h"

#include "ring/helpers.h"
#include "ring/ring.h"

namespace Ring {

#pragma region EncyclopediaEntry

EncyclopediaEntry::EncyclopediaEntry() {
	_target      = 0;
	_field_C     = 0;
	_field_10    = 0;
	_facetype    = kFaceTypeArial;
	_foregroundColor    = 0;
	_backgroundColor    = 0;
	_height      = 0;
	_smallWeight = false;
	_italic      = false;
	_field_2C    = 0;
	_padding     = 0;
}

#define READ_VALUE(name, type) { \
	const char *val = &buffer[offset]; \
	offset += strlen(val) + 1; \
	name = (type)atoi(val); \
}

#define READ_STRING(name) { \
	const char *val = &buffer[offset]; \
	offset += strlen(val) + 1; \
	name = Common::String(val); \
}

uint32 EncyclopediaEntry::load(const char *buffer, uint32 size) {
	uint32 offset = 0;

	READ_VALUE(_target, Id);
	READ_STRING(_label);
	READ_STRING(_filename);
	READ_VALUE(_field_C, uint32);
	READ_VALUE(_field_10, uint32);
	READ_VALUE(_facetype, Facetype);
	READ_VALUE(_foregroundColor, int32);
	READ_VALUE(_backgroundColor, int32);
	READ_VALUE(_height, uint32);
	READ_VALUE(_smallWeight, bool);
	READ_VALUE(_italic, bool);
	READ_VALUE(_field_2C, uint32);
	READ_VALUE(_padding, uint32);
	READ_VALUE(_point.x, int16);
	READ_VALUE(_point.y, int16);

	return offset;
}

Color EncyclopediaEntry::getForegroundColor() const {
	if (_foregroundColor == -1)
		return Color(-1, -1, -1);

	if (_foregroundColor == 0) {
		if (_target > 0)
			return Color(255, -7, -48);
		else
			return Color(255, -29, 32);
	}

	return Color(_foregroundColor & 0xFF,
	             ((uint32)_foregroundColor >> 8) & 0xFF,
	             ((uint32)_foregroundColor >> 16) & 0xFF);
}

Color EncyclopediaEntry::getBackgroundColor() const {
	if (_backgroundColor == -1 || _backgroundColor == 0)
		return Color(-1, -1, -1);

	return Color(_backgroundColor & 0xFF,
	             ((uint32)_backgroundColor >> 8) & 0xFF,
	             ((uint32)_backgroundColor >> 16) & 0xFF);
}

#pragma endregion

#pragma region VisualObjectEncyclopedia

VisualObjectEncyclopedia::VisualObjectEncyclopedia(Id id) : Visual(id) {
	_isLoaded           = false;
	_field_17           = 0;
	_imageArrowDown_dis = nullptr;
	_imageArrowDown_hlt = nullptr;
	_imageArrowDown_nor = nullptr;
	_imageArrowUp_dis   = nullptr;
	_imageArrowUp_hlt   = nullptr;
	_imageArrowUp_nor   = nullptr;
	_image7             = nullptr;
	_imageSlide         = nullptr;
	_imageSlider        = nullptr;
	_imageArrowUpType   = 0;
	_imageArrowDownType = 0;
	_field_47           = 0;
	_field_4B           = 0;
	_field_4F           = 0;
	_clippingCenter     = Common::Point(455, 235);
	_sliderCoordinates  = Common::Point(613, 49);
	_field_60           = 0;
	_field_64           = 0;
	_textHeight         = 0;
	_textTop            = 0;
	_field_70           = 0;
	_field_74           = 0;
	_field_78           = 0;
	_field_7C           = 0;
	_field_80           = 215;
	_field_84           = 0;
	_tickCount           = 0;
	_field_94           = 0;
	_movie              = nullptr;
	_frameCount         = 0;
	_field_A0           = 0;
	_field_A4           = 0;
	_text               = nullptr;
	_clippingRect       = Common::Rect(320, 20, 590, 450);
	_point              = Common::Point(10, 30);
	_field_CC           = true;
	memset(_fonts, 0, sizeof(_fonts));
	_field_541          = 0;
	_field_545          = 0;
	_field_549          = 0;
	_soundId            = 0;
	_field_551          = 0;
	_archiveType        = kArchiveInvalid;
}

VisualObjectEncyclopedia::~VisualObjectEncyclopedia() {
	CLEAR_ARRAY(Hotspot,           _hotspots);
	CLEAR_ARRAY(Text,              _texts);
	CLEAR_ARRAY(EncyclopediaEntry, _entries);

	SAFE_DELETE(_movie);

	SAFE_DELETE(_imageArrowDown_dis);
	SAFE_DELETE(_imageArrowDown_hlt);
	SAFE_DELETE(_imageArrowDown_nor);
	SAFE_DELETE(_imageArrowUp_dis);
	SAFE_DELETE(_imageArrowUp_hlt);
	SAFE_DELETE(_imageArrowUp_nor);
	SAFE_DELETE(_image7);
	SAFE_DELETE(_imageSlide);
	SAFE_DELETE(_imageSlider);

	SAFE_DELETE(_text);
}

void VisualObjectEncyclopedia::draw() {
	if (!_movie)
		error("[VisualObjectEncyclopedia::draw] Movie not initialized properly");

	if (!_visible)
		return;

	if (!_field_4F) {
		// Show arrow up image
		switch (_imageArrowUpType) {
		default:
			break;

		case 0:
			if (_imageArrowUp_dis && _imageArrowUp_dis->isInitialized())
				getApp()->getScreenManager()->draw(_imageArrowUp_dis, Common::Point(606, 20), _imageArrowUp_dis->getDrawType());
			break;

		case 1:
			if (_imageArrowUp_nor && _imageArrowUp_nor->isInitialized())
				getApp()->getScreenManager()->draw(_imageArrowUp_nor, Common::Point(606, 20), _imageArrowUp_nor->getDrawType());
			break;

		case 2:
			if (_imageArrowUp_hlt && _imageArrowUp_hlt->isInitialized())
				getApp()->getScreenManager()->draw(_imageArrowUp_hlt, Common::Point(606, 20), _imageArrowUp_hlt->getDrawType());
			break;
		}

		// Show arrow down image
		switch (_imageArrowDownType) {
		default:
			break;

		case 0:
			if (_imageArrowDown_dis && _imageArrowDown_dis->isInitialized())
				getApp()->getScreenManager()->draw(_imageArrowDown_dis, Common::Point(606, 432), _imageArrowDown_dis->getDrawType());
			break;

		case 1:
			if (_imageArrowDown_nor && _imageArrowDown_nor->isInitialized())
				getApp()->getScreenManager()->draw(_imageArrowDown_nor, Common::Point(606, 432), _imageArrowDown_nor->getDrawType());
			break;

		case 2:
			if (_imageArrowDown_hlt && _imageArrowDown_hlt->isInitialized())
				getApp()->getScreenManager()->draw(_imageArrowDown_hlt, Common::Point(606, 432), _imageArrowDown_hlt->getDrawType());
			break;
		}

		// Show sliders
		if (_imageSlide && _imageSlide->isInitialized())
			getApp()->getScreenManager()->draw(_imageSlide, Common::Point(610, 45), _imageSlide->getDrawType());

		if (_imageSlider && _imageSlider->isInitialized())
			getApp()->getScreenManager()->draw(_imageSlider, _sliderCoordinates, _imageSlider->getDrawType());

		// Show text
		Common::Rect exclude1(_clippingRect.left - 10, 0,                    639, _clippingRect.top);
		Common::Rect exclude2(_clippingRect.left - 10, _clippingRect.bottom, 639, 480);

		for (Common::Array<Text *>::iterator it = _texts.begin(); it != _texts.end(); it++)
			getApp()->getScreenManager()->drawText(*it, exclude1, exclude2);

		--_field_60;
		if (_field_60 < 0)
			_field_60 = 0;

		if (_field_60 > 0 && _text)
			getApp()->getScreenManager()->drawText(_text, exclude1, exclude2);
	}

	if (_field_64 && _image7 && _image7->isInitialized()) {
		// Adjust coordinates
		_point.x = _clippingCenter.x - (int16)(_image7->getWidth()  / 2);
		_point.y = _clippingCenter.y - (int16)(_image7->getHeight() / 2);

		getApp()->getScreenManager()->draw(_image7, _point, _image7->getDrawType());
	}

	// Cleanup after movie is done playing
	if (_field_A4 && _movie->playNextFrame(_point, kDrawTypeAlpha) == _frameCount)
		SAFE_DELETE(_movie);
}

uint32 VisualObjectEncyclopedia::handleLeftButtonUp(const Common::Point &point) {
	if (!_visible)
		return false;

	uint32 tickDelta = g_system->getMillis() - _tickCount;

	if (_imageArrowUpType || _imageArrowDownType) {
		if (tickDelta && tickDelta < 400) {
			if (_imageArrowUpType > 0)
				scrollUp(50);
			else
				scrollDown(50);
		}
	}

	_field_47 = 0;
	_field_4B = 0;

	getApp()->onVisualList(_id, 1, point);

	_field_94 = 0;

	return 0;
}

uint32 VisualObjectEncyclopedia::handleUpdate(const Common::Point &point) {
	if (!_visible)
		return false;

	if (_field_47 == 1) {
		sub_484040(point);

		getApp()->onVisualList(_id, 1, point);

		return 0;
	}

	_imageArrowUpType = 0;
	_imageArrowDownType = 0;

	for (uint32 i = 0; i < _hotspots.size(); i++) {
		if (_hotspots[i]->contains(point)) {
			Hotspot *hotspot = _hotspots[i];

			switch (hotspot->getTarget()) {
			default:
				break;

			case kTargetScrollUp:
				if (i == 0)
					_imageArrowUpType = 1;
				break;

			case kTargetScrollDown:
				if (i == 1)
					_imageArrowDownType = 1;
				break;
			}

			break;
		}
	}

	getApp()->onVisualList(_id, 1, point);

	return 0;
}

uint32 VisualObjectEncyclopedia::handleLeftButtonDown(Common::Point point) {
	if (!_visible)
		return false;

	if (_field_4F) {
		_field_4F = 0;

		return 0;
	}

	_tickCount = g_system->getMillis();
	_field_47 = 1;
	_field_94 = 1;

	if (_field_4F || _hotspots.empty())
		return 0;

	for (uint32 i = 0; i < _hotspots.size(); i++) {
		Hotspot *hotspot = _hotspots[i];

		if (hotspot->contains(point)) {
			switch (hotspot->getTarget()) {
			default:
				break;

			case kTargetScrollUp:
				if (i == 0) {
					_imageArrowUpType = 2;

					if (!_field_94)
						scrollUp(10);
				}
				break;

			case kTargetScrollDown:
				if (i == 1) {
					_imageArrowDownType = 2;

					if (!_field_94)
						scrollDown(10);
				}
				break;

			case kTarget2:
				if (i != 2)
					break;

				_field_60 = 20;
				_field_4B = 1;
				break;

			case kTarget3:
				_field_4F = 1;
				break;

			case kTargetLoadImage:
				loadImage(hotspot->getCursorId());
				_field_60 = 20;
				break;

			case kTarget4:
			case kTargetPlayMovie:
				playMovie(hotspot->getCursorId());
				_field_60 = 20;
				break;

			case kTarget5:
			case kTargetStopMovie:
				stopMovie((uint32)hotspot->getSoundId());
				_field_60 = 20;
				break;

			case kTarget9:
				handleTarget9(hotspot->getCursorId());
				_field_60 = 20;
				break;

			case kTargetLoadPage:
				loadPage(hotspot->getCursorId());
				_field_60 = 20;
				break;
			}

			break;
		}
	}

	return 0;
}

bool VisualObjectEncyclopedia::handleKey(Common::KeyCode key) {
	switch (key) {
	default:
		break;

	case Common::KEYCODE_PAGEUP:
		scrollUp(50);
		setHotspot();
		break;

	case Common::KEYCODE_PAGEDOWN:
		scrollDown(50);
		setHotspot();
		break;
	}

	if (_field_4F)
		_field_4F = 0;

	return false;
}

void VisualObjectEncyclopedia::alloc() {
	if (_isLoaded)
		return;

	if (!load()) {
		_isLoaded = true;
		return;
	}

	if (_imageArrowDown_dis)
		_imageArrowDown_dis->loadImage();

	if (_imageArrowDown_hlt)
		_imageArrowDown_hlt->loadImage();

	if (_imageArrowDown_nor)
		_imageArrowDown_nor->loadImage();

	if (_imageArrowUp_dis)
		_imageArrowUp_dis->loadImage();

	if (_imageArrowUp_hlt)
		_imageArrowUp_hlt->loadImage();

	if (_imageArrowUp_nor)
		_imageArrowUp_nor->loadImage();

	if (_imageSlide)
		_imageSlide->loadImage();

	if (_imageSlider)
		_imageSlider->loadImage();

	_isLoaded = true;
}

void VisualObjectEncyclopedia::dealloc() {
	_isLoaded  = false;
	_field_A4 = 0;

	SAFE_DELETE(_movie);

	CLEAR_ARRAY(EncyclopediaEntry, _entries);

	if (_imageArrowDown_dis)
		_imageArrowDown_dis->destroy();

	if (_imageArrowDown_hlt)
		_imageArrowDown_hlt->destroy();

	if (_imageArrowDown_nor)
		_imageArrowDown_nor->destroy();

	if (_imageArrowUp_dis)
		_imageArrowUp_dis->destroy();

	if (_imageArrowUp_hlt)
		_imageArrowUp_hlt->destroy();

	if (_imageArrowUp_nor)
		_imageArrowUp_nor->destroy();

	if (_image7)
		_image7->destroy();

	if (_imageSlide)
		_imageSlide->destroy();

	if (_imageSlider)
		_imageSlider->destroy();
}

void VisualObjectEncyclopedia::init(const Common::String &name, ArchiveType archiveType) {
	_archiveType = archiveType;

	// Compute image folder
	Common::String path;
	if (_archiveType == kArchiveFile)
		path = Common::String::format("DATA/%s/VISUAL/", getApp()->getCurrentZoneFolder().c_str());
	else
		path = "/VISUAL/";

	// Prepare hotspots
	addHotspots();

	// Create images
	_imageArrowDown_dis = new ImageHandle("ENC_AR-2.BMP", Common::Point(0, 0), true, kDrawTypeNormal, 1000, 0, getApp()->getCurrentZone(), kLoadFromDisk, kImageTypeBackground, _archiveType);
	_imageArrowDown_dis->setDirectory(path);
	_imageArrowDown_hlt = new ImageHandle("ENC_AR-2.BMP", Common::Point(0, 0), true, kDrawTypeNormal, 1000, 0, getApp()->getCurrentZone(), kLoadFromDisk, kImageTypeBackground, _archiveType);
	_imageArrowDown_hlt->setDirectory(path);
	_imageArrowDown_nor = new ImageHandle("ENC_AR-1.BMP", Common::Point(0, 0), true, kDrawTypeNormal, 1000, 0, getApp()->getCurrentZone(), kLoadFromDisk, kImageTypeBackground, _archiveType);
	_imageArrowDown_nor->setDirectory(path);
	_imageArrowUp_dis = new ImageHandle("ENC_AR-1.BMP", Common::Point(0, 0), true, kDrawTypeNormal, 1000, 0, getApp()->getCurrentZone(), kLoadFromDisk, kImageTypeBackground, _archiveType);
	_imageArrowUp_dis->setDirectory(path);
	_imageArrowUp_hlt = new ImageHandle("ENC_AR-3.BMP", Common::Point(0, 0), true, kDrawTypeNormal, 1000, 0, getApp()->getCurrentZone(), kLoadFromDisk, kImageTypeBackground, _archiveType);
	_imageArrowUp_hlt->setDirectory(path);
	_imageArrowUp_nor = new ImageHandle("ENC_AR-4.BMP", Common::Point(0, 0), true, kDrawTypeNormal, 1000, 0, getApp()->getCurrentZone(), kLoadFromDisk, kImageTypeBackground, _archiveType);
	_imageArrowUp_nor->setDirectory(path);

	_imageSlide = new ImageHandle("ENC_SC-1.BMP", Common::Point(0, 0), true, kDrawTypeNormal, 1000, 0, getApp()->getCurrentZone(), kLoadFromDisk, kImageTypeBackground, _archiveType);
	_imageSlide->setDirectory(path);
	_imageSlider = new ImageHandle("ENC_SC-1.TGA", Common::Point(0, 0), true, kDrawTypeNormal, 1000, 0, getApp()->getCurrentZone(), kLoadFromDisk, kImageTypeBackground, _archiveType);
	_imageSlider->setDirectory(path);

	_text = new Text();

	_filename = name;
}

void VisualObjectEncyclopedia::setParameters(const Common::Point &point, const Common::Rect &clippingRect, bool a8) {
	_point = point;
	_clippingRect = clippingRect;
	_field_CC = a8;

	_field_80         = (uint16)clippingRect.height() / 2;
	_clippingCenter.x = clippingRect.left   + (int16)(abs((int)clippingRect.width())  / 2);
	_clippingCenter.y = clippingRect.bottom + (int16)(abs((int)clippingRect.height()) / 2);
}

void VisualObjectEncyclopedia::showFile(const Common::String &filename) {
	_filename = filename;

	if (!_isLoaded)
		return;

	// Cleanup existing data
	CLEAR_ARRAY(Hotspot,           _hotspots);
	CLEAR_ARRAY(Text,              _texts);
	CLEAR_ARRAY(EncyclopediaEntry, _entries);

	SAFE_DELETE(_movie);

	addHotspots();
	load();
}

void VisualObjectEncyclopedia::saveLoadWithSerializer(Common::Serializer &s) {
	s.syncString(_filename);
}

bool VisualObjectEncyclopedia::load() {
	// Compute full path
	Common::String path = "DATA/ENC/" + _filename;

	if (!loadEntries(path)) {
		warning("[VisualObjectEncyclopedia::load] Cannot load encyclopedia entries");
		return false;
	}

	// Prepare values
	uint32 offset = 0;
	uint32 textHeight = 12;
	int16 clippingLeft = _clippingRect.left;
	uint32 clippingTop = (uint16)_clippingRect.top;

	// Process entries
	for (uint32 i = 0; i < _entries.size(); i++) {
		EncyclopediaEntry *entry = _entries[i];

		// Padding
		Common::String str;
		for (uint32 j = 0; j < entry->getPadding(); j++)
			str += " ";

		// Label
		str += entry->getLabel();

		// Adjust coordinates
		Common::Point point(clippingLeft, (int16)clippingTop);
		if (entry->getField2C() > offset) {
			clippingTop += (uint16)(textHeight + ((entry->getField2C() <= offset + 1) ? 0 : 12 * (entry->getField2C() - offset)));
			textHeight = 0;
			offset = entry->getField2C();
			point.y = (int16)clippingTop;
		}

		// Init contents
		Text *text = new Text();
		text->init(str, point, kFontDefault, Color(255, -1, -1), Color(-1, -1, -1));
		text->set(str);

		// Adjust colors
		text->setForegroundColor(entry->getForegroundColor());
		text->setBackgroundColor(entry->getBackgroundColor());

		// Set font
		FontId fontId = getFontId(entry->getFaceType(), entry->getHeight(), entry->getSmallWeight(), entry->getItalic(), entry->getTarget());
		text->setFontId(fontId);
		text->set(str);

		// Compute text dimensions
		if (i == 0) {
			_textTop  = (uint16)point.y;
			_field_7C = (uint16)(point.y - _clippingRect.top);
		}

		if (text->getHeight() > textHeight)
			textHeight = text->getHeight();

		entry->setPoint(point);

		// Adjust text width
		uint32 spaceWidth = 0;
		if (entry->getTarget()) {
			Common::Point coords(clippingLeft, point.y);

			Text *space = new Text();
			space->init(" ", coords, fontId, Color(255, -1, -1), Color(-1, -1, -1));
			spaceWidth = space->getWidth();
			delete space;

			coords.x += (int16)spaceWidth;
			text->setCoordinates(coords);
		}

		if ((int16)text->getWidth() + clippingLeft > _clippingRect.right) {
			clippingLeft = _clippingRect.left;

			Common::Point coords(clippingLeft, (int16)textHeight + point.y);
			text->setCoordinates(coords);

			textHeight = 12;
			clippingTop = (uint16)coords.y + ((entry->getField2C() <= offset + 1) ? 0 : 12 * (entry->getField2C() - offset));
			offset = entry->getField2C();
		}

		// Adjust clipping
		clippingLeft += (int16)(text->getWidth() + spaceWidth);

		_texts.push_back(text);

		// Update hotspot
		if (entry->getTarget())
			addHotspot(entry->getTarget(), text, i);
	}

	_textHeight = textHeight;
	_field_74 = textHeight + clippingTop;
	_field_78 = (uint16)_clippingRect.height();

	return true;
}

bool VisualObjectEncyclopedia::loadEntries(const Common::String &filename) {
	// Open a stream to the OUT file
	Common::SeekableReadStream *archive = SearchMan.createReadStreamForMember(filename);
	if (!archive)
		error("[VisualObjectEncyclopedia::loadEntries] Error opening encyclopedia file (%s)", filename.c_str());

	// Clear the existing entries
	CLEAR_ARRAY(EncyclopediaEntry, _entries);

	// Read the archive contents into a buffer
	uint32 size = (uint32)archive->size();
	if (size <= 1) {
		delete archive;
		return true;
	}

	byte *buffer = (byte *)malloc(size);
	if (!buffer)
		error("[VisualObjectEncyclopedia::loadEntries] Cannot allocate memory for encyclopedia buffer (%d)", size);

	archive->read(buffer, size);
	delete archive;

	// Load entries
	byte *pBuffer = buffer;
	do {
		EncyclopediaEntry *entry = new EncyclopediaEntry();

		uint32 offset = entry->load((const char *)pBuffer, size);
		pBuffer += offset;
		size    -= offset;

		_entries.push_back(entry);

	} while (size > 0);

	free(buffer);

	return true;
}

void VisualObjectEncyclopedia::scrollUp(int16 offset) {
	if (_texts.empty())
		return;

	Text *text = _texts[0];
	if (text->getCoordinates().y >= (int16)_textTop)
		return;

	int16 maxOffset = (int16)abs((int32)_textTop - text->getCoordinates().y);
	if (offset > maxOffset)
		offset = maxOffset;

	// Adjust coordinates of all texts and hotspots
	for (Common::Array<Text *>::iterator it = _texts.begin(); it != _texts.end(); it++) {
		Common::Point point = (*it)->getCoordinates();
		point.y += offset;

		(*it)->setCoordinates(point);
	}

	for (Common::Array<Hotspot *>::iterator it = _hotspots.begin() + 3; it != _hotspots.end(); it++) {
		Common::Rect rect = (*it)->getRect();
		rect.top    += offset;
		rect.bottom += offset;

		(*it)->update(rect);
	}

	setHotspot();

	if (_field_CC)
		sub_487580();
}

void VisualObjectEncyclopedia::scrollDown(int16 offset) {
	if (_texts.empty())
		return;

	if (_field_78 <= _field_74)
		return;

	Text *text = _texts[0];
	if (text->getCoordinates().y <= _clippingRect.bottom)
		return;

	int16 maxOffset = (int16)abs((int)(text->getCoordinates().y - _clippingRect.bottom));
	if (offset > maxOffset)
		offset = maxOffset;

	// Adjust coordinates of all texts and hotspots
	for (Common::Array<Text *>::iterator it = _texts.begin(); it != _texts.end(); it++) {
		Common::Point point = (*it)->getCoordinates();
		point.y -= offset;

		(*it)->setCoordinates(point);
	}

	for (Common::Array<Hotspot *>::iterator it = _hotspots.begin() + 3; it != _hotspots.end(); it++) {
		Common::Rect rect = (*it)->getRect();
		rect.top    -= offset;
		rect.bottom -= offset;

		(*it)->update(rect);
	}

	setHotspot();

	if (_field_CC)
		sub_487580();
}

void VisualObjectEncyclopedia::addHotspots() {
	_hotspots.push_back(new Hotspot(Common::Rect(606, 20, 626, 45), true, 0, kCursorInvalid, 0));
	_hotspots.push_back(new Hotspot(Common::Rect(606, 432, 626, 457), true, 0, kCursorInvalid, 1));
	_hotspots.push_back(new Hotspot(Common::Rect(606, 49, 626, 57), true, 0, kCursorInvalid, 2));
}

void VisualObjectEncyclopedia::addHotspot(Id target, Text *text, uint32 entryIndex) {
	Id soundId = 0;
	if (target == kTarget4 || target == kTargetPlayMovie)
		soundId = addSound(entryIndex);

	_hotspots.push_back(new Hotspot(text->getBoundingBox(), true, soundId, (CursorId)entryIndex, target + 1));
}

void VisualObjectEncyclopedia::setHotspot() {
	if (_texts.empty())
		return;

	Text *text = _texts[0];

	float y = (float)(text->getCoordinates().y - ((int32)_field_7C + _clippingRect.top));
	if (y >= 0) {
		_sliderCoordinates.x = 49;
	} else {
		_sliderCoordinates.x = (int16)(((float)text->getHeight() + ABS(y)) / ((float)((int32)_field_74 - _clippingRect.bottom) * 386.0f + 49.0f));
		if (_sliderCoordinates.x > 421)
			_sliderCoordinates.x = 421;

		if (_sliderCoordinates.x < 49)
			_sliderCoordinates.x = 49;
	}

	if (_hotspots.size() <= 2)
		return;

	_hotspots[2]->update(Common::Rect(_hotspots[2]->getRect().left, _sliderCoordinates.x, _hotspots[2]->getRect().right, _sliderCoordinates.x + 10));
}

Id VisualObjectEncyclopedia::addSound(uint32 entryIndex) {
	++_soundId;

	getApp()->soundAdd(_soundId + 10000000, kSoundTypeAmbientMusic, _entries[entryIndex]->getFilename(), kLoadFromCd);

	return _soundId;
}

void VisualObjectEncyclopedia::playMovie(uint32 entryIndex) {
	if (entryIndex >= _entries.size())
		error("[VisualObjectEncyclopedia::playMovie] Invalid entry index (was: %d, valid:[0-%d])", entryIndex, _entries.size() - 1);

	// Get movie folder
	Common::String folder;
	if (_archiveType == kArchiveFile)
		folder = Common::String::format("DATA/%s/PLA/", getApp()->getCurrentZoneFolder().c_str());
	else
		folder = "/PLA/";

	SAFE_DELETE(_movie);

	// Initialize movie
	_movie = new Movie(getApp()->getScreenManager());

	if (!_movie->init(folder, _entries[entryIndex]->getFilename(), getApp()->getLanguageFolder(), 0))
		error("[VisualObjectEncyclopedia::playMovie] Cannot initialize movie (%s%s)", folder.c_str(), _entries[entryIndex]->getFilename().c_str());

	_frameCount = _movie->getNumberOfFrames();
	_movie->setSynchroOff();
	_field_A0 = 0;
	_field_A4 = 1;
	_field_64 = 0;
}

void VisualObjectEncyclopedia::stopMovie(uint32 soundIndex) {
	SAFE_DELETE(_movie);

	getApp()->soundStopType(kSoundTypeAmbientMusic, 1024);
	getApp()->soundPlay(soundIndex + 1000000);
}

FontId VisualObjectEncyclopedia::getFontId(Facetype faceType, uint32 height, bool smallWeight, bool italic, Id target) {
	// Height
	uint32 height_id = 0;
	switch (height) {
	default:
		height_id = 5;
		break;

	case 6:
		height_id = 1;
		break;

	case 8:
		height_id = 2;
		break;

	case 10:
		height_id = 3;
		break;

	case 11:
		height_id = 4;
		break;

	case 14:
		height_id = 6;
		break;

	case 16:
		height_id = 7;
		break;

	case 18:
		height_id = 8;
		break;

	case 24:
		height_id = 9;
		break;

	case 32:
		height_id = 10;
		break;

	case 64:
		height_id = 11;
		break;

	case 72:
		height_id = 12;
		break;
	}

	if (height == 0)
		height = 12;

	// Facetype
	Common::String facename;
	switch(faceType) {
	default:
		facename = "Arial";
		break;

	case kFaceTypeCourier:
		facename = "Courier";
		break;

	case kFaceTypeImpact:
		facename = "Impact";
		break;
	}

	// Compute font id
	uint32 idPart = 2 * (italic + 2 * smallWeight);
	if (target)
		idPart++;

	uint32 id = 96 * faceType + height_id * (idPart + 1);
	FontId fontId = (FontId)(id + 100);

	if (_fonts[id] == true)
		return fontId;

	getApp()->fontAdd(fontId, facename, height, smallWeight, target != 0, italic, false);

	_fonts[id] = true;

	return fontId;
}

void VisualObjectEncyclopedia::sub_484040(const Common::Point &point) {
	setHotspot();

	uint32 tickDelta = g_system->getMillis() - _tickCount;
	if ((_imageArrowUpType || _imageArrowDownType) && tickDelta > 400)
		_field_94 = 0;

	if (_hotspots.empty())
		return;

	// Process hotspots
	for (uint32 i = 0; i < _hotspots.size(); i++) {
		Hotspot *hotspot = _hotspots[i];

		if (hotspot->contains(point)) {
			switch (hotspot->getTarget()) {
			default:
				break;

			case kTargetScrollUp:
				if (i == 0) {
					_imageArrowUpType = 2;

					if (!_field_94)
						scrollUp(10);
				}
				break;

			case kTargetScrollDown:
				if (i == 1) {
					_imageArrowDownType = 2;

					if (!_field_94)
						scrollDown(10);
				}
				break;
			}

			// Stop processing hotspots
			break;
		}
	}

	if (!_field_4B || _hotspots.size() <= 2)
		return;

	// Check coordinates and scroll up/down
	Hotspot *hotspot2 = _hotspots[2];

	if (point.y <= hotspot2->getRect().top) {
		int32 diff = hotspot2->getRect().top - point.y;

		if (diff <= 20)
			scrollUp(20);
		else if (diff < 30)
			scrollUp(40);
		else if (diff < 40)
			scrollUp(60);
		else if (diff < 60)
			scrollUp(80);
		else if (diff < 1000)
			scrollUp(120);
		else if (diff < 2000)
			scrollUp(220);

	} else {
		int32 diff = point.y - hotspot2->getRect().top;

		if (diff <= 20)
			scrollDown(20);
		else if (diff < 30)
			scrollDown(40);
		else if (diff < 40)
			scrollDown(60);
		else if (diff < 60)
			scrollDown(80);
		else if (diff < 1000)
			scrollDown(120);
		else if (diff < 2000)
			scrollDown(220);
	}
}

void VisualObjectEncyclopedia::loadImage(uint32 entryIndex) {
	if (entryIndex >= _entries.size())
		error("[VisualObjectEncyclopedia::loadImage7] Invalid entry index (was: %d, valid:[0-%d])", entryIndex, _entries.size() - 1);

	// Stop movie and cleanup previous image
	SAFE_DELETE(_movie);
	SAFE_DELETE(_image7);

	// Compute path
	Common::String path;
	if (_archiveType == kArchiveFile)
		path = Common::String::format("DATA/%s/IMAGE/", getApp()->getCurrentZoneFolder().c_str());
	else
		path = "/IMAGE/";

	_image7 = new ImageHandle(_entries[entryIndex]->getFilename(), Common::Point(10, 10), true, kDrawTypeNormal, 1000, 0, getApp()->getCurrentGameZone(), kLoadFromDisk, kImageTypeBackground, _archiveType);
	_image7->setDirectory(path);

	if (!_image7->getNameId().empty())
		_image7->loadImage();

	_field_64 = 1;
}

void VisualObjectEncyclopedia::handleTarget9(uint32 entryIndex) {
	if (entryIndex >= _entries.size())
		error("[VisualObjectEncyclopedia::handleTarget9] Invalid entry index (was: %d, valid:[0-%d])", entryIndex, _entries.size() - 1);

	uint32 field_C = _entries[entryIndex]->getFieldC();

	for (uint32 i = 0; i < _entries.size(); i++) {
		if (_entries[i]->getField10() == field_C) {

			int16 offset = _texts[i]->getCoordinates().y - _clippingRect.bottom;

			if (offset > 0)
				scrollDown(offset);
			else
				scrollUp(offset);

			break;
		}
	}
}

void VisualObjectEncyclopedia::loadPage(uint32 entryIndex) {
	if (entryIndex >= _entries.size())
		error("[VisualObjectEncyclopedia::loadPage] Invalid entry index (was: %d, valid:[0-%d])", entryIndex, _entries.size() - 1);

	_filename = _entries[entryIndex]->getFilename();

	// Cleanup
	CLEAR_ARRAY(EncyclopediaEntry, _entries);
	CLEAR_ARRAY(Hotspot,           _hotspots);
	CLEAR_ARRAY(Text,              _texts);
	SAFE_DELETE(_movie);
	_field_64 = 0;

	// Reload new page
	addHotspots();
	load();
}

void VisualObjectEncyclopedia::sub_487580() {
	if (_hotspots.size() <= 3)
		return;

	uint32 delta = 1410065407;
	uint32 index = 0;
	EncyclopediaTarget target = kTargetScrollUp;

	// Get index and target from hotspots
	for (uint32 i = 3; i < _hotspots.size(); i++) {
		Hotspot *hotspot = _hotspots[i];

		if (hotspot->getTarget() == kTargetLoadImage || hotspot->getTarget() == kTargetPlayMovie || hotspot->getTarget() == kTargetStopMovie) {
			uint32 currentDelta = (uint32)abs((int)(hotspot->getRect().top - (int32)_field_80));
			if (currentDelta < delta) {
				index = i;
				delta = currentDelta;
				target = (EncyclopediaTarget)hotspot->getTarget();
			}
		}
	}

	if (_field_84 == index)
		return;

	_field_84 = index;

	Hotspot *hotspot = _hotspots[index];
	if (hotspot->getRect().top >= _clippingRect.top && hotspot->getRect().bottom <= _clippingRect.bottom) {
		switch (target) {
		default:
			break;

		case kTargetLoadImage:
			loadImage(hotspot->getCursorId());
			break;

		case kTargetPlayMovie:
			playMovie(hotspot->getCursorId());
			break;

		case kTargetStopMovie:
			stopMovie((uint32)hotspot->getSoundId());
			break;
		}
	}
}

#pragma endregion

} // End of namespace Ring
