
#include <vdibind.h>


    WORD
vsf_color( handle, index )
WORD handle;		/* Physical device handle */
WORD index;
{
    intin[0] = index;

    contrl[0] = 25;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
