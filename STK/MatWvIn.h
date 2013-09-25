/*******************************************/
/*  MatWvIn Input Class,                   */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  This object inherits from WvIn and is  */
/*  used to open Matlab MAT-file data      */
/*  (doubles) files for playback.  In      */
/*  order for this class to work, the      */
/*  MAT-file must contain a single array   */
/*  (matrix) of double-precision floating  */
/*  point values (can be multi-channel).   */
/*  It does not work for any other data    */
/*  formats.                               */
/*                                         */
/*  MAT-file data is either big- or        */
/*  little-endian, which can be determined */
/*  from the header.                       */
/*******************************************/

#if !defined(__MatWvIn_h)
#define __MatWvIn_h

#include "Object.h"
#include "WvIn.h"

class MatWvIn : public WvIn
{
  public:
    MatWvIn(char *fileName, char *mode);
    ~MatWvIn();
};

#endif
