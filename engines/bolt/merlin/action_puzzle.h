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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef BOLT_MERLIN_ACTION_PUZZLE_H
#define BOLT_MERLIN_ACTION_PUZZLE_H

#include "bolt/bolt.h"
#include "bolt/merlin/merlin.h"
#include "bolt/boltlib/palette.h"
#include "bolt/boltlib/boltlib.h"
#include "common/random.h"

namespace Bolt {
	
class ActionPuzzle : public Card {
public:
	ActionPuzzle();
	void init(Graphics *graphics, Boltlib &boltlib, BltId resId);
	void enter(uint32 time);
	Signal handleEvent(const BoltEvent &event);
protected:
	Graphics *_graphics;
	BltImage _bgImage;
	BltPalette _backPalette;
	BltPalette _forePalette;
	BltColorCycles _backColorCycles;
	BltColorCycles _foreColorCycles;
	ScopedArray<BltImage> _particleImages;
	typedef ScopedArray<Common::Point> PointArray;
	ScopedArray<PointArray> _paths;
	PointArray _goals;

	typedef ScopedArray<BltImage> ImageArray;
	ImageArray _goalImages;
	static const int kNumDeathSequences = 3;
	ImageArray _deathSequences[kNumDeathSequences];

	uint32 _curTime;

	struct Particle {
		int imageNum;
		int pathNum;
		int deathNum; // 0: not dying; 1+: in death sequence
		int deathProgress;
		int progress;
	};

	typedef Common::List<Particle> ParticleList;
	ParticleList _particles;

	const BltImage& getParticleImage(const Particle &particle);
	Common::Point getParticlePos(const Particle &particle);
	Signal handleClick(const Common::Point &pt);
	bool isParticleAtPoint(const Particle &particle, const Common::Point &pt);
	void spawnParticle(int imageNum, int pathNum);
	void drawBack();
	void drawFore();
	void tick();
	Signal win();

	static const int kTickPeriod = 50;
	Common::RandomSource _random;
	uint _tickNum;
	uint _goalNum;
};

} // End of namespace Bolt

#endif
