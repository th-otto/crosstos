/*
 * CPU abstraction layer
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"
#include "gemdos.h"
#include "tinyalloc.h"
#include "vt52.h"

#include <time.h>
#include <sys/time.h>

#if defined(WIN32)
#include <conio.h>
#else

#include <unistd.h>
#include <termios.h>
#include <assert.h>

#include "paths.h"

static int getch(void)
{
	int c = 0;
	struct termios org_opts;
	struct termios new_opts;
	int res = 0;

	//-----  store old settings -----------
	res = tcgetattr(STDIN_FILENO, &org_opts);
	assert(res == 0);
	//---- set new terminal parms --------
	memcpy(&new_opts, &org_opts, sizeof(new_opts));
	new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
	tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
	c = getchar();
	//------  restore old settings ---------
	res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
	assert(res == 0);

	if (c == 0x0a)
	{
		c = 0x0d;
	}

	return (c);
}
#endif

#define NUM_HANDLES 16
static file_t handles[NUM_HANDLES];
uint8_t ta_base[0x1000 * 16];
static uint8_t *rambase = NULL;

static int trace_gemdos;

#define printable_name(handle) handle >= 0 && handle < NUM_HANDLES ? handles[handle].fname : ""


static int32_t writer_file(int16_t handle, int32_t count, void *buf)
{
	return fwrite(buf, 1, count, handles[handle].fd);
}


static int32_t writer_null(int16_t handle, int32_t count, void *buf)
{
	return count;
}

static int32_t writer_term(int16_t handle, int32_t count, void *buf)
{
	int32_t tmp = count;
	uint8_t *c = buf;

	while (tmp--)
	{
		vt52_out(*c++, &handles[handle]);
	}

	return count;
}


static int32_t reader_file(int16_t handle, int32_t count, void *buf)
{
	return fread(buf, 1, count, handles[handle].fd);
}

static int32_t reader_null(int16_t handle, int32_t count, void *buf)
{
	return 0;
}

static int32_t reader_term(int16_t handle, int32_t count, void *buf)
{
	return fread(buf, 1, count, handles[handle].fd);
}

static int32_t Fopen(char *fname, int16_t mode)
{
	int32_t retval = GEMDOS_EFILNF;
	int i;

	for (i = 0; i < sizeof(handles) / sizeof(handles[0]); i++)
	{
		if (!handles[i].fd)
		{
			char *f = path_open(fname, true);

			switch (mode & 3)
			{
			case 0:
				handles[i].fd = fopen(f, "rb");
				break;
			case 1:
				handles[i].fd = fopen(f, "rb+");
				break;
			case 2:
				handles[i].fd = fopen(f, "rb+");
				break;
			default:
				break;
			}

			if (handles[i].fd)
			{
				handles[i].fname = f;

				retval = i;
			} else
			{
				path_close(f);
			}

			break;
		}
	}

	if (trace_gemdos)
		fprintf(stderr, "Fopen(\"%s\", %d) = %d\n", fname, mode, retval);

	return retval;
}

static int16_t Fclose(int16_t handle)
{
	if (trace_gemdos)
		fprintf(stderr, "Fclose(%d) (%s)\n", handle, printable_name(handle));
	if (handle >= 0 && handle < NUM_HANDLES)
	{
		if (handles[handle].term.state == not_term)
		{
			/* Protect against double Fclose() call
			   (encountered in Lattice C invocations) */
			if (handles[handle].fd)
			{
				fclose(handles[handle].fd);
			}

			if (handles[handle].fname)
			{
				path_close(handles[handle].fname);
			}

			handles[handle].fd = NULL;
			handles[handle].fname = NULL;
		}

		return GEMDOS_E_OK;				/* E_OK */
	}

	return GEMDOS_EBADF;				/* error */
}

static int16_t Fforce(int16_t stdh, int16_t nonstdh)
{
	if (trace_gemdos)
		fprintf(stderr, "Fforce(%d, %d) (not implemented)\n", stdh, nonstdh);
	return GEMDOS_E_OK;
}

static int16_t Fcreate(char *fname, int16_t attr)
{
	int32_t retval = GEMDOS_E_EACCDN;

	char *f = path_open(fname, false);

	if (f)
	{
		FILE *fd = fopen(f, "wb+");

		if (fd)
		{
			fclose(fd);

			/* Steve: I think we need to pass the original name to Fopen()
			   since that re-resolves the path name, using path_open()
			   again.
			   Passing "frame" would transform the already-transformed
			   name, which can lead to malformed paths.
			 */
			retval = Fopen(fname, 2);
		}

		path_close(f);
	}

	if (trace_gemdos)
		fprintf(stderr, "Fcreate(\"%s\", 0x%04x) = %d\n", fname, attr, retval);

	return retval;
}

