/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "fitd/pak.h"
#include "fitd/unpack.h"
#include "common/file.h"
#include "common/debug.h"

namespace Fitd {
typedef struct pakInfoStruct // warning: allignement unsafe
{
	int32 discSize;
	int32 uncompressedSize;
	char compressionFlag;
	char info5;
	int16 offset;
} pakInfoStruct;

static void readPakInfo(pakInfoStruct *pPakInfo, Common::File &f) {
	pPakInfo->discSize = f.readSint32LE();
	pPakInfo->uncompressedSize = f.readSint32LE();
	pPakInfo->compressionFlag = f.readByte();
	pPakInfo->info5 = f.readByte();
	pPakInfo->offset = f.readSint16LE();
}

char *loadPak(const char *fileName, int index) {
	Common::File f;
	f.open(fileName);
	f.readUint32LE();
	uint32 fileOffset = f.readUint32LE();
	uint32 numFiles = (fileOffset / 4) - 2;
	assert(index < numFiles);
	uint32 idOffset = (index + 1) * 4;
	f.seek(idOffset, SEEK_SET);
	fileOffset = f.readUint32LE();
	f.seek(fileOffset, SEEK_SET);
	uint32 additionalDescriptorSize = f.readUint32LE();
	if (additionalDescriptorSize) {
		f.seek(additionalDescriptorSize - 4, SEEK_CUR);
	}
	pakInfoStruct pakInfo;
	readPakInfo(&pakInfo, f);
	if (pakInfo.offset) {
		Common::String name = f.readString();
		debug("Loading %s\n", name.c_str());
	}

	char *ptr = 0;
	switch (pakInfo.compressionFlag) {
	case 0: {
		ptr = (char *)malloc(pakInfo.discSize);
		f.read(ptr, pakInfo.discSize);
		break;
	}
	case 1: {
		char *compressedDataPtr = (char *)malloc(pakInfo.discSize);
		f.read(compressedDataPtr, pakInfo.discSize);
		ptr = (char *)malloc(pakInfo.uncompressedSize);

		PAK_explode((unsigned char *)compressedDataPtr, (unsigned char *)ptr, pakInfo.discSize, pakInfo.uncompressedSize, pakInfo.info5);

		free(compressedDataPtr);
		break;
	}
	case 4: {
		char *compressedDataPtr = (char *)malloc(pakInfo.discSize);
		f.read(compressedDataPtr, pakInfo.discSize);
		ptr = (char *)malloc(pakInfo.uncompressedSize);

		PAK_deflate((unsigned char *)compressedDataPtr, (unsigned char *)ptr, pakInfo.discSize, pakInfo.uncompressedSize);

		free(compressedDataPtr);
		break;
	}
	default:
		assert(false);
		break;
	}
	f.close();
	return ptr;
}

int loadPak(const char* name, int index, char* ptr)
{
    char* lptr;

    lptr = loadPak(name,index);

    memcpy(ptr,lptr,getPakSize(name,index));

    free(lptr);

    return(1);
}

int getPakSize(const char* name, int index)
{
    int32 fileOffset;
    int32 additionalDescriptorSize;
    pakInfoStruct pakInfo;
    int32 size=0;

	Common::File f;
	f.open(name);

	f.seek((index+1)*4, SEEK_SET);

	fileOffset = f.readSint32LE();
	f.seek(fileOffset, SEEK_SET);

	additionalDescriptorSize = f.readSint32LE();
	readPakInfo(&pakInfo, f);

	f.seek(pakInfo.offset, SEEK_CUR);

	if(pakInfo.compressionFlag == 0) // uncompressed
	{
		size = pakInfo.discSize;
	}
	else if(pakInfo.compressionFlag == 1) // compressed
	{
		size = pakInfo.uncompressedSize;
	}
	else if(pakInfo.compressionFlag == 4)
	{
		size = pakInfo.uncompressedSize;
	}

	f.close();

return size;
}

}
