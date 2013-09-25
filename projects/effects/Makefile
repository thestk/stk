# Effects Makefile

OS = $(shell uname)

# The following definition indicates the relative location of
# the core STK classes.
STK_PATH = ../../src/

O_FILES		=	Object.o Reverb.o PRCRev.o JCRev.o \
						NRev.o RtAudio.o DLineN.o Filter.o \
						RtDuplex.o SKINI11.o Envelope.o Echo.o \
						PitShift.o DLineL.o Chorus.o RawWvIn.o \
						WvIn.o ByteSwap.o StkError.o Controller.o \
						RtMidi.o


RM = /bin/rm 

ifeq ($(OS),IRIX) # These are for SGI
	INSTR = effects
	CC = CC -O2 -D__OS_IRIX_ # -g -fullwarn -D__SGI_CC__
	LIBRARY = -L/usr/sgitcl/lib -laudio -lmd -lm -lpthread
	INCLUDE = -I../../include
endif

ifeq ($(OS),Linux) # These are for Linux
	INSTR = effects
	CC = g++ -O3 -Wall -D__OS_Linux_ # -g
	LIBRARY = -lpthread -lm #-lasound
	INCLUDE = -I../../include
endif

%.o : $(STK_PATH)%.cpp
	$(CC) $(INCLUDE) -c $(<) -o $@

all: $(INSTR)

clean : 
	rm *.o
	rm $(INSTR)

cleanIns : 
	rm $(INSTR)

strip : 
	strip $(INSTR)

effects: effects.cpp $(O_FILES)
	$(CC) $(INCLUDE) -o effects effects.cpp $(O_FILES) $(LIBRARY)

# $(O_FILES) :

Echo.o: Echo.cpp
	$(CC) $(INCLUDE) -c Echo.cpp

PitShift.o: PitShift.cpp
	$(CC) $(INCLUDE) -c PitShift.cpp

Chorus.o: Chorus.cpp
	$(CC) $(INCLUDE) -c Chorus.cpp