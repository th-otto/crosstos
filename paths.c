#include <stdint.h>
#include <stdbool.h>
#include "paths.h"

void path_from_tos(char* str)
{
    char* c = str;

    do
    {
        if(*c == '\\')
        {
            *c = '/';
        }

    } while(*c++);
}

void path_to_tos(char* str)
{
    char* c = str;

    do
    {
        if(*c == '/')
        {
            *c = '\\';
        }

    } while(*c++);
}
