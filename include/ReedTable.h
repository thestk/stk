/***************************************************/
/*! \class ReedTable
    \brief STK reed table class.

    This class implements a simple one breakpoint,
    non-linear reed function, as described by
    Smith (1986).  This function is based on a
    memoryless non-linear spring model of the reed
    (the reed mass is ignored) which saturates when
    the reed collides with the mouthpiece facing.

    See McIntyre, Schumacher, & Woodhouse (1983),
    Smith (1986), Hirschman, Cook, Scavone, and
    others for more information.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_REEDTABLE_H
#define STK_REEDTABLE_H

#include "Function.h"

class ReedTable : public Function
{
public:
  //! Default constructor.
  ReedTable();

  //! Class destructor.
  ~ReedTable();

  //! Set the table offset value.
  /*!
    The table offset roughly corresponds to the size
    of the initial reed tip opening (a greater offset
    represents a smaller opening).
  */
  void setOffset(StkFloat offset);

  //! Set the table slope value.
  /*!
   The table slope roughly corresponds to the reed
   stiffness (a greater slope represents a harder
   reed).
  */
  void setSlope(StkFloat slope);

  //! Return the function value for \e input.
  /*!
    The function input represents the differential
    pressure across the reeds.
  */
  StkFloat tick(StkFloat input);

  //! Take \e vectorSize inputs from \e vector and replace them with corresponding outputs.
  StkFloat *tick( StkFloat *vector, unsigned int vectorSize );

  //! Take a channel of the StkFrames object as inputs to the function and replace with corresponding outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

protected:  
  StkFloat offset_;
  StkFloat slope_;

};

#endif
