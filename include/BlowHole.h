/***********************************************/
/*
   Waveguide reed model with a register hole
   and one tonehole

   by Gary P. Scavone, 2000.
*/
/***********************************************/

#if !defined(__BlowHole_h)
#define __BlowHole_h

#include "Instrmnt.h"
#include "DLineL.h"
#include "ReedTabl.h"
#include "OneZero.h"
#include "PoleZero.h"
#include "Envelope.h"
#include "Noise.h"
#include "RawWvIn.h"

class BlowHole : public Instrmnt
{
 protected:  
  DLineL *delays;
  ReedTabl *reedTable;
  OneZero *filter;
  PoleZero *tonehole;
  PoleZero *vent;
  Envelope *envelope;
  Noise *noise;
  RawWvIn *vibr;
  long length;
  MY_FLOAT scatter;
  MY_FLOAT th_coeff;
  MY_FLOAT r_th;
  MY_FLOAT rh_coeff;
  MY_FLOAT rh_gain;
  MY_FLOAT outputGain;
  MY_FLOAT noiseGain;
  MY_FLOAT vibrGain;
 public:
  BlowHole(MY_FLOAT lowestFreq);
  ~BlowHole();
  void clear();
  virtual void setFreq(MY_FLOAT frequency);
  void setTonehole(MY_FLOAT newValue);
  void setVent(MY_FLOAT newValue);
  void startBlowing(MY_FLOAT amplitude,MY_FLOAT rate);
  void stopBlowing(MY_FLOAT rate);
  virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
  virtual void noteOff(MY_FLOAT amp);
  virtual MY_FLOAT tick();
  virtual void controlChange(int number, MY_FLOAT value);
};

#endif
