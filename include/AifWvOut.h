/*******************************************/
/*
  AifWvOut Output Class,
  by Gary P. Scavone, 2000

  This object inherits from WvOut and is
  used to write Audio Interchange File
  Format files with 16-bit data (signed
  integer).

  .aif files are always bif-endian.
*/
/*******************************************/

#if !defined(__AifWvOut_h)
#define __AifWvOut_h

#include "Object.h"
#include "WvOut.h"

class AifWvOut : public WvOut
{
 protected:
	FILE *fd;
 public:
	AifWvOut(char *fileName, int chans = 1);
	~AifWvOut();
	void tick(MY_FLOAT sample);
	void mtick(MY_MULTI samples);
};

#endif
