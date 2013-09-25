/******************************************/  
/*  Karplus-Strong Sitar1 string model   */
/*  by Perry Cook, 1995-96                */
/*                                        */
/*  There exist at least two patents,     */
/*  assigned to Stanford, bearing the     */
/*  names of Karplus and/or Strong.       */
/******************************************/

#if !defined(__Sitar1_h)
#define __Sitar1_h

#include "Instrmnt.h" 
#include "DLineA.h"
#include "OneZero.h"
#include "ADSR.h" 
#include "Noise.h" 

class Sitar1 : public Instrmnt
{
protected:  
  DLineA *delayLine;
  OneZero *loopFilt;
  ADSR *envelope;
  Noise *noise;
  long length;
  MY_FLOAT loopGain;
  MY_FLOAT amPluck;
  MY_FLOAT delay;
  MY_FLOAT delayTarg;
public:
  Sitar1(MY_FLOAT lowestFreq);
  ~Sitar1();
  void clear();
  virtual void setFreq(MY_FLOAT frequency);
  void pluck(MY_FLOAT amplitude);
  virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
  virtual void noteOff(MY_FLOAT amp);
  virtual MY_FLOAT tick();
};

#endif

