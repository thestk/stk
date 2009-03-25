/*******************************************/
/*  NonInterpolating One-Shot Raw Sound-   */
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

#include "RawWvIn.h"

RawWvIn :: RawWvIn(char *fileName)
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
    time = 0.0;
    rate = 1.0;
    lastTime = 0;
    finished = 0;
    gain = 1.0;
    lastOutput = 0.0;
}

RawWvIn :: ~RawWvIn()
{
    this->finish();
}

void RawWvIn :: reset()
{
    if (finished)     {
	myFile = fopen(fileNm,"rb");
    }
    fseek(myFile,0,0); 

    printf("Resetting\n");
    time = 0.0;
    lastTime = 0;
    finished = 0;
    lastOutput = 0.0;
}

void RawWvIn :: normalize()
{
    this->normalize(1.0);
}

void RawWvIn :: normalize(MY_FLOAT newPeak)
{
    long i;
    FILE *fd;

    gain = 0.0;
    
    fd = fopen(fileNm,"rb");
    for (i=0;i<length;i++)    {
	fread(&data,2,1,fd);
	if (fabs(data) > gain) 
	    gain = fabs(data);
    }
    if (gain > 0.0)       {
	gain = newPeak / gain;
    }
    fclose(fd);
}

void RawWvIn :: setRate(MY_FLOAT aRate)
{
    rate = aRate;
}

void RawWvIn :: finish()
{
    finished = 1;
    lastOutput = 0.0;
    if (myFile)   {
	fclose(myFile);
	myFile = 0;
    }
}

MY_FLOAT RawWvIn ::  tick()
{
    this->informTick();
    return lastOutput;
}

int RawWvIn :: informTick()
{
    long temp;
    
    if (!finished)        {
	
      time += rate;                 /*  Update current time               */
									  
      if (time >= length)  {        /*  Check for end of sound            */
	time = length - 1;          /*  stick at end                      */
	finished = 1;               /*  Information for one-shot use      */
	fclose(myFile);
	myFile = 0;
      }
      else    {
	temp = (long) time;         /*  Integer part of time address      */
	if (temp > lastTime)   {    /*  If we cross next sample time      */
	  lastTime = temp;
	  fread(&data,2,1,myFile);  /*  Snarf next sample from file       */
	  lastOutput = data * gain; /*  And save as non-interpolated data */
	}
      }
    }

    return finished;                        
}

MY_FLOAT RawWvIn :: lastOut()
{
    return lastOutput;
}

/************   Test Main Program   *****************/
/*
void main()
{
    RawWvIn oneShot("rawwaves/mandpluk.raw");
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

