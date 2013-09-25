/*******************************************/
/*  Real-Time Audio Output Class,          */
/*  by Perry R. Cook, 1996                 */
/*  Revised by Gary P. Scavone, 2000       */
/*                                         */
/*  This object opens a realtime soundout  */
/*  device, and pokes buffers of samples   */
/*  into it.                               */
/*******************************************/

#include "RtWvOut.h"

#if defined(__STK_REALTIME_)

RtWvOut :: RtWvOut(int chans, int device)
{
  // We'll let RTSoundIO deal with channel and srate limitations.
  channels = chans;
  sound_dev = new RtAudio(channels, SRATE, "play", device);
  data_length = RT_BUFFER_SIZE*channels;
  // Add a few extra samples for good measure
  data = (INT16 *) new INT16[data_length+10];
}

RtWvOut :: ~RtWvOut()
{
  sound_dev->playBuffer(data,counter);
  counter = 0;
  while (counter<data_length)	{
    data[counter++] = 0;
  }
  sound_dev->playBuffer(data,counter);
  sound_dev->playBuffer(data,counter);  // Are these extra writes necessary?
  sound_dev->playBuffer(data,counter);
  delete sound_dev;
}

void RtWvOut :: tick(MY_FLOAT sample)
{
  for (int i=0;i<channels;i++)
    data[counter++] = (INT16) (sample * 32000.0);

  if (counter >= data_length) {
    sound_dev->playBuffer(data,data_length);
    counter = 0;
  }
}

void RtWvOut :: mtick(MY_MULTI samples)
{
  for (int i=0;i<channels;i++)
    data[counter++] = (INT16) (*samples++ * 32000.0);

  if (counter >= data_length) {
    sound_dev->playBuffer(data,data_length);
    counter = 0;
  }
}

//windows stop and start methods ... because windoze sucks
#if (defined(__OS_Win_) )

void RtWvOut :: stopPlay() {
  sound_dev->stopPlay();
}

void RtWvOut :: startPlay() {
  sound_dev->startPlay();
}

void RtWvOut :: stopRecord() {
  sound_dev->stopRecord();
}

void RtWvOut :: startRecord() {
  sound_dev->startRecord();
}

#endif  // extra windoze crap

#endif
