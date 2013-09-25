# Misc Makefile - Global version for Unix systems which have GNU
# Makefile utilities installed.  If this Makefile does not work on
# your system, try using the platform specific Makefiles (.sgi,
# .next, and .linux).

OS = $(shell uname)

# You will have to modify this path to correspond to the correct
# location in your system.  The following definition corresponds
# to an STK project directory that is a subdirectory of the core
# STK distribution.
STK_PATH = ../../src/

O_FILES		=	Object.o WvOut.o WvIn.o RtAudio.o \
						RtWvIn.o RtWvOut.o ByteSwap.o \
						StkError.o WavWvOut.o StrmWvIn.o \
						RtDuplex.o StrmWvOut.o WavWvIn.o \
						RawWvIn.o

RM = /bin/rm 

ifeq ($(OS),Linux) # These are for Linux
	INSTR = sineN playN recordN ioN streamInN streamOutN
	CC = g++ -O3 -Wall -D__OS_Linux_ # -g -pg -O3
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

playN: playN.cpp $(O_FILES)
	$(CC) -o playN playN.cpp $(O_FILES) $(LIBRARY) $(INCLUDE)

streamOutN: streamOutN.cpp $(O_FILES)
	$(CC) -o streamOutN streamOutN.cpp $(O_FILES) $(LIBRARY) $(INCLUDE)

streamInN: streamInN.cpp $(O_FILES)
	$(CC) -o streamInN streamInN.cpp $(O_FILES) $(LIBRARY) $(INCLUDE)

recordN: recordN.cpp $(O_FILES)
	$(CC) -o recordN recordN.cpp $(O_FILES) $(LIBRARY) $(INCLUDE)

ioN: ioN.cpp $(O_FILES)
	$(CC) -o ioN ioN.cpp $(O_FILES) $(LIBRARY) $(INCLUDE)

sineN: sineN.cpp $(O_FILES)
	$(CC) -o sineN sineN.cpp $(O_FILES) $(LIBRARY) $(INCLUDE)
