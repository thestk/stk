#ifndef STK_SIMPLE_H
#define STK_SIMPLE_H

#include "Instrmnt.h"
#include "ADSR.h"
#include "FileLoop.h"
#include "OnePole.h"
#include "BiQuad.h"
#include "Noise.h"

namespace stk {

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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

class Simple : public Instrmnt
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Simple( void );

  //! Class destructor.
  ~Simple( void );

  //! Clear internal states.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( StkFloat frequency );

  //! Start envelope toward "on" target.
  void keyOn( void );

  //! Start envelope toward "off" target.
  void keyOff( void );

  //! Start a note with the given frequency and amplitude.
  void noteOn( StkFloat frequency, StkFloat amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( StkFloat amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange( int number, StkFloat value );

  //! Compute and return one output sample.
  StkFloat tick( unsigned int channel = 0 );

 protected:

  ADSR      adsr_; 
  FileLoop *loop_;
  OnePole   filter_;
  BiQuad    biquad_;
  Noise     noise_;
  StkFloat  baseFrequency_;
  StkFloat  loopGain_;

};

inline StkFloat Simple :: tick( unsigned int )
{
  lastFrame_[0] = loopGain_ * loop_->tick();
  biquad_.tick( noise_.tick() );
  lastFrame_[0] += (1.0 - loopGain_) * biquad_.lastOut();
  lastFrame_[0] = filter_.tick( lastFrame_[0] );
  lastFrame_[0] *= adsr_.tick();
  return lastFrame_[0];
}

} // stk namespace

#endif
