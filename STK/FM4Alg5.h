/******************************************/
/*  Algorithm 5 (TX81Z) Subclass of       */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/*  This connection topology is 2 simple  */
/*  FM Pairs summed together, like:       */
/*                                        */
/*              1 -> 2 -\                 */
/*                       +-> Out          */
/*              3 -> 4 -/                 */
/*                                        */
/*  Controls: control1 = mod index 1      */
/*            control2 = crossfade of two */
/*                          outputs       */
/*            control3 = LFO speed        */
/*            modWheel = LFO amount       */
/*                                        */
/******************************************/

#if !defined(__FM4Alg5_h)
#define __FM4Alg5_h

#include "FM4Op.h"

class FM4Alg5 : public FM4Op
{
  public:
    FM4Alg5();
    virtual ~FM4Alg5();
    MY_FLOAT tick();
};

#endif
