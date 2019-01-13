#include <stdint.h>
#include <stddef.h>
#include "cpu.h"

static uint8_t* rambase = NULL;

uint32_t xbios_dispatch(uint16_t opcode, void* prm)
{
	uint32_t retval = opcode;

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
            uint32_t sp   = m68k_get_reg(NULL, M68K_REG_SP);
            uint32_t call = READ_LONG(rambase, sp + 2);

            sp -= 4;
            WRITE_LONG(rambase, sp, m68k_get_reg(NULL, M68K_REG_PC));
            m68k_set_reg(M68K_REG_SP, sp);

            m68k_set_reg(M68K_REG_PC, call);

            printf("Supexec(%08x)\r\n", call);
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

    return retval;
}

void xbios_init(uint8_t* ram, uint32_t ramsize)
{
	rambase = ram;
}