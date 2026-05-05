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

#include "common/file.h"
#include "mads/madsv2/core/cycle.h"
#include "mads/madsv2/core/env.h"
#include "mads/madsv2/core/himem.h"
#include "mads/madsv2/core/matte.h"
#include "mads/madsv2/core/mcga.h"
#include "mads/madsv2/core/mouse.h"
#include "mads/madsv2/core/pack.h"
#include "mads/madsv2/core/pal.h"
#include "mads/madsv2/core/tile.h"
#include "mads/madsv2/core/timer.h"
#include "mads/madsv2/animview/animview.h"

namespace MADS {
namespace MADSV2 {
namespace AnimView {

struct AnimEntry {
	char name[16];
	uint8 bg_load_status;
	uint8 sound_mode;
	uint8 show_bars;
	uint8 fx;
};
constexpr int MAX_ANIM = 40;
constexpr bool in_mads_mode = true;

static int anim_count;
static AnimEntry anim_list[MAX_ANIM];
static uint8 background_load_status;
static int16 sound_interrupts_mode;
static bool show_white_bars;
static int concat_mode;
static bool resync_timer1, resync_timer2;
static bool exit_immediately_at_end;

/**
 * Initializes animview global variables
 */
static void init_globals() {
	anim_count = 0;
	background_load_status = 0xff;
	sound_interrupts_mode = -1;
	show_white_bars = true;
	concat_mode = 0;
	resync_timer1 = true;
	resync_timer2 = false;
	exit_immediately_at_end = false;
}

/**
 * Adds an animation to the list of .aa files to show in sequence
 * @param name		Animation resource name
 */
static void add_anim(const char *name) {
	static char buf[16];

	if (strlen(name) > 0 && anim_count < MAX_ANIM) {
		Common::strcpy_s(buf, name);
		if (!strchr(buf, '.'))
			Common::strcat_s(buf, ".aa");

		Common::strcpy_s(anim_list[anim_count].name, buf);
		anim_list[anim_count].bg_load_status = background_load_status;
		anim_list[anim_count].sound_mode = sound_interrupts_mode;
		anim_list[anim_count].show_bars = show_white_bars;
		++anim_count;
	}
}

/**
 * Parses a flag from an animation line in the resource file
 */
static void flag_parse(const char *param) {
	switch (tolower(*param++)) {
	case 'o':
		// Specify opening special effect
		assert(anim_count < MAX_ANIM);
		if (*param == ':')
			anim_list[anim_count].fx = atoi(param + 1);
		break;

	case 'r':
		// -r[:abn] Resynch timer (always, beginning, never)
		if (*param == ':') {
			switch (tolower(*++param)) {
			case 'n':
				resync_timer1 = true;
				resync_timer2 = false;
				break;
			case 'a':
				resync_timer1 = false;
				break;
			case 'b':
				resync_timer1 = true;
				resync_timer2 = true;
				break;
			default:
				break;
			}
		}
		break;

	case 'w':
		// Toggle white bars on or off
		show_white_bars = !show_white_bars;
		break;

	case 'x':
		// Exit immediately after last frame
		exit_immediately_at_end = true;
		break;

	default:
		error("Unsupported animview flag - %c", *param);
		break;
	}
}

/**
 * Reads the contents of the resource file stream, and adds
 * entries to the anim_list for what to display
 */
static void read_resource(Common::SeekableReadStream *src) {
	while (!src->eos()) {
		Common::String line = src->readLine();
		line.trim();
		if (line.empty())
			continue;

		// Handle any flags at the start of the line
		const char *lineP = line.c_str();
		while (strchr("/-", *lineP)) {
			// It's a flag
			++lineP;
			const char *switchEnd = strchr(lineP, ' ');
			Common::String param;

			if (switchEnd) {
				// There's more line after the flag
				param = Common::String(lineP, switchEnd);
				for (lineP = switchEnd; *lineP == ' '; ++lineP) {
				}
			} else {
				// This is the last flag of the line
				param = Common::String(lineP);
				lineP = lineP + strlen(lineP);
			}

			flag_parse(param.c_str());
		}

		// As long as we're not at the end of the line, any remainder
		// should be the name of the animation resource to play
		if (*lineP)
			add_anim(lineP);
	}
}

static void perform() {
	char buf[80];// , speech_name[80];
	AnimFile anim_in;

	himem_startup();
	(void)tile_setup();

	mcga_compute_retrace_parameters();
	memset(cycling_palette, 0, sizeof(Palette));
	pal_init(1, 8);
	mouse_hard_cursor_mode(2, master_palette);

	timer_install();
	matte_init(-1);

	for (int count = 0; count < anim_count; ++count) {
		*buf = '\0';
		if (in_mads_mode)
			Common::strcpy_s(buf, "*");
		Common::strcat_s(buf, anim_list[count].name);

		himem_preload_series(buf, 0);

		if (anim_get_header_info(buf, &anim_in))
			continue;
		
		// TODO: More stuff
	}
}

void animview_main(const char *resName) {
	char name[16];

	init_globals();

	pack_enable_pfab_explode();
	(void)env_verify();

	Common::strcpy_s(name, resName);
	if (!Common::File::exists(name)) {
		Common::strcpy_s(name, "*");
		Common::strcat_s(name, resName);
	}

	Common::SeekableReadStream *file = env_open(name);
	if (!file)
		error("Could not open animview resource - %s", name);

	// Read in the resource lines
	read_resource(file);
	delete file;

	perform();
}

} // namespace AnimView
} // namespace MADSV2
} // namespace MADS
