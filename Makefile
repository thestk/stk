O_FILES = Object.o Envelope.o ADSR.o Noise.o SubNoise.o RawWave.o RawLoop.o \
	NIWave1S.o Modulatr.o SingWave.o RawWvOut.o RawWvIn.o Filter.o \
	OneZero.o OnePole.o TwoZero.o TwoPole.o DCBlock.o BiQuad.o AllPass1.o \
	DLineA.o DLineL.o DLineN.o FormSwep.o BowTabl.o JetTabl.o ReedTabl.o \
	LipFilt.o Modal4.o FM4Op.o FM4Alg3.o FM4Alg4.o FM4Alg5.o FM4Alg6.o \
	FM4Alg8.o Plucked2.o SamplFlt.o Sampler.o VoicForm.o\
	MIDIText.o Reverb.o VoicMang.o \
	\
	Instrmnt.o Marimba.o Vibraphn.o AgogoBel.o Plucked.o Mandolin.o \
	Clarinet.o Flute.o Brass.o Bowed.o Rhodey.o Wurley.o TubeBell.o \
	HeavyMtl.o PercFlut.o BeeThree.o FMVoices.o Moog1.o 

RM = /bin/rm 

#   This is for NeXT
#   CC = cc -O
#   INSTR = testMono testMult DrumSynt textVoic


#   These are for SGI   
INSTR = testTextIn testMono testMIDI testMult DrumSynt textVoic
   CC = gcc -O
   MEDIALINK = -lmd
   TCLLIB = /usr/local/lib/libtcl.so.7.4 /usr/local/lib/libtk.so.4.0 /usr/lib/libX11.so
   LIBRARY = /usr/lib/libmx.so /usr/lib/libaudio.a

# .cpp.o:
#	$(CC)  -c $*.cpp

all: $(INSTR)

testTextIn: $(LIBRARY) testTextIn.cpp $(O_FILES) MIDIInpt.o 
	$(CC) $(MEDIALINK) -o testTextIn testTextIn.cpp $(O_FILES) $(LIBRARY)

testMono: $(LIBRARY) testMono.cpp $(O_FILES)
	$(CC) $(MEDIALINK) -o testMono testMono.cpp $(O_FILES) $(LIBRARY)

testMIDI: $(LIBRARY) testMIDI.cpp Object.o MIDIInpt.o
	$(CC) $(MEDIALINK) -o testMIDI testMIDI.cpp Object.o MIDIInpt.o $(LIBRARY) /usr/lib/libmidi.so

testMult: $(LIBRARY) testMult.cpp $(O_FILES)
	$(CC) $(MEDIALINK) -o testMult testMult.cpp $(O_FILES) $(LIBRARY)

textVoic: $(LIBRARY) textVoic.cpp $(O_FILES)
	$(CC) $(MEDIALINK) -o textVoic textVoic.cpp $(O_FILES) $(LIBRARY)

Instrmnt.o: Instrmnt.cpp
	$(CC) -c Instrmnt.cpp

Marimba.o: Marimba.cpp
	$(CC) -c Marimba.cpp

Vibraphn.o: Vibraphn.cpp
	$(CC) -c Vibraphn.cpp

AgogoBel.o: AgogoBel.cpp
	$(CC) -c AgogoBel.cpp

Plucked.o: Plucked.cpp
	$(CC) -c Plucked.cpp

Mandolin.o: Mandolin.cpp
	$(CC) -c Mandolin.cpp

Clarinet.o: Clarinet.cpp
	$(CC) -c Clarinet.cpp

Flute.o: Flute.cpp
	$(CC) -c Flute.cpp

Brass.o: Brass.cpp
	$(CC) -c Brass.cpp

Bowed.o: Bowed.cpp
	$(CC) -c Bowed.cpp

Rhodey.o: Rhodey.cpp
	$(CC) -c Rhodey.cpp

Wurley.o: Wurley.cpp
	$(CC) -c Wurley.cpp

TubeBell.o: TubeBell.cpp
	$(CC) -c TubeBell.cpp

HeavyMtl.o: HeavyMtl.cpp
	$(CC) -c HeavyMtl.cpp

PercFlut.o: PercFlut.cpp
	$(CC) -c PercFlut.cpp

BeeThree.o: BeeThree.cpp
	$(CC) -c BeeThree.cpp

FMVoices.o: FMVoices.cpp
	$(CC) -c FMVoices.cpp

Moog1.o: Moog1.cpp
	$(CC) -c Moog1.cpp

