/*******************************************/
/*  Master Class for Drum Synthesizer      */
/*  by Perry R. Cook, 1995-96              */
/*                                         */
/*  This instrument contains a bunch of    */
/*  RawWvIn objects, run through a bunch   */
/*  of one-pole filters.  All the          */
/*  corresponding rawwave files have been  */
/*  sampled at 22050 Hz.  Thus, if the     */
/*  compile-time SRATE = 22050, then       */
/*  no interpolation is used.  Otherwise,  */
/*  the rawwave data is appropriately      */
/*  interpolated for the current SRATE.    */
/*  You can specify the maximum Polyphony  */
/*  (maximum number of simultaneous voices)*/
/*  in a #define in the .h file.           */
/*                                         */
/*  Modified for RawWvIn class             */
/*  by Gary P. Scavone (4/99)              */
/*******************************************/

#if !defined(__Tabla_h)
#define __Tabla_h

#include "../STK/Instrmnt.h"
#include "../STK/RawWvIn.h"
#include "../STK/OnePole.h"

#define TABLA_NUMWAVES 15
#define TABLA_POLYPHONY 4

class Tabla : public Instrmnt
{
protected:  
  RawWvIn  *waves[TABLA_POLYPHONY];
  OnePole  *filters[TABLA_POLYPHONY];
  int      sounding[TABLA_POLYPHONY];
  int      numSounding;
public:
  Tabla();
  ~Tabla();
  virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
  virtual MY_FLOAT tick();
};

#endif
