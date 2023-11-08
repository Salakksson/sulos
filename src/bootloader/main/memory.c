#include "memory.h"


void memcpy(void far* dst, void far* src, int num)
{
    for(char* cdst = (char*)dst, *csrc = (char*)src; num; num--) *(cdst++) = *(csrc++);
}


