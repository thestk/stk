/*******************************************/
/*  Real-Time Duplex Input/Output Class,   */
/*  by Gary P. Scavone, 1999-2000          */
/*                                         */
/*  This object opens the sound i/o        */
/*  device, reads buffers in from it, and  */
/*  pokes buffers of samples out to it.    */
/*                                         */
/*  At the moment, duplex mode is possible */
/*  only on Linux (OSS), IRIX, and         */
/*  Windows95/98 platforms.                */
/*******************************************/

#include "RtDuplex.h"

#if (defined(__STK_REALTIME_) )

RtDuplex :: RtDuplex(int chans, MY_FLOAT srate, int device)
{
  // We'll let RTSoundIO deal with channel and srate limitations.
  channels = chans;
  sound_dev = new RtAudio(channels, srate, "duplex", device);
  writeCounter = 0;
  gain = 0.00003052;
  data_length = RT_BUFFER_SIZE*channels;
  indata = (INT16 *) new INT16[data_length+10];
  outdata = (INT16 *) new INT16[data_length+10];
  insamples = new MY_FLOAT[channels];
#if (defined(__STK_REALTIME_) && defined(__OS_IRIX_))
  // This is necessary under IRIX because it scales the input by 0.5
  // when using single-channel input.
  if (channels == 1) gain *= 2;
#endif
  // Read half a buffer to get started
  readCounter = data_length / 2;
  sound_dev->recordBuffer(&indata[readCounter],(data_length/2));
}

RtDuplex :: ~RtDuplex()
{
  sound_dev->playBuffer(outdata,writeCounter);
  writeCounter = 0;
  while (writeCounter<data_length)	{
    outdata[writeCounter++] = 0;
  }
  sound_dev->playBuffer(outdata,writeCounter);
  sound_dev->playBuffer(outdata,writeCounter);  // Are these extra writes necessary?
  sound_dev->playBuffer(outdata,writeCounter);
  delete [ ] insamples;
  delete [ ] indata;
  delete [ ] outdata;
  delete sound_dev;
}

MY_FLOAT RtDuplex :: tick(MY_FLOAT outsample)
{
  // We offset the data read and data write calls by RT_BUFFER_SIZE / 2
  if (readCounter >= data_length) {
    sound_dev->recordBuffer(indata,data_length);
    readCounter = 0;
  }
  *insamples = (MY_FLOAT) indata[readCounter++];
  if (channels > 1) {
    int i;
    for (i=1;i<channels;i++)
      *insamples += (MY_FLOAT) indata[readCounter++];
    *insamples /= i;
  }
  *insamples *= gain;

  for (int i=0;i<channels;i++)
    outdata[writeCounter++] = (short) (outsample * 32000.0);

  if (writeCounter >= data_length) {
    sound_dev->playBuffer(outdata,data_length);
    writeCounter = 0;
  }
  return *insamples;
}

MY_MULTI RtDuplex :: mtick(MY_MULTI outsamples)
{
  int i;
  // We offset the data read and data write calls by RT_BUFFER_SIZE / 2
  if (readCounter >= data_length) {
    sound_dev->recordBuffer(indata,data_length);
    readCounter = 0;
  }
  for (i=0;i<channels;i++)
    insamples[i] = (MY_FLOAT) (indata[readCounter++]*gain);

  for (i=0;i<channels;i++)
    outdata[writeCounter++] = (short) (*outsamples++ * 32000.0);

  if (writeCounter >= data_length) {
    sound_dev->playBuffer(outdata,data_length);
    writeCounter = 0;
  }
  return insamples;
}

#endif
