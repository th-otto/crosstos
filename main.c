#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary.h"
#include "cpu.h"

int main( int argc, char* args[] )
{
    bool done = false;

    uint8_t cmd[255] = "";

    int arg;

    int i = 0;

    for(arg = 1; arg < argc; arg++)
    {
        char* aptr = args[arg];

        while(*aptr && i < (sizeof(cmd) - 1))
        {
            cmd[i++] = *aptr++;
        }

        if(i < (sizeof(cmd) - 1))
        {
            cmd[i++] = ' ';
        }
        else
        {
            break;
        }
    }

    cmd[i] = '\0';

    if(cpu_load(binary, cmd))
    {
        do
        {
            char buf[1000];
            m68k_disassemble(buf, m68k_get_reg(NULL, M68K_REG_PC), M68K_CPU_TYPE_68000);

        //    printf("%08x %s\n", m68k_get_reg(NULL, M68K_REG_PC), buf);
         //   cpu_run(1);

            cpu_run(8000000);
      
        } while(!done);
    }

    return 0;
}
