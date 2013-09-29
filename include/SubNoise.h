/***************************************************/
/*! \class SubNoise
    \brief STK sub-sampled noise generator.

    Generates a new random number every "rate" ticks
    using the C rand() function.  The quality of the
    rand() function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_SUBNOISE_H
#define STK_SUBNOISE_H

#include "Noise.h"

class SubNoise : public Noise
{
 public:

  //! Default constructor sets sub-sample rate to 16.
  SubNoise(int subRate = 16);

  //! Class destructor.
  ~SubNoise();

  //! Return the current sub-sampling rate.
  int subRate(void) const;

  //! Set the sub-sampling rate.
  void setRate(int subRate);

  //! Return a sub-sampled random number between -1.0 and 1.0.
  StkFloat tick();

  //! Compute \e vectorSize outputs and return them in \e vector.
  StkFloat *tick( StkFloat *vector, unsigned int vectorSize );

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

 protected:  
  int counter_;
  int rate_;

};

#endif
