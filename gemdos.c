/*
 * CPU abstraction layer
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "gemdos.h"
#include "tlsf.h"

#include <time.h>
#include <sys/time.h>

#define GEMDOS_E_OK (0)

static uint8_t* rambase = NULL;
static void*    rampool = NULL;

static FILE*    handles[16];
static uint32_t block_malloc[256];

int32_t Fopen(char* fname, int16_t mode)
{
    int i;

  //  printf("Fopen(%s, %02x)\n", fname, mode);
    for(i = 0; i < sizeof(handles) / sizeof(handles[0]); i++)
    {
        if(!handles[i])
        {
            switch(mode & 3)
            {
                case 0: handles[i] = fopen(fname, "r+"); break;
                case 1: handles[i] = fopen(fname, "w");  break;
                case 2: handles[i] = fopen(fname, "w+"); break;
                default:
                {

                }
                    break;
            }

            if(handles[i])
            {
                return i;
            }
        }
    }

    return -1;
}

int16_t Fclose(int16_t handle)
{
    if(handle < 16)
    {
        fclose(handles[handle]);

        handles[handle] = NULL;

        return 0; /* E_OK */
    }

    return -1; /* error */
}

int16_t Fcreate (char* fname, int16_t attr)
{
    return Fopen(fname, 2);
}

int32_t Fread(int16_t handle, int32_t count, void *buf)
{
    if(handle >= 0)
    {
        if(handles[handle])
        {
            return fread(buf, 1, count, handles[handle]);
        }
    }

    return -1;
}

int32_t Fwrite(int16_t handle, int32_t count, void *buf)
{
    if(handle >= 0)
    {
        if(handles[handle])
        {
            return fwrite(buf, 1, count, handles[handle]);
        }
    }

    return -1;
}

int32_t Fseek ( int32_t offset, int16_t handle, int16_t seekmode )
{
    int32_t r = -1;

    switch(seekmode)
    {
        case 0: r = fseek (handles[handle], offset, SEEK_SET);  break;
        case 1: r = fseek (handles[handle], offset, SEEK_CUR);  break;
        case 2: r = fseek (handles[handle], offset, SEEK_END);  break;
        default:
        {

        }
            break;
    }

    if(!r)
    {
        r =ftell(handles[handle]);
    }

 //   printf("Fseek(%d, %d, %d) = %d\n", handle, offset, seekmode, r);

    return r;
}

uint32_t Mshrink(uint32_t block, uint32_t bytes)
{
  return 0;
}

void Mfree(uint32_t block)
{
    int i;

    for(i = 0; i < (sizeof(block_malloc) / sizeof(block_malloc[0])); i++)
    {
        if(block_malloc[i] == block)
        {
            free_ex(&rambase[block], rampool);
            block_malloc[i] = 0;
        }
    }
}

uint32_t Malloc(int32_t bytes)
{
    if(bytes > 0)
    {
        int i;

        for(i = 0; i < (sizeof(block_malloc) / sizeof(block_malloc[0])); i++)
        {
            if(!block_malloc[i])
            {
                block_malloc[i] = ((uint8_t*)malloc_ex(bytes, rampool)) - rambase;

                return block_malloc[i];
            }
        }
    }

    return get_max_size(rampool);
}

