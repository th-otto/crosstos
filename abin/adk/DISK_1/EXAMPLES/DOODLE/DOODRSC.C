#define T0OBJ 0
#define T1OBJ 26
#define T2OBJ 37
#define T3OBJ 42
#define FREEBB 7
#define FREEIMG 7
#define FREESTR 41

BYTE *rs_strings[] = {
" Desk ",
" File ",
" Options",
"  Doodle Info...",
"--------------------",
"  Desk Accessory 1  ",
"  Desk Accessory 2  ",
"  Desk Accessory 3  ",
"  Desk Accessory 4  ",
"  Desk Accessory 5  ",
"  Desk Accessory 6  ",
"  Load...",
"  Save",
"  Quit",
"  Save As...",
"  Abandon",
"  Erase Picture",
"  Pen/Eraser Selection",
"-----------------------",
"Doodle",
"GEM Sample Application",
"Authors",
"-------",
"Tom Rolander",
"Ok",
"Version 1.2",
"Copyright 1985, ATARI Corp.",
"Tim Oren",
"Save Doodle picture as",
"Ok",
"Cancel",
"",
"________.DOO",
"ANNNNNNN",
"Doodle Pen/Eraser Selection",
"Pens:",
"Ok",
"Erasers:",
"Cancel",
"Pen Color:",
"Selected:",
"[2][You are about|to write over|an existing file.][Ok| Cancel ]",
"[3][ GEM does not have| any windows left,| Doodle aborting][ Sorry\
 ]"};

