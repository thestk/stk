/**************  Test Effects Program  *********************/

#include "../STK/RTSoundIO.h"
#include "../STK/RTDuplex.h"
#include "../STK/SKINI11.h"
#include "../STK/SKINI11.msg"
#include "../STK/Envelope.h"
#include "../STK/PRCRev.h"
#include "../STK/JCRev.h"
#include "../STK/NRev.h"
#include "Echo.h"
#include "PitShift.h"
#include "Chorus.h"

// The input command pipe and socket threads are defined in threads.cpp.
#include "threads.h"

int numStrings = 0;
int notDone = 1;
char **inputString;

void usage(void) {
  /* Error function in case of incorrect command-line argument specifications */
  printf("\nuseage: effects flag \n");
  printf("    where flag = -ip for realtime SKINI input by pipe\n");
  printf("                 (won't work under Win95/98),\n");
  printf("    and flag = -is for realtime SKINI input by socket.\n");
  exit(0);
}

int main(int argc,char *argv[])
{
  MY_FLOAT inSample = 0.0;
  MY_FLOAT lastSample = 0.0;
  MY_FLOAT byte3;
  long i, synlength;
  int type, j, outOne = 0, effect = 0, useSocket = 0;

  if (argc != 2) usage();

  if (!strcmp(argv[1],"-is") )
    useSocket = 1;
  else if (strcmp(argv[1],"-ip")) {
    usage();
  }

  RTDuplex *inout = new RTDuplex(SRATE,1);
  Echo *echo = new Echo(SRATE);   // one second delay
  PitShift *shifter = new PitShift();
  Chorus *chorus = new Chorus(5000.0);
  PRCRev *prcrev = new PRCRev(2.0);
  JCRev *jcrev = new JCRev(2.0);
  NRev *nrev = new NRev(2.0);
  SKINI11 *score = new SKINI11();
  Envelope *envelope = new Envelope;

  // Start the input thread
  if (useSocket)
    startSocketThread();
  else
    startPipeThread();

  /* Finally ... the runtime loop begins! */
  notDone = 1;
  synlength = RT_BUFFER_SIZE;
  while(notDone || numStrings) {
    if (numStrings > 1) synlength = (long) RT_BUFFER_SIZE / numStrings;
    else synlength = RT_BUFFER_SIZE;
    for ( i=0; i<synlength; i++ ) {
      if (effect == 0)
        inSample = inout->tick(envelope->tick() * echo->tick(lastSample));
      else if (effect == 1)
        inSample = inout->tick(envelope->tick() * shifter->tick(lastSample));
      else if (effect == 2)
        inSample = inout->tick(envelope->tick() * chorus->tick(lastSample));
      else if (effect == 3)
        inSample = inout->tick(envelope->tick() * prcrev->tick(lastSample));
      else if (effect == 4)
        inSample = inout->tick(envelope->tick() * jcrev->tick(lastSample));
      else if (effect == 5)
        inSample = inout->tick(envelope->tick() * nrev->tick(lastSample));
      lastSample = inSample;
    }
    if (numStrings) {
      score->parseThis(inputString[outOne]);
      type = score->getType();
      if (type > 0) {
        switch(type) {
        case __SK_NoteOn_:
          // check to see if velocity is zero ... really a NoteOff
          if (( byte3 = score->getByteThree() ) == 0) { // NoteOff
            envelope->setRate(0.001);
            envelope->setTarget(0.0);
          }
          else { // really a NoteOn
            envelope->setRate(0.001);
            envelope->setTarget(1.0);
          }
        break;

        case __SK_NoteOff_:
          envelope->setRate(0.001);
          envelope->setTarget(0.0);
          break;

        case __SK_ControlChange_:
          j = (int) score->getByteTwo();
          byte3 = score->getByteThree();
          if (j == 20) effect = (int) byte3;  // effect change
          else if (j == 21) { // effects mix
            echo->setEffectMix(byte3*NORM_7);
            shifter->setEffectMix(byte3*NORM_7);
            chorus->setEffectMix(byte3*NORM_7);
            prcrev->setEffectMix(byte3*NORM_7);
            jcrev->setEffectMix(byte3*NORM_7);
            nrev->setEffectMix(byte3*NORM_7);
          }
          else if (j == 22) { // effect1 parameter change
            echo->setDelay(byte3*NORM_7*SRATE*0.95 + 2);
            shifter->setShift(byte3*NORM_7*3 + 0.25);
            chorus->setModFreq(byte3*NORM_7);
          }
          else if (j == 23) { // effect1 parameter change
            chorus->setModDepth(byte3*NORM_7*0.2);
          }
          break;
        }
      }
      outOne += 1;
      if (outOne == MAX_IN_STRINGS) outOne = 0;
      numStrings--;
    }
  }

  envelope->setRate(0.001);
  envelope->setTarget(0.0);
  for (i=0;i<SRATE;i++) { /* let the sound settle a bit */
    if (effect == 0)
      inSample = inout->tick(envelope->tick() * echo->tick(lastSample));
    else if (effect == 1)
      inSample = inout->tick(envelope->tick() * shifter->tick(lastSample));
    else if (effect == 2)
      inSample = inout->tick(envelope->tick() * chorus->tick(lastSample));
      else if (effect == 3)
        inSample = inout->tick(envelope->tick() * prcrev->tick(lastSample));
      else if (effect == 4)
        inSample = inout->tick(envelope->tick() * jcrev->tick(lastSample));
      else if (effect == 5)
        inSample = inout->tick(envelope->tick() * nrev->tick(lastSample));
    lastSample = inSample;
  }

  delete inout;
  delete echo;
  delete shifter;
  delete chorus;
  delete prcrev;
  delete jcrev;
  delete nrev;
  delete score;
  delete envelope;

	printf("effects finished ... goodbye.\n");
  return 0;
}
