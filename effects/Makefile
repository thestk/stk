# Effects Makefile

OS = $(shell uname)

# The following definition indicates the relative location of
# the core STK classes.
STK_PATH = ../STK/

O_FILES		=	Object.o Reverb.o PRCRev.o JCRev.o \
						NRev.o RTSoundIO.o DLineN.o Filter.o \
						RTDuplex.o SKINI11.o Envelope.o Echo.o \
						PitShift.o DLineL.o Chorus.o RawWvIn.o \
						WvIn.o swapstuf.o threads.o


RM = /bin/rm 

ifeq ($(OS),IRIX) # These are for SGI
	INSTR = effects
	CC = CC -O2 -D__OS_IRIX_ # -g -fullwarn -D__SGI_CC__
	LIBRARY = -L/usr/sgitcl/lib -laudio -lmd -lm
endif

ifeq ($(OS),Linux) # These are for Linux
	INSTR = effects
	CC = g++ -O3 -D__OS_Linux_ # -g
	LIBRARY = -lpthread -lm
endif

%.o : $(STK_PATH)%.cpp
	$(CC) -c $(<) -o $@

all: $(INSTR)

clean : 
	rm *.o
	rm $(INSTR)

cleanIns : 
	rm $(INSTR)

strip : 
	strip $(INSTR)

effects: effects.cpp $(O_FILES)
	$(CC) -O3 -o effects effects.cpp $(O_FILES) $(LIBRARY)

# $(O_FILES) :

threads.o: threads.cpp
	$(CC) -c threads.cpp

Echo.o: Echo.cpp
	$(CC) -c Echo.cpp

PitShift.o: PitShift.cpp
	$(CC) -c PitShift.cpp

Chorus.o: Chorus.cpp
	$(CC) -c Chorus.cpp