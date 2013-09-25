/**********************************************/
/*  One break point linear reed table object  */
/*  by Perry R. Cook, 1995-96                 */
/*  Consult McIntyre, Schumacher, & Woodhouse */
/*        Smith, Hirschman, Cook, Scavone,    */
/*        more for information.               */
/**********************************************/

#include "Object.h"

class ReedTabl : public Object
{
  protected:  
    MY_FLOAT offSet;
    MY_FLOAT slope;
    MY_FLOAT lastOutput;
  public:
    ReedTabl();
    ~ReedTabl();
    void setOffset(MY_FLOAT aValue);
    void setSlope(MY_FLOAT aValue);
    MY_FLOAT lookup(MY_FLOAT deltaP);
    MY_FLOAT lastOut();
};
