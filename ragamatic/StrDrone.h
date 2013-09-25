/******************************************/  
/*  Karplus-Strong StrDrone string model   */
/*  by Perry Cook, 1995-96                */
/*                                        */
/*  There exist at least two patents,     */
/*  assigned to Stanford, bearing the     */
/*  names of Karplus and/or Strong.       */
/******************************************/

#if !defined(__StrDrone_h)
#define __StrDrone_h

#include "../STK/Instrmnt.h" 
#include "../STK/DLineA.h"
#include "../STK/OneZero.h"
#include "../STK/ADSR.h" 
#include "../STK/Noise.h" 

class StrDrone : public Instrmnt
{
protected:  
  DLineA *delayLine;
  ADSR *envelope;
  Noise *noise;
  OneZero *loopFilt;
  long length;
  MY_FLOAT loopGain;
public:
  StrDrone(MY_FLOAT lowestFreq);
  ~StrDrone();
  void clear();
  virtual void setFreq(MY_FLOAT frequency);
  void pluck(MY_FLOAT amplitude);
  virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
  virtual void noteOff(MY_FLOAT amp);
  virtual MY_FLOAT tick();
};

#endif

