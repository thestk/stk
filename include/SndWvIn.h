/*******************************************/
/*  SndWvIn Input Class,                   */
/*  by Gary P. Scavone, 2000               */
/*                                         */
/*  This object inherits from WvIn and is  */
/*  used to open NeXT/Sun .snd 16-bit data */
/*  (signed integer) files for playback.   */
/*                                         */
/*  .snd files are always big-endian.      */
/*******************************************/

#if !defined(__SndWvIn_h)
#define __SndWvIn_h

#include "Object.h"
#include "WvIn.h"

class SndWvIn : public WvIn
{
public:
  SndWvIn(char *fileName, const char *mode);
  ~SndWvIn();
protected:
  void getData(long index);
};

#endif
