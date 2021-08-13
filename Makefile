
# note the TARFILE_NAME embeds the release version number
TARFILE_NAME	= nifti2clib-0.0.1

USEZLIB         = -DHAVE_ZLIB

## Compiler  defines
CC		= gcc
#IFLAGS          = -I/home/sapjw12/dev/include/gsl
IFLAGS          = -I/home/sapjw12/dev/include
LFLAGS		= -L/home/sapjw12/dev/lib
#CFLAGS          = -g -Q -v -Wall -std=gnu99 -pedantic $(USEZLIB)
CFLAGS		= -g -Wall -std=gnu99 -pedantic $(USEZLIB) $(IFLAGS)

LLIBS 		= -lz -lm -lgsl -lgslcblas

MISC_OBJS	= znzlib.o argparse.o crosscorr.o
NIFTI_OBJS	= nifti2_io.o nifticdf.o
OBJS	   	= $(NIFTI_OBJS) $(MISC_OBJS)

# List my own programs
MYXS		= VoxelCrossCorr

# --------------------------------------------------
# default compile for C files
#%.o : %.c %.h
#	$(CC) -c $(CFLAGS) $< -o $@

# --------------------------------------------------

all: $(MYXS)

allobjs: $(OBJS)

VoxelCrossCorr: main.o allobjs
	$(CC) -o $@ $(CFLAGS) $(LFLAGS) $< $(OBJS) $(LLIBS)

clean:
	$(RM) *.o $(MYXS) 

