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

	Blt16BitValues difficultiesList(boltlib, difficultiesId);
	// TODO: select difficulty based on player option
	BltResourceList difficulty(boltlib, BltShortId(difficultiesList[0].value));
	BltId forePaletteId = difficulty[1].value;
	BltId backColorCyclesId = difficulty[2].value;
	BltId foreColorCyclesId = difficulty[3].value;
	BltId pathListId = difficulty[4].value;

	_forePalette.load(boltlib, forePaletteId);
	_backColorCycles.load(boltlib, backColorCyclesId);
	_foreColorCycles.load(boltlib, foreColorCyclesId);

	BltResourceList pathList(boltlib, pathListId);
	_paths.alloc(pathList.size());
	for (uint i = 0; i < pathList.size(); ++i) {
		_paths[i].load(boltlib, pathList[i].value);
	}
}

void ActionPuzzle::enter(uint32 time) {
	_curTime = time;
	_tickNum = 0;

	if (_backPalette) {
		_backPalette.set(*_graphics, BltPalette::kBack);
	}
	applyColorCycles(_graphics, &_backColorCycles);
	if (_forePalette) {
		_forePalette.set(*_graphics, BltPalette::kFore);
	}
	// TODO: fore color cycles

	_bgImage.drawAt(_graphics->getBackPlane().getSurface(), 0, 0, false);

	// XXX: spawn particles on all paths
	for (uint i = 0; i < _paths.size(); ++i) {
		spawnParticle(i % _particleImages.size(), i);
	}

	drawFore();

	_graphics->markDirty();
}

Card::Signal ActionPuzzle::handleEvent(const BoltEvent &event) {
	if (event.type == BoltEvent::Click) {
		// TODO: implement puzzle
		return kWin;
	}
	else if (event.type == BoltEvent::Tick) {
		uint32 diff = event.time - _curTime;
		if (diff >= kTickPeriod) {
			_curTime += kTickPeriod;
			tick();
		}
	}

	return kNull;
}

void ActionPuzzle::spawnParticle(int imageNum, int pathNum) {
	Particle particle;
	particle.imageNum = imageNum;
	particle.pathNum = pathNum;
	particle.progress = 0; // FIXME: path point 0 is special
	// FIXME: particles often don't start and end at the right place
	_particles.push_back(particle);
}

void ActionPuzzle::drawFore() {
	_graphics->getForePlane().clear();
	for (ParticleList::const_iterator it = _particles.begin(); it != _particles.end(); ++it) {
		const Particle &p = *it;
		Common::Point point = _paths[p.pathNum][p.progress].value;
		point += _paths[p.pathNum][0].value;
		_particleImages[p.imageNum].drawAt(_graphics->getForePlane().getSurface(),
			point.x, point.y, true);
	}
}

void ActionPuzzle::tick() {
	++_tickNum;

	for (ParticleList::iterator it = _particles.begin(); it != _particles.end();) {
		Particle &p = *it;
		++p.progress;

		if ((uint)p.progress >= _paths[p.pathNum].size()) {
			// Despawn
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

	drawFore();
	_graphics->markDirty();
}

} // End of namespace Bolt
