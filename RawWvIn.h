/*******************************************/
/*  NonInterpolating One-Shot Raw Sound-   */
/*  file Class, by Perry R. Cook, 1995-96  */ 
/*  This Object can open a raw 16bit data  */
/*  (signed integers) file, and play back  */
/*  the data once, with no interpolation   */
/*  on playback.  Once finished, it closes */        
/*  the file, the file is reopened with    */
/*  the reset() method.                    */
/*  This is useful for small memory model, */  
/*  applications, or for streaming from    */
/*  disk (and generally non real-time      */
/*  applications).                         */
/*******************************************/

#if !defined(__RawWvIn_h)
#define __RawWvIn_h

#include "Object.h"

class RawWvIn : public Object
{
  protected:  
    long length;
    long lastTime;
    int finished;
    short data;
    char fileNm[128];
    FILE *myFile;
    MY_FLOAT rate;
    MY_FLOAT time;
    MY_FLOAT gain;
    MY_FLOAT lastOutput;
  public:
    RawWvIn(char *fileName);
    ~RawWvIn();
    void reset();
    void normalize();
    void normalize(MY_FLOAT newPeak);
    void setRate(MY_FLOAT aRate);
    void finish();
    MY_FLOAT tick();
    int informTick();
    MY_FLOAT lastOut();
};

#endif
