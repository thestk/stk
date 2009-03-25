/*******************************************/
/*  Raw Wave File Output Class,            */
/*  by Perry R. Cook, 1995-96              */ 
/*  For now, This Object can open a raw    */
/*  16bit data (signed integers) file, and */
/*  poke buffers of samples into it.       */
/*******************************************/

/*******************************************/
/*  SGI Real-Time Wave File Output Class,  */
/*  by Perry R. Cook, 1995-96              */ 
/*  This Object can opens the SGI soundout */
/*  device, and pokes buffers of samples   */
/*  into it.  The real code that does the  */
/*  is originally from Doug Scott of SGI.  */
/*******************************************/


#include "Object.h"

#if !defined(__RawWvOut_h)
#define __RawWvOut_h

#if !defined(__SGI_REALTIME)

#define BUFFER_SIZE 1

class RawWvOut : public Object
{
  protected:  
    FILE *fd;
    short data[BUFFER_SIZE];
    long counter;
    long totalCount;
  public:
    RawWvOut(char *fileName);
    ~RawWvOut();
    long getCounter();
    void tick(MY_FLOAT sample);
};

#else

#define BUFFER_SIZE 256

class RawWvOut : public Object
{
  protected:  
    FILE *fd;
    short data[BUFFER_SIZE];
    long counter;
  public:
    RawWvOut(char *fileName);
    ~RawWvOut();
    void tick(MY_FLOAT sample);
};

#endif

#endif
