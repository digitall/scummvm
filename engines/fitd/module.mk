MODULE := engines/fitd

MODULE_OBJS = \
	aitd1.o \
	aitd1_tatou.o \
	costable.o \
	console.o \
	file_access.o \
	fitd.o \
	game_time.o \
	gfx.o \
	hqr.o \
	metaengine.o \
	pak.o \
	tatou.o \
	unpack.o \
	vars.o

# This module can be built as a plugin
ifeq ($(ENABLE_FITD), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
