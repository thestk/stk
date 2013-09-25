/***************************************************/
/*! \class Drummer
    \brief STK drum sample player class.

    This class implements a drum sampling
    synthesizer using WvIn objects and one-pole
    filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately
    interpolated for other sample rates.  You can
    specify the maximum polyphony (maximum number
    of simultaneous voices) via a #define in the
    Drummer.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__DRUMMER_H)
#define __DRUMMER_H

#include "Instrmnt.h"
#include "WvIn.h"
#include "OnePole.h"

#define DRUM_NUMWAVES 11
#define DRUM_POLYPHONY 4

class Drummer : public Instrmnt
{
 public:
  //! Class constructor.
  Drummer();

  //! Class destructor.
  ~Drummer();

  //! Start a note with the given drum type and amplitude.
  /*!
    Use general MIDI drum instrument numbers, converted to
    frequency values as if MIDI note numbers, to select a
    particular instrument.
  */
  void noteOn(MY_FLOAT instrument, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

 protected:  
  WvIn    *waves[DRUM_POLYPHONY];
  OnePole *filters[DRUM_POLYPHONY];
  int      sounding[DRUM_POLYPHONY];
  int      nSounding;

};

#endif
