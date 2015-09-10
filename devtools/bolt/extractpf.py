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

# Requires Pillow and construct library

from collections import namedtuple
import io
import os
import struct
import sys
import wave
import PIL.Image
from construct import *
from internal.rl7 import decode_rl7

# Generate _TEST_PALETTE
_TEST_PALETTE = [0] * 768 # holds r, g, b bytes
for r in range(0, 4):
    for g in range(0, 8):
        for b in range(0, 4):
            i = 8 * 4 * r + 4 * g + b
            _TEST_PALETTE[3*i+0] = r * 255 // 3
            _TEST_PALETTE[3*i+1] = g * 255 // 7
            _TEST_PALETTE[3*i+2] = b * 255 // 3
# Repeat for both planes
_TEST_PALETTE[128:256] = _TEST_PALETTE[0:128]

class PacketType:
    HEADER = 0x0000
    AUDIO = 0x0100
    IMAGE0200 = 0x0200
    IMAGE0300 = 0x0300 # Used for big scrolling backgrounds
    UNK0400 = 0x0400 # Occurs at 26th packet of help dialogues, 0 size
    UNK0600 = 0x0600 # Occurs in ma.pf, 0 size
    UNKFE00 = 0xFE00 # Occurs at 27th packet of help dialogues, 0 size
    UNKFF00 = 0xFF00 # Occurs in ma.pf, 0 size, possibly stop marker at eof?
Packet = namedtuple('Packet', 'offset total_size partial_size type data')

def read_packet(in_file):
    offset = in_file.tell()
    header = in_file.read(10)

    total_size, partial_size, type_ = struct.unpack('>IIH', header)

    data = in_file.read(partial_size)

    return Packet(offset=offset,
        total_size=total_size,
        partial_size=partial_size,
        type=type_,
        data=data,)

# FIXME: Don't use global variable for this.
current_palette = _TEST_PALETTE

def extract_image_sequence(out_dir, name, data):
    # TODO: Reverse-engineer image header
    # unknown if "coding" is right interpretation
    coding, width, height, num_images, unk3, unk4, unk5 = \
        struct.unpack('>HHHHIII', data[0:20])

    decoded_image = bytearray(width * height)
    data_view = memoryview(data)

    coding, width, height = struct.unpack('>HHH', data[0:6])
    if coding == 0 or coding == 1: # Single image
        palette = data_view[0xD:0x18D]
        global current_palette
        current_palette = palette
        rl7_data = data_view[0x18D:]
        decode_rl7(decoded_image, rl7_data, width, height)

        out_path = os.path.join(out_dir, name + '.png')
        pil_image = PIL.Image.frombytes('P', (width, height), bytes(decoded_image))
        pil_image.putpalette(current_palette)
        pil_image.save(out_path)
    elif coding == 4: # Sequence of images
        for img_num in range(num_images):
            img_offset, img_size = struct.unpack('>II',
                data[20 + img_num * 8:20 + img_num * 8 + 8])
            rl7_data = data_view[img_offset:img_offset + img_size]
            decode_rl7(decoded_image, rl7_data, width, height)

            out_path = os.path.join(out_dir, name + '_' + str(img_num + 1) + '.png')
            pil_image = PIL.Image.frombytes('P', (width, height), bytes(decoded_image))
            global current_palette
            pil_image.putpalette(current_palette)
            # FIXME: Where does palette come from?  I think it comes from a previous
            # coding 0 or 1 image previously in the scene, but some scenes don't have
            # that.
            pil_image.save(out_path)
    else:
        raise ValueError("unknown image sequence coding " + hex(coding))

PF_COMMAND_INFO = {
    # code: ('name', param_len)
    0x01: ('BlitStream0', 0),
    0x02: ('ClearAndBlitStream1', 0),
    0x0C: ('StartPaletteCycling', 8),
    0x0D: ('StopPaletteCycling', 0),
    0x0F: ('LoadStream4', 0),
    0x10: ('BlitAndAdvanceStream4', 0),
    0x13: ('Fade', 4),
    0x7FFF: ('SetName', 4),
}

def extract_movie_header(out_dir, name, data):
    out_path = os.path.join(out_dir, name)
    with open(out_path, 'w') as out_file:
        in_stream = io.BytesIO(data)

        unk1, unk2, unk3, unk4 = struct.unpack('>IIHH', in_stream.read(0xC))
        out_file.write("unk1-4: {} {} {} {}\n".format(unk1, unk2, unk3, unk4))

        while in_stream.tell() < len(data):
            unk, cmd, rep = struct.unpack('>HHB', in_stream.read(5))
            cmd_info = PF_COMMAND_INFO.get(cmd, None)
            if cmd_info is not None:
                cmd_name = cmd_info[0]
                cmd_param_len = cmd_info[1]
            else:
                cmd_name = "Unknown{:02X}h".format(cmd)
                cmd_param_len = 0
            out_file.write("{}, ".format(unk, cmd_name))
            if rep != 1:
                out_file.write("{}*".format(rep))
            out_file.write(cmd_name)
            if cmd_param_len > 0:
                params = in_stream.read(cmd_param_len)
                for b in params:
                    out_file.write(" {:02X}".format(b))
            out_file.write('\n')


