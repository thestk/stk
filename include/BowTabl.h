/***************************************************/
/*! \class BowTabl
    \brief STK bowed string table class.

    This class implements a simple bowed string
    non-linear function, as described by Smith (1986).

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__BOWTABL_H)
#define __BOWTABL_H

#include "Stk.h"

class BowTabl : public Stk
{
public:
  //! Default constructor.
  BowTabl();

  //! Class destructor.
  ~BowTabl();

  //! Set the table offset value.
  /*!
    The table offset is a bias which controls the
    symmetry of the friction.  If you want the
    friction to vary with direction, use a non-zero
    value for the offset.  The default value is zero.
  */
  void setOffset(MY_FLOAT aValue);

  //! Set the table slope value.
  /*!
   The table slope controls the width of the friction
   pulse, which is related to bow force.
  */
  void setSlope(MY_FLOAT aValue);

  //! Return the last output value.
  MY_FLOAT lastOut(void) const;

  //! Return the function value for \e input.
  /*!
    The function input represents differential
    string-to-bow velocity.
  */
  MY_FLOAT tick(const MY_FLOAT input);

  //! Take \e vectorSize inputs and return the corresponding function values in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

protected:  
  MY_FLOAT offSet;
  MY_FLOAT slope;
  MY_FLOAT lastOutput;

};

#endif
