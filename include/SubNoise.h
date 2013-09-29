/***************************************************/
/*! \class SubNoise
    \brief STK sub-sampled noise generator.

    Generates a new random number every "rate" ticks
    using the C rand() function.  The quality of the
    rand() function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SUBNOISE_H)
#define __SUBNOISE_H

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
  MY_FLOAT tick();

 protected:  
  int counter;
  int rate;

};

#endif
