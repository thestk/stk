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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_MOOG_H
#define STK_MOOG_H

#include "Sampler.h"
#include "FormSwep.h"

class Moog : public Sampler
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Moog();

  //! Class destructor.
  ~Moog();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(StkFloat frequency);

  //! Start a note with the given frequency and amplitude.
  void noteOn(StkFloat frequency, StkFloat amplitude);

  //! Set the modulation (vibrato) speed in Hz.
  void setModulationSpeed(StkFloat mSpeed);

  //! Set the modulation (vibrato) depth.
  void setModulationDepth(StkFloat mDepth);

  //! Compute one output sample.
  StkFloat tick();

  //! Computer \e vectorSize outputs and return them in \e vector.
  StkFloat *tick(StkFloat *vector, unsigned int vectorSize);

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, StkFloat value);

 protected:
  FormSwep filters_[2];
  StkFloat modDepth_;
  StkFloat filterQ_;
  StkFloat filterRate_;

};

#endif
