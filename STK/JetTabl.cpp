/**********************************************/
/* Jet Table Object by Perry R. Cook, 1995-96 */
/* Consult Fletcher and Rossing, Karjalainen, */
/*       Cook, more, for information.         */
/* This, as with many other of my "tables",   */
/* is not a table, but is computed by poly-   */
/* nomial calculation.                        */
/**********************************************/

#include "JetTabl.h"

JetTabl :: JetTabl()
{
    lastOutput = (MY_FLOAT) 0.0;
}

JetTabl :: ~JetTabl()
{
}

MY_FLOAT JetTabl :: lookup(MY_FLOAT sample) /*   Perform "Table Lookup"    */
{                                       /*   By Polynomial Calculation */
    lastOutput = sample * 
                (sample*sample - (MY_FLOAT)  1.0);  /* (x^3 - x) approximates sigmoid of jet */
    if (lastOutput > 1.0) 
        lastOutput = (MY_FLOAT) 1.0;    /* Saturation at +/- 1.0       */
    if (lastOutput < -1.0) 
        lastOutput = (MY_FLOAT) -1.0; 
    return lastOutput;
}

MY_FLOAT JetTabl :: lastOut()
{
    return lastOutput;
}

