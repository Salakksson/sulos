#include "defs.h"

typedef struct
{
    byte id;
    word cylinders;
    word sectors;
    word heads;

}Disk;

bool disk_Init(Disk* disk, byte driveNumber);

bool disk_ReadSectors(Disk* disk, dword lba, byte sectors, byte far* rp_data);
