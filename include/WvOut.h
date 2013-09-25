/********************************************/
/*  WvOut Abstract Class,                   */
/*  by Tim Stilson, 1996                    */
/*  revised by Gary P. Scavone, 1999-2000   */
/*                                          */
/*  This class can handle multi-channel     */
/*  data via the mtick() method.            */
/*                                          */
/*  Currently, WvOut and its subclasses are */
/*  non-interpolating.  Thus, the output    */
/*  rate is always SRATE (defined in        */
/*  Object.h).  A future upgrade could add  */
/*  interpolation functionality to allow    */
/*  output rates different than the STK     */
/*  internal processing rate (SRATE).       */
/********************************************/

#if !defined(__WvOut_h)
#define __WvOut_h

#include "Object.h"
#include "StkError.h"

#define FILE_BUFFER_SIZE 1024

class WvOut : public Object
{
 protected:
  INT16 *data;
  long data_length;
  long counter;
  long totalCount;
  int channels;
 public:
  WvOut();
  virtual ~WvOut();
  long getCounter();
  MY_FLOAT getTime();
  virtual void tick(MY_FLOAT sample) = 0;
  virtual void mtick(MY_MULTI samples) = 0;
};

#endif // defined(__WvOut_h)
