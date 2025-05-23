**
** $Header: e:/src/i\gemdos.i,v 1.1 1993/11/09 10:13:02 AGK Exp $
**
** GEMDOS magic numbers
**
** (c) Copyright 1989, 1992, 1993 HiSoft
**

		IFND	__GEMDOS_I
__GEMDOS_I	EQU 1

p_term0		equ	$00
c_conin		equ	$01
c_conout	equ	$02
c_auxin		equ	$03
c_auxout	equ	$04
c_prnout	equ	$05
c_rawio		equ	$06
c_rawcin	equ	$07
c_necin		equ	$08
c_conws		equ	$09
c_conrs		equ	$0a
c_conis		equ	$0b
d_setdrv	equ	$0e
c_conos		equ	$10
c_prnos		equ	$11
c_auxis		equ	$12
c_auxos		equ	$13
d_getdrv	equ	$19
f_setdta	equ	$1a
super		equ	$20
t_getdate	equ	$2a
t_setdate	equ	$2b
t_gettime	equ	$2c
t_settime	equ	$2d
f_getdta	equ	$2f
s_version	equ	$30
p_termres	equ	$31
d_free		equ	$36
d_create	equ	$39
d_delete	equ	$3a
d_setpath	equ	$3b
f_create	equ	$3c
f_open		equ	$3d
f_close		equ	$3e
f_read		equ	$3f
f_write		equ	$40
f_delete	equ	$41
f_seek		equ	$42
f_attrib	equ	$43
m_xalloc	equ	$44
f_dup		equ	$45
f_force		equ	$46
d_getpath	equ	$47
m_alloc		equ	$48
m_free		equ	$49
m_shrink	equ	$4a
p_exec		equ	$4b
p_term		equ	$4c
f_sfirst	equ	$4e
f_snext		equ	$4f
f_rename	equ	$56
f_datime	equ	$57
f_lock		equ	$5c

; MiNT kernel calls
s_yield		equ	$ff
f_pipe		equ	$100
f_cntl		equ	$104
f_instat	equ	$105
f_outstat	equ	$106
f_getchar	equ	$107
f_putchar	equ	$108
p_wait		equ	$109
p_nice		equ	$10a
p_getpid	equ	$10b
p_getppid	equ	$10c
p_getpgrp	equ	$10d
p_setpgrp	equ	$10e
p_getuid	equ	$10f
p_setuid	equ	$110
p_kill		equ	$111
p_signal	equ	$112
p_vfork		equ	$113
p_getgid	equ	$114
p_setgid	equ	$115
p_sigblock	equ	$116
p_sigsetmask	equ	$117
p_usrval	equ	$118
p_domain	equ	$119
p_sigreturn	equ	$11a
p_fork		equ	$11b
p_wait3		equ	$11c
f_select	equ	$11d
p_rusage	equ	$11e
p_setlimit	equ	$11f
t_alarm		equ	$120
p_ause		equ	$121
s_ysconf	equ	$122
p_sigpending	equ	$123
d_pathconf	equ	$124
p_msg		equ	$125
f_midipipe	equ	$126
p_renice	equ	$127
d_opendir	equ	$128
d_readdir	equ	$129
d_rewinddir	equ	$12a
d_closedir	equ	$12b
f_xattr		equ	$12c
f_link		equ	$12d
f_symlink	equ	$12e
f_readlink	equ	$12f
d_cntl		equ	$130
f_chown		equ	$131
f_chmod		equ	$132
p_umask		equ	$133
p_semaphore	equ	$134 
d_lock		equ	$135
p_sigpause	equ	$136
p_sigaction	equ	$137
p_geteuid	equ	$138
p_getegid	equ	$139
p_waitpid	equ	$13a
d_getcwd	equ	$13b
s_alert		equ	$13c

EINVFN		equ	-32
EFILNF		equ	-33
EPTHNF		equ	-34
ENHNDL		equ	-35
EACCDN		equ	-36
EIHNDL		equ	-37
ENSMEM		equ	-39
EIMBA		equ	-40
EDRIVE		equ	-46
ENSNAME		equ	-48
ENMFIL		equ	-49
ERANGE		equ	-64
EINTRN		equ	-65
EPLFMT		equ	-66
EGSBF		equ	-67
		ENDC
