#pragma once

#include "defs.h"
#include "disk.h"
#include "stdio.h"
#include "memory.h"
#include "string.h"
#include "math.h"
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

static bool FAT_ReadBootSector(Disk* disk);

static bool FAT_ReadFat(Disk* disk);

static dword FAT_ClusterToLBA(dword cluster);



bool FAT_Init(Disk* disk);


FAT_File far* FAT_OpenEntry(Disk* disk, FAT_DirectoryEntry* entry);

dword FAT_NextCluster(dword currentCluster);

dword FAT_Read(Disk* disk, FAT_File far* file, dword byteCount, void* rp_data);

bool FAT_ReadEntry(Disk* disk, FAT_File far* file, FAT_DirectoryEntry* dirEntry);

void FAT_Close(FAT_File far* file);

bool FAT_FindFile(Disk* disk, FAT_File far* file, char* name, FAT_DirectoryEntry* rp_entry);

FAT_File far* FAT_Open(Disk* disk, char* path);