WORD IMAG0[] = {
0x0, 0x0, 0x0, 0x0, 
0x73, 0xC000, 0x0, 0x8C, 
0x9C00, 0x0, 0x390, 0x67C0, 
0x0, 0x7E30, 0x8900, 0x3, 
0xC263, 0x9300, 0xE, 0x14AA, 
0x2600, 0x38, 0xA955, 0x5F00, 
0x60, 0x4000, 0x8AE0, 0x196, 
0x1800, 0x540, 0x345, 0x500, 
0x1050, 0x227, 0x1444, 0x110, 
0x7C0, 0xB00, 0x4428, 0x20, 
0x1211, 0x1148, 0x1FF2, 0x2A44, 
0x4418, 0x1BF5, 0x1311, 0x12A8, 
0x1182, 0xA6AA, 0xA218, 0x18F9, 
0x4D55, 0x5648, 0x80E, 0x70DF, 
0xFEA8, 0x3, 0xF0E0, 0x7C50, 
0x0, 0x18FE, 0x410, 0x0, 
0x88F, 0x80B0, 0x800, 0xA80, 
0xF960, 0x800, 0xA47, 0x80C0, 
0x1400, 0xBC0, 0xF880, 0x1400, 
0x901, 0x180, 0x800, 0x60E, 
0xF00, 0x0, 0x401, 0x7800, 
0x0, 0x7, 0x8000, 0x0, 
0x7C, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG1[] = {
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x3, 0x8000, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG2[] = {
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x7, 0xC000, 
0xF, 0xE000, 0x7, 0xC000, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG3[] = {
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x7, 0xC000, 0x1F, 0xF000, 
0x3F, 0xF800, 0x1F, 0xF000, 
0x7, 0xC000, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG4[] = {
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x7, 0xC000, 0x4, 0x4000, 
0x7, 0xC000, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG5[] = {
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x1F, 0xF000, 
0x10, 0x1000, 0x10, 0x1000, 
0x10, 0x1000, 0x1F, 0xF000, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

WORD IMAG6[] = {
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x7F, 0xFC00, 0x40, 0x400, 
0x40, 0x400, 0x40, 0x400, 
0x40, 0x400, 0x40, 0x400, 
0x7F, 0xFC00, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0};

LONG rs_frstr[] = {
41L,
42L};

BITBLK rs_bitblk[] = {
0L, 6, 32, 0, 0, 1,
1L, 4, 16, 0, 0, 1,
2L, 4, 16, 0, 0, 1,
3L, 4, 16, 0, 0, 1,
4L, 4, 16, 0, 0, 1,
5L, 4, 16, 0, 0, 1,
6L, 4, 16, 0, 0, 1};

LONG rs_frimg[] = {
0};

ICONBLK rs_iconblk[] = {
0};

TEDINFO rs_tedinfo[] = {
31L, 32L, 33L, 3, 6, 0, 0x1180, 0x0, -1, 1,13};

OBJECT rs_object[] = {
-1, 1, 6, G_IBOX, NONE, NORMAL, 0x0L, 0,0, 80,25,
6, 2, 2, G_BOX, NONE, NORMAL, 0x1100L, 0,0, 80,513,
1, 3, 5, G_IBOX, NONE, NORMAL, 0x0L, 2,0, 20,769,
4, -1, -1, G_TITLE, NONE, NORMAL, 0x0L, 0,0, 6,769,
5, -1, -1, G_TITLE, NONE, NORMAL, 0x1L, 6,0, 6,769,
2, -1, -1, G_TITLE, NONE, NORMAL, 0x2L, 12,0, 8,769,
0, 7, 22, G_IBOX, NONE, NORMAL, 0x0L, 0,769, 80,19,
16, 8, 15, G_BOX, NONE, NORMAL, 0xFF1100L, 2,0, 20,8,
9, -1, -1, G_STRING, NONE, NORMAL, 0x3L, 0,0, 19,1,
10, -1, -1, G_STRING, NONE, DISABLED, 0x4L, 0,1, 20,1,
11, -1, -1, G_STRING, NONE, NORMAL, 0x5L, 0,2, 20,1,
12, -1, -1, G_STRING, NONE, NORMAL, 0x6L, 0,3, 20,1,
13, -1, -1, G_STRING, NONE, NORMAL, 0x7L, 0,4, 20,1,
14, -1, -1, G_STRING, NONE, NORMAL, 0x8L, 0,5, 20,1,
15, -1, -1, G_STRING, NONE, NORMAL, 0x9L, 0,6, 20,1,
7, -1, -1, G_STRING, NONE, NORMAL, 0xAL, 0,7, 20,1,
22, 17, 21, G_BOX, NONE, NORMAL, 0xFF1100L, 8,0, 13,5,
18, -1, -1, G_STRING, NONE, NORMAL, 0xBL, 0,0, 13,1,
19, -1, -1, G_STRING, NONE, DISABLED, 0xCL, 0,1, 13,1,
20, -1, -1, G_STRING, NONE, NORMAL, 0xDL, 0,4, 13,1,
21, -1, -1, G_STRING, NONE, NORMAL, 0xEL, 0,2, 13,1,
16, -1, -1, G_STRING, NONE, DISABLED, 0xFL, 0,3, 13,1,
6, 23, 25, G_BOX, NONE, NORMAL, 0xFF1100L, 14,0, 23,3,
24, -1, -1, G_STRING, NONE, NORMAL, 0x10L, 0,2, 23,1,
25, -1, -1, G_STRING, NONE, NORMAL, 0x11L, 0,0, 23,1,
22, -1, -1, G_STRING, LASTOB, DISABLED, 0x12L, 0,1, 23,1,
-1, 1, 10, G_BOX, NONE, OUTLINED, 0x21100L, 8,9, 38,12,
2, -1, -1, G_STRING, NONE, NORMAL, 0x13L, 14,1, 9,1,
3, -1, -1, G_STRING, NONE, NORMAL, 0x14L, 7,2, 22,1,
4, -1, -1, G_STRING, NONE, NORMAL, 0x15L, 14,4, 7,1,
5, -1, -1, G_STRING, NONE, NORMAL, 0x16L, 14,5, 7,1,
6, -1, -1, G_STRING, NONE, NORMAL, 0x17L, 11,6, 12,1,
7, -1, -1, G_BUTTON, 0x7, NORMAL, 0x18L, 27,5, 8,1,
8, -1, -1, G_STRING, NONE, NORMAL, 0x19L, 12,9, 12,1,
9, -1, -1, G_STRING, NONE, NORMAL, 0x1AL, 6,10, 22,1,
10, -1, -1, G_IMAGE, NONE, NORMAL, 0x0L, 2,4, 6,4,
0, -1, -1, G_STRING, LASTOB, NORMAL, 0x1BL, 13,7, 8,1,
-1, 1, 4, G_BOX, NONE, OUTLINED, 0x21100L, 0,0, 38,7,
2, -1, -1, G_STRING, NONE, NORMAL, 0x1CL, 2,2, 25,1,
3, -1, -1, G_BUTTON, 0x7, NORMAL, 0x1DL, 28,2, 8,1,
4, -1, -1, G_BUTTON, 0x5, NORMAL, 0x1EL, 28,4, 8,1,
0, -1, -1, G_FTEXT, 0x28, NORMAL, 0x0L, 6,4, 12,1,
-1, 1, 13, G_BOX, NONE, OUTLINED, 0x21100L, 8,9, 38,13,
2, -1, -1, G_STRING, NONE, NORMAL, 0x22L, 5,1, 30,1,
9, 3, 8, G_IBOX, NONE, NORMAL, 0x1100L, 11,2, 22,6,
4, -1, -1, G_IMAGE, 0x11, NORMAL, 0x1L, 2,1, 4,2,
5, -1, -1, G_IMAGE, 0x11, NORMAL, 0x2L, 9,1, 4,2,
6, -1, -1, G_IMAGE, 0x11, NORMAL, 0x3L, 16,1, 4,2,
7, -1, -1, G_IMAGE, 0x11, NORMAL, 0x4L, 2,3, 4,2,
8, -1, -1, G_IMAGE, 0x11, NORMAL, 0x5L, 9,3, 4,2,
2, -1, -1, G_IMAGE, 0x11, NORMAL, 0x6L, 16,3, 4,2,
10, -1, -1, G_STRING, NONE, NORMAL, 0x23L, 6,3, 5,1,
11, -1, -1, G_BUTTON, 0x7, NORMAL, 0x24L, 29,9, 7,1,
12, -1, -1, G_STRING, NONE, NORMAL, 0x25L, 3,5, 8,1,
13, -1, -1, G_BUTTON, 0x5, NORMAL, 0x26L, 29,11, 7,1,
0, 14, 24, G_IBOX, NONE, NORMAL, 0x100L, 0,8, 29,5,
15, -1, -1, G_STRING, NONE, NORMAL, 0x27L, 1,1, 11,1,
18, 16, 17, G_IBOX, NONE, NORMAL, 0x1100L, 13,1, 12,1,
17, -1, -1, 0x11B, TOUCHEXIT, NORMAL, 0x4FF1100L, 0,0, 2,1,
15, -1, -1, 0x21B, TOUCHEXIT, NORMAL, 0x3FF1100L, 10,0, 2,1,
23, 19, 22, G_IBOX, NONE, NORMAL, 0x1100L, 15,1, 8,1,
20, -1, -1, 0x31B, TOUCHEXIT, NORMAL, 0x31FF1071L, 0,0, 2,1,
21, -1, -1, 0x31B, TOUCHEXIT, NORMAL, 0x32FF1072L, 2,0, 2,1,
22, -1, -1, 0x31B, TOUCHEXIT, NORMAL, 0x33FF1073L, 4,0, 2,1,
18, -1, -1, 0x31B, TOUCHEXIT, NORMAL, 0x34FF1074L, 6,0, 2,1,
24, -1, -1, G_BOXCHAR, NONE, NORMAL, 0x31FF1071L, 13,3, 2,1,
13, -1, -1, G_STRING, LASTOB, NORMAL, 0x28L, 2,3, 9,1};

LONG rs_trindex[] = {
0L,
26L,
37L,
42L};

struct foobar {
	WORD	dummy;
	WORD	*image;
	} rs_imdope[] = {
0, &IMAG0[0],
0, &IMAG1[0],
0, &IMAG2[0],
0, &IMAG3[0],
0, &IMAG4[0],
0, &IMAG5[0],
0, &IMAG6[0]};

#define NUM_STRINGS 43
#define NUM_FRSTR 2
#define NUM_IMAGES 7
#define NUM_BB 7
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_TI 1
#define NUM_OBS 67
#define NUM_TREE 4

BYTE pname[] = "DOODLE.RSC";

