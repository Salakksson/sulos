#include "stdio.h"

int strlen(char* str)
{
    char* cstr = str;
    while (*cstr)
    {
        cstr++;
    }
    return cstr-str;
}

//sometimes shit can fuck up if length is longer than an integer, need to implement size_t or whatever
char* str_reverse(char* str)
{
    int len = strlen(str)-1;
    char buffer;
    int s = 0;
    for (s = 0; s < (len+1)/2; s++)
    {
        buffer = str[s];
        str[s] = str[len-s];
        str[len-s] = buffer;
    }
    return str;
}

void printc_tt(char c)
{
    asm_tt_printc(c, 0);
}

void _cdecl prints_tt(char* str)
{
    while(*str)
    {
        asm_tt_printc(*str, 0);
        if (*str == '\n')
            asm_tt_printc('\r', 0);
        str++;
    }

}

void printd_tt(byte c)
{
    switch (c < 10)
    {
        case true:
            printc_tt(c + 0x30);
            break;
        case false:
            printc_tt(c + 0x37);
    }
}

char* itoa(int value, char* str, int base)
{
    int i = 0, sign;

    if ((sign = value) < 0)             /* record sign */
        value = -value;                 /* make n positive */

    loop:
    switch (value % base < 0xA)
    {
        case true:
            str[i++] = (value % base + '0');
            break;
        case false:
            str[i++] = (value % base + 'A' - 0xA);
            break;
    }

    if ((value /= base) > 0)
    goto loop;

    if (sign < 0)
        str[i++] = '-';

    str_reverse(str);
    str[i] = '\0';
    return str;
}

// My implementation of printf
// max 255 args
void _cdecl printf_tt(char* str, ...)
{
    // if (!str[0])
    // {
    //     return;
    // }
            // wcc has a bug which makes this fuck the code up

    int* argv = (int*)&str + sizeof(str)/sizeof(int);        // Use stack manipulation to find arg array

    char* buffer;
    char* argstr;
    int state = 1;
    int width = 0;
    bool padding = false;

    int number;
    char int_str[10];

    int len;
    int pads;

    while(state)
    {
        switch (state)
        {
            case 1:
                if (*str == 0)
                {
                    state = 0; // end printing
                    break;
                }
                if (*str == '%')
                {
                    if (*(str+1) != '%')
                    {
                        str++;
                        state = 3; // print argument
                        break;
                    }
                    printc_tt('%');
                    str+= 2;
                }
                printc_tt(*str);
                if (*str == '\n')
                    printc_tt('\r');
                str++;
                break;
            case 2:
                padding = false;
                if (*str == '0')
                {
                    padding = true;
                    str++;
                }
                while (*str >= '0' && *str <= '9')
                {
                    width*= 10;
                    width+= (*str - '0');
                }
                state = 3;
                break;
            case 3:
                switch (*str)
                {
                    case 's':
                        argstr = *argv;
                        while (*argstr)
                        {
                            printc_tt(*argstr);
                            if (*argstr == '\n')
                                printc_tt('\r');
                            argstr++;

                        }
                        argv += sizeof(char*)/sizeof(int);
                        state = 1;
                        str++;
                        break;
                    case 'c':
                        printc_tt(*argv);
                        if (*argv == '\n')
                            printc_tt('r');
                        argv += sizeof(char)/sizeof(int);
                        state = 1;
                        str++;
                        break;
                    case 'd':
                        number = *argv;
                        int_str;
                        itoa(number, int_str, 10);

                        len = strlen(int_str);
                        pads = width - len;

                        while (padding && (pads > 0))
                        {
                            printc_tt('0');         // print zeros for padding
                            pads--;
                        }
                        buffer = int_str;
                        while (*buffer)
                        {
                            printc_tt(*buffer);
                            buffer++;
                        }
                        argv += sizeof(char*)/sizeof(int);
                        state = 1;
                        str++;
                        break;
                    case 'x':
                        number = *argv;
                        int_str;
                        itoa(number, int_str, 16);

                        len = strlen(int_str);
                        pads = width - len;

                        while (padding && (pads > 0))
                        {
                            printc_tt('0');         // print zeros for padding
                            pads--;
                        }
                        buffer = int_str;
                        while (*buffer)
                        {
                            printc_tt(*buffer);
                            buffer++;
                        }
                        argv += sizeof(char*)/sizeof(int);
                        state = 1;
                        str++;
                    break;
                }

            default:
                break;
        }
    }
}