static int16_t Fattrib(char *fname, int16_t wflag, int16_t attr)
{
	char *f = path_open(fname, true);

	if (trace_gemdos)
		fprintf(stderr, "Fattrib(\"%s\", %d, 0x%04x)\n", fname, wflag, attr);

	if (f)
	{
		path_close(f);
	}

	return GEMDOS_EFILNF;
}

static int32_t Frename(char *fname, char *new_fname)
{
	int32_t retval = GEMDOS_E_OK;

	char *f1 = path_open(fname, true);
	char *f2 = path_open(new_fname, true);

	if (trace_gemdos)
		fprintf(stderr, "Frename(%s, %s)\n", fname, new_fname);

	if (f1 && f2)
	{
		if (rename(f1, f2))
		{
			retval = GEMDOS_EFILNF;
		}

		path_close(f1);
		path_close(f2);
	}

	return retval;
}

static int16_t Fdelete(char *fname)
{
	int32_t retval = GEMDOS_E_OK;
	char *f = path_open(fname, true);

	if (trace_gemdos)
		fprintf(stderr, "Fdelete(%s)\n", fname);

	if (f)
	{
		if (remove(f))
		{
			retval = GEMDOS_EFILNF;
		}

		path_close(f);
	}

	return retval;
}

static int32_t Fread(int16_t handle, int32_t count, void *buf)
{

	if (handle >= 0 && handle < NUM_HANDLES)
	{
		if (handles[handle].fd)
		{
			int32_t amount = handles[handle].reader(handle, count, buf);

			if (trace_gemdos)
				fprintf(stderr, "Fread(%d, %d, 0x%08x) (%s) = %d\n", handle, count, (unsigned int)((uint8_t *)buf - rambase), printable_name(handle), amount);
			return amount;
		}
	}

	if (trace_gemdos)
		fprintf(stderr, "Fread(%d, %d, 0x%08x) (%s) = EBADF\n", handle, count, (unsigned int)((uint8_t *)buf - rambase), printable_name(handle));
	return GEMDOS_EBADF;
}

static int32_t Fwrite(int16_t handle, int32_t count, void *buf)
{

	if (handle >= 0 && handle < NUM_HANDLES)
	{
		if (handles[handle].fd)
		{
			int32_t amount = handles[handle].writer(handle, count, buf);

			if (trace_gemdos)
				fprintf(stderr, "Fwrite(%d, %d, 0x%08x) (%s) = %d\n", handle, count, (unsigned int)((uint8_t *)buf - rambase), printable_name(handle), amount);
			return amount;
		}
	}

	if (trace_gemdos)
		fprintf(stderr, "Fwrite(%d, %d, 0x%08x) (%s) = EBADF\n", handle, count, (unsigned int)((uint8_t *)buf - rambase), printable_name(handle));
	return GEMDOS_EBADF;
}

static int32_t Fseek(int32_t offset, int16_t handle, int16_t seekmode)
{
	int32_t r = -1;

	if (handle >= 0 && handle < NUM_HANDLES)
	{
		switch (seekmode)
		{
		case 0:
			r = fseek(handles[handle].fd, offset, SEEK_SET);
			break;
		case 1:
			r = fseek(handles[handle].fd, offset, SEEK_CUR);
			break;
		case 2:
			r = fseek(handles[handle].fd, offset, SEEK_END);
			break;
		default:
			break;
		}
	} else
	{
		r = GEMDOS_EBADF;
	}

	if (!r)
	{
		r = ftell(handles[handle].fd);
	}
	if (trace_gemdos)
		fprintf(stderr, "Fseek(%d, %d, %d) = %d\n", handle, offset, seekmode, r);

	return r;
}

static uint32_t Mshrink(uint32_t block, uint32_t newsize)
{
	if (trace_gemdos)
		fprintf(stderr, "Mshrink(0x%08x, 0x%08x) (not implemented)\n", block, newsize);
	return GEMDOS_E_OK;
}


void Mfree(uint32_t block)
{
	if (trace_gemdos)
		fprintf(stderr, "Mfree(0x%08x)\n", block);
	ta_free((void *) ((intptr_t) block));
}

