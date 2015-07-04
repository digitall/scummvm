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

#ifndef RING_VISUAL_BOX_H
#define RING_VISUAL_BOX_H

#include "ring/graphics/visual/visual.h"

namespace Ring {

class Hotspot;
class ImageHandle;
class Text;

/**
 * Visual object box
 *
 * This shows the dialog options when talking to people in the game
 */
class VisualObjectBox : public Visual {
public:
	VisualObjectBox(Id id);
	~VisualObjectBox();

	void draw() override;
	uint32 handleLeftButtonUp(const Common::Point &point) override;
	uint32 handleUpdate(const Common::Point &point) override;
	uint32 handleLeftButtonDown(Common::Point point) override;
	void alloc() override;
	void dealloc() override;

	void init(const Common::String &name, ArchiveType archiveType);
	void setParameters(Id keywordId, const Common::Point &point);
	void hide();

	// Serializable
	void saveLoadWithSerializer(Common::Serializer &s) override;

private:
	struct Keyword : public Common::Serializable {
		Id id;
		Common::String name;

		Keyword() {
			id = 0;
		}

		Keyword(Id kId, const Common::String &kName) {
			id   = kId;
			name = kName;
		}

		Keyword(const Keyword& keyword) {
			id   = keyword.id;
			name = keyword.name;
		}

		// Serializable
		void saveLoadWithSerializer(Common::Serializer &s) {
			s.syncAsSint32LE(id);
			s.syncString(name);
		}
	};

	struct DialogList : public BaseObject, public Common::Serializable {
		Common::Array<Keyword *> keywords;

		DialogList() : BaseObject(0) {}

		DialogList(Id keywordId) : BaseObject(keywordId) {}

		~DialogList();

		// Serializable
		void saveLoadWithSerializer(Common::Serializer &s) {
			s.syncAsSint32LE(_id);

			uint32 count = keywords.size();
			s.syncAsUint32LE(count);

			for (uint32 i = 0; i < count; i++) {
				if (s.isLoading())
					keywords.push_back(new Keyword());

				keywords[i]->saveLoadWithSerializer(s);
			}
		}
	};

	bool                             _isLoaded;
	ArchiveType                      _archiveType;
	Common::Array<Hotspot *>         _hotspots;
	Common::Array<Text *>            _options;
	Common::Array<Keyword *>         _keywords;
	Common::Array<Keyword *>         _subKeywords;
	AssociativeArray<DialogList *>   _dialogs;
	Text                            *_name;
	ImageHandle                     *_background;
	bool                             _isSetup;
	Id                               _keywordId;
	Common::Point                    _point;
	bool                             _isFirstTime;

	bool hasImage() { return true; }
	void reset();

	void setupOptions();
	Id loadKeyword(Id keywordId, bool addToSubTree = false);
	void addHotspot(Text *text, uint32 keywordIndex);
	void loadSubKeyword(Id keywordId);
	void playDialog(uint32 keywordIndex, Id keywordId);
	void updateOptions();
	void saveKeyword();
	bool isKeywordLoaded(Id keywordId);
};

} // End of namespace Ring

#endif // RING_VISUAL_BOX_H
