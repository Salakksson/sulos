#pragma once

#include "defs.h"
#include "disk.h"
#include "stdio.h"
#include "memory.h"

#pragma pack(push, 1)

typedef struct
{
    byte Name[11];
    byte Attributes;
    byte RESERVED;
    byte CreatedTimeTenths;
    word CreatedTime;
    word CreatedDate;
    word AccessedDate;
    word FirstClusterHigh;
    word ModifiedTime;
    word ModifiedDate;
    word FirstClusterLow;
    dword Size;

} FAT_DirectoryEntry;

#pragma pack(pop)

typedef struct
{
    int Handle;
    bool Directory;
    long int Position;
    long int Size;
} FAT_File;

enum FAT_Attributes
{
    FAT_ATTRIBUTE_READ_ONLY             = 0x01,
    FAT_ATTRIBUTE_HIDDEN                = 0x02,
    FAT_ATTRIBUTE_SYSTEM                = 0x04,
    FAT_ATTRIBUTE_VOLUME_ID             = 0X08,
    FAT_ATTRIBUTE_DIRECTORY             = 0x10,
    FAT_ATTRIBUTE_ARCHIVE               = 0x20,
    FAT_ATTRIBUTE_LFN                   = FAT_ATTRIBUTE_READ_ONLY       |
                                          FAT_ATTRIBUTE_HIDDEN          |
                                          FAT_ATTRIBUTE_SYSTEM          |
                                          FAT_ATTRIBUTE_VOLUME_ID       |
                                          FAT_ATTRIBUTE_DIRECTORY       |
                                          FAT_ATTRIBUTE_ARCHIVE 


};

bool FAT_ReadBootSector(Disk* disk);

bool FAT_ReadFat(Disk* disk);

bool FAT_Initialize(Disk* disk);

dword FAT_ClusterToLBA(dword cluster);

FAT_File far* FAT_OpenEntry(Disk* disk, FAT_DirectoryEntry* entry);

bool FAT_FindFile(FAT_File far* file, char* name, FAT_DirectoryEntry* rp_entry);

FAT_File* FAT_Open(Disk* disk, char* path);


