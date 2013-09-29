#ifndef STK_BOWED_H
#define STK_BOWED_H

#include "Instrmnt.h"
#include "DelayL.h"
#include "BowTable.h"
#include "OnePole.h"
#include "BiQuad.h"
#include "SineWave.h"
#include "ADSR.h"

namespace stk {

/***************************************************/
/*! \class Bowed
    \brief STK bowed string instrument class.

    This class implements a bowed string model, a
    la Smith (1986), after McIntyre, Schumacher,
    Woodhouse (1983).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Position = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

class Bowed : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Bowed( StkFloat lowestFrequency );

  //! Class destructor.
  ~Bowed( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( StkFloat frequency );

  //! Set vibrato gain.
  void setVibrato( StkFloat gain );

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBowing( StkFloat amplitude, StkFloat rate );

  //! Decrease breath pressure with given rate of decrease.
  void stopBowing( StkFloat rate );

  //! Start a note with the given frequency and amplitude.
  void noteOn( StkFloat frequency, StkFloat amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( StkFloat amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange( int number, StkFloat value );

  //! Compute and return one output sample.
  StkFloat tick( unsigned int channel = 0 );

 protected:

  DelayL   neckDelay_;
  DelayL   bridgeDelay_;
  BowTable bowTable_;
  OnePole  stringFilter_;
  BiQuad   bodyFilter_;
  SineWave vibrato_;
  ADSR     adsr_;
  StkFloat maxVelocity_;
  StkFloat baseDelay_;
  StkFloat vibratoGain_;
  StkFloat betaRatio_;

};

inline StkFloat Bowed :: tick( unsigned int )
{
  StkFloat bowVelocity = maxVelocity_ * adsr_.tick();
  StkFloat bridgeRefl = -stringFilter_.tick( bridgeDelay_.lastOut() );
  StkFloat nutRefl = -neckDelay_.lastOut();
  StkFloat stringVel = bridgeRefl + nutRefl;               // Sum is string velocity
  StkFloat velDiff = bowVelocity - stringVel;              // Differential velocity
  StkFloat newVel = velDiff * bowTable_.tick( velDiff );   // Non-Linear bow function
  neckDelay_.tick(bridgeRefl + newVel);                    // Do string propagations
  bridgeDelay_.tick(nutRefl + newVel);
    
  if ( vibratoGain_ > 0.0 )  {
    neckDelay_.setDelay( (baseDelay_ * (1.0 - betaRatio_) ) + 
                         (baseDelay_ * vibratoGain_ * vibrato_.tick()) );
  }

  lastFrame_[0] = bodyFilter_.tick( bridgeDelay_.lastOut() );

  return lastFrame_[0];
}

} // stk namespace

#endif
