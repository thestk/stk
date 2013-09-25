/********************************************/  
/*  MandPlyr Player Expert Object to control*/
/*  commuted dual plucked-string model      */
/*  by Perry Cook, 1995-96                  */
/********************************************/  

#if !defined(__MandPlyr_h)
#define __MandPlyr_h

#include "Instrmnt.h"
#include "VoicMang.h"
#include "Noise.h"

#define NUM_STRINGS 4

class MandPlyr : public Instrmnt
{
protected:  
  VoicMang *strings;
  Noise *noise;
  short strumming;
  long strumRate;
  long strumCount;
  MY_FLOAT skill;
  short    nums[NUM_STRINGS];  // For Now Integer Note Nums
  MY_FLOAT amps[NUM_STRINGS];
  long     tags[NUM_STRINGS];
public:
  MandPlyr();
  ~MandPlyr();
  virtual void noteOnN(short num, MY_FLOAT amp);
  virtual void noteOffN(short num, MY_FLOAT amp);
  virtual MY_FLOAT tick();
  MY_FLOAT special_tick();
  virtual void controlChange(int number, MY_FLOAT value);
  virtual void playChord(MY_FLOAT amp,  char* chordString);
};

#endif


