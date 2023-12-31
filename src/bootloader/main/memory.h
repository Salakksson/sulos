#pragma once
#include "defs.h"


#define MEMORY_MIN              0x00000500
#define MEMORY_MAX              0x00080000

#define MEMORY_FAT_ADDR         ((void far*)0x00000500)
#define MEMORY_FAT_SIZE         0x00010000

void memcpy(void far* dst, void far* src, int num);

void far* memset(void far* addr, int value, word num);

int memcmp(void far* ptr1, void far* ptr2, word num);