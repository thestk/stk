/***************************************************/
/*! \class Noise
    \brief STK noise generator.

    Generic random number generation using the
    C rand() function.  The quality of the rand()
    function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_NOISE_H
#define STK_NOISE_H

#include "Generator.h"

class Noise : public Generator
{
public:

  //! Default constructor which seeds the random number generator with the system time.
  Noise();

  //! Constructor which seeds the random number generator with a given seed.
  /*!
    If the seed value is zero, the random number generator is
    seeded with the system time.
  */
  Noise( unsigned int seed );

  //! Class destructor.
  virtual ~Noise();

  //! Seed the random number generator with a specific seed value.
  /*!
    If no seed is provided or the seed value is zero, the random
    number generator is seeded with the current system time.
  */
  void setSeed( unsigned int seed = 0 );

  //! Return a random number between -1.0 and 1.0 using rand().
  virtual StkFloat tick();

  //! Compute \e vectorSize outputs and return them in \e vector.
  virtual StkFloat *tick( StkFloat *vector, unsigned int vectorSize );

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

protected:

};

#endif
