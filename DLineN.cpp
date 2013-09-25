/*******************************************/
/*  Non-Interpolating Delay Line           */
/*  Object by Perry R. Cook 1995-96        */
/*  This one uses a delay line of maximum  */
/*  length specified on creation.  A non-  */
/*  interpolating delay line should be     */
/*  used in non-time varying (reverb) or   */
/*  non-critical (????) applications.      */
/*******************************************/

#include "DLineN.h"

DLineN :: DLineN(long max_length)
{
    length = max_length;
    inputs = (MY_FLOAT *) malloc(length * MY_FLOAT_SIZE);
    this->clear();
    this->setDelay(length * (MY_FLOAT) 0.5);
    inPoint = 0;
    outPoint = 0;
}

DLineN :: ~DLineN()
{
    free(inputs);
}

void DLineN :: clear()
{
    long i;
    for (i=0;i<length;i++) inputs[i] = (MY_FLOAT) 0.0;
    lastOutput = (MY_FLOAT) 0;
}

void DLineN :: setDelay(MY_FLOAT lag)
{
    outPoint = inPoint - (long) lag;        /* read chases write           */
    while (outPoint<0) outPoint += length;  /* modulo maximum length       */
}

MY_FLOAT DLineN :: tick(MY_FLOAT sample)    /*   Take one, yield one       */
{                                              
    inputs[inPoint++] = sample;             /*  Input next sample          */
    if (inPoint == length)                  /*  Check for end condition    */
	inPoint -= length;
    lastOutput = inputs[outPoint++];        //  first 1/2 of interpolation */
    if (outPoint>=length)        {          //  Check for end condition    */
	outPoint -= length;                             
    }
    return lastOutput;
}

/************   Test Main Program   *****************/
/*
void main()
{
    DLineN delay(140);
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
