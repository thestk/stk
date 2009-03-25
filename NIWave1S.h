/*******************************************/
/*  NonInterpolating One-Shot Raw Sound-   */
/*  file Class, by Perry R. Cook, 1995-96  */ 
/*  This Object can open a raw 16bit data  */
/*  (signed integers) file, and play back  */
/*  the data once (again when reset), with */
/*  no interpolation on playback.          */
/*******************************************/

#if !defined(__NIWave1S_h)
#define __NIWave1S_h

#include "Object.h"

class NIWave1S : public Object
{
  protected:  
    long length;
    MY_FLOAT *data;
    MY_FLOAT rate;
    MY_FLOAT time;
    MY_FLOAT lastOutput;
  public:
    NIWave1S(char *fileName);
    void reset();
    void normalize();
    void normalize(MY_FLOAT newPeak);
    void setRate(MY_FLOAT aRate);
    MY_FLOAT tick();
    int informTick();
    MY_FLOAT lastOut();
};

#endif
