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

#include "ring/base/font.h"

#include "ring/helpers.h"

#include "graphics/fonts/winfont.h"
#include "graphics/fontman.h"

namespace Ring {

static const struct {
	const char *name;
	const char *replacement;
} _replacementFontNames[] = {
	{ "Arial", "Arial.bdf" },
	{ "Courier", "courr12-iso-8859-1.bdf" },
	{ "Impact", "ArialBold.bdf" }
};

#pragma region Font

Font::Font(FontId id, Common::String filename, Common::String facename, uint32 height, bool smallWeight, bool underline, bool italic, bool strikeout, LanguageId langId) : BaseObject(id) {
	setup(facename, height, smallWeight, underline, italic, strikeout);

	switch (langId) {
	default:
		break;

	case kLanguageGreek:
		_description.charSet = 161;  // GREEK_CHARSET
		break;

	case kLanguageHebrew:
		_description.charSet = 2;    // SYMBOL_CHARSET
		break;
	}

	// Create font object
	_font = new Graphics::WinFont();
	if (!((Graphics::WinFont *)_font)->loadFromFON(filename, Graphics::WinFontDirEntry(_description.facename, (uint16)_description.height)))
		error("Unable to load font %s, face %s size %d", filename.c_str(), _description.facename.c_str(), _description.height);
}

Font::Font(FontId id, Common::String facename, uint32 height, bool smallWeight, bool underline, bool italic, bool strikeout) : BaseObject(id) {
	setup(facename, height, smallWeight, underline, italic, strikeout);

	// A system font has been asked, check if we have a replacement available
	Common::String fontName;
	for (uint32 i = 0; i < ARRAYSIZE(_replacementFontNames); i++) {
		if (facename == _replacementFontNames[i].name) {
			fontName = _replacementFontNames[i].replacement;
			break;
		}
	}

	// Try getting font replacement
	_font = nullptr;
	if (!fontName.empty())
		_font = const_cast<Graphics::Font *>(FontMan.getFontByName(fontName));

	if (!_font) {
		warning("[Font::Font] Font replacement for \"%s\" not available, using default GUI font", facename.c_str());

		_font = const_cast<Graphics::Font *>(FontMan.getFontByUsage(Graphics::FontManager::kGUIFont));
	}
}

Font::~Font() {
	delete _font;
}

void Font::setup(Common::String facename, uint32 height, bool smallWeight, bool underline, bool italic, bool strikeout) {
	// Setup font description
	_description.facename = facename;
	_description.height = height;
	_description.weight = smallWeight ? 400 : 700;
	_description.underline = underline;
	_description.italic = italic;
	_description.strikeOut = strikeout;
}

#pragma endregion

#pragma region FontHandler

FontHandler::FontHandler() {
}

FontHandler::~FontHandler() {
	CLEAR_ARRAY(Font, _fonts);
}

void FontHandler::add(FontId id, Common::String filename, Common::String facename, uint32 height, bool smallWeight, bool underline, bool italic, bool strikeout, LanguageId langId) {
	if (_fonts.has(id))
		error("[FontHandler::add] ID already exists (%d)", id);

	_fonts.push_back(new Font(id, filename, facename, height, smallWeight, underline, italic, strikeout, langId));
}

void FontHandler::add(FontId id, Common::String facename, uint32 height, bool smallWeight, bool underline, bool italic, bool strikeout) {
	if (_fonts.has(id))
		error("[FontHandler::add] ID already exists (%d)", id);

	_fonts.push_back(new Font(id, facename, height, smallWeight, underline, italic, strikeout));
}

Graphics::Font *FontHandler::getFont(FontId id) {
	if (!_fonts.has(id))
		error("[FontHandler::getFont] ID doesn't exists (%d)", id);

	return _fonts.get(id)->getFont();
}

#pragma endregion

} // End of namespace Ring
