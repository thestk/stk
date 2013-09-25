# STK Makefile - Global version for Unix systems which have GNU
# Makefile utilities installed.  If this Makefile does not work on
# your system, try using the platform specific Makefiles (.sgi,
# .next, and .linux).

OS = $(shell uname)

# The following definition indicates the relative location of
# the core STK classes.
STK_PATH = ../STK/

O_FILES		=	Object.o Envelope.o ADSR.o Noise.o \
				Filter.o DLineA.o DLineL.o DLineN.o \
				OnePole.o OneZero.o DCBlock.o SKINI11.o \
				swapstuf.o Tabla.o Instrmnt.o Sitar1.o \
				StrDrone.o VoicDrum.o WvOut.o WvIn.o RawWvIn.o \
				RTSoundIO.o RTWvOut.o MIDIIO.o Reverb.o \
				NRev.o JCRev.o PRCRev.o threads.o

RM = /bin/rm 

ifeq ($(OS),IRIX) # These are for SGI
	INSTR = ragamat MD2SKINI
	CC = CC -O2 -D__OS_IRIX_ # -g -fullwarn -D__SGI_CC__
	LIBRARY = -L/usr/sgitcl/lib -laudio -lmd -lm -lpthread
endif

ifeq ($(OS),Linux) # These are for Linux
	INSTR = ragamat MD2SKINI
	CC = g++ -O3 -Wall -D__OS_Linux_ # -g
	LIBRARY = -lpthread -lm
endif

%.o : $(STK_PATH)%.cpp
	$(CC) -c $(<) -o $@

all: $(INSTR)

ragamat: ragamat.cpp $(O_FILES)
	$(CC) $(INCLUDE) -o ragamat ragamat.cpp $(O_FILES) $(LIBRARY)

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

threads.o: threads.cpp
	$(CC) -c threads.cpp

Tabla.o: Tabla.cpp
	$(CC) -c Tabla.cpp

Sitar1.o: Sitar1.cpp
	$(CC) -c Sitar1.cpp

StrDrone.o: StrDrone.cpp
	$(CC) -c StrDrone.cpp

VoicDrum.o: VoicDrum.cpp
	$(CC) -c VoicDrum.cpp