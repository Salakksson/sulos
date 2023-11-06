#pragma once
#include "defs.h"
#include "disk.h"

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

bool FAT_Initialise(Disk* disk);
FAT_File far* FAT_Open(Disk* disk, const char* path);
long int FAT_Read(Disk* disk, FAT_File far* file, long int byteCount, void* rp_data);
bool FAT_ReadEntry(Disk* disk, FAT_File far* file, FAT_DirectoryEntry* dirEntry);