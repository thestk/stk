/********************************************/
/*  Commuted Mandolin Subclass of enhanced  */
/*  dual plucked-string model               */
/*  by Perry Cook, 1995-96                  */
/*   Controls:    CONTROL1 = bodySize       */
/*                CONTROL2 = pluckPosition  */
/*                CONTROL3 = loopGain       */
/*                MOD_WHEEL= deTuning       */
/*                                          */
/*  Note: Commuted Synthesis, as with many  */
/*  other WaveGuide techniques, is covered  */
/*  by patents, granted, pending, and/or    */
/*  applied-for.  All are assigned to the   */
/*  Board of Trustees, Stanford University. */
/*  For information, contact the Office of  */ 
/*  Technology Licensing, Stanford U.       */
/********************************************/  

#if !defined(__Mandolin_h)
#define __Mandolin_h

#include "Plucked2.h"
#include "RawWvIn.h"

class Mandolin : public Plucked2
{
  protected:  
    RawWvIn *soundfile[12];
    MY_FLOAT directBody;
    int mic;
    long dampTime;
    int waveDone;
  public:
    Mandolin(MY_FLOAT lowestFreq);
    virtual ~Mandolin();
    void pluck(MY_FLOAT amplitude);
    void pluck(MY_FLOAT amplitude,MY_FLOAT position);
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
    void setBodySize(MY_FLOAT size);
    virtual void controlChange(int number, MY_FLOAT value);
    virtual MY_FLOAT tick();
};

#endif
