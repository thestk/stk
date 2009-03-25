/*******************************************/
/*  One Zero Filter Class,                 */
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
/*  gain / (1.0 - fabs(alpha)).            */
/*******************************************/

#if !defined(__OneZero_h)
#define __OneZero_h

#include "Filter.h"

class OneZero : public Filter
{
  protected:  
    MY_FLOAT zeroCoeff;
    MY_FLOAT sgain;
  public:
    OneZero();
    ~OneZero();
    void clear();
    void setGain(MY_FLOAT aValue);
    void setCoeff(MY_FLOAT aValue);
    MY_FLOAT tick(MY_FLOAT sample);
};

#endif
