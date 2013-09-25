# libstk Makefile - for Linux or SGI with GNU Makefile utilities

LIBRARY = libstk.a
AR = ar -qsc
RM = /bin/rm
INCLUDE = -I../include

OS = $(shell uname)

O_FILES =	Stk.o Noise.o SubNoise.o Envelope.o ADSR.o \
					Filter.o OneZero.o OnePole.o PoleZero.o \
					TwoZero.o TwoPole.o BiQuad.o FormSwep.o \
					Delay.o DelayL.o DelayA.o \
					Reverb.o PRCRev.o JCRev.o NRev.o \
					Table.o JetTabl.o ReedTabl.o BowTabl.o Modulate.o \
					WvIn.o WaveLoop.o WvOut.o \
					Chorus.o Echo.o PitShift.o \
					RtAudio.o RtWvOut.o RtWvIn.o RtDuplex.o \
					Socket.o Thread.o TcpWvOut.o TcpWvIn.o \
					\
					Instrmnt.o Clarinet.o BlowHole.o Saxofony.o Flute.o Brass.o BlowBotl.o \
					Bowed.o Plucked.o StifKarp.o Sitar.o PluckTwo.o Mandolin.o \
					FM.o Rhodey.o Wurley.o TubeBell.o HevyMetl.o PercFlut.o BeeThree.o FMVoices.o \
					Sampler.o Moog.o Simple.o Drummer.o Shakers.o Modal.o ModalBar.o BandedWG.o \
					Mesh2D.o Resonate.o SKINI.o Messager.o RtMidi.o

ifeq ($(OS),IRIX) # These are for SGI
	CC = CC -D__OS_IRIX__
endif

ifeq ($(OS),Linux) # These are for Linux
ifeq ($(strip $(SOUNDDRIVER)),)
SOUNDDRIVER = __LINUX_OSS__
endif
ifeq ($(strip $(RAWWAVES)),)
RAWWAVES = "../../"
endif
	CC = g++ -O3 -Wall -D__LITTLE_ENDIAN__ -D$(SOUNDDRIVER) -DRAWWAVE_PATH=$(RAWWAVES)
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
