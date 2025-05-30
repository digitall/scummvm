MODULE := engines/darkseed

MODULE_OBJS = \
	adlib_dsf.o \
	adlib_worx.o \
	animation.o \
	anm.o \
	big5font.o \
	console.o \
	cursor.o \
	cutscene.o \
	darkseed.o \
	debugconsole.o \
	gamefont.o \
	img.o \
	inventory.o \
	kidpic.o \
	kofont.o \
	langtext.o \
	menu.o \
	metaengine.o \
	midiparser_sbr.o \
	morph.o \
	music.o \
	nsp.o \
	objects.o \
	pal.o \
	pic.o \
	player.o \
	room.o \
	sound.o \
	sprites.o \
	titlefont.o \
	tostext.o \
	usecode.o \
	zhmenufont.o

# This module can be built as a plugin
ifeq ($(ENABLE_DARKSEED), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