def extract_scene(out_dir, name, in_file):
    wav_path = os.path.join(out_dir, name + '.wav')
    with wave.open(wav_path, 'wb') as wav_file:
        wav_file.setparams((1, 1, 22050, 0, 'NONE', 'NONE'))

        # construct images from a stream of partial packets
        image_0200_buffer = None
        image_0200_cursor = 0
        image_0300_buffer = None
        image_0300_cursor = 0

        done = False
        header_found = False
        img_num = 1
        while not done:
            packet = read_packet(in_file)
            if packet.type == PacketType.HEADER:
                if not header_found:
                    header_found = True
                    # Extract it raw.
                    raw_path = os.path.join(out_dir, name + '_header.bin')
                    with open(raw_path, 'wb') as raw_file:
                        raw_file.write(packet.data)
                    extract_movie_header(out_dir, name + '_header.txt', packet.data)
                else:
                    # We probably stumbled onto another scene, end this one
                    # FIXME: What is correct way of finding number of packets in a scene?
                    done = True
            elif packet.type == PacketType.AUDIO:
                wav_file.writeframes(packet.data)
            elif packet.type == PacketType.IMAGE0200:
                if image_0200_buffer is None:
                    # start constructing image from partial packets
                    image_0200_buffer = bytearray(packet.total_size)
                    image_0200_cursor = 0

                image_0200_buffer[image_0200_cursor:image_0200_cursor + packet.partial_size] = \
                    packet.data
                image_0200_cursor += packet.partial_size

                if image_0200_cursor >= packet.total_size:
                    raw_path = os.path.join(out_dir, name + '_' + str(img_num) + '.bin')
                    with open(raw_path, 'wb') as raw_file:
                        raw_file.write(image_0200_buffer)
                    # encoded image completely constructed, now export it and reset
                    extract_image_sequence(out_dir, name + '_' + str(img_num), image_0200_buffer)
                    img_num += 1
                    image_0200_buffer = None
                    image_0200_cursor = 0
            elif packet.type == PacketType.IMAGE0300:
                if image_0300_buffer is None:
                    # start constructing image from partial packets
                    image_0300_buffer = bytearray(packet.total_size)
                    image_0300_cursor = 0

                image_0300_buffer[image_0300_cursor:image_0300_cursor + packet.partial_size] = \
                    packet.data
                image_0300_cursor += packet.partial_size

                if image_0300_cursor >= packet.total_size:
                    raw_path = os.path.join(out_dir, name + '_' + str(img_num) + '.bin')
                    with open(raw_path, 'wb') as raw_file:
                        raw_file.write(image_0300_buffer)
                    # encoded image completely constructed, now export it and reset
                    extract_image_sequence(out_dir, name + '_' + str(img_num), image_0300_buffer)
                    img_num += 1
                    image_0300_buffer = None
                    image_0300_cursor = 0
            elif packet.type == PacketType.UNKFF00:
                # End marker?  Occurs at end of file.
                done = True
            else:
                print("unknown packet type", hex(packet.type), "at offset",
                    hex(packet.offset))

def extract_pf_file(out_dir, in_file):
    os.makedirs(out_dir, exist_ok=True)

    magic = in_file.read(4)
    if magic != b'\xBE\xAD\x95\x00':
        raise ValueError("Magic bytes not found")

    num_blocks = struct.unpack('>I', in_file.read(4))[0]
    print("Number of blocks: " + str(num_blocks))

    for block_num in range(num_blocks):
        name = in_file.read(4).decode()
        offset = struct.unpack('>I', in_file.read(4))[0]
        print("Name: " + name + " Offset: " + hex(offset))

        cursor = in_file.tell()
        in_file.seek(offset)
        extract_scene(out_dir, name, in_file)
        in_file.seek(cursor)

def main(argv):
    print("Welcome to Merlin's Apprentice PF extractor")

    if len(argv) < 2:
        print("Please specify a PF file")
        return

    in_path = argv[1]
    with open(in_path, 'rb') as in_file:
        name = os.path.splitext(os.path.basename(in_path))[0]
        extract_pf_file(name + '_pf_extracted', in_file)

if __name__ == '__main__':
    sys.exit(main(sys.argv))
