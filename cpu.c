/*
 * CPU abstraction layer
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include "cpu.h"
#include "tlsf.h"

#include <time.h>
#include <sys/time.h>

union
{
    struct 
    {
        uint8_t res[0x800];
        uint8_t application[1024*1024*16 - 0x800];  
    } sys;
    
    uint8_t raw[1024*1024*16];
} ram;

#define stram (ram.raw)


FILE* handles[16];

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
    printf("Free %08x\n", block);
  //free_ex(&ram.raw[block], ram.sys.application);
}

uint32_t Malloc(int32_t bytes)
{
    uint32_t mal = 0;

    if(bytes > 0)
    {
        mal = ((uint8_t*)malloc_ex(bytes, ram.sys.application)) - ram.raw;
        printf("Malloc %08x\n", mal);
    }
    else
    {
        mal = get_max_size(ram.sys.application);
        printf("Malloc query %08x\n", mal);
    }

    return mal;
}

void gemdos_init(void)
{
    init_memory_pool(sizeof(ram.sys.application), ram.sys.application);

    handles[0] = stdin;
    handles[1] = stdout;
    handles[2] = stderr;
    handles[3] = stderr;

    memset(block_malloc, 0, sizeof(block_malloc));
}

bool cpu_load(uint8_t* bin, uint8_t* p_cmdlin)
{
    m68k_init();
    m68k_set_cpu_type(M68K_CPU_TYPE_68000);
    m68k_pulse_reset();

    gemdos_init();

    uint32_t addr  = Malloc(500000);
    uint32_t stack = Malloc(10000);

    if(!addr)
    {
        return false;
    }

    if(!stack)
    {
        return false;
    }

    stack += 10000 - 8;

    m68k_set_reg(M68K_REG_SP, stack);
    WRITE_LONG(stram, stack + 4, addr);
    m68k_set_reg(M68K_REG_PC, addr+256);
    m68k_set_reg(M68K_REG_A0, 0); /* launch as non-accessory */


    uint16_t ph_branch      = READ_WORD(bin,  0);   /* Branch to start of the program  */
                                                    /* (must be 0x601a!)               */
    uint32_t ph_tlen        = READ_LONG(bin,  2);   /* Length of the TEXT segment      */
    uint32_t ph_dlen        = READ_LONG(bin,  6);   /* Length of the DATA segment      */
    uint32_t ph_blen        = READ_LONG(bin, 10);   /* Length of the BSS segment       */
    uint32_t ph_slen        = READ_LONG(bin, 14);   /* Length of the symbol table      */
    uint32_t ph_res1        = READ_LONG(bin, 18);   /* Reserved, should be 0;          */
                                                    /* Required by PureC               */
    uint32_t ph_prgflags    = READ_LONG(bin, 22);   /* Program flags                   */
    uint16_t ph_absflag     = READ_WORD(bin, 26);   /* 0 = Relocation info present     */


 /*   printf("ph_branch    = %08x\n", ph_branch);
    printf("ph_tlen      = %08x\n", ph_tlen);
    printf("ph_dlen      = %08x\n", ph_dlen);
    printf("ph_blen      = %08x\n", ph_blen);
    printf("ph_slen      = %08x\n", ph_slen);
    printf("ph_res1      = %08x\n", ph_res1);
    printf("ph_prgflags  = %08x\n", ph_prgflags);
    printf("ph_absflag   = %08x\n", ph_absflag); */
 
  
    if(ph_branch != 0x601a)
    {
        return false;
    }

    uint32_t p_lowtpa  = addr;                  /* Start address of the TPA            */
    uint32_t p_hitpa   = p_lowtpa + 1024*1024;  /* First byte after the end of the TPA */
    uint32_t p_tbase   = p_lowtpa + 256;        /* Start address of the program code   */
    uint32_t p_tlen    = ph_tlen;               /* Length of the program code          */
    uint32_t p_dbase   = p_tbase + p_tlen;      /* Start address of the DATA segment   */
    uint32_t p_dlen    = ph_dlen;               /* Length of the DATA section          */
    uint32_t p_bbase   = p_dbase + p_dlen;      /* Start address of the BSS segment    */
    uint32_t p_blen    = ph_blen;               /* Length of the BSS section           */
    uint32_t p_dta     = p_lowtpa + 128;        /* Pointer to the default DTA          */
                                                /* Warning: Points first to the        */
                                                /* command line !                      */
    uint32_t p_parent  = 0;                     /* Pointer to the basepage of the      */
                                                /* calling processes                   */
    uint32_t p_resrvd0 = 0;                     /* Reserved                            */
    uint32_t p_env     = 0x4000;                /* Address of the environment string   */


    WRITE_LONG(stram, addr +  0 , p_lowtpa);
    WRITE_LONG(stram, addr +  4 , p_hitpa);
    WRITE_LONG(stram, addr +  8 , p_tbase);
    WRITE_LONG(stram, addr + 12 , p_tlen);
    WRITE_LONG(stram, addr + 16 , p_dbase);
    WRITE_LONG(stram, addr + 20 , p_dlen);
    WRITE_LONG(stram, addr + 24 , p_bbase);
    WRITE_LONG(stram, addr + 28 , p_blen);
    WRITE_LONG(stram, addr + 32 , p_dta);
    WRITE_LONG(stram, addr + 36 , p_parent);
    WRITE_LONG(stram, addr + 40 , p_resrvd0);
    WRITE_LONG(stram, addr + 44 , p_env);

    memset(&stram[addr + 48 ], 0, 80);
    stram[addr + 128] = strlen(p_cmdlin);
    strcpy(&stram[addr + 129], p_cmdlin);

    memcpy(&stram[p_tbase], bin + 28, p_tlen + p_dlen);

 /*   printf("p_lowtpa = %08x\n", p_lowtpa);
    printf("p_hitpa  = %08x\n", p_hitpa);
    printf("p_tbase  = %08x\n", p_tbase);
    printf("p_tlen   = %08x\n", p_tlen);
    printf("p_dbase  = %08x\n", p_dbase);
    printf("p_dlen   = %08x\n", p_dlen);
    printf("p_bbase  = %08x\n", p_bbase);
    printf("p_blen   = %08x\n", p_blen);
    printf("p_dta    = %08x\n", p_dta);
    printf("p_parent = %08x\n", p_parent);
    printf("p_resrvd0= %08x\n", p_resrvd0);
    printf("p_env    = %08x\n", p_env);*/
  
    if(!ph_absflag)
    {
        uint32_t fix = p_tbase;
        uint8_t* rel = bin + 28 + ph_tlen + ph_dlen + ph_slen;

        uint32_t off = READ_LONG(rel, 0);

        if(off)
        {
            fix += off;

        //    printf("Relocating... (%08x = %08x + %08x)\r\n", fix, READ_LONG(stram, fix), p_tbase);
            WRITE_LONG(stram, fix, READ_LONG(stram, fix) + p_tbase);
 
            rel += 4;

            while(*rel)
            {
                if(*rel == 1)
                {
                    fix += 254;
                }
                else
                {

                    fix += *rel;

              //      printf("Relocating... (%08x = %08x + %08x)\r\n", fix, READ_LONG(stram, fix), p_tbase);
                    WRITE_LONG(stram, fix, READ_LONG(stram, fix) + p_tbase);
                }

                rel++;
            }
        }

    }

    return true;
}


