#pragma once

#include "defs.h"
#include "string.h"

void _cdecl asm_tt_printc(char c, byte page);

int strlen(char* str);

char* str_reverse(char* str);

void printc(char c);

void _cdecl prints(char* str);

void printd(byte c);

char* itoa(int value, char* str, int base);

void _cdecl printf(char* str, ...);

