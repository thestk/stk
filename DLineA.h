/*******************************************/
/*                                         */
/*  AllPass Interpolating Delay Line       */
/*  Object by Perry R. Cook 1995-96        */
/*  This one uses a delay line of maximum  */
/*  length specified on creation, and      */ 
/*  interpolates fractional length using   */
/*  an all-pass filter.  This version is   */
/*  more efficient for computing static    */
/*  length delay lines (alpha and coeff    */
/*  are computed only when the length      */  
/*  is set, there probably is a more       */ 
/*  efficient computational form if alpha  */
/*  is changed often (each sample)).       */
/*                                         */
/*******************************************/


#if !defined(__DLineA_h)
#define __DLineA_h

#include "Filter.h"

class DLineA : public Filter
{
  protected:  
    long inPoint;
    long outPoint;
    long length;
    MY_FLOAT alpha;
    MY_FLOAT coeff;
    MY_FLOAT lastIn;
  public:
    DLineA(long max_length);
    ~DLineA();
    void clear();
    void setDelay(MY_FLOAT length);
    MY_FLOAT tick(MY_FLOAT sample);
};

#endif
