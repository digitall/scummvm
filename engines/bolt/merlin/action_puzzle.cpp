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

#include "bolt/merlin/action_puzzle.h"

namespace Bolt {

ActionPuzzle::ActionPuzzle() : _random("ActionPuzzleRandomSource")
{ }

struct BltParticleDeathsStruct { // type 45
	static const uint32 kType = kBltParticleDeaths;
	static const uint kSize = 0x12;
	void load(const byte *src, Boltlib &boltlib) {
		numImages[0] = READ_BE_UINT16(&src[0]);
		imagesListId[0] = BltId(READ_BE_UINT32(&src[2]));
		numImages[1] = READ_BE_UINT16(&src[6]);
		imagesListId[1] = BltId(READ_BE_UINT32(&src[8]));
		numImages[2] = READ_BE_UINT16(&src[0xC]);
		imagesListId[2] = BltId(READ_BE_UINT32(&src[0xE]));
	}

	uint16 numImages[3];
	BltId imagesListId[3];
};

typedef BltLoader<BltParticleDeathsStruct> BltParticleDeaths;

struct BltParticlesStruct { // type 46
	static const uint32 kType = kBltParticles;
	static const uint kSize = 2;
	void load(const byte *src, Boltlib &boltlib) {
		numParticles = READ_BE_UINT16(&src[0]);
	}

