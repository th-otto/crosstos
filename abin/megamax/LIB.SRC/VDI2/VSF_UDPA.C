
#include <vdibind.h>

    WORD
vsf_udpat( handle, fill_pat, planes )
WORD handle;		/* Physical device handle */
WORD fill_pat[];
WORD planes;
{
    i_intin( fill_pat );

    contrl[0] = 112;
    contrl[1] = 0;
    contrl[3] = 16*planes;
    contrl[6] = handle;
    vdi();
    i_intin( intin );
}
