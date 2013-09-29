/***************************************************/
/*! \class WaveLoop
    \brief STK waveform oscillator class.

    This class inherits from WvIn and provides
    audio file looping functionality.

    WaveLoop supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which return
    samples produced by averaging across sample
    frames, from the tickFrame() methods, which
    return pointers to multi-channel sample frames.
    For single-channel data, these methods return
    equivalent values.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "WaveLoop.h"
#include <math.h>

WaveLoop :: WaveLoop( const char *fileName, bool raw )
  : WvIn( fileName, raw ), phaseOffset(0.0)
{
  // If at end of file, redo extra sample frame for looping.
  if (chunkPointer+bufferSize == fileSize) {
    for (unsigned int j=0; j<channels; j++)
      data[bufferSize*channels+j] = data[j];
  }
}

WaveLoop :: ~WaveLoop()
{
}

void WaveLoop :: readData( unsigned long index )
{
  WvIn::readData( index );

  // If at end of file, redo extra sample frame for looping.
  if (chunkPointer+bufferSize == fileSize) {
    for (unsigned int j=0; j<channels; j++)
      data[bufferSize*channels+j] = data[j];
  }
}

void WaveLoop :: setFrequency(MY_FLOAT aFrequency)
{
  // This is a looping frequency.
  rate = fileSize * aFrequency / sampleRate();
}

void WaveLoop :: addTime(MY_FLOAT aTime)
{
  // Add an absolute time in samples 
  time += aTime;

  while (time < 0.0)
    time += fileSize;
  while (time >= fileSize)
    time -= fileSize;
}

void WaveLoop :: addPhase(MY_FLOAT anAngle)
{
  // Add a time in cycles (one cycle = fileSize).
  time += fileSize * anAngle;

  while (time < 0.0)
    time += fileSize;
  while (time >= fileSize)
    time -= fileSize;
}

void WaveLoop :: addPhaseOffset(MY_FLOAT anAngle)
{
  // Add a phase offset in cycles, where 1.0 = fileSize.
  phaseOffset = fileSize * anAngle;
}

const MY_FLOAT *WaveLoop :: tickFrame(void)
{
  register MY_FLOAT tyme, alpha;
  register unsigned long i, index;

  // Check limits of time address ... if necessary, recalculate modulo fileSize.
  while (time < 0.0)
    time += fileSize;
  while (time >= fileSize)
    time -= fileSize;

  if (phaseOffset) {
    tyme = time + phaseOffset;
    while (tyme < 0.0)
      tyme += fileSize;
    while (tyme >= fileSize)
      tyme -= fileSize;
  }
  else {
    tyme = time;
  }

  if (chunking) {
    // Check the time address vs. our current buffer limits.
    if ( (tyme < chunkPointer) || (tyme >= chunkPointer+bufferSize) )
      this->readData((long) tyme);
    // Adjust index for the current buffer.
    tyme -= chunkPointer;
  }

  // Always do linear interpolation here ... integer part of time address.
  index = (unsigned long) tyme;

  // Fractional part of time address.
  alpha = tyme - (MY_FLOAT) index;
  index *= channels;
  for (i=0; i<channels; i++) {
    lastOutput[i] = data[index];
    lastOutput[i] += (alpha * (data[index+channels] - lastOutput[i]));
    index++;
  }

  if (chunking) {
    // Scale outputs by gain.
    for (i=0; i<channels; i++)  lastOutput[i] *= gain;
  }

  // Increment time, which can be negative.
  time += rate;

  return lastOutput;
}

