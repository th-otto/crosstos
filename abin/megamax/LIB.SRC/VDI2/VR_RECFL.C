
#include <vdibind.h>

    WORD
vr_recfl( handle, xy )
WORD handle;		/* Physical device handle */
WORD *xy;
{
    i_ptsin( xy );

    contrl[0] = 114;
    contrl[1] = 2;
    contrl[3] = 0;
    contrl[6] = handle;
    vdi();

    i_ptsin( ptsin );
}
