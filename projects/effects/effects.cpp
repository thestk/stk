/**************  Effects Program  *********************/

#include "RtDuplex.h"
#include "SKINI11.h"
#include "SKINI11.msg"
#include "Envelope.h"
#include "PRCRev.h"
#include "JCRev.h"
#include "NRev.h"
#include "Echo.h"
#include "PitShift.h"
#include "Chorus.h"

// The input control handler.
#include "Controller.h"

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
  MY_FLOAT byte2, byte3;
  long i, nTicks;
  int type, effect = 0;
  int controlMask = 0;
  bool done;
  Controller *controller;

  if (argc != 2) usage();

  if (!strcmp(argv[1],"-is") )
    controlMask |= STK_SOCKET;
  else if (!strcmp(argv[1],"-ip") )
    controlMask |= STK_PIPE;
  else
    usage();

  Envelope *envelope = new Envelope;
  PRCRev *prcrev = new PRCRev(2.0);
  JCRev *jcrev = new JCRev(2.0);
  NRev *nrev = new NRev(2.0);
  Echo *echo = new Echo(SRATE);   // one second delay
  PitShift *shifter = new PitShift();
  Chorus *chorus = new Chorus(5000.0);
  SKINI11 *score = new SKINI11();
  RtDuplex *inout;

  try {
    inout = new RtDuplex(1, SRATE);

    // Instantiate the input message controller.
    controller = new Controller( controlMask );
  }
  catch (StkError& m) {
    m.printMessage();
    exit(0);
  }

  // The runtime loop begins here:
  done = FALSE;
  while (!done) {

    nTicks = controller->getNextMessage();

    if (nTicks == -1)
      done = TRUE;

    for (i=0; i<nTicks; i++) {
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

    type = controller->getType();
    if (type > 0) {
      // parse the input control message

      byte2 = controller->getByte2();
      byte3 = controller->getByte3();

      switch(type) {

      case __SK_NoteOn_:
        if (byte3 == 0) { // velocity is zero ... really a NoteOff
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
        if (byte2 == 20) effect = (int) byte3;  // effect change
        else if (byte2 == 44) { // effects mix
          echo->setEffectMix(byte3*NORM_7);
          shifter->setEffectMix(byte3*NORM_7);
          chorus->setEffectMix(byte3*NORM_7);
          prcrev->setEffectMix(byte3*NORM_7);
          jcrev->setEffectMix(byte3*NORM_7);
          nrev->setEffectMix(byte3*NORM_7);
        }
        else if (byte2 == 22) { // effect1 parameter change
          echo->setDelay(byte3*NORM_7*SRATE*0.95 + 2);
          shifter->setShift(byte3*NORM_7*3 + 0.25);
          chorus->setModFreq(byte3*NORM_7);
        }
        else if (byte2 == 23) { // effect1 parameter change
          chorus->setModDepth(byte3*NORM_7*0.2);
        }
        break;
      }
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
  delete controller;

	printf("effects finished ... goodbye.\n");
  return 0;
}
