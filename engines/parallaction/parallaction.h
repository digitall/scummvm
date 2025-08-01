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

#ifndef PARALLACTION_PARALLACTION_H
#define PARALLACTION_PARALLACTION_H

#include "common/str.h"
#include "common/stack.h"
#include "common/array.h"
#include "common/func.h"
#include "common/random.h"
#include "common/savefile.h"
#include "common/textconsole.h"
#include "common/text-to-speech.h"

#include "engines/engine.h"

#include "parallaction/input.h"
#include "parallaction/inventory.h"
#include "parallaction/objects.h"
#include "parallaction/disk.h"
#include "parallaction/detection.h"

#define PATH_LEN	200


/**
 * This is the namespace of the Parallaction engine.
 *
 * Status of this engine: ???
 *
 * Games using this engine:
 * - Nippon Safes Inc. (complete)
 * - The Big Red Adventure (work in progress)
 */
namespace Parallaction {

enum {
	kDebugDisk = 1,
	kDebugWalk,
	kDebugParser,
	kDebugDialogue,
	kDebugGraphics,
	kDebugExec,
	kDebugInput,
	kDebugAudio,
	kDebugMenu,
	kDebugInventory,
};

enum EngineFlags {
	kEnginePauseJobs	= (1 << 1),
	kEngineWalking		= (1 << 3),
	kEngineChangeLocation	= (1 << 4),
	kEngineBlockInput	= (1 << 5),
	kEngineDragging		= (1 << 6),
	kEngineTransformedDonna	= (1 << 7),

	// BRA specific
	kEngineReturn		= (1 << 10)
};

enum {
	kEvNone			= 0,
	kEvSaveGame		= 2000,
	kEvLoadGame		= 4000,
	kEvIngameMenu   = 8000
};

enum LanguageIndex {
	kItalian = 0,
	kFrench = 1,
	kEnglish = 2,
	kGerman = 3
};




extern uint32		g_engineFlags;
extern char			g_saveData1[];
extern uint32		g_globalFlags;
extern const char	*g_dinoName;
extern const char	*g_donnaName;
extern const char	*g_doughName;
extern const char	*g_drkiName;
extern const char	*g_minidinoName;
extern const char	*g_minidonnaName;
extern const char	*g_minidoughName;
extern const char	*g_minidrkiName;






class Debugger;
class Gfx;
class Input;
class DialogueManager;
class MenuInputHelper;
class PathWalker_NS;
class PathWalker_BR;
class CommandExec;
class ProgramExec;
class SoundMan;
class SoundMan_ns;
class SoundMan_br;
class LocationParser_ns;
class LocationParser_br;
class ProgramParser_ns;
class ProgramParser_br;
class BalloonManager;

struct Location {

	Common::Point	_startPosition;
	uint16			_startFrame;
	char			_name[100];

	CommandList		_aCommands;
	CommandList		_commands;
	Common::String	_comment;
	Common::String	_endComment;

	ZoneList		_zones;
	AnimationList	_animations;
	ProgramList		_programs;

	bool		_hasSound;
	char		_soundFile[50];

	// NS specific
	PointList	_walkPoints;
	Common::String _slideText[2];

	// BRA specific
	int			_zeta0;
	int			_zeta1;
	int			_zeta2;
	CommandList		_escapeCommands;
	Common::Point	_followerStartPosition;
	uint16			_followerStartFrame;


protected:
	int			_gameType;

	bool keepZone_br(ZonePtr z);
	bool keepZone_ns(ZonePtr z);
	bool keepAnimation_ns(AnimationPtr a);
	bool keepAnimation_br(AnimationPtr a);

	template<class T>
	void freeList(Common::List<T> &list, bool removeAll, Common::MemFunc1<bool, T, Location> filter);

public:
	Location(int gameType);
	~Location();

	AnimationPtr findAnimation(const char *name);
	ZonePtr findZone(const char *name);

	void cleanup(bool removeAll);
	void freeZones(bool removeAll);

