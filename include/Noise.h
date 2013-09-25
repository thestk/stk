/***************************************************/
/*! \class Noise
    \brief STK noise generator.

    Generic random number generation using the
    C rand() function.  The quality of the rand()
    function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__NOISE_H)
#define __NOISE_H

#include "Stk.h"

class Noise : public Stk
{
public:

  //! Default constructor.
  Noise();

  //! Class destructor.
  virtual ~Noise();

  //! Return a random number between -1.0 and 1.0 using rand().
  virtual MY_FLOAT tick();

  //! Return \e vectorSize random numbers between -1.0 and 1.0 in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

  //! Return the last computed value.
  MY_FLOAT lastOut() const;

protected:  

  MY_FLOAT lastOutput;

};

#endif
