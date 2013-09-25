/*******************************************/
/*  Wave file Output Class,                */
/*  by Perry R. Cook, 1995-96              */ 
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
	short data[WAV_BUFFER_SIZE];
	long counter;
	long totalCount;
	int chans;
	MY_FLOAT pan;
  public:
	WavWvOut(char *fileName);
	WavWvOut(int channels, char *fileName);
	~WavWvOut();
	long getCounter();
	MY_FLOAT getTime();
	void setMonoPan(MY_FLOAT aPan);
	void tick(MY_FLOAT sample);
	void tick(MY_FLOAT lsamp, MY_FLOAT rsamp);
};

#endif
