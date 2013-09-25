/*******************************************/
/*  Wave file Output Class,                */
/*  by Perry R. Cook, 1995-96              */
/*  revised by Gary P. Scavone, 1999       */
/*                                         */
/*  This Object opens a DOS/Windows .wav   */
/*  16bit data (signed integers) file, and */
/*  poke buffers of samples into it.       */
/*******************************************/

#include "Object.h"
#include "WvOut.h"

#if !defined(__WavWvOut_h)
#define __WavWvOut_h

#define WAV_BUFFER_SIZE 1024

class WavWvOut : public WvOut
{
 protected:
	FILE *fd;
	INT16 data[WAV_BUFFER_SIZE];
	long counter;
	long totalCount;
	int channels;
 public:
	WavWvOut(char *fileName);
	WavWvOut(int chans, char *fileName);
	~WavWvOut();
	long getCounter();
	MY_FLOAT getTime();
	void tick(MY_FLOAT sample);
	void mtick(MY_MULTI samples);
};

#endif
