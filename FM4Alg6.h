/******************************************/
/*  Algorithm 6 (TX81Z) Subclass of       */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/*  This connection topology is three     */
/*  Carriers and a common Modulator       */
/*                                        */
/*                /->1 -\                 */
/*             4-|-->2 - +-> Out          */
/*                \->3 -/                 */
/*                                        */
/*  Controls: control1 = vowel      	  */
/*            control2 = spectral tilt    */
/*            control3 = LFO speed        */
/*            modWheel = LFO amount       */
/*                                        */
/******************************************/

#if !defined(__FM4Alg6_h)
#define __FM4Alg6_h

#include "FM4Op.h"

class FM4Alg6 : public FM4Op
{
  protected:
    MY_FLOAT tilt[3];
    MY_FLOAT mods[3];
  public:
    FM4Alg6();
    virtual ~FM4Alg6();
    MY_FLOAT tick();
    virtual void controlChange(int number, MY_FLOAT value);
};

#endif
