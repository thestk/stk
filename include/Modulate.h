/***************************************************/
/*! \class Modulate
    \brief STK periodic/random modulator.

    This class combines random and periodic
    modulations to give a nice, natural human
    modulation function.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#ifndef STK_MODULATE_H
#define STK_MODULATE_H

#include "Generator.h"
#include "SineWave.h"
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

 protected:

  StkFloat computeSample( void );

  SineWave vibrato_;
  SubNoise noise_;
  OnePole  filter_;
  StkFloat vibratoGain_;
  StkFloat randomGain_;

};

#endif
