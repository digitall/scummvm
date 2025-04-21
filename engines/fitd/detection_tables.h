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

namespace Fitd {

const PlainGameDescriptor fitdGames[] = {
	{ "aitd1", "Alone in the Dark" },
	{ "jack", "Jack in the Dark" },
	{ "aitd2", "Alone in the Dark 2" },
	{ "aitd3", "Alone in the Dark 3" },
	{ nullptr, nullptr }
};

const FitdGameDescription gameDescriptions[] = {
	{
		{
			"aitd1",
			nullptr,
			AD_ENTRY1s("LISTBOD2.PAK", "7c86683ab53991ad694ebffec56a8ea3", 268430),
			Common::UNK_LANG,
			Common::kPlatformDOS,
			ADGF_UNSTABLE,
			GUIO2(GUIO_NOMIDI, GUIO_RENDERVGA)
		},
		GID_AITD1
	},
	{
		{
			"jack",
			nullptr,
			AD_ENTRY1s("PERE.PAK", "6ae91d1842fc70f6f2c449016328fe31", 296437),
			Common::UNK_LANG,
			Common::kPlatformDOS,
			ADGF_UNSTABLE,
			GUIO2(GUIO_NOMIDI, GUIO_RENDERVGA)
		},
		GID_JACK
	},
	{
		{
			"aitd2",
			nullptr,
			AD_ENTRY1s("MER.PAK", "0b27a44028286f7b5aa99a744d601b09", 79049),
			Common::UNK_LANG,
			Common::kPlatformDOS,
			ADGF_UNSTABLE,
			GUIO2(GUIO_NOMIDI, GUIO_RENDERVGA)
		},
		GID_AITD2
	},
	{
		{
			"aitd3",
			nullptr,
			AD_ENTRY1s("AN1.PAK", "a9c4fbbc60042f6bdc4ca63d8f8badcd", 306902),
			Common::UNK_LANG,
			Common::kPlatformDOS,
			ADGF_UNSTABLE,
			GUIO2(GUIO_NOMIDI, GUIO_RENDERVGA)
		},
		GID_AITD3
	},
	{AD_TABLE_END_MARKER, GID_NONE}
};

} // End of namespace Fitd
