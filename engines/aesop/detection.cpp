#include "aesop/aesop.h"

#include "common/config-manager.h"
#include "common/error.h"
#include "common/fs.h"

#include "engines/advancedDetector.h"

#include "aesop/detection_tables.h"

class AesopMetaEngine : public AdvancedMetaEngine {
public:
	AesopMetaEngine() : AdvancedMetaEngine(gameDescriptions, sizeof(ADGameDescription), aesopGames, NULL) {
	}

	virtual const char *getName() const {
		return "AESOP: Extensible State-Object Processor";
	}
 
	virtual const char *getOriginalCopyright() const {
		return "AESOP: Extensible State-Object Processor (c) John Miles and Miles Design, Inc."
			"\nEye of the Beholder III (C) TSR, Inc., (C) Strategic Simulations, Inc."
			"\nDungeon Hack (C) TSR, Inc., (C) Strategic Simulations, Inc., (C) DreamForge Intertainment";
	}
	
	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *gd) const;
};

bool AesopMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	*engine = new Aesop::AesopEngine(syst);
	return true;
}

#if PLUGIN_ENABLED_DYNAMIC(AESOP)
	REGISTER_PLUGIN_DYNAMIC(AESOP, PLUGIN_TYPE_ENGINE, AesopMetaEngine);
#else
	REGISTER_PLUGIN_STATIC(AESOP, PLUGIN_TYPE_ENGINE, AesopMetaEngine);
#endif
