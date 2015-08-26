#!/usr/bin/env python3

# ScummVM - Graphic Adventure Engine
#
# ScummVM is the legal property of its developers, whose names
# are too numerous to list here. Please refer to the COPYRIGHT
# file distributed with this source distribution.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#

"""BLT Viewer main program."""

import io
import sys

from PySide import QtGui
from PySide.QtCore import Qt

from construct import *

from internal.bltfile import BltFile
from internal.rl7 import decode_rl7

# Generate _TEST_PALETTE
_TEST_PALETTE = [0] * 256 # holds 32-bit numbers formatted with bytes A, R, G, B
for r in range(0, 4):
    for g in range(0, 8):
        for b in range(0, 4):
            i = 8 * 4 * r + 4 * g + b
            _TEST_PALETTE[i] = 0xFF000000 | ((r * 255 // 3) << 16) | \
                ((g * 255 // 7) << 8) | (b * 255 // 3)
# Repeat for both planes
_TEST_PALETTE[128:256] = _TEST_PALETTE[0:128]

_IMAGE_COMPRESSION_NAMES = {
    0: "CLUT7",
    1: "RL7",
    }

_ImageHeaderStruct = Struct("_ImageHeaderStruct",
    UBInt8("compression"),
    UBInt8("unk_1"),
    UBInt16("unk_2"),
    UBInt16("unk_4"),
    SBInt16("offset_x"),
    SBInt16("offset_y"),
    UBInt16("width"),
    UBInt16("height"),
    )

class BltImageWidget(QtGui.QLabel):
    def __init__(self, data, palette):
        header = _ImageHeaderStruct.parse(data)
        image_data = data[0x18:]

        if header.compression == 0:
            # CLUT7
            super().__init__()
            image = QtGui.QImage(image_data, header.width, header.height, header.width, QtGui.QImage.Format_Indexed8)
            image.setColorTable(palette)
            self.setPixmap(QtGui.QPixmap.fromImage(image).scaled(header.width * 2, header.height * 2))
        elif header.compression == 1:
            # RL7
            super().__init__()
            decoded_image = bytearray(header.width * header.height)
            decode_rl7(decoded_image, image_data, header.width, header.height)
            image = QtGui.QImage(decoded_image, header.width, header.height, header.width, QtGui.QImage.Format_Indexed8)
            image.setColorTable(palette)
            self.setPixmap(QtGui.QPixmap.fromImage(image).scaled(header.width * 2, header.height * 2))
        else:
            super().__init__("Unsupported compression type {}".format(compression))

class MyTableWidget(QtGui.QTableWidget):
    def __init__(self, *col_labels):
        super().__init__()

        self.setColumnCount(len(col_labels))
        self.setHorizontalHeaderLabels(col_labels)

    def add_row(self, row_label, *values):
        row_num = self.rowCount()
        self.setRowCount(row_num + 1)

        headerItem = QtGui.QTableWidgetItem(row_label)
        headerItem.setFlags(Qt.NoItemFlags)
        self.setVerticalHeaderItem(row_num, headerItem)

        for i in range(0, len(values)):
            valueItem = QtGui.QTableWidgetItem(values[i])
            valueItem.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
            self.setItem(row_num, i, valueItem)

_RES_TYPE_HANDLERS = {}

def _register_res_handler(type):
    def decorate(cls):
        _RES_TYPE_HANDLERS[type] = cls
    return decorate

@_register_res_handler(1)
class _Values8BitHandler:
    name = "8-bit Values"

    def open(res, container, app):
        newWidget = MyTableWidget("Value")

        for i in range(0, len(res.data)):
            val = res.data[i]
            newWidget.add_row("{}".format(i), "0x{:02X}".format(val))

        container.addWidget(newWidget)

@_register_res_handler(3)
class _Values16BitHandler:
    name = "16-bit Values"

    def open(res, container, app):
        newWidget = MyTableWidget("Value")

        parsed = GreedyRange(UBInt16("value")).parse(res.data)
        for i in range(0, len(parsed)):
            newWidget.add_row("{}".format(i), "0x{:04X}".format(parsed[i]))

        container.addWidget(newWidget)

@_register_res_handler(6)
class _ResourceListHandler:
    name = "Resource List"

    def open(res, container, app):
        newWidget = MyTableWidget("ID")

        parsed = GreedyRange(UBInt32("value")).parse(res.data)
        for i in range(0, len(parsed)):
            newWidget.add_row("{}".format(i), "0x{:08X}".format(parsed[i]))

        container.addWidget(newWidget)

@_register_res_handler(7)
class _SoundHandler:
    name = "Sound"

@_register_res_handler(8)
class _ImageHandler:
    name = "Image"

    def open(res, container, app):
        header = _ImageHeaderStruct.parse(res.data)

        newLayout = QtGui.QVBoxLayout()

        info_table = MyTableWidget("Value")
        compression_name = _IMAGE_COMPRESSION_NAMES.get(header.compression, "Unknown")
        info_table.add_row("Compression", "{} ({})".format(compression_name, header.compression))
        info_table.add_row("Unk @1", "0x{:02X}".format(header.unk_1))
        info_table.add_row("Unk @2", "0x{:04X}".format(header.unk_2))
        info_table.add_row("Unk @4", "0x{:04X}".format(header.unk_4))
        info_table.add_row("Offset", "({}, {})".format(header.offset_x, header.offset_y))
        info_table.add_row("Width", "{}".format(header.width))
        info_table.add_row("Height", "{}".format(header.height))
        newLayout.addWidget(info_table)

        newLayout.addWidget(QtGui.QLabel("Tip: If colors look wrong, load a palette."))

        newLayout.addWidget(BltImageWidget(res.data, app.cur_palette))

        newWidget = QtGui.QWidget()
        newWidget.setLayout(newLayout)
        container.addWidget(newWidget)

_ColorStruct = Struct("_ColorStruct",
    UBInt8("r"),
    UBInt8("g"),
    UBInt8("b"),
    )

class _PaletteWidget(QtGui.QWidget):
    """A grid of colors."""

    _NUM_COLUMNS = 16

    def __init__(self, colors):
        """Initialize widget with an iterable of _ColorStruct's."""

        super().__init__()
        layout = QtGui.QGridLayout()
        self.setLayout(layout)
        for i in range(0, len(colors)):
            row = i // self._NUM_COLUMNS
            col = i % self._NUM_COLUMNS
            item = QtGui.QWidget()
            item.setAutoFillBackground(True)
            item_palette = QtGui.QPalette()
            qcolor = QtGui.QColor(colors[i].r, colors[i].g, colors[i].b)
            item_palette.setColor(QtGui.QPalette.Background, qcolor)
            item.setPalette(item_palette)
            layout.addWidget(item, row, col)

@_register_res_handler(10)
class _PaletteHandler:
    name = "Palette"

    def open(res, container, app):

        # TODO: parse 6 bytes of header info (plane, number of colors, etc.)
        colors = GreedyRange(_ColorStruct).parse(res.data[6:])

        palette_widget = _PaletteWidget(colors)

        # TODO: handle planes
        app.cur_palette = [0] * 256
        for i in range(0, len(colors)):
            app.cur_palette[i] = 0xFF000000 | (colors[i].r << 16) | (colors[i].g << 8) | colors[i].b

        new_layout = QtGui.QVBoxLayout()
        new_layout.addWidget(palette_widget)
        new_layout.addWidget(QtGui.QLabel("Palette loaded."))

        new_widget = QtGui.QWidget()
        new_widget.setLayout(new_layout)
        container.addWidget(new_widget)

_ColorCyclesStruct = Struct("_ColorCyclesStruct",
    Array(4, UBInt16("num_slots")),
    Array(4, UBInt32("slot_ids")),
    )

@_register_res_handler(11)
class _ColorCyclesHandler:
    name = "Color Cycles"

    def open(res, container, app):
        new_widget = MyTableWidget("# Slots", "Slots ID")

        parsed = _ColorCyclesStruct.parse(res.data)
        for i in range(0, 4):
            new_widget.add_row("{}".format(i),
                "{}".format(parsed.num_slots[i]),
                "0x{:08X}".format(parsed.slot_ids[i])
                )

        container.addWidget(new_widget)

_ColorCycleSlotStruct = Struct("_ColorCycleSlotStruct",
    UBInt16("start"),
    UBInt16("end"),
    UBInt16("unk_4"),
    )

@_register_res_handler(12)
class _ColorCycleSlotHandler:
    name = "Color Cycle Slot"

    def open(res, container, app):
        new_widget = MyTableWidget("Value")

        parsed = _ColorCycleSlotStruct.parse(res.data)
        new_widget.add_row("Start", "{}".format(parsed.start))
        new_widget.add_row("End", "{}".format(parsed.end))
        new_widget.add_row("Unk @4", "0x{:04X}".format(parsed.unk_4))

        container.addWidget(new_widget)

_PlaneStruct = Struct("_PlaneStruct",
    UBInt32("image_id"),
    UBInt32("palette_id"),
    UBInt32("hotspots_id"),
    UBInt32("unk_c"),
    )

@_register_res_handler(26)
class _PlaneHandler:
    name = "Plane"

    def open(res, container, app):
        newWidget = MyTableWidget("Value")

        parsed = _PlaneStruct.parse(res.data)
        newWidget.add_row("Image ID", "0x{:08X}".format(parsed.image_id))
        newWidget.add_row("Palette ID", "0x{:08X}".format(parsed.palette_id))
        newWidget.add_row("Hotspots ID", "0x{:08X}".format(parsed.hotspots_id))
        newWidget.add_row("Unk @C", "0x{:08X}".format(parsed.unk_c))

        container.addWidget(newWidget)

_SpriteStruct = Struct("_SpriteStruct",
    SBInt16("x"),
    SBInt16("y"),
    UBInt32("image_id"),
    )

# Ex: 370E
@_register_res_handler(27)
class _SpritesHandler:
    name = "Sprites"

    def open(res, container, app):
        newWidget = MyTableWidget("Position", "Image ID")

        parsed = GreedyRange(_SpriteStruct).parse(res.data)
        for i in range(0, len(parsed)):
            newWidget.add_row("{}".format(i),
                "({}, {})".format(parsed[i].x, parsed[i].y),
                "0x{:08X}".format(parsed[i].image_id))


        container.addWidget(newWidget)

@_register_res_handler(28)
class _ColorsHandler:
    name = "Colors"

    def open(res, container, app):
        colors = GreedyRange(_ColorStruct).parse(res.data)
        container.addWidget(_PaletteWidget(colors))

_PaletteModStruct = Struct("_ButtonPaletteModStruct",
    UBInt8("index"),
    UBInt8("count"),
    UBInt32("colors_id"),
    )

@_register_res_handler(29)
class _PaletteModHandler:
    name = "Palette Mod"

    def open(res, container, app):
        newWidget = MyTableWidget("Index", "Count", "Colors ID")

        parsed = GreedyRange(_PaletteModStruct).parse(res.data)
        for i in range(0, len(parsed)):
            newWidget.add_row("{}".format(i),
                "{}".format(parsed[i].index),
                "{}".format(parsed[i].count),
                "0x{:08X}".format(parsed[i].colors_id))

        container.addWidget(newWidget)

_BUTTON_GRAPHICS_TYPE_NAMES = {
    1: "Palette Mods",
    2: "Sprites",
    }

_ButtonGraphicsStruct = Struct("_ButtonGraphicsStruct",
    UBInt16("type"),
    UBInt32("unk_2"),
    UBInt32("hovered_id"),
    UBInt32("idle_id"),
    )

# Ex: 69B5
@_register_res_handler(30)
class _ButtonGraphicsHandler:
    name = "Button Graphics"

    def open(res, container, app):
        newWidget = MyTableWidget("Type", "Unk @2", "Hovered", "Idle")

        parsed = GreedyRange(_ButtonGraphicsStruct).parse(res.data)
        for i in range(0, len(parsed)):
            type_name = _BUTTON_GRAPHICS_TYPE_NAMES.get(parsed[i].type, "Unknown")
            newWidget.add_row("{}".format(i),
                "{} ({})".format(type_name, parsed[i].type),
                "0x{:08X}".format(parsed[i].unk_2),
                "0x{:08X}".format(parsed[i].hovered_id),
                "0x{:08X}".format(parsed[i].idle_id))

        container.addWidget(newWidget)

_BUTTON_TYPE_NAMES = {
    1: "Rectangle",
    3: "Hotspot Query",
    }

_ButtonStruct = Struct("_ButtonStruct",
    UBInt16("type"),
    UBInt16("left"),
    UBInt16("right"),
    UBInt16("top"),
    UBInt16("bottom"),
    UBInt16("plane"),
    UBInt16("num_graphics"),
    UBInt16("unk_e"),
    UBInt32("graphics_id"),
    )

# Ex: 312D
@_register_res_handler(31)
class _ButtonsHandler:
    name = "Buttons"

    def open(res, container, app):
        newWidget = MyTableWidget("Type", "(L, R, T, B)", "Plane", "# Graphics",
            "Unk @E", "Graphics ID")

        parsed = GreedyRange(_ButtonStruct).parse(res.data)
        for i in range(0, len(parsed)):
            type_name = _BUTTON_TYPE_NAMES.get(parsed[i].type, "Unknown")
            newWidget.add_row("{}".format(i),
                "{} ({})".format(type_name, parsed[i].type),
                "({}, {}, {}, {})".format(parsed[i].left, parsed[i].right, parsed[i].top, parsed[i].bottom),
                "{}".format(parsed[i].plane),
                "{}".format(parsed[i].num_graphics),
                "{}".format(parsed[i].unk_e),
                "0x{:08X}".format(parsed[i].graphics_id))

        container.addWidget(newWidget)

_SceneStruct = Struct("_SceneStruct",
    UBInt32("fore_plane_id"), # 0
    UBInt32("back_plane_id"), # 4
    UBInt8("num_sprites"), # 8
    UBInt8("unk_9"), # 8
    UBInt32("sprites_id"), # A
    UBInt32("unk_e"), # E
    UBInt32("unk_12"), # 12
    UBInt32("color_cycles_id"), # 16
    UBInt16("num_buttons"), # 1A
    UBInt32("buttons_id"), # 1C
    SBInt16("origin_x"), # 20
    SBInt16("origin_y"), # 22
    )

# Ex: 3A0B
@_register_res_handler(32)
class _SceneHandler:
    name = "Scene"

    def open(res, container, app):
        newWidget = MyTableWidget("Value")

        parsed = _SceneStruct.parse(res.data)
        newWidget.add_row("Fore Plane ID", "0x{:08X}".format(parsed.fore_plane_id))
        newWidget.add_row("Back Plane ID", "0x{:08X}".format(parsed.back_plane_id))
        newWidget.add_row("# Sprites", "0x{:02X}".format(parsed.num_sprites))
        newWidget.add_row("Unk @9", "0x{:02X}".format(parsed.unk_9))
        newWidget.add_row("Sprites ID", "0x{:08X}".format(parsed.sprites_id))
        newWidget.add_row("Unk @Eh", "0x{:08X}".format(parsed.unk_e))
        newWidget.add_row("Unk @12h", "0x{:08X}".format(parsed.unk_12))
        newWidget.add_row("Color Cycles ID", "0x{:08X}".format(parsed.color_cycles_id))
        newWidget.add_row("# Buttons", "{}".format(parsed.num_buttons))
        newWidget.add_row("Buttons ID", "0x{:08X}".format(parsed.buttons_id))
        newWidget.add_row("Origin", "({}, {})".format(parsed.origin_x, parsed.origin_y))

        container.addWidget(newWidget)

_MainMenuStruct = Struct("_MainMenuStruct",
    UBInt32("scene_id"),
    UBInt32("colorbars_id"),
    UBInt32("colorbars_palette_id"),
    )

# Ex: 0118
@_register_res_handler(33)
class _MainMenuHandler:
    name = "Main Menu"

    def open(res, container, app):
        newWidget = MyTableWidget("Value")

        parsed = _MainMenuStruct.parse(res.data)
        newWidget.add_row("Scene ID", "0x{:08X}".format(parsed.scene_id))
        newWidget.add_row("Color Bars ID", "0x{:08X}".format(parsed.colorbars_id))
        newWidget.add_row("Color Bars Palette ID", "0x{:08X}".format(parsed.colorbars_palette_id))

        container.addWidget(newWidget)

_FileMenuStruct = Struct("_FileMenuStruct",
    UBInt32("scene_id"), # 0
    UBInt32("select_game_piece_id"), # 4
    UBInt32("set_new_id"), # 8
    UBInt32("new_id"), # C
    UBInt32("solved_id"), # 10
    UBInt32("one_more_id"), # 14
    UBInt32("x_more_id"), # 18
    UBInt32("xx_more_id"), # 1C
    UBInt32("unk_20"), # 20
    UBInt32("unk_24"), # 24
    Array(10, UBInt32("tens_digit_id")), # 28
    Array(10, UBInt32("ones_digit_id")), # 50
    Array(10, UBInt32("unk_digit_id")), # 78
    UBInt32("unk_a0"), # A0
    UBInt16("sound_id"), # A4
    )

# Ex: 02A0
@_register_res_handler(34)
class _FileMenuHandler:
    name = "File Menu"
    
    def open(res, container, app):
        newWidget = MyTableWidget("Value")

        parsed = _FileMenuStruct.parse(res.data)
        newWidget.add_row("Scene ID", "0x{:08X}".format(parsed.scene_id))
        newWidget.add_row("Select Game Piece ID", "0x{:08X}".format(parsed.select_game_piece_id))
        newWidget.add_row("Set New ID", "0x{:08X}".format(parsed.set_new_id))
        newWidget.add_row("New ID", "0x{:08X}".format(parsed.new_id))
        newWidget.add_row("Solved ID", "0x{:08X}".format(parsed.solved_id))
        newWidget.add_row("One More ID", "0x{:08X}".format(parsed.one_more_id))
        newWidget.add_row("X More ID", "0x{:08X}".format(parsed.x_more_id))
        newWidget.add_row("XX More ID", "0x{:08X}".format(parsed.xx_more_id))
        newWidget.add_row("Unk @20h", "0x{:08X}".format(parsed.unk_20))
        newWidget.add_row("Unk @24h", "0x{:08X}".format(parsed.unk_24))
        for i in range(0, len(parsed.tens_digit_id)):
            newWidget.add_row("Tens Digit {} ID".format(i), "0x{:08X}".format(parsed.tens_digit_id[i]))
        for i in range(0, len(parsed.ones_digit_id)):
            newWidget.add_row("Ones Digit {} ID".format(i), "0x{:08X}".format(parsed.ones_digit_id[i]))
        for i in range(0, len(parsed.unk_digit_id)):
            newWidget.add_row("Unk Digit {} ID".format(i), "0x{:08X}".format(parsed.unk_digit_id[i]))
        newWidget.add_row("Unk @A0h", "0x{:08X}".format(parsed.unk_a0))
        newWidget.add_row("Sound ID", "0x{:04X}".format(parsed.sound_id))

        container.addWidget(newWidget)

_DifficultyMenuStruct = Struct("_DifficultyMenuStruct",
    UBInt32("scene_id"),
    UBInt32("choose_difficulty_id"),
    UBInt32("change_difficulty_id"),
    )

# Ex: 006E
@_register_res_handler(35)
class _DifficultyMenuHandler:
    name = "Difficulty Menu"

    def open(res, container, app):
        newWidget = MyTableWidget("Value")

        parsed = _DifficultyMenuStruct.parse(res.data)
        newWidget.add_row("Scene ID", "0x{:08X}".format(parsed.scene_id))
        newWidget.add_row("Choose Difficulty ID", "0x{:08X}".format(parsed.choose_difficulty_id))
        newWidget.add_row("Change Difficulty ID", "0x{:08X}".format(parsed.change_difficulty_id))

        container.addWidget(newWidget)

_PotionPuzzleStruct = Struct("_PotionPuzzleStruct",
    UBInt32("unk_0"), # 0
    UBInt32("bg_image_id"), # 4
    UBInt32("palette_id"), # 8
    UBInt32("unk_c"), # C
    UBInt32("unk_10"), # 10
    UBInt32("unk_14"), # 14
    UBInt32("unk_18"), # 18
    UBInt32("unk_1c"), # 1C
    UBInt32("unk_20"), # 20
    UBInt32("unk_24"), # 24
    UBInt32("unk_28"), # 28
    UBInt32("unk_2c"), # 2C
    UBInt16("unk_30"), # 30
    UBInt16("delay"), # 32
    Array(7, UBInt16("sound_id")), # 34
    SBInt16("origin_x"), # 42
    SBInt16("origin_y"), # 44
    )

# Ex: 9C0E
@_register_res_handler(59)
class _PotionPuzzleHandler:
    name = "Potion Puzzle"

    def open(res, container, app):
        newWidget = MyTableWidget("Value")

        parsed = _PotionPuzzleStruct.parse(res.data)
        newWidget.add_row("Unk @0", "0x{:08X}".format(parsed.unk_0))
        newWidget.add_row("Background Image ID", "0x{:08X}".format(parsed.bg_image_id))
        newWidget.add_row("Palette ID", "0x{:08X}".format(parsed.palette_id))
        newWidget.add_row("Unk @Ch", "0x{:08X}".format(parsed.unk_c))
        newWidget.add_row("Unk @10h", "0x{:08X}".format(parsed.unk_10))
        newWidget.add_row("Unk @14h", "0x{:08X}".format(parsed.unk_14))
        newWidget.add_row("Unk @18h", "0x{:08X}".format(parsed.unk_18))
        newWidget.add_row("Unk @1Ch", "0x{:08X}".format(parsed.unk_1c))
        newWidget.add_row("Unk @20h", "0x{:08X}".format(parsed.unk_20))
        newWidget.add_row("Unk @24h", "0x{:08X}".format(parsed.unk_24))
        newWidget.add_row("Unk @28h", "0x{:08X}".format(parsed.unk_28))
        newWidget.add_row("Unk @2Ch", "0x{:08X}".format(parsed.unk_2c))
        newWidget.add_row("Unk @30h", "0x{:04X}".format(parsed.unk_30))
        newWidget.add_row("Delay", "{} ms".format(parsed.delay))
        for i in range(0, len(parsed.sound_id)):
            newWidget.add_row("Sound {}".format(i+1), "0x{:04X}".format(parsed.sound_id[i]))
        newWidget.add_row("Origin", "({}, {})".format(parsed.origin_x, parsed.origin_y))

        container.addWidget(newWidget)

# Ex: 9C0A
@_register_res_handler(60)
class _PotionIngredientSlotHandler:
    name = "Potion Ingredient Slot"

# Ex: 9B23
@_register_res_handler(61)
class _PotionIngredientsHandler:
    name = "Potion Ingredients"

# Ex: 9B22
@_register_res_handler(62)
class _PotionComboListHandler:
    name = "Potion Combo List"

# Movie list extracted from MERLIN.EXE
_POTION_MOVIE_NAMES = (
    'ELEC', 'EXPL', 'FLAM', 'FLSH', 'MIST', 'OOZE', 'SHMR',
    'SWRL', 'WIND', 'BOIL', 'BUBL', 'BSPK', 'FBRS', 'FCLD',
    'FFLS', 'FSWR', 'LAVA', 'LFIR', 'LSMK', 'SBLS', 'SCLM',
    'SFLS', 'SPRE', 'WSTM', 'WSWL', 'BUGS', 'CRYS', 'DNCR',
    'FISH', 'GLAC', 'GOLM', 'EYEB', 'MOLE', 'MOTH', 'MUDB',
    'ROCK', 'SHTR', 'SLUG', 'SNAK', 'SPKB', 'SPKM', 'SPDR',
    'SQID', 'CLOD', 'SWIR', 'VOLC', 'WORM',
    )

_PotionComboStruct = Struct("_PotionComboStruct",
    UBInt8("a"),
    UBInt8("b"),
    UBInt8("c"),
    UBInt8("d"),
    UBInt16("movie"),
    )

# Ex: 9B17
@_register_res_handler(63)
class _PotionCombosHandler:
    name = "Potion Combos"

    def open(res, container, app):
        newWidget = MyTableWidget("A", "B", "C", "D", "Movie")
        
        parsed = GreedyRange(_PotionComboStruct).parse(res.data)
        for i in range(0, len(parsed)):
            try:
                movie_name = _POTION_MOVIE_NAMES[parsed[i].movie]
            except IndexError:
                movie_name = "Unknown"
            newWidget.add_row("{}".format(i),
                "0x{:02X}".format(parsed[i].a),
                "0x{:02X}".format(parsed[i].b),
                "0x{:02X}".format(parsed[i].c),
                "0x{:02X}".format(parsed[i].d),
                "{} ({})".format(movie_name, parsed[i].movie),
                )

        container.addWidget(newWidget)

class MyHexViewerWidget(QtGui.QTextEdit):
    def __init__(self, data):
        super().__init__()
        self.setReadOnly(True)
        self.setLineWrapMode(QtGui.QTextEdit.NoWrap)
        self.setFontFamily("courier")

        text = ''
        addr = 0
        while addr < len(data):
            text += "{:08X} ".format(addr)

            num_bytes = len(data) - addr
            if num_bytes > 16:
                num_bytes = 16

            for i in range(0, num_bytes):
                if i == 8:
                    text += " "
                text += " {:02X}".format(data[addr + i])

            text += "\n"
            addr += num_bytes

        self.setPlainText(text)

def _open_hex_viewer(res, container, app):
    container.addWidget(MyHexViewerWidget(res.data))

class BltViewer:
    def __init__(self, argv):
        self.cur_palette = _TEST_PALETTE

        self.app = QtGui.QApplication(argv)

        self.win = QtGui.QMainWindow()
        self.win.setWindowTitle("BLT Viewer")

        menuBar = QtGui.QMenuBar()
        fileMenu = menuBar.addMenu("&File")
        fileMenu.addAction("&Open", self._open_action)
        fileMenu.addSeparator()
        fileMenu.addAction("E&xit", self.app.closeAllWindows)

        self.win.setMenuBar(menuBar)

        self.tree = QtGui.QTreeWidget()
        self.tree.setHeaderLabels(("ID", "Type", "Size"))
        self.tree.itemActivated.connect(self._tree_item_activated_action)

        self.content = QtGui.QStackedWidget()
        self.content.addWidget(QtGui.QLabel("Please open a BLT file"))

        splitter = QtGui.QSplitter()
        splitter.addWidget(self.tree)
        splitter.addWidget(self.content)
        splitter.setStretchFactor(0, 0)
        splitter.setStretchFactor(1, 1)

        self.win.setCentralWidget(splitter)

        self.win.resize(600, 400)

        self.win.show()

    def exec_(self):
        return self.app.exec_()

    def _open_action(self):
        file_dlg_result = QtGui.QFileDialog.getOpenFileName(self.win,
            filter="BLT Files (*.BLT);;All Files (*.*)")
        in_path = file_dlg_result[0]
        if len(in_path) > 0:
            in_file = open(in_path, 'rb')
            self._load_blt_file(in_file)

    def _tree_item_activated_action(self, item, column):
        res_id = item.data(0, Qt.UserRole)
        if res_id is not None:
            print("Opening res id {:04X}...".format(res_id))
            self._load_resource(res_id)

    def _load_blt_file(self, in_file):
        print("Opening BLT file...")
        self.blt_file = BltFile(in_file)

        self.tree.clear()
        for dir in self.blt_file.dir_table:
            dir_item = QtGui.QTreeWidgetItem()
            dir_item.setText(0, dir.name)

            for res in dir.res_table:
                res_item = QtGui.QTreeWidgetItem()
                res_item.setText(0, res.name)
                handler = _RES_TYPE_HANDLERS.get(res.type)
                if handler:
                    res_item.setText(1, "{} ({})".format(handler.name, res.type))
                else:
                    res_item.setText(1, "{}".format(res.type))
                res_item.setText(2, "{}".format(res.size))

                res_item.setData(0, Qt.UserRole, res.id)

                dir_item.addChild(res_item)

            self.tree.addTopLevelItem(dir_item)

        self.content.removeWidget(self.content.currentWidget())
        self.content.addWidget(QtGui.QLabel("Please double-click on a resource"))

    def _load_resource(self, res_id):
        self.content.removeWidget(self.content.currentWidget())

        tabWidget = QtGui.QTabWidget()

        res = self.blt_file.load_resource(res_id)
        handler = _RES_TYPE_HANDLERS.get(res.type)
        if handler and hasattr(handler, "open"):
            resTab = QtGui.QStackedWidget()
            handler.open(res, resTab, self)
            tabWidget.addTab(resTab, "Resource")

        hexViewerTab = QtGui.QStackedWidget()
        _open_hex_viewer(res, hexViewerTab, self)
        tabWidget.addTab(hexViewerTab, "Hex")

        self.content.addWidget(tabWidget)

def main(argv):
    app = BltViewer(argv)
    return app.exec_()

if __name__=='__main__':
    sys.exit(main(sys.argv))
