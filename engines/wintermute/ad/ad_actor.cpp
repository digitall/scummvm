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

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */

#include "engines/wintermute/ad/ad_actor.h"
#include "engines/wintermute/ad/ad_game.h"
#include "engines/wintermute/ad/ad_scene.h"
#include "engines/wintermute/ad/ad_entity.h"
#include "engines/wintermute/ad/ad_sprite_set.h"
#include "engines/wintermute/ad/ad_waypoint_group.h"
#include "engines/wintermute/ad/ad_path.h"
#include "engines/wintermute/ad/ad_sentence.h"
#include "engines/wintermute/base/base_frame.h"
#include "engines/wintermute/base/base_parser.h"
#include "engines/wintermute/base/sound/base_sound.h"
#include "engines/wintermute/base/base_region.h"
#include "engines/wintermute/base/base_file_manager.h"
#include "engines/wintermute/base/base_sprite.h"
#include "engines/wintermute/base/scriptables/script.h"
#include "engines/wintermute/base/scriptables/script_value.h"
#include "engines/wintermute/base/scriptables/script_stack.h"
#include "engines/wintermute/base/particles/part_emitter.h"
#include "engines/wintermute/base/base_engine.h"

namespace Wintermute {

IMPLEMENT_PERSISTENT(AdActor, false)


//////////////////////////////////////////////////////////////////////////
AdActor::AdActor(BaseGame *inGame) : AdTalkHolder(inGame) {
	_path = new AdPath(_gameRef);

	_type = OBJECT_ACTOR;
	_dir = DI_LEFT;

	_walkSprite = nullptr;
	_standSprite = nullptr;
	_turnLeftSprite = nullptr;
	_turnRightSprite = nullptr;

	_targetPoint = new BasePoint;
	_afterWalkDir = DI_NONE;

	_animSprite2 = nullptr;

	setDefaultAnimNames();
}

//////////////////////////////////////////////////////////////////////////
bool AdActor::setDefaultAnimNames() {
	_talkAnimName = "talk";
	_idleAnimName = "idle";
	_walkAnimName = "walk";
	_turnLeftAnimName = "turnleft";
	_turnRightAnimName = "turnright";
	return STATUS_OK;
}

//////////////////////////////////////////////////////////////////////////
AdActor::~AdActor() {
	delete _path;
	delete _targetPoint;
	_path = nullptr;
	_targetPoint = nullptr;

	delete _walkSprite;
	delete _standSprite;
	delete _turnLeftSprite;
	delete _turnRightSprite;
	_walkSprite = nullptr;
	_standSprite = nullptr;
	_turnLeftSprite = nullptr;
	_turnRightSprite = nullptr;

	_animSprite2 = nullptr; // ref only

	for (uint32 i = 0; i < _talkSprites.getSize(); i++) {
		delete _talkSprites[i];
	}
	_talkSprites.removeAll();

	for (uint32 i = 0; i < _talkSpritesEx.getSize(); i++) {
		delete _talkSpritesEx[i];
	}
	_talkSpritesEx.removeAll();

	for (uint32 i = 0; i < _anims.getSize(); i++) {
		delete _anims[i];
		_anims[i] = nullptr;
	}
	_anims.removeAll();
}


//////////////////////////////////////////////////////////////////////////
bool AdActor::loadFile(const char *filename) {
	char *buffer = (char *)BaseFileManager::getEngineInstance()->readWholeFile(filename);
	if (buffer == nullptr) {
		_gameRef->LOG(0, "AdActor::LoadFile failed for file '%s'", filename);
		return STATUS_FAILED;
	}

	bool ret;

	setFilename(filename);

	if (DID_FAIL(ret = loadBuffer(buffer, true))) {
		_gameRef->LOG(0, "Error parsing ACTOR file '%s'", filename);
	}


	delete[] buffer;

	return ret;
}


TOKEN_DEF_START
TOKEN_DEF(ACTOR)
TOKEN_DEF(X)
TOKEN_DEF(Y)
TOKEN_DEF(TEMPLATE)
TOKEN_DEF(NAME)
TOKEN_DEF(SCALABLE)
TOKEN_DEF(REGISTRABLE)
TOKEN_DEF(INTERACTIVE)
TOKEN_DEF(SHADOWABLE)
TOKEN_DEF(COLORABLE)
TOKEN_DEF(ACTIVE)
TOKEN_DEF(WALK)
TOKEN_DEF(STAND)
TOKEN_DEF(TALK_SPECIAL)
TOKEN_DEF(TALK)
TOKEN_DEF(TURN_LEFT)
TOKEN_DEF(TURN_RIGHT)
TOKEN_DEF(EVENTS)
TOKEN_DEF(FONT)
TOKEN_DEF(CURSOR)
TOKEN_DEF(SCRIPT)
TOKEN_DEF(SOUND_VOLUME)
TOKEN_DEF(SOUND_PANNING)
TOKEN_DEF(CAPTION)
TOKEN_DEF(PROPERTY)
TOKEN_DEF(BLOCKED_REGION)
TOKEN_DEF(WAYPOINTS)
TOKEN_DEF(IGNORE_ITEMS)
TOKEN_DEF(ROTABLE)
TOKEN_DEF(ROTATABLE)
TOKEN_DEF(ALPHA_COLOR)
TOKEN_DEF(SCALE)
TOKEN_DEF(RELATIVE_SCALE)
TOKEN_DEF(ALPHA)
TOKEN_DEF(EDITOR_PROPERTY)
TOKEN_DEF(ANIMATION)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////////
bool AdActor::loadBuffer(char *buffer, bool complete) {
	TOKEN_TABLE_START(commands)
	TOKEN_TABLE(ACTOR)
	TOKEN_TABLE(X)
	TOKEN_TABLE(Y)
	TOKEN_TABLE(TEMPLATE)
	TOKEN_TABLE(NAME)
	TOKEN_TABLE(SCALABLE)
	TOKEN_TABLE(REGISTRABLE)
	TOKEN_TABLE(INTERACTIVE)
	TOKEN_TABLE(SHADOWABLE)
	TOKEN_TABLE(COLORABLE)
	TOKEN_TABLE(ACTIVE)
	TOKEN_TABLE(WALK)
	TOKEN_TABLE(STAND)
	TOKEN_TABLE(TALK_SPECIAL)
	TOKEN_TABLE(TALK)
	TOKEN_TABLE(TURN_LEFT)
	TOKEN_TABLE(TURN_RIGHT)
	TOKEN_TABLE(EVENTS)
	TOKEN_TABLE(FONT)
	TOKEN_TABLE(CURSOR)
	TOKEN_TABLE(SCRIPT)
	TOKEN_TABLE(SOUND_VOLUME)
	TOKEN_TABLE(SOUND_PANNING)
	TOKEN_TABLE(CAPTION)
	TOKEN_TABLE(PROPERTY)
	TOKEN_TABLE(BLOCKED_REGION)
	TOKEN_TABLE(WAYPOINTS)
	TOKEN_TABLE(IGNORE_ITEMS)
	TOKEN_TABLE(ROTABLE)
	TOKEN_TABLE(ROTATABLE)
	TOKEN_TABLE(ALPHA_COLOR)
	TOKEN_TABLE(SCALE)
	TOKEN_TABLE(RELATIVE_SCALE)
	TOKEN_TABLE(ALPHA)
	TOKEN_TABLE(EDITOR_PROPERTY)
	TOKEN_TABLE(ANIMATION)
	TOKEN_TABLE_END

	char *params;
	int cmd;
	BaseParser parser;

	if (complete) {
		if (parser.getCommand(&buffer, commands, &params) != TOKEN_ACTOR) {
			_gameRef->LOG(0, "'ACTOR' keyword expected.");
			return STATUS_FAILED;
		}
		buffer = params;
	}

	AdGame *adGame = (AdGame *)_gameRef;
	AdSpriteSet *spr = nullptr;
	int ar = 0, ag = 0, ab = 0, alpha = 0;
	while ((cmd = parser.getCommand(&buffer, commands, &params)) > 0) {
		switch (cmd) {
		case TOKEN_TEMPLATE:
			if (DID_FAIL(loadFile(params))) {
				cmd = PARSERR_GENERIC;
			}
			break;

		case TOKEN_X:
			parser.scanStr(params, "%d", &_posX);
			break;

		case TOKEN_Y:
			parser.scanStr(params, "%d", &_posY);
			break;

		case TOKEN_NAME:
			setName(params);
			break;

		case TOKEN_CAPTION:
			setCaption(params);
			break;

		case TOKEN_FONT:
			setFont(params);
			break;

		case TOKEN_SCALABLE:
			parser.scanStr(params, "%b", &_zoomable);
			break;

		case TOKEN_ROTABLE:
		case TOKEN_ROTATABLE:
			parser.scanStr(params, "%b", &_rotatable);
			break;

		case TOKEN_REGISTRABLE:
		case TOKEN_INTERACTIVE:
			parser.scanStr(params, "%b", &_registrable);
			break;

		case TOKEN_SHADOWABLE:
		case TOKEN_COLORABLE:
			parser.scanStr(params, "%b", &_shadowable);
			break;

		case TOKEN_ACTIVE:
			parser.scanStr(params, "%b", &_active);
			break;

		case TOKEN_WALK:
			delete _walkSprite;
			_walkSprite = nullptr;
			spr = new AdSpriteSet(_gameRef, this);
			if (!spr || DID_FAIL(spr->loadBuffer(params, true, adGame->_texWalkLifeTime, CACHE_HALF))) {
				cmd = PARSERR_GENERIC;
			} else {
				_walkSprite = spr;
			}
			break;

		case TOKEN_TALK:
			spr = new AdSpriteSet(_gameRef, this);
			if (!spr || DID_FAIL(spr->loadBuffer(params, true, adGame->_texTalkLifeTime))) {
				cmd = PARSERR_GENERIC;
			} else {
				_talkSprites.add(spr);
			}
			break;

		case TOKEN_TALK_SPECIAL:
			spr = new AdSpriteSet(_gameRef, this);
			if (!spr || DID_FAIL(spr->loadBuffer(params, true, adGame->_texTalkLifeTime))) {
				cmd = PARSERR_GENERIC;
			} else {
				_talkSpritesEx.add(spr);
			}
			break;

		case TOKEN_STAND:
			delete _standSprite;
			_standSprite = nullptr;
			spr = new AdSpriteSet(_gameRef, this);
			if (!spr || DID_FAIL(spr->loadBuffer(params, true, adGame->_texStandLifeTime))) {
				cmd = PARSERR_GENERIC;
			} else {
				_standSprite = spr;
			}
			break;

		case TOKEN_TURN_LEFT:
			delete _turnLeftSprite;
			_turnLeftSprite = nullptr;
			spr = new AdSpriteSet(_gameRef, this);
			if (!spr || DID_FAIL(spr->loadBuffer(params, true))) {
				cmd = PARSERR_GENERIC;
			} else {
				_turnLeftSprite = spr;
			}
			break;

		case TOKEN_TURN_RIGHT:
			delete _turnRightSprite;
			_turnRightSprite = nullptr;
			spr = new AdSpriteSet(_gameRef, this);
			if (!spr || DID_FAIL(spr->loadBuffer(params, true))) {
				cmd = PARSERR_GENERIC;
			} else {
				_turnRightSprite = spr;
			}
			break;

		case TOKEN_SCRIPT:
			addScript(params);
			break;

		case TOKEN_CURSOR:
			delete _cursor;
			_cursor = new BaseSprite(_gameRef);
			if (!_cursor || DID_FAIL(_cursor->loadFile(params))) {
				delete _cursor;
				_cursor = nullptr;
				cmd = PARSERR_GENERIC;
			}
			break;

		case TOKEN_SOUND_VOLUME:
			parser.scanStr(params, "%d", &_sFXVolume);
			break;

		case TOKEN_SCALE: {
			int s;
			parser.scanStr(params, "%d", &s);
			_scale = (float)s;

		}
		break;

		case TOKEN_RELATIVE_SCALE: {
			int s;
			parser.scanStr(params, "%d", &s);
			_relativeScale = (float)s;

		}
		break;

		case TOKEN_SOUND_PANNING:
			parser.scanStr(params, "%b", &_autoSoundPanning);
			break;

		case TOKEN_PROPERTY:
			parseProperty(params, false);
			break;

		case TOKEN_BLOCKED_REGION: {
			delete _blockRegion;
			delete _currentBlockRegion;
			_blockRegion = nullptr;
			_currentBlockRegion = nullptr;
			BaseRegion *rgn = new BaseRegion(_gameRef);
			BaseRegion *crgn = new BaseRegion(_gameRef);
			if (!rgn || !crgn || DID_FAIL(rgn->loadBuffer(params, false))) {
				delete _blockRegion;
				delete _currentBlockRegion;
				_blockRegion = nullptr;
				_currentBlockRegion = nullptr;
				cmd = PARSERR_GENERIC;
			} else {
				_blockRegion = rgn;
				_currentBlockRegion = crgn;
				_currentBlockRegion->mimic(_blockRegion);
			}
		}
		break;

		case TOKEN_WAYPOINTS: {
			delete _wptGroup;
			delete _currentWptGroup;
			_wptGroup = nullptr;
			_currentWptGroup = nullptr;
			AdWaypointGroup *wpt = new AdWaypointGroup(_gameRef);
			AdWaypointGroup *cwpt = new AdWaypointGroup(_gameRef);
			if (!wpt || !cwpt || DID_FAIL(wpt->loadBuffer(params, false))) {
				delete _wptGroup;
				delete _currentWptGroup;
				_wptGroup = nullptr;
				_currentWptGroup = nullptr;
				cmd = PARSERR_GENERIC;
			} else {
				_wptGroup = wpt;
				_currentWptGroup = cwpt;
				_currentWptGroup->mimic(_wptGroup);
			}
		}
		break;

		case TOKEN_IGNORE_ITEMS:
			parser.scanStr(params, "%b", &_ignoreItems);
			break;

		case TOKEN_ALPHA_COLOR:
			parser.scanStr(params, "%d,%d,%d", &ar, &ag, &ab);
			break;

		case TOKEN_ALPHA:
			parser.scanStr(params, "%d", &alpha);
			break;

		case TOKEN_EDITOR_PROPERTY:
			parseEditorProperty(params, false);
			break;

		case TOKEN_ANIMATION: {
			AdSpriteSet *anim = new AdSpriteSet(_gameRef, this);
			if (!anim || DID_FAIL(anim->loadBuffer(params, false))) {
				cmd = PARSERR_GENERIC;
			} else {
				_anims.add(anim);
			}
		}
		break;

		default:
			break;
		}
	}
	if (cmd == PARSERR_TOKENNOTFOUND) {
		_gameRef->LOG(0, "Syntax error in ACTOR definition");
		return STATUS_FAILED;
	}
	if (cmd == PARSERR_GENERIC) {
		if (spr) {
			delete spr;
		}
		_gameRef->LOG(0, "Error loading ACTOR definition");
		return STATUS_FAILED;
	}

	if (alpha != 0 && ar == 0 && ag == 0 && ab == 0) {
		ar = ag = ab = 255;
	}
	_alphaColor = BYTETORGBA(ar, ag, ab, alpha);
	_state = _nextState = STATE_READY;

	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
void AdActor::turnTo(TDirection dir) {
	int delta1, delta2, delta3, delta;

	delta1 = dir - _dir;
	delta2 = dir + NUM_DIRECTIONS - _dir;
	delta3 = dir - NUM_DIRECTIONS - _dir;

	delta1 = (abs(delta1) <= abs(delta2)) ? delta1 : delta2;
	delta = (abs(delta1) <= abs(delta3)) ? delta1 : delta3;

	// already there?
	if (abs(delta) < 2) {
		_dir = dir;
		_targetDir = dir;
		_state = _nextState;
		_nextState = STATE_READY;
		_tempSprite2 = nullptr;
		return;
	}

	_targetDir = dir;
	_state = delta < 0 ? STATE_TURNING_LEFT : STATE_TURNING_RIGHT;

	_tempSprite2 = nullptr;
}


//////////////////////////////////////////////////////////////////////////
void AdActor::goTo(int x, int y, TDirection afterWalkDir) {
	_afterWalkDir = afterWalkDir;
	if (x == _targetPoint->x && y == _targetPoint->y && _state == STATE_FOLLOWING_PATH) {
		return;
	}

	_path->reset();
	_path->setReady(false);

	_targetPoint->x = x;
	_targetPoint->y = y;

	((AdGame *)_gameRef)->_scene->correctTargetPoint(_posX, _posY, &_targetPoint->x, &_targetPoint->y, true, this);

	_state = STATE_SEARCHING_PATH;

}


//////////////////////////////////////////////////////////////////////////
bool AdActor::display() {
	if (_active) {
		updateSounds();
	}

	uint32 alpha;
	if (_alphaColor != 0) {
		alpha = _alphaColor;
	} else {
		alpha = _shadowable ? ((AdGame *)_gameRef)->_scene->getAlphaAt(_posX, _posY, true) : 0xFFFFFFFF;
	}

	float scaleX, scaleY;
	getScale(&scaleX, &scaleY);


	float rotate;
	if (_rotatable) {
		if (_rotateValid) {
			rotate = _rotate;
		} else {
			rotate = ((AdGame *)_gameRef)->_scene->getRotationAt(_posX, _posY) + _relativeRotate;
		}
	} else {
		rotate = 0.0f;
	}

	if (_active) {
		displaySpriteAttachments(true);
	}

	if (_currentSprite && _active) {
		bool reg = _registrable;
		if (_ignoreItems && ((AdGame *)_gameRef)->_selectedItem) {
			reg = false;
		}

		_currentSprite->display(_posX,
		                        _posY,
		                        reg ? _registerAlias : nullptr,
		                        scaleX,
		                        scaleY,
		                        alpha,
		                        rotate,
		                        _blendMode);

	}

	if (_active) {
		displaySpriteAttachments(false);
	}
	if (_active && _partEmitter) {
		_partEmitter->display();
	}


	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
bool AdActor::update() {
	_currentSprite = nullptr;

	if (_state == STATE_READY) {
		if (_animSprite) {
			delete _animSprite;
			_animSprite = nullptr;
		}
		if (_animSprite2) {
			_animSprite2 = nullptr;
		}
	}

	// finished playing animation?
	if (_state == STATE_PLAYING_ANIM && _animSprite != nullptr && _animSprite->isFinished()) {
		_state = _nextState;
		_nextState = STATE_READY;
		_currentSprite = _animSprite;
	}

	if (_state == STATE_PLAYING_ANIM_SET && _animSprite2 != nullptr && _animSprite2->isFinished()) {
		_state = _nextState;
		_nextState = STATE_READY;
		_currentSprite = _animSprite2;
	}

	if (_sentence && _state != STATE_TALKING) {
		_sentence->finish();
	}

	// Below condition code is present in Lite up to (Feb 8, 2012) (SVN repo)
	// Present in Lite up to (Nov 1, 2015) (Git repo)
	// Present up to 1.9.1 (Jan 1, 2010)
	// Seems removed into 1.10.1 beta (July 19, 2012)
	// or later but before Mar 21, 2013 (import into Git repo)
	//
	// default: stand animation
	if (BaseEngine::instance().getTargetExecutable() < WME_1_9_2 &&
	    BaseEngine::instance().getTargetExecutable() >= WME_LITE &&
	    !_currentSprite) {
		if (_sprite) {
			_currentSprite = _sprite;
		} else {
			if (_standSprite) {
				_currentSprite = _standSprite->getSprite(_dir);
			} else {
				AdSpriteSet *anim = getAnimByName(_idleAnimName);
				if (anim) {
					_currentSprite = anim->getSprite(_dir);
				}
			}
		}
	}

	bool already_moved = false;

	switch (_state) {
		//////////////////////////////////////////////////////////////////////////
	case STATE_PLAYING_ANIM:
		_currentSprite = _animSprite;
		break;

		//////////////////////////////////////////////////////////////////////////
	case STATE_PLAYING_ANIM_SET:
		_currentSprite = _animSprite2;
		break;

		//////////////////////////////////////////////////////////////////////////
	case STATE_TURNING_LEFT:
		if (_tempSprite2 == nullptr || _tempSprite2->isFinished()) {
			if (_dir > 0) {
				_dir = (TDirection)(_dir - 1);
			} else {
				_dir = (TDirection)(NUM_DIRECTIONS - 1);
			}

			if (_dir == _targetDir) {
				_tempSprite2 = nullptr;
				_state = _nextState;
				_nextState = STATE_READY;
			} else {
				if (_turnLeftSprite) {
					_tempSprite2 = _turnLeftSprite->getSprite(_dir);
				} else {
					AdSpriteSet *anim = getAnimByName(_turnLeftAnimName);
					if (anim) {
						_tempSprite2 = anim->getSprite(_dir);
					}
				}

				if (_tempSprite2) {
					_tempSprite2->reset();
					if (_tempSprite2->_looping) {
						_tempSprite2->_looping = false;
					}
				}
				_currentSprite = _tempSprite2;
			}
		} else {
			_currentSprite = _tempSprite2;
		}
		break;


		//////////////////////////////////////////////////////////////////////////
	case STATE_TURNING_RIGHT:
		if (_tempSprite2 == nullptr || _tempSprite2->isFinished()) {
			_dir = (TDirection)(_dir + 1);

			if ((int)_dir >= (int)NUM_DIRECTIONS) {
				_dir = (TDirection)(0);
			}

			if (_dir == _targetDir) {
				_tempSprite2 = nullptr;
				_state = _nextState;
				_nextState = STATE_READY;
			} else {
				if (_turnRightSprite) {
					_tempSprite2 = _turnRightSprite->getSprite(_dir);
				} else {
					AdSpriteSet *anim = getAnimByName(_turnRightAnimName);
					if (anim) {
						_tempSprite2 = anim->getSprite(_dir);
					}
				}

				if (_tempSprite2) {
					_tempSprite2->reset();
					if (_tempSprite2->_looping) {
						_tempSprite2->_looping = false;
					}
				}
				_currentSprite = _tempSprite2;
			}
		} else {
			_currentSprite = _tempSprite2;
		}
		break;


		//////////////////////////////////////////////////////////////////////////
	case STATE_SEARCHING_PATH:
		// keep asking scene for the path
		if (((AdGame *)_gameRef)->_scene->getPath(BasePoint(_posX, _posY), *_targetPoint, _path, this)) {
			_state = STATE_WAITING_PATH;
		}
		break;


		//////////////////////////////////////////////////////////////////////////
	case STATE_WAITING_PATH:
		// wait until the scene finished the path
		if (_path->_ready) {
			followPath();
		}
		break;


		//////////////////////////////////////////////////////////////////////////
	case STATE_FOLLOWING_PATH:
		getNextStep();
		already_moved = true;
		break;

		//////////////////////////////////////////////////////////////////////////
	case STATE_TALKING: {
		_sentence->update(_dir);
		if (_sentence->_currentSprite) {
			_tempSprite2 = _sentence->_currentSprite;
		}

		bool timeIsUp = (_sentence->_sound && _sentence->_soundStarted && (!_sentence->_sound->isPlaying() && !_sentence->_sound->isPaused())) || (!_sentence->_sound && _sentence->_duration <= _gameRef->getTimer()->getTime() - _sentence->_startTime);
		if (_tempSprite2 == nullptr || _tempSprite2->isFinished() || (/*_tempSprite2->_looping &&*/ timeIsUp)) {
			if (timeIsUp) {
				_sentence->finish();
				_tempSprite2 = nullptr;
				_state = _nextState;
				_nextState = STATE_READY;
			} else {
				_tempSprite2 = getTalkStance(_sentence->getNextStance());
				if (_tempSprite2) {
					_tempSprite2->reset();
					_currentSprite = _tempSprite2;
					((AdGame *)_gameRef)->addSentence(_sentence);
				}
			}
		} else {
			_currentSprite = _tempSprite2;
			((AdGame *)_gameRef)->addSentence(_sentence);
		}
	}
	break;

	//////////////////////////////////////////////////////////////////////////
	case STATE_READY:
		if (!_animSprite && !_animSprite2) {
			if (_sprite) {
				_currentSprite = _sprite;
			} else {
				if (_standSprite) {
					_currentSprite = _standSprite->getSprite(_dir);
				} else {
					AdSpriteSet *anim = getAnimByName(_idleAnimName);
					if (anim) {
						_currentSprite = anim->getSprite(_dir);
					}
				}
			}
		}
		break;
	default:
		error("AdActor::Update - Unhandled enum");
	}

	// Below condition code is not present in Lite up to (Feb 8, 2012) (SVN repo)
	// Not present in Lite up to (Nov 1, 2015) (Git repo)
	// Not present up to 1.9.1 (Jan 1, 2010)
	// Seems added into 1.10.1 beta (July 19, 2012)
	// or later but before Mar 21, 2013 (import into Git repo)
	//
	// default: stand animation
	if (BaseEngine::instance().getTargetExecutable() >= WME_1_9_2 &&
	    BaseEngine::instance().getTargetExecutable() < WME_LITE &&
	    !_currentSprite) {
		if (_sprite) {
			_currentSprite = _sprite;
		} else {
			if (_standSprite) {
				_currentSprite = _standSprite->getSprite(_dir);
			} else {
				AdSpriteSet *anim = getAnimByName(_idleAnimName);
				if (anim) {
					_currentSprite = anim->getSprite(_dir);
				}
			}
		}
	}

	if (_currentSprite && !already_moved) {
		_currentSprite->getCurrentFrame(_zoomable ? ((AdGame *)_gameRef)->_scene->getZoomAt(_posX, _posY) : 100, _zoomable ? ((AdGame *)_gameRef)->_scene->getZoomAt(_posX, _posY) : 100);
		if (_currentSprite->isChanged()) {
			_posX += _currentSprite->_moveX;
			_posY += _currentSprite->_moveY;
			afterMove();
		}
	}

	//_gameRef->QuickMessageForm("%s", _currentSprite->_filename);

	updateBlockRegion();
	_ready = (_state == STATE_READY);

	updatePartEmitter();
	updateSpriteAttachments();

	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
void AdActor::followPath() {
	// skip current position
	_path->getFirst();
	while (_path->getCurrent() != nullptr) {
		if (_path->getCurrent()->x != _posX || _path->getCurrent()->y != _posY) {
			break;
		}
		_path->getNext();
	}

	// are there points to follow?
	if (_path->getCurrent() != nullptr) {
		_state = STATE_FOLLOWING_PATH;
		initLine(BasePoint(_posX, _posY), *_path->getCurrent());
	} else {
		if (_afterWalkDir != DI_NONE) {
			turnTo(_afterWalkDir);
		} else {
			_state = STATE_READY;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
void AdActor::getNextStep() {
	if (_walkSprite) {
		_currentSprite = _walkSprite->getSprite(_dir);
	} else {
		AdSpriteSet *anim = getAnimByName(_walkAnimName);
		if (anim) {
			_currentSprite = anim->getSprite(_dir);
		}
	}

	if (!_currentSprite) {
		return;
	}

	_currentSprite->getCurrentFrame(_zoomable ? ((AdGame *)_gameRef)->_scene->getZoomAt(_posX, _posY) : 100, _zoomable ? ((AdGame *)_gameRef)->_scene->getZoomAt(_posX, _posY) : 100);
	if (!_currentSprite->isChanged()) {
		return;
	}


	int maxStepX, maxStepY;
	maxStepX = abs(_currentSprite->_moveX);
	maxStepY = abs(_currentSprite->_moveY);

	maxStepX = MAX(maxStepX, maxStepY);
	maxStepX = MAX(maxStepX, 1);

	while (_pFCount > 0 && maxStepX >= 0) {
		_pFX += _pFStepX;
		_pFY += _pFStepY;

		_pFCount--;
		maxStepX--;
	}

	if (((AdGame *)_gameRef)->_scene->isBlockedAt((int)_pFX, (int)_pFY, true, this)) {
		if (_pFCount == 0) {
			_state = _nextState;
			_nextState = STATE_READY;
			return;
		}
		goTo(_targetPoint->x, _targetPoint->y);
		return;
	}


	_posX = (int)_pFX;
	_posY = (int)_pFY;

	afterMove();


	if (_pFCount == 0) {
		if (_path->getNext() == nullptr) {
			_posX = _targetPoint->x;
			_posY = _targetPoint->y;

			_path->reset();
			if (_afterWalkDir != DI_NONE) {
				turnTo(_afterWalkDir);
			} else {
				_state = _nextState;
				_nextState = STATE_READY;
			}
		} else {
			initLine(BasePoint(_posX, _posY), *_path->getCurrent());
		}
	}
}


//////////////////////////////////////////////////////////////////////////
void AdActor::initLine(const BasePoint &startPt, const BasePoint &endPt) {
	_pFCount = MAX((abs(endPt.x - startPt.x)), (abs(endPt.y - startPt.y)));

	_pFStepX = (double)(endPt.x - startPt.x) / _pFCount;
	_pFStepY = (double)(endPt.y - startPt.y) / _pFCount;

	_pFX = startPt.x;
	_pFY = startPt.y;

	int angle = (int)(atan2((double)(endPt.y - startPt.y), (double)(endPt.x - startPt.x)) * (180 / 3.14));

	_nextState = STATE_FOLLOWING_PATH;

	turnTo(angleToDirection(angle));
}


//////////////////////////////////////////////////////////////////////////
// high level scripting interface
//////////////////////////////////////////////////////////////////////////
bool AdActor::scCallMethod(ScScript *script, ScStack *stack, ScStack *thisStack, const char *name) {
	//////////////////////////////////////////////////////////////////////////
	// GoTo / GoToAsync
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(name, "GoTo") == 0 || strcmp(name, "GoToAsync") == 0) {
		stack->correctParams(2);
		int x = stack->pop()->getInt();
		int y = stack->pop()->getInt();
		goTo(x, y);
		if (strcmp(name, "GoToAsync") != 0) {
			script->waitForExclusive(this);
		}
		stack->pushNULL();
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// GoToObject / GoToObjectAsync
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "GoToObject") == 0 || strcmp(name, "GoToObjectAsync") == 0) {
		stack->correctParams(1);
		ScValue *val = stack->pop();
		if (!val->isNative()) {
			script->runtimeError("actor.%s method accepts an entity reference only", name);
			stack->pushNULL();
			return STATUS_OK;
		}
		AdObject *obj = (AdObject *)val->getNative();
		if (!obj || obj->getType() != OBJECT_ENTITY) {
			script->runtimeError("actor.%s method accepts an entity reference only", name);
			stack->pushNULL();
			return STATUS_OK;
		}
		AdEntity *ent = (AdEntity *)obj;
		if (ent->getWalkToX() == 0 && ent->getWalkToY() == 0) {
			goTo(ent->_posX, ent->_posY);
		} else {
			goTo(ent->getWalkToX(), ent->getWalkToY(), ent->getWalkToDir());
		}
		if (strcmp(name, "GoToObjectAsync") != 0) {
			script->waitForExclusive(this);
		}
		stack->pushNULL();
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// TurnTo / TurnToAsync
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "TurnTo") == 0 || strcmp(name, "TurnToAsync") == 0) {
		stack->correctParams(1);
		int dir;
		ScValue *val = stack->pop();

		// turn to object?
		if (val->isNative() && _gameRef->validObject((BaseObject *)val->getNative())) {
			BaseObject *obj = (BaseObject *)val->getNative();
			int angle = (int)(atan2((double)(obj->_posY - _posY), (double)(obj->_posX - _posX)) * (180 / 3.14));
			dir = (int)angleToDirection(angle);
		}
		// otherwise turn to direction
		else {
			dir = val->getInt();
		}

		if (dir >= 0 && dir < NUM_DIRECTIONS) {
			turnTo((TDirection)dir);
			if (strcmp(name, "TurnToAsync") != 0) {
				script->waitForExclusive(this);
			}
		}
		stack->pushNULL();
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// IsWalking
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "IsWalking") == 0) {
		stack->correctParams(0);
		stack->pushBool(_state == STATE_FOLLOWING_PATH);
		return STATUS_OK;
	}

#ifdef ENABLE_FOXTAIL
	//////////////////////////////////////////////////////////////////////////
	// [FoxTail] StopWalking
	// Used to stop Leah in one scene only at rabbit_run.script in action()
	// Let's just call turnTo() for current direction to finalize movement
	// Return value is never used
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "StopWalking") == 0) {
		stack->correctParams(0);
		turnTo(_dir);
		stack->pushNULL();

		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// [FoxTail] SetSpeedWalkAnim
	// Used to set Leah speed at leah.script in SetSpeed()
	// Modifies walking animations interframe delays
	// Takes integer parameter:
	//     10 on state.ultra_super_mega_fast_walk cheat code
	//     40 on "Fast" settings
	//     70 on "Normal" settings
	//     90 on "Slow" settings
	// Return value is never used
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "SetSpeedWalkAnim") == 0) {
		stack->correctParams(1);
		int speedWalk = stack->pop()->getInt();
		for (uint32 dir = 0; dir < NUM_DIRECTIONS; dir++) {
			AdSpriteSet *anim = getAnimByName(_walkAnimName);
			if (anim != nullptr) {
				BaseSprite *item = anim->getSprite((TDirection)dir);
				if (item != nullptr) {
					for (uint32 i = 0; i < item->_frames.getSize(); i++) {
						BaseFrame *frame = item->_frames[i];
						if (frame != nullptr) {
							frame->_delay = speedWalk;
						}
					}
				}
			}
		}
		stack->pushNULL();

		return STATUS_OK;
	}
#endif

