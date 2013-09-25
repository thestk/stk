/***************************************************/
/*! \class StifKarp
    \brief STK plucked stiff string instrument.

    This class implements a simple plucked string
    algorithm (Karplus Strong) with enhancements
    (Jaffe-Smith, Smith, and others), including
    string stiffness and pluck position controls.
    The stiffness is modeled with allpass filters.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers:
       - Pickup Position = 4
       - String Sustain = 11
       - String Stretch = 1

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__StifKarp_h)
#define __StifKarp_h

#include "Instrmnt.h"
#include "DelayL.h"
#include "DelayA.h"
#include "OneZero.h"
#include "Noise.h"
#include "BiQuad.h"

class StifKarp : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  StifKarp(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~StifKarp();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set the stretch "factor" of the string (0.0 - 1.0).
  void setStretch(MY_FLOAT stretch);

  //! Set the pluck or "excitation" position along the string (0.0 - 1.0).
  void setPickupPosition(MY_FLOAT position);

  //! Set the base loop gain.
  /*!
    The actual loop gain is set according to the frequency.
    Because of high-frequency loop filter roll-off, higher
    frequency settings have greater loop gains.
  */
  void setBaseLoopGain(MY_FLOAT aGain);

  //! Pluck the string with the given amplitude using the current frequency.
  void pluck(MY_FLOAT amplitude);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 protected:  
    DelayA *delayLine;
    DelayL *combDelay;
    OneZero *filter;
    Noise *noise;
    BiQuad *biQuad[4];
    long length;
    MY_FLOAT loopGain;
    MY_FLOAT baseLoopGain;
    MY_FLOAT lastFrequency;
    MY_FLOAT lastLength;
    MY_FLOAT stretching;
    MY_FLOAT pluckAmplitude;
    MY_FLOAT pickupPosition;

};

#endif
