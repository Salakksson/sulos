#include "stdio.h"
#include "cpu.h"
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

void printc(char c)
{
    asm_tt_printc(c, 0);
}

void _cdecl prints(char* str)
{
    while(*str)
    {
        asm_tt_printc(*str, 0);
        if (*str == '\n')
            asm_tt_printc('\r', 0);
        str++;
    }

}

void printd(byte c)
{
    switch (c < 10)
    {
        case true:
            printc(c + 0x30);
            break;
        case false:
            printc(c + 0x37);
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


#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4

const char g_HexChars[] = "0123456789abcdef";

int* printf_number(int* argp, int length, bool sign, int radix)
{
    char buffer[32];
    unsigned long long number;
    int number_sign = 1;
    int pos = 0;

    // process length
    switch (length)
    {
        case PRINTF_LENGTH_SHORT_SHORT:
        case PRINTF_LENGTH_SHORT:
        case PRINTF_LENGTH_DEFAULT:
            if (sign)
            {
                int n = *argp;
                if (n < 0)
                {
                    n = -n;
                    number_sign = -1;
                }
                number = (unsigned long long)n;
            }
            else
            {
                number = *(unsigned int*)argp;
            }
            argp++;
            break;

        case PRINTF_LENGTH_LONG:
            if (sign)
            {
                long int n = *(long int*)argp;
                if (n < 0)
                {
                    n = -n;
                    number_sign = -1;
                }
                number = (unsigned long long)n;
            }
            else
            {
                number = *(unsigned long int*)argp;
            }
            argp += 2;
            break;

        case PRINTF_LENGTH_LONG_LONG:
            if (sign)
            {
                long long int n = *(long long int*)argp;
                if (n < 0)
                {
                    n = -n;
                    number_sign = -1;
                }
                number = (unsigned long long)n;
            }
            else
            {
                number = *(unsigned long long int*)argp;
            }
            argp += 4;
            break;
    }

    // convert number to ASCII
    do 
    {
        dword rem;
        x86_div64_32(number, radix, &number, &rem);
        buffer[pos++] = g_HexChars[rem];
    } while (number > 0);

    // add sign
    if (sign && number_sign < 0)
        buffer[pos++] = '-';

    // print number in reverse order
    while (--pos >= 0)
        printc(buffer[pos]);

    return argp;
}

void _cdecl printf(const char* fmt, ...)
{
    int* argp = (int*)&fmt;
    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int radix = 10;
    bool sign = false;

    argp++;

    while (*fmt)
    {
        switch (state)
        {
            case PRINTF_STATE_NORMAL:
                switch (*fmt)
                {
                    case '%':   state = PRINTF_STATE_LENGTH;
                                break;
                    default:    printc(*fmt);
                                break;
                }
                break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt)
                {
                    case 'h':   length = PRINTF_LENGTH_SHORT;
                                state = PRINTF_STATE_LENGTH_SHORT;
                                break;
                    case 'l':   length = PRINTF_LENGTH_LONG;
                                state = PRINTF_STATE_LENGTH_LONG;
                                break;
                    default:    goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_LENGTH_LONG:
                if (*fmt == 'l')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch (*fmt)
                {
                    case 'c':   printc((char)*argp);
                                argp++;
                                break;

                    case 's':   if (length == PRINTF_LENGTH_LONG || length == PRINTF_LENGTH_LONG_LONG) 
                                {
                                    prints(*(char**)argp);
                                    argp += 2;
                                }
                                else 
                                {
                                    prints(*(const char**)argp);
                                    argp++;
                                }
                                break;

                    case '%':   printc('%');
                                break;

                    case 'd':
                    case 'i':   radix = 10; sign = true;
                                argp = printf_number(argp, length, sign, radix);
                                break;

                    case 'u':   radix = 10; sign = false;
                                argp = printf_number(argp, length, sign, radix);
                                break;

                    case 'X':
                    case 'x':
                    case 'p':   radix = 16; sign = false;
                                argp = printf_number(argp, length, sign, radix);
                                break;

                    case 'o':   radix = 8; sign = false;
                                argp = printf_number(argp, length, sign, radix);
                                break;

                    // ignore invalid spec
                    default:    break;
                }

                // reset state
                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                radix = 10;
                sign = false;
                break;
        }

        fmt++;
    }
}