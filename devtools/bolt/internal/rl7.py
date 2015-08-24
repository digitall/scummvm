
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

"""Methods for decoding RL7 graphics."""

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

            # Every line must end with a run-length code of 0, which means to
            # draw to the end of the line and move to the next line. This code
            # is required even if no pixels are drawn.
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
