/***************************************************/
/*! \class PluckTwo
    \brief STK enhanced plucked string model class.

    This class implements an enhanced two-string,
    plucked physical model, a la Jaffe-Smith,
    Smith, and others.

    PluckTwo is an abstract class, with no excitation
    specified.  Therefore, it can't be directly
    instantiated.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__PLUCKTWO_H)
#define __PLUCKTWO_H

#include "Instrmnt.h"
#include "DelayL.h"
#include "DelayA.h"
#include "OneZero.h"

class PluckTwo : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  PluckTwo(MY_FLOAT lowestFrequency);

  //! Class destructor.
  virtual ~PluckTwo();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Detune the two strings by the given factor.  A value of 1.0 produces unison strings.
  void setDetune(MY_FLOAT detune);

  //! Efficient combined setting of frequency and detuning.
  void setFreqAndDetune(MY_FLOAT frequency, MY_FLOAT detune);

  //! Set the pluck or "excitation" position along the string (0.0 - 1.0).
  void setPluckPosition(MY_FLOAT position);

  //! Set the base loop gain.
  /*!
    The actual loop gain is set according to the frequency.
    Because of high-frequency loop filter roll-off, higher
    frequency settings have greater loop gains.
  */
  void setBaseLoopGain(MY_FLOAT aGain);

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(MY_FLOAT amplitude);

  //! Virtual (abstract) tick function is implemented by subclasses.
  virtual MY_FLOAT tick() = 0;

  protected:  
    DelayA *delayLine;
    DelayA *delayLine2;
    DelayL *combDelay;
    OneZero *filter;
    OneZero *filter2;
    long length;
    MY_FLOAT loopGain;
    MY_FLOAT baseLoopGain;
    MY_FLOAT lastFrequency;
    MY_FLOAT lastLength;
    MY_FLOAT detuning;
    MY_FLOAT pluckAmplitude;
    MY_FLOAT pluckPosition;

};

#endif
