# STK Makefile - Global version for Unix systems which have GNU
# Makefile utilities installed.  If this Makefile does not work on
# your system, try using the platform specific Makefiles (.sgi,
# .next, and .linux).

OS = $(shell uname)

# The following definition indicates the relative location of
# the core STK classes.
STK_PATH = ../../src/

O_FILES		=	Stk.o Envelope.o ADSR.o Noise.o \
						Filter.o DelayA.o Delay.o \
						OnePole.o OneZero.o SKINI.o \
						Tabla.o Instrmnt.o Sitar.o \
						Drone.o VoicDrum.o WvOut.o WvIn.o \
						RtAudio.o RtWvOut.o RtMidi.o Reverb.o \
						JCRev.o Messager.o Socket.o Thread.o

RM = /bin/rm 

ifeq ($(OS),IRIX) # These are for SGI
	INSTR = ragamat
	CC = CC -O2 -D__IRIX_AL__ # -g -fullwarn -D__SGI_CC__
	LIBRARY = -L/usr/sgitcl/lib -laudio -lmd -lm -lpthread
	INCLUDE = -I../../include
endif

ifeq ($(OS),Linux) # These are for Linux
	INSTR = ragamat
	CC = g++ -O3 -Wall -D__LINUX_OSS__ -D__LITTLE_ENDIAN__ # -g
	LIBRARY = -lpthread -lm #-lasound
	INCLUDE = -I../../include
endif

%.o : $(STK_PATH)%.cpp
	$(CC) $(INCLUDE) -c $(<) -o $@

all: $(INSTR)

ragamat: ragamat.cpp $(O_FILES)
	$(CC) $(INCLUDE) -o ragamat ragamat.cpp $(O_FILES) $(LIBRARY)

clean : 
	rm *.o
	rm $(INSTR)

cleanIns : 
	rm $(INSTR)

strip : 
	strip $(INSTR)

# $(O_FILES) :

Tabla.o: Tabla.cpp
	$(CC) $(INCLUDE) -c Tabla.cpp

Drone.o: Drone.cpp
	$(CC) $(INCLUDE) -c Drone.cpp

VoicDrum.o: VoicDrum.cpp
	$(CC) $(INCLUDE) -c VoicDrum.cpp