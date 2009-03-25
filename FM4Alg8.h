/******************************************/
/*  Algorithm 8 (TX81Z) Subclass of       */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/*  This connection topology is simple    */
/*  Additive Synthesis, like:             */
/*                                        */
/*                   1 --.                */
/*                   2 -\|                */
/*                       +-> Out          */
/*                   3 -/|                */
/*                   4 --                 */
/*                                        */
/*  Controls: control1 = op4 (fb) gain    */
/*            control2 = op3 gain         */
/*            control3 = LFO speed        */
/*            modWheel = LFO amount       */
/*                                        */
/******************************************/

#if !defined(__FM4Alg8_h)
#define __FM4Alg8_h

#include "FM4Op.h"

class FM4Alg8 : public FM4Op
{
  public:
    FM4Alg8();
    virtual MY_FLOAT tick();
};

#endif
