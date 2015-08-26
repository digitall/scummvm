#!/usr/bin/env python3

import os
import struct
import sys

from construct import *

def get_file_size(file):
    cursor = file.tell()
    file.seek(0, os.SEEK_END)
    size = file.tell()
    file.seek(cursor)
    return size

CDI_SECTOR_SIZE = 2352

class SubmodeFlags:
    EOF = 0x80 # End Of File
    RT = 0x40 # Real-Time Sector
    F = 0x20 # Form (0: Form 1; 1: Form 2)
    T = 0x10 # Trigger
    D = 0x08 # Data
    A = 0x04 # Audio
    V = 0x02 # Video
    EOR = 0x01 # End Of Record

def read_cdi_sector(in_file, sector_num):

    class CDISector:
        pass

    sector = CDISector()
    sector.sector_num = sector_num
    sector.error = False
    sector.file_num = 0
    sector.channel_num = 0
    sector.submode = 0
    sector.coding_info = 0
    sector.data = b''

    in_file.seek(sector_num * CDI_SECTOR_SIZE)

    sync = in_file.read(12)
    header = in_file.read(4)
    subheader = in_file.read(8)

    if sync != b'\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00':
        print("Sync field not found (found " + str(sync) + ")")
        sector.error = True
        raise ValueError("Sync field not found")
        # return sector

    if subheader[0:4] != subheader[4:8]:
        print("Corrupt subheader field (found " + str(subheader) + ")")
        sector.error = True
        raise ValueError("Corrupt subheader field")
        # return sector

    sector.file_num = subheader[0]
    sector.channel_num = subheader[1]
    sector.submode = subheader[2]
    sector.coding_info = subheader[3]

    # I believe descrambling is already done.
    if not (sector.submode & SubmodeFlags.F):
        # Form 1
        sector.data = in_file.read(2048)
        edc = in_file.read(4)
        ecc = in_file.read(276)
        # TODO: error checking
    else:
        # Form 2
        sector.data = in_file.read(2324)
        in_file.read(4) # ignored ("reserved for quality control")

    return sector

def extract_raw_track(in_file, out_dir):
    in_file_size = get_file_size(in_file)
    num_sectors = in_file_size // CDI_SECTOR_SIZE

    out_path = os.path.join(out_dir, 'raw.bin')
    with open(out_path, 'wb') as out_file:
        for sector_num in range(0, num_sectors):
            sector = read_cdi_sector(in_file, sector_num)
            if sector.error:
                break
            out_file.write(sector.data)

def extract_file(in_file, out_file, file_lbn, file_size):
    sector_num = file_lbn
    cursor = 0
    num_form1 = 0
    num_form2 = 0
    while cursor < file_size:
        try:
            sector = read_cdi_sector(in_file, sector_num)
        except ValueError:
            print("File extraction failed")
            return

        # Count form 1 and form 2 sectors
        if not (sector.submode & SubmodeFlags.F):
            # Form 1
            num_form1 += 1
        else:
            # Form 2
            num_form2 += 1
        
        # NOTE: sector.data may have 2048 or 2324 bytes depending on the form.
        # See below.
        out_size = len(sector.data)
        if (file_size - cursor) < 2048:
            out_size = file_size - cursor

        out_file.write(sector.data[:out_size])

        # From Green Book III-20:
        # "By convention the size of Form 2 sectors is considered as 2048
        # bytes by the system"
        # FIXME: What the heck does that mean?
        # I take it to mean "even if 2324 bytes were in the sector, count it
        # as 2048 bytes."
        # Experimentation needed.
        cursor += 2048

        sector_num += 1

    print("File had {} Form 1 sectors and {} Form 2 sectors".format(num_form1, num_form2))

class Attributes:
    OWNER_READ = 0x0001
    OWNER_EXECUTE = 0x0004
    GROUP_READ = 0x0010
    GROUP_EXECUTE = 0x0040
    WORLD_READ = 0x0100
    WORLD_EXECUTE = 0x0400
    CDDA_FILE = 0x4000
    DIRECTORY = 0x8000
    
# See Green Book Figure III.8 "Structure of a directory record"
DirectoryRecord = Struct("DirectoryRecord",
    UBInt8("record_length"),
    UBInt8("extended_attribute_record_length"),
    Padding(4),
    UBInt32("file_beginning_lbn"),
    Padding(4),
    UBInt32("file_size"),
    Bytes("creation_date", 6), # See page III-21
    Padding(1),
    UBInt8("file_flags"),
    Bytes("interleave", 2),
    Padding(2),
    UBInt16("album_set_sequence_num"),
    PascalString("file_name", UBInt8("file_name_size"), 'ISO-8859-1'),
    # Padding byte here to align to an even offset
    # FIXME: Is this correct?
    Aligned(UBInt32("owner_id"), 2),
    UBInt16("attributes"),
    Padding(2),
    UBInt8("file_num"),
    Padding(1),
    )

