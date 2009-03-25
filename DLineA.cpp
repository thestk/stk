/*******************************************/
/*                                         */
/*  AllPass Interpolating Delay Line       */
/*  Object by Perry R. Cook 1995-96        */
/*  This one uses a delay line of maximum  */
/*  length specified on creation, and      */ 
/*  interpolates fractional length using   */
/*  an all-pass filter.  This version is   */
/*  more efficient for computing static    */
/*  length delay lines (alpha and coeff    */
/*  are computed only when the length      */  
/*  is set, there probably is a more       */ 
/*  efficient computational form if alpha  */
/*  is changed often (each sample)).       */
/*                                         */
/*******************************************/

#include "DLineA.h"

DLineA :: DLineA(long max_length)
{
    long i;
    length = max_length;
    inputs = (MY_FLOAT *) malloc(length * MY_FLOAT_SIZE);
    for (i=0;i<length;i++) inputs[i] = 0;
    this->clear();
    inPoint = 0;
    outPoint = length >> 1;
}

DLineA :: ~DLineA()
{
    free(inputs);
}

void DLineA :: clear()
{
    long i;
    for (i=0;i<length;i++) inputs[i] = 0.0;
    lastIn = 0;
    lastOutput = 0;
}

void DLineA :: setDelay(MY_FLOAT lag)  
{
    MY_FLOAT outputPointer;
    outputPointer = inPoint - lag + 2;  /* outPoint chases inpoint        */
					/*   + 2 for interp and other     */
    while (outputPointer<0)                                               
	outputPointer += length;        /* modulo table length            */
    outPoint = (long) outputPointer;    /* Integer part of delay          */
    alpha = 1.0 + outPoint - outputPointer;   /* fractional part of delay       */
    if (alpha<0.1) {
	outputPointer += 1.0;           /*  Hack to avoid pole/zero       */
	outPoint += 1;                  /*  cancellation.  Keeps allpass  */
	alpha += 1.0;                   /*  delay in range of .1 to 1.1   */
    }
    coeff = (1.0 - alpha) / (1.0 + alpha); /* coefficient for all pass    */
}

MY_FLOAT DLineA :: tick(MY_FLOAT sample)   /*   Take sample, yield sample */
{
    MY_FLOAT temp;
    inputs[inPoint++] = sample;            /* Write input sample          */
    if (inPoint == length)                 /* Increment input pointer     */
	inPoint -= length;                 /* modulo length               */
    temp = inputs[outPoint++];             /* filter input                */
    if (outPoint == length)                /* Increment output pointer    */
	outPoint -= length;                /* modulo length               */
    lastOutput = -coeff * lastOutput;      /* delayed output              */
    lastOutput += lastIn + (coeff * temp); /* input + delayed Input       */
    lastIn = temp;
    return lastOutput;                     /* save output and return      */
}

/************   Test Main Program   *****************/
/*

void main()
{
    DLineA delay(140);
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
