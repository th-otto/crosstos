
#include <vdibind.h>

    WORD
vsl_ends( handle, beg_style, end_style)
WORD handle;		/* Physical device handle */
WORD beg_style;
WORD end_style;
{
    intin[0] = beg_style;
    intin[1] = end_style;

    contrl[0] = 108;
    contrl[1] = 0;
    contrl[3] = 2;
    contrl[6] = handle;
    vdi();
}
