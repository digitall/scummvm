#!/usr/bin/env python3

"""BLT Viewer main program."""

import io
import struct
import sys
import wave
import winsound

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
                dst[out_y * width + out_x : out_y * width + out_x + length] = \
                    [color] * length
                out_x += length

            if length_byte == 0:
                out_y += 1
                out_x = 0

        else:
            # Single pixel
            dst[out_y * width + out_x] = color
            out_x += 1

# RES_TYPE_HANDLERS = {
#     1: "8-bit Values",
#     3: "16-bit Values",
#     6: "Resource List",
#     7: "Sound",
#     8: "Image",
#     10: "Palette",
#     26: "Background",
#     27: "Button Image",
#     28: "Button Colors",
#     29: "Button Palette",
#     30: "Button State",
#     31: "Button",
#     32: "Scene",
#     33: "Main Menu",
#     34: "File Menu",
#     35: "Difficulty Menu",
#     59: "Potion Puzzle",
#     60: "Potion Ingredient Slot",
#     61: "Potion Ingredients",
#     62: "Potion Combo List",
#     63: "Potion Combos",
# }

RES_TYPE_HANDLERS = {}

def _register_res_handler(type):
    def decorate(cls):
        RES_TYPE_HANDLERS[type] = cls
    return decorate

@_register_res_handler(1)
class Values8BitHandler:
    name = "8-bit Values"

    def open(res, widget, app):
        newWidget = QtGui.QTableWidget()

        count = len(res.data)
        newWidget.setRowCount(count)
        newWidget.setColumnCount(1)
        newWidget.setHorizontalHeaderLabels(("Value",))

        for i in range(0, count):
            val = res.data[i]

            headerItem = QtGui.QTableWidgetItem("{}".format(i))
            headerItem.setFlags(Qt.NoItemFlags)
            newWidget.setVerticalHeaderItem(i, headerItem)

            valueItem = QtGui.QTableWidgetItem("0x{:02X}".format(val))
            valueItem.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
            newWidget.setItem(i, 0, valueItem)

        widget.addWidget(newWidget)

@_register_res_handler(3)
class Values16BitHandler:
    name = "16-bit Values"

    def open(res, widget, app):
        newWidget = QtGui.QTableWidget()

        count = len(res.data) // 2
        newWidget.setRowCount(count)
        newWidget.setColumnCount(1)
        newWidget.setHorizontalHeaderLabels(("Value",))

        for i in range(0, count):
            val = struct.unpack('>H', res.data[2*i : 2*i+2])[0]

            headerItem = QtGui.QTableWidgetItem("{}".format(i))
            headerItem.setFlags(Qt.NoItemFlags)
            newWidget.setVerticalHeaderItem(i, headerItem)

            valueItem = QtGui.QTableWidgetItem("0x{:04X}".format(val))
            valueItem.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
            newWidget.setItem(i, 0, valueItem)

        widget.addWidget(newWidget)

@_register_res_handler(6)
class ResourceListHandler:
    name = "Resource List"

    def open(res, widget, app):
        newWidget = QtGui.QTableWidget()

        count = len(res.data) // 4
        newWidget.setRowCount(count)
        newWidget.setColumnCount(1)
        newWidget.setHorizontalHeaderLabels(("Resource ID",))

        for i in range(0, count):
            val = struct.unpack('>I', res.data[4*i : 4*i+4])[0]

            headerItem = QtGui.QTableWidgetItem("{}".format(i))
            headerItem.setFlags(Qt.NoItemFlags)
            newWidget.setVerticalHeaderItem(i, headerItem)

            valueItem = QtGui.QTableWidgetItem("0x{:08X}".format(val))
            valueItem.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
            newWidget.setItem(i, 0, valueItem)

        widget.addWidget(newWidget)

@_register_res_handler(7)
class SoundHandler:
    name = "Sound"

    #FIXME: sounds don't work.
    def open(res, widget, app):
        class SoundPlayer:
            def __init__(self, raw_data):
                # create in-memory .wav file out of raw data
                wav_io = io.BytesIO()
                with wave.open(wav_io, 'wb') as wav_out:
                    wav_out.setparams((1, 1, 22050, 0, 'NONE', 'NONE'))
                    wav_out.writeframes(raw_data)

                self.wav_data = bytes(wav_io.getbuffer())

            def play_action(self):
                # FIXME: This code is never reached...
                print("Playing sound...")
                winsound.PlaySound(self.wav_data, winsound.SND_MEMORY)

        player = SoundPlayer(res.data)

        newWidget = QtGui.QWidget()
        newLayout = QtGui.QVBoxLayout()
        button = QtGui.QPushButton("Play")
        # FIXME: This doesn't work, possibly because of a PySide bug.
        # I CAN connect to a global function.
        button.clicked.connect(player.play_action)
        newLayout.addWidget(button)
        newWidget.setLayout(newLayout)

        widget.addWidget(newWidget)

