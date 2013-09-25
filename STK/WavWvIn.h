/*******************************************/
/*  WavWvIn Input Class,                   */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  This object inherits from WvIn and is  */
/*  used to open DOS/Windows .wav 16-bit   */
/*  data (signed integer) files for        */
/*  playback.                              */
/*                                         */
/*  .wav files are always little-endian.   */
/*******************************************/

#if !defined(__WavWvIn_h)
#define __WavWvIn_h

#include "Object.h"
#include "WvIn.h"

class WavWvIn : public WvIn
{
  public:
    WavWvIn(char *fileName, char *mode);
    ~WavWvIn();
};

#endif
