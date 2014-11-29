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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "bolt/bolt.h"

#include "engines/advancedDetector.h"

namespace Bolt {

// NOTE: Labyrinth of Crete is not yet supported or detected.
static const PlainGameDescriptor boltGames[] = {
	{ "merlin", "Merlin's Apprentice" },
	{ 0, 0 }
};

static const ADGameDescription gameDescriptions[] = {
	{
		"merlin",
		0,
		// FIXME: BOLTLIB.BLT is not detected by ScummVM because it is marked
		// "Hidden". Players are forced to use Windows Explorer to uncheck the
		// "Hidden" checkbox in the file properties. Please fix ScummVM so it
		// can detect these files!
		// NOTE: This is MD5 of first 5000 bytes.
		AD_ENTRY1("BOLTLIB.BLT", "58ef3e35e1f6369056272a30c67bb94d"),
		Common::EN_ANY,
		// Games were released for Win and Mac on the same CD-ROM. There are no
		// significant platform differences.
		Common::kPlatformWindows,
		ADGF_UNSTABLE,
		GUIO0()
	},

	/*
	 * Notes about the CD-I version:
	 * The CD-I has a screen resolution of 384x240, and the backgrounds are
	 * sized accordingly. For the CD-ROM release, the backgrounds were cropped
	 * to the 320x200 "safe area".
	 * The CD-I version had a cross-fade effect that is absent in the CD-ROM
	 * version.
	 * CD-I colors are different from sRGB colors. (FIXME: Investigate)
	 */

	AD_TABLE_END_MARKER
};

class BoltMetaEngine : public AdvancedMetaEngine {
public:
	BoltMetaEngine();

	virtual const char *getName() const;

	virtual const char *getOriginalCopyright() const;

protected:
	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const;
};

BoltMetaEngine::BoltMetaEngine() :
	AdvancedMetaEngine(Bolt::gameDescriptions, sizeof(ADGameDescription), boltGames)
{ }

const char *BoltMetaEngine::getName() const {
	return "BOLT";
}

const char *BoltMetaEngine::getOriginalCopyright() const {
	return "(C) 1994 Philips Interactive Media";
}

bool BoltMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const
{
	if (desc) {
		*engine = new Bolt::BoltEngine(syst, desc);
	}
	return desc != 0;
}

} // End of namespace Bolt

#if PLUGIN_ENABLED_DYNAMIC(BOLT)
	REGISTER_PLUGIN_DYNAMIC(BOLT, PLUGIN_TYPE_ENGINE, Bolt::BoltMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(BOLT, PLUGIN_TYPE_ENGINE, Bolt::BoltMetaEngine);
#endif
