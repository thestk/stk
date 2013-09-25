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

#include "Object.h"
#include "WvOut.h"

#if !defined(__MatWvOut_h)
#define __MatWvOut_h

#define MAT_BUFFER_SIZE 1024

class MatWvOut : public WvOut
{
 protected:  
  FILE *fd;
  double data[MAT_BUFFER_SIZE]; /* not MY_FLOAT because MAT-file uses doubles */
  INT32 counter;
  INT32 totalCount;
  int channels;
 public:
  MatWvOut(char *infileName);
  MatWvOut(int chans, char *infileName);
  ~MatWvOut();
  INT32 getCounter();
  MY_FLOAT getTime();
  void tick(MY_FLOAT sample);
  void mtick(MY_MULTI samples);
};

#endif // defined(__MatWvOut_h)
