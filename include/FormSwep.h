/***************************************************/
/*! \class FormSwep
    \brief STK sweepable formant filter class.

    This public BiQuad filter subclass implements
    a formant (resonance) which can be "swept"
    over time from one frequency setting to another.
    It provides methods for controlling the sweep
    rate and target frequency.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__FORMSWEP_H)
#define __FORMSWEP_H

#include "BiQuad.h"

class FormSwep : public BiQuad
{
 public:

  //! Default constructor creates a second-order pass-through filter.
  FormSwep();

  //! Class destructor.
  ~FormSwep();

  //! Sets the filter coefficients for a resonance at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate poles with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  The filter zeros are
    placed at z = 1, z = -1, and the coefficients are then normalized to
    produce a constant unity gain (independent of the filter \e gain
    parameter).  The resulting filter frequency response has a
    resonance at the given \e frequency.  The closer the poles are to
    the unit-circle (\e radius close to one), the narrower the
    resulting resonance width.
  */
  void setResonance(MY_FLOAT aFrequency, MY_FLOAT aRadius);

  //! Set both the current and target resonance parameters.
  void setStates(MY_FLOAT aFrequency, MY_FLOAT aRadius, MY_FLOAT aGain = 1.0);

  //! Set target resonance parameters.
  void setTargets(MY_FLOAT aFrequency, MY_FLOAT aRadius, MY_FLOAT aGain = 1.0);

  //! Set the sweep rate (between 0.0 - 1.0).
  /*!
    The formant parameters are varied in increments of the
    sweep rate between their current and target values.
    A sweep rate of 1.0 will produce an immediate change in
    resonance parameters from their current values to the
    target values.  A sweep rate of 0.0 will produce no
    change in resonance parameters.  
  */
  void setSweepRate(MY_FLOAT aRate);    

  //! Set the sweep rate in terms of a time value in seconds.
  /*!
    This method adjusts the sweep rate based on a
    given time for the formant parameters to reach
    their target values.
  */
  void setSweepTime(MY_FLOAT aTime);    

  //! Input one sample to the filter and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

 protected:  
  bool dirty;
  MY_FLOAT frequency;
  MY_FLOAT radius;
  MY_FLOAT startFrequency;
  MY_FLOAT startRadius;
  MY_FLOAT startGain;
  MY_FLOAT targetFrequency;
  MY_FLOAT targetRadius;
  MY_FLOAT targetGain;
  MY_FLOAT deltaFrequency;
  MY_FLOAT deltaRadius;
  MY_FLOAT deltaGain;
  MY_FLOAT sweepState;
  MY_FLOAT sweepRate;

};

#endif
