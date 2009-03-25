/******************************************/  
/*  Karplus-Strong plucked string model   */
/*  by Perry Cook, 1995-96                */
/*                                        */
/*  There exist at least two patents,     */
/*  assigned to Stanford, bearing the     */
/*  names of Karplus and/or Strong.       */
/******************************************/

#if !defined(__Plucked_h)
#define __Plucked_h

#include "Instrmnt.h" 
#include "DLineA.h" 
#include "OneZero.h" 
#include "OnePole.h" 
#include "Noise.h" 

class Plucked : public Instrmnt
{
  protected:  
    DLineA *delayLine;
    OneZero *loopFilt;
    OnePole *pickFilt;
    Noise *noise;
    long length;
    MY_FLOAT loopGain;
  public:
    Plucked(MY_FLOAT lowestFreq);
    ~Plucked();
    void clear();
    virtual void setFreq(MY_FLOAT frequency);
    void pluck(MY_FLOAT amplitude);
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
    virtual void noteOff(MY_FLOAT amp);
    virtual MY_FLOAT tick();
};

#endif

