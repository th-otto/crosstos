
#include <vdibind.h>

    WORD
vq_cellarray( handle, xy, row_len, num_rows, el_used, rows_used, stat, colors )
WORD handle;		/* Physical device handle */
WORD xy[];
WORD row_len;
WORD num_rows;
WORD *el_used;
WORD *rows_used;
WORD *stat;
WORD colors[];
{
    i_ptsin( xy );
    i_intout( colors );

    contrl[0] = 27;
    contrl[1] = 2;
    contrl[3] = 0;
    contrl[6] = handle;
    contrl[7] = row_len;
    contrl[8] = num_rows;
    vdi();

    *el_used = contrl[9];
    *rows_used = contrl[10];
    *stat = contrl[11];
    i_ptsin( ptsin );
    i_intout( intout );
}
