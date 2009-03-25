/*******************************************/
/*  Envelope Class, Perry R. Cook, 1995-96 */ 
/*  This is the base class for envelopes.  */
/*  This one is capable of ramping state   */
/*  from where it is to a target value by  */
/*  a rate.  It also responds to simple    */
/*  KeyOn and KeyOff messages, ramping to  */         
/*  1.0 on keyon and to 0.0 on keyoff.     */
/*  There are two tick (update value)      */
/*  methods, one returns the value, and    */
/*  other returns 0 if the envelope is at  */
/*  the target value (the state bit).      */
/*******************************************/

#include "Envelope.h"    

Envelope :: Envelope() : Object()
{    
    target = 0.0;
    value = 0.0;
    rate = 0.001;
    state = 0;
}

Envelope :: ~Envelope()
{    
}

void Envelope :: keyOn()
{
    target = 1.0;
    if (value != target) state = 1;
}

void Envelope :: keyOff()
{
    target = 0.0;
    if (value != target) state = 1;
}

void Envelope :: setRate(MY_FLOAT aRate)
{
    if (aRate < 0.0) {
        printf("negative rates not allowed!!, correcting\n");
        rate = -aRate;
    }
    else rate = aRate;
    rate = rate * RATE_NORM;	/*  SEE Object.h  */
}

void Envelope :: setTarget(MY_FLOAT aTarget)
{
    target = aTarget;
    if (value != target) state = 1;
}

void Envelope :: setValue(MY_FLOAT aValue)
{
    state = 0;
    target = aValue;
    value = aValue;
}

MY_FLOAT Envelope :: tick()
{
    if (state)  {
        if (target > value)    {
            value += rate;
            if (value >= target)    {
                value = target;
                state = 0;
            }
        }
        else    {
            value -= rate;
            if (value <= target)    {
                value = target;
                state = 0;
            }
        }
    }
    return value;
}

int Envelope :: informTick()
{
    this->tick();
    return state;
}

MY_FLOAT Envelope :: lastOut()
{
    return value;
}

/************  Test Main  ************************/
/*
void main()
{
    long i;
    Envelope test;
    
    test.setRate(0.15);
    test.keyOn();
    for (i=0;i<10;i++) printf("%lf\n",test.tick());
    test.setRate(0.1);
    test.setTarget(0.5);
    while (test.informTick()) printf("%lf\n",test.lastOut());
    test.setRate(0.05);
    test.keyOff();
    while(test.informTick()) printf("%lf\n",test.lastOut());
}
*/
