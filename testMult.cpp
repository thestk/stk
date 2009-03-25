/**************  Test Main Program Individual Voice *********************/

#include "miditabl.h"
#include "RawWvOut.h"
#include "MIDIText.h"
#include "Reverb.h"
#include "VoicMang.h"

int main(int argc,char *argv[])
{
    long i, j;
    MY_FLOAT temp,temp2;
    VoicMang *section;    
    RawWvOut output("test.snd");
    Reverb reverb(2137);
    float gain;
    MIDIText *score;

    if (argc<3) 	{
	printf("useage: testMulti <gain> Instr file.mtx\n");
	printf("       where Instr = Marimba Vibraphn AgogoBel Plucked\n");
	printf("                     Mandolin Clarinet Flute Brass Bowed \n");
	printf("                     Rhodey Wurley TubeBell HeavyMtl\n");
	printf("		     PercFlut BeeThree Moog1\n");
	exit(0);
    }
    else if (argc==4)	{
        gain = atof(argv[1]);
	section = new VoicMang(8,argv[2]);
        score = new MIDIText(argv[3]);
    }
    else if (argc==3)	{
        gain = 0.125;
        section = new VoicMang(8,argv[1]);
        score = new MIDIText(argv[2]);
    }
    if (!section) {
        printf("section type must be 1 of:\n");
	printf("Marimba Vibraphn AgogoBel Plucked Mandolin\n");
	printf("Clarinet Flute Brass Bowed Rhodey Wurley\n");
	printf("TubeBell HeavyMtl PercFlut BeeThree Moog1\n");
	exit(0);
    }

    reverb.setEffectMix(0.2);
    while(score->getType() > 0)	{
        j = long (score->getDelta() * SRATE);
//	printf("%i\n",j);
	if (j > 0)	{
	    for (i=0;i<j;i++)	{
		output.tick(reverb.tick(gain * section->tick()));
	    }
	}
 	temp = score->getByteTwo();
 	temp2 = score->getByteThree();
	if (score->getType()==9)	{
	    if (score->getByteThree() == 0) {
		section->noteOffN((int) temp,temp2);
	    }
	    else {
		j = section->noteOn(temp,temp2);
		if (j<0) {
		    printf("Out of Voices Here\n");
		    section->kill(section->oldestVoice());
		    section->noteOn(temp,temp2);
		}
 	    }
	}
        else if (score->getType() == 8)	{
	    section->noteOffN((int) temp,temp2);
	}
	else if (score->getType() == 11)	{
	    j = (int) score->getByteTwo();
	    section->controlChange(j,temp2);
	}
	score->nextMessage();
    }
    for (i=0;i<22000;i++)	{	/* let the reverb settle a little */
	output.tick(reverb.tick(0.125 * section->tick()));
    }

    delete score;
    delete section;

}

    
