# STK98 Makefile - Global version for Unix systems which have GNU
# Makefile utilities installed.  If this Makefile does not work on
# your system, try using the platform specific Makefiles (.sgi,
# .next, and .linux).

OS = $(shell uname)

O_FILES		=	Object.o Envelope.o ADSR.o Noise.o SubNoise.o \
				RawWave.o RawShot.o RawLoop.o RawInterp.o \
				Modulatr.o Filter.o OneZero.o  \
				OnePole.o TwoZero.o TwoPole.o DCBlock.o \
				BiQuad.o DLineA.o DLineL.o DLineN.o VoicMang.o \
				FormSwep.o BowTabl.o JetTabl.o ReedTabl.o \
				LipFilt.o Modal4.o FM4Op.o FM4Alg3.o FM4Alg4.o \
				FM4Alg5.o FM4Alg6.o FM4Alg8.o Plucked2.o \
				SamplFlt.o Sampler.o SKINI11.o Simple.o \
				SingWave.o VoicForm.o FMVoices.o swapstuf.o \
				\
				Instrmnt.o Marimba.o Vibraphn.o AgogoBel.o Shakers.o \
				Plucked.o Mandolin.o Clarinet.o Flute.o Moog1.o \
				Brass.o Bowed.o Rhodey.o Wurley.o TubeBell.o \
				HeavyMtl.o PercFlut.o BeeThree.o DrumSynt.o \
				\
				WvOut.o SndWvOut.o WavWvOut.o MatWvOut.o \
				Reverb.o PRCRev.o JCRev.o NRev.o

RM = /bin/rm 

ifeq ($(OS),NEXTSTEP) # These are for NeXT
	CC = cc -arch m68k -arch i386 -Wall
	INSTR = syntmono
endif

ifeq ($(OS),IRIX) # These are for SGI
	INSTR = MD2SKINI syntmono
	CC = CC -O # -g -fullwarn -D__SGI_CC__
	O_FILES += RTWvOut.o RTSoundIO.o MIDIIO.o
	LIBRARY = -L/usr/sgitcl/lib -laudio -lmd -lm
endif

ifeq ($(OS),Linux) # These are for Linux
	INSTR = syntmono MD2SKINI
	CC = gcc -O3 # -g -pg -O3
	O_FILES += RTWvOut.o RTSoundIO.o MIDIIO.o
	LIBRARY = -lpthread -lm
#	LIBRARY = /lib/libpthread.so.0 -lm
endif

.SUFFIXES:	.cpp
.cpp.o: Object.h
		$(CC) -c $*.cpp

all: $(INSTR)

syntmono: syntmono.cpp $(O_FILES)
	$(CC) $(INCLUDE) -o syntmono syntmono.cpp $(O_FILES) $(LIBRARY)

MD2SKINI: MD2SKINI.cpp $(O_FILES)
	$(CC) -o MD2SKINI MD2SKINI.cpp $(O_FILES) $(LIBRARY)

clean : 
	rm *.o
	rm $(INSTR)

cleanIns : 
	rm $(INSTR)

strip : 
	strip $(INSTR)
