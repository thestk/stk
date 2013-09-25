/***********************************************/
/*
  NeXT (.snd) and Sun (.au) Soundfile Output Class
  by Perry R. Cook, 1996
  Revised by Gary P. Scavone, 1999-2000

  This one opens a NeXT .snd file, and
  even knows how to byte-swap!
*/
/***********************************************/

#if !defined(__SndWvOut_h)
#define __SndWvOut_h

#include "Object.h"
#include "WvOut.h"

class SndWvOut : public WvOut
{
 protected:
  FILE *fd;
 public:
  SndWvOut(char *fileName, int chans = 1);
  ~SndWvOut();
  void tick(MY_FLOAT sample);
  void mtick(MY_MULTI samples);
};

#endif // defined(__SndWvOut_h)
