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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_DRUMMER_H
#define STK_DRUMMER_H

#include "Instrmnt.h"
#include "WvIn.h"
#include "OnePole.h"

const int DRUM_NUMWAVES = 11;
const int DRUM_POLYPHONY = 4;

class Drummer : public Instrmnt
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Drummer();

  //! Class destructor.
  ~Drummer();

  //! Start a note with the given drum type and amplitude.
  /*!
    Use general MIDI drum instrument numbers, converted to
    frequency values as if MIDI note numbers, to select a particular
    instrument.  An StkError will be thrown if the rawwave path is
    incorrectly set.
  */
  void noteOn(StkFloat instrument, StkFloat amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(StkFloat amplitude);

  //! Compute one output sample.
  StkFloat tick();

  //! Computer \e vectorSize outputs and return them in \e vector.
  StkFloat *tick(StkFloat *vector, unsigned int vectorSize);

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

 protected:  
  WvIn    *waves_[DRUM_POLYPHONY];
  OnePole *filters_[DRUM_POLYPHONY];
  int      sounding_[DRUM_POLYPHONY];
  int      nSounding_;
};

#endif
