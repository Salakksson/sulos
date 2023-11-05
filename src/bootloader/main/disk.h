#include "defs.h"
#include "cpu.h"

typedef struct
{
    byte id;
    word cylinders;
    word sectors;
    word heads;

}Disk;





bool Disk_Init(Disk* disk, byte driveNumber);

void Disk_LBA_To_CHS(Disk* disk, dword lba, word* rp_cylinders, word* rp_sectors, word* rp_heads);

bool Disk_ReadSectors(Disk* disk, dword lba, byte sectors, byte far* rp_data);
