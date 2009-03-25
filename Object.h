/*******************************************/
/*  Object Class, by Perry R. Cook, 1995-96*/ 
/*  This is mostly here for compatibility  */
/*  with Objective C.  We'll also stick    */
/*  global defines here, so everyone will  */
/*  see them.                              */
/*******************************************/

#if !defined(__Object_h)
#define __Object_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

class Object
{
  public:
     int MIDI_control1;
     int MIDI_control2;
     int MIDI_control3;
     int MIDI_mod_wheel;
     int MIDI_after_touch;     
  protected:
    Object();
    ~Object();
};

/*   Only use one of __SGI_  __NeXT_  __DOS_  */
/*   And choice of __SGI_REALTIME or not      */

  #define __SGI_
  #define __SGI_REALTIME
/*   #define __NeXT_ 			*/

/* SRATE here is 44100, others are derived accordingly  */
/*   #define SRATE 44100.0
     #define SRATE_OVER_TWO 22050.0
     #define ONE_OVER_SRATE 0.00002267573696
     #define RATE_NORM 0.5
*/

/* SRATE here is 22050, others are derived accordingly   */
   #define SRATE 22050.0
     #define SRATE_OVER_TWO 11025.0
     #define ONE_OVER_SRATE 0.00004535147392
     #define RATE_NORM 1.0


/* SRATE here is 8k, others are derived accordingly   */
/*   #define SRATE 8000.0
   #define SRATE_OVER_TWO 4000
   #define ONE_OVER_SRATE 0.00012500000000
   #define RATE_NORM 2.75625
*/
/*   RATE_NORM is 22050 / 8000  */

/* Yer Basic Trigonometric constants  */
#define PI 3.14159265359
#define TWO_PI 6.28318530718
#define ONE_OVER_TWO_PI 0.15915494309

/* States for Envelopes, etc.         */

#define ATTACK 0
#define DECAY 1
#define SUSTAIN 2
#define RELEASE 3

/* Machine dependent stuff, possibly useful for optimization        */
/*    for example, changing double to float here increases          */
/*    performance (speed) by a whopping 4-6% on 486-flavor machines */
/*    BUT!! a change from float to double here increases speed by   */
/*    30% or so on SGI machines					    */
/*  #define MY_FLOAT      float  */
/*  #define MY_FLOAT_SIZE 4      */

#define MY_FLOAT      double
#define MY_FLOAT_SIZE 8

/* Debugging define, causes massive printf's to come out.           */
/* Also enables timing calculations in WaveOut class, other stuff.  */
/* #define _debug_ 1 */

#include <stdio.h>

#define NORM_7 0.0078125
/* this is 1/128 for MIDI normalization*/
	
#endif
