MODULE := engines/bolt

MODULE_OBJS := \
	blt_file.o \
	bolt.o \
	detection.o \
	graphics.o \
	image.o \
	labyrinth/labyrinth.o \
	menu_card.o \
	merlin/action_puzzle.o \
	merlin/hub.o \
	merlin/main_menu.o \
	merlin/merlin.o \
	movie.o \
	pf_file.o \
	scene.o

# This module can be built as a plugin
ifeq ($(ENABLE_BOLT), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
