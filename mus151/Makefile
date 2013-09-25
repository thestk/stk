# MUS151 Makefile - Global version for Unix systems which have GNU
# Makefile utilities installed.  If you do not have the GNU Makefile
# utilities, I suggest you download them.  If that is not possible,
# look at syntmono/Makefile.sgi to see what has to be done to get
# things to compile.
#
# by Gary P. Scavone
# CCRMA, Stanford University, 1998.

OS = $(shell uname)

# You will have to modify this path to correspond to the correct
# location in your system.  The following definition corresponds
# to an STK project directory that is a subdirectory of the core
# STK distribution.
STK_PATH = ../STK/

O_FILES		=	Object.o Envelope.o WvIn.o RawWvIn.o \
				SKINI11.o swapstuf.o WvOut.o \
				RTWvOut.o RTSoundIO.o \
				\
				TwoOsc.o threads.o

RM = /bin/rm 

ifeq ($(OS),IRIX) # These are for SGI
	INSTR = MUS151
	CC = CC -O2 -D__OS_IRIX_ # -g -fullwarn -D__SGI_CC__
	LIBRARY = -L/usr/sgitcl/lib -laudio -lmd -lm
endif

ifeq ($(OS),Linux) # These are for Linux
	INSTR = MUS151
	CC = g++ -O3 -D__OS_Linux_ # -g
	LIBRARY = -lpthread -lm
endif

%.o : $(STK_PATH)%.cpp
	$(CC) -c $(<) -o $@

MUS151: MUS151.cpp $(O_FILES)
	$(CC) -o MUS151 MUS151.cpp $(O_FILES) $(LIBRARY)

# Personal $(O_FILES) :

TwoOsc.o : TwoOsc.cpp
	$(CC) -c TwoOsc.cpp

threads.o : threads.cpp
	$(CC) -c threads.cpp

all: $(INSTR)

clean : 
	rm *.o
	rm $(INSTR)

cleanIns : 
	rm $(INSTR)

strip : 
	strip $(INSTR)


