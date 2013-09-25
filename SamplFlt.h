/*******************************************/
/*  Swept Filter SubClass of Sampling      */
/*  Synthesizer,  by Perry R. Cook, 1995-96*/ 
/*  This instrument inherits up to 5       */
/*  attack waves, 5 looped waves, an ADSR  */
/*  envelope, and adds a 4 pole swept      */
/*  filter.                                */
/*******************************************/

#if !defined(__SamplFlt_h)
#define __SamplFlt_h

#include "Sampler.h"
#include "FormSwep.h"
#include "TwoZero.h"

class SamplFlt : public Sampler
{
  protected:  
    FormSwep *filters[2];
    TwoZero  *twozeroes[2];
  public:
    SamplFlt();
    virtual ~SamplFlt();
    virtual MY_FLOAT tick();
    virtual void controlChange(int number, MY_FLOAT value);
};

#endif
