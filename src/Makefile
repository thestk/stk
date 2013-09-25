# stklib Makefile - for Linux or SGI with GNU Makefile utilities

LIBRARY = stklib.a
AR = ar -qsc
RM = /bin/rm 
INCLUDE = -I../include

OS = $(shell uname)

O_FILES		=	Object.o Envelope.o ADSR.o Noise.o SubNoise.o \
				Filter.o OneZero.o OnePole.o PoleZero.o DCBlock.o FIR.o \
				TwoZero.o TwoPole.o BiQuad.o DLineA.o DLineL.o DLineN.o \
				BowTabl.o JetTabl.o ReedTabl.o LipFilt.o TablLook.o \
				\
				Instrmnt.o Modal4.o ModalBar.o Shakers.o \
				Plucked.o Plucked2.o Mandolin.o Bowed.o Clarinet.o \
				Flute.o Brass.o BlowHole.o BowedBar.o \
				FM4Op.o FM4Alg3.o FM4Alg4.o FM4Alg5.o FM4Alg6.o FM4Alg8.o \
				Rhodey.o Wurley.o TubeBell.o HeavyMtl.o \
				PercFlut.o BeeThree.o DrumSynt.o Moog1.o \
				Sampler.o SamplFlt.o Simple.o SingWave.o \
				VoicForm.o FMVoices.o FormSwep.o Modulatr.o VoicMang.o \
				\
				WvOut.o SndWvOut.o WavWvOut.o MatWvOut.o RawWvOut.o AifWvOut.o \
				WvIn.o SndWvIn.o WavWvIn.o MatWvIn.o RawWvIn.o AifWvIn.o \
				StrmWvIn.o StrmWvOut.o \
				RtWvOut.o RtAudio.o RtWvIn.o RtMidi.o RtDuplex.o \
				Reverb.o PRCRev.o JCRev.o NRev.o \
				\
				SKINI11.o Controller.o ByteSwap.o StkError.o

ifeq ($(OS),IRIX) # These are for SGI
	CC = CC -D__OS_IRIX_
endif

ifeq ($(OS),Linux) # These are for Linux
	CC = g++ -O3 -Wall -D__OS_Linux_
endif

%.o : %.cpp
	$(CC) $(INCLUDE) -c $(<) -o $@

debug all: $(LIBRARY)

$(LIBRARY): $(O_FILES)
	/bin/rm -f $(LIBRARY)
	$(AR) $(LIBRARY) $(O_FILES)
	ranlib -t $(LIBRARY)

clean : 
	rm *.o
	rm $(LIBRARY)
