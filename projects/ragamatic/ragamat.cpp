/**************  Test Main Program Individual Voice *********************/

#include "RtWvOut.h"
#include "SKINI.msg"
#include "Instrmnt.h"
#include "Reverb.h"
#include "JCRev.h"
#include "Drone.h"
#include "Sitar.h"
#include "Tabla.h"
#include "VoicDrum.h"

// The input control handler.
#include "Messager.h"

MY_FLOAT float_random(MY_FLOAT max) // Return random float between 0.0 and max
{	
  MY_FLOAT temp = (MY_FLOAT) (max * rand() / (RAND_MAX + 1.0) );
  return temp;	
}

void usage(void) {
  /* Error function in case of incorrect command-line argument specifications */
  printf("\nuseage: ragamat flags \n");
  printf("    where flag = -s RATE to specify a sample rate,\n");
  printf("    flag = -ip for realtime SKINI input by pipe\n");
  printf("           (won't work under Win95/98),\n");
  printf("    and flag = -is <port> for realtime SKINI input by socket.\n");
  exit(0);
}

int main(int argc,char *argv[])
{
  bool done;
  RtWvOut *output;
  Instrmnt *drones[3];
  Instrmnt *sitar;
  Instrmnt *voicDrums;
  Instrmnt *tabla;
  Reverb *reverbs[2];
  SKINI *score;
  Messager *messager;
  MY_FLOAT t60 = 4.0;  // in seconds

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

  // If you want to change the default sample rate (set in Stk.h), do
  // it before instantiating any objects!  If the sample rate is
  // specified in the command line, it will override this setting.
  Stk::setSampleRate(22050.0);

  if (argc < 2 || argc > 6) usage();

  int port = -1;
  int i, controlMask = 0;
  for ( i=1; i<argc; i++ ) {
    if (!strcmp(argv[i],"-is") ) {
      controlMask |= STK_SOCKET;
      if (i+1 < argc && argv[i+1][0] != '-' ) port = atoi(argv[++i]);
    }
    else if (!strcmp(argv[i],"-ip") )
      controlMask |= STK_PIPE;
    else if (!strcmp(argv[i],"-s") && (i+1 < argc) && argv[i+1][0] != '-')
      Stk::setSampleRate( atoi(argv[++i]) );
    else
      usage();
  }
  
  try {
    output = new RtWvOut(2);

    // Instantiate the input message controller.
    if ( controlMask & STK_SOCKET && port >= 0 )
      messager = new Messager( controlMask, port );
    else
      messager = new Messager( controlMask );
  }
  catch (StkError &) {
    exit(0);
  }

  drones[0] = new Drone(50.0);
  drones[1] = new Drone(50.0);
  drones[2] = new Drone(50.0);
  sitar = new Sitar(50.0);
  voicDrums = new VoicDrum();
  tabla = new Tabla();

  score = new SKINI();
  reverbs[0] = new JCRev(t60);
  reverbs[0]->setEffectMix(0.5);
  reverbs[1] = new JCRev(2.0);
  reverbs[1]->setEffectMix(0.2);

  drones[0]->noteOn(droneFreqs[0],0.1);
  drones[1]->noteOn(droneFreqs[1],0.1);
  drones[2]->noteOn(droneFreqs[2],0.1);

  MY_FLOAT outSamples[2];
  for (i=0;i<Stk::sampleRate();i++) { /* warm everybody up a little */
    outSamples[0] = reverbs[0]->tick(drones[0]->tick() + drones[2]->tick());
    outSamples[1] = reverbs[1]->tick(1.5 * drones[1]->tick());
    output->tickFrame(outSamples);
  }

  // The runtime loop begins here:
  done = FALSE;
  MY_FLOAT rateScaler = 22050.0 / Stk::sampleRate();
  int nTicks, type;
  MY_FLOAT temp, byte2, byte3;
  while (!done) {

    type = messager->nextMessage();
    if (type < 0)
      done = TRUE;

    nTicks = messager->getDelta();

    for (i=0; i<nTicks; i++) {
      outSamples[0] = reverbs[0]->tick(drones[0]->tick() + drones[2]->tick()
                                       + sitar->tick());
      outSamples[1] = reverbs[1]->tick(1.5 * drones[1]->tick() + 0.5 * voicDrums->tick()
                                       + 0.5 * tabla->tick());
      // mix a little left to right and back
      temp = outSamples[0];
      outSamples[0] += 0.3 * outSamples[1];
      outSamples[1] += 0.3 * temp;
      output->tickFrame(outSamples);

      counter -= 1;
      if (counter == 0)	{
        counter = (int) (tempo / rateScaler);
        if (float_random(1.0) < drone_prob)
          drones[0]->noteOn(droneFreqs[0], 0.1);
        if (float_random(1.0) < drone_prob)
          drones[1]->noteOn(droneFreqs[1], 0.1);
        if (float_random(1.0) < drone_prob)
          drones[2]->noteOn(droneFreqs[2], 0.1);
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
            sitar->noteOn(Midi2Pitch[ragaUp[key][ragaPoint]],
                          0.05 + float_random(0.3));
          else
            sitar->noteOn(Midi2Pitch[ragaDown[key][ragaPoint]],
                          0.05 + float_random(0.3));
        }
        if (float_random(1.0) < voic_prob)	{
          voicNote = (int) float_random(11);
          voicDrums->noteOn(voicNote, 0.3 + (0.4 * drum_prob) +
                            float_random(0.3 * voic_prob));
        }
        if (float_random(1.0) < drum_prob)	{
          voicNote = (int) float_random(TABLA_NUMWAVES);
          tabla->noteOn(voicNote, 0.2 + (0.2 * drum_prob) + 
                        float_random(0.6 * drum_prob));
        }
      }
    }

    if ( type > 0 ) {
      // parse the input control message

      byte2 = messager->getByteTwo();
      byte3 = messager->getByteThree();

      switch(type) {

      case __SK_ControlChange_:
        if (byte2 == 1)	{
          drone_prob = byte3 * ONE_OVER_128;
        }
        else if (byte2 == 2)	{
          note_prob = byte3 * ONE_OVER_128;
        }
        else if (byte2 == 4)	{
          voic_prob = byte3 * ONE_OVER_128;
        }
        else if (byte2 == 11)	{
          drum_prob = byte3 * ONE_OVER_128;
        }
        else if (byte2 == 7)	{
          tempo = (int) (11025 - (byte3 * 70));
        }
        else if (byte2 == 64)	{
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
  }

  nTicks = (long) (t60 * Stk::sampleRate());

  printf("What Need Have I for This?\n");
  drones[1]->noteOn(droneFreqs[1],0.1);
  for (i=0; i<nTicks; i++) { // Calm down a little
    outSamples[0] = reverbs[0]->tick(drones[0]->tick() + drones[2]->tick());
    outSamples[1] = reverbs[1]->tick(1.5 * drones[1]->tick());
    output->tickFrame(outSamples);
  }
  printf("What Need Have I for This?\n");
  drones[2]->noteOn(droneFreqs[2],0.1);
  for (i=0; i<nTicks; i++) { // and a little more
    outSamples[0] = reverbs[0]->tick(drones[0]->tick() + drones[2]->tick());
    outSamples[1] = reverbs[1]->tick(1.5 * drones[1]->tick());
    output->tickFrame(outSamples);
  }
  printf("RagaMatic finished ... \n");
  drones[0]->noteOn(droneFreqs[0],0.1);
  for (i=0; i<nTicks; i++) { // almost ready to think about ending
    outSamples[0] = reverbs[0]->tick(drones[0]->tick() + drones[2]->tick());
    outSamples[1] = reverbs[1]->tick(1.5 * drones[1]->tick());
    output->tickFrame(outSamples);
  }
  printf("All is Bliss ...\n");
  for (i=0; i<nTicks; i++) { // nearly finished now
    outSamples[0] = reverbs[0]->tick(drones[0]->tick() + drones[2]->tick());
    outSamples[1] = reverbs[1]->tick(1.5 * drones[1]->tick());
    output->tickFrame(outSamples);
  }
  printf("All is Bliss ...\n");
  for (i=0; i<nTicks; i++) { // all is bliss....
    outSamples[0] = reverbs[0]->tick(drones[0]->tick() + drones[2]->tick());
    outSamples[1] = reverbs[1]->tick(1.5 * drones[1]->tick());
    output->tickFrame(outSamples);
  }

  delete output;
  delete score;
  delete drones[0];
  delete drones[1];
  delete drones[2];
  delete sitar;
  delete tabla;
  delete voicDrums;
  delete reverbs[0];
  delete reverbs[1];
  delete messager;

  return 0;
}
