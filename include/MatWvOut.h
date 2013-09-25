/*******************************************/
/*  Matlab MAT-file Output Class,          */
/*  by Gary P. Scavone, 1999.              */
/*  This object creates a Matlab MAT-file  */
/*  structure with a numeric array         */
/*  subelement and fills it with buffers   */
/*  of samples (doubles).                  */
/*                                         */
/*  When originally created, the Matlab    */
/*  MAT-file format was not public and I   */
/*  had to reverse-engineer the format.    */
/*  Matlab finally released the format in  */
/*  the Spring of 1999, and this class was */
/*  updated to more closely adhere to      */
/*  specifications.                        */
/*******************************************/

#if !defined(__MatWvOut_h)
#define __MatWvOut_h

#include "Object.h"
#include "WvOut.h"

class MatWvOut : public WvOut
{
 protected:  
  FILE *fd;
  double *matdata;  /* not MY_FLOAT because MAT-file uses doubles */
 public:
  MatWvOut(char *infileName, int chans = 1);
  ~MatWvOut();
  void tick(MY_FLOAT sample);
  void mtick(MY_MULTI samples);
};

#endif // defined(__MatWvOut_h)
