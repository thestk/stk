/**************  Effects Program  *********************/

#include "RtDuplex.h"
#include "SKINI.h"
#include "SKINI.msg"
#include "Envelope.h"
#include "PRCRev.h"
#include "JCRev.h"
#include "NRev.h"
#include "Echo.h"
#include "PitShift.h"
#include "Chorus.h"

// The input control handler.
#include "Messager.h"

void usage(void) {
  /* Error function in case of incorrect command-line argument specifications */
  printf("\nuseage: effects flags \n");
  printf("    where flag = -s RATE to specify a sample rate,\n");
  printf("    flag = -ip for realtime SKINI input by pipe\n");
  printf("           (won't work under Win95/98),\n");
  printf("    and flag = -is <port> for realtime SKINI input by socket.\n");
  exit(0);
}

int main(int argc,char *argv[])
{
  if (argc < 2 || argc > 6) usage();

  // If you want to change the default sample rate (set in Stk.h), do
  // it before instantiating any objects!  If the sample rate is
  // specified in the command line, it will override this setting.
  Stk::setSampleRate(22050.0);

  int port = -1;
  int controlMask = 0;
  for (int k=1; k<argc; k++ ) {
    if (!strcmp(argv[k],"-is") ) {
      controlMask |= STK_SOCKET;
      if (k+1 < argc && argv[k+1][0] != '-' ) port = atoi(argv[++k]);
    }
    else if (!strcmp(argv[k],"-ip") )
      controlMask |= STK_PIPE;
    else if (!strcmp(argv[k],"-s") && (k+1 < argc) && argv[k+1][0] != '-')
      Stk::setSampleRate( atoi(argv[++k]) );
    else
      usage();
  }

  bool done;
  int effect = 0;
  MY_FLOAT lastSample, inSample;
  Envelope *envelope = new Envelope;
  PRCRev *prcrev = new PRCRev(2.0);
  JCRev *jcrev = new JCRev(2.0);
  NRev *nrev = new NRev(2.0);
  Echo *echo = new Echo( (long) Stk::sampleRate() );   // one second delay
  PitShift *shifter = new PitShift();
  Chorus *chorus = new Chorus(5000.0);
  SKINI *score = new SKINI();
  Messager *messager = 0;
  RtDuplex *inout = 0;

  try {
    // Change the nBuffers parameter to a smaller number to get better input/output latency.
    inout = new RtDuplex(1, Stk::sampleRate(), 0, RT_BUFFER_SIZE, 10);

    // Instantiate the input message controller.
    if ( controlMask & STK_SOCKET && port >= 0 )
      messager = new Messager( controlMask, port );
    else
      messager = new Messager( controlMask );
  }
  catch (StkError &) {
    goto cleanup;
  }

  // The runtime loop begins here:
  long i, nTicks;
  int type;
  lastSample = 0.0;
  inSample = 0.0;
  MY_FLOAT byte2, byte3;
  done = FALSE;
  while (!done) {

    // Look for new messages and return a delta time (in samples).
    type = messager->nextMessage();
    if (type < 0)
      done = TRUE;

    nTicks = messager->getDelta();

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

    if (type > 0) {
      // parse the input control message

      byte2 = messager->getByteTwo();
      byte3 = messager->getByteThree();

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
          echo->setEffectMix(byte3*ONE_OVER_128);
          shifter->setEffectMix(byte3*ONE_OVER_128);
          chorus->setEffectMix(byte3*ONE_OVER_128);
          prcrev->setEffectMix(byte3*ONE_OVER_128);
          jcrev->setEffectMix(byte3*ONE_OVER_128);
          nrev->setEffectMix(byte3*ONE_OVER_128);
        }
        else if (byte2 == 22) { // effect1 parameter change
          echo->setDelay(byte3*ONE_OVER_128*Stk::sampleRate()*0.95);
          shifter->setShift(byte3*ONE_OVER_128*3 + 0.25);
          chorus->setModFrequency(byte3*ONE_OVER_128);
        }
        else if (byte2 == 23) { // effect1 parameter change
          chorus->setModDepth(byte3*ONE_OVER_128*0.2);
        }
        break;
      }
    }
  }

  envelope->setRate(0.001);
  envelope->setTarget(0.0);
  nTicks = (long) Stk::sampleRate();
  for (i=0; i<nTicks; i++) { // let the sound settle a bit
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

  delete echo;
  delete shifter;
  delete chorus;
  delete prcrev;
  delete jcrev;
  delete nrev;
  delete score;
  delete envelope;

 cleanup:
  delete messager;
  delete inout;

	printf("effects finished ... goodbye.\n");
  return 0;
}
