/***************************************************/
/*! \class PRCRev
    \brief Perry's simple reverberator class.

    This class is based on some of the famous
    Stanford/CCRMA reverbs (NRev, KipRev), which
    were based on the Chowning/Moorer/Schroeder
    reverberators using networks of simple allpass
    and comb delay filters.  This class implements
    two series allpass units and two parallel comb
    filters.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_PRCREV_H
#define STK_PRCREV_H

#include "Effect.h" 
#include "Delay.h" 

class PRCRev : public Effect
{
public:
  //! Class constructor taking a T60 decay time argument (one second default value).
  PRCRev( StkFloat T60 = 1.0 );

  //! Class destructor.
  ~PRCRev();

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
  Delay allpassDelays_[2];
  Delay combDelays_[2];
  StkFloat allpassCoefficient_;
  StkFloat combCoefficient_[2];

};

#endif

