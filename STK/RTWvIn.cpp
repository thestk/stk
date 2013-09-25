/*******************************************/
/*  RTWvIn Input Class,                    */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  This object inherits from WvIn and is  */
/*  used to read in realtime 16-bit data   */
/*  from a computer's audio port.          */
/*                                         */
/*  NOTE: This object is NOT intended for  */
/*  use in achieving simultaneous realtime */
/*  audio input/output (together with      */
/*  RTWvOut). Under certain circumstances  */
/*  such a scheme is possible, though you  */
/*  should definitely know what you are    */
/*  doing before trying.  For safer "full- */
/*  duplex" operation, use the RTDuplex    */
/*  class.                                 */
/*******************************************/

#include "RTWvIn.h"

RTWvIn :: RTWvIn(MY_FLOAT srate, short chans)
{
  soundIO = new RTSoundIO(srate, chans, "record");
  channels = chans;
  length = RT_BUFFER_SIZE/channels;
  data = 0;
  rtdata = (short *) new short[RT_BUFFER_SIZE+channels];

  this->getMoreData();

  rate = (MY_FLOAT) srate / SRATE;
  if (fmod(rate, 1.0) > 0.0) interpolate = 1;
  else interpolate = 0;
  looping = 0;
  time = (MY_FLOAT) 0.0;
  phaseOffset = (MY_FLOAT) 0.0;
  finished = 0;
  lastOutput = (MY_FLOAT *) calloc(channels, sizeof(MY_FLOAT));
  gain = 0.00003052;
#if (defined(__STK_REALTIME_) && defined(__OS_IRIX_))
  // This is necessary under IRIX because it scales the input by 0.5
  // when using single-channel input.
  if (channels == 1) gain *= 2;
#endif
}

RTWvIn :: ~RTWvIn()
{
  delete soundIO;
  if (rtdata) {
    delete [ ] rtdata;
    rtdata = 0;
  }
}

void RTWvIn :: normalize()
{
  /* Do nothing ... cannot normalize realtime input */
}

void RTWvIn :: normalize(MY_FLOAT newPeak)
{
  /* Do nothing ... cannot normalize realtime input */
}

void RTWvIn :: addPhaseOffset(MY_FLOAT anAngle)
{
  /* No phaseOffset for realtime input */
  phaseOffset = 0.0;
}                                            

void RTWvIn :: setLooping(int aLoopStatus)
{
  /* Cannot loop realtime input */
  looping = 0;
}

void RTWvIn :: getMoreData()
{
  soundIO->recordBuffer(rtdata,RT_BUFFER_SIZE);
  long temp = RT_BUFFER_SIZE;
  for (int i=0;i<channels;i++) {
    rtdata[temp] = rtdata[temp-channels];
    temp++;
  }
}

int RTWvIn :: informTick()
{
  static long temp;
  static MY_FLOAT alpha;

  temp = (long) time;                    /* Integer part of time address    */

  if (interpolate) {
    alpha = time - (MY_FLOAT) temp;      /* Fractional part of time address */
    temp *= channels;
    for (int i=0;i<channels;i++) {
      /* Do linear interpolation */
      lastOutput[i] = (MY_FLOAT) rtdata[temp];
      lastOutput[i] = lastOutput[i] + 
        (alpha * ((MY_FLOAT) rtdata[temp+channels] - lastOutput[i]));
      lastOutput[i] *= gain;
      temp++;
    }
  }
  else {
    temp *= channels;
    for (int i=0;i<channels;i++) {
      lastOutput[i] = rtdata[temp++];
      lastOutput[i] *= gain;
    }
  }

  time += rate;                 /* Increment time           */
  if (time >= length) {
    this->getMoreData();
  }
  while (time >= length)        /*  Check for end of sound  */
    time -= length;             /*  loop back to beginning  */
  while (time < 0.0)            /*  Check for end of sound  */
    time += length;             /*  loop back to beginning  */

  return finished;
}
