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
import struct
import sys

from PySide import QtGui
from PySide.QtCore import Qt

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


class BltImageWidget(QtGui.QLabel):
    def __init__(self, data, palette):
        compression = data[0]
        offset_x, offset_y, width, height = struct.unpack('>hhHH', data[6:0xE])

        image_data = data[0x18:]

        if compression == 0:
            # CLUT7
            super().__init__()
            image = QtGui.QImage(image_data, width, height, width, QtGui.QImage.Format_Indexed8)
            image.setColorTable(palette)
            self.setPixmap(QtGui.QPixmap.fromImage(image).scaled(width * 2, height * 2))
        elif compression == 1:
            # RL7
            super().__init__()
            decoded_image = bytearray(width * height)
            decode_rl7(decoded_image, image_data, width, height)
            image = QtGui.QImage(decoded_image, width, height, width, QtGui.QImage.Format_Indexed8)
            image.setColorTable(palette)
            self.setPixmap(QtGui.QPixmap.fromImage(image).scaled(width * 2, height * 2))
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

        for i in range(0, len(res.data) // 2):
            val = struct.unpack('>H', res.data[2*i:][:2])[0]
            newWidget.add_row("{}".format(i), "0x{:04X}".format(val))

        widget.addWidget(newWidget)

@_register_res_handler(6)
class _ResourceListHandler:
    name = "Resource List"

    def open(res, widget, app):
        newWidget = MyTableWidget("ID")

        for i in range(0, len(res.data) // 4):
            val = struct.unpack('>I', res.data[4*i:][:4])[0]
            newWidget.add_row("{}".format(i), "0x{:08X}".format(val))

        widget.addWidget(newWidget)

@_register_res_handler(7)
class _SoundHandler:
    name = "Sound"

@_register_res_handler(8)
class _ImageHandler:
    name = "Image"

    def open(res, widget, app):
        compression = res.data[0]
        offset_x, offset_y, width, height = struct.unpack('>hhHH', res.data[6:0xE])

        newLayout = QtGui.QVBoxLayout()

        newLayout.addWidget(QtGui.QLabel("Compression: {}\nOffset: ({}, {})\nWidth: {}\nHeight: {}".format(
            compression, offset_x, offset_y, width, height)))

        newLayout.addWidget(QtGui.QLabel("Tip: Load a Palette if colors are wrong"))

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

@_register_res_handler(26)
class _BackgroundHandler:
    name = "Background"

@_register_res_handler(27)
class _ButtonImageHandler:
    name = "Button Image"

    def open(res, widget, app):
        newWidget = MyTableWidget("Position", "Image ID")

        for i in range(0, len(res.data) // 8):
            x, y, image_id = struct.unpack('>hhI', res.data[8*i:][:8])

            newWidget.add_row("{}".format(i),
                "({}, {})".format(x, y),
                "0x{:08X}".format(image_id))


        widget.addWidget(newWidget)

@_register_res_handler(28)
class _ButtonColorsHandler:
    name = "Button Colors"

@_register_res_handler(29)
class _ButtonPaletteHandler:
    name = "Button Palette Mod"

    def open(res, widget, app):
        newWidget = MyTableWidget("Index", "Count", "Colors ID")

        for i in range(0, len(res.data) // 6):
            index, count, colors_id = struct.unpack('>BBI',
                res.data[6*i:][:6])

            newWidget.add_row("{}".format(i),
                "{}".format(index),
                "{}".format(count),
                "0x{:08X}".format(colors_id))

        widget.addWidget(newWidget)

_BUTTON_GRAPHICS_TYPE_NAMES = {
    1: "Palette Mods",
    2: "Images",
    }

@_register_res_handler(30)
class _ButtonStateHandler:
    name = "Button Graphics"

    def open(res, widget, app):
        newWidget = MyTableWidget("Type", "Unk @2", "Hovered", "Idle")

        for i in range(0, len(res.data) // 0xE):
            type_, id1, id2, id3 = struct.unpack('>HIII',
                res.data[0xE*i:][:0xE])
            type_name = _BUTTON_GRAPHICS_TYPE_NAMES.get(type_, "Unknown")

            newWidget.add_row("{}".format(i),
                "{} ({})".format(type_name, type_),
                "0x{:08X}".format(id1),
                "0x{:08X}".format(id2),
                "0x{:08X}".format(id3))

        widget.addWidget(newWidget)

_BUTTON_TYPE_NAMES = {
    1: "Rectangle",
    3: "Hotspot Query",
    }

@_register_res_handler(31)
class _ButtonsHandler:
    name = "Buttons"

    def open(res, widget, app):
        newWidget = MyTableWidget("Type", "(L, R, T, B)", "Plane", "# Graphics",
            "Unk @E", "Graphics ID")

        for i in range(0, len(res.data) // 0x14):
            type_, left, right, top, bottom, plane, num_gfx, unkE, gfx_id = \
                struct.unpack('>HHHHHHHHI', res.data[0x14*i:][:0x14])
            type_name = _BUTTON_TYPE_NAMES.get(type_, "Unknown")

            newWidget.add_row("{}".format(i),
                "{} ({})".format(type_name, type_),
                "({}, {}, {}, {})".format(left, right, top, bottom),
                "{}".format(plane),
                "{}".format(num_gfx),
                "{}".format(unkE),
                "0x{:08X}".format(gfx_id))

        widget.addWidget(newWidget)

@_register_res_handler(32)
class _SceneHandler:
    name = "Scene"

@_register_res_handler(33)
class _MainMenuHandler:
    name = "Main Menu"

@_register_res_handler(34)
class _FileMenuHandler:
    name = "File Menu"

@_register_res_handler(35)
class _DifficultyMenuHandler:
    name = "Difficulty Menu"

@_register_res_handler(59)
class _PotionPuzzleHandler:
    name = "Potion Puzzle"

    def open(res, widget, app):
        label_str = "Potion Puzzle:"

        dd0_blt3, dd4_bg_image, dd8_palette, ddC = struct.unpack('>IIII', res.data[0:0x10])
        label_str += "\n@0 Resource: {:08X}".format(dd0_blt3)
        label_str += "\nBackground Image: {:08X}".format(dd4_bg_image)
        label_str += "\nPalette: {:08X}".format(dd8_palette)
        label_str += "\n@0Ch Resource: {:08X}".format(ddC)

        dd18_blt60, dd1C_blt1, dd20_blt60 = struct.unpack('>III', res.data[0x18:0x24])
        label_str += "\n@18h Ingredient Slots: {:08X}".format(dd18_blt60)
        label_str += "\n@1Ch Resource: {:08X}".format(dd1C_blt1)
        label_str += "\n@20h Ingredient Slots: {:08X}".format(dd20_blt60)

        pause_time = struct.unpack('>H', res.data[0x32:0x34])[0]
        label_str += "\nPause Time: {} ms".format(pause_time)

        for i in range(0, 7):
            sound_id = struct.unpack('>H', res.data[0x34+2*i:][:2])[0]
            label_str += "\nSound {}: {:04X}".format(i+1, sound_id)

        origin_x, origin_y = struct.unpack('>hh', res.data[0x42:0x46])
        label_str += "\nOrigin: ({}, {})".format(origin_x, origin_y)

        widget.addWidget(QtGui.QLabel(label_str))

@_register_res_handler(60)
class _PotionIngredientSlotHandler:
    name = "Potion Ingredient Slot"

@_register_res_handler(61)
class _PotionIngredientsHandler:
    name = "Potion Ingredients"

@_register_res_handler(62)
class _PotionComboListHandler:
    name = "Potion Combo List"

_POTION_MOVIE_NAMES = (
    'ELEC', 'EXPL', 'FLAM', 'FLSH', 'MIST', 'OOZE', 'SHMR',
    'SWRL', 'WIND', 'BOIL', 'BUBL', 'BSPK', 'FBRS', 'FCLD',
    'FFLS', 'FSWR', 'LAVA', 'LFIR', 'LSMK', 'SBLS', 'SCLM',
    'SFLS', 'SPRE', 'WSTM', 'WSWL', 'BUGS', 'CRYS', 'DNCR',
    'FISH', 'GLAC', 'GOLM', 'EYEB', 'MOLE', 'MOTH', 'MUDB',
    'ROCK', 'SHTR', 'SLUG', 'SNAK', 'SPKB', 'SPKM', 'SPDR',
    'SQID', 'CLOD', 'SWIR', 'VOLC', 'WORM',
    )

@_register_res_handler(63)
class _PotionCombosHandler:
    name = "Potion Combos"

    def open(res, widget, app):
        newWidget = MyTableWidget("A", "B", "C", "D", "Movie")

        for i in range(0, len(res.data) // 6):
            a, b, c, d, movie_num = struct.unpack('>BBBBH',
                res.data[6*i:][:6])

            newWidget.add_row("{}".format(i),
                "0x{:02X}".format(a),
                "0x{:02X}".format(b),
                "0x{:02X}".format(c),
                "0x{:02X}".format(d),
                "{} ({})".format(_POTION_MOVIE_NAMES[movie_num], movie_num),
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
