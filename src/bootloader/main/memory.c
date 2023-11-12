#include "memory.h"


void memcpy(void far* dst, void far* src, int num)
{
    for(char* cdst = (char*)dst, *csrc = (char*)src; num; num--) *(cdst++) = *(csrc++);
}


void far* memset(void far* ptr, int value, word num)
{
    byte far* cptr = (byte far*)ptr;
    for (word i = 0; i < num;) cptr[i++] = (byte)value;
    return ptr;
}

int memcmp(void far* ptr1, void far* ptr2, word num)
{
    byte far* cptr1 = (byte far*)ptr1;
    byte far* cptr2 = (byte far*)ptr2;

    for (word i = 0; i < num; i++) if (cptr1[i] != cptr2[i]) return 1;
    return 0;
}