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
#include "tinyalloc.h"

#include <time.h>
#include <sys/time.h>

typedef struct file_s
{
    FILE* fd;
    char* fname;
    bool  std;
} file_t;

uint8_t ta_base[0x1000*16];

static uint8_t* rambase = NULL;
static file_t   handles[16];

int32_t Fopen(char* fname, int16_t mode)
{
    int32_t retval = -1;
    int i;

    for(i = 0; i < sizeof(handles) / sizeof(handles[0]); i++)
    {
        if(!handles[i].fd)
        {
            switch(mode & 3)
            {
                case 0: handles[i].fd = fopen(fname, "rb"); break;
                case 1: handles[i].fd = fopen(fname, "rb+"); break;
                case 2: handles[i].fd = fopen(fname, "rb+"); break;
                default:
                {

                }
                    break;
            }

            if(handles[i].fd)
            {
                handles[i].fname = malloc(strlen(fname) + 1);

                if(handles[i].fname)
                {
                    strcpy(handles[i].fname, fname);
                }

                retval = i;
                break;
            }
        }
    }

 //   printf("Fopen(%s, %02x) = %d\n", fname, mode, retval);

    return retval;
}

int16_t Fclose(int16_t handle)
{
    if(handle < 16)
    {
        if(!handles[handle].std)
        {
            fclose(handles[handle].fd);

            if(handles[handle].fname)
            {
     //           printf("Fclose(%d) (%s)\r\n", handle, handles[handle].fname);
                free(handles[handle].fname);
            }
 
            handles[handle].fd    = NULL;
            handles[handle].fname = NULL;
        }

        return GEMDOS_E_OK; /* E_OK */
    }

    return -1; /* error */
}

int16_t Fforce ( int16_t stdh, int16_t nonstdh )
{
 //   printf("Fforce() not implemented\r\n", stdh, nonstdh);
    return GEMDOS_E_OK;
}

int16_t Fcreate (char* fname, int16_t attr)
{
    int32_t retval = GEMDOS_E_EACCDN;

    FILE* fd;

  //  printf("Fcreate(\"%s\", %04x)\r\n", fname, attr);

    fd = fopen(fname,"wb+");

    if(fd)
    {
        fclose(fd);

        retval = Fopen(fname, 2);
    }

    return retval;
}

int16_t Fattrib (char* fname, int16_t wflag, int16_t attr )
{
  //  printf("Fattrib(\"%s\", %04x, %04x)\r\n", fname, wflag, attr);

    return GEMDOS_EFILNF;
}

int32_t Frename(char *fname, char *new_fname)
{
  //  printf("Frename (%s, %s)\r\n", fname, new_fname);

    if(rename(fname, new_fname))
    {
        return GEMDOS_EFILNF;
    }

    return GEMDOS_E_OK; 
}

int16_t Fdelete (char *fname )
{
 //   printf("Fdelete (%s)\r\n", fname);
    if(remove(fname))
    {
        return GEMDOS_EFILNF;
    }

    return GEMDOS_E_OK; 
}

int32_t Fread(int16_t handle, int32_t count, void *buf)
{
    if(handle >= 0)
    {
        if(handles[handle].fd)
        {
            return fread(buf, 1, count, handles[handle].fd);
        }
    }

    return -1;
}

int32_t Fwrite(int16_t handle, int32_t count, void *buf)
{
    if(handle >= 0)
    {
        if(handles[handle].fd)
        {
            return fwrite(buf, 1, count, handles[handle].fd);
        }
    }

    return -1;
}

