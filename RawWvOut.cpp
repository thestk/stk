/*******************************************/
/*  Raw Wave File Output Class,            */
/*  by Perry R. Cook, 1995-96              */ 
/*  This version opens a mono NeXT .snd    */
/*  file 16bit data at 22KHz, and          */
/*  pokes buffers of samples into it.      */
/*******************************************/

#include "RawWvOut.h"

#if !defined(__SGI_REALTIME)

/********  NeXT Soundfile Header Struct   *******/
struct headerform {
    char pref[4];
    long hdr_length;
    long file_length;
    long mode;
    long samp_rate;
    long num_channels;
    char comment[1024];
};

RawWvOut :: RawWvOut(char *fileName)
{
    struct headerform hdr = {".sn",28,0,3,(long) SRATE,1,"TK"};
    hdr.pref[3] = 'd';

    fd = fopen(fileName,"wb");
    if (!fd)   {
	printf("Couldn't create soundfile %s  !!!!!!!!\n",fileName);
	exit(0);
    }
    fwrite(&hdr,4,7,fd);
    counter = 0;
    totalCount = 0;
}

RawWvOut :: ~RawWvOut()
{
    MY_FLOAT temp;
    fwrite(data,2,counter,fd);
    fseek(fd,8,SEEK_SET);
    fwrite(&totalCount,4,1,fd);
    fclose(fd);
    temp = (double) totalCount * ONE_OVER_SRATE;
    printf("%f Seconds Computed\n",temp);
}

long RawWvOut :: getCounter()
{
    return totalCount;
}

void RawWvOut :: tick(MY_FLOAT sample)
{

    data[counter++] = (short) (sample * 32000.0);
    totalCount += 1;
    if (counter == BUFFER_SIZE) {
	fwrite(data,2,BUFFER_SIZE,fd);
	counter = 0;
    }
}

#else


/*******************************************/
/*  SGI Real-Time Wave File Output Class,  */
/*  by Perry R. Cook, 1996                 */ 
/*  This Object can opens the SGI soundout */
/*  device, and pokes buffers of samples   */
/*  into it.  The real code that does the  */
/*  is from Doug Scott of SGI.             */
/*******************************************/

#include "sgio.C"

RawWvOut :: RawWvOut(char *fileName)
{
    int fd;

    fd = init_sound(SRATE,1);
    if (fd<0)   {
	printf("Couldn't open SoundOut Device  !!!!!!!!\n");
	exit(0);
    }
    counter = 0;
}

RawWvOut :: ~RawWvOut()
{
    playbuf(data,counter);
    counter = 0;
    while (counter<BUFFER_SIZE)	{
        data[counter++] = 0;
    }
    playbuf(data,counter);
    playbuf(data,counter);
    playbuf(data,counter);
    shuddup();
}

void RawWvOut :: tick(MY_FLOAT sample)
{

    data[counter++] = (short) (sample * 32000.0);
    if (counter == BUFFER_SIZE) {
	playbuf(data,counter);
	counter = 0;
    }
}

#endif