unsigned int cpu_read_byte(unsigned int address)
{
    if(address > (1024*1024*16))
    {
        return 0;
    }

    return READ_BYTE(stram, address);
}

unsigned int cpu_read_word(unsigned int address)
{
    if(address > (1024*1024*16))
    {
        return 0;
    }

    return READ_WORD(stram, address);
}

unsigned int cpu_read_long(unsigned int address)
{
    if(address > (1024*1024*16))
    {
        return 0;
    }

    switch(address)
    {
        case 0x4ba:
        {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC_RAW, &ts);

            uint32_t ticks200 = (ts.tv_nsec / (1000000000/200)) + ts.tv_sec * 200;

            WRITE_LONG(stram, address, ticks200);
        }
            break;

        default:
        {

        }
            break;
    }

    return READ_LONG(stram, address);
}

unsigned int cpu_read_word_dasm(unsigned int address)
{
    if(address > (1024*1024*16))
    {
        return 0;
    }

    return READ_WORD(stram, address);
}

unsigned int cpu_read_long_dasm(unsigned int address)
{
    if(address > (1024*1024*16))
    {
        return 0;
    }

    return READ_LONG(stram, address);
}

void cpu_write_byte(unsigned int address, unsigned int value)
{
    if(address > (1024*1024*16))
    {
        return;
    }

    WRITE_BYTE(stram, address, value);
}

