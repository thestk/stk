/*******************************************/
/*  Sweepable Formant (2-pole)             */
/*  Filter Class, by Perry R. Cook, 1995-96*/ 
/*  See books on filters to understand     */
/*  more about how this works.  This drives*/
/*  to a target at speed set by rate.      */
/*******************************************/

#include "FormSwep.h"

FormSwep :: FormSwep() : Filter()
{
    outputs = (MY_FLOAT *) malloc(2 * MY_FLOAT_SIZE);
    poleCoeffs[0] = (MY_FLOAT) 0.0;
    poleCoeffs[1] = (MY_FLOAT) 0.0;
    gain = (MY_FLOAT) 1.0;
    freq = (MY_FLOAT) 0.0;
    reson = (MY_FLOAT) 0.0;
    currentGain = (MY_FLOAT) 1.0;
    currentFreq = (MY_FLOAT) 0.0;
    currentReson = (MY_FLOAT) 0.0;
    targetGain = (MY_FLOAT) 1.0;
    targetFreq = (MY_FLOAT) 0.0;
    targetReson = (MY_FLOAT) 0.0;
    deltaGain = (MY_FLOAT) 0.0;
    deltaFreq = (MY_FLOAT) 0.0;
    deltaReson = (MY_FLOAT) 0.0;
    sweepState = (MY_FLOAT)  0.0;
    sweepRate = (MY_FLOAT) 0.002;
    dirty = 0;
    this->clear();
}

FormSwep :: ~FormSwep()
{
    free(outputs);
}

void FormSwep :: clear()
{
    outputs[0] = (MY_FLOAT) 0.0;
    outputs[1] = (MY_FLOAT) 0.0;
}

void FormSwep :: setPoleCoeffs(MY_FLOAT *coeffs)
{
    dirty = 0;
    poleCoeffs[0] = coeffs[0];
    poleCoeffs[1] = coeffs[1];
}

void FormSwep :: setFreqAndReson(MY_FLOAT aFreq, MY_FLOAT aReson)
{
    dirty = 0;
    reson = aReson;
    freq = aFreq;
    currentReson = aReson;
    currentFreq = aFreq;
    poleCoeffs[1] = - (reson * reson);
    poleCoeffs[0] = (MY_FLOAT) 2.0 * reson * (MY_FLOAT)  cos(TWO_PI * freq / SRATE);
}

void FormSwep :: setStates(MY_FLOAT aFreq, MY_FLOAT aReson, MY_FLOAT aGain)
{
    dirty = 0;
    freq = aFreq;
    reson = aReson;
    gain = aGain;
    targetFreq = aFreq;
    targetReson = aReson;
    targetGain = aGain;
    currentFreq = aFreq;
    currentReson = aReson;
    currentGain = aGain;
}

void FormSwep :: setTargets(MY_FLOAT aFreq, MY_FLOAT aReson, MY_FLOAT aGain)
{
    dirty = 1;
    targetFreq = aFreq;
    targetReson = aReson;
    targetGain = aGain;
    deltaFreq = aFreq - currentFreq;
    deltaReson = aReson - currentReson;
    deltaGain = aGain - currentGain;
    sweepState = (MY_FLOAT) 0.0;
}

void FormSwep :: setSweepRate(MY_FLOAT aRate)
{
    sweepRate = aRate;
}

void FormSwep :: setSweepTime(MY_FLOAT aTime)
{
    sweepRate = ONE_OVER_SRATE / aTime;
}

void FormSwep :: setGain(MY_FLOAT aValue)
{
    gain = aValue;
}

MY_FLOAT FormSwep :: tick(MY_FLOAT sample)   /*   Perform Filter Operation */
{                                     
    MY_FLOAT temp;                    

    if (dirty)  {
        sweepState += sweepRate;
        if (sweepState>= 1.0)   {
            sweepState = (MY_FLOAT) 1.0;
            dirty = 0;
            currentReson = targetReson;
            reson        = targetReson;
            currentFreq = targetFreq;
            freq        = targetFreq;
            currentGain = targetGain;
            gain        = targetGain;
        }
        else    {
            currentReson = reson + (deltaReson * sweepState);
            currentFreq = freq + (deltaFreq * sweepState);
            currentGain = gain + (deltaGain * sweepState);
        }
        poleCoeffs[1] = - (currentReson * currentReson);
        poleCoeffs[0] = (MY_FLOAT) 2.0 * currentReson * 
					(MY_FLOAT)  cos(TWO_PI * currentFreq / SRATE);
    }

    temp = currentGain * sample;
    temp += poleCoeffs[0] * outputs[0];
    temp += poleCoeffs[1] * outputs[1];
    outputs[1] = outputs[0];
    outputs[0] = temp;
    lastOutput = outputs[0];
    return lastOutput;
}

/************   Test Main Program   *****************/
/*

void main()
{
    FormSwep filter;
    FILE *fd;
    MY_FLOAT temp;
    short data;
    long i;
    
    fd = fopen("test.raw","wb");
    
    filter.setTargets(100.0,0.99,0.01);
    for (i=0;i<20000;i++) {
        if (i%100 != 0) temp = 0.0; else temp = 1.0;
        data = filter.tick(temp) * 32000.0;
        fwrite(&data,2,1,fd);
    }
    
    filter.setTargets(1000.0,0.99,0.01);
    for (i=0;i<20000;i++) {
        if (i%100 != 0) temp = 0.0; else temp = 1.0;
        data = filter.tick(temp) * 32000.0;
        fwrite(&data,2,1,fd);
    }

    filter.setTargets(500.0,0.9999,0.001);
    for (i=0;i<20000;i++) {
        if (i%100 != 0) temp = 0.0; else temp = 1.0;
        data = filter.tick(temp) * 32000.0;
        fwrite(&data,2,1,fd);
    }

    fclose(fd);
}
*/

