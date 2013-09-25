/*******************************************/
/*  Master Class for Drum Synthesizer      */
/*  by Perry R. Cook, 1995-96              */ 
/*  This instrument contains a bunch of    */
/*  RawWvIn objects (Non-Interpolating,    */
/*  1 shot players), run through a bunch   */
/*  of one-pole filters.  You can specify  */
/*  the maximum Polyphony (maximum number  */
/*  of simultaneous voices) in a #define   */
/*  in the .h file.  			   */
/*******************************************/

#if !defined(__DrumSynt_h)
#define __DrumSynt_h

#include "Instrmnt.h"
#include "RawWvIn.h"
#include "OnePole.h"

#define DRUM_NUMWAVES 11
#define DRUM_POLYPHONY 4

class DrumSynt : public Instrmnt
{
  protected:  
    RawWvIn  *waves[DRUM_POLYPHONY];
    OnePole  *filters[DRUM_POLYPHONY];
    int      sounding[DRUM_POLYPHONY];
    int      numSounding;
  public:
    DrumSynt();
/*    ~DrumSynt();                        */
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
    virtual MY_FLOAT tick();
};

#endif
