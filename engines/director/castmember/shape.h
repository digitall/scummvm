/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef DIRECTOR_CASTMEMBER_SHAPE_H
#define DIRECTOR_CASTMEMBER_SHAPE_H

#include "director/castmember/castmember.h"

namespace Director {

class ShapeCastMember : public CastMember {
public:
	ShapeCastMember(Cast *cast, uint16 castId, Common::SeekableReadStreamEndian &stream, uint16 version);
	ShapeCastMember(Cast *cast, uint16 castId, ShapeCastMember &source);

	CastMember *duplicate(Cast *cast, uint16 castId) override { return (CastMember *)(new ShapeCastMember(cast, castId, *this)); }

	uint32 getForeColor() override { return _fgCol; }
	uint32 getBackColor() override { return _bgCol; }
	void setBackColor(uint32 bgCol) override;
	void setForeColor(uint32 fgCol) override;

	bool hasField(int field) override;
	Datum getField(int field) override;
	bool setField(int field, const Datum &value) override;

	Common::String formatInfo() override;

	uint32 getCastDataSize() override;
	void writeCastData(Common::MemoryWriteStream *writeStream) override;

	ShapeType _shapeType;
	uint16 _pattern;
	byte _fillType;
	byte _lineThickness;
	byte _lineDirection;
	InkType _ink;

private:
	uint32 _fgCol;
	uint32 _bgCol;
};

} // End of namespace Director

#endif
