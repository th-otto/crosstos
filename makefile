NAME			= test
CC				= gcc
AS              = gcc
CFLAGS  		= -Wall -pedantic -std=gnu99 -O3 -DTLSF_STATISTIC
#DEFINES			= -DTOS_BINARY="\"./abin/PCC.TTP\"" -D__UNIX__ 
DEFINES			= -UTOS_BINARY -D__UNIX__ 

INCLUDES		= -I. -I./mushashi -I./TLSF-2.4.6/src
LIBS			=
LDFLAGS   		= 

OBJECTS			= ./main.o \
				  ./cpu.o  	\
				  ./gemdos.o  	\
				  ./bios.o  	\
				  ./xbios.o  	\
				  ./musashi/m68kcpu.o 	\
				  ./musashi/m68kops.o 	\
				  ./musashi/m68kopac.o 	\
				  ./musashi/m68kopnz.o 	\
				  ./musashi/m68kopdm.o 	\
				  ./musashi/m68kdasm.o

OBJECTS			+= ./TLSF-2.4.6/src/tlsf.o

OBJECTS			+= ./binary.o


# Build all
all:		$(NAME)

# Generate target
$(NAME):	$(OBJECTS)
		$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@
		strip $(NAME)

# Generic compilation target
.c.o:
		$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

.S.o:
		$(CC) -c $(CFLAGS) $(INCLUDES) $(DEFINES) $< -o $@

# Remove all build files
clean:
		rm -f $(OBJECTS)
