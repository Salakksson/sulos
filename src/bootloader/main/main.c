#include "defs.h"
#include "stdio.h"
#include "cpu.h"
#include "disk.h"
#include "fat.h"



void _cdecl boot_main(word bootDrive)
{
    printf("os testing\n\r");
    Disk disk;
    if (!Disk_Init(&disk, bootDrive))
    {
        printf("Failed to initialise Disk\n\r");
        goto halt;
    }
    
    if(!FAT_Init(&disk))
    {
        printf("Failed to initialise FAT\n\r");
        goto halt;
    }

    byte data[512];
    char string[12];
    string[12] = 0;

    FAT_Read(&disk, 1, string);
    printf(string);
    printf("success?\n\r");

halt:
    for(;;);

}

