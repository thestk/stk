/**************  Test Main Program Individual Voice *********************/

#include "../STK/WvOut.h"
#include "../STK/RTWvOut.h"
#include "../STK/SKINI11.h"
#include "../STK/SKINI11.msg"
#include "../STK/Instrmnt.h"
#include "../STK/Reverb.h"
#include "../STK/NRev.h"
#include "../STK/PRCRev.h"
#include "../STK/JCRev.h"
#include "StrDrone.h"
#include "Sitar1.h"
#include "Tabla.h"
#include "VoicDrum.h"
#include "miditabl.h"

#define RATE_NORM (MY_FLOAT)  (22050.0/SRATE)

int numStrings = 0;
int notDone = 1;
char **inputString;

// The input command pipe and socket threads are defined in threads.cpp.
#include "threads.h"

// Return random float between 0.0 and max
MY_FLOAT float_random(MY_FLOAT max)	{
  MY_FLOAT temp;
#if defined(__OS_Win_) /* For Windoze */
  temp = (MY_FLOAT) (rand() * max);
  temp *= ONE_OVER_RANDLIMIT;
#else /* This is for unix */
  temp = (MY_FLOAT) (random() * max);
  temp *= ONE_OVER_RANDLIMIT;
#endif
  return temp;	
}

void usage(void) {
  /* Error function in case of incorrect command-line argument specifications */
  printf("\nuseage: ragamat flag \n");
  printf("    where flag = -ip for realtime SKINI input by pipe\n");
  printf("                 (won't work under Win95/98),\n");
  printf("    and flag = -is for realtime SKINI input by socket.\n");
  exit(0);
}

