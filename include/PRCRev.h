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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__PRCREV_H)
#define __PRCREV_H

#include "Reverb.h" 
#include "Delay.h" 

class PRCRev : public Reverb
{
 public:
  // Class constructor taking a T60 decay time argument.
  PRCRev(MY_FLOAT T60);

  // Class destructor.
  ~PRCRev();

  //! Reset and clear all internal state.
  void clear();

  //! Compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

  protected:  
    Delay *allpassDelays[2];
    Delay *combDelays[2];
    MY_FLOAT allpassCoefficient;
    MY_FLOAT combCoefficient[2];

};

#endif

