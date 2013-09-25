/*******************************************/
/*  Matlab MAT File Output Class,          */
/*  by Gary P. Scavone, 1998.              */
/*  This object creates a Matlab MAT-file  */
/*  structure and fills it with buffers of */
/*  samples (doubles).                     */
/*                                         */
/*  The Matlab MAT-file format is not      */
/*  available to the general public.  I    */
/*  spent several days reverse-engineering */
/*  the file format to create this class.  */
/*  I couldn't figure out what a few of    */
/*  the header fields correspond to, but   */
/*  for the purposes of STK, this          */
/*  shouldn't create any problems.         */
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
  double data[MAT_BUFFER_SIZE]; /* not MY_FLOAT because MAT uses doubles */
  long counter;
  long totalCount;
  int chans;
  MY_FLOAT pan;
 public:
  MatWvOut(char *infileName);
  MatWvOut(int channels, char *infileName);
  ~MatWvOut();
  long getCounter();
  MY_FLOAT getTime();
  void setMonoPan(MY_FLOAT aPan);
  void tick(MY_FLOAT sample);
  void tick(MY_FLOAT lsamp, MY_FLOAT rsamp);
};

#endif // defined(__MatWvOut_h)
