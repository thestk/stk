/*******************************************/
/*  Real-Time Duplex Input/Output Class,   */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  This object opens the sound i/o        */
/*  device, reads buffers in from it, and  */
/*  pokes buffers of samples out to it.    */
/*                                         */
/*  At the moment, duplex mode is possible */
/*  only on Linux (OSS), IRIX, and         */
/*  Windows95/98 platforms.                */
/*******************************************/

#include "RTDuplex.h"

#if (defined(__STK_REALTIME_) )

RTDuplex :: RTDuplex(MY_FLOAT srate, int chans)
{
  // We'll let RTSoundIO deal with channel and srate limitations.
  channels = chans;
  soundIO = new RTSoundIO(SRATE, channels, "duplex");
  writeCounter = 0;
  gain = 0.00003052;
  insamples = new MY_FLOAT[channels];
#if (defined(__STK_REALTIME_) && defined(__OS_IRIX_))
  // This is necessary under IRIX because it scales the input by 0.5
  // when using single-channel input.
  if (channels == 1) gain *= 2;
#endif
  // Read half a buffer to get started
  readCounter = RT_BUFFER_SIZE / 2;
  soundIO->recordBuffer(&indata[readCounter],(RT_BUFFER_SIZE/2));
}

RTDuplex :: ~RTDuplex()
{
  soundIO->playBuffer(outdata,writeCounter);
  writeCounter = 0;
  while (writeCounter<RT_BUFFER_SIZE)	{
    outdata[writeCounter++] = 0;
  }
  soundIO->playBuffer(outdata,writeCounter);
  soundIO->playBuffer(outdata,writeCounter);  // Are these extra writes necessary?
  soundIO->playBuffer(outdata,writeCounter);
  delete soundIO;
  delete [ ] insamples;
}

MY_FLOAT RTDuplex :: tick(MY_FLOAT outsample)
{
  // We offset the data read and data write calls by RT_BUFFER_SIZE / 2
  if (readCounter >= RT_BUFFER_SIZE) {
    soundIO->recordBuffer(indata,RT_BUFFER_SIZE);
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

  if (writeCounter >= RT_BUFFER_SIZE) {
    soundIO->playBuffer(outdata,writeCounter);
    writeCounter = 0;
  }
  return *insamples;
}

MY_MULTI RTDuplex :: mtick(MY_MULTI outsamples)
{
  int i;
  // We offset the data read and data write calls by RT_BUFFER_SIZE / 2
  if (readCounter >= RT_BUFFER_SIZE) {
    soundIO->recordBuffer(indata,RT_BUFFER_SIZE);
    readCounter = 0;
  }
  for (i=0;i<channels;i++)
    insamples[i] = (MY_FLOAT) (indata[readCounter++]*gain);

  for (i=0;i<channels;i++)
    outdata[writeCounter++] = (short) (*outsamples++ * 32000.0);

  if (writeCounter >= RT_BUFFER_SIZE) {
    soundIO->playBuffer(outdata,writeCounter);
    writeCounter = 0;
  }
  return insamples;
}

#endif
