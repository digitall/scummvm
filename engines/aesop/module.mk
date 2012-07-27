MODULE := engines/aesop

MODULE_OBJS := \
	aesop.o \
	detection.o
	eye.o
	hack.o
	resource.o
	script.o
	
MODULE_DIRS += \
	engines/aesop

# This module can be built as a plugin
ifeq ($(ENABLE_AESOP), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
