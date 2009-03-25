/*******************************************/
/*  Two Pole Filter Class,                 */
/*  by Perry R. Cook, 1995-96              */ 
/*  See books on filters to understand     */
/*  more about how this works.  Nothing    */
/*  out of the ordinary in this version.   */
/*******************************************/

#if !defined(__TwoPole_h)
#define __TwoPole_h

#include "Filter.h"

class TwoPole : public Filter
{
  protected:  
    MY_FLOAT poleCoeffs[2];
  public:
    TwoPole();
    ~TwoPole();
    void clear();
    void setPoleCoeffs(MY_FLOAT *coeffs);
    void setGain(MY_FLOAT aValue);
    void setFreqAndReson(MY_FLOAT freq, MY_FLOAT reson);
    MY_FLOAT tick(MY_FLOAT sample);
};

#endif
