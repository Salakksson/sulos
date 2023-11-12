#include "fat.h"


#define SECTOR_SIZE              512
#define MAX_FILE_HANDLES         10
#define ROOT_DIRECTORY_HANDLE    -1
#define MAX_PATH_SIZE            256


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

static bool FAT_ReadBootSector(Disk* disk)
{
    return Disk_ReadSectors(disk, 0, 1, g_Data->BS.BootSectorBytes);
}

static bool FAT_ReadFat(Disk* disk)
{
    return Disk_ReadSectors(disk, g_Data->BS.BootSector.ReservedSectors, g_Data->BS.BootSector.SectorsPerFat, g_FAT);
}

static dword FAT_ClusterToLBA(dword cluster)
{
    return g_DataSectionLBA + (cluster - 2) * g_Data->BS.BootSector.SectorsPerCluster;
}




bool FAT_Init(Disk* disk)
{
    printf("FAT - Initialising file system\n");
    if (!FAT_ReadBootSector(disk))
    {
        printf("ERROR 0x0000F0001: FAT - Failed to read boot sector\n");
        return false;
    }

    g_FAT = (byte far*)g_Data + sizeof(FAT_Data);
    dword fatSize = g_Data->BS.BootSector.BytesPerSector * g_Data->BS.BootSector.SectorsPerFat;
    if (sizeof(FAT_Data) + fatSize >= MEMORY_FAT_SIZE)
    {
        printf("ERROR 0x0000F0002: FAT - Not enough memory to read file allocation table\n");
        return false;
    }

    if (!FAT_ReadFat(disk))
    {
        printf("ERROR 0x0000F0003: FAT - Failed to read file allocation table\n");
        return false;
    }


    dword rootDirLBA = g_Data->BS.BootSector.ReservedSectors + g_Data->BS.BootSector.SectorsPerFat * g_Data-> BS.BootSector.FAT_Count;
    dword rootDirSize = sizeof(FAT_DirectoryEntry) * g_Data->BS.BootSector.DirEntryCount;

    // open root directory file
    g_Data->RootDirectory.Public.Handle = ROOT_DIRECTORY_HANDLE;
    g_Data->RootDirectory.Public.Directory = true;
    g_Data->RootDirectory.Public.Position = 0;    
    g_Data->RootDirectory.Public.Size = sizeof(FAT_DirectoryEntry) * g_Data->BS.BootSector.DirEntryCount;
    g_Data->RootDirectory.Opened = true;

    g_Data->RootDirectory.FirstCluster = 0;
    g_Data->RootDirectory.CurrentCluster = false;
    g_Data->RootDirectory.CurrentSectorInCluster = false;

    if (!Disk_ReadSectors(disk, rootDirLBA, 1, g_Data->RootDirectory.Buffer))
    {
        printf("ERROR 0x0000F0005: FAT - Failed to read root directory\n");
        return false;   
    }
    dword rootDirSectors = (rootDirSize + g_Data->BS.BootSector.BytesPerSector - 1) / g_Data->BS.BootSector.BytesPerSector;
    g_DataSectionLBA = rootDirLBA + rootDirSectors;

    // reset opened files
    for (int i = 0; i++ < MAX_FILE_HANDLES; g_Data->OpenedFiles[i].Opened = false);

    return true;
}