uint32_t Malloc(int32_t bytes)
{
	uint32_t retval = 0;

	if (bytes == -1)
	{
		retval = ta_biggest_block();
	} else
	{
		retval = (uint32_t) ((intptr_t) ta_alloc(bytes));
	}

	if (trace_gemdos)
		fprintf(stderr, "Malloc(%08x) = 0x%08x\n", bytes, retval);

	return retval;
}

static uint32_t Mxalloc(int32_t bytes, int16_t mode)
{
	(void) mode;
	return Malloc(bytes);
}

static uint32_t Pexec(int16_t mode, char *name, char *cmd, char *env)
{
	if (trace_gemdos)
		fprintf(stderr, "Pexec(%d, %s, %s, %s) (not implemented)\n", mode, name, cmd, env);

	return 0;
}


uint32_t gemdos_dispatch(uint16_t opcode, uint32_t pd)
{
	uint32_t retval;

	if (opcode >= 0x100)				/* MiNT calls: not supported, but ignored */
	{
		if (trace_gemdos)
			fprintf(stderr, "MiNT GEMDOS call (0x%04x)\n", opcode);
		return -32;
	}

	switch (opcode)
	{
	case 0x0000:						/* Pterm0() */
		retval = GEMDOS_E_OK;
		if (trace_gemdos)
			fprintf(stderr, "Pterm0()\n");
		exit(0);
		break;

	case 0x0001:						/* Cconin() */
		if (trace_gemdos)
			fprintf(stderr, "Cconin()\n");
		retval = getchar();			/* ASCII in bits 0...7 */
		break;

	case 0x0002:						/* Cconout() */
		{
			int16_t c = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

			if (trace_gemdos)
				fprintf(stderr, "Cconout(0x%02x)\n", c);
			retval = (vt52_out(c, &handles[1]) == EOF) ? -1 : GEMDOS_E_OK;
		}
		break;

	case 0x0004:						/* int32_t Cauxout ( int16_t c ); */
		{
			int16_t c = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

			if (trace_gemdos)
				fprintf(stderr, "Cauxout(0x%02x)\n", c);
			retval = (vt52_out(c, &handles[2]) == EOF) ? -1 : GEMDOS_E_OK;
		}
		break;

	case 0x0006:						/* int32_t Crawio ( int16_t w ); */
		{
			uint16_t w = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

			if (trace_gemdos)
				fprintf(stderr, "Crawio(0x%02x)\n", w);
			if (w == 0x00ff)			/* Read character from standard input */
			{
				/* ignore */
				retval = GEMDOS_E_OK;
			} else
			{
				retval = (vt52_out(w, &handles[1]) == EOF) ? -1 : GEMDOS_E_OK;
			}
		}
		break;

	case 0x0007:						/* int32_t Crawcin ( void ); */
		if (trace_gemdos)
			fprintf(stderr, "Crawcin()\n");
		do
		{
			retval = getch();		/* ASCII in bits 0...7 */
		} while (!retval);
		break;

	case 0x0008:						/* Cnecin() */
		if (trace_gemdos)
			fprintf(stderr, "Cnecin()\n");
		retval = GEMDOS_E_OK;
		break;

	case 0x0009:						/* Cconws() */
		{
			uint32_t addr = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
			char *buf = (char *) &rambase[addr];

			if (trace_gemdos)
				fprintf(stderr, "Cconws(0x%08x)\n", addr);
			retval = GEMDOS_E_OK;

			while (*buf)
			{
				if (vt52_out(*buf++, &handles[1]) == EOF)
				{
					break;
				}
			}
		}
		break;

	case 0x000a:						/* Cconrs() */
		{
			uint32_t addr = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
			char *buf = (char *) &rambase[addr];

			if (trace_gemdos)
				fprintf(stderr, "Cconrs(0x%08x)\n", addr);
			//  getline(buf,128,stdin);

			sprintf(buf, "%s", "abc");

			retval = GEMDOS_E_OK;
		}
		break;

	case 0x000b:						/* Cconis() */
		if (trace_gemdos)
			fprintf(stderr, "Cconis()\n");
		retval = GEMDOS_E_OK;
		break;

	case 0x000e:						/* Dsetdrv() */
		{
			int drv = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

			retval = 1L << 2;
			if (trace_gemdos)
				fprintf(stderr, "Dsetdrv(%d) = 0x%08x\n", drv, retval);
		}
		break;

	case 0x0019:						/* Dgetdrv() */
		retval = 2;					/* allways C: */
		if (trace_gemdos)
			fprintf(stderr, "Dgetdrv() = %d\n", retval);
		break;

	case 0x001A:						/* void Fsetdta ( DTA *buf ); */
		{
			uint32_t dta = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

			if (trace_gemdos)
				fprintf(stderr, "Fsetdta(0x%08x)\n", dta);
			WRITE_LONG(rambase, pd + OFF_P_DTA, dta);
			retval = GEMDOS_E_OK;
		}
		break;

	case 0x0020:						/* int32_t Super ( void *stack ); */
		{
			uint32_t stack = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

			if (trace_gemdos)
				fprintf(stderr, "Super(0x%08x)\n", stack);
			if (stack == 1)
			{
				retval = 0;				/* user mode */
			} else
			{
				retval = 0x1234567;
			}
		}
		break;

	case 0x002a:						/* Tgetdate() */
		{
			time_t t;
			struct tm tm;
			
			time(&t);
			tm = *localtime(&t);
			retval = (((tm.tm_year + 1900 - 1980) & 0x7f) << 9) | ((tm.tm_mon + 1) << 5) | (tm.tm_mday & 0x1f);
			if (trace_gemdos)
				fprintf(stderr, "Tgetdate() = 0x%04x (%04d/%02d/%02d)\n", retval, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
		}
		break;

	case 0x002c:						/* Tgettime() */
		{
			time_t t;
			struct tm tm;
			
			time(&t);
			tm = *localtime(&t);
			retval = ((tm.tm_hour & 0x1f) << 11) | ((tm.tm_min & 0x3f) << 5) | ((tm.tm_sec & 0x3f) >> 1);
			if (trace_gemdos)
				fprintf(stderr, "Tgettime() = 0x%04x (%02d:%02d:%02d)\n", retval, tm.tm_hour, tm.tm_min, tm.tm_sec);
		}
		break;

	case 0x002f:						/* DTA *Fgetdta ( void ); */
		if (trace_gemdos)
			fprintf(stderr, "Fgetdta()\n");
		retval = READ_LONG(rambase, pd + OFF_P_DTA);
		break;

	case 0x0030:						/* Sversion() */
		if (trace_gemdos)
			fprintf(stderr, "Sversion()\n");
		retval = 30 << 8;				/* GEMDOS 0.30 */
		break;

	case 0x3c:							/* Fcreate() */
		{
			uint32_t addr = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
			char *fname = (char *) &rambase[addr];
			int16_t attr = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

			retval = Fcreate(fname, attr);
		}
		break;

	case 0x3d:							/* Fopen() */
		{
			char *fname = (char *) &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
			int16_t mode = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

			retval = Fopen(fname, mode);
		}
		break;

	case 0x003e:						/* Fclose() */
		{
			int16_t handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

			retval = Fclose(handle);
		}
		break;

	case 0x003f:						/* Fread() */
		{
			int16_t handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
			int32_t count = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);
			char *buf = (char *) &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

			retval = Fread(handle, count, buf);
		}
		break;

	case 0x0040:						/* Fwrite() */
		{
			int16_t handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
			int32_t count = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);
			char *buf = (char *) &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

			retval = Fwrite(handle, count, buf);
		}
		break;

	case 0x0043:						/* int16_t Fattrib ( const int8_t *filename, int16_t wflag, int16_t attrib ); */
		{
			char *fname = (char *) &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
			int16_t wflag = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);
			int16_t attrib = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8);

			retval = Fattrib(fname, wflag, attrib);
		}
		break;

	case 0x0041:						/* int16_t Fdelete ( const int8_t *fname ) */
		{
			char *fname = (char *) &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];

			retval = Fdelete(fname);
		}
		break;

	case 0x0042:						/* Fseek() */
		{
			int32_t offset = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
			int16_t handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);
			int16_t mode = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8);

			retval = Fseek(offset, handle, mode);
		}
		break;

	case 0x0044:						/* Mxalloc() */
		{
			int32_t count = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
			int16_t mode = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

			retval = Mxalloc(count, mode);
		}
		break;

	case 0x0045:						/* Fdup() */
		{
			int16_t stdh = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

			if (trace_gemdos)
				fprintf(stderr, "Fdup(%d) (not implemented)\n", stdh);
			retval = GEMDOS_E_OK;
		}
		break;

	case 0x0046:						/* int16_t Fforce ( int16_t stdh, int16_t nonstdh ); */
		{
			int16_t stdh = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
			int16_t nonstdh = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);

			retval = Fforce(stdh, nonstdh);
		}
		break;

	case 0x0047:						/* int16_t Dgetpath ( int8_t *path, int16_t driveno ); */
		{
			uint32_t addr = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
			char *buf = (char *) &rambase[addr];
			int16_t drive = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

			(void) drive;

			strcpy(buf, ".");

			retval = GEMDOS_E_OK;
		}
		break;

	case 0x0048:						/* Malloc() */
		{
			int32_t count = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

			retval = Malloc(count);
		}
		break;

	case 0x0049:						/* Mfree() */
		{
			uint32_t block = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

			Mfree(block);

			retval = GEMDOS_E_OK;
		}
		break;

	case 0x004a:						/* Mshrink() */
		{
			uint32_t block = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4);
			uint32_t newsize = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8);

			retval = Mshrink(block, newsize);
		}
		break;

	case 0x004b:						/* int32_t Pexec ( uint16_t mode, ... ) */
		{
			int16_t mode = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

			char *name = (char *) &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4)];
			char *cmd = (char *) &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];
			char *env = (char *) &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 12)];

			retval = Pexec(mode, name, cmd, env);
		}
		break;

	case 0x004c:						/* Pterm() */
		{
			int16_t status = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);

			if (trace_gemdos)
				fprintf(stderr, "Pterm(%d)\n", status);
			exit(status);
		}
		break;

	case 0x004e:						/* int32_t Fsfirst ( const int8_t *filename, int16_t attr ) */
		{
			char *fname = (char *) &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2)];
			int16_t attr = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);

			char *f = path_open(fname, true);

			if (f)
			{
				path_close(f);
			}

			if (trace_gemdos)
				fprintf(stderr, "Fsfirst(\"%s\", 0x%04x) (%s)\n", fname, attr, f);

			retval = GEMDOS_EFILNF;
		}
		break;

	case 0x004f:						/* int16_t Fsnext ( void ) */
		if (trace_gemdos)
			fprintf(stderr, "Fsnext()\n");
		retval = GEMDOS_EFILNF;
		break;

	case 0x0056:						/* int32_t Frename ( const int8_t *oldname, const int8_t *newname ) */
		{
			char *fname = (char *) &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 4)];
			char *new_fname = (char *) &rambase[READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8)];

			retval = Frename(fname, new_fname);
		}
		break;

	case 0x0057:						/* void Fdatime ( DOSTIME *timeptr, int16_t handle, int16_t wflag ); */
		{
			uint32_t tptr = READ_LONG(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 2);
			int16_t handle = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 6);
			int16_t wflag = READ_WORD(rambase, m68k_get_reg(NULL, M68K_REG_SP) + 8);

			if (trace_gemdos)
				fprintf(stderr, "Fdatime(0x%08x, %d, %d) (%s)\n", tptr, handle, wflag, printable_name(handle));

			if (wflag)
			{
				/* Set values */
			} else
			{
				/* Obtain values */
				WRITE_WORD(rambase, tptr + 0, 0x0000);	/* time */
				WRITE_WORD(rambase, tptr + 2, 0x0000);	/* date */
			}
			retval = GEMDOS_E_OK;
		}
		break;

	default:
		retval = GEMDOS_E_OK;
		fprintf(stderr, "unknown GEMDOS call (0x%04x)\n", opcode);
		exit(1);
		break;
	}

	return retval;
}


