MODULE := engines/fitd

MODULE_OBJS = \
	console.o \
	fitd.o \
	metaengine.o \
	unpack.o

# This module can be built as a plugin
ifeq ($(ENABLE_FITD), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
