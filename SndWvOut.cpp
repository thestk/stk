/*******************************************/
/*  NeXT Sound File Output Class,          */
/*  by Perry R. Cook, 1995-96              */ 
/*  This version opens a mono NeXT .snd    */
/*  file 16bit data at 22KHz, and          */
/*  pokes buffers of samples into it.      */
/*******************************************/

/*******************************************/
/*  NeXT Soundfile Output Class, Improved  */
/*  by Perry R. Cook, 1996                 */ 
/*  This one opens a NeXT .snd file, and   */
/*  even knows how to byte-swap!           */
/*******************************************/

#include "SndWvOut.h"

#ifdef __LITTLE_ENDIAN__
  #include "swapstuf.h"
#endif

/********  NeXT Soundfile Header Struct   *******/
struct headerform {
  char pref[4];
  long hdr_length;
  long file_length;
  long mode;
  long samp_rate;
  long num_channels;
  char comment[16];
};

FILE *openNeXTFile(int chans,char *fileName)	{
  struct headerform hdr = {".sn",40,0,3,(long) SRATE,1,"STK98"};
  char tempName[128];
  FILE *fd;
    
  hdr.pref[3] = 'd';

  strcpy(tempName,fileName);
  strcat(tempName,".snd");
  hdr.num_channels = chans;
  fd = fopen(tempName,"wb");
  if (!fd)   {
    printf("Couldn't create soundfile %s  !!!!!!!!\n",fileName);
    exit(0);
  }
#ifdef __LITTLE_ENDIAN__
  hdr.hdr_length = SwapInt (hdr.hdr_length);
  hdr.file_length = SwapInt (hdr.file_length);
  hdr.mode = SwapInt (hdr.mode);
  hdr.samp_rate = SwapInt (hdr.samp_rate);
  hdr.num_channels = SwapInt (hdr.num_channels);
#endif
  printf("Creating soundfile %s.\n", tempName);
  fwrite(&hdr,4,10,fd);
  return fd;
}

SndWvOut :: SndWvOut(char *fileName)
{
  chans = 1;
  pan = 0.5;
  fd = openNeXTFile(chans,fileName);
  counter = 0;
  totalCount = 0;
}

SndWvOut :: SndWvOut(int channels, char *fileName)
{
  chans = channels;
  pan = 0.5;
  fd = openNeXTFile(chans,fileName);
  counter = 0;
  totalCount = 0;
}

SndWvOut :: ~SndWvOut()
{
  double temp;

  fwrite(data,2,counter,fd);
  fseek(fd,8,SEEK_SET);
  temp = (double) totalCount * ONE_OVER_SRATE;
  printf("%f Seconds Computed\n",temp);
  totalCount *= 2*chans;
#ifdef __LITTLE_ENDIAN__
  totalCount = SwapInt (totalCount);
#endif
  fwrite(&totalCount,4,1,fd);
  fclose(fd);
}

long SndWvOut :: getCounter()
{
  return totalCount;
}

MY_FLOAT SndWvOut :: getTime()
{
  return (MY_FLOAT) totalCount * ONE_OVER_SRATE;
}

void SndWvOut :: tick(MY_FLOAT sample)
{
  short isample;
    
  isample = (short) (sample * 32000.0);
  if (chans==1)	{
    data[counter] = isample;
#ifdef __LITTLE_ENDIAN__
    data[counter] = SwapShort (data[counter]);
#endif
	counter += 1;
  }
  else 	{
    data[counter] = (short) (isample * (1.0 - pan));
    data[counter+1] = (short) (isample * pan);
#ifdef __LITTLE_ENDIAN__
    data[counter] = SwapShort (data[counter]);
    data[counter+1] = SwapShort (data[counter+1]);
#endif
	counter += 2;
  }
  totalCount += 1;
  if (counter == SND_BUFFER_SIZE) {
    fwrite(data,2,SND_BUFFER_SIZE,fd);
    counter = 0;
  }
}

void SndWvOut :: tick(MY_FLOAT lsamp, MY_FLOAT rsamp)
{
  if (chans==1)	{
    data[counter] = (short) ((lsamp + rsamp) * 16000.0);
#ifdef __LITTLE_ENDIAN__
    data[counter] = SwapShort (data[counter]);
#endif
    counter += 1;
  }
  else 	{
    data[counter] = (short) (lsamp * 32000.0);
    data[counter+1] = (short) (rsamp * 32000.0);
#ifdef __LITTLE_ENDIAN__
    data[counter] = SwapShort (data[counter]);
    data[counter+1] = SwapShort (data[counter+1]);
#endif
    counter += 2;
  }
  totalCount += 1;
  if (counter == SND_BUFFER_SIZE) {
    fwrite(data,2,SND_BUFFER_SIZE,fd);
    counter = 0;
  }
}

void SndWvOut :: setMonoPan(MY_FLOAT aPan)
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
