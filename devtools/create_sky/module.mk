
MODULE := devtools/create_sky

MODULE_OBJS := \
	util.o \
	create_sky.o

# Set the name of the executable
TOOL_EXECUTABLE := create_sky

# Include common rules
include $(srcdir)/rules.mk
