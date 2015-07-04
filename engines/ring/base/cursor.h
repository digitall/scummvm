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

#ifndef RING_CURSOR_H
#define RING_CURSOR_H

#include "ring/graphics/animation.h"

#include "ring/shared.h"

namespace Ring {

class ImageSurface;

//////////////////////////////////////////////////////////////////////////
class CursorBase : public BaseObject {
public:
	CursorBase();
	virtual ~CursorBase();

	virtual void init(CursorId id, Common::String name, CursorType cursorType, byte frameCount);
	virtual void draw() { error("[CursorBase::draw] Invalid call to base cursor draw method"); }

	// Accessors
	void setOffset(Common::Point offset) { _offset = offset; }

	Common::String getName() { return _name; }
	CursorType getType() { return _type; }
	byte getFrameCount() { return _frameCount; }

protected:
	Common::String _name;
	CursorType _type;
	Common::Point _offset;
	byte _frameCount;

	virtual void alloc() {};
	virtual void dealloc() {};
};

//////////////////////////////////////////////////////////////////////////
class CursorImage : public CursorBase {
public:
	CursorImage();
	~CursorImage();

	void init(CursorId id, Common::String name, CursorType cursorType, byte frameCount, ArchiveType archiveType);
	void alloc() override;
	void dealloc() override;
	void draw() override;

private:
	ImageSurface *_image;
	ArchiveType   _archiveType;
};

//////////////////////////////////////////////////////////////////////////
class CursorAnimation : public CursorBase, public AnimationImage {
public:
	CursorAnimation();
	~CursorAnimation();

	void init(CursorId id, Common::String name, CursorType cursorType, byte imageCount, uint32 frameCount, float frameRate, byte a7, LoadFrom loadFrom, ArchiveType archiveType);
	void alloc() override;
	void dealloc() override;
	void draw() override;
};

//////////////////////////////////////////////////////////////////////////
class CursorHandler {
public:
	CursorHandler();
	~CursorHandler();

	void add(CursorId id, Common::String name, CursorType cursorType, byte imageCount, uint32 frameCount, float frameRate, byte a6, LoadFrom loadFrom, ArchiveType archiveType);
	void removeByFrameCount(int framecount);

	void draw();
	void hide() const;
	void select(CursorId id);
	void setOffset(CursorId id, const Common::Point &offset);
	CursorType getType();

private:
	AssociativeArray<CursorBase *> _cursors;
	uint32 _index;
};

} // End of namespace Ring

#endif // RING_CURSOR_H
