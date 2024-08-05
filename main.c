#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "binary.h"
#include "cpu.h"

int main(int argc, char **argv, char **envp)
{
    bool done = false;

    uint32_t sys_pd = cpu_init();

    if(cpu_load(binary, argc, argv, sys_pd))
    {
        do
        {
           // char buf[1000];
         //   m68k_disassemble(buf, m68k_get_reg(NULL, M68K_REG_PC), M68K_CPU_TYPE_68000);

        //    printf("%08x %s\n", m68k_get_reg(NULL, M68K_REG_PC), buf);
         //   cpu_run(1);

            cpu_run(8000000);
      
        } while(!done);
    }

    return 0;
}
