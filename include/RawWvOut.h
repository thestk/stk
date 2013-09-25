/*******************************************/
/*  RawWvOut Output Class                  */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  This object spits samples into a raw   */
/*  16-bit data (signed integer) file.     */
/*                                         */
/*  STK RawWave files are assumed to be    */
/*  monaural and big-endian.               */
/*******************************************/

#if !defined(__RawWvOut_h)
#define __RawWvOut_h

#include "Object.h"
#include "WvOut.h"

class RawWvOut : public WvOut
{
 protected:  
  FILE *fd;
 public:
  RawWvOut(char *fileName, int chans = 1);
  ~RawWvOut();
  void tick(MY_FLOAT sample);
  void mtick(MY_MULTI samples);
};

#endif // defined(__RawWvOut_h)
