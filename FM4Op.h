/*******************************************/
/*  Master Class for 4 Operator FM Synth   */
/*  by Perry R. Cook, 1995-96              */ 
/*  This instrument contains an 4 waves,   */
/*  4 envelopes, and various state vars.   */
/*                                         */
/*  The basic Chowning/Stanford FM patent  */
/*  expired April 1995, but there exist    */
/*  follow-on patents, mostly assigned to  */
/*  Yamaha.  If you are of the type who    */
/*  should worry about this (making money) */
/*  worry away.                            */
/*                                         */
/*******************************************/

#if !defined(__FM4Op_h)
#define __FM4Op_h

#include "Instrmnt.h"
#include "ADSR.h"
#include "RawLoop.h"
#include "TwoZero.h"

class FM4Op : public Instrmnt
{
  protected:  
    ADSR    *adsr[4]; 
    RawLoop *waves[4];
    RawLoop *vibWave;
    TwoZero *twozero;
    MY_FLOAT baseFreq;
    MY_FLOAT ratios[4];
    MY_FLOAT gains[4];
    MY_FLOAT modDepth;
    MY_FLOAT control1;
    MY_FLOAT control2;
    MY_FLOAT __FM4Op_gains[100];
    MY_FLOAT __FM4Op_susLevels[16];
    MY_FLOAT __FM4Op_attTimes[32];
  public:
    FM4Op();
    virtual ~FM4Op();
    void loadWaves(char* wave1, char* wave2, char* wave3, char* wave4);
    void clear();
    void setFreq(MY_FLOAT frequency);
    void setRatio(int whichOne, MY_FLOAT ratio);
    void setGain(int whichOne, MY_FLOAT gain);
    void keyOn();
    void keyOff();
    void noteOff(MY_FLOAT amp);
    /*  There's no tick() method here, because that depends on the algorithm  */
    void setModulationSpeed(MY_FLOAT mSpeed);
    void setModulationDepth(MY_FLOAT mDepth);
    void setControl1(MY_FLOAT cVal);
    void setControl2(MY_FLOAT cVal);
    virtual void controlChange(int number, MY_FLOAT value);
};

#endif
