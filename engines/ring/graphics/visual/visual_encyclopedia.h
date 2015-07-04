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

#ifndef RING_VISUAL_ENCYCLOPEDIA_H
#define RING_VISUAL_ENCYCLOPEDIA_H

#include "ring/graphics/visual/visual.h"

namespace Ring {

class Hotspot;
class ImageHandle;
class Movie;
class Text;

enum Facetype {
	kFaceTypeArial   = 0,
	kFaceTypeCourier = 1,
	kFaceTypeImpact  = 2
};

enum EncyclopediaTarget {
	kTargetScrollUp   = 0,
	kTargetScrollDown = 1,
	kTarget2          = 2,
	kTarget3          = 3,
	kTarget4          = 4,
	kTarget5          = 5,
	kTargetLoadImage  = 6,
	kTargetPlayMovie  = 7,
	kTargetStopMovie  = 8,
	kTarget9          = 9,
	kTargetLoadPage   = 10
};

class EncyclopediaEntry {
public:
	EncyclopediaEntry();
	~EncyclopediaEntry() {}

	uint32 load(const char *buffer, uint32 size);

	Id                   getTarget()       { return _target; }
	const Common::String getLabel()        { return _label; }
	const Common::String getFilename()     { return _filename; }
	Facetype             getFaceType()     { return _facetype; }
	uint32               getFieldC()       { return _field_C; }
	uint32               getField10()      { return _field_10; }
	Color                getForegroundColor() const;
	Color                getBackgroundColor() const;
	uint32               getHeight()        { return _height; }
	bool                 getSmallWeight()   { return _smallWeight; }
	bool                 getItalic()        { return _italic; }
	uint32               getField2C()       { return _field_2C; }
	uint32               getPadding()       { return _padding; }

	void                 setPoint(const Common::Point &point) { _point = point; }

private:
	Id             _target;
	Common::String _label;
	Common::String _filename;
	uint32         _field_C;
	uint32         _field_10;
	Facetype       _facetype;
	int32          _foregroundColor;
	int32          _backgroundColor;
	uint32         _height;
	bool           _smallWeight;
	bool           _italic;
	uint32         _field_2C;
	uint32         _padding;
	Common::Point  _point;
};

class VisualObjectEncyclopedia : public Visual {
public:
	VisualObjectEncyclopedia(Id id);
	~VisualObjectEncyclopedia();

	void draw() override;
	uint32 handleLeftButtonUp(const Common::Point &point) override;
	uint32 handleUpdate(const Common::Point &point) override;
	uint32 handleLeftButtonDown(Common::Point point) override;
	bool handleKey(Common::KeyCode key) override;
	void alloc() override;
	void dealloc() override;

	void init(const Common::String &name, ArchiveType archiveType);

	/**
	 * Sets the encyclopedia parameters.
	 *
	 * @param	point			The coordinates.
	 * @param	clippingRect	The clipping rectangle (original order is top/bottom/left/right)
	 * @param	a8				???.
	 */
	void setParameters(const Common::Point &point, const Common::Rect &clippingRect, bool a8);

	void showFile(const Common::String &filename);

	// Serializable
	void saveLoadWithSerializer(Common::Serializer &s) override;

private:
	bool                                _isLoaded;
	ArchiveType                         _archiveType;
	Common::Array<Hotspot *>            _hotspots;
	Common::Array<Text *>               _texts;
	uint32                              _field_17;
	ImageHandle                        *_imageArrowDown_dis;
	ImageHandle                        *_imageArrowDown_hlt;
	ImageHandle                        *_imageArrowDown_nor;
	ImageHandle                        *_imageArrowUp_dis;
	ImageHandle                        *_imageArrowUp_hlt;
	ImageHandle                        *_imageArrowUp_nor;
	ImageHandle                        *_image7;
	ImageHandle                        *_imageSlide;
	ImageHandle                        *_imageSlider;
	uint32                              _imageArrowUpType;
	uint32                              _imageArrowDownType;
	uint32                              _field_47;
	uint32                              _field_4B;
	uint32                              _field_4F;
	Common::Point                       _clippingCenter;
	Common::Point                       _sliderCoordinates;
	int32                               _field_60;
	uint32                              _field_64;
	uint32                              _textHeight;
	uint32                              _textTop;
	uint32                              _field_70;
	uint32                              _field_74;
	uint32                              _field_78;
	uint32                              _field_7C;
	uint32                              _field_80;
	uint32                              _field_84;
	uint32                              _tickCount;
	uint32                              _field_94;
	Movie                              *_movie;
	uint32                              _frameCount;
	uint32                              _field_A0;
	uint32                              _field_A4;
	Text                               *_text;
	// Original uses _hotspot as a temporary variable
	Common::String                      _filename;
	Common::Rect                        _clippingRect;
	Common::Point                       _point;
	bool                                _field_CC;
	bool                                _fonts[285];
	uint32                              _field_541;
	uint32                              _field_545;
	uint32                              _field_549;
	uint32                              _soundId;
	uint32                              _field_551;
	Common::Array<EncyclopediaEntry *>  _entries;

	bool load();
	bool loadEntries(const Common::String &filename);
	void scrollUp(int16 offset);
	void scrollDown(int16 offset);

	void addHotspots();
	void addHotspot(Id target, Text *text, uint32 entryIndex);
	void setHotspot();

	Id addSound(uint32 entryIndex);

	void playMovie(uint32 entryIndex);
	void stopMovie(uint32 soundIndex);

	FontId getFontId(Facetype faceType, uint32 height, bool smallWeight, bool italic, Id target);

	void sub_484040(const Common::Point &point);
	void loadImage(uint32 entryIndex);
	void handleTarget9(uint32 entryIndex);
	void loadPage(uint32 entryIndex);
	void sub_487580();
	void setupArrowType(const Common::Point & point);
};

} // End of namespace Ring

#endif // RING_VISUAL_ENCYCLOPEDIA_H
