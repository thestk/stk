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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#ifndef STK_BEETHREE_H
#define STK_BEETHREE_H

#include "FM.h"

class BeeThree : public FM
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  BeeThree();

  //! Class destructor.
  ~BeeThree();

  //! Start a note with the given frequency and amplitude.
  void noteOn(StkFloat frequency, StkFloat amplitude);

 protected:

  StkFloat computeSample( void );
};

#endif
