
#include <vdibind.h>

    WORD
vsl_type( handle, style )
WORD handle;		/* Physical device handle */
WORD style;
{
    intin[0] = style;

    contrl[0] = 15;
    contrl[1] = 0;
    contrl[3] = 1;
    contrl[6] = handle;
    vdi();
    return( intout[0] );
}
