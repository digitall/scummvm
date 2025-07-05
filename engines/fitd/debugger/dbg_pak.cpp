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

#include "backends/imgui/imgui.h"
#include "common/archive.h"
#include "common/array.h"
#include "common/config-manager.h"
#include "common/file.h"
#include "common/fs.h"
#include "common/system.h"
#include "fitd/common.h"
#include "fitd/pak.h"
#include "graphics/managed_surface.h"
#include "graphics/pixelformat.h"

namespace Fitd {

typedef struct PakInfoStruct // warning: alignment unsafe
{
	int32 discSize;
	int32 uncompressedSize;
	byte compressionFlag;
	byte info5;
	int16 offset;
} pakInfoStruct;

class State {
public:
	Common::StringArray members;
	Common::String member;
	Common::Array<pakInfoStruct> infos;
	uint selectedRes = UINT_MAX;
	void *selectedTexture = nullptr;
};

static State *_state = nullptr;

static void readPakInfo(pakInfoStruct *pPakInfo, Common::File &f) {
	pPakInfo->discSize = f.readSint32LE();
	pPakInfo->uncompressedSize = f.readSint32LE();
	pPakInfo->compressionFlag = f.readByte();
	pPakInfo->info5 = f.readByte();
	pPakInfo->offset = f.readSint16LE();
}

static Common::String toHumanReadableBytes(uint64 size) {
	const char *units;
	const Common::String text(Common::getHumanReadableBytes(size, units));
	return Common::String::format("%s %s", text.c_str(), units);
}

static void refreshPAK(const char *name) {
	_state->infos.clear();

	Common::File f;
	if (!f.open(name)) {
		error("Fitd::refreshPAK: can't open %s", name);
	}
	f.readUint32LE();
	uint32 fileOffset = f.readUint32LE();
	const uint32 numFiles = fileOffset / 4 - 2;

	for (uint32 i = 0; i < numFiles; ++i) {
		const uint32 idOffset = (i + 1) * 4;
		f.seek(idOffset, SEEK_SET);
		fileOffset = f.readUint32LE();
		f.seek(fileOffset, SEEK_SET);
		const uint32 additionalDescriptorSize = f.readUint32LE();
		if (additionalDescriptorSize) {
			f.seek(additionalDescriptorSize - 4, SEEK_CUR);
		}
		pakInfoStruct pakInfo;
		readPakInfo(&pakInfo, f);
		_state->infos.push_back(pakInfo);
	}
}

static void selectRes(int resIndex) {
	// it should be an image
	if (_state->infos[resIndex].uncompressedSize == 64000) {
		ScopedPtr pal(pakLoad("ITD_RESS.PAK", 3));
		ScopedPtr selectedResData(pakLoad(_state->member.c_str(), resIndex));
		const Graphics::PixelFormat format = Graphics::PixelFormat::createFormatCLUT8();
		Graphics::ManagedSurface *s = new Graphics::ManagedSurface(320, 200, format);
		s->setPalette(pal.get(), 0, 256);
		byte *dst = static_cast<byte *>(s->getBasePtr(0, 0));
		memcpy(dst, selectedResData.get(), 64000);
		_state->selectedTexture = g_system->getImGuiTexture(*s, pal.get(), 256);
		delete s;
	}
}

void debugPakInit() {
	_state = new State();
	const Common::Path &path = ConfMan.getPath("path");
	Common::FSDirectory gameRoot(path);
	Common::ArchiveMemberList members;
	gameRoot.listMatchingMembers(members, "*.PAK");
	for (auto it = members.begin(); it != members.end(); ++it) {
		_state->members.push_back((*it)->getName());
	}
}

void debugPakCleanup() {
	delete _state;
	_state = nullptr;
}

void debugPakDraw() {
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("PAKS")) {
		for (auto it = _state->members.begin(); it != _state->members.end(); ++it) {
			const bool isSelected = *it == _state->member;
			if (ImGui::Selectable(it->c_str(), isSelected)) {
				if (_state->member != *it) {
					refreshPAK(it->c_str());
				}
				_state->member = *it;
			}
		}
	}
	ImGui::End();

	// Resources
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Resources")) {
		if (!_state->infos.empty()) {
			if (ImGui::BeginTable("Entries", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg)) {
				ImGui::TableSetupColumn("Compressed Size");
				ImGui::TableSetupColumn("Uncompressed Size");
				ImGui::TableSetupColumn("Compression");
				ImGui::TableSetupColumn("Offset");
				ImGui::TableHeadersRow();

				static const char *compressions[] = {"None", "Explode", "??", "??", "Deflate"};
				for (uint i = 0; i < _state->infos.size(); ++i) {
					ImGui::PushID(i);
					ImGui::TableNextColumn();
					Common::String sizeText(toHumanReadableBytes(_state->infos[i].discSize));

					if (ImGui::Selectable(sizeText.c_str(), _state->selectedRes == i, ImGuiSelectableFlags_SpanAllColumns)) {
						if (_state->selectedRes != i) {
							selectRes(i);
						}
						_state->selectedRes = i;
					}
					ImGui::TableNextColumn();
					sizeText = toHumanReadableBytes(_state->infos[i].uncompressedSize);
					ImGui::Text("%s", sizeText.c_str());
					ImGui::TableNextColumn();
					ImGui::Text("%d %s", _state->infos[i].compressionFlag, compressions[_state->infos[i].compressionFlag]);
					ImGui::TableNextColumn();
					ImGui::Text("%d", _state->infos[i].offset);
					ImGui::PopID();
				}
				ImGui::EndTable();
			}
		}
	}
	ImGui::End();

	// Preview
	ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Preview")) {
		if (_state->selectedTexture) {
			ImGui::Image(_state->selectedTexture, ImVec2(320, 200));
		} else {
			ImGui::Text("No preview available, select a resource first.");
		}
	}
	ImGui::End();
}

} // namespace Fitd
