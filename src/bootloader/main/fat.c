#include "fat.h"
#include "error.h"
#include "memory.h"

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
static byte far* g_FAT = nullptr;
static FAT_DirectoryEntry* g_RootDirectory = nullptr;
static dword g_RootDirectoryEnd;

bool FAT_ReadBootSector(Disk* disk)
{
    return Disk_ReadSectors(disk, 0, 1, g_Data->BS.BootSectorBytes);
}

bool FAT_ReadFat(Disk* disk)
{
    return Disk_ReadSectors(disk, g_Data->BS.BootSector.ReservedSectors, g_Data->BS.BootSector.SectorsPerFat, g_FAT);
}
bool FAT_Initialize(Disk* disk)
{
    g_Data = (FAT_Data far*)MEMORY_FAT_ADDR;
    if (!FAT_ReadBootSector(disk))
    {
        ERROR_CODE = FAT_ReadBootSector_Code;
        ERROR_MESG = FAT_ReadBootSector_Mesg;
        return false;
    }

    g_FAT = (byte far*)g_Data + dizeof(FAT_Data);
    dword fatSize = g_Data->BS.BootSector.BytesPerSector * g_Data->BS.BootSector.SectorsPerFat;
    if (sizeof(FAT_Data) + fatSize >= MEMORY_FAT_SIZE)
    {
        ERROR_CODE = FAT_NotEnoughMemory_Code;
        ERROR_MESG = FAT_NotEnoughMemory_Mesg;
        return false;
    }

    if (!FAT_ReadFat(disk))
    {
        ERROR_CODE = FAT_ReadFailed_Code;
        ERROR_MESG = FAT_ReadFailed_Mesg;
        return false;
    }
    
}
