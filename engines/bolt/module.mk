MODULE := engines/bolt

MODULE_OBJS := \
	blt_file.o \
	bolt.o \
	detection.o \
	graphics.o \
	menu_state.o \
	movie.o \
	movie_state.o \
	pf_file.o

# This module can be built as a plugin
ifeq ($(ENABLE_BOLT), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
