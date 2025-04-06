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

#include "fitd/detection.h"
#include "fitd/debugtools.h"
#include "fitd/gfx.h"
#include "fitd/room.h"
#include "backends/imgui/imgui.h"
#include "common/debug.h"

namespace Fitd {

typedef struct ImGuiState {
	bool showCamera = true;
} ImGuiState;

ImGuiState *_state = nullptr;

static void InputS16(const char *name, int16 *value) {
	ImGui::InputScalar(name, ImGuiDataType_S16, value);
}

static void drawCamera() {
	if (!_state->showCamera)
		return;

	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Camera", &_state->showCamera)) {
		ImGui::Text("timer: %u", timer);

		cameraDataStruct *pCamera = cameraDataTable[currentCamera];
		if (pCamera) {
			ImGui::BeginGroup();
			ImGui::PushID("Position");
			ImGui::InputInt("X", &translateX);
			ImGui::InputInt("Y", &translateY);
			ImGui::InputInt("Z", &translateZ);
			ImGui::PopID();
			ImGui::EndGroup();

			ImGui::BeginGroup();
			ImGui::PushID("Center");
			InputS16("Pitch", &pCamera->alpha);
			InputS16("Yaw", &pCamera->beta);
			InputS16("Roll", &pCamera->gamma);
			ImGui::PopID();
			ImGui::EndGroup();

			setAngleCamera(pCamera->alpha, pCamera->beta, pCamera->gamma);

			ImGui::BeginGroup();
			ImGui::PushID("Center");
			ImGui::InputInt("HCenter", &cameraCenterX);
			ImGui::InputInt("VCenter", &cameraCenterY);
			ImGui::PopID();
			ImGui::EndGroup();

			ImGui::BeginGroup();
			ImGui::PushID("Projection");
			ImGui::InputInt("Perspective", &cameraPerspective);
			ImGui::InputInt("XFov", &cameraFovX);
			ImGui::InputInt("YFov", &cameraFovY);
			ImGui::PopID();
			ImGui::EndGroup();
		}
	}
	ImGui::End();
}

