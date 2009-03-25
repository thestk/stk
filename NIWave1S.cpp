/*******************************************/
/*  NonInterpolating One-Shot Raw Sound-   */
/*  file Class, by Perry R. Cook, 1995-96  */ 
/*  This Object can open a raw 16bit data  */
/*  (signed integers) file, and play back  */
/*  the data once (again when reset), with */
/*  no interpolation on playback.          */
/*******************************************/

#include "NIWave1S.h"

NIWave1S :: NIWave1S(char *fileName)
{
    long i;
    short temp;
    FILE *fd;
    fd = fopen(fileName,"rb");
    if (!fd)   {
	printf("Couldn't find soundfile %s  !!!!!!!!\n",fileName);
	exit(0);
    }
    i = 0;
    while (fread(&temp,2,1,fd)) i++;
    length = i;
    fseek(fd,0,0);
    data = (MY_FLOAT *) malloc(MY_FLOAT_SIZE * (length + 1));
    i = 0;
    while (fread(&temp,2,1,fd))   {
	data[i] = temp;
	i++;
    }
    data[length] = data[length-1];
    fclose(fd);
    time = 0.0;
    rate = 1.0;
}

void NIWave1S :: reset()
{
    time = 0.0;
    lastOutput = 0.0;
}

void NIWave1S :: normalize()
{
    this->normalize(1.0);
}

void NIWave1S :: normalize(MY_FLOAT newPeak)
{
    long i;
    MY_FLOAT max = 0.0;
    for (i=0;i<=length;i++)
	if (fabs(data[i]) > max) 
	    max = fabs(data[i]);
    if (max > 0.0)       {
	max = 1.0 / max;
	max *= newPeak;
	for (i=0;i<=length;i++)
	    data[i] *= max;
    }
}

void NIWave1S :: setRate(MY_FLOAT aRate)
{
    rate = aRate;
}

MY_FLOAT NIWave1S ::  tick()
{
    this->informTick();
    return lastOutput;
}

int NIWave1S :: informTick()
{
    int allDone = 0;
    long temp;
    
    time += rate;                /*  Update current time          */
    
    if (time >= length)  {       /*  Check for end of sound       */
	time = length-1;         /*  stick at end                 */
	allDone = 1;             /*  Information for one-shot use */
    }
    temp = (long) time;          /*  Integer part of time address */
    lastOutput = data[temp];     /*  Get non-interpolated data    */
    
    return allDone;                        
}

MY_FLOAT NIWave1S :: lastOut()
{
    return lastOutput;
}

/************   Test Main Program   *****************/
/*
void main()
{
    NIWave1S oneShot("rawwaves/mandpluk.raw");
    FILE *fd;
    short data;
    long i;
    
    fd = fopen("test.raw","wb");
    for (i=0;i<8192;i++) {
	data = oneShot.tick();
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
