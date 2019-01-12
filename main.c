#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "cpu.h"

int main( int argc, char* args[] )
{
    uint8_t* binary = NULL;
    bool done = false;

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
 
    if(binary)
    {
        char cmd[255] = "";

        int arg;

        for(arg = 2; arg < argc; arg++)
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