	//////////////////////////////////////////////////////////////////////////
	// MergeAnims
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "MergeAnims") == 0) {
		stack->correctParams(1);
		stack->pushBool(DID_SUCCEED(mergeAnims(stack->pop()->getString())));
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// UnloadAnim
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "UnloadAnim") == 0) {
		stack->correctParams(1);
		const char *animName = stack->pop()->getString();

		bool found = false;
		for (uint32 i = 0; i < _anims.getSize(); i++) {
			if (scumm_stricmp(_anims[i]->getName(), animName) == 0) {
				// invalidate sprites in use
				if (_anims[i]->containsSprite(_tempSprite2)) {
					_tempSprite2 = nullptr;
				}
				if (_anims[i]->containsSprite(_currentSprite)) {
					_currentSprite = nullptr;
				}
				if (_anims[i]->containsSprite(_animSprite2)) {
					_animSprite2 = nullptr;
				}

				delete _anims[i];
				_anims[i] = nullptr;
				_anims.removeAt(i);
				i--;
				found = true;
			}
		}
		stack->pushBool(found);
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// HasAnim
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "HasAnim") == 0) {
		stack->correctParams(1);
		const char *animName = stack->pop()->getString();
		stack->pushBool(getAnimByName(animName) != nullptr);
		return STATUS_OK;
	} else {
		return AdTalkHolder::scCallMethod(script, stack, thisStack, name);
	}
}


