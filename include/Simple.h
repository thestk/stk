/***************************************************/
/*! \class Simple
    \brief STK wavetable/noise instrument.

    This class combines a looped wave, a
    noise source, a biquad resonance filter,
    a one-pole filter, and an ADSR envelope
    to create some interesting sounds.

    Control Change Numbers: 
       - Filter Pole Position = 2
       - Noise/Pitched Cross-Fade = 4
       - Envelope Rate = 11
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SIMPLE_H)
#define __SIMPLE_H

#include "Instrmnt.h"
#include "ADSR.h"
#include "WaveLoop.h"
#include "OnePole.h"
#include "BiQuad.h"
#include "Noise.h"

class Simple : public Instrmnt
{
 public:
  //! Class constructor.
  Simple();

  //! Class destructor.
  virtual ~Simple();

  //! Clear internal states.
  void clear();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Start envelope toward "on" target.
  void keyOn();

  //! Start envelope toward "off" target.
  void keyOff();

  //! Start a note with the given frequency and amplitude.
  virtual void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  virtual MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

 protected:  
  ADSR     *adsr; 
  WaveLoop  *loop;
  OnePole  *filter;
  BiQuad   *biquad;
  Noise    *noise;
  MY_FLOAT baseFrequency;
  MY_FLOAT loopGain;

};

#endif