int32_t Fseek ( int32_t offset, int16_t handle, int16_t seekmode )
{
    int32_t r = -1;

    switch(seekmode)
    {
        case 0: r = fseek (handles[handle].fd, offset, SEEK_SET);  break;
        case 1: r = fseek (handles[handle].fd, offset, SEEK_CUR);  break;
        case 2: r = fseek (handles[handle].fd, offset, SEEK_END);  break;
        default:
        {

        }
            break;
    }

    if(!r)
    {
        r =ftell(handles[handle].fd);
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
    ta_free(block);
}

uint32_t Malloc(int32_t bytes)
{
    uint32_t retval = 0;

    if(bytes == -1)
    {
        retval = ta_biggest_block();
    }
    else
    {
        retval = ta_alloc(bytes);
    }

  //  printf("Malloc(%08x) = 0x%08x\r\n", bytes, retval);

    return retval;
}

uint32_t gemdos_dispatch(uint16_t opcode, uint32_t pd)
{
    uint32_t retval = opcode;

 //   printf("gemdos %02x\n", opcode);

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

        case 0x0006: /* int32_t Crawio ( int16_t w ); */
        {
            uint16_t w = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            if(w == 0x00ff) /* Read character from standard input */
            {
                /* ignore */
                retval = 0;
            }
            else
            {
                putchar(w);
                retval = 0;
            }

        }
            break;

        case 0x0007: /* int32_t Crawcin ( void ); */
        {
            retval = ' '; /* space pressed */
        }
            break;

        case 0x0008: /* Cnecin() */
        {
            retval = GEMDOS_E_OK;
        }
            break;

        case 0x0009: /* Cconws() */
        {
            char* buf = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];

            printf("%s\n", buf);

            retval = GEMDOS_E_OK;
        }
            break;

        case 0x000a: /* Cconrs() */
        {
            char* buf = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];

          //  getline(buf,128,stdin);

            sprintf(buf, "%s", "abc");

            retval = GEMDOS_E_OK;
        }
            break;


        case 0x0019: /* Dgetdrv() */
        {
            retval = 2; /* allways C: */
        }
            break;

        case 0x001A: /* void Fsetdta ( DTA *buf ); */
        {
            uint32_t dta = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            WRITE_LONG(rambase, pd + OFF_P_DTA, dta);
        }
            break;

        case 0x0020: /* int32_t Super ( void *stack ); */
        {
           uint32_t stack = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

           if(stack == 1)
           {
                retval = 0; /* user mode */
           }
           else
           {
                retval = 0x1234567;
           }
        }
            break;

        case 0x002a: /* Tgetdate() */
        {
            retval = 0; /* bogus */
        }
            break;

        case 0x002c: /* Tgettime() */
        {
            retval = 0; /* bogus */
        }
            break;

        case 0x002f: /* DTA *Fgetdta ( void ); */
        {
            retval = READ_LONG(rambase, pd + OFF_P_DTA);
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

        case 0x003e:  /* Fclose() */
        {
            int16_t  handle  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            retval = Fclose(handle);
        }
            break;

        case 0x003f: /* Fread() */
        {
            int16_t  handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int32_t  count  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);
            char*    buf    = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

            retval = Fread(handle, count, buf);
        }
            break;

        case 0x0040: /* Fwrite() */
        {
            int16_t  handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int32_t  count  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);
            char*    buf    = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

            retval = Fwrite(handle, count, buf);
        }
            break;

        case 0x0043: /* int16_t Fattrib ( const int8_t *filename, int16_t wflag, int16_t attrib );*/ 
        {
            char*    fname  = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
            int16_t  wflag  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);
            int16_t  attrib = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8);

            retval = Fattrib(fname, wflag, attrib);
        }
            break;

        case 0x0041: /* int16_t Fdelete ( const int8_t *fname ) */
        {
            char*    fname  = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];

            retval = Fdelete(fname);
        }
            break;

        case 0x0042: /* Fseek() */
        {
            int32_t  offset = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int16_t  handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);
            int16_t  mode   = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8);

            retval = Fseek(offset, handle, mode);
        }
            break;

        case 0x0046: /* int16_t Fforce ( int16_t stdh, int16_t nonstdh ); */
        {
            int16_t  stdh    = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            int16_t  nonstdh = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);

            retval = Fforce(stdh,nonstdh);
        }
            break;

        case 0x0047: /* int16_t Dgetpath ( int8_t *path, int16_t driveno ); */
        {
            char*    buf    = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
            int16_t  drive  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

            (void)drive;

            strcpy(buf, ".");

            retval = GEMDOS_E_OK;
        }
            break;

        case 0x0048: /* Malloc() */
        {
            int32_t  count  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            retval = Malloc(count); 
        }
            break;

        case 0x0049: /* Mfree() */
        {
            uint32_t  block  = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            Mfree(block);

            retval = GEMDOS_E_OK;
        }
            break;

        case 0x004a: /* Mshrink() */
        {
            retval = GEMDOS_E_OK;
        }
            break;

        case 0x004c: /* Pterm() */
        {
            int16_t status = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

            printf("\n");

            exit(status);
        }   
            break;

        case 0x004e: /* int32_t Fsfirst ( const int8_t *filename, int16_t attr ) */
        {
            char*   fname = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
            int16_t attr  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

            printf("Fsfirst(\"%s\", 0x%04x)\r\n", fname, attr);

            retval = GEMDOS_EFILNF;
        }
            break;

        case 0x004f: /* int16_t Fsnext ( void ) */
        {
            printf("Fsnext()\r\n");

            retval = GEMDOS_EFILNF;
        }
            break;

        case 0x0056: /* int32_t Frename ( const int8_t *oldname, const int8_t *newname ) */
        {
            uint16_t reserved  = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
            char*    fname     = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4)];
            char*    new_fname = &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

            (void)reserved;

            retval = Frename(fname, new_fname);
        }
            break;

        case 0x0057: /* void Fdatime ( DOSTIME *timeptr, int16_t handle, int16_t wflag ); */
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

    ta_init();

    handles[0].fd    = stdin;
    handles[0].fname = "STDIN:";
    handles[0].std   = true;

    handles[1].fd    = stdout;
    handles[1].fname = "STDOUT:";
    handles[1].std   = true;

    handles[2].fd    = stderr;
    handles[2].fname = "STDAUX:";
    handles[2].std   = true;

    handles[3].fd    = stderr;
    handles[3].fname = "STDPRN:";
    handles[3].std   = true;
}
