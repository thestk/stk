/**********************************************/
/* Jet Table Object by Perry R. Cook, 1995-96 */
/* Consult Fletcher and Rossing, Karjalainen, */
/*       Cook, more, for information.         */
/* This, as with many other of my "tables",   */
/* is not a table, but is computed by poly-   */
/* nomial calculation.                        */
/**********************************************/

#if !defined(__JetTabl_h)
#define __JetTabl_h

#include "Object.h"

class JetTabl : public Object
{
  protected:  
    MY_FLOAT lastOutput;
  public:
    JetTabl();
    ~JetTabl();
    MY_FLOAT lookup(MY_FLOAT deltaP);
    MY_FLOAT lastOut();
};

#endif