DrumSynt: $(LIBRARY) DrumSynt.cpp $(O_FILES)
	$(CC) -o DrumSynt DrumSynt.cpp $(O_FILES) $(LIBRARY)

testVoic: $(LIBRARY) testVoic.cpp $(O_FILES)
	$(CC) -o testVoic testVoic.cpp $(O_FILES) $(LIBRARY)

# $(O_FILES) :
#	$(CC) -c -o $@ $*.cpp

Object.o : Object.cpp
	$(CC) -c Object.cpp

Envelope.o : Envelope.cpp
	$(CC) -c Envelope.cpp

ADSR.o : ADSR.cpp
	$(CC) -c ADSR.cpp

Noise.o : Noise.cpp
	$(CC) -c Noise.cpp

SubNoise.o : SubNoise.cpp
	$(CC) -c SubNoise.cpp

RawWave.o : RawWave.cpp
	$(CC) -c RawWave.cpp

RawLoop.o : RawLoop.cpp
	$(CC) -c RawLoop.cpp

NIWave1S.o : NIWave1S.cpp
	$(CC) -c NIWave1S.cpp

Modulatr.o : Modulatr.cpp
	$(CC) -c Modulatr.cpp

SingWave.o : SingWave.cpp
	$(CC) -c SingWave.cpp

RawWvOut.o : RawWvOut.cpp
	$(CC) -c RawWvOut.cpp

RawWvIn.o : RawWvIn.cpp
	$(CC) -c RawWvIn.cpp

Filter.o : Filter.cpp
	$(CC) -c Filter.cpp

OneZero.o : OneZero.cpp
	$(CC) -c OneZero.cpp

OnePole.o : OnePole.cpp
	$(CC) -c OnePole.cpp

TwoZero.o : TwoZero.cpp
	$(CC) -c TwoZero.cpp

TwoPole.o : TwoPole.cpp
	$(CC) -c TwoPole.cpp

DCBlock.o : DCBlock.cpp
	$(CC) -c DCBlock.cpp

BiQuad.o : BiQuad.cpp
	$(CC) -c BiQuad.cpp

AllPass1.o : AllPass1.cpp
	$(CC) -c AllPass1.cpp

DLineA.o : DLineA.cpp
	$(CC) -c DLineA.cpp

DLineL.o : DLineL.cpp
	$(CC) -c DLineL.cpp

DLineN.o : DLineN.cpp
	$(CC) -c DLineN.cpp

FormSwep.o : FormSwep.cpp
	$(CC) -c FormSwep.cpp

BowTabl.o : BowTabl.cpp
	$(CC) -c BowTabl.cpp

JetTabl.o : JetTabl.cpp
	$(CC) -c JetTabl.cpp

ReedTabl.o : ReedTabl.cpp
	$(CC) -c ReedTabl.cpp

LipFilt.o : LipFilt.cpp
	$(CC) -c LipFilt.cpp

Modal4.o : Modal4.cpp
	$(CC) -c Modal4.cpp

FM4Op.o : FM4Op.cpp
	$(CC) -c FM4Op.cpp

FM4Alg3.o : FM4Alg3.cpp
	$(CC) -c FM4Alg3.cpp

FM4Alg4.o : FM4Alg4.cpp
	$(CC) -c FM4Alg4.cpp

FM4Alg5.o : FM4Alg5.cpp
	$(CC) -c FM4Alg5.cpp

FM4Alg6.o : FM4Alg6.cpp
	$(CC) -c FM4Alg6.cpp

FM4Alg8.o : FM4Alg8.cpp
	$(CC) -c FM4Alg8.cpp

Plucked2.o : Plucked2.cpp
	$(CC) -c Plucked2.cpp

SamplFlt.o : SamplFlt.cpp
	$(CC) -c SamplFlt.cpp

Sampler.o : Sampler.cpp
	$(CC) -c Sampler.cpp

VoicForm.o : VoicForm.cpp
	$(CC) -c VoicForm.cpp

MIDIText.o : MIDIText.cpp
	$(CC) -c MIDIText.cpp

MIDIInpt.o : MIDIInpt.cpp
	$(CC) -c MIDIInpt.cpp

VoicMang.o : VoicMang.cpp
	$(CC) -c VoicMang.cpp

Reverb.o : Reverb.cpp
	$(CC) -c Reverb.cpp

clean : 
	rm $(INSTR)
	rm *.o

cleanIns : 
	rm $(INSTR)

strip : 
	strip $(INSTR)
