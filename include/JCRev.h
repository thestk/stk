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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__JCREV_H)
#define __JCREV_H

#include "Reverb.h"
#include "Delay.h" 

class JCRev : public Reverb
{
 public:
  //! Class constructor taking a T60 decay time argument.
  JCRev(MY_FLOAT T60);

  //! Class destructor.
  ~JCRev();

  //! Reset and clear all internal state.
  void clear();

  //! Compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

 protected:
  Delay *allpassDelays[3];
  Delay *combDelays[4];
  Delay *outLeftDelay;
  Delay *outRightDelay;
  MY_FLOAT allpassCoefficient;
  MY_FLOAT combCoefficient[4];

};

#endif

