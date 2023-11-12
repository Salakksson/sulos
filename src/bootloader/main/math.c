#include "math.h"



int max (int n, int m)
{
    switch(n > m)
    {
        case true:
            return n;
        case false:
            return m;
    }
}

int min (int n, int m)
{
    switch(n > m)
    {
        case true:
            return m;
        case false:
            return n;
    }
}

// returns length of integer not including sign
int intlen (int value, int base)
{
    int length = 0;
    int quotient = value;
    loop:
        length++;
        quotient = quotient/base;
        if (quotient != 0)
            goto loop;

    return length;
}

int align(int number, int alignTo)
{
    if (alignTo == 0)
        return number;

    int rem = number % alignTo;
    return (rem > 0) ? (number + alignTo - rem) : number;
}

dword mod(register dword num, register dword mod)
{
    if (!mod)
        return false;

    register dword cnum = num / mod;
    return num - cnum * mod;
}