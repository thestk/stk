/**********************************************/
/*  Lip Filter Object by Perry R. Cook, 1995-96*/    
/*  The lip of the brass player has dynamics  */
/*  which are controlled by the mass, spring  */
/*  constant, and damping of the lip.  This   */
/*  filter simulates that behavior and the    */
/*  transmission/reflection properties as     */
/*  well.  See Cook TBone and HosePlayer      */
/*  instruments and articles.                 */
/**********************************************/

#include "LipFilt.h"

LipFilt :: LipFilt()
{
    MY_FLOAT coeffs[2];
    filter = new BiQuad;
    coeffs[0] = 0.0;
    coeffs[1] = 0.0;
    filter->setZeroCoeffs(coeffs);
    this->clear();
}

LipFilt :: ~LipFilt()
{
    delete filter;
}

void LipFilt :: clear()
{
    filter->clear();
    lastOutput = 0.0;
}

void LipFilt :: setFreq(MY_FLOAT frequency)
{
    MY_FLOAT coeffs[2];
    coeffs[0] = 2.0 * 0.997 * 
		cos(TWO_PI * frequency / SRATE);  /* damping should change with  */
    coeffs[1] = -0.997 * 0.997;                 /* lip parameters, but not yet.*/
    filter->setPoleCoeffs(coeffs);                             
    filter->setGain(0.03);
}

/*  NOTE:  Here we should add lip tension                 */
/*              settings based on Mass/Spring/Damping     */
/*              Maybe in TookKit97                        */

MY_FLOAT LipFilt :: tick(MY_FLOAT mouthSample,MY_FLOAT boreSample)    
		/*   Perform "Table Lookup" By Polynomial Calculation */
{
    MY_FLOAT temp;
    temp = mouthSample - boreSample;          /* Differential pressure              */
    temp = filter->tick(temp);                /* Force -> position                  */
    temp = temp*temp;                         /* Simple position to area mapping    */
    if (temp > 1.0) temp = 1.0;               /* Saturation at + 1.0                */
    lastOutput = temp * mouthSample;          /* Assume mouth input = area          */
    lastOutput += (1.0 - temp) * boreSample;  /* and Bore reflection is compliment. */
    return lastOutput;
}

MY_FLOAT LipFilt :: lastOut()
{
    return lastOutput;
}

