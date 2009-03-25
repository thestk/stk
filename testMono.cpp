/**************  Test Main Program Individual Voice *********************/

#include "miditabl.h"
#include "RawWvOut.h"
#include "MIDIText.h"
#include "Instrmnt.h"
#include "Marimba.h"
#include "Vibraphn.h"
#include "AgogoBel.h"
#include "Plucked.h"
#include "Mandolin.h"
#include "Clarinet.h"
#include "Flute.h"
#include "Brass.h"
#include "Bowed.h"
#include "Rhodey.h"
#include "Wurley.h"
#include "TubeBell.h"
#include "HeavyMtl.h"
#include "PercFlut.h"
#include "BeeThree.h"
#include "FMVoices.h"
#include "VoicForm.h"
#include "Moog1.h"
#include "Reverb.h"

int main(int argc,char *argv[])
{
    long i, j;
    MY_FLOAT temp,temp2;
    Instrmnt *instrument;    
    RawWvOut output("test.snd");
    Reverb reverb(2137);
    MIDIText *score;

    if (argc<3) 	{
	printf("useage: testMono Instr file.mtx\n");
	printf("       where Instr = Marimba Vibraphn AgogoBel Plucked\n");
	printf("                     Mandolin Clarinet Flute Brass Bowed \n");
	printf("                     Rhodey Wurley TubeBell HeavyMtl\n");
	printf("		     PercFlut BeeThree FMVoices Moog1 VoicForm\n");
	exit(0);
    }
    if      (!strcmp(argv[1],"Marimba"))  instrument = new Marimba;
    else if (!strcmp(argv[1],"Vibraphn")) instrument = new Vibraphn;
    else if (!strcmp(argv[1],"AgogoBel")) instrument = new AgogoBel;
    else if (!strcmp(argv[1],"Plucked"))  instrument = new Plucked(50.0);
    else if (!strcmp(argv[1],"Mandolin")) instrument = new Mandolin(50.0);
    else if (!strcmp(argv[1],"Clarinet")) instrument = new Clarinet(50.0);
    else if (!strcmp(argv[1],"Flute"))    instrument = new Flute(50.0);
    else if (!strcmp(argv[1],"Brass"))    instrument = new Brass(50.0);
    else if (!strcmp(argv[1],"Bowed"))    instrument = new Bowed(50.0);
    else if (!strcmp(argv[1],"Rhodey"))   instrument = new Rhodey;
    else if (!strcmp(argv[1],"Wurley"))   instrument = new Wurley;
    else if (!strcmp(argv[1],"TubeBell")) instrument = new TubeBell;
    else if (!strcmp(argv[1],"HeavyMtl")) instrument = new HeavyMtl;
    else if (!strcmp(argv[1],"PercFlut")) instrument = new PercFlut;
    else if (!strcmp(argv[1],"BeeThree")) instrument = new BeeThree;
    else if (!strcmp(argv[1],"FMVoices")) instrument = new FMVoices;
    else if (!strcmp(argv[1],"VoicForm")) instrument = new VoicForm;
    else if (!strcmp(argv[1],"Moog1"))    instrument = new Moog1;
    else {
        printf("Instrument type must be 1 of:\n");
	printf("Marimba Vibraphn AgogoBel Plucked Mandolin\n");
	printf("Clarinet Flute Brass Bowed Rhodey Wurley TubeBell\n");
	printf("HeavyMtl PercFlut BeeThree FMVoices Moog1 VoicForm\n");
	exit(0);
    }
    score = new MIDIText(argv[2]);

    reverb.setEffectMix(0.2);
    while(score->getType() > 0)	{
        j = (long) (score->getDelta() * SRATE);
#if defined(_debug_) 
	printf("%i\n",j);
#endif
	if (j > 0)	{
	    for (i=0;i<j;i++)	{
		output.tick(reverb.tick(instrument->tick()));
	    }
	}
 	temp2 = score->getByteThree()*NORM_7;
	if (score->getType()==9)	{
	    if (score->getByteThree() == 0) 
 		instrument->noteOff(temp2);
	    else {
	        j = (int) score->getByteTwo();
	    	temp = __MIDI_To_Pitch[j];
		instrument->noteOn(temp,temp2);
	    }
	}
        else if (score->getType() == 8)	{
	    instrument->noteOff(temp2);
	}
	else if (score->getType() == 11)	{
	    j = (int) score->getByteTwo();
	    instrument->controlChange(j,temp2*128.0);
	}
	score->nextMessage();
    }
    for (i=0;i<22000;i++)	{	/* let the reverb settle a little */
	output.tick(reverb.tick(instrument->tick()));
    }

    delete score;
    delete instrument;

}

    
