/***************************************************/
/*! \class NRev
    \brief CCRMA's NRev reverberator class.

    This class is derived from the CLM NRev
    function, which is based on the use of
    networks of simple allpass and comb delay
    filters.  This particular arrangement consists
    of 6 comb filters in parallel, followed by 3
    allpass filters, a lowpass filter, and another
    allpass in series, followed by two allpass
    filters in parallel with corresponding right
    and left outputs.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__NREV_H)
#define __NREV_H

#include "Reverb.h" 
#include "Delay.h" 

class NRev : public Reverb
{
 public:
  // Class constructor taking a T60 decay time argument.
  NRev(MY_FLOAT T60);

  // Class destructor.
  ~NRev();

  //! Reset and clear all internal state.
  void clear();

  //! Compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

 protected:  
  Delay *allpassDelays[8];
  Delay *combDelays[6];
  MY_FLOAT allpassCoefficient;
  MY_FLOAT combCoefficient[6];
	MY_FLOAT lowpassState;

};

#endif

