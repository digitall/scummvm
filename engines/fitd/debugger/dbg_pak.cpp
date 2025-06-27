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
 #include "backends/imgui/imgui.h"
 #include "common/archive.h"
 #include "common/array.h"
 #include "common/config-manager.h"
 #include "common/file.h"
 #include "common/fs.h"
 #include "common/system.h"
 #include "graphics/pixelformat.h"
 #include "graphics/surface.h"
 #include "graphics/managed_surface.h"
 #include "fitd/debugger/dbg_utils.h"
 #include "fitd/debugger/dbg_vars.h"
 #include "fitd/fitd.h"
 #include "fitd/pak.h"
 #include "fitd/vars.h"

 namespace Fitd {

 typedef struct PakInfoStruct // warning: allignment unsafe
 {
	 int32 discSize;
	 int32 uncompressedSize;
	 byte compressionFlag;
	 byte info5;
	 int16 offset;
 } pakInfoStruct;

 static Common::ArchiveMemberList _members;
 static Common::String _member;
 static Common::Array<pakInfoStruct> _infos;
 static int _selectedRes = -1;
 static void *_selectedTexture = nullptr;

 static void readPakInfo(pakInfoStruct *pPakInfo, Common::File &f) {
	 pPakInfo->discSize = f.readSint32LE();
	 pPakInfo->uncompressedSize = f.readSint32LE();
	 pPakInfo->compressionFlag = f.readByte();
	 pPakInfo->info5 = f.readByte();
	 pPakInfo->offset = f.readSint16LE();
 }

 static Common::String toHumanReadableBytes(uint64 size) {
	 const char *units;
	 Common::String text(Common::getHumanReadableBytes(size, units));
	 return Common::String::format("%s %s", text.c_str(), units);
 }

 static void refreshPAK(const char* name) {
	 _infos.clear();

	 Common::File f;
	 f.open(name);
	 f.readUint32LE();
	 uint32 fileOffset = f.readUint32LE();
	 uint32 numFiles = fileOffset / 4 - 2;

	 for (uint32 i = 0; i < numFiles; ++i) {
		 uint32 idOffset = (i + 1) * 4;
		 f.seek(idOffset, SEEK_SET);
		 fileOffset = f.readUint32LE();
		 f.seek(fileOffset, SEEK_SET);
		 uint32 additionalDescriptorSize = f.readUint32LE();
		 if (additionalDescriptorSize) {
			 f.seek(additionalDescriptorSize - 4, SEEK_CUR);
		 }
		 pakInfoStruct pakInfo;
		 readPakInfo(&pakInfo, f);
		 _infos.push_back(pakInfo);
	 }
 }

 static void selectRes(int resIndex) {
	 // it should be an image
	 if(_infos[resIndex].uncompressedSize == 64000) {
		 byte *pal = (byte *)pakLoad("ITD_RESS.PAK", 3);
		 char *selectedResData = pakLoad(_member.c_str(), resIndex);
		 Graphics::PixelFormat format = Graphics::PixelFormat::createFormatCLUT8();
		 Graphics::ManagedSurface *s = new Graphics::ManagedSurface(320, 200, format);
		 s->setPalette(pal, 0, 256);
		 char *dst = (char *)s->getBasePtr(0, 0);
		 memcpy(dst, selectedResData, 64000);
		 _selectedTexture = g_system->getImGuiTexture(*s, pal, 256);
		 free(pal);
		 free(selectedResData);
		 delete s;
	 }
 }

 void drawPak() {
	 ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	 if (ImGui::Begin("PAKS")) {
		 // first time: get all PAK files
		 if (_members.empty()) {
			 const Common::Path &path = ConfMan.getPath("path");
			 Common::FSDirectory gameRoot(path);

			 gameRoot.listMatchingMembers(_members, "*.PAK");
		 }

		 for (auto it = _members.begin(); it != _members.end(); ++it) {
			 Common::String name((*it)->getName().c_str());
			 bool isSelected = name == _member;
			 if (ImGui::Selectable(name.c_str(), isSelected)) {
				 if (_member != name) {
					 refreshPAK(name.c_str());
				 }
				 _member = name;
			 }
		 }
	 }
	 ImGui::End();

	 // Resources
	 ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	 if (ImGui::Begin("Resources")) {
		 if (!_infos.empty()) {
			 if (ImGui::BeginTable("Entries", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg)) {
				 ImGui::TableSetupColumn("Compressed Size");
				 ImGui::TableSetupColumn("Uncompressed Size");
				 ImGui::TableSetupColumn("Compression");
				 ImGui::TableSetupColumn("Offset");
				 ImGui::TableHeadersRow();

				 static const char *compressions[] = {"None", "Explode", "??", "??", "Deflate"};
				 for (uint i = 0; i < _infos.size(); ++i) {
					 ImGui::PushID(i);
					 ImGui::TableNextColumn();
					 Common::String sizeText(toHumanReadableBytes(_infos[i].discSize));

					 if (ImGui::Selectable(sizeText.c_str(), _selectedRes == i, ImGuiSelectableFlags_SpanAllColumns)) {
						 if (_selectedRes != i) {
							 selectRes(i);
						 }
						 _selectedRes = i;
					 }
					 ImGui::TableNextColumn();
					 sizeText = toHumanReadableBytes(_infos[i].uncompressedSize);
					 ImGui::Text("%s", sizeText.c_str());
					 ImGui::TableNextColumn();
					 ImGui::Text("%d %s", _infos[i].compressionFlag, compressions[_infos[i].compressionFlag]);
					 ImGui::TableNextColumn();
					 ImGui::Text("%d", _infos[i].offset);
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
		 if (_selectedTexture) {
			 ImGui::Image(_selectedTexture, ImVec2(320, 200));
		 } else {
			 ImGui::Text("No preview available, select a resource first.");
		 }
	 }
	 ImGui::End();
 }

 } // namespace Fitd
