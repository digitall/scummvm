
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

"""BltFile class for reading BLT files."""

import os
import struct

def _get_file_size(file):
    cursor = file.tell()
    file.seek(0, os.SEEK_END)
    size = file.tell()
    file.seek(cursor)
    return size

def decompress_bolt_lz(dst, src):
    """Decompress BOLT-LZ compressed data.

    dst must be a bytearray sized to fit the decompressed data.
    """

    in_cursor = 0
    out_cursor = 0
    remaining = len(dst)
    while remaining > 0:
        control = src[in_cursor]
        in_cursor += 1
        comp = control >> 6
        flag = (control >> 5) & 1
        num = control & 0x1F

        if comp == 0:
            # Raw bytes
            count = 31 - num
            remaining -= count
            dst[out_cursor:out_cursor+count] = src[in_cursor:in_cursor+count]
            out_cursor += count
            in_cursor += count
        elif comp == 1:
            # Small repeat from previously-decoded window
            count = 35 - num
            remaining -= count
            offset = src[in_cursor] + (256 if flag else 0)
            in_cursor += 1
            for i in range(count):
                dst[out_cursor] = dst[out_cursor - offset]
                out_cursor += 1
        elif comp == 2:
            # Big repeat from previously-decoded window
            count = (32 - num) * 4 + (2 if flag else 0)
            remaining -= count
            offset = src[in_cursor] * 2
            in_cursor += 1
            for i in range(count):
                dst[out_cursor] = dst[out_cursor - offset]
                out_cursor += 1
        elif comp == 3 and flag:
            # Original checked for end of data in here. I check on every loop
            # iteration.
            pass
        elif comp == 3 and not flag:
            # Big block filled with single byte
            count = (32 - num + 32 * src[in_cursor]) * 4
            in_cursor += 1
            remaining -= count
            in_cursor += 1 # byte ignored!
            b = src[in_cursor]
            in_cursor += 1
            dst[out_cursor:out_cursor + count] = [b] * count
            out_cursor += count
        else:
            assert False # unreachable

class BltFile:
    def __init__(self, in_file):
        self.in_file = in_file

        magic = in_file.read(4)
        if magic != b'BOLT':
            raise ValueError("Magic BOLT value not found")

        in_file.read(7) # unknown

        num_dirs = in_file.read(1)[0]
        print("Number of directories: {}".format(num_dirs))

        self.file_size = struct.unpack('>I', in_file.read(4))[0]
        if self.file_size != _get_file_size(in_file):
            raise ValueError("Invalid file size field")

        self.dir_table = [None] * num_dirs
        for dir_num in range(num_dirs):
            class Dir:
                pass

            dir = Dir()
            dir.num = dir_num
            dir.name = "{:02X}".format(dir_num)

            dir.num_resources, dir.comp_buf_size, dir.offset, unk2 = \
                struct.unpack('>IIII', in_file.read(0x10))

            # load resource table

            dir_cursor = in_file.tell()
            in_file.seek(dir.offset)

            dir.res_table = [None] * dir.num_resources
            for res_num in range(dir.num_resources):
                class ResEntry:
                    pass

                res = ResEntry()
                res.num = res_num
                res.id = (dir_num << 8) | res_num
                res.name = "{:04X}".format(res.id)

                type_field, res.size, res.offset, unk3 = \
                    struct.unpack('>IIII', in_file.read(0x10))

                res.type = type_field & 0x00FFFFFF
                res.compression = type_field >> 24

                dir.res_table[res_num] = res

            in_file.seek(dir_cursor)

            self.dir_table[dir_num] = dir

    def load_resource(self, res_id):
        dir_num = res_id >> 8
        res_num = res_id & 0xFF

        dir_entry = self.dir_table[dir_num]
        res_entry = dir_entry.res_table[res_num]

        class Resource:
            pass

        res = Resource()
        res.dir_num = dir_num
        res.res_num = res_num
        res.id = res_entry.id
        res.name = res_entry.name
        res.type = res_entry.type
        res.offset = res_entry.offset
        res.size = res_entry.size

        self.in_file.seek(res_entry.offset)
        if res_entry.compression == 0:
            # BOLT-LZ
            compressed_data = self.in_file.read(dir_entry.comp_buf_size)
            res.data = bytearray(res_entry.size)
            decompress_bolt_lz(res.data, compressed_data)
        elif res_entry.compression == 8:
            # Raw
            res.data = self.in_file.read(res_entry.size)
        else:
            raise ValueError("Unknown compression type {}".format(res_entry.compression))

        return res
