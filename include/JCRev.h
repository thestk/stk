/***************************************************/
/*! \class JCRev
    \brief John Chowning's reverberator class.

    This class is derived from the CLM JCRev
    function, which is based on the use of
    networks of simple allpass and comb delay
    filters.  This class implements three series
    allpass units, followed by four parallel comb
    filters, and two decorrelation delay lines in
    parallel at the output.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_JCREV_H
#define STK_JCREV_H

#include "Effect.h"
#include "Delay.h" 

class JCRev : public Effect
{
 public:
  //! Class constructor taking a T60 decay time argument (one second default value).
  JCRev( StkFloat T60 = 1.0 );

  //! Class destructor.
  ~JCRev();

  //! Reset and clear all internal state.
  void clear();

  //! Set the reverberation T60 decay time.
  void setT60( StkFloat T60 );

  //! Compute one output sample.
  StkFloat tick(StkFloat input);

  //! Take \e vectorSize inputs, compute the same number of outputs and return them in \e vector.
  StkFloat *tick( StkFloat *vector, unsigned int vectorSize );

  //! Take a channel of the StkFrames object as inputs to the effect and replace with corresponding outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

 protected:
  Delay allpassDelays_[3];
  Delay combDelays_[4];
  Delay outLeftDelay_;
  Delay outRightDelay_;
  StkFloat allpassCoefficient_;
  StkFloat combCoefficient_[4];

};

#endif

