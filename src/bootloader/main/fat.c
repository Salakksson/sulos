#include "fat.h"
#include "error.h"
#include "memory.h"

#define SECTOR_SIZE              512
#define MAX_FILE_HANDLES         10
#define ROOT_DIRECTORY_HANDLE    -1


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
    FAT_File Public;
    bool Opened;
    dword FirstCluster;
    dword CurrentCluster;
    dword CurrentSectorInCluster;
    byte Buffer[SECTOR_SIZE];

} FAT_FileData;


typedef struct
{
    union
    {
        FAT_BootSector BootSector;
        char BootSectorBytes[SECTOR_SIZE];
    } BS;

    FAT_FileData RootDirectory;
    FAT_FileData OpenedFiles[MAX_FILE_HANDLES];

} FAT_Data;

static FAT_Data far* g_Data;
static byte far* g_FAT = nullptr;
static dword g_DataSectionLBA;

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
    
    if (!FAT_ReadBootSector(disk))
    {
        ERROR_CODE = FAT_ReadBootSector_Code;
        ERROR_MESG = FAT_ReadBootSector_Mesg;
        return false;
    }

    g_FAT = (byte far*)g_Data + sizeof(FAT_Data);
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

    //g_RootDirectory = (FAT_DirectoryEntry far*)(g_FAT + fatSize);
    dword rootDirLBA = g_Data->BS.BootSector.ReservedSectors + g_Data->BS.BootSector.SectorsPerFat * g_Data-> BS.BootSector.FAT_Count;
    dword rootDirSize = sizeof(FAT_DirectoryEntry) * g_Data->BS.BootSector.DirEntryCount;
    //rootDirSize = align(rootDirSize, g_Data->BS.BootSector.BytesPerSector);

    if (sizeof(FAT_Data) + fatSize + rootDirSize >= MEMORY_FAT_SIZE)
    {
        ERROR_CODE = FAT_NotEnoughMemoryRoot_Code;
        ERROR_MESG = FAT_NotEnoughMemoryRoot_Mesg;
        return false;
    }

    if(!FAT_ReadRootDirectory(disk))
    {
        ERROR_CODE = FAT_ReadFailedRoot_Code;
        ERROR_MESG = FAT_ReadFailedRoot_Mesg;
        return false;   
    }

    // open root directory file
    g_Data->RootDirectory.Public.Handle = ROOT_DIRECTORY_HANDLE;
    g_Data->RootDirectory.Public.Directory = true;
    g_Data->RootDirectory.Public.Position = 0;    
    g_Data->RootDirectory.Public.Size = sizeof(FAT_DirectoryEntry) * g_Data->BS.BootSector.DirEntryCount;
    g_Data->RootDirectory.Opened = true;

    g_Data->RootDirectory.FirstCluster = 0;
    g_Data->RootDirectory.CurrentCluster = false;
    g_Data->RootDirectory.CurrentSectorInCluster = false;

    if (!DISK_ReadSectors(disk, rootDirLBA, 1, g_Data->RootDirectory.Buffer))
    {
        ERROR_CODE = FAT_ReadFailedRoot_Code;
        ERROR_MESG = FAT_ReadFailedRoot_Mesg;
        return false;   
    }
    dword rootDirSectors = (rootDirSize + g_Data->BS.BootSector.BytesPerSector - 1) / g_Data->BS.BootSector.BytesPerSector;
    g_DataSectionLBA = rootDirLBA + rootDirSectors;

    // reset opened files
    for (int i = 0; i++ < MAX_FILE_HANDLES; g_Data->OpenedFiles[i].Opened = false);

}

FAT_File* FAT_Open(Disk* disk, FAT_DirectoryEntry* entry)
{
    int handle = -1;
    for(int i = 0; i++ < MAX_FILE_HANDLES; )
    if(!g_Data->OpenedFiles[i].Opened)
        handle = 1;
    
    if (handle < 0)
    {
        ERROR_CODE = FAT_NoHandles_Code;
        ERROR_MESG = FAT_NoHandles_Mesg;
        return false;   
    }

    FAT_FileData far* fd = &g_Data->OpenedFiles[handle];
    fd->Public.
}



FAT_File* FAT_Open(Disk* disk, const char* path)
{



}