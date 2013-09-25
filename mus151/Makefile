# MUS151 Makefile - Global version for Unix systems which have GNU
# Makefile utilities installed.
#
# This is an example project Makefile, to demonstrate how one might
# develop an STK project that resides apart from the core STK
# distribution.  It is highly recommended that you keep your
# personal STK projects separate from the core distribution, to
# make upgrading to newer releases more simple.
#
# by Gary P. Scavone
# CCRMA, Stanford University, 1998.

OS = $(shell uname)

# You will have to modify this path to correspond to the correct
# location in your system.  The following definition corresponds
# to an STK project directory that is a subdirectory of the core
# STK distribution.
STK_PATH = ../

O_FILES		=	Object.o Envelope.o RawWave.o RawLoop.o \
				SKINI11.o swapstuf.o WvOut.o \
				RTWvOut.o RTSoundIO.o \
				\
				TwoOsc.o

RM = /bin/rm 

ifeq ($(OS),IRIX) # These are for SGI
	INSTR = MUS151
	CC = CC -O # -g -fullwarn -D__SGI_CC__
	LIBRARY = -L/usr/sgitcl/lib -laudio -lmd -lm
endif

ifeq ($(OS),Linux) # These are for Linux
	INSTR = MUS151
	CC = gcc -O3 # -g -pg -O3
	LIBRARY = -lpthread -lm
#	LIBRARY = /lib/libpthread.so.0 -lm
endif

%.o : $(STK_PATH)%.cpp
	$(CC) -c $(<) -o $@

MUS151: MUS151.cpp $(O_FILES)
	$(CC) -o MUS151 MUS151.cpp $(O_FILES) $(LIBRARY)

# Personal $(O_FILES) :

TwoOsc.o : TwoOsc.cpp
	$(CC) -c TwoOsc.cpp

all: $(INSTR)

clean : 
	rm *.o
	rm $(INSTR)

cleanIns : 
	rm $(INSTR)

strip : 
	strip $(INSTR)


