/********************************************/
/*  Reverb Abstract Class,                  */
/*  by Tim Stilson, 1998                    */
/*                                          */
/*  Integrated into STK by Gary Scavone     */
/*  with T60 argument.                      */
/********************************************/

#include "Object.h"

#if !defined(__Reverb_h)
#define __Reverb_h

class Reverb : public Object
{
  public:
	Reverb();
    virtual MY_FLOAT tick(MY_FLOAT sample);
    virtual void setEffectMix(MY_FLOAT mix);
	int isprime(int val);
};

#endif // defined(__Reverb_h)

/* CLM also had JLRev and JLLRev variations on the JCRev: JLRev had
   longer combs and alpasses, JLLRev further placed the comb coefs
   closer to 1.0.  In my modified testMono.cpp, I allowed for a
   "JLRev" argument, though JLRev.cpp/.h doesn't exist, testMono
   simply uses a JCRev but passes a longer base comb length.  I also
   have comments in JCRev.cpp for the JLLRev coefs. 
*/
