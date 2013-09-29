#ifndef STK_HEVYMETL_H
#define STK_HEVYMETL_H

#include "FM.h"

namespace stk {

/***************************************************/
/*! \class HevyMetl
    \brief STK heavy metal FM synthesis instrument.

    This class implements 3 cascade operators with
    feedback modulation, also referred to as
    algorithm 3 of the TX81Z.

    \code
    Algorithm 3 is :    4--\
                    3-->2-- + -->1-->Out
    \endcode

    Control Change Numbers: 
       - Total Modulator Index = 2
       - Modulator Crossfade = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

class HevyMetl : public FM
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  HevyMetl( void );

  //! Class destructor.
  ~HevyMetl( void );

  //! Start a note with the given frequency and amplitude.
  void noteOn( StkFloat frequency, StkFloat amplitude );

  //! Compute and return one output sample.
  StkFloat tick( unsigned int channel = 0 );

 protected:

};

inline StkFloat HevyMetl :: tick( unsigned int )
{
  register StkFloat temp;

  temp = vibrato_.tick() * modDepth_ * 0.2;    
  waves_[0]->setFrequency(baseFrequency_ * (1.0 + temp) * ratios_[0]);
  waves_[1]->setFrequency(baseFrequency_ * (1.0 + temp) * ratios_[1]);
  waves_[2]->setFrequency(baseFrequency_ * (1.0 + temp) * ratios_[2]);
  waves_[3]->setFrequency(baseFrequency_ * (1.0 + temp) * ratios_[3]);
    
  temp = gains_[2] * adsr_[2]->tick() * waves_[2]->tick();
  waves_[1]->addPhaseOffset( temp );
    
  waves_[3]->addPhaseOffset( twozero_.lastOut() );
  temp = (1.0 - (control2_ * 0.5)) * gains_[3] * adsr_[3]->tick() * waves_[3]->tick();
  twozero_.tick(temp);
    
  temp += control2_ * 0.5 * gains_[1] * adsr_[1]->tick() * waves_[1]->tick();
  temp = temp * control1_;
    
  waves_[0]->addPhaseOffset( temp );
  temp = gains_[0] * adsr_[0]->tick() * waves_[0]->tick();
    
  lastFrame_[0] = temp * 0.5;
  return lastFrame_[0];
}

} // stk namespace

#endif
