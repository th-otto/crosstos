
#include <vdibind.h>

    WORD
v_cellarray( handle, xy, row_length, el_per_row, num_rows, wr_mode, colors )
WORD handle;
WORD xy[4];
WORD row_length;
WORD el_per_row;
WORD num_rows;
WORD wr_mode;
WORD *colors;
{
    i_intin( colors );
    i_ptsin( xy );

    contrl[0] = 10;
    contrl[1] = 2;
    contrl[3] = row_length * num_rows;
    contrl[6] = handle;
    contrl[7] = row_length;
    contrl[8] = el_per_row;
    contrl[9] = num_rows;
    contrl[10] = wr_mode;
    vdi();

    i_intin( intin );
    i_ptsin( ptsin );
}
