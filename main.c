#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "cpu.h"

#if defined(TOS_BINARY)
extern uint8_t binary[];
#define FIRST_PARAM (1)
#else
#define FIRST_PARAM (2)
#endif

int main( int argc, char* args[] )
{
    bool done = false;

#if !defined(TOS_BINARY)
     uint8_t* binary = NULL;

    if(argc > 1)
    {
        FILE* fd = fopen(args[1], "r");

        if(fd)
        {
            int size;

            fseek(fd, 0, SEEK_END);
            size = ftell(fd);
            fseek(fd, 0, SEEK_SET);

            binary = malloc(size);

            if(binary)
            {
                int read = fread(binary, 1, size, fd);

                free(binary);

                if(read != size)
                {
                    binary = NULL;
                }
            }


            fclose(fd);
        }
    }
#endif

    if(binary)
    {
        char cmd[255] = "";

        int arg;

        for(arg = FIRST_PARAM; arg < argc; arg++)
        {
            strlcat(cmd, args[arg], sizeof(cmd));
            strlcat(cmd, " ", sizeof(cmd));
        }

        printf("Command line: %s\n", cmd);

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
    }
    else
    {
        printf("Binary not specified.\n");
    }

    return 0;
}
