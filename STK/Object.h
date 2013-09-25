/*********************************************/
/*  Object Class, by Perry R. Cook, 1995-99  */
/*                                           */
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

/* The OS type definitions are made in the Makefile */

#if defined(__OS_NeXT_)    /* For NeXTStep - Black or White Hardware */
  #define RANDLIMIT 2147483647
#elif defined(__OS_IRIX_)  /* For SGI */
  #define __STK_REALTIME_
  #define RANDLIMIT 2147483647
#elif defined(__OS_Linux_) /* For Linux */
  #define __STK_REALTIME_
  #define __OSS_API_       /* Use OSS API */
  #define __LITTLE_ENDIAN__
  #define RANDLIMIT 2147483647
#elif defined(__OS_Win_)   /* For WindowsXX or NT */
  #define __STK_REALTIME_
  #define __WINDS_API_     /* For DirectSound API */
//  #define __WINMM_API_     /* For Win MM API */
  #define __LITTLE_ENDIAN__
  #define RANDLIMIT 32767
#endif

/* Real-time audio input and output buffer size.  The value of
 * this buffer should be an integer multiple of the number of
 * channels your application plans to support, in order that
 * multi-channel data is not split across multiple buffers. If
 * clicks are occuring in the input or output sound stream, a
 * larger buffer size may help.  Larger buffer sizes, however,
 * produce more latency between input and output.
 *
 * NOTE FOR WINDOZE USERS: Given inherent delays in the audio
 * input and output mechanism under Windoze, there is a trade-off
 * between smoothness of fast SKINI parameter updates and input/output
 * latency as discussed above.  You can use buffer sizes as low
 * as 100 (maybe lower) for delay critical applications, but in
 * this case SKINI parameter updates will be clumped together
 * and sound unnatural.  If you want smoother parameter updates
 * and you can live with more delay, try using buffer sizes
 * closer to 1000.  If you need smooth parameter updates AND
 * low delay, don't use Windoze!
*/
#define RT_BUFFER_SIZE 256

/* This sets the maximum number of simultaneous
 * (within a buffer) MIDI messages that can be
 * serviced before messages begin to be lost or
 * overwritten.  It should be a function of
 * RT_BUFFER_SIZE 
 */
#define MAX_IN_STRINGS 25

/* The following definition is concatenated to the
 * beginning of all references to rawwave files in
 * the various STK core classes (ex. Clarinet.cpp).
 * If you wish to move the rawwaves directory to a
 * different location in your file system, you will
 * need to set this path definition appropriately.
 * The current definition is a relative reference
 * that will work "out of the box" for the STK
 * distribution.
 */
#define RAWWAVE_PATH "../"

/* Sampling Rate */
#define SRATE (MY_FLOAT) 22050.0

/* Other SRATE derived defines  */
#define SRATE_OVER_TWO (MY_FLOAT) (SRATE / 2)
#define ONE_OVER_SRATE (MY_FLOAT) (1 / SRATE)
#define TWO_PI_OVER_SRATE (MY_FLOAT) (2 * PI / SRATE)

/* Yer Basic Trigonometric constants  */
#if !defined(PI)
  #define PI (MY_FLOAT) 3.14159265359
#endif
#define TWO_PI (MY_FLOAT) 6.28318530718
#define ONE_OVER_TWO_PI (MY_FLOAT) 0.15915494309
#define SQRT_TWO 1.414213562

/* Useful random number generator values */
#define ONE_OVER_RANDLIMIT (1.0/RANDLIMIT)
#define RANDLIMIT_OVER_TWO (int)(RANDLIMIT/2)

/* FPU Underflow Limit
 * The IEEE specification doesn't call for automatic
 * zeroing of floating-point values when they reach
 * their numerical limits.  Instead, most processors
 * switch to a much more computation-intensive mode
 * when a FPU underflow occurs.  We set a lower limit
 * here for our own (not so efficient) checks.  Here's
 * a useful macro for limiting MY_FLOATs.  At this time,
 * no FPU underflow checks are being performed.
 */
#define FPU_UFLOW_LIMIT 0.0000000001
#define LIMIT_MY_FLOAT(j) ((((j)<(MY_FLOAT)FPU_UFLOW_LIMIT)&&((j)>(MY_FLOAT)-FPU_UFLOW_LIMIT))?(MY_FLOAT)0.0:(j))

/* States for Envelopes, etc. */
#define ATTACK 0
#define DECAY 1
#define SUSTAIN 2
#define RELEASE 3

/* Machine dependent stuff, possibly useful for optimization.
 * For example, changing double to float here increases
 * performance (speed) by a whopping 4-6% on 486-flavor machines.
 * BUT!! a change from float to double here increases speed by
 * 30% or so on SGI machines.
*/
#define MY_FLOAT      double
//#define MY_FLOAT      float

/* MY_MULTI is just a pointer to MY_FLOAT.  This type is used
 * to pass multichannel data back and forth within STK.
*/
typedef MY_FLOAT *MY_MULTI;

/* INT16 is just that ... a 16-bit signed integer. */
typedef signed short INT16;

/* INT32 is just that ... a 32-bit signed integer. */
typedef signed long INT32;

/* Debugging define, causes massive printf's to come out.
 * Also enables timing calculations in WaveOut class, other stuff.
 * Uncomment to enable.
 */
//#define _debug_ 1

/* MIDI definitions  */
#define NORM_7 (MY_FLOAT) 0.0078125 /* this is 1/128 */
		
#endif
