# STK Makefile - Global version for Unix systems which have GNU
# Makefile utilities installed.  If this Makefile does not work on
# your system, try using the platform specific Makefiles (.sgi,
# .next, and .linux).

OS = $(shell uname)

# The following definition indicates the relative location of
# the STK src directory.
STK_SRC = ../../src/

OBJECTS	=	Stk.o Envelope.o ADSR.o Noise.o WvIn.o WaveLoop.o WvOut.o \
					Filter.o OneZero.o OnePole.o PoleZero.o TwoZero.o \
					BiQuad.o Delay.o DelayL.o DelayA.o Reverb.o PRCRev.o \
					FormSwep.o ReedTabl.o JetTabl.o BowTabl.o \
					Instrmnt.o Clarinet.o BlowHole.o Saxofony.o Flute.o Brass.o BlowBotl.o \
					Bowed.o Plucked.o StifKarp.o Sitar.o PluckTwo.o Mandolin.o Mesh2D.o \
					FM.o Rhodey.o Wurley.o TubeBell.o HevyMetl.o PercFlut.o BeeThree.o FMVoices.o \
					Sampler.o Moog.o Simple.o Drummer.o BandedWG.o Shakers.o \
					 Modal.o ModalBar.o Resonate.o \
					\
					Messager.o SKINI.o utilities.o


RT_OBJECTS  =	RtMidi.o RtWvOut.o RtAudio.o Thread.o Socket.o

INCLUDE = -I../../include
RM = /bin/rm

INSTR = demo

ifeq ($(OS),NEXTSTEP) # These are for NeXT
#	CC = cc -arch m68k -arch i386 -Wall -D__OS_NeXT_
	CC = g++ -bm68k-next-nextstep3 -bi386-next-nextstep3 -Wall -fhandle-exceptions
endif

ifeq ($(OS),IRIX) # These are for SGI
	CC = CC -D__IRIX_AL__ # -g -fullwarn -D__SGI_CC__ -O2
	OBJECTS += $(RT_OBJECTS)
	LIBRARY = -laudio -lmd -lm -lpthread
endif

ifeq ($(OS),Linux) # These are for Linux
	OBJECTS += $(RT_OBJECTS)
	CC = g++ -O3 -Wall -D__LITTLE_ENDIAN__ -D__LINUX_OSS__ #-g
	LIBRARY = -lpthread -lm #-lasound
endif

%.o : $(STK_SRC)%.cpp
	$(CC) $(INCLUDE) -c $(<) -o $@

all: $(INSTR)

demo: demo.cpp $(OBJECTS)
	$(CC) $(INCLUDE) -o demo demo.cpp $(OBJECTS) $(LIBRARY)

Md2Skini: Md2Skini.cpp Stk.o RtMidi.o Thread.o Socket.o
	$(CC) -o Md2Skini Md2Skini.cpp Stk.o RtMidi.o Thread.o Socket.o $(LIBRARY) $(INCLUDE)

clean : 
	rm *.o
	rm $(INSTR)

cleanIns : 
	rm $(INSTR)

strip : 
	strip $(INSTR)

# Project specific objects:

utilities.o: utilities.cpp
	$(CC) $(INCLUDE) -c utilities.cpp
