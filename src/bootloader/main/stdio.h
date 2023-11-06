#pragma once

#include "defs.h"

void _cdecl asm_tt_printc(char c, byte page);

int strlen(char* str);

char* str_reverse(char* str);

void printc_tt(char c);

void _cdecl prints_tt(char* str);

void printd_tt(byte c);

char* itoa(int value, char* str, int base);

void _cdecl printf_tt(char* str, ...);