FAT_File far* FAT_OpenEntry(Disk* disk, FAT_DirectoryEntry* entry)
{
    int handle = -1;
    for(int i = 0; i++ < MAX_FILE_HANDLES; )
    if(!g_Data->OpenedFiles[i].Opened)
        handle = 1;
    
    if (handle < 0)
    {
        printf("ERROR 0x0000F0006: FAT - There are no more file handles\n");
        return nullptr;   
    }

    FAT_FileData far* fd = &g_Data->OpenedFiles[handle];
    fd->Public.Directory = (entry->Attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
    fd->Public.Position = 0;
    fd->Public.Size = 0;
    fd->FirstCluster = entry->FirstClusterLow + ((dword)entry->FirstClusterHigh << 16);
    fd->CurrentCluster = fd->FirstCluster;
    fd->CurrentSectorInCluster = 0; 

    if(!Disk_ReadSectors(disk, FAT_ClusterToLBA(fd->CurrentCluster), 1, fd->Buffer))
    {
        printf("ERROR 0x0000F0003: FAT - Failed to read entry\n");
        return nullptr;     
    }
    fd->Opened = true;
    return &fd->Public;
}

dword FAT_NextCluster(dword currentCluster)
{
    dword fatIndex = currentCluster * 3/2;
    return (currentCluster % 2 == 0)
        ? (*(word*)(g_FAT + fatIndex)) & 0x0FFF
        : (*(word*)(g_FAT + fatIndex)) >> 4;
}

dword FAT_Read(Disk* disk, FAT_File far* file, dword byteCount, void* rp_data)
{
    FAT_FileData far* fd = (file->Handle == ROOT_DIRECTORY_HANDLE)
        ? &g_Data->RootDirectory
        : &g_Data->OpenedFiles[file->Handle];

        byte* data = (byte*)rp_data;

    byteCount = min(byteCount, fd->Public.Size - fd->Public.Position);
    while (byteCount > 0)
    {
        dword leftInBuffer = SECTOR_SIZE - mod(fd->Public.Position, SECTOR_SIZE);
        dword take = min (byteCount, leftInBuffer);

        memcpy(data, fd->Buffer + mod(fd->Public.Position, SECTOR_SIZE), take);
        data += take;
        fd->Public.Position += take;
        byteCount -= take;

        if (byteCount > 0)
        {
            if (fd->Public.Handle == ROOT_DIRECTORY_HANDLE)
            {
                ++fd->CurrentCluster;

                if (!Disk_ReadSectors(disk, fd->CurrentCluster, 1, fd->Buffer))
                {
                    printf("ERROR: FAT - Failed to read from disk\n");
                    break;
                }
            }
            else
            {
                if (++fd->CurrentSectorInCluster >= g_Data->BS.BootSector.SectorsPerCluster)
                {
                    fd->CurrentSectorInCluster = 0;
                    fd->CurrentCluster = FAT_NextCluster(fd->CurrentCluster);
                }

                if(fd->CurrentCluster >= 0xFF8)
                {
                    fd->Public.Size = fd->Public.Position;
                    break;
                }

                if (!Disk_ReadSectors(disk, FAT_ClusterToLBA(fd->CurrentCluster) + fd->CurrentSectorInCluster, 1, fd->Buffer))
                {
                    printf("ERROR: FAT - Failed to read from disk\n");
                    break;
                }
            }
            
        }
    }

    return data - (byte*)rp_data;
}

bool FAT_ReadEntry(Disk* disk, FAT_File far* file, FAT_DirectoryEntry* dirEntry)
{
    return FAT_Read(disk, file, sizeof(FAT_DirectoryEntry), dirEntry) == sizeof(FAT_DirectoryEntry);
}

void FAT_Close(FAT_File far* file)
{
    if (file->Handle = ROOT_DIRECTORY_HANDLE)
    {
        file->Position = 0;
        g_Data->RootDirectory.CurrentCluster = g_Data->RootDirectory.FirstCluster;
    }
    else
    {
        g_Data->OpenedFiles[file->Handle].Opened = false;
    }
}

bool FAT_FindFile(Disk* disk, FAT_File far* file, char* name, FAT_DirectoryEntry* rp_entry)
{
    char fatName[11];
    FAT_DirectoryEntry entry;

    memset(fatName, ' ', sizeof(fatName));
    char* ext = strchr(name, '.');
    if (ext == nullptr)
        ext = name + 11;
    
    
    for (int i = 0; i < 8 && name + i < ext; i++)
        fatName[i] = upper(name[i]);

    if (ext != nullptr)
    {
        for (int i = 0; i < 3 && ext[i]; i++)
            fatName[i+8] = ext[i];
    }
    while (FAT_ReadEntry(disk, file, &entry))
    {
        if (memcmp(fatName, entry.Name, 11) == 0)
        {
            *rp_entry = entry;
            return true;
        }
    }

    return false;
}

FAT_File far* FAT_Open(Disk* disk, char* path)
{
    char name[MAX_PATH_SIZE];

    if (path[0] == '/')
        path++;

    FAT_File far* parent = nullptr;
    FAT_File far* current = &g_Data->RootDirectory.Public;

    while (*path)
    {
        bool isLast = false;
        char* delim = strchr(path, '/');
        if (delim != nullptr)
        {
            memcpy(name, path, delim - path);
            name[delim - path + 1] = null;
            path = ++delim;
        }
        else
        {
            word len = strlen(path);
            memcpy(name, path, len);
            name[delim - path + 1] = 0;

        }

        FAT_DirectoryEntry entry;
        if (!FAT_FindFile(disk, current, name, &entry))
        {
            if (!isLast && entry.Attributes & FAT_ATTRIBUTE_DIRECTORY == 0)
            {
                printf("FAT: Cannot find directory %s", name);
                return nullptr;
            }
            FAT_Close(current);
            current = FAT_OpenEntry(disk, &entry);

        }
        else
        {
            FAT_Close(current);

            printf("ERROR: FAT - Unable to find %s\n", name);
            return nullptr;
        }
    }

    return current;
}
