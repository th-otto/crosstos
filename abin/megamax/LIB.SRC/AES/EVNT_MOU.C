
#include <portab.h>
#include <gembind.h>


	WORD
evnt_mouse(flags, x, y, width, height, pmx, pmy, pmb, pks)
	WORD		flags, x, y, width, height;
	WORD		*pmx, *pmy, *pmb, *pks;
{
	MO_FLAGS = flags;
	MO_X = x;
	MO_Y = y;
	MO_WIDTH = width;
	MO_HEIGHT = height;
	crys_if(EVNT_MOUSE);
	*pmx = EV_MX;
	*pmy = EV_MY;
	*pmb = EV_MB;
	*pks = EV_KS;
	return(RET_CODE);
}
