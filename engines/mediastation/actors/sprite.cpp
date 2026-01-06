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

#include "mediastation/actors/sprite.h"
#include "mediastation/debugchannels.h"
#include "mediastation/mediastation.h"

namespace MediaStation {

SpriteFrameHeader::SpriteFrameHeader(Chunk &chunk) : BitmapHeader(chunk) {
	_index = chunk.readTypedUint16();
	_offset = chunk.readTypedPoint();
}

Common::String SpriteMovieClip::getDebugString() const {
	return Common::String::format("%s: [%d, %d]", g_engine->formatParamTokenName(id).c_str(), firstFrameIndex, lastFrameIndex);
}

SpriteFrame::SpriteFrame(Chunk &chunk, SpriteFrameHeader *header) : Bitmap(chunk, header) {
	_bitmapHeader = header;
}

SpriteFrame::~SpriteFrame() {
	// The base class destructor takes care of deleting the bitmap header.
}

uint32 SpriteFrame::left() {
	return _bitmapHeader->_offset.x;
}

uint32 SpriteFrame::top() {
	return _bitmapHeader->_offset.y;
}

Common::Point SpriteFrame::topLeft() {
	return Common::Point(left(), top());
}

Common::Rect SpriteFrame::boundingBox() {
	return Common::Rect(topLeft(), width(), height());
}

uint32 SpriteFrame::index() {
	return _bitmapHeader->_index;
}

SpriteAsset::~SpriteAsset() {
	for (SpriteFrame *frame : frames) {
		delete frame;
	}
}

SpriteMovieActor::~SpriteMovieActor() {
	unregisterWithStreamManager();
}

void SpriteMovieActor::readParameter(Chunk &chunk, ActorHeaderSectionType paramType) {
	switch (paramType) {
	case kActorHeaderChannelIdent:
		_channelIdent = chunk.readTypedChannelIdent();
		registerWithStreamManager();
		_asset = Common::SharedPtr<SpriteAsset>(new SpriteAsset);
		break;

	case kActorHeaderFrameRate:
		_frameRate = static_cast<uint32>(chunk.readTypedDouble());
		break;

	case kActorHeaderLoadType:
		_loadType = chunk.readTypedByte();
		break;

	case kActorHeaderStartup:
		_isVisible = static_cast<bool>(chunk.readTypedByte());
		break;

	case kActorHeaderSpriteChunkCount:
		_asset->_frameCount = chunk.readTypedUint16();
		break;

	case kActorHeaderSpriteClip: {
		SpriteMovieClip clip;
		clip.id = chunk.readTypedUint16();
		clip.firstFrameIndex = chunk.readTypedUint16();
		clip.lastFrameIndex = chunk.readTypedUint16();
		_clips.setVal(clip.id, clip);
		break;
	}

	case kActorHeaderDefaultSpriteClip:
		_defaultClipId = chunk.readTypedUint16();
		break;

	case kActorHeaderActorReference: {
		_actorReference = chunk.readTypedUint16();
		SpriteMovieActor *referencedSprite = static_cast<SpriteMovieActor *>(g_engine->getActorByIdAndType(_actorReference, kActorTypeSprite));
		_asset = referencedSprite->_asset;
		break;
	}

	default:
		SpatialEntity::readParameter(chunk, paramType);
	}
}

void SpriteMovieActor::loadIsComplete() {
	// This clip goes forward through all the sprite's frames.
	SpriteMovieClip forwardClip(DEFAULT_FORWARD_CLIP_ID, 0, _asset->_frameCount - 1);
	if (!_clips.contains(DEFAULT_FORWARD_CLIP_ID)) {
		_clips.setVal(forwardClip.id, forwardClip);
	}

	// This clip goes backward through all the sprite's frames.
	SpriteMovieClip backwardClip(DEFAULT_BACKWARD_CLIP_ID, _asset->_frameCount - 1, 0);
	if (!_clips.contains(DEFAULT_BACKWARD_CLIP_ID)) {
		_clips.setVal(backwardClip.id, backwardClip);
	}

	SpatialEntity::loadIsComplete();
	setCurrentClip(_defaultClipId);
}

ScriptValue SpriteMovieActor::callMethod(BuiltInMethod methodId, Common::Array<ScriptValue> &args) {
	ScriptValue returnValue;

	switch (methodId) {
	case kSpatialShowMethod: {
		ARGCOUNTCHECK(0);
		setVisibility(true);
		return returnValue;
	}

	case kSpatialHideMethod: {
		ARGCOUNTCHECK(0);
		setVisibility(false);
		return returnValue;
	}

	case kTimePlayMethod: {
		ARGCOUNTCHECK(0);
		play();
		return returnValue;
	}

	case kTimeStopMethod: {
		ARGCOUNTCHECK(0);
		stop();
		return returnValue;
	}

	case kMovieResetMethod: {
		ARGCOUNTCHECK(0);
		setCurrentFrameToInitial();
		return returnValue;
	}

	case kSetCurrentClipMethod: {
		ARGCOUNTRANGE(0, 1);
		uint clipId = DEFAULT_FORWARD_CLIP_ID;
		if (args.size() == 1) {
			clipId = args[0].asParamToken();
		}
		setCurrentClip(clipId);
		return returnValue;
	}

	case kIncrementFrameMethod: {
		ARGCOUNTRANGE(0, 1);
		bool loopAround = false;
		if (args.size() == 1) {
			loopAround = args[0].asBool();
		}

		bool moreFrames = activateNextFrame();
		if (!moreFrames) {
			if (loopAround) {
				setCurrentFrameToInitial();
			}
		}
		return returnValue;
	}

	case kDecrementFrameMethod: {
		bool shouldSetCurrentFrameToFinal = false;
		if (args.size() == 1) {
			shouldSetCurrentFrameToFinal = args[0].asBool();
		}

		bool moreFrames = activatePreviousFrame();
		if (!moreFrames) {
			if (shouldSetCurrentFrameToFinal) {
				setCurrentFrameToFinal();
			}
		}
		return returnValue;
	}

	case kGetCurrentClipIdMethod: {
		returnValue.setToParamToken(_activeClip.id);
		return returnValue;
	}

	case kIsPlayingMethod: {
		returnValue.setToBool(_isPlaying);
		return returnValue;
	}

	default:
		return SpatialEntity::callMethod(methodId, args);
	}
}

bool SpriteMovieActor::activateNextFrame() {
	bool clipMovesForward = _activeClip.firstFrameIndex <= _activeClip.lastFrameIndex;
	if (clipMovesForward) {
		debugC(3, kDebugSpriteMovie, "[%s] %s: FORWARD: currentFrameIndex: %d; activeClip.lastFrameIndex: %d",
			debugName(), __func__, _currentFrameIndex, _activeClip.lastFrameIndex);

		bool canMoveForward = _currentFrameIndex < _activeClip.lastFrameIndex;
		if (canMoveForward) {
			dirtyIfVisible();
			_currentFrameIndex++;
			dirtyIfVisible();
			return true;
		}

	} else {
		debugC(3, kDebugSpriteMovie, "[%s] %s: BACKWARD: currentFrameIndex: %d; activeClip.lastFrameIndex: %d",
			debugName(), __func__, _currentFrameIndex, _activeClip.lastFrameIndex);

		bool canMoveBackward = _currentFrameIndex > _activeClip.lastFrameIndex;
		if (canMoveBackward) {
			dirtyIfVisible();
			_currentFrameIndex--;
			dirtyIfVisible();
			return true;
		}
	}
	return false;
}

bool SpriteMovieActor::activatePreviousFrame() {
	bool clipMovesBackward = _activeClip.lastFrameIndex < _activeClip.firstFrameIndex;
	if (clipMovesBackward) {
		bool canMoveTowardFirst = _currentFrameIndex < _activeClip.firstFrameIndex;
		if (canMoveTowardFirst) {
			dirtyIfVisible();
			_currentFrameIndex++;
			dirtyIfVisible();
			return true;
		}

	} else {
		bool canMoveTowardFirst = _activeClip.firstFrameIndex < _currentFrameIndex;
		if (canMoveTowardFirst) {
			dirtyIfVisible();
			_currentFrameIndex--;
			dirtyIfVisible();
			return true;
		}
	}
	return false;
}

void SpriteMovieActor::dirtyIfVisible() {
	if (_isVisible) {
		invalidateLocalBounds();
	}
}

void SpriteMovieActor::setVisibility(bool visibility) {
	if (_isVisible != visibility) {
		_isVisible = visibility;
		invalidateLocalBounds();
	}
}

void SpriteMovieActor::play() {
	_isPlaying = true;
	_startTime = g_system->getMillis();
	_lastProcessedTime = 0;
	_nextFrameTime = 0;

	scheduleNextFrame();
	debugC(3, kDebugSpriteMovie, "[%s] %s", debugName(), __func__);
}

void SpriteMovieActor::stop() {
	_nextFrameTime = 0;
	_isPlaying = false;
	debugC(3, kDebugSpriteMovie, "[%s] %s", debugName(), __func__);
}

void SpriteMovieActor::setCurrentClip(uint clipId) {
	if (_activeClip.id != clipId) {
		if (_clips.contains(clipId)) {
			SpriteMovieClip newClip = _clips.getVal(clipId);
			debugC(3, kDebugSpriteMovie, "[%s] %s: (frameCount: %d) activeClip: %s; newClip: %s",
				debugName(), __func__, _asset->_frameCount, _activeClip.getDebugString().c_str(), newClip.getDebugString().c_str());
			_activeClip = _clips.getVal(clipId);
		} else {
			_activeClip.id = clipId;
			warning("[%s] %s: Clip %s not found", debugName(), __func__, _activeClip.getDebugString().c_str());
		}

		setCurrentFrameToInitial();
	}
}

void SpriteMovieActor::setCurrentFrameToInitial() {
	debugC(3, kDebugSpriteMovie, "[%s] %s: currentFrameIndex: %d, activeClip.firstFrameIndex: %d",
		debugName(), __func__, _currentFrameIndex, _activeClip.firstFrameIndex);
	if (_currentFrameIndex != _activeClip.firstFrameIndex) {
		dirtyIfVisible();
		_currentFrameIndex = _activeClip.firstFrameIndex;
		dirtyIfVisible();
	}
}

void SpriteMovieActor::setCurrentFrameToFinal() {
	debugC(3, kDebugSpriteMovie, "[%s] %s: currentFrameIndex: %d, activeClip.lastFrameIndex: %d",
		debugName(), __func__, _currentFrameIndex, _activeClip.lastFrameIndex);
	if (_currentFrameIndex != _activeClip.lastFrameIndex) {
		dirtyIfVisible();
		_currentFrameIndex = _activeClip.lastFrameIndex;
		dirtyIfVisible();
	}
}

void SpriteMovieActor::process() {
	updateFrameState();
	// Sprites don't have time event handlers, separate timers do time handling.
}

void SpriteMovieActor::readChunk(Chunk &chunk) {
	// Reads one frame from the sprite.
	SpriteFrameHeader *header = new SpriteFrameHeader(chunk);
	SpriteFrame *frame = new SpriteFrame(chunk, header);
	_asset->frames.push_back(frame);

	// TODO: Are these in exactly reverse order? If we can just reverse the
	// whole thing once.
	Common::sort(_asset->frames.begin(), _asset->frames.end(), [](SpriteFrame *a, SpriteFrame *b) {
		return a->index() < b->index();
	});
}

void SpriteMovieActor::scheduleNextFrame() {
	if (!_isPlaying) {
		return;
	}

	debugC(3, kDebugSpriteMovie, "[%s] %s: currentFrame: %d; activeClip: [%d, %d]",
		debugName(), __func__, _currentFrameIndex,
		_activeClip.firstFrameIndex, _activeClip.lastFrameIndex);
	int firstFrameIndex = _activeClip.firstFrameIndex;
	int lastFrameIndex = _activeClip.lastFrameIndex;

	// For backward clips, we've "passed" the last frame when currentFrameIndex <= lastFrameIndex.
	bool clipMovesBackward = lastFrameIndex < firstFrameIndex;
	bool currentIsAtOrBeyondLast = lastFrameIndex <= _currentFrameIndex;
	bool needsStopEvaluation = clipMovesBackward || currentIsAtOrBeyondLast;
	bool backwardClipContinues = clipMovesBackward && currentIsAtOrBeyondLast;

	if (needsStopEvaluation) {
		if (backwardClipContinues) {
			// Backward clip still has frames to show (current > last).
			scheduleNextTimerEvent();
		} else {
			// We reached the end of the clip, regardless of which direction we were moving.
			stop();
		}
	} else {
		// The forward clip still in progress.
		scheduleNextTimerEvent();
	}
}

void SpriteMovieActor::scheduleNextTimerEvent() {
	uint frameDuration = 1000 / _frameRate;
	_nextFrameTime += frameDuration;
	debugC(3, kDebugSpriteMovie, "[%s] %s", debugName(), __func__);
}

void SpriteMovieActor::updateFrameState() {
	if (!_isPlaying) {
		return;
	}

	uint currentTime = g_system->getMillis() - _startTime;
	bool drawNextFrame = currentTime >= _nextFrameTime;
	debugC(3, kDebugSpriteMovie, "[%s] %s: nextFrameTime: %d; startTime: %d, currentTime: %d",
		debugName(), __func__, _nextFrameTime, _startTime, currentTime);
	if (drawNextFrame) {
		timerEvent();
	}
}

void SpriteMovieActor::timerEvent() {
	if (!_isPlaying) {
		warning("[%s] %s: Not playing", debugName(), __func__);
		return;
	}

	bool moreFramesToShow = activateNextFrame();
	if (moreFramesToShow) {
		postMovieEndEventIfNecessary();
		scheduleNextFrame();
	} else {
		stop();
	}
}

void SpriteMovieActor::postMovieEndEventIfNecessary() {
	if (_currentFrameIndex != _activeClip.lastFrameIndex) {
		return;
	}

	_isPlaying = false;
	_startTime = 0;
	_nextFrameTime = 0;
	debugC(3, kDebugSpriteMovie, "[%s] %s: Posting movie end", debugName(), __func__);

	ScriptValue value;
	value.setToParamToken(_activeClip.id);
	runEventHandlerIfExists(kSpriteMovieEndEvent, value);
}

void SpriteMovieActor::draw(DisplayContext &displayContext) {
	if (static_cast<uint>(_currentFrameIndex) >= _asset->frames.size()) {
		warning("[%s] %s: Requested frame %d, but we only have %d frames. Showing last frame",
			debugName(), __func__, _currentFrameIndex, _asset->frames.size());
		_currentFrameIndex = _asset->frames.size() - 1;
	}

	SpriteFrame *activeFrame = _asset->frames[_currentFrameIndex];
	if (_isVisible) {
		Common::Rect frameBbox = activeFrame->boundingBox();
		frameBbox.translate(_boundingBox.left, _boundingBox.top);
		debugC(8, kDebugSpriteMovie, "[%s] %s: frame %d",
			debugName(), __func__, _currentFrameIndex);
		g_engine->getDisplayManager()->imageBlit(frameBbox.origin(), activeFrame, _dissolveFactor, &displayContext);
	}
}

} // End of namespace MediaStation
