/*******************************************/
/*  Wave file Output Class,                */
/*  by Perry R. Cook, 1995-96              */ 
/*  This Object opens a DOS/Windows .wav   */
/*  16bit data (signed integers) file, and */
/*  poke buffers of samples into it.       */
/*******************************************/

#include "WavWvOut.h"
#include "swapstuf.h"

/********  Wav Soundfile Header Struct   *******/
struct wavehdr {char riff[4];
		long flength;
		char wave[4];
		char fmt[4];
		long format;
		short aone;
		short numchans;
		long srate;
		long bps;
		short numchans2;
		short bps2;
		char data[4];
		long dlength;
		};

FILE *openWAVFile(int chans,char *fileName)	{
  struct wavehdr hdr = {"RIF",44,"WAV","fmt",
						16,1,1,(long) SRATE,(long) SRATE*2,1,16,"dat",0};
  char tempName[128];
  FILE *fd;
  extern int SwapInt(int);
  extern short SwapShort(short);

  hdr.riff[3] = 'F';
  hdr.wave[3] = 'E';
  hdr.fmt[3]  = ' ';
  hdr.data[3] = 'a';
    
  strcpy(tempName,fileName);
  strcat(tempName,".wav");
  hdr.numchans = chans;
  hdr.numchans2 = chans;
  hdr.bps = 16;		//  WARNING!!  This may need to be 16 * chans //
  hdr.bps2 = 16;
  fd = fopen(tempName,"wb");
  if (!fd)   {
    printf("Couldn't create soundfile %s  !!!!!!!!\n",fileName);
    exit(0);
  }

#ifndef __LITTLE_ENDIAN__
  hdr.flength = SwapInt(hdr.flength);
  hdr.format = SwapInt(hdr.format);
  hdr.aone = SwapShort(hdr.aone);
  hdr.numchans = SwapShort(hdr.numchans);
  hdr.srate = SwapInt(hdr.srate);
  hdr.bps = SwapInt(hdr.bps);
  hdr.numchans2 = SwapShort(hdr.numchans2);
  hdr.bps2 = SwapShort(hdr.bps2);
  hdr.dlength = SwapInt(hdr.dlength);
#endif

  printf("Creating soundfile %s.\n", tempName);
  fwrite(&hdr,4,11,fd);
  return fd;
}

WavWvOut :: WavWvOut(char *fileName)
{
  chans = 1;
  pan = 0.5;
  fd = openWAVFile(chans,fileName);
  counter = 0;
  totalCount = 0;
}

WavWvOut :: WavWvOut(int channels, char *fileName)
{
  chans = channels;
  pan = 0.5;
  fd = openWAVFile(chans,fileName);
  counter = 0;
  totalCount = 0;
}

WavWvOut :: ~WavWvOut()
{
  MY_FLOAT time;
  long bytes;
  extern int SwapInt(int);

  fwrite(data,2,counter,fd);
  fseek(fd,40,SEEK_SET);
  time = (double) totalCount * ONE_OVER_SRATE;
  printf("%f Seconds Computed\n",time);
  bytes = totalCount*2*chans;
#ifndef __LITTLE_ENDIAN__
  bytes = SwapInt(bytes);
#endif
  fwrite(&bytes,4,1,fd);
  bytes = totalCount*2*chans + 44;
#ifndef __LITTLE_ENDIAN__
  bytes = SwapInt(bytes);
#endif
  fseek(fd,4,SEEK_SET);
  fwrite(&totalCount,4,1,fd);
  fclose(fd);
}

long WavWvOut :: getCounter()
{
  return totalCount;
}

MY_FLOAT WavWvOut :: getTime()
{
  return (MY_FLOAT) totalCount * ONE_OVER_SRATE;
}

void WavWvOut :: tick(MY_FLOAT sample)
{
  short isample;
  extern short SwapShort(short);

  isample = (short) (sample * 32000.0);
  if (chans==1)	{
    data[counter] = isample;
#ifndef __LITTLE_ENDIAN__
    data[counter] = SwapShort(data[counter]);
#endif
	counter += 1;
  }
  else 	{
    data[counter] = (short) (isample * (1.0 - pan));
    data[counter+1] = (short) (isample * pan);
#ifndef __LITTLE_ENDIAN__
    data[counter] = SwapShort(data[counter]);
    data[counter+1] = SwapShort(data[counter+1]);
#endif
	counter += 2;
  }
  totalCount += 1;
  if (counter == WAV_BUFFER_SIZE) {
    fwrite(data,2,WAV_BUFFER_SIZE,fd);
    counter = 0;
  }
}

void WavWvOut :: tick(MY_FLOAT lsamp, MY_FLOAT rsamp)
{
  extern short SwapShort(short);

  if (chans==1)	{
    data[counter] = (short) ((lsamp + rsamp) * 16000.0);
#ifndef __LITTLE_ENDIAN__
    data[counter] = SwapShort(data[counter]);
#endif
    counter += 1;
  }
  else 	{
    data[counter] = (short) (lsamp * 32000.0);
    data[counter+1] = (short) (rsamp * 32000.0);
#ifndef __LITTLE_ENDIAN__
    data[counter] = SwapShort(data[counter]);
    data[counter+1] = SwapShort(data[counter+1]);
#endif
    counter += 2;
  }
  totalCount += 1;
  if (counter == WAV_BUFFER_SIZE) {
    fwrite(data,2,WAV_BUFFER_SIZE,fd);
    counter = 0;
  }
}

void WavWvOut :: setMonoPan(MY_FLOAT aPan)
{
  pan = aPan;
  if (aPan < 0.0) {
    pan = 0.0;
    printf("Pan < 0.0, correcting to 0.0\n");
  }
  if (aPan > 1.0) {
    pan = 1.0;
    printf("Pan > 1.0, correcting to 1.0\n");
  }
}
