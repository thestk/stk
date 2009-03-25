/*******************************************/
/*  Object Class, by Perry R. Cook, 1995-96*/ 
/*  This is mostly here for compatibility  */
/*  with Objective C.  We'll also stick    */
/*  global defines here, so everyone will  */
/*  see them.                              */
/*******************************************/

#include "Object.h"


/*  This is just here for compatibility and convenience,
        so there's no need to do any real calculations.
    I do set up some redefinable variables here.     */


Object :: Object()
{
    MIDI_mod_wheel = 1;		/* Controller # 1 	*/
    MIDI_control1 = 2;		/* Breath Pressure 	*/
    MIDI_control2 = 4;		/* Foot Control    	*/
    MIDI_control3 = 11;		/* Expression Pedal 	*/
    MIDI_after_touch = 128;	/* Channel Pressure -> new controller */
}

Object :: ~Object()
{
}

