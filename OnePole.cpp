/*******************************************/
/*  One Pole Filter Class,                 */
/*  by Perry R. Cook, 1995-96              */ 
/*  The parameter gain is an additional    */
/*  gain parameter applied to the filter   */  
/*  on top of the normalization that takes */
/*  place automatically.  So the net max   */
/*  gain through the system equals the     */
/*  value of gain.  sgain is the combina-  */
/*  tion of gain and the normalization     */
/*  parameter, so if you set the poleCoeff */
/*  to alpha, sgain is always set to       */
/*  gain * (1.0 - fabs(alpha)).            */
/*******************************************/

#include "OnePole.h"

OnePole :: OnePole() : Filter()
{
    poleCoeff = (MY_FLOAT) 0.9;
    gain = (MY_FLOAT) 1.0;
    sgain = (MY_FLOAT) 0.1;
    outputs = (MY_FLOAT *) malloc(MY_FLOAT_SIZE);
    outputs[0] = (MY_FLOAT) 0.0;
}

OnePole :: ~OnePole()    
{
  free(outputs);
}

void OnePole :: clear()
{
    outputs[0] = (MY_FLOAT) 0.0;
    lastOutput = (MY_FLOAT) 0.0;
}

void OnePole :: setPole(MY_FLOAT aValue)
{
    poleCoeff = aValue;
    if (poleCoeff > 0.0)                   /*  Normalize gain to 1.0 max */
	sgain = gain * ((MY_FLOAT) 1.0 - poleCoeff);
    else
	sgain = gain * ((MY_FLOAT) 1.0 + poleCoeff);
}

void OnePole :: setGain(MY_FLOAT aValue)
{
    gain = aValue;
    if (poleCoeff > 0.0)        
	sgain = gain * ((MY_FLOAT) 1.0 - poleCoeff);  /*  Normalize gain to 1.0 max */
    else
	sgain = gain * ((MY_FLOAT) 1.0 + poleCoeff);
}

MY_FLOAT OnePole :: tick(MY_FLOAT sample)  /*   Perform Filter Operation */
{
    outputs[0] = (sgain * sample) + (poleCoeff * outputs[0]);              
    lastOutput = outputs[0];
    return lastOutput;
}

/************  Test Main  ************************/
/*
#include <stdio.h>

void main()
{
    long i;
    OnePole test;
    test.setPole(0.99);
    for (i=0;i<150;i++) printf("%lf  ",test.tick(1.0));
    printf("\n\n");

    test.clear();
    test.setPole(0.9);
    test.setGain(2.0);
    for (i=0;i<150;i++) printf("%lf  ",test.tick(0.5));
    printf("\n\n");
}
*/
