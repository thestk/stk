/*******************************************/
/*  NeXT Soundfile Output Class            */
/*  by Perry R. Cook, 1996                 */
/*  revised by Gary P. Scavone, 1999       */
/*                                         */
/*  This one opens a NeXT .snd file, and   */
/*  even knows how to byte-swap!           */
/*******************************************/

#include "Object.h"
#include "WvOut.h"

#if !defined(__SndWvOut_h)
#define __SndWvOut_h

#define SND_BUFFER_SIZE 1024

class SndWvOut : public WvOut
{
 protected:  
  FILE *fd;
  INT16 data[SND_BUFFER_SIZE];
  long counter;
  long totalCount;
  int channels;
 public:
  SndWvOut(char *fileName);
  SndWvOut(int chans, char *fileName);
  ~SndWvOut();
  long getCounter();
  MY_FLOAT getTime();
  void tick(MY_FLOAT sample);
  void mtick(MY_MULTI samples);
};

#endif // defined(__SndWvOut_h)
