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
 #include "fitd/fitd.h"
 #include "fitd/vars.h"
 #include "fitd/debugger/dbg_utils.h"
 #include "fitd/debugger/dbg_vars.h"

 namespace Fitd {

 void drawVars() {
	 ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	 if (ImGui::Begin("CVars")) {

		 static const char *names[] = {"page turn sample",
									   "gun flash BODY",
									   "max inventory weight",
									   "credits text",
									   "lightning sample",
									   "intro text (Edward)",
									   "intro text (Emily)",
									   "initial camera target",
									   "choice of characters (0 = Edward 1 = Emily)",
									   "thrown actor collision sample",
									   "water splash sample (unused)",
									   "lamp-deflecting actor ID",
									   "Pregtz dead (ignore LIGHT commands etc.)",
									   "light circle actor ID in E6 (-1 none; 1 player; 13 dropped lamp)",
									   "ashtray smoke state",
									   "player dead"};

		 for (uint8 i = 0; i < CVarsSize; ++i) {
			 if (g_engine->getGameId() == GID_AITD1 && i < 16) {
				 InputS16(names[i], &CVars[i]);
			 } else {
				 Common::String s(Common::String::format("CVar%u", i));
				 InputS16(s.c_str(), &CVars[i]);
			 }
		 }
	 }
	 ImGui::End();

	 ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
	 if (ImGui::Begin("Vars")) {

		 static const char *varNames[] = {
			 "player has control",
			 "player pushing something",
			 "E0R0 floor hatch opening attempts",
			 "E0R0 floor hatch open",
			 "footstep sound 1",
			 "footstep sound 2",
			 "oil lamp has fuel",
			 "oil lamp lit",
			 "amount of oil in lamp",
			 "object being dropped",
			 "throwing something/using jug",
			 "oil can in E3 used",
			 "oil can in E1 used",
			 "cartridges in rifle",
			 "rifle ready to fire",
			 "rifle shot sound",
			 "standing in trigger 2 in E2R1 (in front of settler portrait)",
			 "Indian cover in place",
			 "E0R0 zombie hitpoints",
			 "E0R0 bird dead",
			 "active monster count",
			 "player hitpoints",
			 "E0R0 bird hitpoints",
			 "E1R0-R1 door open",
			 "player moving backwards (walking backwards or just took a hit, not rifle knockback)",
			 "being in the intro sequence",
			 "arrow pointer (gets values 0-3, used for determining which one gets fired next)",
			 "E1R1-R3 door open",
			 "player in a dangerous area",
			 "type of death (0 - normal, 1 - falling into pits, 2 - De Vermis, 3 - jelly)",
			 "E1R1-E2 door open",
			 "E1R2 zombie spawned",
			 "saber chest unlocked",
			 "Pregzt dead",
			 "saber blade in place",
			 "player standing in trigger 0 in E3R11",
			 "saber handle in place",
			 "saber durability",
			 "saber broken",
			 "saber damage (set to 2 after it breaks)",
			 "E1R2 zombie hitpoints",
			 "searching cabinet in E1R3",
			 "opening cabinet in E1R3",
			 "E1R1-R4 door open",
			 "dresser unlocked in E1R4",
			 "E1R4 bird alerted",
			 "vase broken",
			 "mirror 1 broken",
			 "player standing in trigger 0 in E1R7",
			 "mirror placed in N statue in E1R7",
			 "unbroken mirrors placed counter (increments by 1 or 2)",
			 "player standing in trigger 1 in E1R7",
			 "mirror placed in S statue in E1R7",
			 "mirror 2 broken",
			 "E1R4 bird hitpoints",
			 "E1R1-R5 door open",
			 "E1R0 zombie spawned",
			 "E1R0 zombie hitpoints",
			 "E1R1-R7 door open",
			 "nightgaunts left",
			 "scary sounds played in E1R7",
			 "north nightgaunt aggroed",
			 "south nightgaunt aggroed",
			 "E2R0-R2 S door open",
			 "E2R0 west doors unlocked",
			 "E2R0-R2 N door open",
			 "E2R0-R1 N door open",
			 "E2R0-R1 S door open",
			 "player standing in trigger 0 in E2R0",
			 "light source in E2R0",
			 "area lit (E2R0)",
			 "player in E2R0",
			 "playing scary sounds when entering E2R0 from E2R8",
			 "standing in trigger 0 in E2R8",
			 "ready to spawn axe",
			 "ready to spawn arrow",
			 "Indian portrait dead",
			 "E2R3-R4 door open",
			 "E2R3 (and E5R4) birds are both alive",
			 "E2R3 bird spawned",
			 "E2R3 bird hitpoints",
			 "E2R2-R4 door open",
			 "E2R4-R1 door open",
			 "player in E2R6",
			 "player in E2R5",
			 "area lit (E2R6)",
			 "light source in E2R6",
			 "E2R4-R5 door open",
			 "jug filled",
			 "player standing in trigger 0 in E3R13",
			 "player current action",
			 "E2R4-R6 door open",
			 "bullets in revolver",
			 "E2R1-R7 door open",
			 "player standing in trigger 7 in E6R6 (in front of sarcophagus)",
			 "talisman placed",
			 "E2R2-R10 door open",
			 "record in gramophone (1 - The Blue Danube, 2 - Dance of Death, 3 - Opus Posthumous 69 Nr 1)",
			 "E2R10 ghost aggroed",
			 "N front door open",
			 "S front door open",
			 "jelly ready to attack",
			 "E3R1-R13 E door open",
			 "doors between E3R1 and E3R13 have been unlocked",
			 "E3R1-R13 W door open",
			 "jelly should attack (unused)",
			 "E3R1-R2 E door open",
			 "E3R1-R2 W door open",
			 "E3R2-R3 S door open",
			 "doors between E3R2 and E3R3 unlocked",
			 "E3R2-R3 N door open",
			 "player in E3R3",
			 "dancer 1 transformed into vortex (1) or dead (2)",
			 "dancer 2 transformed into vortex (1) or dead (2)",
			 "dancer 3 transformed into vortex (1) or dead (2)",
			 "pirate hitpoints",
			 "E3R3-R4 S door open",
			 "E3R3-R4 N door open",
			 "dancer 1 transformed into vortex",
			 "dancer 2 transformed into vortex",
			 "dancer 3 transformed into vortex",
			 "E3R4-R5 door open",
			 "spiders released in E3R4",
			 "statue searched",
			 "cellar door open",
			 "cellar door unlocked",
			 "E3R5-R8 door open",
			 "E3R7-R8 door open",
			 "E3R8-R10 door open",
			 "player standing in trigger 0 in E3R12",
			 "E3R12 solved",
			 "zombie 1 in E3R12 hitpoints",
			 "player standing in trigger 1 in E3R12",
			 "E3R8-R9 door open",
			 "E3R10 zombie spawned",
			 "revolver ready to fire",
			 "E3R10 zombie hitpoints",
			 "E3R11-R13 W door open",
			 "E3R11 doors unlocked",
			 "E3R11-R13 E door open",
			 "E3R0-R12 W door open",
			 "E3R0-R12 E door open",
			 "E3R9-R12 door open",
			 "zombie 2 in E3R12 hitpoints",
			 "zombie 2 in E3R12 dead",
			 "zombie 2 in E3R12 eating",
			 "next active zombie in E3R12 (0-4 means zombie 1-5; 5 means all dead)",
			 "there are zombies active in E3R12 (resets to 0 for 3 seconds when any zombie has died regardless of how many were active before; if all were active, stays at 0)",
			 "zombie 3 in E3R12 hitpoints",
			 "zombie 3 in E3R12 dead",
			 "zombie 3 in E3R12 eating",
			 "zombie 4 in E3R12 hitpoints",
			 "zombie 4 in E3R12 dead",
			 "zombie 4 in E3R12 eating",
			 "zombie 5 in E3R12 hitpoints",
			 "zombie 5 in E3R12 dead",
			 "zombie 5 in E3R12 eating",
			 "E3R12 trap has been sprung",
			 "zombie 1 in E3R12 aggroed",
			 "E3R12-R13 door open",
			 "bridge segment 1 stepped on in E5R0",
			 "bridge segment 2 stepped on in E5R0",
			 "bridge segment 3 stepped on in E5R0",
			 "bridge segment 4 stepped on in E5R0",
			 "bridge segment 5 stepped on in E5R0",
			 "bridge segment 6 stepped on in E5R0",
			 "bridge segment 7 stepped on in E5R0",
			 "bridge segment 8 stepped on in E5R0",
			 "player inside trigger 1 in E5R0 or trigger 0 in E5R6 (used to trigger correct death while falling into pits)",
			 "E5 step sound switch (set to 1 when player inside trigger 0 in E5R0; set to 0 when player in trigger 0 in E5R6; set to 1 when player is in E5R7 but not on the ledge in NW corner; set to 0 when entering E5R8 or E5R9; in E5R10, set to 0 if standing in trigger 12 (unreachable), otherwise 1)",
			 "Cthonian state (default is 0; values 1-2 tell which way the Cthonian is supposed to spawn, set to 1 when hitting trigger 11 in E4R0, set to 2 when hitting trigger 9 in E3R11; set to 3 after it has been spawned (happens when entering E5R1 or E5R2). The Cthonian is despawned if this is set to 3 when the player enters E5R8 or R9)",
			 "Cthonian has stopped past the boulders",
			 "Cthonian has turned around the corner (happens after hitting trigger 0 in E5R4)",
			 "E5R4 bird hitpoints",
			 "E5 Deep One should be spawned",
			 "E5 Deep One hitpoints",
			 "E5 Deep One should be despawned",
			 "E5R8 wasp hitpoints",
			 "not in the dark in E5-E6",
			 "beetle hitpoints",
			 "falling bridge segment 1 stepped on in E5R10",
			 "falling bridge segment 2 stepped on in E5R10",
			 "falling bridge segment 3 stepped on in E5R10",
			 "falling bridge segment 4 stepped on in E5R10",
			 "falling bridge segment 5 stepped on in E5R10",
			 "falling bridge segment 6 stepped on in E5R10",
			 "falling bridge segment 7 stepped on in E5R10",
			 "falling bridge segment 8 stepped on in E5R10",
			 "falling bridge segment 9 stepped on in E5R10",
			 "falling bridge segment 10 stepped on in E5R10",
			 "falling bridge segment 11 stepped on in E5R10",
			 "falling bridge segment 12 stepped on in E5R10",
			 "falling bridge segment 13 stepped on in E5R10",
			 "E5R10 wasp hitpoints",
			 "E5R11 chest unlocked",
			 "player standing in trigger 0 in E6R5",
			 "E6 Deep One should be spawned",
			 "E6 Deep One hitpoints",
			 "E6 Deep One should be despawned",
			 "E6R6 fireball switch",
			 "step sound 1 during intro",
			 "step sound 2 during intro",
			 "unused (set to 1 13 seconds into the endgame)",
			 "unused (set to 1 10 seconds into the endgame)",
			 "temporary BODY variable (used by LIFE 59 for returning previous BODY after drinking flask)",
			 "player is in E5 or E6 (never checked)",
			 "fall height",
		 };

		 for (int i = 0; i < varSize; ++i) {
			 if (g_engine->getGameId() == GID_AITD1 && i < 207) {
				 InputS16(varNames[i], &vars[i]);
			 } else {
				 Common::String s(Common::String::format("Var%u", i));
				 InputS16(s.c_str(), &vars[i]);
			 }
		 }
	 }
	 ImGui::End();
 }

 } // namespace Fitd
