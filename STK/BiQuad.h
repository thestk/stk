/*******************************************/
/*  BiQuad (2-pole, 2-zero) Filter Class,  */
/*  by Perry R. Cook, 1995-96              */ 
/*  See books on filters to understand     */
/*  more about how this works.  Nothing    */
/*  out of the ordinary in this version.   */
/*******************************************/

#if !defined(__BiQuad_h)
#define __BiQuad_h

#include "Filter.h"

class BiQuad : public Filter
{
  protected:  
    MY_FLOAT poleCoeffs[2];
    MY_FLOAT zeroCoeffs[2];
  public:
    BiQuad();
    ~BiQuad();
    void clear();
    void setPoleCoeffs(MY_FLOAT *coeffs);
    void setZeroCoeffs(MY_FLOAT *coeffs);
    void setGain(MY_FLOAT aValue);
    void setFreqAndReson(MY_FLOAT freq, MY_FLOAT reson);
    void setEqualGainZeroes();
    MY_FLOAT tick(MY_FLOAT sample);
};

#endif
