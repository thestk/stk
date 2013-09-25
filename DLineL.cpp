/*******************************************/
/*  Linearly Interpolating Delay Line      */
/*  Object by Perry R. Cook 1995-96        */
/*  This one uses a delay line of maximum  */
/*  length specified on creation, and      */ 
/*  linearly interpolates fractional       */
/*  length.  It is designed to be more     */
/*  efficient if the delay length is not   */
/*  changed very often.                    */
/*******************************************/

#include "DLineL.h"

DLineL :: DLineL(long max_length)
{
    length = max_length;
    inputs = (MY_FLOAT *) malloc(length * MY_FLOAT_SIZE);
    this->clear();
    outPoint = 0;
    inPoint = length >> 1;
}

DLineL :: ~DLineL()
{
    free(inputs);
}

void DLineL :: clear()
{
    long i;
    for (i=0;i<length;i++) inputs[i] = (MY_FLOAT) 0.0;
    lastOutput = (MY_FLOAT) 0;
}

void DLineL :: setDelay(MY_FLOAT lag)
{
    MY_FLOAT outputPointer;
    outputPointer = inPoint - lag;       /* read chases write, add 1 for interp.   */
    while (outputPointer<0)
	outputPointer += length;           /* modulo maximum length                  */
    outPoint = (long) outputPointer;   /* integer part                           */
    alpha = outputPointer - outPoint;  /* fractional part                        */
    omAlpha = (MY_FLOAT) 1.0 - alpha;  /* 1.0 - fractional part (more efficient) */
}

MY_FLOAT DLineL :: tick(MY_FLOAT sample)        /*   Take one, yield one           */
{
    inputs[inPoint++] = sample;                 /*  Input next sample              */
    if (inPoint == length)                      /*  Check for end condition        */
	inPoint -= length;
    lastOutput = inputs[outPoint++] * omAlpha;  /*  first 1/2 of interpolation     */
    if (outPoint<length)        {               /*  Check for end condition        */
	lastOutput += inputs[outPoint] * alpha; /*  second 1/2 of interpolation    */
    }                                          
    else        {                               /*  if at end . . .                */
	lastOutput += inputs[0] * alpha;        /*  second 1/2 of interpolation    */
	outPoint -= length;                             
    }
    return lastOutput;
}

/************   Test Main Program   *****************/
/*

void main()
{
    DLineL delay(140);
    FILE *fd;
    MY_FLOAT temp;
    short data;
    long i;
    
    fd = fopen("test.raw","wb");
    
    delay.setDelay(128);
    for (i=0;i<4096;i++) {
	if (i%256 != 0) temp = 0.0; else temp = 1.0;
	data = (temp + delay.tick(temp)) * 16000.0;
	fwrite(&data,2,1,fd);
    }
    delay.setDelay(64.5);
    for (i=0;i<4096;i++) {
	if (i%256 != 0) temp = 0.0; else temp = 1.0;
	data = (temp + delay.tick(temp)) * 16000.0;
	fwrite(&data,2,1,fd);
    }
    
    fclose(fd);
}
*/
