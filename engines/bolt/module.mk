MODULE := engines/bolt

MODULE_OBJS := \
	bolt.o \
	detection.o \
	graphics.o \
	menu_card.o \
	movie.o \
	pf_file.o \
	scene.o \
	boltlib/boltlib.o \
	boltlib/palette.o \
	labyrinth/labyrinth.o \
	merlin/action_puzzle.o \
	merlin/color_puzzle.o \
	merlin/hub.o \
	merlin/main_menu.o \
	merlin/memory_puzzle.o \
	merlin/merlin.o \
	merlin/potion_puzzle.o \
	merlin/sliding_puzzle.o \
	merlin/synch_puzzle.o \
	merlin/tangram_puzzle.o \
	merlin/word_puzzle.o

# This module can be built as a plugin
ifeq ($(ENABLE_BOLT), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
