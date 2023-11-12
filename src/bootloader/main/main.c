#include "defs.h"
#include "stdio.h"
#include "cpu.h"
#include "fat.h"




void _cdecl boot_main(word bootDrive)
{
    Disk disk;
    if (!Disk_Init(&disk, bootDrive))
    {
        printf("Failed to initialise Disk\n");
        goto halt;
    }

    if(!FAT_Init(&disk))
    {
        printf("Failed to initialise FAT\n");
        goto halt;
    }

    FAT_File far* fd = FAT_Open(&disk, "/");
    FAT_DirectoryEntry entry;

    int i = 0;
    while (FAT_ReadEntry(&disk, fd, &entry) && i++ < 10)
    {
        printf(" ");
        for (int i = 0; i < 11; i++)
            printc(entry.Name[i]);
        printf("\n");
    }
    FAT_Close(fd);

    printf("success?\n");

halt:
    for(;;);
}

