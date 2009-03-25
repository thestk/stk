/***************************************************/
/*! \class Modulate
    \brief STK periodic/random modulator.

    This class combines random and periodic
    modulations to give a nice, natural human
    modulation function.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_MODULATE_H
#define STK_MODULATE_H

#include "Generator.h"
#include "WaveLoop.h"
#include "SubNoise.h"
#include "OnePole.h"

class Modulate : public Generator
{
 public:
  //! Class constructor.
  /*!
    An StkError can be thrown if the rawwave path is incorrect.
   */
  Modulate();

  //! Class destructor.
  ~Modulate();

  //! Reset internal state.
  void reset();

  //! Set the periodic (vibrato) rate or frequency in Hz.
  void setVibratoRate(StkFloat rate);

  //! Set the periodic (vibrato) gain.
  void setVibratoGain(StkFloat gain);

  //! Set the random modulation gain.
  void setRandomGain(StkFloat gain);

  //! Compute one output sample.
  StkFloat tick();

  //! Compute \e vectorSize outputs and return them in \e vector.
  StkFloat *tick( StkFloat *vector, unsigned int vectorSize );

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

 protected:
  WaveLoop *vibrato_;
  SubNoise noise_;
  OnePole  filter_;
  StkFloat vibratoGain_;
  StkFloat randomGain_;

};

#endif
