#include "fat.h"

#define SECTOR_SIZE        512

#pragma pack(push, 1)

typedef struct
{
    byte BootInstructions[3];
    byte OEM_Identifier[8];
    word BytesPerSector;
    byte SectorsPerCluster;
    word ReservedSectors;
    byte FAT_Count;
    word DirEntryCount;
    word TotalSectors;
    byte MediaDescriptorType;
    word SectorsPerFat;
    word SectorsPerTrack;
    word Heads;
    dword HiddenSectors;
    dword LargeSectorCount;

    byte DriveNumber;
    byte RESERVED;
    byte Signature;
    dword VolumeID;
    byte VolumeLabel;
    byte SystemID;
    
} FAT_BootSector;

#pragma pack(push, 1)

typedef struct
{
    union
    {
        FAT_BootSector BootSector;
        char BootSectorBytes[SECTOR_SIZE];
    } BS;
    

} FAT_Data;

static FAT_Data far* g_Data;

byte* g_FAT = nullptr;
FAT_DirectoryEntry* g_RootDirectory = nullptr;
dword g_RootDirectoryEnd;

bool readBootSector(FILE* disk)
{
    return DISK_ReadSectors(disk, 0, 1, g_Data->BS.BootSectorBytes);

}

FAT_Initialize(DISK* disk)
