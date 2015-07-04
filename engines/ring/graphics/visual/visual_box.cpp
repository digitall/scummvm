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

#include "ring/graphics/visual/visual_box.h"

#include "ring/base/application.h"
#include "ring/base/text.h"

#include "ring/graphics/hotspot.h"
#include "ring/graphics/image.h"
#include "ring/graphics/screen.h"

#include "ring/ring.h"
#include "ring/helpers.h"

#include "common/tokenizer.h"

namespace Ring {

VisualObjectBox::DialogList::~DialogList() {
	CLEAR_ARRAY(Keyword, keywords);
}

VisualObjectBox::VisualObjectBox(Id id) : Visual(id) {
	_isLoaded       = false;
	_archiveType    = kArchiveInvalid;
	_name           = nullptr;
	_background     = nullptr;
	_isSetup        = false;
	_keywordId      = 0;
	_isFirstTime    = true;
}

VisualObjectBox::~VisualObjectBox() {
	CLEAR_ARRAY(Hotspot,    _hotspots);
	CLEAR_ARRAY(Text,       _options);
	CLEAR_ARRAY(Keyword,    _keywords);
	CLEAR_ARRAY(Keyword,    _subKeywords);
	CLEAR_ARRAY(DialogList, _dialogs);

	SAFE_DELETE(_name);
	SAFE_DELETE(_background);
}

void VisualObjectBox::reset() {
	CLEAR_ARRAY(Hotspot, _hotspots);
	CLEAR_ARRAY(Text,    _options);
	CLEAR_ARRAY(Keyword, _keywords);
	CLEAR_ARRAY(Keyword, _subKeywords);
}

void VisualObjectBox::draw() {
	if (!_visible || !_isSetup || !_background)
		return;

	if (_keywords.empty()) {
		getApp()->onVisualList(_id, 7, Common::Point((int16)_keywordId, 1));
		_visible = false;
		reset();
		return;
	}

	// Draw name
	if (_name)
		getApp()->getScreenManager()->drawText(_name);

	int16 y = _point.y + 4;

	// Draw background
	if (_background->isInitialized())
		getApp()->getScreenManager()->draw(_background, Common::Point(_point.x, y), _background->getDrawType());

	// Draw options
	for (Common::Array<Text *>::iterator it = _options.begin(); it != _options.end(); it++) {
		y += 13;

		if (_background->isInitialized())
			getApp()->getScreenManager()->draw(_background, Common::Point(_point.x, y), _background->getDrawType());

		getApp()->getScreenManager()->drawText(*it);
	}
}

uint32 VisualObjectBox::handleLeftButtonUp(const Common::Point &point) {
	if (_visible)
		return handleUpdate(point);

	return 0;
}

uint32 VisualObjectBox::handleUpdate(const Common::Point &point) {
	if (!_visible || _hotspots.size() == 0)
		return 0;

	for (Common::Array<Hotspot *>::iterator it = _hotspots.begin(); it != _hotspots.end(); it++) {
		Hotspot *hotspot = *it;

		if (!hotspot->contains(point))
			continue;

		switch (hotspot->getTarget()) {
		default:
			return 0;

		case 0:
		case 60:
			getApp()->cursorSelect(kCursorIdle);
			break;

		case 10:
		case 50:
			getApp()->cursorSelect(kCursorMenuActive);
			break;
		}

		return 1;
	}

	return 0;
}

uint32 VisualObjectBox::handleLeftButtonDown(Common::Point point) {
	if (!_visible || !_isSetup || _hotspots.size() == 0)
		return 0;

	for (uint32 i = 0; i < _hotspots.size(); i++) {
		Hotspot *hotspot = _hotspots[i];

		if (!hotspot->contains(point))
			continue;

		switch (hotspot->getTarget()) {
		default:
			return 0;

		case 0:
		case 60:
			break;

		case 10: {
			if (hotspot->getCursorId() >= (CursorId)_keywords.size())
				break;

			Keyword *keyword = _keywords[hotspot->getCursorId()];

			getApp()->onVisualList(_id, 6, Common::Point((int16)keyword->id, 0));
			playDialog(i, keyword->id);

			}
			break;

		case 50:
			_isSetup = false;
			_visible = false;
			reset();
			break;
		}

		return 1;
	}

	return 0;
}

void VisualObjectBox::alloc() {
	if (_isLoaded)
		return;

	if (!hasImage()) {
		_isLoaded = true;
		return;
	}

	if (_background)
		_background->loadImage();

	_isLoaded = true;
}

void VisualObjectBox::dealloc() {
	_isLoaded = false;

	if (_background)
		_background->destroy();
}

void VisualObjectBox::saveLoadWithSerializer(Common::Serializer &s) {
	s.syncAsUint32LE(_keywordId);
	s.syncAsSint16LE(_point.x);
	s.syncAsSint16LE(_point.y);
	s.syncAsByte(_isSetup);

	// Save keyword list
	uint32 count = _keywords.size();
	s.syncAsUint32LE(count);

	for (uint32 i = 0; i < count; i++) {
		if (s.isLoading())
			_keywords.push_back(new Keyword());

		_keywords[i]->saveLoadWithSerializer(s);
	}

	// Save dialog list
	count = _dialogs.size();
	s.syncAsUint32LE(count);

	for (uint32 i = 0; i < count; i++) {
		if (s.isLoading())
			_dialogs.push_back(new DialogList());

		_dialogs[i]->saveLoadWithSerializer(s);
	}
}

void VisualObjectBox::init(const Common::String &/*name*/, ArchiveType archiveType) {
	_archiveType = archiveType;

	// Compute image path
	Common::String path;
	if (_archiveType == kArchiveFile)
		path = Common::String::format("DATA/%s/VISUAL/", getApp()->getCurrentZoneFolder().c_str());
	else
		path = "/VISUAL/";

	// Create image
	_background = new ImageHandle("keywords.tga", Common::Point(0, 0), true, kDrawTypeAlpha, 1000, 0, getApp()->getCurrentZone(), kLoadFromDisk, kImageTypeBackground, _archiveType);
	_background->setDirectory(path);

	_name = new Text();
}

void VisualObjectBox::setParameters(Id keywordId, const Common::Point &point) {
	if (!_isFirstTime)
		saveKeyword();

	reset();

	_visible = true;

	if (!isKeywordLoaded(keywordId))
		loadKeyword(keywordId);

	_point = point;

	setupOptions();

	_isFirstTime = false;
	_keywordId = keywordId;
}

void VisualObjectBox::hide() {
	_isSetup = false;
	_visible = false;
}

void VisualObjectBox::setupOptions() {
	int16 y = _point.y + 10;

	_isSetup = true;

	// Setup options
	for (uint32 i = 0; i < _keywords.size(); i++) {
		Keyword *keyword = _keywords[i];

		// Initialize dialog option
		Text *text = new Text();
		text->init(keyword->name, Common::Point(_point.x + 10, y), kFontDefault, Color(255, -1, -1), Color(-1, -1, -1));

		// Compute text dimensions
		text->set(keyword->name);
		y += (int16)text->getHeight();

		_options.push_back(text);

		// Add hotspot (with cursorId == keywordIndex)
		addHotspot(text, i);
	}
}

Id VisualObjectBox::loadKeyword(Id keywordId, bool addToSubTree) {
	Id parent = 0;

	// Open a stream to the keywords file
	Common::SeekableReadStream *archive = SearchMan.createReadStreamForMember("keywords.ini");
	if (!archive)
		error("[VisualObjectBox::loadKeyword] Error opening keywords file");

	// Read each keyword info
	char prefix = ' ';
	while (!archive->eos() && !archive->err()) {

		Common::String line = archive->readLine();
		if (archive->eos() || archive->err())
			break;

		// Check keyword id
		Id keywordIdTarget = 0;
		Id keywordIdParent = 0;
		if (sscanf(line.c_str(), "%c %d %d", &prefix, &keywordIdTarget, &keywordIdParent) == 3
		 && keywordIdTarget == keywordId) {

			// Found keyword, read the options
			while (!archive->eos() && !archive->err()) {

				Common::String option = archive->readLine();
				if (archive->eos() || archive->err())
					break;

				// Empty line => end of option list
				if (option.empty())
					break;

				// Get option id and name
				Common::StringTokenizer tokenizer(option, "\t");
				if (tokenizer.empty())
					error("[VisualObjectBox::loadKeyword] Invalid option format (missing tab separator)");

				Common::String id = tokenizer.nextToken();
				if (tokenizer.empty() || id.empty())
					error("[VisualObjectBox::loadKeyword] Invalid line format (missing keyword id)");

				Keyword *keyword = new Keyword(atoi(id.c_str()), tokenizer.nextToken());
				if (addToSubTree) {
					_subKeywords.push_back(keyword);
					parent = keywordIdParent;
				} else {
					_keywords.push_back(keyword);
				}
			}
		}
	}

	delete archive;

	return parent;
}

void VisualObjectBox::addHotspot(Text *text, uint32 keywordIndex) {
	_hotspots.push_back(new Hotspot(text->getBoundingBox(), true, 0, (CursorId)keywordIndex, 10));
}

void VisualObjectBox::loadSubKeyword(Id keywordId) {
	CLEAR_ARRAY(Keyword, _subKeywords);

	Id parent = loadKeyword(keywordId, true);

	// Merge keywords
	if (parent == _keywordId) {
		for (uint32 i = 0; i < _subKeywords.size(); i++)
			_keywords.push_back(new Keyword(*_subKeywords[i]));
	}

	updateOptions();
}

void VisualObjectBox::playDialog(uint32 keywordIndex, Id keywordId) {
	// Remove keyword (it has been used)
	if (keywordIndex < _keywords.size())
		_keywords.remove_at(keywordIndex);

	// Play dialog
	if (_dialogs.empty() || !_dialogs.has(keywordId))
		getApp()->soundPlay(keywordId);

	loadSubKeyword(keywordId);
	updateOptions();
}

void VisualObjectBox::updateOptions() {
	CLEAR_ARRAY(Hotspot, _hotspots);
	CLEAR_ARRAY(Text,    _options);

	setupOptions();
}

void VisualObjectBox::saveKeyword() {
	if (_dialogs.has(_keywordId))
		_dialogs.remove(_keywordId);

	DialogList *dialog = new DialogList(_keywordId);

	for (uint32 i = 0; i < _keywords.size(); i++)
		dialog->keywords.push_back(new Keyword(*_keywords[i]));

	_dialogs.push_back(dialog);
}

bool VisualObjectBox::isKeywordLoaded(Id keywordId) {
	if (_dialogs.empty() || !_dialogs.has(keywordId))
		return false;

	DialogList *dialog = _dialogs.get(keywordId);
	if (dialog->keywords.empty())
		return true;

	// Repopulate keywords list
	for (uint32 i = 0; i < dialog->keywords.size(); i++)
		_keywords.push_back(new Keyword(*dialog->keywords[i]));

	return true;
}


} // End of namespace Ring
