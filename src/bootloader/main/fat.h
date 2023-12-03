#define SECTOR_SIZE              512
#define MAX_FILE_HANDLES         10
#define ROOT_DIRECTORY_HANDLE    -1
#define MAX_PATH_SIZE            256

#include "disk.h"
#include "defs.h"
#include "stdio.h"
#include "memory.h"

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

    // EBR

    byte DriveNumber;
    byte RESERVED;
    byte Signature;
    dword VolumeID;
    byte VolumeLabel;
    byte SystemID;
    
} FAT_BootSector;

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

typedef struct
{
    byte Buffer[SECTOR_SIZE];
    FAT_File Public;
    bool Opened;
    dword FirstCluster;
    dword CurrentCluster;
    dword CurrentSectorInCluster;

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


u16 FAT_Init(Disk* disk);

u16 FAT_Read(Disk* disk, u8 index, void* rp_data);