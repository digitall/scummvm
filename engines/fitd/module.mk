MODULE := engines/fitd

MODULE_OBJS = \
	actor_list.o \
	aitd1.o \
	aitd_box.o \
	aitd1_tatou.o \
	anim.o \
	anim_action.o \
	costable.o \
	common.o \
	console.o \
	file_access.o \
	fitd.o \
	font.o \
	floor.o \
	game_time.o \
	gfx.o \
	hqr.o \
	input.o \
	inventory.o \
	life.o \
	lines.o \
	main_loop.o \
	metaengine.o \
	object.o \
	pak.o \
	polys.o \
	room.o \
	save.o \
	startup_menu.o \
	system_menu.o \
	tatou.o \
	unpack.o \
	vars.o \
	zv.o

# This module can be built as a plugin
ifeq ($(ENABLE_FITD), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
