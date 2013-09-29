#ifndef STK_BEETHREE_H
#define STK_BEETHREE_H

#include "FM.h"

namespace stk {

/***************************************************/
/*! \class BeeThree
    \brief STK Hammond-oid organ FM synthesis instrument.

    This class implements a simple 4 operator
    topology, also referred to as algorithm 8 of
    the TX81Z.

    \code
    Algorithm 8 is :
                     1 --.
                     2 -\|
                         +-> Out
                     3 -/|
                     4 --
    \endcode

    Control Change Numbers: 
       - Operator 4 (feedback) Gain = 2
       - Operator 3 Gain = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

class BeeThree : public FM
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  BeeThree( void );

  //! Class destructor.
  ~BeeThree( void );

  //! Start a note with the given frequency and amplitude.
  void noteOn( StkFloat frequency, StkFloat amplitude );

  //! Compute and return one output sample.
  StkFloat tick( unsigned int channel = 0 );

 protected:

};

inline StkFloat BeeThree :: tick( unsigned int )
{
  register StkFloat temp;

  if ( modDepth_ > 0.0 )	{
    temp = 1.0 + ( modDepth_ * vibrato_.tick() * 0.1 );
    waves_[0]->setFrequency( baseFrequency_ * temp * ratios_[0] );
    waves_[1]->setFrequency( baseFrequency_ * temp * ratios_[1] );
    waves_[2]->setFrequency( baseFrequency_ * temp * ratios_[2] );
    waves_[3]->setFrequency( baseFrequency_ * temp * ratios_[3] );
  }

  waves_[3]->addPhaseOffset( twozero_.lastOut() );
  temp = control1_ * 2.0 * gains_[3] * adsr_[3]->tick() * waves_[3]->tick();
  twozero_.tick( temp );

  temp += control2_ * 2.0 * gains_[2] * adsr_[2]->tick() * waves_[2]->tick();
  temp += gains_[1] * adsr_[1]->tick() * waves_[1]->tick();
  temp += gains_[0] * adsr_[0]->tick() * waves_[0]->tick();

  lastFrame_[0] = temp * 0.125;
  return lastFrame_[0];
}

} // stk namespace

#endif
