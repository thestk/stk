/*******************************************/
/*  Real-Time Output Class,                */
/*  by Perry R. Cook, 1995-96              */ 
/*  Separated into just realtime by        */
/*  Tim Stilson, 1996                      */
/*******************************************/

/*******************************************/
/*  SGI Real-Time Wave File Output Class,  */
/*  by Perry R. Cook, 1995-96              */ 
/*  This Object can opens the SGI soundout */
/*  device, and pokes buffers of samples   */
/*  into it.  The real code that does the  */
/*  is originally from Doug Scott of SGI.  */
/*******************************************/

/*******************************************/
/*  USS Real-Time Wave File Output Class,  */
/*  by Tim Stilson, 1996                   */
/*  based on code by Perry R. Cook, 1996   */
/*  This Object opens the USS sound output */
/*  device, and pokes buffers of samples   */
/*  into it.                               */
/*******************************************/


#include "Object.h"
#include "WvOut.h"
#include "RTSoundIO.h"

#if !defined(__RTWvOut_h)
#define __RTWvOut_h

class RTWvOut : public WvOut
{
 protected:
	RTSoundIO *soundIO;
    short data[RT_BUFFER_SIZE];
    long counter;
 public:
    RTWvOut();
    ~RTWvOut();
    void tick(MY_FLOAT sample);
};

#endif // defined(__RTWvOut_h)
