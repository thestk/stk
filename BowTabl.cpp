/***********************************************/ 
/*  Simple Bow Table Object, after Smith       */
/*    by Perry R. Cook, 1995-96                */
/***********************************************/

#include "BowTabl.h"

BowTabl :: BowTabl()
{
    offSet = 0.0;    /*  offset is a bias, really not needed unless   */
		     /*  friction is different in each direction      */
    slope = 0.1;     /*  controls width of friction pulse,            */
		     /*         related to bowForce                   */
}

BowTabl :: ~BowTabl()
{
}

void BowTabl :: setOffset(MY_FLOAT aValue)
{
    offSet = aValue;
}

void BowTabl :: setSlope(MY_FLOAT aValue)
{
    slope = aValue;
}

MY_FLOAT BowTabl :: lookup(MY_FLOAT sample) /*  Perform Table Lookup    */
{                                           /*  sample is differential  */
					    /*  string vs. bow velocity */
    MY_FLOAT input;
    input = sample + offSet;                /*  add bias to sample      */
    input *= slope;                         /*  scale it                */
    lastOutput = fabs(input) + 0.75;        /*  below min delta, friction = 1 */
    lastOutput = pow(lastOutput,-4.0);
//    if (lastOutput < 0.0 ) lastOutput = 0.0; /*  minimum friction is 0.0 */
    if (lastOutput > 1.0 ) lastOutput = 1.0; /*  maximum friction is 1.0 */
    return lastOutput;
}

MY_FLOAT BowTabl :: lastOut()
{
    return lastOutput;
}
