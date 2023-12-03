#include "disk.h"
#include "stdio.h"

bool Disk_Init(Disk* disk, byte driveNumber)
{
    byte driveType;
    word cylinders, sectors, heads;
    if (!asm_disk_get_parameters(disk->id, &driveType, &cylinders, &sectors, &heads))
        return false;

    disk->id = driveNumber;
    disk->cylinders = cylinders + 1;
    disk->heads = heads + 1;
    disk->sectors = sectors;

    printf("cylinders: %d\n\rheads: %d\n\rsectors: %d\n\r", disk->cylinders, disk->heads, disk->sectors);
    printf("------------------\n\r"); 

    return true;
}

void Disk_LBA_To_CHS(Disk* disk, dword lba, word* rp_cylinders, word* rp_sectors, word* rp_heads)
{
    *rp_sectors = lba % disk->sectors + 1;

    *rp_cylinders = (lba / disk->sectors) / disk->heads;

    *rp_heads = (lba / disk->sectors) % disk->heads;
}

bool Disk_ReadSectors(Disk* disk, dword lba, byte sectors, void far* rp_data)
{
    word cylinder, sector, head;

    Disk_LBA_To_CHS(disk, lba, &cylinder, &sector, &head);

    //word sec = (word)((dword)rp_data >> 16);
    //word adr = (word)((dword)rp_data & 0xFFFF);
    printf("Reading %d sectors from CHS: %d, %d, %d, LBA: %ld, offset %x\n\r",
                    sectors,     cylinder, head, sector, lba,   0x200*lba);
    for (int i = 0; i < 3; i++)
    {   
        if (asm_disk_read(disk->id, cylinder, sector, head, sectors, rp_data))
            return true;

        asm_disk_reset(disk->id);
    }
    return false;
}
