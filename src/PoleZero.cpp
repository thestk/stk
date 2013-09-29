/***************************************************/
/*! \class PoleZero
    \brief STK one-pole, one-zero filter class.

    This class implements a one-pole, one-zero digital filter.  A
    method is provided for creating an allpass filter with a given
    coefficient.  Another method is provided to create a DC blocking
    filter.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

#include "PoleZero.h"

namespace stk {

PoleZero :: PoleZero()
{
  // Default setting for pass-through.
  b_.resize( 2, 0.0 );
  a_.resize( 2, 0.0 );
  b_[0] = 1.0;
  a_[0] = 1.0;
  inputs_.resize( 2, 1, 0.0 );
  outputs_.resize( 2, 1, 0.0 );
}

PoleZero :: ~PoleZero()
{
}

void PoleZero :: setCoefficients( StkFloat b0, StkFloat b1, StkFloat a1, bool clearState )
{
  b_[0] = b0;
  b_[1] = b1;
  a_[1] = a1;

  if ( clearState ) this->clear();
}

void PoleZero :: setAllpass( StkFloat coefficient )
{
  b_[0] = coefficient;
  b_[1] = 1.0;
  a_[0] = 1.0; // just in case
  a_[1] = coefficient;
}

void PoleZero :: setBlockZero( StkFloat thePole )
{
  b_[0] = 1.0;
  b_[1] = -1.0;
  a_[0] = 1.0; // just in case
  a_[1] = -thePole;
}

} // stk namespace
