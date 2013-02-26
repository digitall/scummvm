#ifndef RESOURCE_H
#define RESOURCE_H

#include "common/file.h"
#include "common/hashmap.h"
#include "common/debug.h"

#include "script.h"
#include "aesop.h"

namespace Aesop
{

#include "common/pack-start.h"

struct RESGlobalHeader {
	char signature[16];
	uint32 fileSize;
	uint32 lostSpace;
	uint32 firstDirectoryBlock;
	uint32 createTime;
	uint32 modifyTime;
} PACKED_STRUCT;

struct RESDirectoryBlock {
	uint32 nextDirectoryBlock;
	byte dataAttributes[128];
	uint32 entryHeaderIndex[128];
} PACKED_STRUCT;

struct RESEntryHeader {
	uint32 storageTime;
	uint32 dataAttributes;
	uint32 dataSize;
} PACKED_STRUCT;

struct RESTableHeader {
	uint16 count;
	uint32 offsets[512];
} PACKED_STRUCT;

struct PaletteHeader {
	uint16 ncolors;
	uint16 RGB;
	uint16 fade[11];
} PACKED_STRUCT;

struct RGB {
	byte r;
	byte g;
	byte b;
} PACKED_STRUCT;

#include "common/pack-end.h"

class Resource {
public:
	Resource();
	~Resource();
	uint32 id;
	byte *data;                    // pointer to resource data
	uint32 size;                   // size of resource in bytes
	Thunk *thunk;
};

typedef Common::HashMap<Common::String, uint32, Common::IgnoreCase_Hash, Common::IgnoreCase_EqualTo> ResourceMap;
typedef Common::HashMap<uint32, Resource *> ResourceCache;
typedef Common::HashMap<Common::String, CodeResource, Common::IgnoreCase_Hash, Common::IgnoreCase_EqualTo> CodeResourceMap;

class ResourceManager {
public:
	~ResourceManager();
	void init(const char *filename);
	uint32 getResourceId(const char *resourceName);
	Resource* getResource(uint32 resourceId);
	CodeResource getCodeResource(const char *codeResourceName);

private:
	void loadResourceNames();
	void loadCodeResources();
	uint32 searchNameDir(uint32 resourceId);
	uint32 seek(uint32 resourceId);

	Common::File *_file;
	RESGlobalHeader *_header;
	uint32 _currentDirBlockNumber;
	int32 _currentDirBlockPos;
	RESDirectoryBlock _currentDirBlock;
	ResourceMap *_resourceNames;
	ResourceCache *_resourceCache;
	CodeResourceMap *_codeResources;
};

}	// End of namespace Aesop

#endif