	int getScale(int z) const;
};


class CharacterName {
	const char *_prefix;
	const char *_suffix;
	bool _dummy;
	char _name[30];
	char _baseName[30];
	char _fullName[30];
	static const char _prefixMini[];
	static const char _suffixTras[];
	static const char _empty[];
	void dummify();
public:
	CharacterName();
	CharacterName(const char *name);
	void bind(const char *name);
	const char *getName() const;
	const char *getBaseName() const;
	const char *getFullName() const;
	bool dummy() const;
};


struct Character {
	AnimationPtr	_ani;
	GfxObj			*_head;
	GfxObj			*_talk;

	Character();

protected:
	CharacterName	_name;

public:
	void setName(const char *name);
	const char *getName() const;
	const char *getBaseName() const;
	const char *getFullName() const;
	bool dummy() const;
};

#ifdef USE_TTS

struct CharacterVoiceData {
	const char *characterName;
	uint8 voiceID;
	bool male;
};

static const CharacterVoiceData characterVoiceDatas[] = {
	{ nullptr, 0, true },		// Used as the narrator for cutscene text
	{ "dough", 1, true },
	{ "donna", 0, false },
	{ "dino", 2, true },
	{ "drki", 3, true },
	{ "ddd.talk", 1, true },
	{ nullptr, 0, false },	// Donna in ddd.talk
	{ nullptr, 2, true },	// Dino in ddd.talk
	{ "police.talk", 4, true },
	{ "vecchio.talk", 5, true },
	{ "karaoke.talk", 6, true },
	{ "suicida.talk", 7, true },
	{ "direttore.talk", 8, true },
	{ "guardiano.talk", 9, true },
	{ "sento.talk", 10, true },
	{ "giocatore.talk", 11, true },
	{ "mona.talk", 12, true },
	{ "monaco2.talk", 13, true },
	{ "uccello.talk", 14, true },
	{ "guru.talk", 15, true },
	{ "monaco1.talk", 16, true },
	{ "segretario.talk", 17, true },
	{ "imperatore.talk", 18, true },
	{ "secondo.talk", 19, true },
	{ "taxista.talk", 20, true },
	{ "bemutsu.talk", 21, true },
	{ "negro.talk", 22, true },
	{ "punk.talk", 23, true },
	{ "chan.talk", 24, true },
	{ "donna0.talk", 0, false },
	{ "maxkos.talk", 25, true },
	{ nullptr, 26, true },		// Second person in the maxkos.talk dialogue
	{ "drki.talk", 3, true },
	{ "barman.talk", 27, true },
	{ "losco.talk", 28, true },
	{ "mitsu.talk", 29, true },
	{ "autoradio.talk", 30, true },
	{ "passa1.talk", 31, true },
	{ "passa2.talk", 1, false },
	{ "passa3.talk", 32, true },
	{ "giornalaio.talk", 33, true },
	{ "pazza1.talk", 2, false },
	{ "pazza2.talk", 3, false },
	{ "pazza3.talk", 4, false },
	{ "citofono.talk", 34, true },
	{ "perdelook.talk", 0, false },
	{ "tele.talk", 35, true },
	{ "dough.talk", 1, true },
	{ "donna.talk", 0, false },
	{ "guanti.talk", 36, true },
	{ "cuoco.talk", 37, true },
	{ "punks.talk", 38, true },
	{ nullptr, 39, true },		// Second person in the punks.talk dialogue
	{ "punko.talk", 40, true },
	{ "hotdog.talk", 41, true },
	{ "cameriera.talk", 5, false },
	{ "rice1.talk", 42, true },
	{ nullptr, 43, true },		// Second person in the rice1.talk dialogue
	{ "segretaria.talk", 6, false },
	{ "robot.talk", 44, true },
	{ "portiere.talk", 45, true },
	{ "figaro.talk", 46, true },
	{ "ominos.talk", 47, true },
	{ "cassiera.talk", 7, false },
	{ "dino.talk", 2, true }
};

#endif

enum PlayableCharacterVoiceID {
	kDoug = 1,
	kDonna = 2,
	kDino = 3,
	kDrki = 4
};

static const int kNarratorVoiceID = 0;

class SaveLoad;

#define NUM_LOCATIONS 120

class Parallaction : public Engine {
	friend class Debugger;

public:
	int getGameType() const;
	uint32 getFeatures() const;
	Common::Language getLanguage() const;
	Common::Platform getPlatform() const;

protected:		// members
	bool detectGame();

private:
	const PARALLACTIONGameDescription *_gameDescription;
	uint16	_language;

public:
	Parallaction(OSystem *syst, const PARALLACTIONGameDescription *gameDesc);
	~Parallaction() override;

