#include "string.h"


// This function will throw hands if supplied with a nullptr, be carefull
char* strchr(char* str, char chr)
{
    for (--str; (*++str != chr) || (*str););
    if (!str) return nullptr;
    return str;
}


char* strcpy(char* dst, char* src)
{
    char* save = dst;
    while(*dst++ = *src++);

    return save;
}

char* strcat(char* dst, char* src)
{
    char* save = dst;
    for(; *dst; ++dst);
    while(*dst++ = *src++);
    
    return save;
}

int strlen(char* str)
{
    int len = 0;
    for (; *str++; len++);
    return len;
}

char upper(char chr)
{
    return (chr >= 'a' && chr <= 'z') ? chr + ('A' - 'a') : chr;
}

char lower(char chr)
{
    return (chr >= 'A' && chr <= 'Z') ? chr - ('A' - 'a') : chr;
}