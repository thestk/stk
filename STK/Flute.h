/******************************************/
/*  WaveGuide Flute ala Karjalainen,      */
/*  Smith, Waryznyk, etc.                 */
/*  with polynomial Jet ala Cook          */
/*  by Perry Cook, 1995-96                */
/*                                        */
/*  This is a waveguide model, and thus   */
/*  relates to various Stanford Univ.     */
/*  and possibly Yamaha and other patents.*/
/*                                        */
/*   Controls:    CONTROL1 = jetDelay     */
/*                CONTROL2 = noiseGain    */
/*                CONTROL3 = vibFreq      */
/*                MOD_WHEEL= vibAmt       */
/******************************************/

#if !defined(__Flute_h)
#define __Flute_h

#include "Instrmnt.h"
#include "JetTabl.h"
#include "DLineL.h"
#include "OnePole.h"
#include "DCBlock.h"
#include "Noise.h"
#include "ADSR.h"
#include "RawWvIn.h"

class Flute : public Instrmnt
{
 protected:  
  DLineL *jetDelay;
  DLineL *boreDelay;
  JetTabl *jetTable;
  OnePole *filter;
  DCBlock *dcBlock;
  Noise *noise;
  ADSR *adsr;
  RawWvIn *vibr;
  MY_FLOAT lastFreq;
  MY_FLOAT maxPressure;
  MY_FLOAT jetRefl;
  MY_FLOAT endRefl;
  MY_FLOAT noiseGain;
  MY_FLOAT vibrGain;
  MY_FLOAT outputGain;
  MY_FLOAT jetRatio;
 public:
  Flute(MY_FLOAT lowestFreq);
  ~Flute();
  void clear();
  void startBlowing(MY_FLOAT amplitude,MY_FLOAT rate);
  void stopBlowing(MY_FLOAT rate);
  virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
  virtual void noteOff(MY_FLOAT amp);
  void setJetRefl(MY_FLOAT refl);
  void setEndRefl(MY_FLOAT refl);
  virtual void setFreq(MY_FLOAT frequency);
  virtual MY_FLOAT tick();
  virtual void controlChange(int number, MY_FLOAT value);
  void setJetDelay(MY_FLOAT aLength);
};

#endif
