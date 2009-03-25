/***************************************************/
/*! \class PercFlut
    \brief STK percussive flute FM synthesis instrument.

    This class implements algorithm 4 of the TX81Z.

    \code
    Algorithm 4 is :   4->3--\
                          2-- + -->1-->Out
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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#ifndef STK_PERCFLUT_H
#define STK_PERCFLUT_H

#include "FM.h"

class PercFlut : public FM
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  PercFlut();

  //! Class destructor.
  ~PercFlut();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(StkFloat frequency);

  //! Start a note with the given frequency and amplitude.
  void noteOn(StkFloat frequency, StkFloat amplitude);

 protected:

  StkFloat computeSample( void );
};

#endif