	uint16 numParticles;
};

typedef BltLoader<BltParticlesStruct> BltParticles;

void ActionPuzzle::init(Graphics *graphics, Boltlib &boltlib, BltId resId) {
	_graphics = graphics;

	BltResourceList resourceList(boltlib, resId);
	BltId difficultiesId = resourceList[0].value;
	BltId particlesId = resourceList[1].value;
	BltId bgImageId = resourceList[2].value;
	BltId backPaletteId = resourceList[3].value;
	BltId particleImagesId = resourceList[4].value;

	BltParticles particles(boltlib, particlesId);
	_particleImages.alloc(particles->numParticles);
	BltResourceList particleImagesList(boltlib, particleImagesId);
	for (uint16 i = 0; i < particles->numParticles; ++i) {
		_particleImages[i].load(boltlib, particleImagesList[i].value);
	}

	_bgImage.load(boltlib, bgImageId);
	_backPalette.load(boltlib, backPaletteId);

	BltU16Values difficultiesList(boltlib, difficultiesId);
	// TODO: select difficulty based on player option
	BltResourceList difficulty(boltlib, BltShortId(difficultiesList[0].value));
	BltId forePaletteId = difficulty[1].value;
	BltId backColorCyclesId = difficulty[2].value;
	BltId foreColorCyclesId = difficulty[3].value;
	BltId pathListId = difficulty[4].value;
	BltId goalsId = difficulty[5].value;
	BltId goalImagesListId = difficulty[6].value;
	BltId particleDeathsId = difficulty[8].value;

	_forePalette.load(boltlib, forePaletteId);
	_backColorCycles.load(boltlib, backColorCyclesId);
	_foreColorCycles.load(boltlib, foreColorCyclesId);

	BltResourceList pathList(boltlib, pathListId);
	_paths.alloc(pathList.size());
	for (uint i = 0; i < pathList.size(); ++i) {
		BltS16Values pathValues(boltlib, pathList[i].value);
		int16 numPoints = pathValues[0].value;
		if (numPoints != (int)(pathValues.size() - 1) / 2) {
			warning("Invalid particle path, specified wrong number of points");
		}
		else {
			_paths[i].alloc(numPoints);
			for (int16 j = 0; j < numPoints; ++j) {
				_paths[i][j].x = pathValues[1 + 2 * j].value;
				_paths[i][j].y = pathValues[1 + 2 * j + 1].value;
			}
		}
	}

	BltS16Values goalsValues(boltlib, goalsId);
	_goals.alloc(goalsValues.size() / 2);
	for (uint i = 0; i < _goals.size(); ++i) {
		_goals[i].x = goalsValues[2 * i].value;
		_goals[i].y = goalsValues[2 * i + 1].value;
	}

	BltResourceList goalImagesList(boltlib, goalImagesListId);
	_goalImages.alloc(goalImagesList.size());
	for (uint i = 0; i < goalImagesList.size(); ++i) {
		_goalImages[i].load(boltlib, goalImagesList[i].value);
	}

	BltParticleDeaths particleDeaths(boltlib, particleDeathsId);
	for (int i = 0; i < kNumDeathSequences; ++i) {
		_deathSequences[i].alloc(particleDeaths->numImages[i]);
		BltResourceList imageList(boltlib, particleDeaths->imagesListId[i]);
		for (int j = 0; j < particleDeaths->numImages[i]; ++j) {
			_deathSequences[i][j].load(boltlib, imageList[j].value);
		}
	}
}

void ActionPuzzle::enter(uint32 time) {
	_curTime = time;
	_tickNum = 0;
	// TODO: Load progress from save data
	// (check original to see if action puzzles are saved)
	// (and what happens when you change difficulty mid-puzzle?)
	_goalNum = 0;

	applyPalette(_graphics, kBack, _backPalette);
	applyColorCycles(_graphics, &_backColorCycles);
	applyPalette(_graphics, kFore, _forePalette);
	// TODO: fore color cycles

	drawBack();

	// XXX: spawn particles on all paths
	for (uint i = 0; i < _paths.size(); ++i) {
		spawnParticle(i % _particleImages.size(), i);
	}

	drawFore();

	_graphics->markDirty();
}

Card::Signal ActionPuzzle::handleEvent(const BoltEvent &event) {
	if (event.type == BoltEvent::Click) {
		return handleClick(event.point);
	}
	else if (event.type == BoltEvent::Tick) {
		uint32 diff = event.time - _curTime;
		if (diff >= kTickPeriod) {
			_curTime += kTickPeriod;
			tick();
		}

		if (_goalNum >= _goals.size()) {
			// Reset background before starting win movie
			_bgImage.drawAt(_graphics->getPlaneSurface(kBack), 0, 0, false);
			_graphics->clearPlane(kFore);
			return kWin;
		}
	}

	return kNull;
}

const BltImage& ActionPuzzle::getParticleImage(const Particle &particle) {
	if (particle.deathNum > 0) {
		const ImageArray &deathSequence = _deathSequences[particle.deathNum - 1];
		return deathSequence[particle.deathProgress];
	}
	else {
		return _particleImages[particle.imageNum];
	}
}

Common::Point ActionPuzzle::getParticlePos(const Particle &particle) {
	return _paths[particle.pathNum][particle.progress];
}

Card::Signal ActionPuzzle::handleClick(const Common::Point &pt) {
	for (ParticleList::iterator it = _particles.begin(); it != _particles.end(); ++it) {
		if (isParticleAtPoint(*it, pt)) {
			// Kill particle
			it->deathNum = _random.getRandomNumberRng(1, 3);
		}
	}

	return kNull;
}

bool ActionPuzzle::isParticleAtPoint(const Particle &particle, const Common::Point &pt) {
	// Only consider particles that are not dying
	if (particle.deathNum == 0) {
		Common::Rect rect = getParticleImage(particle).getRect(getParticlePos(particle));
		return rect.contains(pt);
	}

	return false;
}

void ActionPuzzle::spawnParticle(int imageNum, int pathNum) {
	Particle particle;
	particle.imageNum = imageNum;
	particle.pathNum = pathNum;
	particle.deathNum = 0;
	particle.deathProgress = 0;
	particle.progress = 0;
	_particles.push_back(particle);
}

void ActionPuzzle::drawBack() {
	_bgImage.drawAt(_graphics->getPlaneSurface(kBack), 0, 0, false);
	for (uint i = 0; i < _goals.size(); ++i) {
		if (i < _goalNum) {
			const Common::Point &pt = _goals[i];
			// TODO: there may be multiple sets of goals
			// (player has to complete one set and then the next)
			_goalImages[0].drawAt(_graphics->getPlaneSurface(kBack), pt.x, pt.y, true);
		}
	}
}

void ActionPuzzle::drawFore() {
	_graphics->clearPlane(kFore);

	for (ParticleList::const_iterator it = _particles.begin(); it != _particles.end(); ++it) {
		const Particle &p = *it;
		const BltImage &image = getParticleImage(p);
		Common::Point pt = getParticlePos(p);
		// FIXME: positions of particles in death sequence are wrong
		image.drawAt(_graphics->getPlaneSurface(kFore), pt.x, pt.y, true);
	}
}

void ActionPuzzle::tick() {
	++_tickNum;

	for (ParticleList::iterator it = _particles.begin(); it != _particles.end();) {
		Particle &p = *it;
		++p.progress;

		if (p.deathNum > 0) {
			++p.deathProgress;
		}

		bool despawn = (uint)p.progress >= _paths[p.pathNum].size();
		if (p.deathNum > 0) {
			const ImageArray &deathSequence = _deathSequences[p.deathNum - 1];
			despawn |= (uint)p.deathProgress >= deathSequence.size();
		}

		if (despawn) {
			it = _particles.erase(it);
		}
		else {
			++it;
		}
	}

	// Spawn a new particle every 20 ticks
	static const int kNewParticleTicks = 20;
	if (_tickNum % kNewParticleTicks == 0) {
		spawnParticle(_random.getRandomNumber(_particleImages.size()-1),
			_random.getRandomNumber(_paths.size()-1));
	}

	// Award new goal every 100 ticks
	// TODO: implement game
	static const int kGoalTicks = 100;
	if (_tickNum % kGoalTicks == 0) {
		if (_goalNum < _goals.size()) {
			++_goalNum;
			drawBack();
		}
	}

	drawFore();
	_graphics->markDirty();
}

} // End of namespace Bolt