	// Engine APIs
	virtual Common::Error init();
	virtual Common::Error go() = 0;
	Common::Error run() override {
		Common::Error err;
		err = init();
		if (err.getCode() != Common::kNoError)
			return err;
		return go();
	}

	bool hasFeature(EngineFeature f) const override;
	void pauseEngineIntern(bool pause) override;

	// info
	int32			_screenWidth;
	int32			_screenHeight;
	int32			_screenSize;
	int				_gameType;

	// subsystems
	Gfx				*_gfx;
	Disk			*_disk;
	Input			*_input;
	SaveLoad		*_saveLoad;
	MenuInputHelper *_menuHelper;
	Common::RandomSource _rnd;
	SoundMan		*_soundMan;

	// fonts
	Font		*_labelFont;
	Font		*_menuFont;
	Font		*_introFont;
	Font		*_dialogueFont;

	// game utilities
	Table				*_globalFlagsNames;
	Table				*_objectsNames;
	GfxObj				*_objects;
	Table				*_callableNames;
	Table				*_localFlagNames;
	CommandExec			*_cmdExec;
	ProgramExec			*_programExec;
	BalloonManager		*_balloonMan;
	DialogueManager		*_dialogueMan;
	InventoryRenderer	*_inventoryRenderer;
	Inventory			*_inventory;			// inventory for the current character

	// game data
	Character		_char;
	uint32			_localFlags[NUM_LOCATIONS];
	char			_locationNames[NUM_LOCATIONS][32];
	int16			_currentLocationIndex;
	uint16			_numLocations;
	Location		_location;
	ZonePtr			_activeZone;
	char			_characterName1[50];	// only used in changeCharacter
	int				_characterVoiceID;
	ZonePtr			_zoneTrap;
	ZonePtr			_commentZone;
	Common::String  _newLocationName;

protected:
	void	runGame();
	void	runGameFrame(int event);
	void	runGuiFrame();
	void	cleanupGui();
	void	runDialogueFrame();
	void	exitDialogueMode();
	void	runCommentFrame();
	void	enterCommentMode(ZonePtr z);
	void	exitCommentMode();
	void	updateView();
	void	drawAnimation(AnimationPtr anim);
	void	drawZone(ZonePtr zone);
	void	updateZones();
	void	doLocationEnterTransition();
	void	allocateLocationSlot(const char *name);
	void	finalizeLocationParsing();
	void	showLocationComment(const Common::String &text, bool end);
	void	destroyDialogueManager();

public:
	void	beep();
	void	pauseJobs();
	void	resumeJobs();
	uint	getInternLanguage();
	void	setInternLanguage(uint id);
	void	enterDialogueMode(ZonePtr z);
	void	scheduleLocationSwitch(const char *location);
	void	showSlide(const char *name, int x = 0, int y = 0);

public:
	void		setLocationFlags(uint32 flags);
	void		clearLocationFlags(uint32 flags);
	void		toggleLocationFlags(uint32 flags);
	uint32		getLocationFlags();
	bool		checkSpecialZoneBox(ZonePtr z, uint32 type, uint x, uint y);
	bool		checkZoneBox(ZonePtr z, uint32 type, uint x, uint y);
	bool		checkZoneType(ZonePtr z, uint32 type);
	bool		checkLinkedAnimBox(ZonePtr z, uint32 type, uint x, uint y);
	ZonePtr		hitZone(uint32 type, uint16 x, uint16 y);
	void		runZone(ZonePtr z);
	bool		pickupItem(ZonePtr z);
	void		updateDoor(ZonePtr z, bool close);
	void		showZone(ZonePtr z, bool visible);
	void		highlightInventoryItem(ItemPosition pos);
	int16		getHoverInventoryItem(int16 x, int16 y);
	int		addInventoryItem(ItemName item);
	int		addInventoryItem(ItemName item, uint32 value);
	void		dropItem(uint16 v);
	bool		isItemInInventory(int32 v);
	const		InventoryItem* getInventoryItem(int16 pos);
	int16		getInventoryItemIndex(int16 pos);
	void		openInventory();
	void		closeInventory();
	void		sayText(const Common::String &text, Common::TextToSpeechManager::Action action) const;
	void		setTTSVoice(int id) const;
	void		stopTextToSpeech() const;

