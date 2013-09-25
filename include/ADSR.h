/*******************************************/
/*  ADSR Subclass of the Envelope Class,   */
/*  by Perry R. Cook, 1995-96              */ 
/*  This is the traditional ADSR (Attack   */
/*  Decay, Sustain, Release) envelope.     */
/*  It responds to simple KeyOn and KeyOff */
/*  messages, keeping track of it's state. */         
/*  There are two tick (update value)      */
/*  methods, one returns the value, and    */
/*  other returns the state (0 = A, 1 = D, */
/*  2 = S, 3 = R)                          */
/*******************************************/

#if !defined(__ADSR_h)
#define __ADSR_h

#include "Envelope.h"

class ADSR : public Envelope
{
 protected:  
  MY_FLOAT attackRate;
  MY_FLOAT decayRate;
  MY_FLOAT sustainLevel;
  MY_FLOAT releaseRate;
 public:
  ADSR();
  ~ADSR();
  void keyOn();
  void keyOff();
  void setAttackRate(MY_FLOAT aRate);
  void setDecayRate(MY_FLOAT aRate);
  void setSustainLevel(MY_FLOAT aLevel);
  void setReleaseRate(MY_FLOAT aRate);
  void setAttackTime(MY_FLOAT aTime);
  void setDecayTime(MY_FLOAT aTime);
  void setReleaseTime(MY_FLOAT aTime);
  void setAllTimes(MY_FLOAT attTime, MY_FLOAT decTime, MY_FLOAT susLevel, MY_FLOAT relTime);
  void setTarget(MY_FLOAT aTarget);
  void setValue(MY_FLOAT aValue);
  MY_FLOAT tick();
  int informTick();  
  MY_FLOAT lastOut();
};

#endif
