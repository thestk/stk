/***************************************************/
/*! \class Moog
    \brief STK moog-like swept filter sampling synthesis class.

    This instrument uses one attack wave, one
    looped wave, and an ADSR envelope (inherited
    from the Sampler class) and adds two sweepable
    formant (FormSwep) filters.

    Control Change Numbers: 
       - Filter Q = 2
       - Filter Sweep Rate = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__MOOG_H)
#define __MOOG_H

#include "Sampler.h"
#include "FormSwep.h"

class Moog : public Sampler
{
 public:
  //! Class constructor.
  Moog();

  //! Class destructor.
  ~Moog();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Start a note with the given frequency and amplitude.
  virtual void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Set the modulation (vibrato) speed in Hz.
  void setModulationSpeed(MY_FLOAT mSpeed);

  //! Set the modulation (vibrato) depth.
  void setModulationDepth(MY_FLOAT mDepth);

  //! Compute one output sample.
  virtual MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

 protected:
  FormSwep *filters[2];
  MY_FLOAT modDepth;
  MY_FLOAT filterQ;
  MY_FLOAT filterRate;

};

#endif
