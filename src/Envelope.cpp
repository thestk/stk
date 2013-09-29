/***************************************************/
/*! \class Envelope
    \brief STK linear line envelope class.

    This class implements a simple linear line envelope generator
    which is capable of ramping to an arbitrary target value by a
    specified \e rate.  It also responds to simple \e keyOn and \e
    keyOff messages, ramping to 1.0 on keyOn and to 0.0 on keyOff.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

#include "Envelope.h"

namespace stk {

Envelope :: Envelope( void ) : Generator()
{    
  target_ = 0.0;
  value_ = 0.0;
  rate_ = 0.001;
  state_ = 0;
  Stk::addSampleRateAlert( this );
}

Envelope :: ~Envelope( void )
{
  Stk::removeSampleRateAlert( this );
}

Envelope& Envelope :: operator= ( const Envelope& e )
{
  if ( this != &e ) {
    target_ = e.target_;
    value_ = e.value_;
    rate_ = e.rate_;
    state_ = e.state_;
  }

  return *this;
}

void Envelope :: sampleRateChanged( StkFloat newRate, StkFloat oldRate )
{
  if ( !ignoreSampleRateChange_ )
    rate_ = oldRate * rate_ / newRate;
}

} // stk namespace
