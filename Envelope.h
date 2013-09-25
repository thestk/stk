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

#if !defined(__Envelope_h)
#define __Envelope_h

#include "Object.h"

class Envelope : public Object
{
  protected:  
    MY_FLOAT value;
    MY_FLOAT target;
    MY_FLOAT rate;
    int state;
  public:
    Envelope();
    virtual ~Envelope();
    void keyOn();
    void keyOff();
    void setRate(MY_FLOAT aRate);
    void setTime(MY_FLOAT aTime);
    void setTarget(MY_FLOAT aTarget);
    void setValue(MY_FLOAT aValue);
    MY_FLOAT tick();
    int informTick();
    MY_FLOAT lastOut();
};

#endif
