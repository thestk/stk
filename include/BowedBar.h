/*********************************************/
/*  Bowed Bar model                          */
/*  by Georg Essl, 1999                      */
/*  For details refer to:                    */
/*    G.Essl, P.R.Cook: "Banded Waveguides:  */
/*    Towards Physical Modelling of Bar      */
/*    Percussion Instruments", ICMC'99       */
/*********************************************/

#if !defined(__BowedBar_h)
#define __BowedBar_h

#include "Instrmnt.h"
#include "DLineN.h"
#include "BowTabl.h"
#include "ADSR.h"
#include "BiQuad.h"

class BowedBar : public Instrmnt
{
 protected:     
  BowTabl *bowTabl;
  ADSR *adsr;
  BiQuad *bandpass;
  MY_FLOAT maxVelocity;
  MY_FLOAT modes[4];
  DLineN delay[4];
  int NR_MODES;
  float Zs[4][2];
  MY_FLOAT coeffs[4][2];
  float filtOut[4];
  float filtIn[4];
  MY_FLOAT filtGain[4];
  MY_FLOAT freq;
  int length;
  MY_FLOAT R;
  MY_FLOAT GAIN;
  MY_FLOAT gains[4];
  MY_FLOAT slope;
  MY_FLOAT velinput;
  MY_FLOAT integration_const;
  int trackVel;
  MY_FLOAT bowvel, bowTarg, bowPos, lastBowPos;
  int doPluck;

 public:
  BowedBar();
  ~BowedBar();
  void tuneBandPasses();
  void clear();
  void startBowing(MY_FLOAT amplitude,MY_FLOAT rate);
  void stopBowing(MY_FLOAT rate);
  void pluck(MY_FLOAT amp);
  void setStrikePosition(MY_FLOAT position);
  void noteOn(MY_FLOAT freq, MY_FLOAT amp);
  void noteOff(MY_FLOAT amp);
  void setFreq(MY_FLOAT frequency);
  void controlChange(int number, MY_FLOAT value);
  MY_FLOAT tick();
};

#endif