uint32_t gemdos_dispatch(uint16_t opcode, uint32_t prm)
{
    uint32_t retval = opcode;

    //    printf("gemdos %02x\n", opcode);

    switch(opcode)
    {
        case 0x0000: /* Pterm0() */
        {
            printf("\n");

            exit(0);
        }
            break;

        case 0x0001: /* Cconin() */
        {
            retval = getchar(); /* ASCII in bits 0...7 */
        }
            break;

        case 0x0002: /* Cconout() */
        {
            putchar(READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2));

            retval = GEMDOS_E_OK;
        }
            break;

        case 0x08: /* Cnecin() */
        {
            retval = GEMDOS_E_OK;
        }
            break;

        case 0x09: /* Cconws() */
        {
            char* buf = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];

            printf("%s\n", buf);

            retval = GEMDOS_E_OK;
        }
            break;

        case 0x0a: /* Cconrs() */
        {
            char* buf = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];

          //  getline(buf,128,stdin);

            sprintf(buf, "%s", "abc");

            retval = GEMDOS_E_OK;
        }
            break;


        case 0x19: /* Dgetdrv() */
        {
            retval = 2; /* allways C: */
        }
            break;

        case 0x2a: /* Tgetdate() */
        {
            retval = 0; /* bogus */
        }
            break;

        case 0x2c: /* Tgettime() */
        {
            retval = 0; /* bogus */
        }
            break;

        case 0x0030: /* Sversion() */
        {
            retval = 30 << 8; /* GEMDOS 0.30 */
        }
            break;

        case 0x3c: /* Fcreate() */
        {
            char*    fname = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
            int16_t  attr  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

            retval = Fcreate(fname, attr);
        }
            break;

        case 0x3d: /* Fopen() */
        {
            char*    fname = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
            int16_t  mode  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

            retval = Fopen(fname, mode);
        }
            break;

        case 0x3e:  /* Fclose() */
        {
            int16_t  handle  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            retval = Fclose(handle);
        }
            break;

        case 0x3f: /* Fread() */
        {
            int16_t  handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int32_t  count  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);
            char*    buf    = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

            retval = Fread(handle, count, buf);
        }
            break;

        case 0x40: /* Fwrite() */
        {
            int16_t  handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int32_t  count  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);
            char*    buf    = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

            retval = Fwrite(handle, count, buf);
        }
            break;

        case 0x41: /* Fdelete() */
        {
            retval = GEMDOS_E_OK;
        }
            break;

        case 0x42: /* Fseek() */
        {
            int32_t  offset = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int16_t  handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);
            int16_t  mode   = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8);

            retval = Fseek(offset, handle, mode);
        }
            break;

        case 0x47: /* int16_t Dgetpath ( int8_t *path, int16_t driveno ); */
        {
            char*    buf    = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
            int16_t  drive  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

            (void)drive;

            strcpy(buf, ".");

            retval = GEMDOS_E_OK;
        }
            break;

        case 0x48: /* Malloc() */
        {
            int32_t  count  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            retval = Malloc(count); 
        }
            break;

        case 0x49: /* Mfree() */
        {
            uint32_t  block  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            Mfree(block);

            retval = GEMDOS_E_OK;
        }
            break;

        case 0x4a: /* Mshrink() */
        {
            retval = GEMDOS_E_OK;
        }
            break;

        case 0x4c: /* Pterm() */
        {
            int16_t status = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            printf("\n");

            exit(status);
        }   
            break;

        case 0x57: /* void Fdatime ( DOSTIME *timeptr, int16_t handle, int16_t wflag ); */
        {
            uint32_t tptr   = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int16_t  handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);
            int16_t  wflag  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8);

            (void)handle;

            if(wflag)
            {
                /* Set values */
            }
            else
            {
                /* Obtain values */
                WRITE_WORD(rambase, tptr + 0, 0x0000); /* time */
                WRITE_WORD(rambase, tptr + 2, 0x0000); /* date */
            }
        }
            break;

        case 0x0119: /* Pdomain() */
        case 0x012c: /* Fxattre() */
        case 0x0154: /* Ssystem() */
        {
            /* ignored */
        }
            break;

        default:
        {
            printf("unknown GEMDOS call (%04x)\n", opcode);
            exit(0);
        }
            break;
    }

    return retval;
}

void gemdos_init(uint8_t* ram, uint32_t ramsize)
{
    rambase = ram;
    rampool = &ram[0x800];

    init_memory_pool(ramsize - 0x800, rampool);

    handles[0] = stdin;
    handles[1] = stdout;
    handles[2] = stderr;
    handles[3] = stderr;

    memset(block_malloc, 0, sizeof(block_malloc));
}
