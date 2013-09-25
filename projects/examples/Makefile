# Examples Makefile - Global version for Unix systems which have GNU
# Makefile utilities installed.

OS = $(shell uname)

# You might have to modify the following path if you move things
# around on your system.
STK_SRC = ../../src/
INCLUDE = -I../../include
RM = /bin/rm 

INSTR = sine play record io tcpIn tcpOut Moogy

ifeq ($(OS),Linux) # These are for Linux
	CC = g++ -O3 -Wall -D__LITTLE_ENDIAN__ -D__LINUX_OSS__
	LIBRARY = -lpthread -lm #-lasound
endif

ifeq ($(OS),IRIX) # These are for SGI
	CC = CC -D__IRIX_AL__ # -g -fullwarn -O2
	LIBRARY = -L/usr/sgitcl/lib -laudio -lmd -lm -lpthread
endif

%.o : $(STK_SRC)%.cpp
	$(CC) $(INCLUDE) -c $(<) -o $@

all: $(INSTR)

clean :
	rm *.o
	rm $(INSTR)

cleanIns : 
	rm $(INSTR)

strip : 
	strip $(INSTR)

play: play.cpp Stk.o WvIn.o WvOut.o RtWvOut.o RtAudio.o
	$(CC) -o play play.cpp Stk.o WvIn.o WvOut.o RtWvOut.o RtAudio.o $(LIBRARY) $(INCLUDE)

record: record.cpp Stk.o WvIn.o WvOut.o RtWvIn.o RtAudio.o
	$(CC) -o record record.cpp Stk.o WvIn.o WvOut.o RtWvIn.o RtAudio.o $(LIBRARY) $(INCLUDE)

sine: sine.cpp Stk.o WvIn.o WvOut.o WaveLoop.o
	$(CC) -o sine sine.cpp Stk.o WvIn.o WvOut.o WaveLoop.o $(LIBRARY) $(INCLUDE)

io: io.cpp Stk.o RtAudio.o RtDuplex.o
	$(CC) -o io io.cpp Stk.o RtAudio.o RtDuplex.o $(LIBRARY) $(INCLUDE)

tcpIn: tcpIn.cpp Stk.o WvIn.o TcpWvIn.o WvOut.o RtWvOut.o RtAudio.o Socket.o Thread.o
	$(CC) -o tcpIn tcpIn.cpp Stk.o WvIn.o Socket.o Thread.o TcpWvIn.o WvOut.o RtWvOut.o RtAudio.o $(LIBRARY) $(INCLUDE)

tcpOut: tcpOut.cpp Stk.o WvIn.o WvOut.o TcpWvOut.o Socket.o Thread.o
	$(CC) -o tcpOut tcpOut.cpp Stk.o WvIn.o WvOut.o Socket.o Thread.o TcpWvOut.o $(LIBRARY) $(INCLUDE)

Moogy: Moogy.cpp Stk.o WvIn.o WaveLoop.o WvOut.o RtWvOut.o RtAudio.o Instrmnt.o Sampler.o FormSwep.o Filter.o BiQuad.o Envelope.o ADSR.o OnePole.o Moog.o 
	$(CC) -o Moogy Stk.o WvIn.o WaveLoop.o WvOut.o RtWvOut.o RtAudio.o Instrmnt.o Sampler.o FormSwep.o Filter.o BiQuad.o Envelope.o ADSR.o OnePole.o Moog.o Moogy.cpp $(LIBRARY) $(INCLUDE)