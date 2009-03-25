/***************************************************/
/*! \class Table
    \brief STK table lookup class.

    This class loads a table of floating-point
    doubles, which are assumed to be in big-endian
    format.  Linear interpolation is performed for
    fractional lookup indexes.

    An StkError will be thrown if the table file
    is not found.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__TABLE_H)
#define __TABLE_H

#include "Stk.h"

class Table : public Stk
{
public:
  //! Constructor loads the data from \e fileName.
  Table(char *fileName);

  //! Class destructor.
  ~Table();

  //! Return the number of elements in the table.
  long getLength() const;

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the table value at position \e index.
  /*!
    Linear interpolation is performed if \e index is
    fractional.
   */
  MY_FLOAT tick(MY_FLOAT index);

  //! Take \e vectorSize index positions and return the corresponding table values in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

protected:  
  long length;
  MY_FLOAT *data;
  MY_FLOAT lastOutput;

};

#endif // defined(__TABLE_H)