void cpu_write_word(unsigned int address, unsigned int value)
{
    if(address > (1024*1024*16))
    {
        return;
    }

    WRITE_WORD(stram, address, value);
}

void cpu_write_long(unsigned int address, unsigned int value)
{
    if(address > (1024*1024*16))
    {
        return;
    }

    WRITE_LONG(stram, address, value);
}

char* m68ki_disassemble_quick(unsigned int pc, unsigned int cpu_type);
void cpu_callback_instruction(void)
{
}

void cpu_callback_pc_changed(uint32_t pc)
{
    
}

bool cpu_callback_trap(uint32_t vector, uint32_t* newpc)
{
    switch(vector)
    {
        case 0x21: /* GEMDOS */
        {
            uint16_t opcode = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP));

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
                    uint32_t value = getchar();
                    m68k_set_reg(M68K_REG_D0, value); /* ASCII in bits 0...7 */
                }
                    break;

                case 0x0002: /* Cconout() */
                {
                    putchar(READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2));
                }
                    break;

                case 0x08: /* Cnecin() */
                {
                    m68k_set_reg(M68K_REG_D0, 0); /* E_OK */
                }
                    break;

                case 0x09: /* Cconws() */
                {
                    char* buf = &stram[READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2)];

                    printf("%s\n", buf);

                    m68k_set_reg(M68K_REG_D0, 0); /* E_OK */
                }
                    break;

                case 0x0a: /* Cconrs() */
                {
                    char* buf = &stram[READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2)];

                  //  getline(buf,128,stdin);

                    sprintf(buf, "%s", "abc");

                    m68k_set_reg(M68K_REG_D0, 0); /* E_OK */
                }
                    break;


                case 0x19: /* Dgetdrv() */
                {
                    m68k_set_reg(M68K_REG_D0, 2); /* allways C: */
                }
                    break;

                case 0x2a: /* Tgetdate() */
                {
                    m68k_set_reg(M68K_REG_D0, 0); /* bogus */
                }
                    break;

                case 0x2c: /* Tgettime() */
                {
                    m68k_set_reg(M68K_REG_D0, 0); /* bogus */
                }
                    break;

                case 0x0030: /* Sversion() */
                {
                    m68k_set_reg(M68K_REG_D0, 30<<8); /* GEMDOS 0.30 */
                }
                    break;

                case 0x3c: /* Fcreate() */
                {
                    char*    fname = &stram[READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
                    int16_t  attr  = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 6);

                    m68k_set_reg(M68K_REG_D0, Fcreate(fname, attr));
                }
                    break;

                case 0x3d: /* Fopen() */
                {
                    char*    fname = &stram[READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
                    int16_t  mode  = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 6);

                    m68k_set_reg(M68K_REG_D0, Fopen(fname, mode));
                }
                    break;

                case 0x3e:  /* Fclose() */
                {
                    int16_t  handle  = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2);

                    m68k_set_reg(M68K_REG_D0, Fclose(handle));
                }
                    break;

                case 0x3f: /* Fread() */
                {
                    int16_t  handle = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2);
                    int32_t  count  = READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 4);
                    char*    buf    = &stram[READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

                    m68k_set_reg(M68K_REG_D0, Fread(handle, count, buf));
                }
                    break;

                case 0x40: /* Fwrite() */
                {
                    int16_t  handle = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2);
                    int32_t  count  = READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 4);
                    char*    buf    = &stram[READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

                    m68k_set_reg(M68K_REG_D0, Fwrite(handle, count, buf));
                }
                    break;

                case 0x41: /* Fdelete() */
                {
                    m68k_set_reg(M68K_REG_D0, 0); /* E_OK */
                }
                    break;

                case 0x42: /* Fseek() */
                {
                    int32_t  offset = READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2);
                    int16_t  handle = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 6);
                    int16_t  mode   = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 8);

                    m68k_set_reg(M68K_REG_D0, Fseek(offset, handle, mode));
                }
                    break;

                case 0x47: /* int16_t Dgetpath ( int8_t *path, int16_t driveno ); */
                {
                    char*    buf    = &stram[READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
                    int16_t  drive  = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 6);

                    strcpy(buf, ".");

                    m68k_set_reg(M68K_REG_D0, 0); /* E_OK */
                }
                    break;

                case 0x48: /* Malloc() */
                {
                    int32_t  count  = READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2);

                    m68k_set_reg(M68K_REG_D0, Malloc(count)); 
                }
                    break;

                case 0x49: /* Mfree() */
                {
                    uint32_t  block  = READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2);

                    Mfree(block);

                    m68k_set_reg(M68K_REG_D0, 0); /* E_OK */
                }
                    break;

                case 0x4a: /* Mshrink() */
                {
                    m68k_set_reg(M68K_REG_D0, 0); /* E_OK */
                }
                    break;

                case 0x4c: /* Pterm() */
                {
                    int16_t status = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2);

                    printf("\n");

                    exit(status);
                }   
                    break;

                case 0x57: /* void Fdatime ( DOSTIME *timeptr, int16_t handle, int16_t wflag ); */
                {
                    uint32_t tptr   = READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2);
                    int16_t  handle = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 6);
                    int16_t  wflag  = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 8);

                    if(wflag)
                    {
                        /* Set values */
                    }
                    else
                    {
                        /* Obtain values */
                        WRITE_WORD(stram, tptr + 0, 0x0000); /* time */
                        WRITE_WORD(stram, tptr + 2, 0x0000); /* date */
                    }
                }
                    break;

                case 0x0119: /* Pdomain() */
                case 0x012c: /* Fxattre() */
                case 0x0154: /* Ssystem() */
                {
                    m68k_set_reg(M68K_REG_D0, opcode); /* not available */
                }
                    break;

                default:
                {
                    printf("unknown GEMDOS call (%04x)\n", opcode);
                    exit(0);
                }
                    break;
            }
        }
            break;

        case 0x22:
        {
            uint16_t opcode = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP));

            switch(opcode)
            {
                default:
                {
                    printf("unknown VDI/AES call (%04x)\n", opcode);
                    exit(0);
                }
                    break;
            }

        }
          break;


        case 0x2d:
        {
            uint16_t opcode = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP));

            switch(opcode)
            {
                case 0x0003: /* VOID Bconout ( int16_t dev, int16_t c ); */
                {
                    int16_t dev = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2);
                    int16_t c   = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 4);

                    switch(dev)
                    {

                        case 1: /* AUX: */
                        case 2: /* CON: */
                        {
                            putchar((char)c);
                        }
                            break;

                        case 0: /* PRN: */
                        case 3: /* MIDI */
                        case 4: /* KBD  */
                        case 5: /* SCRN */
                        case 6: /* MODEM-1 */
                        case 7: /* MODEM-2 */
                        case 8: /* MODEM-3 */
                        case 9: /* MODEM-4 */
                        default:
                        {
                            /* ignore */
                        }
                            break;
                    }
                }
                    break;

                case 0x0005: /* int32_t Setexc ( int16_t number, VOID (*vec)() ); */
                {
                    int16_t number = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2);
                    uint32_t value = READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 4);

                    uint32_t prev  = READ_LONG(stram, number << 2);

                    if(value != 0xffffffff)
                    {
                        WRITE_LONG(stram, number << 2, value);
                    }

                    m68k_set_reg(M68K_REG_D0, prev); /* Return previous value */
                }
                    break;

                case 0x000B: /* int32_t Kbshift ( int16_t mode ); */
                {
                     m68k_set_reg(M68K_REG_D0, 0); /* No modifier keys emulated */
                }
                    break;

                default:
                {
                    printf("unknown BIOS call (%04x)\n", opcode);
                    exit(0);
                }
                    break;
            }

        }
          break;


        case 0x2e:
        {
            uint16_t opcode = READ_WORD(stram, m68k_get_reg(NULL, M68K_REG_SP));

        //    printf("xbios %02x\n", opcode);

            switch(opcode)
            {
                case 0x10:  /* Keytabl() */
                {
               //     printf("ignored XBIOS call (%04x)\n", opcode);
                }
                    break;

                case 0x26: /* Supexec() */
                {
                    *newpc = READ_LONG(stram, m68k_get_reg(NULL, M68K_REG_SP) + 2);

                 //   printf("Supexec(%08x)\r\n", *newpc);
                    return true;
                }
                    break;

                default:
                {
                    printf("unknown XBIOS call (%04x)\n", opcode);
                    exit(0);
                }
                    break;
            }

        }
          break;

        default:
        {
            printf("unknown trap (vec %08x)\n", vector);
            exit(0);
        }
            break;
    }

    return false;
}
