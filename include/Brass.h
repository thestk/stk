#ifndef STK_BRASS_H
#define STK_BRASS_H

#include "Instrmnt.h"
#include "DelayA.h"
#include "BiQuad.h"
#include "PoleZero.h"
#include "ADSR.h"
#include "SineWave.h"

namespace stk {

/***************************************************/
/*! \class Brass
    \brief STK simple brass instrument class.

    This class implements a simple brass instrument
    waveguide model, a la Cook (TBone, HosePlayer).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Lip Tension = 2
       - Slide Length = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

class Brass: public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Brass( StkFloat lowestFrequency );

  //! Class destructor.
  ~Brass(  );

  //! Reset and clear all internal state.
  void clear(  );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( StkFloat frequency );

  //! Set the lips frequency.
  void setLip( StkFloat frequency );

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBlowing( StkFloat amplitude, StkFloat rate );

  //! Decrease breath pressure with given rate of decrease.
  void stopBlowing( StkFloat rate );

  //! Start a note with the given frequency and amplitude.
  void noteOn( StkFloat frequency, StkFloat amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( StkFloat amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange( int number, StkFloat value );

  //! Compute and return one output sample.
  StkFloat tick( unsigned int channel = 0 );

 protected:

  DelayA   delayLine_;
  BiQuad   lipFilter_;
  PoleZero dcBlock_;
  ADSR     adsr_;
  SineWave vibrato_;
  unsigned long length_;
  StkFloat lipTarget_;
  StkFloat slideTarget_;
  StkFloat vibratoGain_;
  StkFloat maxPressure_;

};

inline StkFloat Brass :: tick( unsigned int )
{
  StkFloat breathPressure = maxPressure_ * adsr_.tick();
  breathPressure += vibratoGain_ * vibrato_.tick();

  StkFloat mouthPressure = 0.3 * breathPressure;
  StkFloat borePressure = 0.85 * delayLine_.lastOut();
  StkFloat deltaPressure = mouthPressure - borePressure; // Differential pressure.
  deltaPressure = lipFilter_.tick( deltaPressure );      // Force - > position.
  deltaPressure *= deltaPressure;                        // Basic position to area mapping.
  if ( deltaPressure > 1.0 ) deltaPressure = 1.0;        // Non-linear saturation.

  // The following input scattering assumes the mouthPressure = area.
  lastFrame_[0] = deltaPressure * mouthPressure + ( 1.0 - deltaPressure) * borePressure;
  lastFrame_[0] = delayLine_.tick( dcBlock_.tick( lastFrame_[0] ) );

  return lastFrame_[0];
}

} // stk namespace

#endif
