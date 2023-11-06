#pragma once


#include "defs.h"




#define FAT_ReadBootSector_Code                 0x0000F001
#define FAT_ReadBootSector_Mesg                 "FAT: Failed to read boot sector\n"
#define FAT_NotEnoughMemory_Code                0x0000F002
#define FAT_NotEnoughMemory_Mesg                "FAT: Not enough memory to read file allocation table\n"
#define FAT_ReadFailed_Code                     0x0000F003
#define FAT_ReadFailed_Mesg                     "FAT: Failed to read file allocation table\n"
dword ERROR_CODE;
char* ERROR_MESG;