static void drawObjects() {
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("World objects")) {

		if (ListWorldObjets.size()) {
			static int selectedWorldObject = 0;
			ImGui::InputInt("Index", &selectedWorldObject);

			ImGui::Separator();

			if (selectedWorldObject > ListWorldObjets.size())
				selectedWorldObject = ListWorldObjets.size() - 1;

			tWorldObject *pWorldObject = &ListWorldObjets[selectedWorldObject];

			if (pWorldObject) {
				InputS16("objectIndex", &pWorldObject->objIndex);
				InputS16("body", &pWorldObject->body);
				InputS16("flags", &pWorldObject->flags);
				InputS16("typeZV", &pWorldObject->typeZV);
				InputS16("foundBody", &pWorldObject->foundBody);
				InputS16("foundName", &pWorldObject->foundName);
				InputS16("flags2", &pWorldObject->flags2);
				InputS16("foundLife", &pWorldObject->foundLife);
				InputS16("x", &pWorldObject->x);
				InputS16("y", &pWorldObject->y);
				InputS16("z", &pWorldObject->z);
				InputS16("alpha", &pWorldObject->alpha);
				InputS16("beta", &pWorldObject->beta);
				InputS16("gamma", &pWorldObject->gamma);
				InputS16("stage", &pWorldObject->stage);
				InputS16("room", &pWorldObject->room);
				InputS16("lifeMode", &pWorldObject->lifeMode);
				InputS16("life", &pWorldObject->life);
				InputS16("floorLife", &pWorldObject->floorLife);
				InputS16("anim", &pWorldObject->anim);
				InputS16("frame", &pWorldObject->frame);
				InputS16("animType", &pWorldObject->animType);
				InputS16("animInfo", &pWorldObject->animInfo);
				InputS16("trackMode", &pWorldObject->trackMode);
				InputS16("trackNumber", &pWorldObject->trackNumber);
				InputS16("positionInTrack", &pWorldObject->positionInTrack);
				InputS16("mark", &pWorldObject->mark);
			}
		}
	}
	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	ImGui::Begin("Active objects");

	if (ListWorldObjets.size()) {
		static int selectedObject = 0;
		ImGui::InputInt("Index", &selectedObject);

		// ImGui::Separator();

		if (selectedObject > NUM_MAX_OBJECT)
			selectedObject = NUM_MAX_OBJECT - 1;

		tObject *pObject = &objectTable[selectedObject];

		ImGui::PushItemWidth(100);

		InputS16("world index", &pObject->indexInWorld);
		ImGui::SameLine();
		InputS16("bodyNum", &pObject->bodyNum);
		//            InputS16("_flags", &pObject->_flags);
		InputS16("dynFlags", &pObject->dynFlags);
		// ZVStruct zv;
		InputS16("screenXMin", &pObject->screenXMin);
		ImGui::SameLine();
		InputS16("screenYMin", &pObject->screenYMin);
		ImGui::SameLine();
		InputS16("screenXMax", &pObject->screenXMax);
		ImGui::SameLine();
		InputS16("screenYMax", &pObject->screenYMax);

		InputS16("roomX", &pObject->roomX);
		ImGui::SameLine();
		InputS16("roomY", &pObject->roomY);
		ImGui::SameLine();
		InputS16("roomZ", &pObject->roomZ);
		ImGui::SameLine();

		InputS16("worldX", &pObject->worldX);
		InputS16("worldY", &pObject->worldY);
		InputS16("worldZ", &pObject->worldZ);

		InputS16("alpha", &pObject->alpha);
		InputS16("beta", &pObject->beta);
		InputS16("gamma", &pObject->gamma);

		InputS16("stage", &pObject->stage);
		InputS16("room", &pObject->room);

		InputS16("lifeMode", &pObject->lifeMode);
		InputS16("life", &pObject->life);
		// unsigned int CHRONO;
		// unsigned int ROOM_CHRONO;
		InputS16("ANIM", &pObject->ANIM);
		InputS16("animType", &pObject->animType);
		InputS16("animInfo", &pObject->animInfo);
		InputS16("newAnim", &pObject->newAnim);
		InputS16("newAnimType", &pObject->newAnimType);
		InputS16("newAnimInfo", &pObject->newAnimInfo);
		InputS16("FRAME", &pObject->FRAME);
		InputS16("numOfFrames", &pObject->numOfFrames);
		InputS16("END_FRAME", &pObject->END_FRAME);
		InputS16("END_ANIM", &pObject->END_ANIM);
		InputS16("trackMode", &pObject->trackMode);
		InputS16("trackNumber", &pObject->trackNumber);
		InputS16("MARK", &pObject->MARK);
		InputS16("positionInTrack", &pObject->positionInTrack);

		InputS16("stepX", &pObject->stepX);
		InputS16("stepY", &pObject->stepY);
		InputS16("stepZ", &pObject->stepZ);

		InputS16("animNegX", &pObject->animNegX);
		InputS16("animNegY", &pObject->animNegY);
		InputS16("animNegZ", &pObject->animNegZ);

		// interpolatedValue YHandler;
		InputS16("falling", &pObject->falling);
		// interpolatedValue rotate;
		InputS16("direction", &pObject->direction);
		InputS16("speed", &pObject->speed);
		// interpolatedValue speedChange;
		// s16 COL[3];
		InputS16("COL_BY", &pObject->COL_BY);
		InputS16("HARD_DEC", &pObject->HARD_DEC);
		InputS16("HARD_COL", &pObject->HARD_COL);
		InputS16("HIT", &pObject->HIT);
		InputS16("HIT_BY", &pObject->HIT_BY);
		InputS16("animActionType", &pObject->animActionType);
		InputS16("animActionANIM", &pObject->animActionANIM);
		InputS16("animActionFRAME", &pObject->animActionFRAME);
		InputS16("animActionParam", &pObject->animActionParam);
		InputS16("hitForce", &pObject->hitForce);
		InputS16("hotPointID", &pObject->hotPointID);
		// point3dStruct hotPoint;
		InputS16("hardMat", &pObject->hardMat);

		ImGui::PopItemWidth();
	}

	ImGui::End();
}

void onImGuiInit() {
	_state = new ImGuiState();
}

void onImGuiRender() {
	if (!debugChannelSet(-1, kDebugConsole)) {
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange | ImGuiConfigFlags_NoMouse;
		return;
	}

	ImGui::GetIO().ConfigFlags &= ~(ImGuiConfigFlags_NoMouseCursorChange | ImGuiConfigFlags_NoMouse);
	drawCamera();
	drawObjects();
}

void onImGuiCleanup() {
	delete _state;
	_state = nullptr;
}
} // namespace Fitd
