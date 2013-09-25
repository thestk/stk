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

#include "Object.h"
#include "WvOut.h"

#if !defined(__RawWvOut_h)
#define __RawWvOut_h

#define RAW_BUFFER_SIZE 1024

class RawWvOut : public WvOut
{
 protected:  
  FILE *fd;
  INT16 data[RAW_BUFFER_SIZE];
  long counter;
  long totalCount;
  int channels;
 public:
  RawWvOut(char *fileName);
  RawWvOut(int chans, char *fileName);
  ~RawWvOut();
  long getCounter();
  MY_FLOAT getTime();
  void tick(MY_FLOAT sample);
  void mtick(MY_MULTI samples);
};

#endif // defined(__RawWvOut_h)