	virtual void parseLocation(const char* name) = 0;
	virtual void changeLocation() = 0;
	virtual void changeCharacter(const char *name) = 0;
	virtual	void callFunction(uint index, void* parm) = 0;
	virtual void runPendingZones() = 0;
	virtual void cleanupGame() = 0;
	virtual void updateWalkers() = 0;
	virtual void scheduleWalk(int16 x, int16 y, bool fromUser) = 0;
	virtual DialogueManager *createDialogueManager(ZonePtr z) = 0;
	virtual bool processGameEvent(int event) = 0;
};



class Parallaction_ns : public Parallaction {

public:
	Parallaction_ns(OSystem* syst, const PARALLACTIONGameDescription *gameDesc);
	~Parallaction_ns() override;

	// Engine APIs
	Common::Error init() override;
	Common::Error go() override;

	SoundMan_ns*	_soundManI;

	uint16			_score;
	Common::String	_password;

	bool _endCredits;


public:
	void parseLocation(const char *filename) override;
	void changeLocation() override;
	void changeCharacter(const char *name) override;
	void callFunction(uint index, void* parm) override;
	void runPendingZones() override;
	void cleanupGame() override;
	void updateWalkers() override;
	void scheduleWalk(int16 x, int16 y, bool fromUser) override;
	DialogueManager *createDialogueManager(ZonePtr z) override;
	bool processGameEvent(int event) override;
	void cleanInventory(bool keepVerbs);
	void	changeBackground(const char *background, const char *mask = 0, const char *path = 0);

private:
	bool				_inTestResult;
	LocationParser_ns	*_locationParser;
	ProgramParser_ns	*_programParser;

private:
	void	initFonts();
	void	freeFonts();
	void	initResources();
	void	initInventory();
	void	destroyInventory();
	void	setupBalloonManager();
	void	startGui();
	void	startCreditSequence();
	void	startEndPartSequence();
	void	loadProgram(AnimationPtr a, const char *filename);
	void	freeLocation(bool removeAll);
	void	freeCharacter();
	void	destroyTestResultLabels();
	void	startMovingSarcophagus(ZonePtr sarc);
	void	stopMovingSarcophagus();


	//  callables data
	typedef void (Parallaction_ns::*Callable)(void *);
	const Callable *_callables;
	ZonePtr _moveSarcGetZone;
	ZonePtr _moveSarcExaZone;
	ZonePtr _moveSarcGetZones[5];
	ZonePtr _moveSarcExaZones[5];
	uint16 num_foglie;

	int16 _sarcophagusDeltaX;
	bool	_movingSarcophagus;		 // sarcophagus stuff to be saved
	uint16	_freeSarcophagusSlotX;		 // sarcophagus stuff to be saved
	AnimationPtr _rightHandAnim;
	bool _intro;
	static const Callable _dosCallables[25];
	static const Callable _amigaCallables[25];

	GfxObj *_testResultLabels[2];

	PathWalker_NS		*_walker;

