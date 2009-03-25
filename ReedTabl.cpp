/**********************************************/
/*  One break point linear reed table object  */
/*  by Perry R. Cook, 1995-96                 */
/*  Consult McIntyre, Schumacher, & Woodhouse */
/*        Smith, Hirschman, Cook, Scavone,    */
/*        more for information.               */
/**********************************************/

#include "ReedTabl.h"

ReedTabl :: ReedTabl() : Object()
{
    offSet = 0.6;    /* Offset is a bias, related to reed rest position  */
    slope = -0.8;    /* Slope corresponds loosely to reed stiffness      */
}

ReedTabl :: ~ReedTabl()
{

}

void ReedTabl :: setOffset(double aValue)
{
    offSet = aValue;     /* Offset is a bias, related to reed rest position */
}

void ReedTabl :: setSlope(double aValue)
{
    slope = aValue;      /* Slope corresponds loosely to reed stiffness  */
}

MY_FLOAT ReedTabl :: lookup(double deltaP)    
    /*   Perform "Table Lookup" by direct clipped  */
    /*   linear function calculation               */
{   /*   deltaP is differential reed pressure      */
    lastOutput = offSet + (slope * deltaP); /* compute basic non-linearity     */
    if (lastOutput > 1.0) lastOutput = 1.0;     /* if other way, reed slams shut   */
    if (lastOutput < -1.0) lastOutput = -1.0;   /* if all the way open, acts like open end */
    return lastOutput;
}

MY_FLOAT ReedTabl :: lastOut()
{
    return lastOutput;
}

