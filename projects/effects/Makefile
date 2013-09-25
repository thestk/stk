# Effects Makefile

OS = $(shell uname)

# The following definition indicates the relative location of
# the core STK classes.
STK_PATH = ../../src/

O_FILES		=	Stk.o Reverb.o PRCRev.o JCRev.o \
						NRev.o RtAudio.o Delay.o Filter.o \
						RtDuplex.o SKINI.o Envelope.o Echo.o \
						PitShift.o DelayL.o Chorus.o WvIn.o \
						WaveLoop.o Messager.o Thread.o Socket.o \
						RtMidi.o


RM = /bin/rm 

ifeq ($(OS),IRIX) # These are for SGI
	INSTR = effects
	CC = CC -O2 -D__IRIX_AL__ # -g -fullwarn -D__SGI_CC__
	LIBRARY = -L/usr/sgitcl/lib -laudio -lmd -lm -lpthread
	INCLUDE = -I../../include
endif

ifeq ($(OS),Linux) # These are for Linux
	INSTR = effects
	CC = g++ -O3 -Wall -D__LINUX_OSS__ -D__LITTLE_ENDIAN__ # -g
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
