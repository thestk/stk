/*******************************************/
/*  Sweepable Formant (2-pole)             */
/*  Filter Class, by Perry R. Cook, 1995-96*/ 
/*  See books on filters to understand     */
/*  more about how this works.  Nothing    */
/*  out of the ordinary in this version.   */
/*******************************************/

#if !defined(__FormSwep_h)
#define __FormSwep_h

#include "Filter.h"

class FormSwep : public Filter
{
  protected:  
    MY_FLOAT poleCoeffs[2];
    MY_FLOAT freq;
    MY_FLOAT reson;
    int dirty;
    MY_FLOAT targetFreq;
    MY_FLOAT targetReson;
    MY_FLOAT targetGain;
    MY_FLOAT currentFreq;
    MY_FLOAT currentReson;
    MY_FLOAT currentGain;
    MY_FLOAT deltaFreq;
    MY_FLOAT deltaReson;
    MY_FLOAT deltaGain;
    MY_FLOAT sweepState;
    MY_FLOAT sweepRate;
  public:
    FormSwep();
    ~FormSwep();
    void clear();
    void setPoleCoeffs(MY_FLOAT *coeffs);
    void setGain(MY_FLOAT aValue);
    void setFreqAndReson(MY_FLOAT aFreq, MY_FLOAT aReson);
    void setStates(MY_FLOAT aFreq, MY_FLOAT aReson, MY_FLOAT aGain);
    void setTargets(MY_FLOAT aFreq, MY_FLOAT aReson, MY_FLOAT aGain);
    void setSweepRate(MY_FLOAT aRate);    
    void setSweepTime(MY_FLOAT aTime);    
    MY_FLOAT tick(MY_FLOAT sample);
};

#endif
