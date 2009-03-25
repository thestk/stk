/**************  Test Main Program Individual Voice *********************/

#include "miditabl.h"
#include "RawWvOut.h"
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
#include "Moog1.h"
#include "VoicForm.h"
#include "Reverb.h"

#include <sys/types.h>
#include <sys/prctl.h>
#define _BSD_SIGNALS
#include <signal.h>

#define MAX_BUF 16

int gotOne = 0;
char inputString[128];
int test_pid;
 
void newString(void *)
{
    while (1)	{
	fgets(inputString,128,stdin);
	gotOne = 1;
    }
}

int main(int argc,char *argv[])
{
    long i, j;
    MY_FLOAT temp,temp2;
    Instrmnt *instrument;    
    RawWvOut output("test.snd");
    Reverb reverb(2137);
    int outAHere;
    double deltaTime;
    int channel;
    int point;
    char tempString[32];
    float byteTwo,byteThree,lastPitch=100.0;

    if (argc<2) 	{
	printf("useage: testTCL Instr\n");
	printf("       where Instr = Marimba Vibraphn AgogoBel Plucked\n");
	printf("                     Mandolin Clarinet Flute Brass Bowed \n");
	printf("                     Rhodey Wurley TubeBell HeavyMtl\n");
	printf("		     PercFlut BeeThree FMVoices VoicForm Moog1\n");
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
	printf("HeavyMtl PercFlut BeeThree FMVoices VoicForm Moog1\n");
	exit(0);
    }

    reverb.setEffectMix(0.2);
    instrument->noteOn(440.0, 0.95);

    test_pid = sproc(newString, PR_SALL);

    if (test_pid == -1)
    {
       fprintf(stderr, "unable to create input thread...aborting.\n");
       exit(-1);
    }

    outAHere = 0;

    while (!outAHere)	{
	while (gotOne == 0)	{
  	    output.tick(reverb.tick(instrument->tick()));
	}
	if (gotOne > 0)	{
            sscanf(inputString,"%s %lf %i %f %f",tempString,&deltaTime,&channel,&byteTwo,&byteThree);
	    gotOne = 0;
	    point = 5;
	    if (tempString[0]=='%') point = 1;
	    if (tempString[point] == 'n')	{	// NoteO'n'
		j = (int) byteTwo;
		byteTwo = __MIDI_To_Pitch[j];
	        lastPitch = byteTwo;
		instrument->noteOn(byteTwo,byteThree*NORM_7);
            }
	    else if (tempString[point] == 'f')   {  // NoteO'f'f
	        j = (int) byteTwo;
		instrument->noteOff(byteThree*NORM_7);
	    }	    
	    else if (tempString[point] == 'B')   {  // Pitch'B'end
	        byteThree = byteTwo;
		j = (int) byteThree;
	    	byteThree -= j;
		lastPitch = __MIDI_To_Pitch[j] * pow(2.0,byteThree / 12.0) ;
		instrument->setFreq(lastPitch);
	    }	    
	    else if (tempString[point] == 'a')   {  // Progr'a'mChange
	        instrument->noteOff(1.0);
		for (i=0;i<4096;i++)	{
	     	    output.tick(reverb.tick(instrument->tick()));
		}
		delete instrument;
		if      (byteTwo==0) instrument = new Clarinet(50.0);
		else if (byteTwo==1) instrument = new Flute(50.0);
		else if (byteTwo==2) instrument = new Brass(50.0);
		else if (byteTwo==3) instrument = new Bowed(50.0);

		else if (byteTwo==4) instrument = new Mandolin(50.0);
		else if (byteTwo==5) instrument = new Marimba;
		else if (byteTwo==6) instrument = new Vibraphn;
		else if (byteTwo==7) instrument = new AgogoBel;
		else if (byteTwo==8) instrument = new Rhodey;
		else if (byteTwo==9) instrument = new Wurley;
		else if (byteTwo==10) instrument = new TubeBell;

		else if (byteTwo==11) instrument = new FMVoices;
		else if (byteTwo==12) instrument = new VoicForm;

		else if (byteTwo==13) instrument = new HeavyMtl;
		else if (byteTwo==14) instrument = new PercFlut;
		else if (byteTwo==15) instrument = new Moog1;

    		instrument->noteOn(lastPitch, 0.95);
	    }	    
	    else if (tempString[point] == 'T')   {  // After'T'ouch
		instrument->controlChange(128,byteTwo);
	    }	    
	    else if (tempString[point] == 'o')   {  // Contr'o'lChange
	        j = (long) byteTwo;
		instrument->controlChange(j,byteThree);
	    }
	    else if (tempString[0] == 'E' && tempString[1] == 'x')	{
	        gotOne = 0;
		outAHere = 1;
	    }

//	    else if (tempString[0] == '/')	{
//		notDone = 1;
//	    }
	    
	        gotOne = 0;

	}
		
    }

   kill(test_pid, SIGKILL);
    delete instrument;

}

    
