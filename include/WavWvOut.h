/*******************************************/
/*  Wave file Output Class,                */
/*  by Perry R. Cook, 1995-96              */
/*  revised by Gary P. Scavone, 1999       */
/*                                         */
/*  This Object opens a DOS/Windows .wav   */
/*  16bit data (signed integers) file, and */
/*  poke buffers of samples into it.       */
/*******************************************/

#if !defined(__WavWvOut_h)
#define __WavWvOut_h

#include "Object.h"
#include "WvOut.h"

class WavWvOut : public WvOut
{
 protected:
	FILE *fd;
 public:
	WavWvOut(char *fileName, int chans = 1);
	~WavWvOut();
	void tick(MY_FLOAT sample);
	void mtick(MY_MULTI samples);
};

#endif