int main(int argc,char *argv[])
{
  long i, j, synlength;
  int type, outOne = 0, useSocket = 0;
  MY_FLOAT reverbTime = 5.0;  /* in seconds */
  MY_FLOAT byte3, outSample, temp;
  MY_FLOAT drone_prob = 0.01, note_prob = 0.0;
  MY_FLOAT drum_prob = 0.0, voic_prob = 0.0;
  MY_FLOAT droneFreqs[3] = {55.0,82.5,220.0};
  int tempo = 3000;
  int counter = 3000;
  int key = 0;
  int ragaStep, ragaPoint = 6, voicNote;
  int ragaUp[2][13] = {{57, 60, 62, 64, 65, 68, 69, 71, 72, 76, 77, 81},
                       {52, 54, 55, 57, 59, 60, 63, 64, 66, 67, 71, 72}};
  int ragaDown[2][13] = {{57, 60, 62, 64, 65, 67, 69, 71, 72, 76, 79, 81},
                         {48, 52, 53, 55, 57, 59, 60, 64, 66, 68, 70, 72}};
  WvOut *output;
  Instrmnt *drones[3];
  Instrmnt *sitar;
  Instrmnt *voicDrums;
  Instrmnt *drums;
  Reverb *reverbs[2];
  SKINI11 *score;

  if (argc != 2) usage();

  if (!strcmp(argv[1],"-is") )
    useSocket = 1;
  else if (strcmp(argv[1],"-ip")) {
    usage();
  }

  output = new RTWvOut(SRATE,1);       

  drones[0] = new StrDrone(50.0);
  drones[1] = new StrDrone(50.0);
  drones[2] = new StrDrone(50.0);
  sitar = new Sitar1(50.0);
  voicDrums = new VoicDrum();
  drums = new Tabla();

  score = new SKINI11();
  reverbs[0] = new JCRev(reverbTime);
  reverbs[0]->setEffectMix(0.5);
  reverbs[1] = new JCRev(2.0);
  reverbs[1]->setEffectMix(0.2);

  // Start the input thread
  if (useSocket)
    startSocketThread();
  else
    startPipeThread();

  drones[0]->noteOn(droneFreqs[0],0.1);
  drones[1]->noteOn(droneFreqs[1],0.1);
  drones[2]->noteOn(droneFreqs[2],0.1);

  for (i=0;i<SRATE;i++) { /* warm everybody up a little */
    outSample = 0;
    for (j=0;j<3;j++)
	    outSample += drones[j]->tick();
    output->tick(reverbs[0]->tick(outSample));
  }

  /* Finally ... the runtime loop begins! */
  notDone = 1;
  synlength = RT_BUFFER_SIZE;
  while(notDone || numStrings)  {
    if (numStrings > 1) synlength = (long) RT_BUFFER_SIZE / numStrings;
    else synlength = RT_BUFFER_SIZE;
    for ( i=0; i<synlength; i++ )  {
      outSample = 0;
      for (j=0;j<3;j++)
        outSample += drones[j]->tick();
      outSample -= sitar->tick();
      output->tick(reverbs[0]->tick(outSample)
                   + reverbs[1]->tick(voicDrums->tick() + drums->tick()));
      counter -= 1;
      if (counter == 0)	{
        counter = (int) (tempo / RATE_NORM);
        if (float_random(1.0) < drone_prob)
          drones[0]->noteOn(droneFreqs[0],0.1);
        if (float_random(1.0) < drone_prob)
          drones[1]->noteOn(droneFreqs[1],0.1);
        if (float_random(1.0) < drone_prob)
          drones[2]->noteOn(droneFreqs[2],0.1);
        if (float_random(1.0) < note_prob)	{
          if ((temp = float_random(1.0)) < 0.1)
            ragaStep = 0;
          else if (temp < 0.5)
            ragaStep = 1;
          else
            ragaStep = -1;
          ragaPoint += ragaStep;
          if (ragaPoint < 0) 
            ragaPoint -= (2*ragaStep);
          if (ragaPoint > 11) 
            ragaPoint = 11;
          if (ragaStep > 0)
            sitar->noteOn(__MIDI_To_Pitch[ragaUp[key][ragaPoint]]
                          ,0.05 + float_random(0.3));
          else
            sitar->noteOn(__MIDI_To_Pitch[ragaDown[key][ragaPoint]]
                          ,0.05 + float_random(0.3));
        }
        if (float_random(1.0) < voic_prob)	{
          voicNote = (int) float_random(11);
          voicDrums->noteOn(voicNote,0.3 + (0.4 * drum_prob) +
                            float_random(0.9 * voic_prob));
        }
        if (float_random(1.0) < drum_prob)	{
          voicNote = (int) float_random(TABLA_NUMWAVES);
          drums->noteOn(voicNote,0.2 + (0.2 * drum_prob) + 
                        float_random(0.7 * drum_prob));
        }
      }
    }
    if (numStrings) {
      score->parseThis(inputString[outOne]);
      type = score->getType();
      if (type > 0) {
        if (type == __SK_ControlChange_)   {
          j = (int) score->getByteTwo();
          byte3 = score->getByteThree();
          if (j == 1)	{
            drone_prob = byte3 * NORM_7;
          }
          else if (j == 2)	{
            note_prob = byte3 * NORM_7;
          }
          else if (j == 4)	{
            voic_prob = byte3 * NORM_7;
          }
          else if (j == 11)	{
            drum_prob = byte3 * NORM_7;
          }
          else if (j == 7)	{
            tempo = (int) (11025 - (byte3 * 70));
          }
          else if (j == 64)	{
            if (byte3 == 0) {
              key = 1;
              droneFreqs[0] = 55.0;
              droneFreqs[1] = 82.5;
              droneFreqs[2] = 220.0;
            }
            else 	{
              key = 0;
              droneFreqs[0] = 82.5;
              droneFreqs[1] = 123.5;
              droneFreqs[2] = 330.0;
            }
          }
        }
      }
      outOne += 1;
      if (outOne == MAX_IN_STRINGS) outOne = 0;
      numStrings--;
    }
  }

  printf("What Need Have I for This?\n");
  drones[1]->noteOn(droneFreqs[1],0.1);
  for (i=0;i<reverbTime*SRATE;i++) { // Calm down a little
    outSample = 0;
    for (j=0;j<3;j++)
	    outSample += drones[j]->tick();
    output->tick(reverbs[0]->tick(outSample));
  }
  printf("What Need Have I for This?\n");
  drones[2]->noteOn(droneFreqs[2],0.1);
  for (i=0;i<reverbTime*SRATE;i++) { // and a little more
    outSample = 0;
    for (j=0;j<3;j++)
	    outSample += drones[j]->tick();
    output->tick(reverbs[0]->tick(outSample));
  }
  printf("RagaMatic finished ... \n");
  drones[0]->noteOn(droneFreqs[0],0.1);
  for (i=0;i<reverbTime*SRATE;i++) { // almost ready to think about ending
    outSample = 0;
    for (j=0;j<3;j++)
	    outSample += drones[j]->tick();
    output->tick(reverbs[0]->tick(outSample));
  }
  printf("All is Bliss ...\n");
  for (i=0;i<reverbTime*SRATE;i++) { // nearly finished now
    outSample = 0;
    for (j=0;j<3;j++)
	    outSample += drones[j]->tick();
    output->tick(reverbs[0]->tick(outSample));
  }
  printf("All is Bliss ...\n");
  for (i=0;i<reverbTime*SRATE;i++) { // all is bliss....
    outSample = 0;
    for (j=0;j<3;j++)
	    outSample += drones[j]->tick();
    output->tick(reverbs[0]->tick(outSample));
  }

  delete output;
  delete score;
  delete drones[0];
  delete drones[1];
  delete drones[2];
  delete sitar;
  delete drums;
  delete voicDrums;
  delete reverbs[0];
  delete reverbs[1];

  return 0;
}
