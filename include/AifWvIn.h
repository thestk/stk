/*******************************************/
/*
  AifWvIn Input Class,
  by Gary P. Scavone, 2000

  This object inherits from WvIn and is
  used to open Audio Interchange File
  Format files with 16-bit data (signed
  integer) for playback.

  .aif files are always bif-endian.
*/
/*******************************************/

#if !defined(__AifWvIn_h)
#define __AifWvIn_h

#include "Object.h"
#include "WvIn.h"

class AifWvIn : public WvIn
{
public:
  AifWvIn(char *fileName, const char *mode);
  ~AifWvIn();
protected:
  void getData(long index);
};

#endif
