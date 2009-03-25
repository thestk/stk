/**********************************************/
/* Jet Table Object by Perry R. Cook, 1995-96 */
/* Consult Fletcher and Rossing, Karjalainen, */
/*       Cook, more, for information.         */
/* This, as with many other of my "tables",   */
/* is not a table, but is computed by poly-   */
/* nomial calculation.                        */
/**********************************************/

#include "Object.h"

class JetTabl : public Object
{
  protected:  
    double lastOutput;
  public:
    JetTabl();
    ~JetTabl();
    double lookup(double deltaP);
    double lastOut();
};