//////////////////////////////////////////////////////////////////////////
ScValue *AdActor::scGetProperty(const Common::String &name) {
	_scValue->setNULL();

	//////////////////////////////////////////////////////////////////////////
	// Direction
	//////////////////////////////////////////////////////////////////////////
	if (name == "Direction") {
		_scValue->setInt(_dir);
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// Type
	//////////////////////////////////////////////////////////////////////////
	else if (name == "Type") {
		_scValue->setString("actor");
		return _scValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// TalkAnimName
	//////////////////////////////////////////////////////////////////////////
	else if (name == "TalkAnimName") {
		_scValue->setString(_talkAnimName);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// WalkAnimName
	//////////////////////////////////////////////////////////////////////////
	else if (name == "WalkAnimName") {
		_scValue->setString(_walkAnimName);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// IdleAnimName
	//////////////////////////////////////////////////////////////////////////
	else if (name == "IdleAnimName") {
		_scValue->setString(_idleAnimName);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// TurnLeftAnimName
	//////////////////////////////////////////////////////////////////////////
	else if (name == "TurnLeftAnimName") {
		_scValue->setString(_turnLeftAnimName);
		return _scValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// TurnRightAnimName
	//////////////////////////////////////////////////////////////////////////
	else if (name == "TurnRightAnimName") {
		_scValue->setString(_turnRightAnimName);
		return _scValue;
	} else {
		return AdTalkHolder::scGetProperty(name);
	}
}


//////////////////////////////////////////////////////////////////////////
bool AdActor::scSetProperty(const char *name, ScValue *value) {
	//////////////////////////////////////////////////////////////////////////
	// Direction
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(name, "Direction") == 0) {
		int dir = value->getInt();
		if (dir >= 0 && dir < NUM_DIRECTIONS) {
			_dir = (TDirection)dir;
		}
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// TalkAnimName
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "TalkAnimName") == 0) {
		if (value->isNULL()) {
			_talkAnimName = "talk";
		} else {
			_talkAnimName = value->getString();
		}
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// WalkAnimName
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "WalkAnimName") == 0) {
		if (value->isNULL()) {
			_walkAnimName = "walk";
		} else {
			_walkAnimName = value->getString();
		}
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// IdleAnimName
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "IdleAnimName") == 0) {
		if (value->isNULL()) {
			_idleAnimName = "idle";
		} else {
			_idleAnimName = value->getString();
		}
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// TurnLeftAnimName
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "TurnLeftAnimName") == 0) {
		if (value->isNULL()) {
			_turnLeftAnimName = "turnleft";
		} else {
			_turnLeftAnimName = value->getString();
		}
		return STATUS_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// TurnRightAnimName
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(name, "TurnRightAnimName") == 0) {
		if (value->isNULL()) {
			_turnRightAnimName = "turnright";
		} else {
			_turnRightAnimName = value->getString();
		}
		return STATUS_OK;
	} else {
		return AdTalkHolder::scSetProperty(name, value);
	}
}


//////////////////////////////////////////////////////////////////////////
const char *AdActor::scToString() {
	return "[actor object]";
}


//////////////////////////////////////////////////////////////////////////
BaseSprite *AdActor::getTalkStance(const char *stance) {
	// forced stance?
	if (_forcedTalkAnimName && !_forcedTalkAnimUsed) {
		_forcedTalkAnimUsed = true;
		delete _animSprite;
		_animSprite = new BaseSprite(_gameRef, this);
		if (_animSprite) {
			bool res = _animSprite->loadFile(_forcedTalkAnimName);
			if (DID_FAIL(res)) {
				_gameRef->LOG(res, "AdActor::GetTalkStance: error loading talk sprite (object:\"%s\" sprite:\"%s\")", getName(), _forcedTalkAnimName);
				delete _animSprite;
				_animSprite = nullptr;
			} else {
				return _animSprite;
			}
		}
	}

	// old way
	if (_talkSprites.getSize() > 0 || _talkSpritesEx.getSize() > 0) {
		return getTalkStanceOld(stance);
	}

	// new way
	BaseSprite *ret = nullptr;

	// do we have an animation with this name?
	AdSpriteSet *anim = getAnimByName(stance);
	if (anim) {
		ret = anim->getSprite(_dir);
	}

	// not - get a random talk
	if (!ret) {
		BaseArray<AdSpriteSet *> talkAnims;
		for (uint32 i = 0; i < _anims.getSize(); i++) {
			if (_talkAnimName.compareToIgnoreCase(_anims[i]->getName()) == 0) {
				talkAnims.add(_anims[i]);
			}
		}

		if (talkAnims.getSize() > 0) {
			int rnd = BaseEngine::instance().randInt(0, talkAnims.getSize() - 1);
			ret = talkAnims[rnd]->getSprite(_dir);
		} else {
			if (_standSprite) {
				ret = _standSprite->getSprite(_dir);
			} else {
				anim = getAnimByName(_idleAnimName);
				if (anim) {
					ret = anim->getSprite(_dir);
				}
			}
		}
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////
BaseSprite *AdActor::getTalkStanceOld(const char *stance) {
	BaseSprite *ret = nullptr;

	if (stance != nullptr) {
		// search special stances
		for (uint32 i = 0; i < _talkSpritesEx.getSize(); i++) {
			if (scumm_stricmp(_talkSpritesEx[i]->getName(), stance) == 0) {
				ret = _talkSpritesEx[i]->getSprite(_dir);
				break;
			}
		}
		if (ret == nullptr) {
			// search generic stances
			for (uint32 i = 0; i < _talkSprites.getSize(); i++) {
				if (scumm_stricmp(_talkSprites[i]->getName(), stance) == 0) {
					ret = _talkSprites[i]->getSprite(_dir);
					break;
				}
			}
		}
	}

	// not a valid stance? get a random one
	if (ret == nullptr) {
		if (_talkSprites.getSize() < 1) {
			ret = _standSprite->getSprite(_dir);
		} else {
			// TODO: remember last
			int rnd = BaseEngine::instance().randInt(0, _talkSprites.getSize() - 1);
			ret = _talkSprites[rnd]->getSprite(_dir);
		}
	}

	return ret;
}

//////////////////////////////////////////////////////////////////////////
bool AdActor::persist(BasePersistenceManager *persistMgr) {
	AdTalkHolder::persist(persistMgr);

	persistMgr->transferSint32(TMEMBER_INT(_dir));
	persistMgr->transferPtr(TMEMBER_PTR(_path));
	persistMgr->transferSint32(TMEMBER(_pFCount));
	persistMgr->transferDouble(TMEMBER(_pFStepX));
	persistMgr->transferDouble(TMEMBER(_pFStepY));
	persistMgr->transferDouble(TMEMBER(_pFX));
	persistMgr->transferDouble(TMEMBER(_pFY));
	persistMgr->transferPtr(TMEMBER_PTR(_standSprite));
	_talkSprites.persist(persistMgr);
	_talkSpritesEx.persist(persistMgr);
	persistMgr->transferSint32(TMEMBER_INT(_targetDir));
	persistMgr->transferSint32(TMEMBER_INT(_afterWalkDir));
	persistMgr->transferPtr(TMEMBER_PTR(_targetPoint));
	persistMgr->transferPtr(TMEMBER_PTR(_turnLeftSprite));
	persistMgr->transferPtr(TMEMBER_PTR(_turnRightSprite));
	persistMgr->transferPtr(TMEMBER_PTR(_walkSprite));

	persistMgr->transferPtr(TMEMBER_PTR(_animSprite2));
	persistMgr->transferString(TMEMBER(_talkAnimName));
	persistMgr->transferString(TMEMBER(_idleAnimName));
	persistMgr->transferString(TMEMBER(_walkAnimName));
	persistMgr->transferString(TMEMBER(_turnLeftAnimName));
	persistMgr->transferString(TMEMBER(_turnRightAnimName));

	_anims.persist(persistMgr);

	return STATUS_OK;
}


//////////////////////////////////////////////////////////////////////////
TDirection AdActor::angleToDirection(int angle) {
	TDirection ret = DI_DOWN;

	if (angle > -112 && angle <= -67) {
		ret = DI_UP;
	} else if (angle > -67  && angle <= -22) {
		ret = DI_UPRIGHT;
	} else if (angle > -22  && angle <= 22) {
		ret = DI_RIGHT;
	} else if (angle > 22   && angle <= 67) {
		ret = DI_DOWNRIGHT;
	} else if (angle > 67   && angle <= 112) {
		ret = DI_DOWN;
	} else if (angle > 112  && angle <= 157) {
		ret = DI_DOWNLEFT;
	} else if ((angle > 157 && angle <= 180) || (angle >= -180 && angle <= -157)) {
		ret = DI_LEFT;
	} else if (angle > -157 && angle <= -112) {
		ret = DI_UPLEFT;
	}

	return ret;
}


//////////////////////////////////////////////////////////////////////////
int32 AdActor::getHeight() {
	// if no current sprite is set, set some
	if (_currentSprite == nullptr) {
		if (_standSprite) {
			_currentSprite = _standSprite->getSprite(_dir);
		} else {
			AdSpriteSet *anim = getAnimByName(_idleAnimName);
			if (anim) {
				_currentSprite = anim->getSprite(_dir);
			}
		}
	}
	// and get height
	return AdTalkHolder::getHeight();
}


//////////////////////////////////////////////////////////////////////////
AdSpriteSet *AdActor::getAnimByName(const Common::String &animName) {
	if (animName.empty())
		return nullptr;

	for (uint32 i = 0; i < _anims.getSize(); i++) {
		if (animName.compareToIgnoreCase(_anims[i]->getName()) == 0) {
			return _anims[i];
		}
	}
	return nullptr;
}

//////////////////////////////////////////////////////////////////////////
bool AdActor::mergeAnims(const char *animsFilename) {
	TOKEN_TABLE_START(commands)
	TOKEN_TABLE(ANIMATION)
	TOKEN_TABLE_END


	char *fileBuffer = (char *)BaseFileManager::getEngineInstance()->readWholeFile(animsFilename);
	if (fileBuffer == nullptr) {
		_gameRef->LOG(0, "AdActor::MergeAnims failed for file '%s'", animsFilename);
		return STATUS_FAILED;
	}

	char *buffer = fileBuffer;
	char *params;
	int cmd;
	BaseParser parser;

	bool ret = STATUS_OK;

	while ((cmd = parser.getCommand(&buffer, commands, &params)) > 0) {
		switch (cmd) {
		case TOKEN_ANIMATION: {
			AdSpriteSet *anim = new AdSpriteSet(_gameRef, this);
			if (!anim || DID_FAIL(anim->loadBuffer(params, false))) {
				cmd = PARSERR_GENERIC;
				ret = STATUS_FAILED;
			} else {
				_anims.add(anim);
			}
		}
		break;

		default:
			break;
		}
	}
	delete[] fileBuffer;
	return ret;
}

//////////////////////////////////////////////////////////////////////////
bool AdActor::playAnim(const char *filename) {
	// if we have an anim with this name, use it
	AdSpriteSet *anim = getAnimByName(filename);
	if (anim) {
		_animSprite2 = anim->getSprite(_dir);
		if (_animSprite2) {
			_animSprite2->reset();
			_state = STATE_PLAYING_ANIM_SET;
			return STATUS_OK;
		}
	}
	// otherwise call the standard handler
	return AdTalkHolder::playAnim(filename);
}

} // End of namespace Wintermute