@_register_res_handler(8)
class ImageHandler:
    name = "Image"

    def open(res, widget, app):
        compression = res.data[0]
        offset_x, offset_y, width, height = struct.unpack('>hhHH', res.data[6:0xE])

        newLayout = QtGui.QVBoxLayout()

        newLayout.addWidget(QtGui.QLabel("Compression: {}\nOffset: ({}, {})\nWidth: {}\nHeight: {}".format(
            compression, offset_x, offset_y, width, height)))

        if compression == 0:
            # CLUT7
            image_data = res.data[0x18:]
            image = QtGui.QImage(image_data, width, height, width, QtGui.QImage.Format_Indexed8)
            image.setColorTable(app.cur_palette)
            imagePixmap = QtGui.QPixmap.fromImage(image).scaled(width * 2, height * 2)
            imageWidget = QtGui.QLabel()
            imageWidget.setPixmap(imagePixmap)
            newLayout.addWidget(imageWidget)
        elif compression == 1:
            # RL7
            image_data = bytearray(width * height)
            rl7_data = res.data[0x18:]
            decode_rl7(image_data, rl7_data, width, height)
            image = QtGui.QImage(image_data, width, height, width, QtGui.QImage.Format_Indexed8)
            image.setColorTable(app.cur_palette)
            imagePixmap = QtGui.QPixmap.fromImage(image).scaled(width * 2, height * 2)
            imageWidget = QtGui.QLabel()
            imageWidget.setPixmap(imagePixmap)
            newLayout.addWidget(imageWidget)
        else:
            print("Unhandled image compression {}".format(compression))

        newWidget = QtGui.QWidget()
        newWidget.setLayout(newLayout)
        widget.addWidget(newWidget)

@_register_res_handler(10)
class PaletteHandler:
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

@_register_res_handler(27)
class ButtonImageHandler:
    name = "Button Image"

    def open(res, widget, app):
        x, y, image_id = struct.unpack('>hhI', res.data[0:8])

        widget.addWidget(QtGui.QLabel("Position: ({}, {})\nImage ID: {:08X}".format(
            x, y, image_id)))

@_register_res_handler(59)
class PotionPuzzleHandler:
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
            sound_id = struct.unpack('>H', res.data[0x34+2*i : 0x34+2*i+2])[0]
            label_str += "\nSound {}: {:04X}".format(i+1, sound_id)

        origin_x, origin_y = struct.unpack('>hh', res.data[0x42:0x46])
        label_str += "\nOrigin: ({}, {})".format(origin_x, origin_y)

        widget.addWidget(QtGui.QLabel(label_str))

POTION_MOVIE_NAMES = (
    'ELEC', 'EXPL', 'FLAM', 'FLSH', 'MIST', 'OOZE', 'SHMR',
    'SWRL', 'WIND', 'BOIL', 'BUBL', 'BSPK', 'FBRS', 'FCLD',
    'FFLS', 'FSWR', 'LAVA', 'LFIR', 'LSMK', 'SBLS', 'SCLM',
    'SFLS', 'SPRE', 'WSTM', 'WSWL', 'BUGS', 'CRYS', 'DNCR',
    'FISH', 'GLAC', 'GOLM', 'EYEB', 'MOLE', 'MOTH', 'MUDB',
    'ROCK', 'SHTR', 'SLUG', 'SNAK', 'SPKB', 'SPKM', 'SPDR',
    'SQID', 'CLOD', 'SWIR', 'VOLC', 'WORM',
    )

@_register_res_handler(63)
class PotionCombosHandler:
    name = "Potion Combos"

    def open(res, widget, app):
        newWidget = QtGui.QTableWidget()

        count = len(res.data) // 6
        newWidget.setRowCount(count)
        newWidget.setColumnCount(5)
        newWidget.setHorizontalHeaderLabels(("A", "B", "C", "D", "Movie"))

        for i in range(0, count):
            a, b, c, d, movie_num = struct.unpack('>BBBBH',
                res.data[6*i : 6*i+6])

            headerItem = QtGui.QTableWidgetItem("{}".format(i))
            headerItem.setFlags(Qt.NoItemFlags)
            newWidget.setVerticalHeaderItem(i, headerItem)

            item = QtGui.QTableWidgetItem("0x{:02X}".format(a))
            item.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
            newWidget.setItem(i, 0, item)

            item = QtGui.QTableWidgetItem("0x{:02X}".format(b))
            item.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
            newWidget.setItem(i, 1, item)

            item = QtGui.QTableWidgetItem("0x{:02X}".format(c))
            item.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
            newWidget.setItem(i, 2, item)

            item = QtGui.QTableWidgetItem("0x{:02X}".format(d))
            item.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
            newWidget.setItem(i, 3, item)

            movie_str = "{} ({})".format(POTION_MOVIE_NAMES[movie_num], movie_num)
            item = QtGui.QTableWidgetItem(movie_str)
            item.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
            newWidget.setItem(i, 4, item)

        widget.addWidget(newWidget)


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
        self.tree.setHeaderLabels(("Name", "Type"))
        self.tree.itemActivated.connect(self._tree_item_activated_action)

        self.content = QtGui.QStackedWidget()
        self.content.addWidget(QtGui.QLabel("Please open a BLT file"))

        splitter = QtGui.QSplitter()
        splitter.addWidget(self.tree)
        splitter.addWidget(self.content)

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
                handler = RES_TYPE_HANDLERS.get(res.type)
                if handler:
                    res_item.setText(1, "{} ({})".format(handler.name, res.type))
                else:
                    res_item.setText(1, "{}".format(res.type))

                res_item.setData(0, Qt.UserRole, res.id)

                dir_item.addChild(res_item)

            self.tree.addTopLevelItem(dir_item)

        self.content.removeWidget(self.content.currentWidget())
        self.content.addWidget(QtGui.QLabel("Please double-click on a resource"))

    def _load_resource(self, res_id):
        self.content.removeWidget(self.content.currentWidget())

        res = self.blt_file.load_resource(res_id)
        handler = RES_TYPE_HANDLERS.get(res.type)
        if handler and hasattr(handler, "open"):
            handler.open(res, self.content, self)
        else:
            self.content.addWidget(QtGui.QLabel("This resource cannot be viewed"))

def main(argv):
    app = BltViewer(argv)
    return app.exec_()

if __name__=='__main__':
    sys.exit(main(sys.argv))
