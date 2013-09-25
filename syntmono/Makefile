# STK Makefile - Global version for Unix systems which have GNU
# Makefile utilities installed.  If this Makefile does not work on
# your system, try using the platform specific Makefiles (.sgi,
# .next, and .linux).

OS = $(shell uname)

# The following definition indicates the relative location of
# the core STK classes.
STK_PATH = ../STK/

O_FILES		=	Object.o Envelope.o ADSR.o Noise.o SubNoise.o \
				Modulatr.o Filter.o OneZero.o  TablLook.o \
				OnePole.o TwoZero.o TwoPole.o DCBlock.o \
				BiQuad.o DLineA.o DLineL.o DLineN.o VoicMang.o \
				FormSwep.o BowTabl.o JetTabl.o ReedTabl.o \
				LipFilt.o Modal4.o FM4Op.o FM4Alg3.o FM4Alg4.o \
				FM4Alg5.o FM4Alg6.o FM4Alg8.o Plucked2.o \
				SamplFlt.o Sampler.o SKINI11.o Simple.o \
				SingWave.o VoicForm.o FMVoices.o swapstuf.o \
				utilities.o threads.o \
				\
				Instrmnt.o Marimba.o Vibraphn.o AgogoBel.o Shakers.o \
				Plucked.o Mandolin.o Clarinet.o Flute.o Moog1.o \
				Brass.o Bowed.o Rhodey.o Wurley.o TubeBell.o \
				HeavyMtl.o PercFlut.o BeeThree.o DrumSynt.o \
				\
				WvOut.o SndWvOut.o WavWvOut.o MatWvOut.o RawWvOut.o \
				WvIn.o SndWvIn.o WavWvIn.o MatWvIn.o RawWvIn.o \
				Reverb.o PRCRev.o JCRev.o NRev.o BowedBar.o

RM = /bin/rm 

ifeq ($(OS),NEXTSTEP) # These are for NeXT
	CC = cc -arch m68k -arch i386 -Wall -D__OS_NeXT_
	INSTR = syntmono
endif

ifeq ($(OS),IRIX) # These are for SGI
	INSTR = MD2SKINI syntmono
	CC = CC -O2 -D__OS_IRIX_ # -g -fullwarn -D__SGI_CC__
	O_FILES += RTWvOut.o RTSoundIO.o RTWvIn.o MIDIIO.o RTDuplex.o
	LIBRARY = -L/usr/sgitcl/lib -laudio -lmd -lm -lpthread
endif

ifeq ($(OS),Linux) # These are for Linux
	INSTR = syntmono MD2SKINI
	CC = g++ -O3 -Wall -D__OS_Linux_ # -g -O3 -Wall
	O_FILES += RTWvOut.o RTSoundIO.o RTWvIn.o MIDIIO.o RTDuplex.o
	LIBRARY = -lpthread -lm
endif

%.o : $(STK_PATH)%.cpp
	$(CC) -c $(<) -o $@

all: $(INSTR)

syntmono: syntmono.cpp $(O_FILES)
	$(CC) $(INCLUDE) -o syntmono syntmono.cpp $(O_FILES) $(LIBRARY)

MD2SKINI: $(STK_PATH)MD2SKINI.cpp Object.o MIDIIO.o
	$(CC) -o MD2SKINI $(STK_PATH)MD2SKINI.cpp Object.o MIDIIO.o $(LIBRARY)

clean : 
	rm *.o
	rm $(INSTR)

cleanIns : 
	rm $(INSTR)

strip : 
	strip $(INSTR)

# $(O_FILES) :

utilities.o: utilities.cpp
	$(CC) -c utilities.cpp

threads.o: threads.cpp
	$(CC) -c threads.cpp