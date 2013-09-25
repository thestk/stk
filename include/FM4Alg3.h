/******************************************/
/*  Algorithm 3 (TX81Z) Subclass of       */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/*                                        */
/*  Alg 3 is :          4--\              */
/*                  3-->2-- + -->1-->Out  */
/*                                        */
/*  Controls: control1 = total mod index  */
/*            control2 = crossfade of two */
/*                          modulators    */
/*            control3 = LFO speed        */
/*            modWheel = LFO amount       */
/*                                        */
/******************************************/

#if !defined(__FM4Alg3_h)
#define __FM4Alg3_h

#include "FM4Op.h"

class FM4Alg3 : public FM4Op
{
  public:
    FM4Alg3();
    virtual ~FM4Alg3();
    MY_FLOAT tick();
};

#endif
