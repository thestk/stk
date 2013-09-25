/******************************************/
/*  Two oscillator instrument for use     */
/*  with MUS151 Psychoacoustics course.   */
/*  by Gary P. Scavone, 1998              */
/*  CCRMA, Stanford Unviversity           */
/******************************************/

#if !defined(__TwoOsc_h)
#define __TwoOsc_h

#include "../STK/Envelope.h"
#include "../STK/RawWvIn.h"

class TwoOsc : public Object
{
 protected:
  Envelope *envelope;
  RawWvIn *osc[2];
  MY_FLOAT lastOutput;
  MY_FLOAT amps[2];

 public:
  TwoOsc();
  ~TwoOsc();
  virtual void setFreq(int oscnum, MY_FLOAT frequency);
  virtual void setAmp(int oscnum, MY_FLOAT amp);
  virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
  virtual void noteOff(MY_FLOAT amp);
  virtual MY_FLOAT tick();
  virtual void controlChange(int number, MY_FLOAT value);
};

#endif
