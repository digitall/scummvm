MODULE := engines/fitd

MODULE_OBJS = \
	actor_list.o \
	aitd1.o \
	aitd2.o \
	aitd3.o \
	aitd_box.o \
	aitd1_tatou.o \
	anim.o \
	anim_action.o \
	costable.o \
	common.o \
	eval_var.o \
	file_access.o \
	fitd.o \
	font.o \
	floor.o \
	game_time.o \
	gfx.o \
	hqr.o \
	input.o \
	inventory.o \
	jack.o \
	life.o \
	lines.o \
	main_loop.o \
	metaengine.o \
	music.o \
	object.o \
	pak.o \
	polys.o \
	renderer_opengl.o \
	renderer_soft.o \
	room.o \
	save.o \
	sequence.o \
	startup_menu.o \
	system_menu.o \
	tatou.o \
	track.o \
	unpack.o \
	vars.o \
	zv.o

ifdef USE_IMGUI
MODULE_OBJS += \
	debugtools.o \
	debugger/dbg_pak.o \
	debugger/dbg_utils.o \
	debugger/dbg_vars.o \

endif

# This module can be built as a plugin
ifeq ($(ENABLE_FITD), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk

# Detection objects
DETECT_OBJS += $(MODULE)/detection.o