def extract_directory(in_file, out_dir, sector_num):
    sector = read_cdi_sector(in_file, sector_num)

    cursor = 0
    index = -1
    done = False
    while cursor < CDI_SECTOR_SIZE and not done:

        index += 1
        print("\nFile entry {}:".format(index))

        record = DirectoryRecord.parse(sector.data[cursor:])

        print("Record length: {}".format(record.record_length))
        if record.record_length == 0:
            # FIXME: is this the ideal way to detect end?
            done = True
            continue

        print("Extended Attribute record length: {}".format(record.extended_attribute_record_length))
        print("File beginning LBN: {}".format(record.file_beginning_lbn))
        print("File size: {}".format(record.file_size))
        print("Interleave: {}:{}".format(record.interleave[0], record.interleave[1]))
        print("File name: {}".format(record.file_name))
        print("Owner ID: {}".format(record.owner_id))

        attributes = record.attributes
        attrs_list = []
        if attributes & Attributes.OWNER_READ:
            attrs_list.append("Owner read")
        if attributes & Attributes.OWNER_EXECUTE:
            attrs_list.append("Owner execute")
        if attributes & Attributes.GROUP_READ:
            attrs_list.append("Group read")
        if attributes & Attributes.GROUP_EXECUTE:
            attrs_list.append("Group execute")
        if attributes & Attributes.WORLD_READ:
            attrs_list.append("World read")
        if attributes & Attributes.WORLD_EXECUTE:
            attrs_list.append("World execute")
        if attributes & Attributes.CDDA_FILE:
            attrs_list.append("CD-DA file")
        if attributes & Attributes.DIRECTORY:
            attrs_list.append("Directory")
        print("Attributes: {}".format(', '.join(attrs_list)))

        print("File number: {}".format(record.file_num))

        if not (attributes & Attributes.DIRECTORY):
            # TODO: handle file nums and channel nums (for interleaved files)
            out_path = os.path.join(out_dir, record.file_name)
            with open(out_path, 'wb') as out_file:
                extract_file(in_file, out_file, record.file_beginning_lbn, record.file_size)

        cursor += record.record_length

def extract_path_table(in_file, out_dir, path_table, size):
    cursor = 0
    while cursor < size:
        name_size = path_table[cursor+0]
        ext_attr_record_len = path_table[cursor+1]
        dir_addr = struct.unpack('>I', path_table[cursor+2:cursor+6])[0]
        parent_dir_num = struct.unpack('>H', path_table[cursor+6:cursor+8])[0]
        name = path_table[cursor+8:cursor+8+name_size]

        if name != b'\x00':
            raise IOError("Directory name in path table not handled.")

        extract_directory(in_file, out_dir, dir_addr)

        # round up to even number
        cursor += 8 + name_size + (name_size % 2)

# See Green Book Figure III.1 "Structure of a File Structure Volume Descriptor"
VolumeDescriptor = Struct("VolumeDescriptor",
    UBInt8("disc_label_record_type"),
    Bytes("volume_structure_standard_id", 5),
    UBInt8("volume_structure_version_num"),
    UBInt8("volume_flags"),
    Bytes("system_id", 32),
    Bytes("volume_id", 32),
    Padding(12),
    UBInt32("volume_space_size"),
    Bytes("coded_character_set_id", 32),
    Padding(2),
    UBInt16("num_volumes_in_album"),
    Padding(2),
    UBInt16("album_set_sequence_num"),
    Padding(2),
    UBInt16("logical_block_size"),
    Padding(4),
    UBInt32("path_table_size"),
    Padding(8),
    UBInt32("path_table_address"),
    Padding(38),
    Bytes("album_id", 128),
    Bytes("publisher_id", 128),
    Bytes("data_preparer_id", 128),
    Bytes("application_id", 128),
    Bytes("copyright_file_name", 32),
    Padding(5),
    Bytes("abstract_file_name", 32),
    Padding(5),
    Bytes("bibliographic_file_name", 32),
    Padding(5),
    Bytes("creation_date_time", 16),
    Padding(1),
    Bytes("modification_date_time", 16),
    Padding(1),
    Bytes("expiration_date_time", 16),
    Padding(1),
    Bytes("effective_date_time", 16),
    Padding(1),
    UBInt8("file_structure_standard_version_num"),
    Padding(1),
    Bytes("application_use", 512),
    Padding(653),
    )

def extract_disc(in_file, out_dir):

    # disc label begins at sector 16
    sector_num = 16

    # extract all volumes
    done = False
    while not done:

        sector = read_cdi_sector(in_file, sector_num)
        if sector.error:
            raise IOError("Unable to read disc label sector")

        record_type = sector.data[0]
        print("Disc Label Record Type: {}".format(record_type))

        if record_type == 1:

            # File Structure Volume Descriptor

            volume_desc = VolumeDescriptor.parse(sector.data)

            print("Volume Structure Standard ID: {}".format(
                volume_desc.volume_structure_standard_id.decode('ISO-8859-1').rstrip()
                ))

            print("Volume Structure Version Number: {}".format(volume_desc.volume_structure_version_num))

            print("System Identifier: {}".format(volume_desc.system_id.decode('ISO-8859-1').rstrip()))

            # TODO: print more junk

            volume_id_str = volume_desc.volume_id.decode('ISO-8859-1').rstrip()
            print("Volume ID: {}".format(volume_id_str))

            volume_path = os.path.join(out_dir, volume_id_str)
            os.makedirs(volume_path, exist_ok=True)
            path_table_sector = read_cdi_sector(in_file, volume_desc.path_table_address)
            extract_path_table(in_file, volume_path, path_table_sector.data,
                volume_desc.path_table_size)

        elif record_type == 0xFF:

            # Terminator
            done = True

        else:

            print("Unknown disc label record type {}".format(record_type))
            done = True

        sector_num += 1

def main(args = sys.argv[1:]):
    print("CD-I File System Extractor")

    if len(args) == 0:
        print("Please specify a raw CD-I track file (as extracted by MESS's chdman)")
        return

    in_path = args[0]
    with open(in_path, 'rb') as in_file:
        base_name_without_ext = os.path.splitext(os.path.basename(in_path))[0]
        out_dir = base_name_without_ext + '_extracted'
        os.makedirs(out_dir, exist_ok=True)
        # extract_raw_track(in_file, out_dir)
        extract_disc(in_file, out_dir)

if __name__ == '__main__':
    main()
