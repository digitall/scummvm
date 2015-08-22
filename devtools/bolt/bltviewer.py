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

TEST_PALETTE = [0] * 256 # holds 32-bit values formatted with bytes A, R, G, B

# Generate TEST_PALETTE
for r in range(0, 4):
    for g in range(0, 8):
        for b in range(0, 4):
            i = 8 * 4 * r + 4 * g + b
            TEST_PALETTE[i] = 0xFF000000 | ((r * 255 // 3) << 16) | \
                ((g * 255 // 7) << 8) | (b * 255 // 3)
TEST_PALETTE[128:256] = TEST_PALETTE[0:128]

def decode_rl7(dst, src, width, height):
    in_cursor = 0
    out_x = 0
    out_y = 0
    while out_y < height:
        if in_cursor >= len(src):
            break
        in_byte = src[in_cursor]
        in_cursor += 1

        color = in_byte & 0x7F

        if (in_byte & 0x80):
            # Run of pixels
            if in_cursor >= len(src):
                break
            length_byte = src[in_cursor]
            in_cursor += 1

            # length_byte 0 means continue until end of line, then move to next
            # line. This is REQUIRED to end every line.
            length = (width - out_x) if length_byte == 0 else length_byte
            if length:
                dst_idx = out_y * width + out_x
                dst[dst_idx : dst_idx + length] = [color] * length
                out_x += length

            if length_byte == 0:
                out_y += 1
                out_x = 0

        else:
            # Single pixel
            dst[out_y * width + out_x] = color
            out_x += 1


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

    def open(res, widget, app):
        newWidget = MyTableWidget("Value")

        for i in range(0, len(res.data)):
            val = res.data[i]
            newWidget.add_row("{}".format(i), "0x{:02X}".format(val))

        widget.addWidget(newWidget)

@_register_res_handler(3)
class _Values16BitHandler:
    name = "16-bit Values"

    def open(res, widget, app):
        newWidget = MyTableWidget("Value")

        parsed = GreedyRange(UBInt16("value")).parse(res.data)
        for i in range(0, len(parsed)):
            newWidget.add_row("{}".format(i), "0x{:04X}".format(parsed[i]))

        widget.addWidget(newWidget)

@_register_res_handler(6)
class _ResourceListHandler:
    name = "Resource List"

    def open(res, widget, app):
        newWidget = MyTableWidget("ID")

        parsed = GreedyRange(UBInt32("value")).parse(res.data)
        for i in range(0, len(parsed)):
            newWidget.add_row("{}".format(i), "0x{:08X}".format(parsed[i]))

        widget.addWidget(newWidget)

@_register_res_handler(7)
class _SoundHandler:
    name = "Sound"

@_register_res_handler(8)
class _ImageHandler:
    name = "Image"

    def open(res, widget, app):
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
        widget.addWidget(newWidget)

@_register_res_handler(10)
class _PaletteHandler:
    name = "Palette"

    def open(res, widget, app):
        # TODO: handle planes
        app.cur_palette = [0] * 256
        for i in range(0, 128):
            r = res.data[6 + i * 3 + 0]
            g = res.data[6 + i * 3 + 1]
            b = res.data[6 + i * 3 + 2]
            app.cur_palette[i] = 0xFF000000 | (r << 16) | (g << 8) | b
        app.cur_palette[128:256] = app.cur_palette[0:128]

        widget.addWidget(QtGui.QLabel("Palette loaded"))

_BackgroundStruct = Struct("_BackgroundStruct",
    UBInt32("image_id"),
    UBInt32("palette_id"),
    UBInt32("hotspots_id"),
    UBInt32("unk_c"),
    )

@_register_res_handler(26)
class _BackgroundHandler:
    name = "Background"

    def open(res, widget, app):
        newWidget = MyTableWidget("Value")

        parsed = _BackgroundStruct.parse(res.data)
        newWidget.add_row("Image ID", "0x{:08X}".format(parsed.image_id))
        newWidget.add_row("Palette ID", "0x{:08X}".format(parsed.palette_id))
        newWidget.add_row("Hotspots ID", "0x{:08X}".format(parsed.hotspots_id))
        newWidget.add_row("Unk @C", "0x{:08X}".format(parsed.unk_c))

        widget.addWidget(newWidget)

_ButtonImageStruct = Struct("_ButtonImageStruct",
    SBInt16("x"),
    SBInt16("y"),
    UBInt32("image_id"),
    )

_ButtonImageArray = GreedyRange(_ButtonImageStruct)

# Ex: 370E
@_register_res_handler(27)
class _ButtonImageHandler:
    name = "Button Image"

    def open(res, widget, app):
        newWidget = MyTableWidget("Position", "Image ID")

        parsed = _ButtonImageArray.parse(res.data)
        for i in range(0, len(parsed)):
            newWidget.add_row("{}".format(i),
                "({}, {})".format(parsed[i].x, parsed[i].y),
                "0x{:08X}".format(parsed[i].image_id))


        widget.addWidget(newWidget)

@_register_res_handler(28)
class _ButtonColorsHandler:
    name = "Button Colors"

_ButtonPaletteModStruct = Struct("_ButtonPaletteModStruct",
    UBInt8("index"),
    UBInt8("count"),
    UBInt32("colors_id"),
    )

_ButtonPaletteModArray = GreedyRange(_ButtonPaletteModStruct)

@_register_res_handler(29)
class _ButtonPaletteHandler:
    name = "Button Palette Mod"

    def open(res, widget, app):
        newWidget = MyTableWidget("Index", "Count", "Colors ID")

        parsed = _ButtonPaletteModArray.parse(res.data)
        for i in range(0, len(parsed)):
            newWidget.add_row("{}".format(i),
                "{}".format(parsed[i].index),
                "{}".format(parsed[i].count),
                "0x{:08X}".format(parsed[i].colors_id))

        widget.addWidget(newWidget)

_BUTTON_GRAPHICS_TYPE_NAMES = {
    1: "Palette Mods",
    2: "Images",
    }

_ButtonGraphicsStruct = Struct("_ButtonGraphicsStruct",
    UBInt16("type"),
    UBInt32("unk_2"),
    UBInt32("hovered_id"),
    UBInt32("idle_id"),
    )

_ButtonGraphicsArray = GreedyRange(_ButtonGraphicsStruct)

# Ex: 69B5
@_register_res_handler(30)
class _ButtonGraphicsHandler:
    name = "Button Graphics"

    def open(res, widget, app):
        newWidget = MyTableWidget("Type", "Unk @2", "Hovered", "Idle")

        parsed = _ButtonGraphicsArray.parse(res.data)
        for i in range(0, len(parsed)):
            type_name = _BUTTON_GRAPHICS_TYPE_NAMES.get(parsed[i].type, "Unknown")
            newWidget.add_row("{}".format(i),
                "{} ({})".format(type_name, parsed[i].type),
                "0x{:08X}".format(parsed[i].unk_2),
                "0x{:08X}".format(parsed[i].hovered_id),
                "0x{:08X}".format(parsed[i].idle_id))

        widget.addWidget(newWidget)

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

_ButtonArray = GreedyRange(_ButtonStruct)

# Ex: 312D
@_register_res_handler(31)
class _ButtonsHandler:
    name = "Buttons"

    def open(res, widget, app):
        newWidget = MyTableWidget("Type", "(L, R, T, B)", "Plane", "# Graphics",
            "Unk @E", "Graphics ID")

        parsed = _ButtonArray.parse(res.data)
        for i in range(0, len(parsed)):
            type_name = _BUTTON_TYPE_NAMES.get(parsed[i].type, "Unknown")
            newWidget.add_row("{}".format(i),
                "{} ({})".format(type_name, parsed[i].type),
                "({}, {}, {}, {})".format(parsed[i].left, parsed[i].right, parsed[i].top, parsed[i].bottom),
                "{}".format(parsed[i].plane),
                "{}".format(parsed[i].num_graphics),
                "{}".format(parsed[i].unk_e),
                "0x{:08X}".format(parsed[i].graphics_id))

        widget.addWidget(newWidget)

_SceneStruct = Struct("_SceneStruct",
    UBInt32("unk_0"), # 0
    UBInt32("background_id"), # 4
    UBInt16("unk_8"), # 8
    UBInt32("unk_a"), # A
    UBInt32("unk_e"), # E
    UBInt32("unk_12"), # 12
    UBInt32("unk_16"), # 16
    UBInt16("num_buttons"), # 1A
    UBInt32("buttons_id"), # 1C
    SBInt16("origin_x"), # 20
    SBInt16("origin_y"), # 22
    )

# Ex: 3A0B
@_register_res_handler(32)
class _SceneHandler:
    name = "Scene"

    def open(res, widget, app):
        newWidget = MyTableWidget("Value")

        parsed = _SceneStruct.parse(res.data)
        newWidget.add_row("Unk @0", "0x{:08X}".format(parsed.unk_0))
        newWidget.add_row("Background ID", "0x{:08X}".format(parsed.background_id))
        newWidget.add_row("Unk @8", "0x{:04X}".format(parsed.unk_8))
        newWidget.add_row("Unk @Ah", "0x{:08X}".format(parsed.unk_a))
        newWidget.add_row("Unk @Eh", "0x{:08X}".format(parsed.unk_e))
        newWidget.add_row("Unk @12h", "0x{:08X}".format(parsed.unk_12))
        newWidget.add_row("Unk @16h", "0x{:08X}".format(parsed.unk_16))
        newWidget.add_row("# Buttons", "{}".format(parsed.num_buttons))
        newWidget.add_row("Buttons ID", "0x{:08X}".format(parsed.buttons_id))
        newWidget.add_row("Origin", "({}, {})".format(parsed.origin_x, parsed.origin_y))

        widget.addWidget(newWidget)

_MainMenuStruct = Struct("_MainMenuStruct",
    UBInt32("scene_id"),
    UBInt32("colorbars_id"),
    UBInt32("colorbars_palette_id"),
    )

# Ex: 0118
@_register_res_handler(33)
class _MainMenuHandler:
    name = "Main Menu"

    def open(res, widget, app):
        newWidget = MyTableWidget("Value")

        parsed = _MainMenuStruct.parse(res.data)
        newWidget.add_row("Scene ID", "0x{:08X}".format(parsed.scene_id))
        newWidget.add_row("Color Bars ID", "0x{:08X}".format(parsed.colorbars_id))
        newWidget.add_row("Color Bars Palette ID", "0x{:08X}".format(parsed.colorbars_palette_id))

        widget.addWidget(newWidget)

# Ex: 02A0
@_register_res_handler(34)
class _FileMenuHandler:
    name = "File Menu"

# Ex: 006E
@_register_res_handler(35)
class _DifficultyMenuHandler:
    name = "Difficulty Menu"

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

    def open(res, widget, app):
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

        widget.addWidget(newWidget)

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

_PotionComboArray = GreedyRange(_PotionComboStruct)

# Ex: 9B17
@_register_res_handler(63)
class _PotionCombosHandler:
    name = "Potion Combos"

    def open(res, widget, app):
        newWidget = MyTableWidget("A", "B", "C", "D", "Movie")
        
        parsed = _PotionComboArray.parse(res.data)
        for i in range(0, len(parsed)):
            movie_name = _POTION_MOVIE_NAMES.get(parsed[i].movie, "Unknown")
            newWidget.add_row("{}".format(i),
                "0x{:02X}".format(parsed[i].a),
                "0x{:02X}".format(parsed[i].b),
                "0x{:02X}".format(parsed[i].c),
                "0x{:02X}".format(parsed[i].d),
                "{} ({})".format(movie_name, parsed[i].movie),
                )

        widget.addWidget(newWidget)

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

def _open_hex_viewer(res, widget, app):
    widget.addWidget(MyHexViewerWidget(res.data))

class BltViewer:
    def __init__(self, argv):
        self.cur_palette = TEST_PALETTE

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
