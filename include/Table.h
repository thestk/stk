/***************************************************/
/*! \class Table
    \brief STK table lookup class.

    This class loads a table of floating-point
    doubles, which are assumed to be in big-endian
    format.  Linear interpolation is performed for
    fractional lookup indexes.

    An StkError will be thrown if the table file
    is not found.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_TABLE_H
#define STK_TABLE_H

#include "Function.h"

class Table : public Function
{
public:
  //! The constructor loads the data from \e fileName.
  /*!
    An StkError will be thrown in the file cannot be found or
    opened.
  */
  Table( std::string fileName );

  //! Class destructor.
  ~Table();

  //! Return the number of elements in the table.
  long getLength() const;

  //! Return the table value at position \e index.
  /*!
    Linear interpolation is performed if \e index is
    fractional.
   */
  StkFloat tick(StkFloat index);

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
  long length_;
  std::valarray<StkFloat> data_;

};

#endif // defined(__TABLE_H)
