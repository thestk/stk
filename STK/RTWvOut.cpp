/*******************************************/
/*  Real-Time Wave File Output Class,      */
/*  by Perry R. Cook, 1996                 */
/*  revised by Gary P. Scavone, 1999       */
/*                                         */
/*  This object opens a realtime soundout  */
/*  device, and pokes buffers of samples   */
/*  into it.                               */
/*******************************************/

#include "RTWvOut.h"

#if defined(__STK_REALTIME_)

RTWvOut :: RTWvOut(MY_FLOAT srate, int chans)
{
  // We'll let RTSoundIO deal with channel and srate limitations.
  channels = chans;
  soundIO = new RTSoundIO(srate, channels, "play");
  counter = 0;
}

RTWvOut :: ~RTWvOut()
{
  soundIO->playBuffer(data,counter);
  counter = 0;
  while (counter<RT_BUFFER_SIZE)	{
    data[counter++] = 0;
  }
  soundIO->playBuffer(data,counter);
  soundIO->playBuffer(data,counter);  // Are these extra writes necessary?
  soundIO->playBuffer(data,counter);
  delete soundIO;
}

void RTWvOut :: tick(MY_FLOAT sample)
{
  for (int i=0;i<channels;i++)
    data[counter++] = (short) (sample * 32000.0);

  if (counter >= RT_BUFFER_SIZE) {
    soundIO->playBuffer(data,counter);
    counter = 0;
  }
}

void RTWvOut :: mtick(MY_MULTI samples)
{
  for (int i=0;i<channels;i++)
    data[counter++] = (short) (*samples++ * 32000.0);

  if (counter >= RT_BUFFER_SIZE) {
    soundIO->playBuffer(data,counter);
    counter = 0;
  }
}

#endif
