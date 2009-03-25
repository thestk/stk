/***************************************************/
/*! \class FormSwep
    \brief STK sweepable formant filter class.

    This public BiQuad filter subclass implements
    a formant (resonance) which can be "swept"
    over time from one frequency setting to another.
    It provides methods for controlling the sweep
    rate and target frequency.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#ifndef STK_FORMSWEP_H
#define STK_FORMSWEP_H

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
  void setResonance(StkFloat frequency, StkFloat radius);

  //! Set both the current and target resonance parameters.
  void setStates(StkFloat frequency, StkFloat radius, StkFloat gain = 1.0);

  //! Set target resonance parameters.
  void setTargets(StkFloat frequency, StkFloat radius, StkFloat gain = 1.0);

  //! Set the sweep rate (between 0.0 - 1.0).
  /*!
    The formant parameters are varied in increments of the
    sweep rate between their current and target values.
    A sweep rate of 1.0 will produce an immediate change in
    resonance parameters from their current values to the
    target values.  A sweep rate of 0.0 will produce no
    change in resonance parameters.  
  */
  void setSweepRate(StkFloat rate);

  //! Set the sweep rate in terms of a time value in seconds.
  /*!
    This method adjusts the sweep rate based on a
    given time for the formant parameters to reach
    their target values.
  */
  void setSweepTime(StkFloat time);

 protected:

  StkFloat computeSample( StkFloat input );

  bool dirty_;
  StkFloat frequency_;
  StkFloat radius_;
  StkFloat startFrequency_;
  StkFloat startRadius_;
  StkFloat startGain_;
  StkFloat targetFrequency_;
  StkFloat targetRadius_;
  StkFloat targetGain_;
  StkFloat deltaFrequency_;
  StkFloat deltaRadius_;
  StkFloat deltaGain_;
  StkFloat sweepState_;
  StkFloat sweepRate_;

};

#endif
