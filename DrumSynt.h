/*******************************************/
/*  Master Class for Drum Synthesizer      */
/*  by Perry R. Cook, 1995-96              */ 
/*  This instrument contains a bunch of    */
/*  RawWave objects, run through a bunch   */
/*  of one-pole filters.  All the          */
/*  corresponding rawwave files have been  */
/*  sampled at 22050 Hz.  Thus, if the     */
/*  compile-time SRATE = 22050, then       */
/*  RawShot objects are used.  Otherwise,  */
/*  RawInterp objects are used.  You can   */
/*  specify the maximum Polyphony (maximum */
/*  number of simultaneous voices) in a    */
/*  #define in the .h file.                */
/*                                         */
/*  Modified for RawWave abstract class    */
/*  by Gary P. Scavone (11/11/98)          */
/*******************************************/

#if !defined(__DrumSynt_h)
#define __DrumSynt_h

#include "Instrmnt.h"
#include "RawWave.h"
#include "RawShot.h"
#include "RawInterp.h"
#include "OnePole.h"

#define DRUM_NUMWAVES 11
#define DRUM_POLYPHONY 4

class DrumSynt : public Instrmnt
{
  protected:  
    RawWave  *waves[DRUM_POLYPHONY];
    OnePole  *filters[DRUM_POLYPHONY];
    int      sounding[DRUM_POLYPHONY];
    int      numSounding;
  public:
    DrumSynt();
    ~DrumSynt();
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
    virtual MY_FLOAT tick();
};

#endif
