static const PlainGameDescriptor aesopGames[] = {
	{"aesop", "AESOP engine game"},
	{"eob3", "Eye of the Beholder III: Assault on Myth Drannor"},
	{"hack", "Dungeon Hack"},
	{0, 0}
};

static const ADGameDescription gameDescriptions[] = {
	{
		"eob3",
		"",
		AD_ENTRY1("eye.res", "a4ad50b2dfd38e67e2c7c671d4d15624"),
		Common::EN_ANY,
		Common::kPlatformPC,
		ADGF_UNSTABLE,
		GUIO0()
	},
	{
		"hack",
		"",
		AD_ENTRY1("hack.res", "67345ba1870656dd54d8c8544954d834"),
		Common::EN_ANY,
		Common::kPlatformPC,
		ADGF_UNSTABLE,
		GUIO0()
	},
	AD_TABLE_END_MARKER
};