	// common callables
	void _c_play_boogie(void *);
	void _c_startIntro(void *);
	void _c_endIntro(void *);
	void _c_moveSheet(void *);
	void _c_sketch(void *);
	void _c_shade(void *);
	void _c_score(void *);
	void _c_fade(void *);
	void _c_moveSarc(void *);
	void _c_contaFoglie(void *);
	void _c_zeroFoglie(void *);
	void _c_trasformata(void *);
	void _c_offMouse(void *);
	void _c_onMouse(void *);
	void _c_setMask(void *);
	void _c_endComment(void *);
	void _c_frankenstein(void *);
	void _c_finito(void *);
	void _c_ridux(void *);
	void _c_testResult(void *);

	// dos specific callables
	void _c_null(void *);

	// amiga specific callables
	void _c_projector(void *);
	void _c_HBOff(void *);
	void _c_offSound(void *);
	void _c_startMusic(void *);
	void _c_closeMusic(void *);
	void _c_HBOn(void *);
};



#define NUM_ZONES	100

class Parallaction_br : public Parallaction {

public:
	Parallaction_br(OSystem* syst, const PARALLACTIONGameDescription *gameDesc);
	~Parallaction_br() override;

	Common::Error init() override;
	Common::Error go() override;

public:
	void parseLocation(const char* name) override;
	void changeLocation() override;
	void changeCharacter(const char *name) override;
		void callFunction(uint index, void* parm) override;
	void runPendingZones() override;
	void cleanupGame() override;
	void updateWalkers() override;
	void scheduleWalk(int16 x, int16 y, bool fromUser) override;
	DialogueManager *createDialogueManager(ZonePtr z) override;
	bool processGameEvent(int event) override;

	void setupSubtitles(const char *s, const char *s2, int y);
	void clearSubtitles();

	Inventory *findInventory(const char *name);
	void linkUnlinkedZoneAnimations();

	void testCounterCondition(const Common::String &name, int op, int value);
	void restoreOrSaveZoneFlags(ZonePtr z, bool restore);

public:
	bool	counterExists(const Common::String &name);
	int		getCounterValue(const Common::String &name);
	void	setCounterValue(const Common::String &name, int value);

	void	setFollower(const Common::String &name);

	int		getSfxStatus();
	int		getMusicStatus();
	void	enableSfx(bool enable);
	void	enableMusic(bool enable);

	const char **_audioCommandsNamesRes;
	static const char *_partNames[];
	int			_part;
	int			_nextPart;


#if 0	// disabled since I couldn't find any references to lip sync in the scripts
	int16		_lipSyncVal;
	uint		_subtitleLipSync;
#endif
	int			_subtitleY;
	GfxObj		*_subtitle[2];
	ZonePtr		_activeZone2;
	uint32		_zoneFlags[NUM_LOCATIONS][NUM_ZONES];

private:
	LocationParser_br		*_locationParser;
	ProgramParser_br		*_programParser;
	SoundMan_br				*_soundManI;
	Inventory				*_dinoInventory;
	Inventory				*_donnaInventory;
	Inventory				*_dougInventory;

	int32		_counters[32];
	Table		*_countersNames;

private:
	void	initResources();
	void	initInventory();
	void	destroyInventory();

	void	cleanInventory(bool keepVerbs);
	void	setupBalloonManager();
	void	initFonts();
	void	freeFonts();
	void	freeLocation(bool removeAll);
	void	loadProgram(AnimationPtr a, const char *filename);
	void	startGui(bool showSplash);
	void	startIngameMenu();
	void	freeCharacter();

	typedef void (Parallaction_br::*Callable)(void *);
	const Callable *_callables;
	static const Callable _dosCallables[6];
	static const Callable _amigaCallables[6];

	Common::String		_followerName;
	AnimationPtr		_follower;
	PathWalker_BR		*_walker;
	int					_ferrcycleMode;

	// dos callables
	void _c_null(void *);
	void _c_blufade(void *);
	void _c_resetpalette(void *);
	void _c_ferrcycle(void *);
	void _c_lipsinc(void *);
	void _c_albcycle(void *);
	void _c_password(void *);
};

extern Parallaction *g_vm;


} // End of namespace Parallaction


#endif
