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
  offSet = (MY_FLOAT) 0.6;    /* Offset is a bias, related to reed rest position  */
  slope = (MY_FLOAT) -0.8;    /* Slope corresponds loosely to reed stiffness      */
}

ReedTabl :: ~ReedTabl()
{

}

void ReedTabl :: setOffset(MY_FLOAT aValue)
{
  offSet = aValue;     /* Offset is a bias, related to reed rest position */
}

void ReedTabl :: setSlope(MY_FLOAT aValue)
{
  slope = aValue;      /* Slope corresponds loosely to reed stiffness  */
}

MY_FLOAT ReedTabl :: lookup(MY_FLOAT deltaP)    
{
  return this->tick(deltaP);
}

MY_FLOAT ReedTabl :: tick(MY_FLOAT deltaP)    
    /*   Perform "Table Lookup" by direct clipped  */
    /*   linear function calculation               */
{   /*   deltaP is differential reed pressure      */
  lastOutput = offSet + (slope * deltaP); /* compute basic non-linearity     */
  if (lastOutput > 1.0) lastOutput = (MY_FLOAT) 1.0;     /* if other way, reed slams shut   */
  if (lastOutput < -1.0) lastOutput = (MY_FLOAT) -1.0;   /* if all the way open, acts like open end */
  return lastOutput;
}

MY_FLOAT ReedTabl :: lastOut()
{
    return lastOutput;
}