void gemdos_init(uint8_t *ram, uint32_t ramsize)
{
	int i;

	rambase = ram;

	ta_init();

	i = 0;
	handles[i].fd = stdin;
	handles[i].fname = "STDIN:";
	handles[i].term.state = normal;
	handles[i].reader = reader_term;
	handles[i].writer = writer_null;

	i++;
	handles[i].fd = stdout;
	handles[i].fname = "STDOUT:";
	handles[i].term.state = normal;
	handles[i].reader = reader_null;
	handles[i].writer = writer_term;

	i++;
	handles[i].fd = stderr;
	handles[i].fname = "STDAUX:";
	handles[i].term.state = normal;
	handles[i].reader = reader_null;
	handles[i].writer = writer_term;

	i++;
	handles[i].fd = stderr;
	handles[i].fname = "STDPRN:";
	handles[i].term.state = normal;
	handles[i].reader = reader_null;
	handles[i].writer = writer_term;

	for (; i < sizeof(handles) / sizeof(handles[0]); i++)
	{
		handles[i].fd = 0;
		handles[i].fname = NULL;
		handles[i].term.state = not_term;
		handles[i].reader = reader_file;
		handles[i].writer = writer_file;
	}

	trace_gemdos = getenv("TRACE_GEMDOS") != NULL;
}
