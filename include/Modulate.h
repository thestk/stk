/***************************************************/
/*! \class Modulate
    \brief STK periodic/random modulator.

    This class combines random and periodic
    modulations to give a nice, natural human
    modulation function.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__MODULATE_H)
#define __MODULATE_H

#include "Stk.h"
#include "WaveLoop.h"
#include "SubNoise.h"
#include "OnePole.h"

class Modulate : public Stk
{
 public:
  //! Class constructor.
  Modulate();

  //! Class destructor.
  ~Modulate();

  //! Reset internal state.
  void reset();

  //! Set the periodic (vibrato) rate or frequency in Hz.
  void setVibratoRate(MY_FLOAT aRate);

  //! Set the periodic (vibrato) gain.
  void setVibratoGain(MY_FLOAT aGain);

  //! Set the random modulation gain.
  void setRandomGain(MY_FLOAT aGain);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Return \e vectorSize outputs in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

  //! Return the last computed output value.
  MY_FLOAT lastOut() const;

 protected:
  WaveLoop *vibrato;
  SubNoise *noise;
  OnePole  *filter;
  MY_FLOAT vibratoGain;
  MY_FLOAT randomGain;
  MY_FLOAT lastOutput;

};

#endif
