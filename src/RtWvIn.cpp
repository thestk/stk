/*******************************************/
/*  RtWvIn Input Class,                    */
/*  by Gary P. Scavone, 1999-2000          */
/*                                         */
/*  This object inherits from WvIn and is  */
/*  used to read in realtime 16-bit data   */
/*  from a computer's audio port.          */
/*                                         */
/*  NOTE: This object is NOT intended for  */
/*  use in achieving simultaneous realtime */
/*  audio input/output (together with      */
/*  RtWvOut). Under certain circumstances  */
/*  such a scheme is possible, though you  */
/*  should definitely know what you are    */
/*  doing before trying.  For safer "full- */
/*  duplex" operation, use the RtDuplex    */
/*  class.                                 */
/*******************************************/

#include "RtWvIn.h"

RtWvIn :: RtWvIn(int chans, MY_FLOAT srate, int device)
{
  chunking = 1;
  looping = 0;
  sound_dev = new RtAudio(chans, srate, "record", device);
  channels = chans;
  bufferSize = RT_BUFFER_SIZE;
  data = 0;
  rtdata = (INT16 *) new INT16[(bufferSize+1)*channels];

  lastSamples = (INT16 *) new INT16[channels];
  for (int i=0;i<channels;i++) {
    lastSamples[i] = (INT16) 0.0;
  }

  this->getData(0);

  rate = (MY_FLOAT) srate / SRATE;
  if (fmod(rate, 1.0) > 0.0) interpolate = 1;
  else interpolate = 0;
  phaseOffset = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT *) new MY_FLOAT[channels];
  this->reset();

  gain = 0.00003052;
#if (defined(__STK_REALTIME_) && defined(__OS_IRIX_))
  // This is necessary under IRIX because it scales the input by 0.5
  // when using single-channel input.
  if (channels == 1) gain *= 2;
#endif
}

RtWvIn :: ~RtWvIn()
{
  delete sound_dev;
  if (rtdata) {
    delete [ ] rtdata;
    rtdata = 0;
  }
  if (lastSamples) {
    delete [ ] lastSamples;
    lastSamples = 0;
  }
}

void RtWvIn :: setRate(MY_FLOAT aRate)
{
  // Negative rates not allowed for realtime input
  rate = fabs(aRate);

  if (fmod(rate, 1.0) != 0.0) interpolate = 1;
  else interpolate = 0;
}

void RtWvIn :: addTime(MY_FLOAT aTime)   
{
  // Negative time shift no allowed for realtime input
  time += fabs(aTime);
}

void RtWvIn :: setLooping(int aLoopStatus)
{
  // No looping for realtime data.
  looping = 0;
}

long RtWvIn :: getSize()
{
  return bufferSize;
}

void RtWvIn :: getData(long index)
{
  static long temp = RT_BUFFER_SIZE*channels;
  sound_dev->recordBuffer(&rtdata[channels],temp);

  /* Fill in the extra sample frame for interpolation.
   * We do this by pre-pending the last sample frame
   * from the previous input buffer to the current one.
   */
  for (int i=0;i<channels;i++) {
    rtdata[i] = lastSamples[i];
    lastSamples[i] = rtdata[temp+i];
  }
}

int RtWvIn :: informTick()
{
  static MY_FLOAT alpha;
  static long index;

  if (time >= bufferSize) {
    this->getData(0);
    while (time >= bufferSize)
      time -= bufferSize;
  }

  // integer part of time address
  index = (long) time;

  if (interpolate) {
    // fractional part of time address
    alpha = time - (MY_FLOAT) index;
    index *= channels;
    for (int i=0;i<channels;i++) {
      //  Do linear interpolation
      lastOutput[i] = (MY_FLOAT) rtdata[index];
      lastOutput[i] += alpha * ((MY_FLOAT) rtdata[index+channels] - lastOutput[i]);
      lastOutput[i] *= gain;
      index++;
    }
  }
  else {
    index *= channels;
    for (int i=0;i<channels;i++) {
      lastOutput[i] = rtdata[index++];
      lastOutput[i] *= gain;
    }
  }

  // increment time, which can be negative
  time += rate;

  return finished;
}
