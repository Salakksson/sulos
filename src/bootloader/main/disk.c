#include "disk.h"

bool Disk_Init(Disk* disk, byte driveNumber)
{
    byte driveType;
    word cylinders, sectors, heads;
    if (!asm_disk_get_parameters(disk->id, &driveType, &cylinders, &sectors, &heads))
        return false;


    disk->id = driveNumber;
    disk->cylinders = cylinders;
    disk->heads = heads;
    disk->sectors = sectors;

    return true;
}

void Disk_LBA_To_CHS(Disk* disk, dword lba, word* rp_cylinders, word* rp_sectors, word* rp_heads)
{
    *rp_sectors = lba % disk->sectors + 1;

    *rp_cylinders = (lba / disk->sectors) / disk->heads;

    *rp_heads = (lba / disk->sectors) % disk->heads;
}

bool Disk_ReadSectors(Disk* disk, dword lba, byte sectors, byte far* rp_data)
{
    int attempts = 3;
    bool ok;
    word cylinder, sector, head;

    Disk_LBA_To_CHS(disk, lba, &cylinder, &sector, &head);


    retry:
    ok = asm_disk_read(disk->id, cylinder, sector, head, sectors, rp_data);
    if (ok)
        return true;
    asm_disk_reset(disk->id);
    attempts--;
    goto retry;

    return false;
}
