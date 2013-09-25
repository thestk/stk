/*********************************************/
/*  Object Class, by Perry R. Cook, 1995-96  */ 
/*  This is mostly here for compatibility    */
/*  with Objective C.  We'll also stick      */
/*  global defines here, so everyone will    */
/*  see them.                                */
/*********************************************/

#if !defined(__Object_h)
#define __Object_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

class Object
{
  public:
  protected:
    Object();
    virtual ~Object();
};

/* Uncomment your OS type below! */

/* #define __OS_NeXT_ */
#define __OS_IRIX_
/* #define __OS_Linux_ */
/* #define __OS_Win_ */

#if defined(__OS_NeXT_) /* For NeXTStep - Black or White Hardware */
  #define __NeXT_
#elif defined(__OS_IRIX_) /* For SGI */
  #define __SGI_REALTIME_
  typedef int bool;
#elif defined(__OS_Linux_) /* For Linux */
  #define __USS_REALTIME_
  #define __LITTLE_ENDIAN__
#elif defined(__OS_Win_) /* For Windows95 or NT */
  #define __WINDS_REALTIME_  /* For Direct Sound API */
/*  #define __WINMM_REALTIME_ */ /* For Win MM API */
  #define __SOCKET
  #define __LITTLE_ENDIAN__
#endif

/* Real-time output buffer size.  If clicks are occuring in the
 * output sound stream, a larger buffer size may help.  Larger
 * buffer sizes, however, produce more latency between input and
 * output.
*/
#define RT_BUFFER_SIZE 256

/* This sets the maximum number of simultaneous
 * (within a buffer) MIDI messages that can be
 * serviced before messages begin to be lost or
 * overwritten.  It should be a function of
 * RT_BUFFER_SIZE 
 */
#define MAX_IN_STRINGS 25

/* SRATE here is 44100, others are derived accordingly  */
/*
#define SRATE (MY_FLOAT) 44100.0
#define SRATE_OVER_TWO (MY_FLOAT) 22050.0
#define ONE_OVER_SRATE (MY_FLOAT) 0.00002267573696
#define RATE_NORM (MY_FLOAT) 0.5
#define TWO_PI_OVER_SRATE (MY_FLOAT) 0.0001424758573
*/
/* SRATE here is 22050, others are derived accordingly   */

#define SRATE (MY_FLOAT)  22050.0
#define SRATE_OVER_TWO (MY_FLOAT)  11025.0
#define ONE_OVER_SRATE (MY_FLOAT)  0.00004535147392
#define RATE_NORM (MY_FLOAT)  1.0
#define TWO_PI_OVER_SRATE (MY_FLOAT) 0.0002849517146

/* SRATE here is 16000, others are derived accordingly   */
/*
#define SRATE (MY_FLOAT)  16000.0
#define SRATE_OVER_TWO (MY_FLOAT)  8000.0
#define ONE_OVER_SRATE (MY_FLOAT)  0.0000625
#define RATE_NORM (MY_FLOAT)  1.375
#define TWO_PI_OVER_SRATE (MY_FLOAT) 0.000392699
*/
/* SRATE here is 8k, others are derived accordingly   */
/*
#define SRATE (MY_FLOAT) 8000.0
#define SRATE_OVER_TWO (MY_FLOAT) 4000
#define ONE_OVER_SRATE (MY_FLOAT) 0.00012500000000
#define RATE_NORM (MY_FLOAT) 2.75625
#define TWO_PI_OVER_SRATE (MY_FLOAT) 0.0002849517146
*/

/* Yer Basic Trigonometric constants  */
#if !defined(PI)
  #define PI (MY_FLOAT) 3.14159265359
#endif
#define TWO_PI (MY_FLOAT) 6.28318530718
#define ONE_OVER_TWO_PI (MY_FLOAT) 0.15915494309
#define SQRT_TWO 1.414213562

/* States for Envelopes, etc.         */

#define ATTACK 0
#define DECAY 1
#define SUSTAIN 2
#define RELEASE 3

/* Machine dependent stuff, possibly useful for optimization.
 * For example, changing double to float here increasesf
 * performance (speed) by a whopping 4-6% on 486-flavor machines.
 * BUT!! a change from float to double here increases speed by
 * 30% or so on SGI machines.
*/

#define MY_FLOAT      double
#define MY_FLOAT_SIZE 8

/*
#define MY_FLOAT      float
#define MY_FLOAT_SIZE 4
*/

/* Debugging define, causes massive printf's to come out.
 * Also enables timing calculations in WaveOut class, other stuff.
 */

/* #define _debug_ 1 */

/* MIDI definitions  */
#define NORM_7 (MY_FLOAT) 0.0078125 /* this is 1/128 */
		
#endif
