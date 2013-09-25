/*******************************************/
/*  Non-Interpolating One-Shot Raw Sound-  */
/*  file Class, by Perry R. Cook, 1995-96  */ 
/*  This Object can open a raw 16bit data  */
/*  (signed integers) file, and play back  */
/*  the data once, with no interpolation   */
/*  on playback.  Once finished, it closes */        
/*  the file, the file is reopened with    */
/*  the reset() method.                    */
/*  This is useful for small memory model, */  
/*  applications, or for streaming from    */
/*  disk (and generally non real-time      */
/*  applications).                         */
/*******************************************/

#include "RawShot.h"

#ifdef __LITTLE_ENDIAN__
  #include "swapstuf.h"
#endif

RawShot :: RawShot(char *fileName)
{
	long i;

	strcpy(fileNm,fileName);
	myFile = fopen(fileNm,"rb");
	if (!myFile)   {
		printf("Couldn't find soundfile %s  !!!!!!!!\n",fileName);
		exit(0);
	}
    
	i = 0;
	while (fread(&data,2,1,myFile)) i++;
	length = i;
	fseek(myFile,0,0);

	time = (MY_FLOAT) 0.0;
	rate = (MY_FLOAT) 1.0;
	lastTime = 0;
	finished = 0;
	gain = (MY_FLOAT) 1.0;
	lastOutput = (MY_FLOAT) 0.0;
}

RawShot :: ~RawShot()
{
	this->finish();
}

void RawShot :: reset()
{
	if (finished)     {
		myFile = fopen(fileNm,"rb");
	}
	fseek(myFile,0,0); 

#if defined(_debug_)        
	printf("Resetting\n");
#endif    

	time = (MY_FLOAT) 0.0;
	lastTime = 0;
	finished = 0;
	lastOutput = (MY_FLOAT) 0.0;
}

void RawShot :: normalize()
{
	this->normalize((MY_FLOAT) 1.0);
}

void RawShot :: normalize(MY_FLOAT newPeak)
{
	long i;
	FILE *fd;

	gain = (MY_FLOAT) 0.0;
	fd = fopen(fileNm,"rb");
	for (i=0;i<length;i++)    {
		fread(&data,2,1,fd);
#ifdef __LITTLE_ENDIAN_
		data = SwapShort(data);
#endif
		if (fabs(data) > gain) 
	    gain = (MY_FLOAT) fabs((double) data);
	}
	if (gain > 0.0) {
		gain = newPeak / gain;
	}
	fclose(fd);
}

void RawShot :: setRate(MY_FLOAT aRate)
{
	rate = aRate;
}

void RawShot :: finish()
{
	finished = 1;
	lastOutput = (MY_FLOAT) 0.0;
	if (myFile)   {
		fclose(myFile);
		myFile = 0;
	}
}

int RawShot :: isFinished()
{
  return finished;
}

MY_FLOAT RawShot ::  tick()
{
	this->informTick();
	return lastOutput;
}

int RawShot :: informTick()
{
  long temp;

  if (!finished) {
    time += rate;          /* Update current time */

    if (time >= length) {  /* Check for end of sound */
      time = (MY_FLOAT) length - 1;   /* stick at end */
      finished = 1;                   /* Information for one-shot use */
      fclose(myFile);
      myFile = 0;
    }
    else {
      temp = (long) time;    /* Integer part of time address */
      if (temp > lastTime) { /* If we cross next sample time */
        lastTime = temp;
        fread(&data,2,1,myFile);  /* Snarf next sample from file */
#ifdef __LITTLE_ENDIAN__
        data = SwapShort(data);
#endif
        lastOutput = data * gain; /* And save as non-interpolated data */
      }
    }
  }

  return finished;                        
}

MY_FLOAT RawShot :: lastOut()
{
	return lastOutput;
}

/************   Test Main Program   *****************/
/*
void main()
{
    RawShot oneShot("rawwaves/mandpluk.raw");
    FILE *fd;
    short data;
    long i;
    
    fd = fopen("test.raw","wb");
    oneShot.setRate(1.0);
    while (!oneShot.informTick()) {
	data = oneShot.lastOut();
	fwrite(&data,2,1,fd);
    }
    
    oneShot.reset();
    oneShot.setRate(0.5);
    for (i=0;i<16384;i++) {
	data = oneShot.tick();
	fwrite(&data,2,1,fd);
    }
    fclose(fd);
}
*/
