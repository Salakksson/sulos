#include "fat.h"

static FAT_Data far* g_Data;
static byte far* g_FAT = nullptr;
static dword g_DataSectionLBA;
static dword rootDirLBA;
static dword rootDirSize;
u16 FAT_Init(Disk* disk)
{
    printf("Initialising file system\n\r");
    if(!Disk_ReadSectors(disk, 0, 1, g_Data->BS.BootSectorBytes))
    {
        printf("ERROR 0xF001: FAT - Failed to read boot sector\n");
        return 0xF001;
    }
   
    printf("%s", g_Data->BS.BootSectorBytes);
    g_FAT = (byte far*)g_Data + sizeof(FAT_Data);
    dword fatSize = g_Data->BS.BootSector.BytesPerSector * g_Data->BS.BootSector.SectorsPerFat;
    if (sizeof(FAT_Data) + fatSize >= MEMORY_FAT_SIZE)
    {
        printf("ERROR 0xF002: FAT - Not enough memory to read file allocation table\n");
        return 0xF002;
    }

    printf("Reading FAT\n\r");
    if (Disk_ReadSectors(disk, g_Data->BS.BootSector.ReservedSectors, g_Data->BS.BootSector.SectorsPerFat, g_FAT))
    {
        printf("ERROR 0xF003: FAT - Failed to read file allocation table\n");
        return 0xF003;
    }

    rootDirLBA = g_Data->BS.BootSector.ReservedSectors + g_Data->BS.BootSector.SectorsPerFat * g_Data-> BS.BootSector.FAT_Count;
    rootDirSize = sizeof(FAT_DirectoryEntry) * g_Data->BS.BootSector.DirEntryCount;



    return 0;
}

u16 FAT_Read(Disk* disk, u8 index, void* rp_data)
{
    char data[512];
    Disk_ReadSectors(disk, rootDirLBA, 1, data);
    memcpy(rp_data, data + sizeof(FAT_DirectoryEntry) * index, 11);
}