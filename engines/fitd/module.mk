MODULE := engines/fitd

MODULE_OBJS = \
	costable.o \
	console.o \
	fitd.o \
	gfx.o \
	hqr.o \
	metaengine.o \
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
