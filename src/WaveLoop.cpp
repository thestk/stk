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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "WaveLoop.h"

WaveLoop :: WaveLoop( std::string fileName, bool raw )
  : WvIn( fileName, raw ), phaseOffset_(0.0)
{
  // If at end of file, redo extra sample frame for looping.
  if (chunkPointer_+bufferSize_ == fileSize_) {
    for (unsigned int j=0; j<channels_; j++)
      data_[bufferSize_*channels_+j] = data_[j];
  }
}

WaveLoop :: ~WaveLoop()
{
}

void WaveLoop :: readData( unsigned long index )
{
  WvIn::readData( index );

  // If at end of file, redo extra sample frame for looping.
  if (chunkPointer_+bufferSize_ == fileSize_) {
    for (unsigned int j=0; j<channels_; j++)
      data_[bufferSize_*channels_+j] = data_[j];
  }
}

void WaveLoop :: setFrequency(StkFloat frequency)
{
  // This is a looping frequency.
  rate_ = fileSize_ * frequency / Stk::sampleRate();
}

void WaveLoop :: addTime(StkFloat time)
{
  // Add an absolute time in samples 
  time_ += time;

  while (time_ < 0.0)
    time_ += fileSize_;
  while (time_ >= fileSize_)
    time_ -= fileSize_;
}

void WaveLoop :: addPhase(StkFloat angle)
{
  // Add a time in cycles (one cycle = fileSize).
  time_ += fileSize_ * angle;

  while (time_ < 0.0)
    time_ += fileSize_;
  while (time_ >= fileSize_)
    time_ -= fileSize_;
}

void WaveLoop :: addPhaseOffset(StkFloat angle)
{
  // Add a phase offset in cycles, where 1.0 = fileSize.
  phaseOffset_ = fileSize_ * angle;
}

const StkFloat *WaveLoop :: tickFrame(void)
{
  register StkFloat tyme, alpha;
  register unsigned long i, index;

  // Check limits of time address ... if necessary, recalculate modulo fileSize.
  while (time_ < 0.0)
    time_ += fileSize_;
  while (time_ >= fileSize_)
    time_ -= fileSize_;

  if (phaseOffset_) {
    tyme = time_ + phaseOffset_;
    while (tyme < 0.0)
      tyme += fileSize_;
    while (tyme >= fileSize_)
      tyme -= fileSize_;
  }
  else {
    tyme = time_;
  }

  if (chunking_) {
    // Check the time address vs. our current buffer limits.
    if ( (tyme < chunkPointer_) || (tyme >= chunkPointer_+bufferSize_) )
      this->readData((long) tyme);
    // Adjust index for the current buffer.
    tyme -= chunkPointer_;
  }

  // Always do linear interpolation here ... integer part of time address.
  index = (unsigned long) tyme;

  // Fractional part of time address.
  alpha = tyme - (StkFloat) index;
  index *= channels_;
  for (i=0; i<channels_; i++) {
    lastOutputs_[i] = data_[index];
    lastOutputs_[i] += (alpha * (data_[index+channels_] - lastOutputs_[i]));
    index++;
  }

  if (chunking_) {
    // Scale outputs by gain.
    for (i=0; i<channels_; i++)  lastOutputs_[i] *= gain_;
  }

  // Increment time, which can be negative.
  time_ += rate_;

  return lastOutputs_;
}

StkFloat *WaveLoop :: tickFrame(StkFloat *frameVector, unsigned int frames)
{
  return WvIn::tickFrame( frameVector, frames );
}

StkFrames& WaveLoop :: tickFrame( StkFrames& frames )
{
  return WvIn::tickFrame( frames );
}
